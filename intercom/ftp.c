/****************************************************************************
 *
 * Function:	ftp functions
 *
 * Description:	Routines used to send data via FTP
 *
 ***************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "ftp_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "close_down.h"
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_server_globals.h"
#include "trans_server_errs.h"
#include "trans_server_macros.h"
#include "reg_globals.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"
#include "trans_defines.h"
#include "global_debug.h"

#define Ftpmkdir(ftp,dir)           Ftpcommand(ftp,"MKD %s",dir,200,257,EOF)
#define SEND_SIZE 1024*1024*2
#define DEFAULT_DIR "nxp_data"

/*!
 *  If this file "db_admin/parallel_ftp_sftp_disabled.config",   exists. it means:
           we are forcing SERIAL_TRANSFER anyway

    Also the compiler directive FORCE_SERIAL_TRANSFTP_TRANSFER also disables de parallel transfer
 */
#define CONFIG_FTP_ALLOW_PARALLEL_TRANSFER()  (access("db_admin/parallel_ftp_sftp_disabled.config", 0) != 0)


COMPANY_DATA cp;
extern TRANS_FILE_STAT fs;
FTP_TRANSFER_PARAMS ft;
    long long local_size;
long long offset;
    char email_buffer[10000];
int is_foundry;
int use_proxy;

extern choose_SFTP_or_FTP(char*);  // defined in intercom/send2foundy.c

/* Build extra info from send2foundry */
void extra_data_build(char *extra_data, char *description)
{
    char job_number[16];
    char to_dir[FILE_SIZE];
    char *list[20];
    int list_cnt;
    int fnd = DBNO;
    int cnt;

    DBG();

    job_number[0] = 0;

    /* Indicate no GDB ID found */
    ft.gdb_id = 0;
    /* convert description to string format */
    list_cnt = string2list_convert(description, list);
    /* Go through list looking for GDB */
    for (cnt = 0; cnt < list_cnt; ++cnt)
    {
	if (strcasecmp(list[cnt], "GDB") == 0)
	{
	    fnd = DBYES;
	    break;
	} /* end if */
    }
    /* if item found, build string */
    if ((fnd == DBYES)
    && (list_cnt >= cnt+3))
    {
	strcpy(job_number, list[2]);
	/* Scan in the GDB ID and directory */
	sscanf(list[cnt+2], "%d", &ft.gdb_id);
	strcpy(to_dir, list+3);
	if (ft.gdb_id == 0) return;
	/* Now grab needed info from the database */
	sql_gdb_data_rtv(extra_data, ft.gdb_id);
	/* Put the keyword in the send2foundry job table */
	sql_send2foundry_results_update(job_number, fs.key);
    strcpy(gu.par4, "send2foundry");
    }
}

void ftp_email_send(char **email_array)
{
    char subject[80];
    int cnt;

    DBG();

    strcpy2(subject, "Data transfer to ", cp.name);
    /* Send email to the NXP sender */
    email_send("", gu.email1, subject, email_buffer);
    data_log2(tr.log_file, "Sent e-mail notification to ", gu.email1);
    /* Send e-mail to team */
    email_send("", "transbug@nxp.com", subject, email_buffer);
    data_log(tr.log_file, "sending email notification to team");
    /* If other user specified, send him e-mail notification */
    if (strcmp(gu.par1, NONE) != 0)
    {
	for (cnt = 0; email_array[cnt] != '\0'; ++cnt)
	email_send("", email_array[cnt], subject, email_buffer);
	data_log2(tr.log_file, "Sent email notification to ", gu.par1);
    } /* end if for sending to other users */
    /* Send e-mail to the system administrator */
    strcat(subject, " for admin");
    email_send("", admin.from_email, subject, email_buffer);
    data_log2(tr.log_file, "Sent email notification to ", admin.from_email);
} /* end ftp_email_send */
/* Scan in params from history file */
/* These come from the first transfer if present */
void ftp_params_scan(void)
{
    FILE *fp;
    char line[260];
    char history_file[36];
    char param[64];
    char value[FILE_SIZE];
    int len;

    DBG();

    /* Set default values */
    ft.local_finish_time[0] = '\0';
    /* Build name for history file */
    strcpy3(history_file, "history_data/", fs.key, ".history");
    if ((fp = fopen(history_file, "r")) == NULL) return;
    while (line_get(fp, line) >= 0)
    {
	len = sscanf(line, "%[^:]", param);
	if (len == 1)
	{
	    strcpy(value, line+strlen(param) + 1);
	    str_strip_leading_spaces(value);
	}
	str_strip_leading_spaces(param);
    if (strcmp(param, "Start date") == 0) strcpy2(ft.start_time, value, "\n");
    else if (strcmp(param, "Local transfer time") == 0) strcpy(ft.local_transfer_time, value);
    } /* end while */
} /* end ftp_params_scan */

/* Build message to send to user */
void ftp_serial_email_build(char *checksum, char *remote_file, char *remote_dir, char *extra_data, time_t ftp_start_time)
{
extern COMPANY_DATA cp;
extern char email_buffer[];
    char *file_type_ptr;

    DBG();

    if (fs.file_type == 'd')
    file_type_ptr = "Server directory name";
    else file_type_ptr = "Server file name";
    /* time to perform local transfer calculated when ftp_send called */
    /* Calculate FTP transfer time */
    elapsed_time_calc(ftp_start_time, ft.remote_transfer_time, ft.remote_finish_time);
    int len = sprintf(email_buffer, "Data transfer to %s:\n\n\
    %s%s:  %s\n\
    Cksum for local zipped data:  %s\n\
    Remote file name:  %s\n\
    Remote directory:  %s%s\n\
    Remote login:  %s\n\
    Remote machine:  %s (%s)\n\
    Start date:  %s\
    Local transfer time:  %s\n\
    Local transfer date:  %s\n",
cp.name, extra_data, file_type_ptr, fs.real_name, checksum, remote_file, cp.root_dir, remote_dir,
    cp.login, cp.machine, cp.address,
    ft.start_time, ft.local_transfer_time, ft.local_finish_time) ;
    sprintf(email_buffer+len,  "    FTP transfer time:  %s\n\
    FTP transfer date:  %s\n\
    Original size:       %llu bytes\n\
    Compressed size:    %llu bytes\n\
    Encrypted size:      %llu bytes\n\
    Transfer size:      %llu bytes\n\
    Software version:  %s\n",
    ft.remote_transfer_time, ft.remote_finish_time,
    fs.original_size, fs.compressed_size, ft.encrypt_size, ft.transfer_size, TRANS_VERSION);
} /* End ftp_serial_email_build */

/* Build remote file name */
void remote_file_name_bld(char *remote_file)
{
    char *ptr;

    DBG();

    strcpy(remote_file, fs.real_name);
    /* If sent with the -f option, need to strip off everything after the space */
    if ((ptr = strchr(remote_file, ' ')) != NULL)
    ptr[0] = '\0';
    /* If this is a directory need to add the .tar suffix */
    if ((fs.file_type == DIRECTORY) || (fs.file_type == FILES))
    {
        strcat(remote_file, ".tar");
    }
    /* build remote file name */
    /* If data compressed, it has a .gz in it */
    if (fs.original_size != fs.compressed_size && strcasestr(remote_file, ".gz") == NULL)
    {
        strcat(remote_file, ".gz");
    }
    /* If this file will be encrypted, add the .gpg suffix */
    if (strcasecmp(cp.encrypt_key, "none") != 0 && strcasestr(remote_file, ".gpg") == NULL)
    {
        strcat(remote_file, ".gpg");
    }
} /* end remote_file_name_bld */

void cksum_build(char *cksum, char *local_file)
{
    FILE *po;
    char command[FILE_SIZE];

    DBG();

    /* Get the cksum for the original file */
    strcpy2(command, "cksum ", local_file);
    po = popen(command, "r");
    fscanf(po, "%s", cksum);
    pclose(po);
} /* end cksum_build */


/* Read in defined companies */
void company_file_read(void)
{
    extern COMPANY_DATA cp;
    FILE *fp;
    char line[260];
    int len = strlen(gu.to_company);

    DBG();


    if ((fp = fopen(FTP_MAPPING, "r")) == NULL)
    close_down1m(rd_open_err, FTP_MAPPING, RD_OPEN_ERR);
    while (line_get(fp, line) != EOF)
    {
        if (strncmp(gu.to_company, line, len) == 0)
        {
            /* Close file no longer needed */
            fclose(fp);
            if (sscanf(line, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%d %d",
                       cp.company, cp.name, cp.machine, cp.address, cp.login, cp.password, cp.root_dir, cp.encrypt_key, &is_foundry, &use_proxy) != 10)
            {
                DBG("calling close_down1m()");
                close_down1m(read_err, FTP_MAPPING, READ_ERR);
            }
            /* when no password, set password as 0 in the ftp_mapping */
            if (cp.password[0] == '0' && cp.password[1] == '\0')
            {
                cp.password[0] = 0;
            }
            strcpy3(cp.user, cp.login, "@", cp.address);
            DBG("return");
            return;
        } /* End if */
    } /* End while */
    /* If reached this point, company not found */
    DBG("calling close_down1m()");
    close_down1m(comp_err, gu.to_company, COMP_ERR);
} /* End company_file_read */

static char *percent(long long size, long long total_size)
{
    /* First calculate percentage */
    static char percent[16];
    float x = 100.0 *size/total_size;
    /* If over 100%, display it as 99.9 */
    if (x >= 100.0) x = 99.9;
    sprintf (percent, "  (%0.1f%%)", x);
    DBG("size=%lld total_size=%lld percent=%s", size, total_size, percent);
    return(percent);
} /* End percent */

static char* num_bytes (long long bytes)
{
  static char byte_string [64];

  if (bytes < 1024 * 1024)
	sprintf (byte_string, "%lld bytes", bytes);
    else if (bytes < 1024*1024 * 1024)
	sprintf (byte_string, "%0.2f MB", 1.0 * bytes / 1024 / 1024);
	else sprintf (byte_string, "%0.4f GB", 1.0 * bytes / 1024 / 1024/1024);
  DBG("bytes=%lld byte_string=%s", bytes, byte_string);
  return byte_string;
} /* End num_bytes */

void encrypt_itar_data(void)
{
    char file_name[PATH_SIZE];
    char command[PATH_SIZE];
    int  status = 0;

    /* Look up the company */
    company_file_read();
    /* Build local file name */
    strcpy3(file_name, user.cwd, "/", gu.key1);
	sprintf(command, "gpg --output %s.gpg --batch --recipient %s --encrypt %s",
	file_name, cp.encrypt_key, file_name);
	data_log(tr.log_file, command);
    status = system(command);

    DBG("command=[%s] return code=%d", command, status);

    /* Verify the encrypted file was created */
    strcat(file_name, ".gpg");
    if (access(file_name, 0) == 0)
    {
        put_short(SUCCESS);
        /* Tell client encryption successful */
        strcpy3(rtn.msg, "Encryption of '", gu.key1, "' was successful");
        put_success_msg(rtn.msg);
    }
    else shut_down("Encryption failed", -1);
} /* End encrypt_itar_data */

long elapsed_time_calc(long old_time, char *hms, char *date_string)
{

    time_t new_time = time(0);
    char *time_ptr = ctime(&new_time);

    DBG();

    /* Put in hour, minute, second representation */
    hms_calc(new_time-old_time, hms);
    /* Copy time string into variable where it will not be overwritten */
    time_ptr[24] = '\0';
    strcpy(date_string, time_ptr);
    return(new_time);
} /* End elapsed_time_calc */

/* Update the status in the meta data */
void ftp_status_update(char *key, char *string)
{
    char info_file[32];

    DBG("key=%s string=%s", key, string);

    strcpy2(info_file, "global_data/", key);
    data_log_offset(info_file, string, 16);
} /* end ftp_status_update */

void name_build(char *name)
{
     char time_stamp[24];

     DBG();

    /* Build time stamp */
    time_stamp_build(time_stamp);
    /* Name will consist of key plus the time stamp */
    strcpy2(name, gu.key1, time_stamp);
} /* End name_build */

/* Gather all information needed to encrypt and send the data */
/* this function initializes common things needed by encrypt_data and ftp_put */
/* It then calls both of these functions */
void ftp_send(char *remote_dir, char *description, char send_email, char *email_string, char *log_string, char **email_array)
{
    long encrypt_time;  
    char remote_file[FILE_SIZE];
    char history_file[32];
    char extra_data[1024] = "";

    DBG();
    if (description)
    {
        DBG("description=%s", description);
    }

    if (description[0] != '\0')
    extra_data_build(extra_data, description);
    else extra_data[0] = '\0';
    /* Check out from Designsync if needed */
    /* This function simply returns if check out is not needed */
    /* If checkout not needed, file name set in put_big_file */
    /* Otherwise it is initialized by the check out function */
    gdb_checkout(remote_dir, fs.key);
    /* Initialize the start date */
    char *tmp_start_time = ctime(&tr.upd_time);
    strcpy(ft.start_time, tmp_start_time);
    /* Calculate the local transfer time for future use */
    elapsed_time_calc(tr.upd_time, ft.local_transfer_time, ft.local_finish_time);
    /* Make this temporarily send2foundry */
    strcpy(gu.par4, "send2foundry");
    /* If extra info from Send to Foundry present, build extra message */
    /* Build local file name */
    strcpy3(ft.local_file, disk.path[fs.disk_index], fs.key, ".gz");
    strcpy3(ft.encrypt_file, disk.path[fs.disk_index], fs.key, ".gz.gpg");
    ft.encrypt_size = 0;
    strcpy3(history_file, "history_data/", fs.key, ".history");
    /* Build remote file name */
    remote_file_name_bld(remote_file);

    FORCE_DBG("cp.company=%s cp.is_fso=%d", cp.company, cp.is_fso);

    DBG_WAIT_LONGER();

    choose_SFTP_or_FTP(remote_dir);

    if (cp.is_fso == DBYES)
    {
        char local_description[64];
        strcpy2(local_description, "ITAR data for ", cp.company);
        ftp_put_transweb(ft.local_file, remote_file, ft.encrypt_size, send_email, email_string, local_description, log_string);
    }
#if !defined(FORCE_SERIAL_TRANSFTP_TRANSFER)
    else
    if ( CONFIG_FTP_ALLOW_PARALLEL_TRANSFER() &&
             (  (strncmp(cp.company, "Chartered", 9) == 0)
             || (strncmp(cp.company, "Local", 5) == 0)
             || (strncmp(cp.company, "Global Foundries", 16) == 0 && strcasestr(cp.company, "NTAS flow") == NULL)))
    {
        ftp_put_parallel(4, ft.local_file, remote_file, remote_dir,
                         history_file, extra_data, encrypt_time, email_array);
    }
#endif
    else
    {
        ftp_put_serial(4, ft.local_file, remote_file, remote_dir, history_file, extra_data, encrypt_time, email_array);
    }
    /* Insert transaction in dt_transactions table */
    sql_dt_transaction_insert (SUCCESS, rtn.msg);
} /* End ftp_send */


#ifdef LINUX
void ftp_put_serial(int offset, char *local_file, char *remote_file, char *remote_dir,
 char *history_file, char *extra_data, long encrypt_time, char **email_array)
{
    char command[80];
    char cksum[128] = "";
    char status_file[64];
    char time_stamp[32];
    time_t ftp_start_time;
    int total_blocks;

    /* Update status to indicate ready to start FTP */
    ftp_status_update(fs.key, "Transferring data.");
    /* Initialize the start date */
    ftp_start_time = time(0);
    ft.first_block_offset = 0;
    ft.adjusted_block_num = 1;
    ft.local_block_size = FTP_SERIAL_SIZE;
    /* Get the encryption running */
    /* Do not encrypt the data if this is a resend */
    if (gu.req_type != DBFTP_RESEND)
    ft.encryption_done = process_encrypt(local_file, remote_dir, remote_file, history_file, extra_data, &encrypt_time);
    else ft.encryption_done = DBYES;
    /* initialize everything needed for the data transfer */
    /* Build the time stamp */
    /* Not currently inserting the time stamp */
/*
    time_stamp_bld(time_stamp);
    time_stamp_insert(remote_file, time_stamp);
*/
    /* Get rid of leading slashes before anything else done */
    leading_slashes_remove(remote_dir);
    /* Calculate the number of blocks to send */

/* Each block will be 20000000 bytes in size */
    total_blocks = ((ft.encrypt_size)/ft.local_block_size) +ft.adjusted_block_num;
    ftp_start_time = time(0);
    /* Initialize the status file name */
    strcpy3(status_file, "history_data/", fs.key, ".status");
    /* If this file is already present, delete it */
    if (access(status_file, 0) == 0)
    unlink(status_file);
    /* Now create it as a blank file */
	strcpy2(command, "touch ", status_file);
    system(command);
    /* Now ready to send the data */

    /**
       Carlos Mazieri, Feb 2018, for serial transfers only one block is used
     */
#define JUST_ONE_SINGLE_BLOCK 1
    ftp_put_serial_blocks(ft.encrypt_file, remote_file, remote_dir, status_file, JUST_ONE_SINGLE_BLOCK);

    /* Build the cksum */
    cksum_build(cksum, local_file);
    /* Update completion status and encryption size in meta data */
    ftp_complete_update(fs.key, ft.encrypt_size);
    /* Build transfer time, transfer size, etc. to email_buffer */
    ftp_serial_email_build(cksum, remote_file, remote_dir, extra_data, ftp_start_time);
    /* Append new information to history file */
    data_log_offset(history_file, email_buffer, offset);
    /* Send email to sender, and admin */
    ftp_email_send(email_array);
    data_log(tr.log_file, rtn.msg);
    /* Update the SQL database for traceability */
    sql_intercom_history_update(fs.key);
} /* End ftp_put_serial */
#endif /* for Linux */


/* Have not yet updated this function */
void ftp_resend(char *remote_dir, char *key)
{
    struct stat buf;
    char extra_data[4] = "";
    char local_file[FILE_SIZE];
    char remote_file[FILE_SIZE];
    char history_file[32];
    char *email_array[1];
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    /* Get needed information from the meta data */
    FILE *fp = read_big_file(key, lines, &fs);

    DBG();

    /* close file no longer needed */
    fclose(fp);
    /* Make sure the encrypted data is present */
#ifdef LINUX
    strcpy3(ft.encrypt_file, disk.path[fs.disk_index], fs.key, ".gz.gpg");
#else
    strcpy3(ft.encrypt_file, disk.path[fs.disk_index], fs.key, ".gz");
#endif
    strcpy3(ft.local_file, disk.path[fs.disk_index], fs.key, ".gz");
    if (stat(ft.encrypt_file, &buf) != 0)
    {
	return_non_err(key);
    }
    /* Verify the file is not 0 bytes in length */
    if (buf.st_size == 0)
    shut_down(bad_encrypt_size, BAD_ENCRYPT_SIZE);
    /* Initialize the size */
    ft.encrypt_size = buf.st_size;
    /* Put to company info in right place */
    strcpy(gu.to_company, lines[13]+13);
    /* Read company file */
    company_file_read();
    /* Indicate e-mail notification is not being sent */
    email_array[0] = '\0';
    /* Indicate message will be sent */
    put_short(SUCCESS);
    strcpy7(rtn.msg, "Data with keyword '", fs.key, "' has been found on the TransFTP server.\n\
The data will be re-sent to ", cp.name, " over the internet using FTP.\n\
You can check the status of data transfer at any time with keyword '",
     fs.key,
"'.\nYou will receive email when data transfer has completed.");
    put_success_msg(fs.key);
    /* Build remote file name */
    remote_file_name_bld(remote_file);
    strcpy3(history_file, "history_data/", fs.key, ".history");
    /* Send the data to the remote company */
    sscanf(lines[3], "%*S %llu", &fs.compressed_size);
    /* Initialize values from previous transactions */

    ftp_params_scan();
    choose_SFTP_or_FTP(remote_dir);


    if ( CONFIG_FTP_ALLOW_PARALLEL_TRANSFER() &&
             (  (strncmp(cp.company, "Chartered", 9) == 0)
             || (strncmp(cp.company, "Local", 5) == 0)
             || (strncmp(cp.company, "Global Foundries", 16) == 0)))
    {
        ftp_put_parallel(4, ft.local_file, remote_file, remote_dir, history_file, extra_data, tr.upd_time, email_array);
    } /* end if */
    else
    {
        ftp_put_serial(4, ft.local_file, remote_file, remote_dir, history_file, extra_data, tr.upd_time, email_array);
    }
} /* end ftp_resend */



void ftp_complete_update(char *key, long long encrypt_size)
{
    FILE *fp;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];

    DBG();

    fp = read_big_file(key, lines, &fs);
    /* Put in update lines */
    fs.mode = ft.encrypt_size;
    strcpy(lines[15], "Status:      Transfer complete");
    DBG("Before call write_big_file()");
    write_big_file(fp, lines, &fs);
    DBG("after call write_big_file()");
    /* Throw away extra data by using ftp_status_update */
    ftp_status_update(key, lines[15]);
} /* end ftp_complete_update */

/* build the md5sum */
void md5sum_build(char *md5sum, char *local_file)
{
    FILE *po;
    char command[FILE_SIZE];

    DBG();

    /* Get the md5sum for the original file */
    strcpy2(command, "md5sum ", local_file);
    po = popen(command, "r");
    fscanf(po, "%s", md5sum);
    pclose(po);
} /* end md5sum_build */

long long ftp_command_put(char *command, long long tmp_size)
{
    short ret;
    short len;
    long long total_size = 0;
    FILE *po;

    DBG();

    /* Open command to pipe data from client through */
    if ((po = popen(command, "w")) == NULL)
    close_down1m(rd_open_err, command,  RD_OPEN_ERR);
    /* Tell client ready to get text */
    put_short(SUCCESS);
    /* Read in blocks of text and write them out to the pipe */
    while ((ret = get_string(bf.buffer)) > 0)
    {
	len = fwrite(bf.buffer, ret, 1, po);
	total_size += len;
    } /* end while */
    /* Close pipe just wrote to */
    pclose(po);
    return(total_size);
} /* End ftp_command_put */

