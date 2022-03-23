#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	transsimple_request and supporting functions
 *
 * Description:	Functions to perform simple requests
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "company.h"
#include "ssl_client_lib.h"
#include "global_defines.h"
#include "global_params.h"
#include "rtn.h"
#include "strcpy.h"
#include "server_params.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"

#include "global_debug.h" // Mazieri's debug facilities

extern int sock;

/* Set the request type and keyword for a simple keyword request */
/* This function is hidden from the API */
int trans_simple_keyword_request_set(int index, REQ_TYPE req_type, char *keyword)
{
    DBG();
    int ret;

    /* Verify the keyword is valid */
    if ((ret = trans_is_keyword(keyword )) != SUCCESS)
    return(ret);
    /* Set the transaction type */
    trans[index]->transaction_type = KEYWORD_REQUEST;
    /* Set the action */
    trans[index]->req_type = req_type;
    /* Set the keyword */
    strcpy(trans[index]->keyword, keyword);
    /* If this is an extend request, set default value to 3 days */
    if (trans[index]->req_type == DBEXTEND_DATA)
    trans[index]->num_days = 3;
    /* find repository where keyword is stored */
    trans_repository_find(keyword[8]);
    return(SUCCESS);
} /* end trans_simple_keyword_request_set */

/* Set the request type and key for a simple request */
/* These transactions are for requests which do not use a keyword */
/* This function is hidden from the API */
int trans_simple_basic_request_set(int index, REQ_TYPE req_type, char *key)
{
    DBG();
    char int_pwd[128];
    int lenkey = strlen(key);
    int is_key_encrypted = 0;
    // Carlos Mazieri, check if the password is already encrypted
    if (lenkey > 2 && isdigit(key[0]) && key[lenkey-1] == '.' && isdigit(key[lenkey-2]))
    {
        is_key_encrypted = 1;
        lenkey -= 2;
        while (lenkey-- > 1)
        {
            if (key[lenkey] != '.' && isdigit(key[lenkey]) == 0)
            {
                is_key_encrypted = 0;
                break;
            }
        }
    }

    /* Set the transaction type */
    trans[index]->transaction_type = SIMPLE_REQUEST;
    /* Set the action */
    trans[index]->req_type = req_type;
    /* If this is an authentication request, encrypt it */
    if (req_type == DBX500AUTHEN && is_key_encrypted == 0)
    {
        encrypt_ascii_pwd(int_pwd, key);
        strcpy(trans[index]->keyword, int_pwd);
    } /* end if */
    else strcpy(trans[index]->keyword, key);
    return(SUCCESS);
} /* end trans_simple_basic_request_set */

/* Send simple request to the server */
/* This consists of sending a header and retrieving a response from the server */
int trans_simple_request(int index)
{
    DBG();

    short ret;

    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS)
    return(ret);
    /* Get the return code */
    return(trans_ret_code_get());
} /* end trans_simple_request */

/* Scan in remote directory for data sent to the foundry */
void foundry_info_get(int index)
{
    DBG();
    char tmp_dir[128];
    int len;

    PRINTF(FTP_TRANSFER_MSG, remote_company_email, remote_company_email);
        len = arg_scan(tmp_dir);
    /* If len greater than 1 know found a directory */
    /* Otherwise use the default of '0' which is one character in length */
    if (len > 1) strcpy(trans[index]->par1, tmp_dir);
    else len = 1;
} /* End foundry_info_get */

/* Retrieve a simple file such as release, list of keywords, etc */
int   trans_simple_file_get(int index, REQ_TYPE req_type, char *keyword)
{
    DBG();
     char c_string[8];
    char *file_ptr;
    short ret;

    /* Might need to set the commpany here */
    /* Set the request type */
    trans_simple_basic_request_set(index, req_type, keyword);
    /* Connect to the Transcend database */
    if ((ret = trans_socket_init(index)) < SUCCESS)
    return(ret);
   /* Put the path in the file name */
    file_ptr = trans[index]->file_name + strcpy2(trans[index]->file_name, user.cwd, "/");
    /* Get the file name */
    ret = get_ssl_string(file_ptr);
    /* At this point return mesage is initialized */
    if (ret < 0)
	return(ret);
    /* If file present in this directory, warn them it will be overwritten */
    if (access(trans[index]->file_name, 0) == 0) 
    {
	/* If overwrite_files set to yes, overwrite the file */
	if (trans[index]->overwrite_files == DBYES)
	unlink(trans[index]->file_name);
	else
    {
	printf("File '%s' already exists.  Overwrite this file, y/n?  ", file_ptr);
	scanf("%s", c_string);
	printf("\n");
	if (( c_string[0] == 'y') || (c_string[0] == 'Y')) 
	{
	    /* Delete the file and indicate files have been overwritten */
	    ret = unlink(trans[index]->file_name);
	} /* End if */
	else
	{
	    /* Tell server unable to create file */
	    put_ssl_short(BAD_CREATE);
	ret = get_ssl_short();
	get_ssl_string(rtn.msg);
	    close(sock);
	    return(ret);
	    } /* End else for deleting file */
	} /* end else for overwrite files set to no */
    } /* end if for file present */
    /* specify file permission */
    trans[index]->file_mode = 0700;
    /* Get the file */
    return(custom_file_get(index));
} /* end trans_simple_file_get */


/* Get number of days to extend file from user */
int trans_days_set(int argc, char **argv)
{
    DBG();
    int days;

    if (argc < 4)
    {
	    printf("You may extend the expiration date up to 3 days. \n");
	    printf("Enter the number of days to extend the expiration.  ");
	    scanf("%d", &days);
	    printf("\n");
    } /* End if for days not entered on command line */
    else days = atoi(argv[3]);

    if ((days < 1) || (days > 3))
    {
	    printf("You cannot extend the expiration date more than three days.\n");
	    printf("Enter 1, 2, or 3 to extend the expiration date.  Enter\n");
	    printf("anything else to exit %s.\n> ", so.program);
	    scanf("%d", &days);
	    printf("\n");
	
	    if ((days < 1) || (days > 3))
	    {
	        return(-1);
	    } /* End if for exiting Transcend */

    } /* End if for days greater than 3 */

    return(days);
} /* end trans_days_set */


/* Verify this is a valid keyword */
int trans_is_keyword (char *keyword)
{
    DBG();

    /* Return success for "all" and NONE */
    if ((strcasecmp(keyword, "all") == 0)
    || (strcmp(keyword, NONE) == 0))
    return(SUCCESS);
    if (strlen(keyword) != 9)
    {
	sprintf(rtn.msg, invalid_trans_keyword, keyword);
	return(INVALID_TRANS_KEYWORD);
    }
if ((keyword[8] < 'A') || (keyword[8] > 'z')
|| (keyword[0] < 'a') || (keyword[0] > 'z')
    || (keyword[1] < 'a') || (keyword[1] > 'z')
    || (keyword[2] < 'a') || (keyword[2] > 'z')
    || (keyword[3] < 'a') || (keyword[3] > 'z')
    || (keyword[4] < '0') || (keyword[4] > '9')
    || (keyword[5] < '0') || (keyword[5] > '9')
    || (keyword[6] < '0') || (keyword[6] > '9')
    || (keyword[7] < '0') || (keyword[7] > '9'))
    {
	sprintf(rtn.msg,invalid_trans_keyword, keyword);
	strcat(rtn.msg, "\n\n");
	return(INVALID_TRANS_KEYWORD);
    } /* End if */
    else return(SUCCESS);
} /* end trans_is_keyword */

/* Set the request type and  keyword for a keyword request */
int trans_keyword_request_set(int index, REQ_TYPE req_type, char *keyword)
{
    DBG();
    int ret;

    switch(req_type)
    {
	case DBHIST_DATA: ret = trans_custom_keyword_request_set(index, req_type, keyword); break;
	/* Send secure keyword to one or more customers */
	case DBSEND_KEYWORD: ret = trans_custom_keyword_request_set(index, req_type, keyword); break;
	case TRANSDIRLIST_RTV: ret = trans_custom_keyword_request_set(index, req_type, keyword); break;
	case TRANSDIRLIST_FILE_RTV: ret = trans_custom_keyword_request_set(index, req_type, keyword); break;
	case DB1STEP_DOWNLOAD: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DB1STEP_DOWNLOAD_COMPLETE: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DB1STEP_ZIP: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBSIZE_CHECK: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBLOCK_DATA: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBUNLOCK_DATA: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBEXTEND_DATA: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBSET_FILE_EMAIL: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBCAN_FILE_EMAIL: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBDLT_DATA: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBOWNER_DLT: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBFTP_RESEND: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBUSER_DLT: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBINFO_DATA: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBFILENAME_RTV: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	case DBSTATUS_DATA: ret = trans_simple_keyword_request_set(index, req_type, keyword); break;
	default:  
	{
	    strcpy(rtn.msg, invalid_request_type);
	    return(INVALID_REQUEST_TYPE);
	}
	break;
    } /* end switch */
    return(ret);
} /* end trans_keyword_request_set */
#endif
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
