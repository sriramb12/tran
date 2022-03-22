/****************************************************************************
 *
 * Function:	put_big_file
 *
 * Description:	Puts a big file in the InterCom/TransWeb database.
 *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "close_down.h"
#include "customer_table.h"
#include "external_intercom_prototypes.h"
#include "ftp_info.h"
#include "edc_defines.h"
#include "edc_prototypes.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "reg_globals.h"
#include "report_log.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_paths.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "trans_server_arrays.h"
#include "global_debug.h"

void big_file_put(char *file_keyword)
{
    DBG();

    REQ_TYPE transfer_type = transfer_type_set();
    if (transfer_type == SERIAL)
    {
        DBG("transfer_type == SERIAL");
        fs.compressed_size = rcv_file(disk.path[fs.disk_index],  file_keyword, DBNO,  &fs.original_size);
#ifdef TRANSWEB
    /* Use the original size if it is greater than 0 */
    /* This is done to avoid problems with incorrect sizes */
    if (gu.original_size > 0)
	    fs.original_size = gu.original_size;
#endif
    } /* End if */
    /* If specified by the user, send as separate blocks */
    else
    {
        if (transfer_type == DBPUT_SEPARATE)
        {
            DBG("transfer_type == DBPUT_SEPARATE");
            fs.compressed_size = rcv_separate(DBPUT_SEPARATE, disk.path[fs.disk_index], gu.key1, file_keyword, &fs.original_size);
            /* If client requested sending the data in parallel mode  */
            /* or size is > the configured parallel size & is a zipped file */
            /* send in parallel mode */
        }
        else
        {
            DBG("transfer_type=%d", transfer_type);
            fs.compressed_size = rcv_parallel(transfer_type, disk.path[fs.disk_index], gu.key1, file_keyword, &fs.original_size);
        }
    }

    DBG("end big_file_put()");
} /* end big_file_put */


/* Initialize return message sent to the client */
void rtn_msg_init(char *expiration_time_string, char *dlt_time_string, char *gmt_time_string, char *file_size_string, char *file_keyword)
{
    char url_string[100];

    DBG();

    /* Initialize return message sent to user */
    /* If this is external TransWeb, put in the URL */
    if (customer_type == INTERNAL_CUSTOMER)
        strcpy2(url_string, "\rExternal URL to access data: https://www.nxp.com/transweb/?", file_keyword);

    strcpy2(url_string, "\rTransWeb URL: https://nww.transweb.nxp.com/index.cgi?go=KEYWORD&KEYWORD=", file_keyword);

    if (fs.file_type == DIRECTORY)
        rtn.len = strcpy10(rtn.msg, "Directory '", fs.real_name, "' (size ", file_size_string,  ") transferred.\n",
            "Retrieve the directory with the keyword:  ", file_keyword, url_string,
            "\nThis directory will be deleted in ", expiration_time_string);
    else if (fs.file_type == FILES)
        rtn.len = strcpy10(rtn.msg, "Files '", fs.real_name, "' (size ", file_size_string,  ") transferred.\n",
            "Retrieve the files with the keyword:  ", file_keyword, url_string,
            "\nThese files will be deleted in ", expiration_time_string);
    else
        rtn.len = strcpy10(rtn.msg, "File '", fs.real_name, "' (size ", file_size_string,  ") transferred.\n",
        "Retrieve the file with the keyword:  ", file_keyword, url_string,
        "\nThis file will be deleted in ", expiration_time_string);

    /* Add more things to message to send to user */
    rtn.len += strcpy5(rtn.msg+rtn.len, " days.\nLocal Deletion Time:  ", dlt_time_string, "\nGreenwich Mean Time:  ", gmt_time_string, "\n\n");
} /* end rtn_msg_init */


/* Strip off the Windows path */
void windows_path_strip(char *file_name)
{
    int len;
    char *ptr;
    char tmp_file[FILE_SIZE];

    DBG();

    /* See if this file name contains a Windows path */
    /* First make sure the last character is not a slash */
    len = strlen(file_name)-1;

    if ((file_name[len] == '/') || (file_name[len] == '\\'))
        file_name[len] = '\0';

    if ((ptr = strrchr(file_name, '/')) != NULL)
    {
	    strcpy(tmp_file, ptr+1);
	    strcpy(file_name, tmp_file);
    } /* end if */
    else if ((ptr = strrchr(file_name, '\\')) != NULL)
    {
	    strcpy(tmp_file, ptr+1);
	    strcpy(file_name, tmp_file);
    } /* end if */
} /* End windows_path_strip */


/* verify that this is not CompuGraphics */
/* this check is based on their e-mail address */
int is_compugraphics(char *email)
{
    int len = strlen(email);

    DBG();

    if (len < 13) return(DBNO);
    else if (strcasecmp("@compsus.com", email+ len -12) == 0) return(DBYES);
    else return(DBNO);
} /* End is_compugraphics */


/* Get description, email, and set other params */
int email_get_set(char *description, char *email_array[], char *email, int *edc_auth)
{
    int customer_email = DBYES;
    int cnt = 0;

    DBG();

    /* Indicate blank description present */
    description[0] = '\0';
    get_string(description);

    /* If description is blank, create it */
    if (description[0] == '\0') description_create(description);
    get_string(email);

    /* Carlos Mazieri, Feb 2018
     * check if there is a company abbreviation such as "-rtc"
     */
    if (email[0] == '-')
    {
        TRANS_COMPANY * company_info = get_company_info(email);
        if (company_info != NULL)
        {
            strcpy(email,company_info->email);
        }
    }
    DBG("email=%s", email);

    /* otherwise if sending to NXP Support, change customer type */
    if ((customer == EXTERNAL_TRANSWEB) && (strcmp(email, SUPPORT_EMAIL) == 0))
        customer = EXTERNAL_MM;

    /* convert email addresses to list format */
    /* Change commas to spaces */
    strexc(email, ',', ' ');

    /* Change semicolons to spaces */
    strexc(email, ';', ' ');

    /* Change carriage returns to spaces */
    strexc(email, '\n', ' ');
    cnt = convert_emailString_2_list(email, email_array);

    /* If this is an external customer, make sure at least one address ends with "nxp.com" */
    if (customer_type == EXTERNAL_CUSTOMER)
        customer_email = check4email(email_array);

    /* If this is an internal customer, verify EDC authorization */
    if ((customer_type == INTERNAL_CUSTOMER) && (is_trusted_sender(gu.email1) == DBNO))
        *edc_auth = sql_pre_auth_check(email_array);

    return(cnt);
} /* End email_get_set */


void put_big_file(
    char *remote_dir,
    char *key,	/* File name passed from user */
    long long file_size)	/* Size of file */
{
    extern METADATA md;
    extern int deletion_days;
    int edc_auth = APPROVED;
    unsigned long long rcv_parallel();
    unsigned long long rcv_separate();
    int email_cnt = 0;
    char email[BUFFER_SIZE];
    int customer_email = DBYES;
    char *email_array[1000];
    extern char global_mask[];
    char description_buffer[2048];
     void num_gbytes_calc();
    char send_email;
    char file_type_get(char *file_name);
    extern TRANS_FILE_STAT fs;
    char file_size_string[32];
    FILE *fp;
    char file_buffer[1024];
    char global_file[64];
    char *file_ptr;
    char own_id[EMAIL_LEN];

    if (gu.email1[0] != 0)
    {
        strcpy(own_id, gu.email1);
    }
    else if (user.email[0] != 0)
    {
        strcpy(own_id, user.email);
    }
    else
    {
         strcpy(own_id, user.login);
    }
    DBG("own_id=%s", own_id);

#ifdef TRANS_FTP
    /* If using TransFTP, make sure company is present */
    company_file_read();
#endif

    /* Strip path off of keyword */
    windows_path_strip(key);

    /* Get index for disk to use */
    /* fs.disk_index = check_disk_usage(file_size/1024); */
    fs.disk_index = get_random_index();

    /* Build new keyword */
    keyword_create(fs.key, tr.user_site, 7);

    /* Calculate the delete time */
    /* Use value for deletion_days set in global_params file */
    if (deletion_days > 3)
    {
	    fs.dlt_time = day_dlt_time_calc(deletion_days, md.current_gmt_time_string, md.dlt_time_string, md.gmt_time_string);
	    sprintf(md.expiration_time_string, "%d", deletion_days);
    } else {
	    fs.dlt_time = trans_dlt_time_calc(md.current_gmt_time_string, md.dlt_time_string, md.gmt_time_string, md.expiration_time_string);
	    strcpy(md.expiration_time_string, "3 working");
    } /* End else */

    /* If user specified email address, set email notification on */
    if (strcmp(gu.email1, NONE)  != 0)
    {
	    if (strchr(gu.email1, '@') == NULL)
	        fs.email_flag = DBYES;
    } /* End if for email notification on */
    else fs.email_flag = DBNO;

    /* Set the file mode */
    /* If mode set to 0, use 0700 */
    if (gu.file_mode == 0)
        fs.mode = 33216;
    else
        fs.mode = gu.file_mode;

    /* Set the file type */
    fs.file_type = gu.file_type;

    if (fs.file_type == UNKNOWN)
        fs.file_type = file_type_get(key);

    /* Set the file type string */
    if ((fs.file_type == DIRECTORY) || (fs.file_type == HUGE_DIR))
        strcpy(fs.title, "Directory:   ");
    else if (fs.file_type == FILES)
        strcpy(fs.title, "Files:       ");
    else
        strcpy(fs.title, "File:        ");

    /* Get the file size */
    num_bytes_calc(file_size_string, file_size);

    /* Build name for file */
    strcpy(fs.real_name, key);
    fs.compression = (int)gu.compressed_size;

    /* Set the data classification */
    icap_class_set(fs.icap_class);
    strcpy2(global_file, GLOBAL_FILE_PATH, fs.key);

    DBG("opening global_file=%s ", global_file);
    if ((fp = fopen(global_file, "w")) == NULL)
        close_down1m(wr_open_err, global_file, WR_OPEN_ERR);

    /* Tell client successful up to this point */
    put_short(SUCCESS);

    /* Get description, e-mail and set necessary params */
    if ((strcmp(gu.par1, NONE) != 0) && (str_digits(gu.par1) != SUCCESS))
    {
	    if ((email_cnt = email_get_set(md.description, email_array, email, &edc_auth)) > 0)
	        email_description_build(description_buffer, md.description, email_array);

	    send_email = DBYES;
    } else {
	    email_array[0] = NULL;
	    email_cnt = 0;
	    send_email = DBNO;
    } /* End if */

    /* Set the delete after get flag */
    /* If sending e-mail to multiple users, do not deleted after first retrieval */
    if (IS_REQUEST_PUT_DELETE(gu.req_type) && email_cnt <= 1 )
    {
        fs.dlt_flag = DBYES;
    }
    else
    {
        fs.dlt_flag = DBNO;
    }




    /* Send file keyword to user */
#ifdef TRANS_FTP
    if (is_fso() == DBYES)
        strcpy2(rtn.msg, "Intermediate file keyword is ", fs.key);
    else
#endif
    strcpy2(rtn.msg, "File keyword is ", fs.key);
    put_string(rtn.msg);

    /** July 2020, changed from gu.email1 to user.email, because gu.email1 is not filled for non LDAP users such as dtms */
    sprintf(file_buffer, "%c %d %ld %s 2 0 %d l %d %d %s %s\n%s%s\nUser size:   %s\nCompressed:  0 bytes\nKeyword:     ",
        fs.file_type, fs.disk_index, fs.dlt_time, own_id, fs.dlt_flag, fs.mode, fs.compression, global_mask, gu.par2, fs.title, key, file_size_string);

    file_ptr  = file_buffer + strlen(file_buffer);
    file_ptr += strcpy6(file_ptr, fs.key, "\nCreator:     ", user.name, "\nCreated:     ", tr.log_date, "\n             ");
    file_ptr += strcpy6(file_ptr, md.current_gmt_time_string,  "\nExpires:     ", md.dlt_time_string, "\n             ", md.gmt_time_string, "\n");

    strcpy10(file_ptr, "Data type:   ", trans_filetype_array[fs.file_type-'a'],
        "\nPlatform:    ", gu.machine_type,
        "\nSender:      ", gu.from_company,
        "\nReceiver:    ", gu.to_company,
        "\nClassified:  ", fs.icap_class);

    DBG("saving global_file=%s ", global_file);
    fprintf(fp, "%s\n", file_buffer);

    if (send_email == DBYES) fprintf(fp, description_buffer);
        fclose(fp);

    if (customer_email == DBNO)
        shut_down(fss_email_err, FSS_EMAIL_ERR);

    /* Send the data */
    big_file_put(fs.key);

    /* Send ack to client */
    put_big_file_ack(global_file, fs.key,   email_array, email, remote_dir, edc_auth);
} /* end put_big_file */


/* Send file ack */
void put_big_file_ack(char *global_file, char *file_keyword,  char **email_array, char *email, char *remote_dir, int edc_auth)
{
#ifdef TRANS_FTP
    extern COMPANY_DATA cp;
#endif
    time_t dlt_time;
    int send_email;
    extern char global_mask[];
    extern METADATA md;
    extern char *secure_keyword_array[];
    char compressed_size_string[32];
    int len;
    char file_size_string[32];
    char log_string[512];
    char file_buffer[1024];
    char description_buffer[2048];
    char *file_ptr;
    char history_file[64];
    FILE *fp;
    int use_last;
    char own_id[EMAIL_LEN];

    if (gu.email1[0] != 0)
    {
        strcpy(own_id, gu.email1);
    }
    else if (user.email[0] != 0)
    {
        strcpy(own_id, user.email);
    }
    else
    {
         strcpy(own_id, user.login);
    }
    DBG("own_id=%s", own_id);


    /* Send keywords if data in e-mail array */
    if (email_array[0] != NULL)
        send_email = DBYES;
    else
        send_email = DBNO;

    /* If original size set to 0, always used size sent when transfer completed */
    if (gu.original_size == 0)
        use_last = DBYES;
    else if (fs.original_size > 150000000000)
        use_last = DBNO;
    else
        use_last = DBYES;

    if (use_last == DBYES)
        gu.original_size = fs.original_size;

	num_bytes_calc(file_size_string, fs.original_size);

    /* Now calculate the compressed size */
	num_bytes_calc(compressed_size_string, fs.compressed_size);

    if (fs.original_size == fs.compressed_size) fs.compression = DBNO;
    else fs.compression = DBYES;

    /* If authorization is pending, change the delete time */
    if (edc_auth == PENDING)
    {
	    fs.dlt_time = day_dlt_time_calc(30, md.current_gmt_time_string, md.dlt_time_string, md.gmt_time_string);
	    sprintf(md.expiration_time_string, "%d", 30);
        dlt_time = tr.upd_time +ONE_MONTH;
    } /* end if */

    /* Write out the file and indicate it is unlocked */
    sprintf(file_buffer, "%c %d %ld %s 2 0 %ld u %d %d %s %s\n%s%s\nUser size:   %s\nCompressed:  %s\nKeyword:     ",
        fs.file_type, fs.disk_index, fs.dlt_time, own_id, fs.dlt_flag, fs.mode, fs.compression, global_mask, gu.par2, fs.title,fs.real_name, file_size_string, compressed_size_string);

    file_ptr  = file_buffer + strlen(file_buffer);
    file_ptr += strcpy6(file_ptr,     file_keyword,
        "\nCreator:     ", user.name,
        "\nCreated:     ", tr.log_date, "\n             ");
        file_ptr += strcpy6(file_ptr, md.current_gmt_time_string,  "\nExpires:     ", md.dlt_time_string,
        "\n             ",  md.gmt_time_string, " \n");
        strcpy10(file_ptr, "Data type:   ", trans_filetype_array[fs.file_type-'a'],
        "\nPlatform:    ", gu.machine_type,
        "\nSender:      ", gu.from_company,
        "\nReceiver:    ", gu.to_company,
        "\nClassified:  ", fs.icap_class);

    DBG("saving global_file=%s ", global_file);
    /* Open the file for writing */
    if ((fp = fopen(global_file, "w")) == NULL)
        close_down1m(wr_open_err, global_file, WR_OPEN_ERR);

    fprintf(fp, "%s\n", file_buffer);
    fclose(fp);

    if (send_email == DBYES)
    {
	    email_description_build(description_buffer, md.description, email_array);
	    data_log(global_file, description_buffer);
    }

    /* Build name for history file */
    strcpy3(history_file, HISTORY_FILE_PATH, file_keyword, ".history");
    DBG("saving history_file=%s ", history_file);

    /* Open the history file */
    if ((fp = fopen(history_file, "w")) == NULL)
        close_down1m(wr_open_err, history_file, WR_OPEN_ERR);

    /* Write the deletion time */
    /* It is 14 days after the current expiration date */
    fprintf(fp, "%ld %s %s\n", tr.upd_time+1209600, gu.from_company, gu.email1);

    /* close the history file */
    fclose(fp);

#ifndef TRANS_FTP
    /* Initialize file size for return message */
    num_gmb_calc(file_size_string, fs.original_size);

    /* Initialize the return message sent to the user */
    rtn_msg_init(md.expiration_time_string, md.dlt_time_string, md.gmt_time_string, file_size_string, file_keyword);

    /* If this is an external customer, automaticly send him a secure keyword */
    /* he needs the secure keyword to access the data */
    /* CompuGraphics does not want a secure keyword */
    if (customer_type == EXTERNAL_CUSTOMER)
        send_external_keyword(file_keyword, gu.email1, fs.real_name, file_size_string, file_buffer[0], dlt_time, md.dlt_time_string, md.gmt_time_string, md.description);

    if (send_email == DBYES)
    {
	    /* Send secure keywords to specified userss */
        send_transweb_keyword(file_keyword, email_array, fs.real_name,
            file_size_string, file_buffer[0], dlt_time, md.dlt_time_string, md.gmt_time_string,
            gu.to_company, md.description);
    } /* end if */

    /* If sender needs to send receiver file keyword, send reminder */
    /* Send file keyword to internal TransWeb and Intercom customers going to */
    /* external TransWeb or Intercom customers */
    if ((customer_type == INTERNAL_CUSTOMER) && (send_email == DBYES))
    {
	    if (customer == INTERNAL_MM)
	        mm_file_key_send(fs.real_name, md.description, file_keyword, file_size_string, md.dlt_time_string, md.gmt_time_string, email_array, secure_keyword_array);
	    else
        {
	        transweb_file_key_send(fs.real_name, md.description, file_keyword, file_size_string, md.dlt_time_string, md.gmt_time_string, email_array, secure_keyword_array);
	        rtn.len += strcpy5(rtn.msg+rtn.len, "Sending file keyword '", file_keyword, "' to '", gu.email1, "'.\n");
	    } /* end else */
    } /* End if for sending file keyword to sender */

    /* Put the disk index in the log file */
    sprintf(bf.buffer, "Disk index %d for %s", fs.disk_index, file_keyword);
    data_log(tr.log_file, bf.buffer);

    /* Build the message for the log file on the user's disk */
    len = strcpy6(log_string, file_keyword, " #File:  ", fs.real_name, " Directory:  ", user.cwd,  "  Created:  ");
    strcpy3(log_string+len, tr.log_date, "  Expires:  ", md.dlt_time_string);

    /* Send the user the return code */
    /* put_msg used instead of put_success_msg so log string is sent */
    put_msg(file_keyword, log_string);
#endif /* for not TRANS_FTP */

#ifdef TRANS_FTP
    /*
     * Oct 28 2014 Carlos Mazieri
     *
     * It was missing this ACK for transfers between "icom x transFTP"
     * causing the the icom to be used with "-ps"
     */
       DBG("DOING extra ACK ...");
       put_short(SUCCESS);

    /* Initialize message sent to user */
    rtn.len = strcpy7(rtn.msg, "Data has been transferred to the ", server.site, " TransFTP server.\n\
The data will be encrypted and sent to ", cp.name, " over the internet using FTP.\n\
You can check the status of data transfer at any time with keyword '", file_keyword,
"'.\nYou will receive email when data transfer has completed.");

    if (send_email == DBYES)
	    strcpy(rtn.msg+rtn.len, "\nEmail notification will also be sent to additional users at that time.");

    put_msg(file_keyword, log_string);

    /* Initialize name for use in next function */
    strcpy(fs.real_name, gu.key1);

    /* Get rid of any spaces in file name */
    strexc(fs.real_name, ' ', '_');
    strcpy(fs.key, file_keyword);

    /* Send data to TSMC, Global Foundires, etc */
    ftp_send(remote_dir, md.description, send_email, email, email, email_array);
#endif
} /* End put_big_file_ack */


/* Verify at least one E-mail message ends in @nxp.com */
int check4email(char *email_list[])
{
    int cnt;
    int fnd = DBNO;
    int len;

#ifdef DEBUG
    int i;
    for (i = 0; email_list[i] != NULL; ++i)
        DBG("email_list[%d] = '%s'", i, email_list[i]);
#endif

    for (cnt = 0; email_list[cnt] != NULL; ++cnt)
    {
	    len = strlen(email_list[cnt]);

	    /* If address too short, do not check it */
	    if (len < 8) continue;

	    if (strcasecmp(email_list[cnt]+len -8, "@nxp.com") != 0)
	    {
	        fnd = DBYES;
	        //put_string("Transaction aborted.");
	        //shut_down(external_email_err, EXTERNAL_EMAIL_ERR);
	    } /* End if */
    } /* End for */

    return(fnd);
} /* end check4email */
