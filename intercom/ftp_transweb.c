#ifdef TRANS_FTP
/****************************************************************************
 *
 * Function:	ftp_put_transweb
 *
 * Description:	Sends the Transcend/InterCom server a big file or tar file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "client_maskshop.h"
#include "client_disk_usage.h"
#include "ftp_info.h"
#include "global_defines.h"
#include "global_params.h"
#include "hash.h"
#include "list.h"
#include "server_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "user_params.h"
#include "trans_prototypes.h"
#include "client_lib.h"
#include "zconf.h"
#include "zlib.h"
#include "global_debug.h"

/* See if this is an FSO company */
int is_fso(void)
{
    extern COMPANY_DATA cp;
    FILE *fp;
    char line[64];
    int len = strlen(cp.company);

    DBG();

    /* Set default value */
    cp.is_fso = DBNO;
    /* Open FSO file for reading */
    if ((fp = fopen(FSO_FILE, "r")) == NULL)
    return(cp.is_fso);
    /* Read through the file comparing names with the current company */
    while (line_get(fp, line) != EOF)
    {
	if (strncmp(line, cp.company, len) == 0)
	{
	cp.is_fso = DBYES;
		break;
	} /* end if for company found */
    } /* End while */
    fclose(fp);
    return(cp.is_fso);
} /* end is_fso */

/* Sends encrypted data to external company using TransWeb */
int ftp_put_transweb(
    char *tmp_file_name,
    char *real_file_name,
    long long size,
    char send_email,
    char *email_string,
    char *description,
    char *log_string)
{
    int tmp_sock;
    int len;
    char buffer[BLOCK_SIZE];
    char keyword[12];
    long long final_size = 0;
    FILE *fp;
    short ret;

    DBG();

    /* Store socket value for later use */
    tmp_sock = sock;
	 /* Connect to the database server */
	if ((ret = init_ftp_transweb_socket(DBPUT_DATA, real_file_name, "External TransWeb Customer")) != SUCCESS)
    return(ret);
    send_email = DBYES;
    if (send_email == DBYES)
    {
	put_string(description);
    /* Send email in string format */
    put_string(email_string);
    } /* End if */
    /* Now send the file data itself */
    /* Get the file keyword */
    get_string(rtn.msg);
     /* Get indication that server ready to receive data */
    if ((ret = get_short()) < SUCCESS)
    {
    ret = get_short();
	get_string(rtn.msg);
	close(sock);
	return(ret);
    }
    /* Scan keyword into variable for Transcend/InterCom library */
    sscanf(rtn.msg, "%*s %*s %*s %s", keyword);
  fp = fopen(tmp_file_name, "r");
    if (fp == NULL)
    {
	sprintf(rtn.msg, "Error -381:    Unable to open for read file:\n\n               %s", real_file_name);
	return(-1);
    }
    /* Now send the file data itself */
    /* Write out data for as long as can read it in */
    while ( 0 != (len = fread(buffer, 1, BLOCK_SIZE, fp)) )
    {
    	put_binary_string(buffer, len);
	final_size += len;
    } /* End while */
    /* Tell server all data sent */
    if (put_short(0) != SUCCESS)
    {
	sprintf(rtn.msg, bad_put, errno, HELP_MSG);
	close(sock);
	return(BAD_PUT);
    }
    /* Close local file */
    fclose(fp);
    put_longlong(final_size);
    /* Get the return code */
    /* Also get the string to put in the log */
    if ((ret = get_short()) == SUCCESS)
    {
	rtn.len = strcpy1(rtn.msg, "Data was encrypted/compressed before transfer to the TransWeb repository.\n");
	rtn.len += strcpy1(rtn.msg+rtn.len, "Thus, the size displayed below is the size of the encrypted compressed file.\n\n");
    } /* end if */
    else rtn.len = 0;
    ret = get_string(rtn.msg+rtn.len);
    close(sock);
    sock = tmp_sock;
    put_msg(keyword, log_string);
    return(ret);
} /* end ftp_put_transweb */
#endif
