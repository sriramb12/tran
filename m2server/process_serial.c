/****************************************************************************
 *
 * Function:	process_serial
 *
 * Description:	Functions to assign serial numbers for mos6,
 *		mos16, mos17, and mos21.  Serial
 *		numbers for Com1 are no longer needed.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include "close_down.h"
#include "contract_num.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "lock_file.h"
#include "mars_defines.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "pre_suffix.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "reg_globals.h"
#include <fcntl.h>
#include <stdio.h>
#include "m2req_types.h"
#include "m2server_errs.h"
#include "m2state_file.h"
#include "mti.h"
#include "rtn.h"
#include "server_lib.h"
#include "m2msg.h"
#include "m2server_prototypes.h"
#include "strcpy.h"
#include "user_params.h"


void contract_num_retrieve(void)
{
    int index;
     FILE *fp;
    char *files[3];
    int len;
    char company[48];
    register short total_cnt;
    register short cnt;
    int new_serial_num;
    int serial_num[5];
    int upper_range[5];
    char serial_string[5][6];
    char new_serial_string[6];
    char serial_char[12];

    /* Build file name where contract number is */
    /* mask_msg.par1 holds the name of the Mask shop  table */
    strcpy3(bf.file_name, CONTRACT_TABLE_PATH, mask_msg.par1, "_num");
    /* Lock the file */
    index = lock_file_set(bf.file_name);
    /* Now open the file with a stream pointer */
    if ((fp = fopen(bf.file_name, "r+")) == NULL)
    shut_down(contract_tbl_err, CONTRACT_TBL_ERR);
    /* Read in all of the serial numbers */
    total_cnt = 0;
    new_serial_num = 0;
    while ((fscanf(fp, "%s %d %d",
    serial_string[total_cnt], &serial_num[total_cnt], &upper_range[total_cnt])) != EOF)
    {
	/* If haven't found a serial number yet, */
	/* see if this line has an available one */
	if (new_serial_num == 0)
	{
	    if (serial_num[total_cnt] <= upper_range[total_cnt])
	    {
		new_serial_num = serial_num[total_cnt];
		strcpy(new_serial_string, serial_string[total_cnt]);
		++serial_num[total_cnt];
	    } /* End if for serial number available */
	} /* End if for new serial number not found */
	/* Increment the number of lines found */
	++total_cnt;
    } /* End while */
    /* If there are no available serial numbers, tell the user */
    if (new_serial_num == 0)
    {
	strcpy3(rtn.msg, contract_range_err, DOUBLE_RET, help_msg);
    {
	lock_file_clear(index);
	shut_down(rtn.msg, CONTRACT_RANGE_ERR);
    } /* End */
    }
    /* Tell client serial number will be sent */
    put_short(SUCCESS);
    /* Send the number to the user */
    len = strlen(new_serial_string);
    if (len == 3)
    sprintf(serial_char, "%s%05d", new_serial_string, new_serial_num);
    else if (len == 4)
    sprintf(serial_char, "%s%04d", new_serial_string, new_serial_num);
    else if (len == 5)
    sprintf(serial_char, "%s%03d", new_serial_string, new_serial_num);
    put_string(serial_char);
	/* Point to the beginning of the file */
    if (fseek(fp, 0L, 0) != SUCCESS)
	{
	    lock_file_clear(index);
	    close_down1q(seek_err, bf.file_name, SEEK_ERR);
	}
    /* Write lines back out to the file */
    for (cnt = 0; cnt <total_cnt; ++cnt)
    {
	len = strlen(serial_string[cnt]);
	if (len == 3)
	fprintf(fp, "%s %05d %05d\n",
	serial_string[cnt], serial_num[cnt], upper_range[cnt]);
	else if (len == 4)
	fprintf(fp, "%s %04d %04d\n",
	serial_string[cnt], serial_num[cnt], upper_range[cnt]);
	else if (len == 5)
	fprintf(fp, "%s %03d %03d\n",
	serial_string[cnt], serial_num[cnt], upper_range[cnt]);
    }
    /* Close the file with stream pointer */
    fclose(fp);
    /* Unlock the file */
    lock_file_clear(index);
    /* Set the prefix */
    strcpy6(mti.prefix, user.email, " issued serial number ",
    serial_char, " for mask set '", mask_msg.key1, "'");
    /* Send the user the return code */
    /* Also set the key so the message can be logged */
    strcpy1(key, mask_msg.key1);
    mti.suffix_ptr = ".";
    /* Transaction gets logged in put_ret_code */
    /* Therefore delta path must be initialized */
    strcpy3(mti.delta_dir, MASK_DELTA, SLASH, mask_msg.key1);
    /* If mask set present, log transaction in mask daily log */
    if (access(mti.delta_dir, 0) == 0)
    put_ret_code(DBYES);
    else put_ret_code(DBNO);
    /* Make an entry in the serial log file */
    /* Put Mask Shop name in different format */
    if (strcmp(mask_msg.par1, "dupont") == 0)
    strcpy(company, "DuPont");
    else if (strcmp(mask_msg.par1, "photronics") == 0)
    strcpy(company, "Photronics");
    else if (strcmp(mask_msg.par1, "rtc") == 0)
    strcpy(company, "rtc");
    else strcpy(company, mask_msg.par1);
    /* Make entry in serial log */
    contract_log(mask_msg.par1, company, serial_char);
    /* Shadow the new contract number to the remote servers */
    len = strlen(mask_msg.par1);
    files[0] = malloc(len+5);
    strcpy2(files[0], mask_msg.par1, "_num");
    files[1] = malloc(len +15);
     strcpy3(files[1], mask_msg.par1, "_", tr.log_file);
    files[2] = NULL;
    files_shadow(CONTRACT_TABLE_DIR, files);
    log3((char *)"Closing ", (char *)"transaction ", tr.pid_string);
    exit(0);
} /* End contract_num_retrieve */

void contract_log(char *log_file, char *company, char *serial_char)
{
    int len;
     char user_phone[20];
    char contract_log_file[48];

    /* Get the user's name and phone */
     ldapfsl_get_user_name_phone(user.email, user.name, user_phone);
	    /* Now pad all the fields to a specified length */
    str_pad(mask_msg.key1, 5);
    str_pad(company, 11);
    str_pad(serial_char, 9);
    str_pad(user.email, 8);
    str_pad(user.name, 16);
    str_pad(user_phone, 13);
     /* Concatinate fields together into one string */
    len = strcpy8(rtn.msg, mask_msg.key1, " ", log_file, " ",
    serial_char, " ", user.email, " ");
    log_date[16] = 0;
    strcpy5(rtn.msg+len, user.name, " ", user_phone+1, " ", log_date+4);
    /* Build the log file name */
    strcpy4(contract_log_file, CONTRACT_TABLE_PATH, log_file, "_", tr.log_file);
    data_log(contract_log_file, rtn.msg);
} /* end contract_log */

void serial_log(char *log_file, int serial_num, int num)
{
    char serial_char[6];
    int cnt;
    int len;
     char user_phone[20];

    serial_num -= num;
    /* Get the user's name and phone */
     ldapfsl_get_user_name_phone(user.email, user.name, user_phone);
	    /* Now pad all the fields to a specified length */
    str_pad(mask_msg.key1, 5);
    str_pad(user.email, 8);
    str_pad(user.name, 16);
    str_pad(user_phone, 13);
    log_date[16] = 0;
     for (cnt = 0; cnt <num; ++cnt, ++serial_num)
    {
	sprintf(serial_char, "%.5d", serial_num);
	str_pad(serial_char, 9);
	 /* Concatinate fields together into one string */
	len = strcpy6(rtn.msg, mask_msg.key1, " ",
	serial_char, " ", user.email, " ");
	strcpy5(rtn.msg+len, user.name, " ", user_phone+1, " ", log_date+4);
	data_log(log_file, rtn.msg);
     } /* end for */
} /* end serial_log */

void rtv_serial(char *serial_prefix)
{
    int index;
    int fd;
    char *files[3];
    int serial_num;
    int len;
    char serial_char[8];
     char log_file[48];

    /* Build complete name for serial file */
    strcpy3(bf.file_name, SERIAL_PATH, serial_prefix, "num");
    /* Lock the file */
    index = lock_file_set(bf.file_name);
    /* Open the file where serial number is kept */
    if ((fd = open(bf.file_name, O_RDWR)) == EOF)
    /* Tell user could not open the file */
    close_down1m(rd_open_err, bf.file_name, RD_OPEN_ERR);
    /* Read in the serial number */
    if ((read(fd, serial_char, SERIAL_SIZE)) != SERIAL_SIZE)
     {
	lock_file_clear(index);
	close_down1m(read_err, bf.file_name, READ_ERR);
    }
    /*Add the trailing 0 */
	serial_char[SERIAL_SIZE] = '\0';
    /* Convert to integer representation */
    serial_num = atoi(serial_char);
    /* Calculate the offset */
    serial_num += mask_msg.num;
    /* Make sure the serial number has a valid range */
    if ((serial_num < LOWER_SERIAL) || (serial_num > UPPER_SERIAL))
    {
	lock_file_clear(index);
	close_down1d(bad_serial, serial_num, BAD_SERIAL);
     }
    /* Send indication transaction successful so far */
    put_short(SUCCESS);
    /* Send the number to the user */
    put_binary_string(serial_char, SERIAL_SIZE);
	/* Point to the beginning of the file */
	if (lseek(fd, 0, SEEK_SET) != SUCCESS)
	{
	    lock_file_clear(index);
	    close_down1q(seek_err, bf.file_name, SEEK_ERR);
	}
	/* Set the prefix before beginning value lost */
	if (mask_msg.num == 1)
	strcpy4(mti.prefix, "Mask set '", mask_msg.key1, "' assigned serial number ", serial_char);
	else sprintf(mti.prefix, "Mask set '%s' assigned %d serial numbers beginning at %s",
	mask_msg.key1, mask_msg.num, serial_char);
    /* Convert serial number back to string format */
    sprintf(serial_char, "%.5d\n", serial_num);
    /* Write out the new number */
	if ( write(fd, serial_char, SERIAL_SIZE+1) != SERIAL_SIZE+1)
	{
	    lock_file_clear(index);
	    close_down1m(write_err, bf.file_name, WRITE_ERR);
	}
    /* Unlock the file so other processes may access it */
    close(fd);
    /* Unlock the file */
    lock_file_clear(index);
    /* Send the user the return code */
    /* Set the key so the message can be logged */
    strcpy1(key, mask_msg.key1);
    mti.suffix_ptr = ".";
    /* Set the delta directory so the transaction can be logged */
    strcpy3(mti.delta_dir, MASK_DELTA, SLASH, mask_msg.key1);
    if (access(mti.delta_dir, 0) == 0)
    put_ret_code(DBYES);
    else put_ret_code(DBNO);
    /* Make an entry in the serial log file */
    log_date[28] = '\0';
    strcpy5(mti.prefix+strlen(mti.prefix), "\nby ", user.login, " ", log_date, ".");
     strcpy3(log_file, SERIAL_PATH, serial_prefix, tr.log_file);
     serial_log(log_file, serial_num, mask_msg.num);
    /* Send copy of serial file to remote servers */
    len = strlen(serial_prefix);
    files[0] = malloc(len+4);
    strcpy2(files[0], serial_prefix, "num");
    files[1] = malloc(len+13);
     strcpy2(files[1], serial_prefix, tr.log_file);
    files[2] = NULL;
    files_shadow(SERIAL_DIR, files);
    log3((char *)"Closing ", (char *)"transaction ", tr.pid_string);
    exit(0);
} /* End rtv_serial */

/* Retrieve a mask set name */
void mask_name_rtv(void)
{
    void html_filter();
    int fd;
    char fab[32];
    char tech[32];
    char mbg[32];
    char orig_mask[64];
    char description[BUFFER_SIZE];
    char remarks[BUFFER_SIZE];
    char from_email[64];
    char to_email[64];
    int test;
    int index;
    char user_id[10];
    char name[32];
    char dept[10];
    char *files[3];
    char recipients[64];
    char new_mask[8];
    char current_mask[8];
    int num;

    /* Unpack user ID and original mask from key1 */
    sscanf(mask_msg.key1, "%[^,]%*c%[^,]%*c%s", from_email, to_email, orig_mask);
    /* Make sure valid user ID or commerce ID entered for requester */
    requester_whobu(from_email, user_id, name, dept);
    /* Check to see if this is a test */
    /* If this is Mike Boatright, this is a test */
    if ((strcasecmp(to_email, "-rtest") == 0)
    || (strcasecmp(from_email, "rxab40") == 0)
    || (strcasecmp(from_email, "rrnj60") == 0)
    || (strcasecmp(from_email, "rxfh80") == 0))
    test = DBYES;
    else test = DBNO;
    /* Take underscores out of name */
    strexc(name, '_', ' ');
    /* Lock the file */
    index = lock_file_set(MASKSET_TABLE);
    /* Open the file where the mask set name is kept */
    if ((fd = open(MASKSET_TABLE, O_RDWR)) == EOF)
    /* Inform user data does not exist */
    close_down1m(rd_open_err, MASKSET_TABLE, RD_OPEN_ERR);
    /* Read in the data from the file */
    if (read(fd, current_mask, 4) < 4)
    /* If could not pull in a mask set name, give error message */
    close_down1m(read_err, MASKSET_TABLE, READ_ERR);
    current_mask[4] = '\0';
    /*Build the mask set name */
    build_new_mask(new_mask, current_mask);
    /* Get scription from user */
    put_short(SUCCESS);
    get_string(description);
    get_string(remarks);
    /* Point to the beginning of the file */
    if (lseek(fd, 0, SEEK_SET) != SUCCESS)
    close_down1q(seek_err, MASKSET_TABLE, SEEK_ERR);
    /* Write out last mask set to be assigned */
    new_mask[4] = '\n';
    /* If this is a test, do not write out new mask */
    /* The mask set written to the file is the mask set just issued */
    if (test == DBNO)
    {
	if ( write(fd, new_mask, 5) != 5)
	close_down1m(write_err, MASKSET_TABLE, WRITE_ERR);
    } /* end if */
    /* Close the file */
    close(fd);
    /* Unlock the file so other processes may access it */
    lock_file_clear(index);
    /* Get rid of carriage return added */
    new_mask[4] = '\0';
    /* Build string to send */
    rtn.len = strcpy3(rtn.msg,
"You have been issued the following mask set name:\n\n    '",
    new_mask, "'\n\n");
    /* Pull the fab, tech and MBG out of key2 */
    sscanf(mask_msg.key2, "%[^,]%*c%[^,]%*c%s", fab, tech, mbg);
    rtn.len += strcpy8(rtn.msg+rtn.len, " \nFab:  ", fab,
    " \nProcess technology:  ", tech,
    "\nOriginal mask set:  ", orig_mask,
    " \nProjected release date to mask preparation:  ", mask_msg.par4);
    /* Scan info out of key3 */
    rtn.len += strcpy2(rtn.msg+rtn.len,  " \nDescription:  ", description);
    rtn.len += strcpy8(rtn.msg+rtn.len, " \nProject leader name:  ", mask_msg.key3,
    " \nProject leader user ID:  ", user.pin,
    " \nProject name:  ", mask_msg.par2,
    " \nProject Dante number:  ", mask_msg.par1);
        rtn.len += strcpy7(rtn.msg+rtn.len, " \nPlatform/chip set:  ", mask_msg.par3,
    " \nExternal control number (ECCN):  ", mask_msg.par5,
    " \nAdditional comments/remarks:  ", remarks, " \n");
    /* Get rid of HTMl notation */
    html_filter(rtn.msg);
    /* Log mask set names in log file */
    /* Do not log if this is a test */
    if (test == DBNO)
    data_write2log("contract_tables/mask_", new_mask, name,
    from_email, orig_mask, description, remarks);
    if (strcmp(to_email, NONE) != 0)
    {
	/* Verify receiver's email address */
	if (receiver_whobu(to_email, to_email) != SUCCESS)
	strcpy(to_email, NONE);
    } /* End if */
    /* Send mask set names to the client */
    return_success(rtn.msg, DBNO);
    /* Build addresses to send email to */
    num = strcpy2(recipients, from_email, "@freescale.com");
    if (strcmp(to_email, NONE) != 0)
    strcpy3(recipients+num, " ", to_email, "@freescale.com");
    /* Send the message */
    email_send("", recipients, "Mask Set Retrieval", rtn.msg);
    /* Write the mask set to the MARS database */
    if (test == DBNO)
    mars_insert(new_mask, name,      from_email, orig_mask,
fab, tech, mbg, mask_msg.key3, user.pin,
mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4, mask_msg.par5, description, remarks);
    /* Keep track of numbers assigned */
    /* Send a copy of the file to the remote server */
    /* The file name must be in the files array */
    files[0] = malloc(MASKSET_ROOT_LEN);
    strcpy(files[0], MASKSET_ROOT);
    files[1] = malloc(MASKSET_PREFIX_LEN +13);
    strcpy2(files[1], MASKSET_PREFIX, tr.log_file);
    files[2] = NULL;
    files_shadow(CONTRACT_DIR, files);
    log3((char *)"Closing ", (char *)"transaction ", tr.pid_string);
    exit(0);
} /* End mask_name_rtv */

void pc_name_rtv(void)
{
    char description[BUFFER_SIZE];
    char remarks[BUFFER_SIZE];
    char dept[10];
    char from_email[32];
    char to_email[32];
    char orig_mask[32];
    int index;
    char user_id[10];
    char name[32];
    char *files[3];
    char recipients[64];
    char current_pc[16];
    char tmp_pc[8];
    char new_pc[16];
    int int_pc;
    int num;
    int fd;

    /* Unpack user ID and MBG from key1 */
    sscanf(mask_msg.key1, "%[^,]%*c%[^,]%*c%s", from_email, to_email, orig_mask);
    /* Make sure valid user ID or commerce ID entered for requester */
    requester_whobu(from_email, user_id, name, dept);
    index = lock_file_set(PC_TABLE);
    /* Open the file where the PC number is kept */
    if ((fd = open(PC_TABLE, O_RDWR)) == EOF)
    /* Inform user data does not exist */
    close_down1m(rd_open_err, PC_TABLE, RD_OPEN_ERR);
    /* Read in the data from the file */
    if (read(fd, current_pc, 11) < 11)
    /* If could not pull in a PC name, give error message */
    close_down1m(read_err, PC_TABLE, READ_ERR);
    current_pc[11] = '\0';
    strncpy(tmp_pc, current_pc+2, 4);
    tmp_pc[4] = '\0';
    /* Convert to integer representation */
    int_pc = atoi(tmp_pc);
    /* build the new PC name */
	sprintf(new_pc, "PC%d/%d\n", int_pc+1, int_pc+5001);
	if (int_pc > 40001)
	shut_down(pc_name_err, PC_NAME_ERR);
    /* Get the PC description */
    put_short(SUCCESS);
    get_string(description);
    /* Gets the remarks */
    get_string(remarks);
    /* Point to the beginning of the file */
    if (lseek(fd, 0, SEEK_SET) != SUCCESS)
    close_down1q(seek_err, PC_TABLE, SEEK_ERR);
    /* Write out next PC to assign */
	if ( write(fd, new_pc, 12) != 12)
	close_down1m(write_err, PC_TABLE, WRITE_ERR);
    /* Close the file */
    close(fd);
    /* Unlock the file */
    lock_file_clear(index);
    /* Build string to send */
    strcpy4(rtn.msg, "You have been issued the following PC name:\n\n",
    "    ", current_pc, "\n");
    /* See if valid receiver email present */
    if (strcmp(to_email, NONE) != 0)
    {
	/* Verify receiver's email address */
	if (receiver_whobu(to_email, to_email) != SUCCESS)
	strcpy(to_email, NONE);
    } /* End if */
    /* Send pc names to the client */
    return_success(rtn.msg, DBNO);
    /* Build addresses to send email to */
    num = strcpy2(recipients, from_email, "@freescale.com");
    if (strcmp(to_email, NONE) != 0)
    strcpy3(recipients+num, " ", to_email, "@freescale.com");
    /* Send the message */
    email_send("", recipients, "PC Number Retrieval", rtn.msg);
    /* Log pc  names in log file */
    data_write2log("contract_tables/pc_", current_pc, name, from_email, orig_mask, description, remarks);
    /* Send a copy of the file to the remote server */
    /* The file name must be in the files array */
    files[0] = malloc(PC_ROOT_LEN);
    strcpy(files[0], PC_ROOT);
    files[1] = malloc(PC_PREFIX_LEN+13);
    strcpy2(files[1], PC_PREFIX, tr.log_file);
    files[2] = NULL;
    files_shadow(CONTRACT_DIR, files);
    log3((char *)"Closing ", (char *)"transaction ", tr.pid_string);
    exit(0);
} /* End pc_name_rtv */


void build_new_mask(char *new_mask, char *current_mask)
{
    int ret;
    int new_mask_fnd = DBNO;
    int num;
    int cnt;
    char digits[3];

    /* Copy old mask into new mask */
    strcpy(new_mask, current_mask);
    /* do the following until a new mask number is found */
    /* Only try it 50 times */
    for (cnt = 0; cnt < 50; ++cnt)
    {
    /* Must increment the mask number in position s 1 and 2 first */
   strn_cpy(digits, current_mask+1, 2);
    num = atoi(digits);
    ++num;
    if (num == 100) num = 00;
    sprintf(digits, "%2d", num);
    if (digits[0] == ' ') new_mask[1] = '0';
    else new_mask[1] = digits[0];
    new_mask[2] = digits[1];
    /* If had to start back at 0, must increment fourth character */
    if (num == 0)
    {
	++new_mask[3];
	if (strchr("iloq", new_mask[3])) ++new_mask[3];
	else if (new_mask[3] == 123) new_mask[3] = 'a';
    } /* End if */
    /* If went through all digits, must increment first character in mask set */
    if ((num == 00) && (new_mask[3] == 'a'))
    {
	/* Go to the next letter for the first character */
	++new_mask[0];
	if (strchr("ioqu", new_mask[0])) ++new_mask[0];
	if (new_mask[0] == 123) shut_down(mask_name_err, MASK_NAME_ERR);
    } /* End if for incrementing first character of mask set name */
    /* if mask not found in MARS, fall out of loop */
    if ((ret = sql_mars_mask_check(new_mask)) == 0)
    {
	new_mask_fnd = DBYES;
	break;
    } /* end if */
	/* otherwise, must build a new mask set name */
	else strcpy(current_mask, new_mask);
  } /* End for */
    /* If tried to find mask 50 times and failed, give error */
  if (new_mask_fnd == DBNO)
    shut_down("mask not found", -1);
} /* End bld_new_mask */

void data_write2log(char *log_prefix, char *mask, char *name,
     char *user_id, char *orig_mask, char *description, char *remarks)
{
    char log_file[48];
    char line[512];
    char command[512];
    char date[12];

    /* Build complete log name */
    strcpy2(log_file, log_prefix, tr.log_file);
    /* Build line to put in log */
    /* Pad strings with spaces before concatenating */
    strcpy2(date, tr.log_date+4, "   ");
    /* Switch data in keys 2 and 3 to semicolons */
    strexc(mask_msg.key2, ',', ';');
    strexc(mask_msg.key3, ',', ';');
    /* Now concatenate string */
    sprintf(line, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s",
    mask, user_id, name, orig_mask,
    /* Fab tech and MBG, project leader, product number */
    mask_msg.key2, mask_msg.key3, user.pin,
    /* Product name, chipset, due date, eccn, description */
    mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4, mask_msg.par5, description, remarks);
    /* Get rid of ~ signs embedded in string */
    strexc(line, '~', ' ');
    /* log message */
    data_log(log_file, line);
    /* Call script to update in MySQL */
	strcpy3(command, "cd mars; perl add_mars_record.pl '", line, "'; cd ..");
    system(command);
} /* End data_write2log */


/* Verifies correct information entered for requester */
/* Pass in user ID or commerce ID in first parameter */
/* Returns user ID, department, and name */
void requester_whobu(char *id, char *user_id, char *name, char *dept)
{
     int ret;
     int exist_id = ldapfsl_get_user_name_dept(id, name, dept);
    /* See if this is a commerce ID */
    if (is_commerce_id(id) == DBYES)
    {
	    if (exist_id != DBYES)
            {
                shut_down(requester_cid_err, REQUESTOR_CID_ERR);
            }
    } /* End if */
    else if (exist_id != DBYES)
    {
	sprintf(rtn.msg, requester_uid_err, id);
	shut_down(rtn.msg, REQUESTOR_UID_ERR);
    } /* End else */
    strcpy(user_id, id);
} /* End requester_whobu */

/* Verifies correct information entered for receiver */
/* First argument holds user's user ID or commerce ID */
/* User's user ID is returned in the second argument */
int receiver_whobu(char *id, char *user_id)
{
     extern USER_PARAMS user;
    if (strlen(id) == 0) return(FAILURE);
    if (is_commerce_id(id) == DBYES)
    {
        if (ldapfsl_user_lookup_and_copy_global_structures(id) != DBYES)
	{
	    strcat(rtn.msg, receiver_cid_wrn);
	    return(RECEIVER_CID_WRN);
	} /* End if */
    } /* End if */
    strcpy(user_id, user.login);
    return(SUCCESS);
} /* End whobu */

/* See if this is a commerce ID */
int is_commerce_id(char *id)
{
    int cnt;
    if (strlen(id) != 8) return(DBNO);
    for (cnt = 0; cnt <8; ++cnt)
    {
	if ((id[cnt] < 0) || (id[cnt] > 9)) return(DBNO);
    } /* End for */
    /* If reached this point, it is a commerce id */
    return(DBYES);
} /* End is_commerce_id */

/* Retrieve info about specified mask set */
void mask_info_rtv(char *mask, char *user_id, char *log_name)
{
    char buffer[64];
    MARS_INFO md;
    FILE *po;
    char command[80];

    /* Verify user is valid Motorolan */
    if (ldapfsl_exists_account_by_coreid(user_id) != DBYES)
    {
        close_down1f(requester_uid_err, user_id, REQUESTOR_UID_ERR);
    }
    /* Build command to search for mask info */
    if (mask_msg.req_type == RTV_MASK_INFO)
    strcpy5(command, "cd contract_tables/", log_name, "; grep '^", mask, "' *");
    else strcpy5(command, "cd contract_tables/", log_name, "; grep '^", mask, "' *");
    if ((po = popen(command, "r")) == NULL)
    {
	old_mask_info_rtv(mask, user_id, log_name);
	exit(0);
    }
    if (fscanf(po, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%s",
    buffer, md.user_id, md.name,  md.orig_mask,
    /* Fab tech and MBG, project leader, product number */
    md.fab, md.tech, md.mbg, md.leader, md.leader_id,
    /* Product name, chipset, due date, eccn, description */
    md.product_num, md.product_name, md.chipset, md.duedate, md.eccn, md.desc, md.remarks) != 16)
    {
	old_mask_info_rtv(mask, user_id, log_name);
	exit(0);
    }
    /* Initialize date issued */
    strn_cpy(md.request_date, buffer, 8);
    md.request_date[2] = md.request_date[5] = '/';
    /* Initialize the mask set */
    strcpy(md.mask, buffer+9);
    /* Put data into rtn.msg */
    sprintf(rtn.msg,
    "set Version=\"2.0\";set DateIssued=\"%s\";set MasksetName=\"%s\";set Requester=\"%s\";set Name=\"%s\";set Orig_mask=\"%s\";\
set Fab=\"%s\";set Tech=\"%s\";set MBG=\"%s\";set Leader=\"%s\";set Leader_id=\"%s\";set ProductNumber=\"%s\";\
set ProductName=\"%s\";set ChipSet=\"%s\";set DueDate=\"%s\";set Eccn=\"%s\";set Desc=\"%s\";set Remarks=\"%s\";\n",
    md.request_date, md.mask, md.user_id, md.name, md.orig_mask,
    /* Fab tech and original mask, project leader, product number */
    md.fab, md.tech, md.mbg, md.leader, md.leader_id,
    /* Product name, chipset, due date, eccn, description */
    md.product_num, md.product_name, md.chipset, md.duedate, md.eccn, md.desc, md.remarks);
    put_short(SUCCESS);
     return_success(rtn.msg, DBNO);
} /* End mask_info_rtv */

void old_mask_info_rtv(char *mask, char *user_id, char *log_name)
{
    int len;
    char layers[8];
    char buffer[64];
    OLD_MARS_INFO md;
    FILE *po;
    char command[80];

    /* Verification of user done in previous function */
    /* Build command to search for mask info */
    if (mask_msg.req_type == RTV_MASK_INFO)
    strcpy5(command, "cd contract_tables/old_", log_name, "; grep '^", mask, "' *");
    else strcpy5(command, "cd contract_tables/old_", log_name, "; grep '^", mask, "' *");
    if ((po = popen(command, "r")) == NULL)
    {
	if (mask_msg.req_type == RTV_MASK_INFO)
	strcpy3(mti.suffix, "mask set '", mask, "'");
	else strcpy3(mti.suffix, "PC '", mask, "'");
	close_down1f(not_fnd_err, mti.suffix, NOT_FND_ERR);
    }
    if ((fscanf(po, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c",
    buffer, md.name1, md.dpt, md.user_id, md.old_mask,
    &layers, md.size, md.maskshop, md.date)) < 9)
    {
	if (mask_msg.req_type == RTV_MASK_INFO)
	strcpy3(mti.suffix, "mask set '", mask, "'");
	else strcpy3(mti.suffix, "PC '", mask, "'");
	close_down1f(not_fnd_err, mti.suffix, NOT_FND_ERR);
    }
    /* Grab rest of data for remarks field */
    if (line_get(po, md.remarks) <= 0) strcpy(md.remarks, "None");
    len = strlen(md.date);
    if (md.date[len] == '\n') md.date[len] = '\0';
    /* Initialize date issued */
    strn_cpy(md.request_date, buffer, 8);
    md.request_date[2] = md.request_date[5] = '/';
    /* Initialize the mask set */
    strcpy(md.mask, buffer+9);
    /* Put data into rtn.msg */
    sprintf(rtn.msg, "set Version=\"1.0\";set dateIssued=\"%s\";set maskName=\"%s\";set quantity=\"%d\";\
set Requestor=\"%s\";set Name=\"%s\";set Department=\"%s\";set originalMask=\"%s\";\
set maskLayers=\"%s\";set Shop=\"%s\";set maskUnit=\"%s\";set maskDate=\"%s\";set Remarks=\"%s\";\n",
    md.request_date, md.mask, 1,
    md.user_id, md.name1, md.dpt, md.old_mask,
    layers, md.maskshop, md.size, md.date, md.remarks);
    put_short(SUCCESS);
     return_success(rtn.msg, DBNO);
} /* End old_mask_info_rtv */

/* Get rid of HTML notation */
void html_filter(char *string)
{
    int fd;
    int ret;
    char command[80];
    char tmp_file[64];

    /* Build temporary file name */
    strcpy5(tmp_file, "db_data/tmp_files/", user.login, "_", tr.pid_string, ".tmp");
    data_log(tmp_file, string);
    strcpy2(command, "cat db_data/filter.html | ex - ", tmp_file);
    system(command);
    /* Read the text back in */
    fd = open(tmp_file, O_RDONLY);
    ret = read(fd, string, BLOCK_SIZE);
    string[ret] = '\0';
    close(fd);
    unlink(tmp_file);
    /* Put white space back in */
    strexc(string, '~', '\n');
} /* end html_filter */

/* Process global data requests */
/* Also processes system administrator requests */
void process_serial()
{
    FILE *fp;
    FILE*fopen();
    extern TMP_BUFFER bf;
    extern TMP_BUFFER bf;
    extern TMP_BUFFER bf;
    extern TMP_BUFFER bf;
    extern TMP_BUFFER bf;

    /* All contract numbers, serial numbers, mask set names, */
    /* PC names, and H numbers are retrieved from WACC */
    /* Make sure user is retrieving contract release  number from wacc site */
    if ((strcmp(ip.master_site, "wacc") != 0)
    && (mask_msg.req_type != 	RETRIEVE_CONTRACT_NUM))
    m2server_send("wacc");
    /* Perform specific action based on request type */
    switch (mask_msg.req_type)
    {
	case RTV_RET_MSO_PASSWORDS:
	{
	extern char ret_password[];
	extern char mso_password[];

	    put_short(SUCCESS);
    strcpy3(rtn.msg, ret_password, " ", mso_password);
	    return_success(rtn.msg, DBNO);
	}
	break;
	case RTV_RET_PASSWORD:
	{
	extern char ret_password[];

	    put_short(SUCCESS);
	    return_success(ret_password, DBNO);
	}
	break;
	/* Retrieve mask set names requested from MARS */
	case RETRIEVE_MASK_NAME:
	{
	    mask_name_rtv();
	}
	break;
	case RETRIEVE_PC_NAME:
	{
	    pc_name_rtv();
	}
	break;
	/* Retrieve a annual contract number */
	case RETRIEVE_ANNUAL:
	{
	    /* Open the CNB file */
	    if ((fp = fopen(CNB_FILE, "r")) == NULL)
	    close_down1m(rd_open_err, CNB_FILE, RD_OPEN_ERR);
	    /* Read until match found or reach end of file */
	    while (fscanf(fp, "%s %s", bf.buffer, bf.entry) != EOF)
	    {
		if (strcmp(bf.buffer, mask_msg.key1) == 0)
		{
		    /* Close the file */
		    fclose(fp);
		    mti.suffix_ptr = RTV;
		    /* Tell user transaction successful */
		    put_short(SUCCESS);
		    /* CNB number is in the entry */
		    put_string(bf.entry);
		    strcpy(mti.prefix, "CNB ");
		    put_ret_code(DBNO);
		    log3((char *)"Closing ", (char *)"transaction ", tr.pid_string);
		    exit(0);
		} /* End if */
	    } /* End while */
	    /* Close the file */
	    fclose(fp);
	    /* If reached this point, mask shop was not found */
	    /* Take the '* out of mask shop name and replace with ' ' */
	    strcpy_ex(bf.entry, mask_msg.key1, ' ', '*');
	    sprintf(rtn.msg, cnb_err, bf.entry);
	    shut_down(rtn.msg, CNB_ERR);
	}
	break;
	/* Read the global tables */
	/* Retrieve name of the C4 mask */
	    /* Retrieve one or more mos6 serial numbers */
	case RETRIEVE_MOS6SERIAL:
	{
	    rtv_serial(MOS6SERIAL_PREFIX);
	}
	break;
	/* Retrieve one or more mos16 serial numbers */
	case RETRIEVE_MOS16SERIAL:
	{
	    rtv_serial(MOS16SERIAL_PREFIX);
	}
	break;
	/* Retrieve one or more mos17 serial numbers */
	case RETRIEVE_MOS17SERIAL:
	{
	    rtv_serial(MOS17SERIAL_PREFIX);
	}
	break;
	/* Retrieve one or more mos21 serial numbers */
	case RETRIEVE_MOS21SERIAL:
	{
	    rtv_serial(MOS21SERIAL_PREFIX);
	}
	break;
	/* Retrieve a contract  release serial number for the RVU */
	/* Currently being used for Dupont and Photronics */
	case RETRIEVE_CONTRACT_NUM: contract_num_retrieve(); break;
	case RTV_MASK_INFO:  mask_info_rtv(mask_msg.key1, mask_msg.key2, "mask_log"); break;
	case RTV_PC_INFO:  mask_info_rtv(mask_msg.key1, mask_msg.key2, "pc_log"); break;
	default:
	{
	    /* Some other functions have been moved to process list */
	    /* Return failure code to client */
	    close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
	} /* end default */
    } /* end switch */
	    exit(0);
} /* end process_serial */
