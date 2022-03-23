/****************************************************************************
 *
 * Function:	rcv_parallel
 *
 * Description:	Receives a parallel file
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/uio.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include "close_down.h"
#include "edc_defines.h"
#include "external_intercom_prototypes.h"
#include "ftp_info.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"
#include "zlib.h"
#include "trans_server_globals.h"
#include "global_debug.h"
/*
#include "trans_server_prototypes.h"
*/

/* Returns DBYES if parallel type supported */
int machine_supports_parallel(void)
{
    DBG();

    if ((fs.file_type != DIRECTORY)
    && (fs.file_type != FILES)
    && (strncasecmp(gu.machine_type, "Windows", 7) != 0)
    && (strncasecmp(gu.machine_type, "Web browser", 11) != 0)
    && (strncasecmp(gu.machine_type, "AIX", 3) != 0)
    && (strncasecmp(gu.machine_type, "TransGUI", 8) != 0))
    return(DBYES);
    else return(DBNO);
} /* end machine_supports_parallel */


/* Set the parallel type for put transactions */
/* This is used just to decide how to receive the file, it does not set request type */
REQ_TYPE transfer_type_set(void)
{
extern unsigned long long parallel_configured_size;
extern unsigned long long large_parallel_transfer_size;

    int ret = SERIAL;

    /* convert parallel type sent from the client */
    int parallel_type = atoi(gu.par5);
    DBG("parallel_type=%d DBPUT_SEPARATE=%d DBPUT_PARALLEL=%d DBPUT_LARGE_PARALLEL=%d SERIAL=%d", parallel_type, DBPUT_SEPARATE, DBPUT_PARALLEL, DBPUT_LARGE_PARALLEL, SERIAL);
    DBG("gu.original_size=%llu parallel_configured_size=%llu large_parallel_transfer_size=%llu", gu.original_size, parallel_configured_size, large_parallel_transfer_size);

    /* If serial specified or is Windows or AIX do serial transfer */
    if ((gu.original_size < 5000000)
    || (parallel_type == SERIAL)
    || (parallel_type == DBPUT_FILES)
    || (gu.file_type == DIRECTORY)
    || (gu.file_type == FILES)
    || (strncmp(gu.machine_type, "AIX", 3) == 0)
    || (strncmp(gu.machine_type, "TransGUI", 8) == 0)
    || (strncmp(gu.machine_type, "Web browser", 11) == 0)
    || (strncmp(gu.machine_type, "Windows", 7) == 0))
    {
         ret = SERIAL;
    }
#ifndef TRANSWEB
    else if (parallel_type == DBPUT_SEPARATE)
    {
       ret = DBPUT_SEPARATE;
    }
    else
    {
        if (parallel_type == DBPUT_PARALLEL
             || parallel_type == DBPUT_LARGE_PARALLEL
             || (gu.original_size >= parallel_configured_size && is_zipped_file(gu.key1) == DBYES))
        {

            ret = DBPUT_LARGE_PARALLEL;
#if 0 // do not check size
            if (gu.original_size >= large_parallel_transfer_size)
            {
                ret = DBPUT_LARGE_PARALLEL;
            }
            else
            {
                ret = DBPUT_PARALLEL;
            }
#endif
        }
    }
#endif /* for TRANSWEB */
   DBG("ret=%d", ret);
   return ret;
} /* end transfer_type_set */

/* Put Description and e-mail list in global file */
void email_description_build(char *description_buffer, char *description, char *email_array[])
    {
    int cnt;

    DBG();

	char *ptr = description_buffer + strcpy3(description_buffer, "Description: ", description,
    "\nCustomer:    ");
    for (cnt = 0; email_array[cnt] != NULL; ++cnt)
    ptr += strcpy2(ptr, email_array[cnt], " ");
} /* end email_description_append */

/* Initialize metadata for a put ack request */
void metadata_init(char **email_array)
{
    extern int deletion_days;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    extern METADATA md;

    DBG();

    strncpy(fs.title, lines[1], 13);
    strcpy(md.current_time_string, lines[6]+13);
    strcpy(md.current_gmt_time_string, lines[7]+13);
    strcpy(md.dlt_time_string, lines[8]+13);
    strcpy(md.gmt_time_string, lines[9]+13);
#ifdef INTERCOM
    strcpy(md.icap_class, lines[14]+13);
    strcpy(md.description, lines[15]+13);
    /* See if email present to be sent */
    if (lines[16] != NULL)
    {
    /* Convert to array format */
    string2list_convert(lines[16] + 13, email_array);
    }
#else
    strcpy(md.icap_class, lines[13]+13);
    strcpy(md.description, lines[14]+13);
    /* See if email present to be sent */
    if (lines[15] != NULL)
    {
    /* Convert to array format */
    string2list_convert(lines[15] + 13, email_array);
    }
#endif
    /* Otherwise indicate no e-mail to send */
    else email_array[0] = NULL;
    if (deletion_days > 3)
    sprintf(md.expiration_time_string, "%d", deletion_days);
    else strcpy(md.expiration_time_string, "will be deleted in 3 working days.\n");
    fs.lock_flag = 'u';
    /* Take trailing \n off of log date */
    tr.log_date[24] = '\0';
#ifdef TRANS_FTP
    /* Read in the remote company */
    company_file_read();
#endif
} /* end metadata_init */

/* Check last modification time of file */
/* If more than 5 minutes return an error */
/* Currently not being used */
int file_mod_time_check(char *file_name)
{
    time_t current_time;
    struct stat buf;

    DBG();

    if (stat(file_name, &buf) != 0)
    return(FAILURE);
    current_time = time(0);
    if ((current_time - buf.st_mtime) > 1500)
    return(FAILURE);
    else return(SUCCESS);
} /* End file_mod_time_check */


/* Delete data transferred and exit */
void delete_file_and_exit(char *keyword, char *file_name, char *msg, int err_code)
{
     DBG();
    /* Delete the file just transferred */
    unlink(file_name);
    /* Delete the global file */
    /* build name for global file */
    strcpy2(file_name, "global_data/", keyword);
    unlink(file_name);
    /* Release shared memory */
    shared_memory_release();
    /* Send error message */
    email_send("", admin.err_email, "Incorrect size", rtn.msg);
    shut_down(rtn.msg, err_code);
} /* end delete_file_and_exit */

unsigned long long rcv_parallel(
    int parallel_type,
    char *path,
    char *real_name,
    char *file_keyword,
    long long *final_size)
{
#ifdef TRANS_FTP
    extern COMPANY_DATA cp;
#endif
    int mem_key;
    unsigned long long compressed_size = 0;
    struct stat buf;
    int fd;
    int finished;
    int len;
    unsigned long long checksum = 0;
    unsigned long long client_checksum;
    unsigned long long server_size;
    char file_name[PATH_SIZE];

    DBG();
    /* Build name of new file */
    len = strcpy3(file_name, path, file_keyword, ".gz");
    /* If this is DTMS create the directory and concatenate real file name */
    if (strcmp(server.site, DTMS) == 0)
    {
        if (mkdir( file_name, 02770) != 0)
        close_down2d(mkdir_err, errno, file_name, MKDIR_ERR);
        /* Now concatenate real file name */
        len += strcpy2(file_name+len, "/", real_name);
    } /* End if */
    /* Open file for writing */
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0644)) < 0)
    close_down1q(wr_open_err, file_name,  WR_OPEN_ERR);
    /* Create the shared memory */
    mem_key = shared_memory_create(fs.key, 10);
    /* Assign new request type for logging purposes */
    gu.req_type = parallel_type;
    /* Tell client ready to get data */
    put_short(parallel_type);
    /* Send client the mem_key */
    put_long(mem_key);
    close(fd);

    // exit anyway, trans 9.3.4 and icom 7.5 work for this
    // client must open a new connection and send DPUT_ACK

    if (IS_REQUEST_PUT_LARGET_PARALLEL(gu.req_type))
    {
        return_success("Transfer initiated", DBNO);
        put_string("");
        DBG("quiting this process because LARGE PARALLEL");
    }
    else
    {
        stop_server_quietly();
        // common PARALLEL do not send message just exists;
        DBG("quiting server quietly ...");
    }

    exit(0);
    return(compressed_size);
} /* end rcv_parallel */

unsigned long long rcv_separate(
    int parallel_type,
    char *path,
    char *real_name,
    char *file_keyword,
    long long *final_size)
{
    unsigned int block_cnt;
    int mem_key;
    char file_name[PATH_SIZE];
    unsigned long long compressed_size = 0;
    struct stat buf;
    int fd;
    int finished = 0;
    int len;
    unsigned long long checksum = 0;
    unsigned long long client_checksum;
    unsigned long long server_size;
    char buffer[FILE_SIZE];

     DBG();

    /* Build name of new file */
    len = strcpy3(file_name, path, file_keyword, ".gz");
    /* If this is DTMS create the directory and concatenate real file name */
    if (strcmp(server.site, DTMS) == 0)
    {
	if (mkdir( file_name, 02770) != 0)
    close_down2d(mkdir_err, errno, file_name, MKDIR_ERR);
	/* Now concatenate real file name */
	len += strcpy2(file_name+len, "/", real_name);
    } /* End if */
    /* Open file for writing */
    if ((fd = open(file_name, O_WRONLY | O_CREAT, 0644)) < 0)
    close_down1q(wr_open_err, file_name,  WR_OPEN_ERR);
    /* Create the shared memory */
    mem_key = shared_memory_create(fs.key, 1);
    /* Initialize the memory */
    shared_memory1init();
    /* Tell client ready to get data */
    put_short(parallel_type);
    /* Build string with mem key, block size, offset, etc */
    block_cnt = block_size_buffer_build(buffer, mem_key, gu.original_size, gu.par3);
    /* Send client this string */
    put_string(buffer);
    put_short(SUCCESS);
    close(fd);
    if (gu.req_type == DBPUT_LARGE_PARALLEL)
    {
        return_success("Transfer initiated", DBNO);
        put_string("");
        DBG("exiting this process because \"gu.req_type == DBPUT_LARGE_PARALLEL\"");
        exit(0);
    }
    /* Monitor the status file to see if all blocks transferred */
    while (1)
    {
        finished = shared_memory1read(&server_size, &checksum);
        if (finished >= block_cnt)
        {
           DBG("breaking the Monitor \"%d >= %d\" ", finished, block_cnt);
           break;
        }
        sleep(2);
	/* Check file modification time once a minute */
	/* If not updated for 5 minutes, abort transfer */
/*
    if (file_mod_time_check (file_name) == FAILURE)
    {
	sprintf(rtn.msg,  file_update_err, file_name);
    email_send("", admin.err_email, "File update error", rtn.msg);
    shut_down(rtn.msg, FILE_UPDATE_ERR);
    }
*/
    } /* End while */
    /* Get the actual size */
    compressed_size = get_longlong();
    client_checksum = get_longlong();
    *final_size = gu.original_size;
    /* Compare the two file sizes */
    /* Do a stat to compare compressed size with real size */
    if (stat (file_name, &buf) != 0)
        shut_down("stat err", -1);
    DBG("file_name=%s compressed_size=%llu buf.st_size=%lld", file_name, compressed_size, buf.st_size);
    if (buf.st_size != compressed_size)
    {
        DBG("bad_size");
        sprintf(rtn.msg, bad_size, PROGRAM, real_name, file_keyword, compressed_size, buf.st_size);
        delete_file_and_exit(file_keyword, file_name, rtn.msg, BAD_SIZE);
    } /* End if */
    if (server_size != *final_size)
    {
        DBG("bad_size");        
        sprintf(rtn.msg, bad_size, PROGRAM, real_name, file_keyword, *final_size, server_size);
        delete_file_and_exit(file_keyword, file_name, rtn.msg, BAD_SIZE);
    } /* End if */
    /* Verify that the check sum is correct */
    if (checksum != client_checksum)
    {
        DBG("bad_size");
        sprintf(rtn.msg, bad_checksum, PROGRAM, client_checksum, checksum);
        delete_file_and_exit(file_keyword, file_name, rtn.msg, BAD_CHECKSUM);
    } /* End if */
    /* Remove shared memory no longer needed */
    shared_memory_release();

    return(compressed_size);
} /* end rcv_separate_parallel */


/* Get the ack and complete the transfer */
void put_ack(char *file_keyword, int mem_key)
{
    extern METADATA md;
    extern int deletion_days;
    extern  TRANS_FILE_STAT fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char *email_array[100];
    unsigned long long final_size = 0;
    unsigned long seconds;
    int finished;
    unsigned long long checksum = 0;
    unsigned long long client_checksum;
    long long server_size;
    struct stat buf;
    char file_name[PATH_SIZE];
    char global_file[64];
    char dlt_string[16];
    int edc_auth = APPROVED;
    FILE *fp;

    DBG();

    /* Read in the file data */
    fp = read_big_file(file_keyword, lines, &fs);
    fclose(fp);
    DBG("after read_big_file()");

    /* Must allocate the memory before it can be read */
    DBG("BEFORE call shared_memory_allocate(mem_key, 10);");
    shared_memory_allocate(file_keyword, mem_key, 10);
    DBG("AFTER call shared_memory_allocate(mem_key, 10);");

    DBG("BEFORE `finished = shared_memory_read(&server_size, &checksum)`");
	finished = shared_memory_read(&server_size, &checksum);
    DBG("AFTER `finished = shared_memory_read(&server_size, &checksum)`");

    /* Remove shared memory no longer needed */
    shared_memory_release();

    /* Tell client ready to retrieve size and checksum */
    put_short(SUCCESS);

    /* Get the actual size */
     fs.compressed_size = get_longlong();
    client_checksum = get_longlong();

    /* Get the number of seconds for transaction */
    seconds = get_long();

    /* Set the time back to the original starttime */
    tr.upd_time = time(0)-seconds;
    final_size = fs.original_size;

    /* Compare the two file sizes */
    /* build the file name */
    if (strcmp(server.site, "DTMS") == 0)
        sprintf(file_name, "data_files%d/%s.gz/%s", fs.disk_index, file_keyword, fs.real_name);
    else
        sprintf(file_name, "data_files%d/%s.gz", fs.disk_index, file_keyword);
    /* Do a stat to compare compressed size with real size */

    DBG("file_name=%s", file_name);

    if (stat (file_name, &buf) != 0)
    {
       DBG("exiting");
       shut_down(stat_err, -1);
    }

    /* If client sent extra data, delete it */
    if (buf.st_size > fs.compressed_size)
    {
        truncate(file_name, fs.compressed_size);
    }
    else if (buf.st_size < fs.compressed_size)
    {
        sprintf(rtn.msg, bad_size, PROGRAM, fs.real_name, fs.key, fs.compressed_size, buf.st_size);
        delete_file_and_exit(file_keyword, file_name, rtn.msg, BAD_SIZE);
    } /* End if */

    if (server_size != final_size)
    {
        sprintf(rtn.msg, bad_size, PROGRAM, fs.real_name, fs.key, final_size, server_size);
        delete_file_and_exit(file_keyword, file_name, rtn.msg, BAD_SIZE);
    } /* End if */

    /* Verify that the check sum is correct */
    if (checksum != client_checksum)
    {
        sprintf(rtn.msg, bad_checksum, PROGRAM, client_checksum, checksum);
        delete_file_and_exit(file_keyword, file_name, rtn.msg, BAD_CHECKSUM);
    } /* End if */

    /* Calculate the delete days */
    /* Use value for deletion_days set in global_params file */
    if (deletion_days > 3) sprintf(dlt_string, "%d", deletion_days);
    else strcpy(dlt_string, "3 working");

    /* Initialize the metadata fields */
    metadata_init(email_array);

    /* Do final update to the file */
    /* and send the return code to the client */
    /* Build the global file name */
    strcpy2(global_file, GLOBAL_FILE_PATH, file_keyword);

#ifdef TRANSCEND
    put_trans_file_ack(global_file,   email_array);
#endif
#if defined(INTERCOM) || defined(TRANS_FTP)
    put_big_file_ack(global_file, file_keyword,   email_array, NULL, NONE, edc_auth);
#endif

} /* end put_ack */

/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
