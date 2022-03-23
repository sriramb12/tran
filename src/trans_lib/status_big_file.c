/****************************************************************************
 *
 * Function:	mod_email_big_file
 *
 * Description:	Modifies the Email status of a big file.
 *		Returns string containing  the file status
 *		Byte 0 is the global email flag
 *		Byte 1 is the file email flag
 *		Byte 2 is the retrieval flag
 *		Byte 3 is the delete flag
 *		Byte 4 is the owner flag
 *		Byte 5 is the extension flag
 *
 ***************************************************************************/

#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_server_paths.h"
#include "trans_defines.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "rtn.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

/* Defines for elements in status array */
typedef enum
{
    GLOBAL_EMAIL_SET,
    FILE_EMAIL_SET,
    RETRIEVAL_LOCK_SET,
    DELETE_SET,
    DAYS_EXTENDED,
    OWN_DATA,
    TRANSFER_COMPLETE,
    END_STATUS,
} FILE_STATUS;


int is_owner(char *own_id, char **name_email)
{
    char name[32];
    char *ptr;

    DBG();

    *name_email += strcpy4(*name_email, name, "(", own_id, ")\n");

    return(keyword_auth_check(own_id));
} /* End is_owner */


void status_big_file(char *keyword)
{
    int cnt;
    extern TRANS_FILE_STAT fs;
    FILE *fp;
    char *msg = rtn.msg;
    char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char *list[END_STATUS+1];


    DBG();

    /* Read in data from global file */
    fp = read_big_file(keyword, lines, &fs);
    fclose(fp);
    /* Allocate space for array */
    for (cnt = 0; cnt < END_STATUS; ++cnt)
    {
        list[cnt] = malloc(2);
        list[cnt][0] = '\0';
        list[cnt][1] = '\0';
    }
    list[END_STATUS] = NULL;
    /* build string to send to user */
    list[GLOBAL_EMAIL_SET][0] =   search_email_file(fs.own_id, GLOBAL_EMAIL_FILE)+48;
    list[FILE_EMAIL_SET][0]   =   fs.email_flag+48;
    if (fs.lock_flag == 'L')
    {
        list[RETRIEVAL_LOCK_SET][0] = DBYES+48;
    }
    else
    {
        list[RETRIEVAL_LOCK_SET][0] = DBNO+48;
    }
    if (fs.lock_flag == 'l')
    {
        list[TRANSFER_COMPLETE][0] = DBNO+48;
    }
    else
    {
        list[TRANSFER_COMPLETE][0] = DBYES+48;
    }
    list[DELETE_SET][0] =  fs.dlt_flag+48;
    // May 09 2018, Carlos Mazieri,  fixed a buffer overflow here, because list[SIZE] must be list[SIZE+1]
    msg = rtn.msg;
    if (is_owner(fs.own_id, &msg) == 0)
    {
        list[OWN_DATA][0] = DBYES+48;
    }
    else
    {
        list[OWN_DATA][0] = DBNO+48;
    }
    list[DAYS_EXTENDED][0] = fs.extension+48;
    /* Call function to send the list */
    put_short(7);
    list_get(list);
    strcpy(rtn.msg, "File status retrieved.");
    put_success_msg(keyword);
} /* End status_big_file */

/* Get the file name */
void filename_rtv(char *key)
{
    FILE *fp;
    extern TRANS_FILE_STAT fs;
    char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char tmp_key[10];

    DBG();

    strcpy(tmp_key, key);
    /* If the key is found, it is returned in variable 'key' */
    /* Returns status indicating if key sent in is a secure keyword */
    /* Looks up the file keyword if this is a secure keyword */
    /* Read meta data into line buffer */
    fp = read_big_file(key, lines, &fs);
    fclose(fp);
    /* Put file name in return message */
    strcpy(rtn.msg, fs.real_name);
    /* Indicate data will be sent */
    put_short(SUCCESS);
    /* Send the return code */
    put_success_msg(key);
} /* end filename_rtv */

void lock_status_check(void)
{
    extern TRANS_FILE_STAT fs;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];

    DBG();

    /* If this file is locked for data transfer, give error and exit */
    if (fs.lock_flag == 'l')
    {
	long long total_size;
	char *msg = rtn.msg + sprintf(rtn.msg, trans_err, fs.real_name);
	/* Pass in the total size */
	sscanf(lines[2], "%*s %*s %lld", &total_size);
	transfer_status_big_file(fs.key, msg, total_size);
	shut_down(rtn.msg, TRANS_ERR);
    }
    /* If this file is locked for zip give error */
    else if (fs.lock_flag == 'z')
    close_down1f(zip_lock_err, fs.real_name, ZIP_LOCK_ERR);
    /* If this file is locked for user retrieval, give error and exit */
    else if (fs.lock_flag == 'L')
    {
	sprintf(rtn.msg, block_err, fs.real_name, lines[5]+13, fs.own_id);
	shut_down(rtn.msg, BLOCK_ERR);
    }
} /* end lock_status_check */

/* Adds transfer status and last update time to message */
void transfer_status_big_file(char *key, char *msg, unsigned long long total_size)
{
    int ret;
    int len;
    struct stat buf;

    DBG();

    /* Build real file name */
    len = strcpy3(bf.file_name, disk.path[fs.disk_index], key, ".gz");
    /* Add extra file name if in DTMS data repository */
    if (strcmp(server.site, "DTMS") == 0)
    strcpy2(bf.file_name+len, "/", fs.real_name);
    /* Get file status */
    ret = stat(bf.file_name, &buf);
    if (ret == 0)
    {
	/* Add file status to end of return message */
msg += sprintf(msg, "Status:	  %llu bytes transferred\n",
	buf.st_size);
	/* show the total size of the file */
	msg+= sprintf(msg, "Total size:  %llu bytes\n", total_size);
	/* Also show the last time the file was updated */
	/* This is helpful if transfer did not complete */
	/* Allows user to see last time data was written to the repository */

#if 1  // Carlos Mazieri: looks like an error here, it does matter SOLARIS or LINUX there is no global time_buffer
      sprintf(msg, "Last update: %s", ctime(&buf.st_mtime));
#else
#ifdef SOLARIS
	sprintf(msg, "Last update: %s", ctime(&buf.st_mtime));
#endif
#ifdef LINUX
	sprintf(msg, "Last update: %s", ctime_r(&buf.st_mtime, time_buffer));
#endif
#endif
    } /* end if */
} /* End transfer_status_big_file */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
