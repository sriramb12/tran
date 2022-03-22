#ifndef TRANS_FTP
/****************************************************************************
 *
 * Function:	trans1step_upload
 *
 * Description:	Moves file from another directory into Transcend repository
 *
 ***************************************************************************/

#include "errno.h"
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
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_arrays.h"
#include "trans_lib_prototypes.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"


void trans1step_upload(char *client_file_name)
{
    extern METADATA md;
    extern char keyword_link_extension[];
    struct stat buf;
    char file_size_string[32];
    char global_file[32];
    char data_file[32];
    char history_file[64];
    FILE *fp;
    int email_num ;
    int len;
    char *email_array[1000];
    char file_buffer[1024];
    char log_string[512];
    char email[BUFFER_SIZE];
    char *file_ptr;
    char *root_file_ptr;

    DBG("client_file_name='%s'", client_file_name);

    /* Get file status */
    if (stat(client_file_name, &buf) != 0)
        close_down1f(stat_err, client_file_name, STAT_ERR);

    /* Get index for disk to use */
    int size = buf.st_size;
    /* fs.disk_index = check_disk_usage(size); */
    fs.disk_index = get_random_index();

    /* Build keyword for the data */
    keyword_create(fs.key, tr.user_site, 7);

    /* Tell client successful up to this point */
    put_short(SUCCESS);

    /* Send file keyword to user */
    strcpy2(rtn.msg, "File keyword is ", fs.key);
    put_string(rtn.msg);

    if (strcmp(gu.par1, "0") != 0)
    {
	    get_string(md.description);

        /* Get string that contains email address data will be sent to */
	    get_string(email);

        /* Convert commas to spaces */
	    strexc(email, ',', ' ');

        /* Convert semicolons to spaces */
	    strexc(email, ';', ' ');

        /* convert email addresses to list format */
	    email_num = string2list_convert(email, email_array);

	    /* Save e-mail and description for later use */
        strcpy4(file_buffer, "Description: ", md.description, "\nCustomer:    ", email);

	    /* Do not allow deletion after the first retrieval if more than one e-mail address specified */
	    if (email_num > 1)
	        fs.dlt_flag = DBNO;

    } else email_array[0] = NULL;

    /* build name for file */
    sprintf(data_file, "data_files%d/%s.gz", fs.disk_index, fs.key);

    /* Move the file to its new location */
    file_rename(gu.key1, data_file);

    /* Calculate the file name */
    root_file_ptr = root_find(gu.key1, '/');

    /* Calculate the deletion time */
    fs.dlt_time = trans_dlt_time_calc(md.current_gmt_time_string, md.dlt_time_string,
    md.gmt_time_string, md.expiration_time_string);

    /* Save current time for later use */
    strcpy(md.current_time_string, tr.log_date);

    /* Set the delete after get flag */
    fs.dlt_flag = DBNO;
    fs.email_flag = DBNO;

    /* Set the file type */
    gu.file_type = UNSPECIFIED;

    /* Set the file type string */
    strcpy(fs.title, "File:        ");

    /* Copy in the file name */
    /* Get the file size */
    num_bytes_calc(file_size_string, size);
    fs.compression = (int) gu.compressed_size;

    /* Set the iCAP classification */
    icap_class_set(md.icap_class);

    sprintf(file_buffer, "%c %d %ld %s %d 0 %d u %ld %d %s\n%s%s\nUser size:   %s\nCompressed:  %s\nKeyword:     ",
        gu.file_type, fs.disk_index, fs.dlt_time, user.email, fs.email_flag, fs.dlt_flag, 
        buf.st_mode, fs.compression, gu.par2, fs.title, root_file_ptr, file_size_string, file_size_string
    );

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
/*
    if (file_buffer[0] != '\0') data_log(global_file, file_buffer);
*/
    /* Build name for history file */
    strcpy3(history_file, HISTORY_FILE_PATH, fs.key, ".history");

    /* Open the history file */
    if ((fp = fopen(history_file, "w")) == NULL)
        close_down1m(wr_open_err, history_file, WR_OPEN_ERR);

    /* Write the deletion time */
    /* It is 14 days after the current expiration date */
    /* If the file does not expire, there is no expiration time */
    if (fs.dlt_time != 0) fs.dlt_time +=1209600;
        fprintf(fp, "%ld\n", fs.dlt_time);

    /* close the history file */
    fclose(fp);

    /* Initialize size for return message */
    str2size(file_size_string, size);

    /* Initialize return message sent to user */
    rtn.len = strcpy12(rtn.msg,
        "File '", root_file_ptr, "' (size ", file_size_string,  ") transcended.\n",
        "Retrieve the file with the keyword:  ", fs.key,
        "\nTransWeb URL: https://nww.transweb.nxp.com/index", keyword_link_extension, "?go=KEYWORD&KEYWORD=",
        fs.key, "\nThis file ");

    /* Add more things to message to send to user */
    rtn.len += strcpy6(rtn.msg+rtn.len,  md.expiration_time_string,
        "Local Deletion Time:  ",
        md.dlt_time_string, "\nGreenwich Mean Time:  ",
        md.gmt_time_string, "\n\n");

    if (email_array[0] != NULL)
    {
        send_secure_keyword(fs.key, email_array, root_file_ptr, file_size_string, file_buffer[0], 0l, md.dlt_time_string, md.gmt_time_string, md.description);
    } /* End if for sending email */

    /* If this is in the DTMS data repository add the warning message */
    if ((fs.key[8] == 'd') || (fs.key[8] == 'D'))
        strcat(rtn.msg, DTMS_WRN);

    len = strcpy6(log_string, fs.key, " #File:  ", client_file_name, " Directory:  ", user.cwd,  "  Created:  ");
    strcpy3(log_string+len, tr.log_date, "  Expires:  ", md.dlt_time_string);

    /* Send the user the return code */
    /* Put the disk index in the log file */
    sprintf(bf.buffer, "Disk index %d for %s", fs.disk_index, fs.key);
    data_log(tr.log_file, bf.buffer);
    put_short(SUCCESS);
    put_msg(fs.key, log_string);
} /* End trans1step_upload */


/* zip file after uploaded using 1 step upload */
void trans1step_zip(char *keyword)
{
    extern TRANS_FILE_STAT  fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char command[40];
    char compressed_file[32];
    char file_size_string[32];
    int ret;
    FILE *fp;
    struct stat buf;

    DBG();

    /* Open the global file to get needed info */
    fp = read_big_file(keyword, lines, &fs);

    /* Verify file is not already zipped */
    lock_status_check();

    /* If file already zipped, give error */
    if ((is_zipped_file(fs.real_name) == DBYES) || (fs.original_size != fs.compressed_size))
    {
	    sprintf(rtn.msg, already_zipped_err, "File", fs.real_name);
	    shut_down(rtn.msg, ALREADY_ZIPPED_ERR);
    }

    /* Send message to client indicating file will be zipped */
    put_short(SUCCESS);

    strcpy3(rtn.msg, "File '", fs.real_name, "' being zipped.");

    put_success_msg(keyword);

    /* Lock this file while it is being zipped */
    fs.lock_flag = 'z';

    DBG("Before call write_big_file()");
    write_big_file(fp, lines, &fs);
    DBG("after call write_big_file()");

    /* build command to zip the file */
    /* First move keyword to temporary location */
    sprintf(command, "mv data_files%d/%s.gz data_files%d/%s ; gzip data_files%d/%s", fs.disk_index, keyword, fs.disk_index, keyword, fs.disk_index, keyword);
    data_log(tr.log_file, command);

    /* Run zip command */
    system(command);

    /* Get the size of the zipped file */
    sprintf(compressed_file, "data_files%d/%s.gz", fs.disk_index, keyword);

    if (stat(compressed_file, &buf) != 0)
        close_down1f(stat_err, compressed_file, STAT_ERR);

    /* Calculate size of compressed file */
    int size = buf.st_size;
    num_bytes_calc(file_size_string, size);

    /* Unlock the file and exit */
    /* Open the global file again */
    fp = read_big_file(keyword, lines, &fs);
    fs.lock_flag = 'u';

    /* Overwrite the compressed size with the new compressed size */
    strcpy2(lines[3], "Compressed:  ", file_size_string);

    /* Write out the changes to the file */
    DBG("Before call write_big_file()");
    write_big_file(fp, lines, &fs);
    DBG("after call write_big_file()");

    exit(0);
} /* end trans1step_zip */
#endif
