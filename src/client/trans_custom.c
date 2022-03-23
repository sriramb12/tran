#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	trans_custom_request and supporting functions
 *
 * Description:	Functions to perform custom requests
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
#include "rtn.h"
#include "strcpy.h"
#include "server_params.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "transrc.h"

#include "global_debug.h" // Mazieri's debug facilities

extern int sock;
extern TRANS_TOOL tool;

int trans1step_upload(int index);

/* Set the request type and keyword for a custom request */
/* This function is used by the API */
int trans_custom_request_set(int index, REQ_TYPE req_type, char *keyword)
{
    DBG();
    int ret;

    switch(req_type)
    {
	/* Send secure keyword to one or more customers */
	case DB1STEP_UPLOAD: ret = trans_custom_basic_request_set(index, req_type, keyword); break;
	case DBFAST_QUERY_USER: ret = trans_custom_basic_request_set(index, req_type, keyword); break;
	case DBQUERY_USER: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBGET_RELEASE: ret = trans_custom_basic_request_set(index, req_type, keyword); break;
	    case SHARED_MEMORY_CLEANUP: ret = trans_custom_basic_request_set(index, req_type, keyword); break;
	case DBAUTO_REG: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBUSR_KEYS: ret = trans_custom_basic_request_set(index, req_type, keyword); break;
	/* Retrieve list of sites */
	case DBSITE_RTV: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	/* Retrieve list of repositories */
	case DBREPOSITORY_RTV: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case GUI_COMPANY_TABLE_RTV: ret = trans_custom_basic_request_set(index, req_type, keyword); break;
	case DBSET_USER_EMAIL: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBCAN_USER_EMAIL: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBX500LOOKUP: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBX500AUTHEN: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBSITE_CHECK: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBCRON_DLT: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBREG_URL: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBREG_QUERY: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBPING: ret = trans_simple_basic_request_set(index, req_type, keyword); break;
	case DBICAP_MSG: ret = trans_custom_basic_request_set(index, req_type, keyword); break;
	default:
	{
	    strcpy(rtn.msg, invalid_request_type);
        DBG("INVALID_REQUEST_TYPE");
        return(INVALID_REQUEST_TYPE);
	}
	break;
    } /* end switch */
    DBG("SUCCESS");
    return(SUCCESS);
} /* end trans_custom_request_set */

/* Set the request type and keyword for a custom keyword request */
/* This function is used by the Transcend client and is a hidden function that the API does not see */
int trans_custom_keyword_request_set(int index, REQ_TYPE req_type, char *keyword)
{
    DBG();
    int ret;

    /* Verify the keyword is valid */
    if ((ret = trans_is_keyword(keyword )) != SUCCESS) {
        DBG("Keyword is not valid");
        return(ret);
    }
    /* Set the transaction type */
    trans[index]->transaction_type = CUSTOM_REQUEST;

    /* Set the action */
    trans[index]->req_type = req_type;

    /* Set the keyword */
    strcpy(trans[index]->keyword, keyword);

    /* Set the repository for this keyword */
    trans_repository_find(keyword[8]);

    DBG("SUCCESS");
    return(SUCCESS);
} /* end trans_custom_keyword_request_set */

/* Set the request type and keyword for a basic custom request */
/* This function is used by the Transcend client and is a hidden function that the API does not see */
int trans_custom_basic_request_set(int index, REQ_TYPE req_type, char *keyword)
{
    DBG();

    /* Set the transaction type */
    trans[index]->transaction_type = CUSTOM_REQUEST;
    /* Set the action */
    trans[index]->req_type = req_type;
    /* Set the keyword */
    if (keyword != NULL)
    strcpy(trans[index]->keyword, keyword);
    DBG("SUCCESS");
    return(SUCCESS);
} /* end trans_custom_basic_request_set */

/* Retrieve company list used by the Intercom GUI */
int gui_company_table_rtv(int index)
{
    DBG();

    char buffer[BUFFER_SIZE];
    int ret;

    if (trans_socket_init(index) < SUCCESS) return(ret);

    /* Now retrieve company table and write to standard out */
    while (get_ssl_string(buffer) > 0)
        PRINTF("%s\n", buffer);

    /* Get the return code */
    DBG("will call trans_ret_code_get()");
    return( trans_ret_code_get() );
} /* End trans_gui_company_table_rtv */

/* Used to delete both secure and user keys from one or all repositories */
int trans_keywords_delete(int index, int req_type, char *site)
{
    DBG();
    char msg[BUFFER_SIZE];
    char *msg_ptr = msg;
    int cnt;

    /* Set the request type */
    trans[index]->req_type = req_type;
    if (strncmp(site, "All", 3) != 0)
    {
        trans_repository_scan(site);
        DBG("calling trans_request_send(trans[index]->req_type, index)");
        return(trans_request_send(trans[index]->req_type, index));
    } 
    else if ((tool.type == INTERNAL_INTERCOM)
    || (tool.type == EXTERNAL_INTERCOM))
    {
	trans_repository_set(tool.repository_index[0]);
	trans_request_send(trans[index]->req_type, index);
    }
    else
    {
        /* Do the following for all of the sites */
        for (cnt = tool.repository_index[0]; cnt <= tool.repository_index[1]; ++cnt)
        {
            trans_repository_set(cnt);
            trans_request_send(trans[index]->req_type, index);
            msg_ptr += strcpy2(msg_ptr, rtn.msg, "\n\n");
        } /* End for */
        strcpy(rtn.msg, msg);
        DBG("Will return SUCCESS");
        return(SUCCESS);
    } /* End else */
    DBG("Will return SUCCESS");
    return(SUCCESS);
} /* End trans_keywords_delete */

/* Send secure keyword to one or more customers */
 int trans_secure_keyword_send(int index)
{
    DBG();

    int ret;
    /* Verify valid keyword passed in */
    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS) {
        DBG("will return ret='%d'", ret);
        return(ret);
    }

    /* Send the description */
	put_ssl_string(trans[index]->description);

    /* Send email address to server in string format */
    put_ssl_string(trans[index]->email_list);
    
    DBG("will call trans_ret_code_get()");
    return(trans_ret_code_get());
} /* end trans_secure_keyword_send */

/* Retrieve history for keyword */
int trans_history_big_file(int index)
{
    DBG();

    char *file_ptr;
    short ret;

    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS) {
        DBG("not connected to the database server");
        return(ret);
    }
   /* Put the path in the file name */
#ifdef WINDOWS_NT4
    file_ptr = trans[index]->file_name;
#else
    file_ptr = trans[index]->file_name + strcpy2(trans[index]->file_name, transrc.historyDir, "/");
#endif
    /* Get the file name */
    ret = get_ssl_string(file_ptr);
    /* If transaction failed,  return mesage is initialized */
    if (ret < 0) {
        DBG("ret is less then 0");
        return(ret);
    }
    /* do not pull down the history file if user does not want it */
#ifdef TRANSCEND_GUI
    /* Do not want to get the history file for Transcend GUI */
    transrc.historyDir[0] = '\0';
#endif
    if (transrc.historyDir[0] == '\0')
    {
	put_ssl_short(DBNO);
	ret = trans_ret_code_get();
    } else {
	/* If file already present, delete it */
    	if (access(trans[index]->file_name, 0) == 0) 
	unlink(trans[index]->file_name);
    /* Set the file mode */
    trans[index]->file_mode = 33206;
	/* Get the file */
    ret = custom_file_get(index);
	/* Return message retrieved in trans_file_get */
	/* If got file in home directory, let user know */
     if((strcmp(transrc.historyDir, user.cwd) != 0)
     &&(strcmp(transrc.historyDir, "None") != 0))
	strcat(rtn.msg, "\nThis file is in your home directory.");
    }

    DBG("end trans_history_big_file()");
    return(ret);
} /* end trans_history_big_file */

/* Retrieve LDAP infor for user */
int trans_query_user(int index)
{
    DBG();

    char *file_ptr;
    short ret;

    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS) {
        DBG("Cannot Connect to the database server");
        return(ret);
    }
   /* Put the path in the file name */
#ifdef WINDOWS_NT4
    file_ptr = trans[index]->file_name;
#else
    file_ptr = trans[index]->file_name + strcpy2(trans[index]->file_name, transrc.historyDir, "/");
#endif
    /* Get the file name */
    ret = get_ssl_string(file_ptr);
    /* If transaction failed,  return mesage is initialized */
    if (ret < 0)
	return(ret);
	/* If file already present, delete it */
    	if (access(trans[index]->file_name, 0) == 0) 
	unlink(trans[index]->file_name);
	/* Get the file */
    DBG("Will call custom_file_get(index)");
    return(custom_file_get(index));
} /* end trans_query_user */

/* Retrieve list of file keywords for this user */
int trans_user_keys_rtv(int index)
{
    DBG();

    char string[FILE_SIZE];
    int cnt;
    int len;
    short ret;
    char **list;
    char status[5000];

    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS) {
        DBG("Cannot connect to the database server");
        return(ret);
    }
    list = malloc(sizeof(char *) *(ret+1));
    /* Retrieve list in loop */
    for (cnt = 0; cnt < ret; ++cnt)
    {
	len = get_ssl_string(string);
	list[cnt] = malloc(len+1);
	strcpy(list[cnt], string);
    } /* End for */
    /* Get short indicating all data sent */
    get_ssl_short();
    /* Point last pointer at NULL */
    list[cnt] = NULL;
    /* Get the status string */
    ret = get_ssl_string(status);
    /* Get the return code */
    if ((ret = trans_ret_code_get()) < SUCCESS) {
        DBG("trans_ret_code_get()");
        return(ret);
    }

    /* Build list of user keys and their status */
    rtn.len = strlen(rtn.msg);
    for (cnt = 0; list[cnt] != NULL; ++cnt)
        rtn.len += sprintf(rtn.msg+rtn.len, "%c %s\n", status[cnt], list[cnt]);

    DBG("Will return SUCCESS");
    return(SUCCESS);
} /* end trans_user_keys_rtv */

/* Retrieve multi list of keywords */
int trans_multi_keys_rtv(int index)
{
    DBG();

    int rep_index;
    int cnt = 0;
    int status_printed = DBNO;

    for (rep_index = tool.repository_index[0]; rep_index <= tool.repository_index[1]; ++ rep_index)
    {
	/* Initialize the server */
	trans_repository_set(rep_index);
	 cnt += trans_commandline_user_keys_rtv(index, &status_printed);
    } /* end for */
    if (cnt > 0)
    {
	    rtn.msg[0] = '\0';
        DBG("Will return SUCCESS");
        return(SUCCESS);
    }
    else
    {
        DBG("Will return FAILURE");
        return(FAILURE);
    }
} /* end trans_multi_keys_rtv */

int trans_commandline_user_keys_rtv(int index, int *status_printed)
{
    DBG();

    char string[FILE_SIZE];
    int cnt;
    int len;
    short ret;
    char **list;
    char status[5000];

    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS) {
        DBG("cannot connect to the database");
        return(ret);
    }

    list = malloc(sizeof(char *) *(ret+1));

    /* Retrievelist in loop */
    for (cnt = 0; cnt < ret; ++cnt)
    {
	    len = get_ssl_string(string);
	    list[cnt] = malloc(len+1);
	    strcpy(list[cnt], string);
    } /* End for */

    /* Get short indicating all data sent */
    get_ssl_short();

    /* Point last pointer at NULL */
    list[cnt] = NULL;
    ret = get_ssl_string(status);

    /* Get the return code */
    if ((ret = trans_ret_code_get()) < SUCCESS){
        DBG("return code < SUCCESS");
        return(ret);
    }

    if (*status_printed == DBNO)
    {
	    *status_printed = DBYES;
        PRINTF("%s\n\n", rtn.msg);
    }

    /* Print list of user keys and their status */
    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	    PRINTF ("%c %s\n", status[cnt], list[cnt]);
	    free(list[cnt]);
    }

    /* clear out return message so not printed again */
    rtn.msg[0] = '\0';

    DBG("end trans_commandline_user_keys_rtv()");
    return(cnt);
} /* end trans_commandline_user_keys_rtv */

/* Perform auto registration */
int trans_reg_user(int index)
{
    DBG();

    if (index > 0) 
        return(-1);
    else
        return(-1);
} /* end trans_reg_user */

/* Perform transcend registration */
int trans_reg_perform(int index, int argc, char **argv)
{
    DBG();

    /* Check to see if this is an external customer */
    if ((tool.type == EX_TRANSWEB) || (tool.type == EXTERNAL_INTERCOM))
    {
	trans_simple_basic_request_set(index, DBREG_URL , NONE);
	return( trans_perform(index));
    } else {
	if (argc < 3)
	    {
		char *ptr = root_find(argv[0], '/');
		return(trans_help_print( ptr, argv[1][1], DBYES));
	    } /* end if */
	else
	{
	    	    /* Perform a simple request */
	    	    trans_simple_basic_request_set(index, DBAUTO_REG, argv[2]);
	    	    /* The user's login is in arg[2] */
	    	    trans_option_set(index, PAR1, argv[2]);
	    	    /* If password not passed on command line, prompt him for it */
	    	    if (argc < 4)
	    	    {
				    PRINTF("Enter your Active Directory Password .\n> ");
				    echo__stars(trans[index]->par2);
		    /* Separate with two carriage returns */
		    PRINTF("\n\n");
	}
	    else     trans_option_set(index, PAR2, argv[3]);
	    return(trans_perform(index));
	} /* end else */
    } /* end else */
} /* end trans_reg_perform */

int trans1step_upload(int index)
{
    DBG();

    int ret;

    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS)
        return(ret);

    /* See if email notification is being sent */
    if (trans[index]->email_list != NULL)
    {
	    /* If description set, send it, otherwise send blank description */
	    if (trans[index]->description != NULL)
	        put_ssl_string(trans[index]->description);
	    else put_ssl_string("");

	    /* Send the list of e-mail addresses */
        put_ssl_string(trans[index]->email_list);
    } /* end if for e-mail notification */

    /* Get keyword */
    get_ssl_string(rtn.msg);

    /* If output not suppressed, print keyword */
    if (trans[index]->display_transfer_status == DBYES)
        PRINTF("%s\n\n", rtn.msg);

    /* Copy keyword into trans structure */
    sscanf(rtn.msg, "%*s %*S %*s %s", trans[index]->file_keyword);
    strcpy(trans[index]->keyword, trans[index]->file_keyword);

     /* Get indication that server ready to move data */
    if ((ret = get_ssl_short()) < SUCCESS)
    {
	    get_ssl_string(rtn.msg);
	    close(sock);

	    return(ret);
    }

    /* Otherwise get return message from server */
    DBG("calling get_log_string() to return");
    return(get_log_string());
} /* end trans1step_upload */

/* Perform a custom request */
int trans_custom_request(int index)
{

    int ret;

    DBG();

    switch(trans[index]->req_type)
    {
	/* Perform 1 step upload when transfer performed using web interface */
	case DB1STEP_UPLOAD: ret = trans1step_upload(index); break;
	case DBHIST_DATA:  ret = trans_history_big_file(index); break;
	/* Send secure keyword to one or more customers */
	case DBSEND_KEYWORD: ret = trans_secure_keyword_send(index); break;
	case TRANSDIRLIST_RTV: ret = trans_dirlist_rtv(index); break;
	case TRANSDIRLIST_FILE_RTV: ret = trans_dirlist_file_rtv(index); break;
	case DBFAST_QUERY_USER: ret = trans_query_user(index); break;
	case DBQUERY_USER: ret = trans_query_user(index); break;
	case DBGET_RELEASE: ret = trans_release_get(index); break;
#ifndef WINDOWS_NT4
	    case SHARED_MEMORY_CLEANUP: ret = shared_memory_cleanup(user.login);  break;
#endif
	case DBAUTO_REG: ret = trans_reg_user(index); break;
	case DBUSR_KEYS: ret = trans_user_keys_rtv(index); break;
	/* Retrieve list of sites */
	case DBSITE_RTV: ret = trans_site_print(); break;
	/* Retrieve list of repositories */
	case DBREPOSITORY_RTV: ret = trans_repository_print(); break;
	/* Display ICAP classifications */
	case DBICAP_MSG: 
	{
	    strcpy(rtn.msg, ICAP_MSG); 
	     ret = SUCCESS;
	}
	break;
	case GUI_COMPANY_TABLE_RTV: gui_company_table_rtv(index); break;
	default:  
	{
	    strcpy(rtn.msg, invalid_request_type);
	    return(INVALID_REQUEST_TYPE);
	}
	break;
    } /* end switch */
    return(ret);
} /* end trans_custom_request */
#endif
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
