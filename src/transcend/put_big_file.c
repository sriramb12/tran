/****************************************************************************
 *
 * Function:	put_big_file
 *
 * Description:	Puts a big file in the Transcend database.
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_defines.h"
#include "trans_server_defines.h"
#include "trans_server_paths.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_arrays.h"
#include "trans_lib_prototypes.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

extern TRANS_FILE_STAT fs;

/* This function decides whether to do serial or parallel transfer and performs the transfer */
void big_file_put(char *file_keyword)
{
    DBG("file_keyword='%s'", file_keyword);

    REQ_TYPE transfer_type = transfer_type_set();

    if (transfer_type == SERIAL)
    {
        fs.compressed_size = rcv_file(disk.path[fs.disk_index], file_keyword, DBNO,  &fs.original_size);
#ifdef TRANSWEB
        /* Use the original size if it is greater than 0 */
        /* This is done to avoid problems with incorrect sizes */
        if (gu.original_size > 0)
	        fs.original_size = gu.original_size;
#endif
    } else if (transfer_type == DBPUT_SEPARATE) {
        /* If specified by the user, send as separate blocks */
        fs.compressed_size = rcv_separate(DBPUT_SEPARATE, disk.path[fs.disk_index], gu.key1, file_keyword,  &fs.original_size);
    } else {
        /* If client requested sending the data in parallel mode or size is > the configured
         * parallel size & is a zipped file send in parallel mode */
        fs.compressed_size = rcv_parallel(transfer_type, disk.path[fs.disk_index], gu.key1, file_keyword,  &fs.original_size);
    }
    DBG("end big_file_put()");
} /* end big_file_put */


int never_expires(void)
{
    DBG();

    /* If this is the DTMS server, and the -pd option was specified,  */
    /* and user is on test list, file never expires  */
    if ((strcmp(server.site, "DTMS") == 0) && (strcmp(gu.par6, "d") == 0) && (sql_is_dtms_tester(user.email) == DBYES))
        return(DBYES);
    else
        return(DBNO);
} /* end never_expires */


/* Strip off the Windows path */
void windows_path_strip(char *file_name)
{
    DBG("file_name='%s'", file_name);

    int len;
    char *ptr;
    char tmp_file[FILE_SIZE];

    /* See if this file name contains a Windows path */
    /* First make sure the last character is not a slash */
    len = strlen(file_name) - 1;

    if ((file_name[len] == '/') || (file_name[len] == '\\'))
        file_name[len] = '\0';

    if ((ptr = strrchr(file_name, '/')) != NULL)
    {
	    strcpy(tmp_file, ptr + 1);
	    strcpy(file_name, tmp_file);
    }
    else if ((ptr = strrchr(file_name, '\\')) != NULL)
    {
	    strcpy(tmp_file, ptr + 1);
	    strcpy(file_name, tmp_file);
    } /* end if */
} /* End windows_path_strip */


int dtms_send(char *key, char *file_keyword)
{
    DBG("key='%s', file_keyword='%s'", key, file_keyword);

    int len = strlen(key);

    /* If this is a zipped tarred directory, uncompress and untar data as it is written to disk */
    if (strcasecmp(key+len-7, ".tar.gz") == 0)
    {
	    key[len-7] = '\0';
	    fs.compressed_size = command_put_compressed_dir(file_keyword);
	    gu.file_type = DIRECTORY;
	    strcpy(fs.title, "Directory:   ");

	    /* Build the transdirlist file */
	    transdirlist_build(file_keyword, key, fs.disk_index);

	    return(DBYES);
    } else if (gu.file_type == DIRECTORY)
    {
        /* Untar and unzip directory which already has a file list */
        fs.compressed_size = command_put_compressed_dir(file_keyword);
	    return(DBYES);
    } else
        return(DBNO);
} /* End dtms_send */


/* Verify all email being sent to NXP customers */
void company_email_check(char *email_array[])
{
    DBG();

    char *email_ptr;
    int cnt;

    for (cnt = 0; email_array[cnt] != NULL; ++cnt)
    {
	    /* First strip off brackets and quotes */
	    str_quotes_strip(email_array[cnt]);

	    if ((email_ptr = strchr(email_array[cnt], '@')) == NULL) continue;

        DBG("email_ptr='%s'", email_ptr);
	    if (strcasecmp(email_ptr, "@nxp.com") != 0)
        {
	        sprintf(rtn.msg, nxp_email_err, server.site, server.site);
	        shut_down(rtn.msg, NXP_EMAIL_ERR);
	    } /* End if for invalid e-mail address */
    } /* End for */
} /* end company_email_check */


/* Parameter:
 *  File name passed from user */
/*  Size of file */
void put_big_file(char *key, unsigned long long file_size)
{
    DBG("key='%s', file_size='%llu'", key, file_size);

    extern METADATA md;
    int email_num;
    char *email_array[1000];
    char email[BUFFER_SIZE];
    char file_size_string[32];
    char global_file[64];
    FILE *fp;
    char file_buffer[1024];
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

    /* Strip path off of keyword */
    windows_path_strip(key);

    /* Get index for disk to use */
    /* fs.disk_index = check_disk_usage(file_size); */
    fs.disk_index = get_random_index();

    /* Build keyword for the data */
    keyword_create(fs.key, tr.user_site, 7);

    /* Calculate the deletion time */
    /* See if file will never expire */
    if (never_expires() == DBYES)
        fs.dlt_time = no_expiration_calc(md.current_gmt_time_string, md.dlt_time_string, md.gmt_time_string, md.expiration_time_string);
    else
        fs.dlt_time = trans_dlt_time_calc(md.current_gmt_time_string, md.dlt_time_string, md.gmt_time_string, md.expiration_time_string);

    /* Save current time for later use */
    strcpy(md.current_time_string, tr.log_date);

    /* Set the delete after get flag */
    fs.dlt_flag = IS_REQUEST_PUT_DELETE(gu.req_type) ? DBYES : DBNO;

    /* Always using default of no for email notification */
    /* Code to set it to DBYES has been taken out */
    fs.email_flag = DBNO;

    /* Set the file type */
    if (gu.file_type == UNKNOWN)
        gu.file_type = UNSPECIFIED;

    /* Set the file type string */
    if (gu.file_type == DIRECTORY)
        strcpy(fs.title, "Directory:   ");
    else if ((gu.file_type == FILES) || (fs.file_type == HUGE_FILES))
        strcpy(fs.title, "Files:       ");
    else
        strcpy(fs.title, "File:        ");

    /* Copy in the file name */
    strcpy(fs.real_name, gu.key1);

    /* Save file mode for later use */
    /* If mode of 0 present, set mode to 0700 */
    if (gu.file_mode == 0)
        fs.mode = 33216;
    else
        fs.mode = gu.file_mode;

    /* Get the file size */
    num_bytes_calc(file_size_string, file_size);
    fs.compression = (int) gu.compressed_size;

    /* Set the iCAP classification */
    icap_class_set(md.icap_class);

    sprintf(file_buffer, "%c %d %ld %s %d 0 %d l %ld %d %s\n%s%s\nUser size:   %s\nCompressed:  0 bytes\nKeyword:     ",
    gu.file_type, fs.disk_index, fs.dlt_time, own_id, fs.email_flag,
    fs.dlt_flag, fs.mode, fs.compression, gu.par2, fs.title, key, file_size_string);
    file_ptr = file_buffer + strlen(file_buffer);
    file_ptr += strcpy10(file_ptr,     fs.key, "\nCreator:     ", user.name,
    "\nCreated:     ", tr.log_date, "\n             ", md.current_gmt_time_string,
    "\nExpires:     ", md.dlt_time_string,
    "\n             ");

    strcpy10(file_ptr,
    md.gmt_time_string,
     "\nData type:   ", trans_filetype_array[gu.file_type-'a'],
    "\nPlatform:    ", gu.machine_type,
    "\nServer site:   ", server.site,
    "\nClassified:  ", md.icap_class, "\n");

    /* Build name for file */
    strcpy2(global_file, GLOBAL_FILE_PATH, fs.key);
    if ((fp = fopen(global_file, "w")) == NULL)
        close_down1m(wr_open_err, global_file, WR_OPEN_ERR);

    fprintf(fp, "%s", file_buffer);
    fclose(fp);

    /* Get the file */
    /* Tell client successful up to this point */
    put_short(SUCCESS);

    /* Get description from user */
    fs.lock_flag = 'u';
    if (strcmp(gu.par1, "0") != 0)
    {
	    get_string(md.description);

        /* Get string that contains email address data will be sent to */
	    get_string(email);
	    DBG("email address data will be sent to is '%s'", email); 

        /* Convert commas to spaces */
        DBG("Before call strexc(email, ',', ' '); where email='%s'", email);
        //DBG_WAIT_LONGER();
	    strexc(email, ',', ' ');
        DBG("After call strexc(email, ',', ' ');");

        /* Convert semicolons to spaces */
        DBG("Before call strexc(email, ';', ' ');");
	    strexc(email, ';', ' ');
        DBG("After call strexc(email, ';', ' ');");

        /* convert email addresses to list format */
        DBG("Before call string2list_convert()");
        email_num = convert_emailString_2_list(email, email_array);
        DBG("After call string2list_convert()");

        /* Append description and e-mail list to global file */
        strcpy4(file_buffer, "Description: ", md.description, "\nCustomer:    ", email);
	    data_log(global_file, file_buffer);

	    /* Do not allow deletion after the first retrieval if more than one e-mail address specified */
        if (email_num > 1)
        {
            fs.dlt_flag = DBNO;
        }
    } else
    {
        email_array[0] = NULL;
    }

    /* Send file keyword to user */
    strcpy2(rtn.msg, "File keyword is ", fs.key);
    put_string(rtn.msg);

    /* Verify e-mail addresses are only for NXP customers */
    DBG("Before verify email address");
    if (email_array[0] != NULL)
	    company_email_check(email_array);
    DBG("After verify email address");

    DBG("Before big_file_put(fs.key)");
    big_file_put(fs.key);
    DBG("After big_file_put(fs.key)");

    DBG("Before put_trans_file_ack(global_file, email_array)");
    put_trans_file_ack(global_file, email_array);
    DBG("After put_trans_file_ack(global_file, email_array)");

    DBG("End put_big_file()");
} /* End put_big_file */


/* Send data to user when put completed */
void put_trans_file_ack(char *global_file, char *email_array[])
{
    DBG("global_file='%s'", global_file);

    extern char keyword_link_extension[];
    extern METADATA md;
    char description_buffer[1024];
    char compressed_size_string[32];
    int len;
    char file_size_string[32];
    char history_file[64];
    char log_string[1024];
    char file_buffer[1024];
    char *file_ptr;
    FILE *fp;
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


    /* If the compressed size is negative, make it positive */
    if (fs.compressed_size < 0)
        fs.compressed_size = 0 - fs.compressed_size;

    /* If the original size is negative, make it positive */
    DBG("fs.original_size='%d'", fs.original_size);
    if (fs.original_size < 0)
        fs.original_size = 0 - fs.original_size;

    /* Use the original size if it is greater than 0 */
    /* This is done to avoid problems with incorrect sizes */
    if (gu.original_size > 0)
	    fs.original_size = gu.original_size;

    DBG("fs.original_size='%d'", fs.original_size);

	num_bytes_calc(compressed_size_string, fs.compressed_size);
	num_bytes_calc(file_size_string, fs.original_size);

    /* If two sizes equal and not transferring directory on DTMS */
    /* Mark the data uncompressed */
    if ((fs.original_size == fs.compressed_size) && (fs.file_type != DIRECTORY) && (fs.file_type != FILES))
        fs.compression = DBNO;

    /* Reset file type if needed */
    if (gu.file_type == UNKNOWN)
        gu.file_type = UNSPECIFIED;

    /* Write out the file and indicate it is unlocked */
    sprintf(file_buffer, "%c %d %ld %s %d 0 %d %c %ld %d %s\n%s%s\nUser size:   %s\nCompressed:  %s\nKeyword:     ",
        gu.file_type, fs.disk_index, fs.dlt_time, own_id, fs.email_flag,
        fs.dlt_flag, fs.lock_flag, fs.mode, fs.compression, gu.par2, fs.title,
        fs.real_name, file_size_string, compressed_size_string
    );

    file_ptr = file_buffer + strlen(file_buffer);

    file_ptr += strcpy7(file_ptr,     fs.key, "\nCreator:     ", user.name,
    "\nCreated:     ", md.current_time_string, "\n             ", md.current_gmt_time_string);

    file_ptr += strcpy4(file_ptr, "\nExpires:     ", md.dlt_time_string,
    "\n             ", md.gmt_time_string);

    file_ptr += strcpy9(file_ptr, "\nData type:   ",
    trans_filetype_array[gu.file_type-'a'],
    "\nPlatform:    ", gu.machine_type, "\nSite:        ",
    server.site,
    "\nClassified:  ", md.icap_class, "\n");

    if ((fp = fopen(global_file, "w")) == NULL)
        close_down1m(wr_open_err, global_file, WR_OPEN_ERR);

    fprintf(fp, "%s", file_buffer);

    /* If sending a secure keyword and description not empty write it */
    if (email_array[0] != NULL)
    {
	    email_description_build(description_buffer, md.description, email_array);
	    fprintf(fp, "%s\n", description_buffer);
    }
    fclose(fp);

    /* Build name for history file */
    strcpy3(history_file, HISTORY_FILE_PATH, fs.key, ".history");

    /* Open the history file */
    if ((fp = fopen(history_file, "w")) == NULL)
        close_down1m(wr_open_err, history_file, WR_OPEN_ERR);

    /* Write the deletion time */
    /* It is 14 days after the current expiration date */
    /* If the file does not expire, there is no expiration time */
    if (fs.dlt_time != 0) fs.dlt_time += 1209600;
    fprintf(fp, "%ld\n", fs.dlt_time);

    /* close the history file */
    fclose(fp);

    /* Initialize size for return message */
    DBG("fs.original_size='%d'", fs.original_size);
    str2size(file_size_string, fs.original_size);

    /* Initialize return message sent to user */
     if (gu.file_type == DIRECTORY)
        rtn.len = strcpy12(rtn.msg, "Directory '", fs.real_name, "' (size ", file_size_string,  ") transcended.\n", "Retrieve the directory with the keyword:  ", fs.key,
        "\rTransWeb URL: https://nww.transweb.nxp.com/index", keyword_link_extension, "?go=KEYWORD&KEYWORD=",
        fs.key, "\nThis directory ");

    else if (gu.file_type == FILES)
        rtn.len = strcpy12(rtn.msg, "Files '", fs.real_name, "' (size ", file_size_string,  ") transcended.\n", "Retrieve the files with the keyword:  ", fs.key,
        "\rTransWeb URL: https://nww.transweb.nxp.com/index", keyword_link_extension, "?go=KEYWORD&KEYWORD=",
        fs.key, "\nThese files ");

    else
        rtn.len = strcpy12(rtn.msg, "File '", fs.real_name, "' (size ", file_size_string,  ") transcended.\n", "Retrieve the file with the keyword:  ", fs.key,
        "\rTransWeb URL: https://nww.transweb.nxp.com/index", keyword_link_extension, "?go=KEYWORD&KEYWORD=", fs.key, "\nThis file ");

    /* Add more things to message to send to user */
    rtn.len += strcpy6(rtn.msg+rtn.len,  md.expiration_time_string, "Local Deletion Time:  ", md.dlt_time_string, "\nGreenwich Mean Time:  ", md.gmt_time_string, "\n\n");

    if (email_array[0] != NULL)
    {
        DBG("Before send_secure_keyword()");
        send_secure_keyword(fs.key, email_array, fs.real_name, file_size_string, file_buffer[0], 0l, md.dlt_time_string, md.gmt_time_string, md.description);
        DBG("After send_secure_keyword()");
    } /* End if for sending email */

    /* If this is in the DTMS data repository add the warning message */
    if ((fs.key[8] == 'd') || (fs.key[8] == 'D'))
        strcat(rtn.msg, DTMS_WRN);
    else if ((strcmp(gu.to_company, "DTMS") == 0) && (strcmp(user.client_version, "7.2") < 0))
        strcat(rtn.msg, dtms_upgrade_wrn);

    /* Build the message for the log file on the user's disk */
    len = strcpy6(log_string, fs.key, " #File:  ", fs.real_name, " Directory:  ", user.cwd,  "  Created:  ");

    strcpy3(log_string+len, tr.log_date, "  Expires:  ", md.dlt_time_string);

    /* Send the user the return code */
    /* put_msg used instead of put_success_msg so log string is sent */
    /* Put the disk index in the log file */
    sprintf(bf.buffer, "Disk index %d for %s", fs.disk_index, fs.key);
    data_log(tr.log_file, bf.buffer);
    put_msg(fs.key, log_string);
} /* End put_big_file_ack */


/* Verify at least one E-mail message ends in @nxp.com */
int check4email(char *email_list[])
{
    DBG();

    int cnt;
    int fnd = DBNO;
    int len;

    for (cnt = 0; email_list[cnt] != NULL; ++cnt)
    {
	    len = strlen(email_list[cnt]);

	    /* If address too short, do not check it */
	    if (len < 8) continue;

	    if (strcasecmp(email_list[cnt]+len -8, "@nxp.com") != 0)
	    {
	        fnd = DBYES;
	        put_string("Transaction aborted.");
	        shut_down(external_email_err, EXTERNAL_EMAIL_ERR);
	    } /* End if */
    } /* End for */

    return(fnd);
} /* end check4email */
