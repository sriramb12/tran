
/****************************************************************************
 *
 * Function:	TransAPI functions
 *
 * Description:	Functions the API can call
 *
 ***************************************************************************/

#include <errno.h>
#include <malloc.h>
#include <pwd.h>
#include <sys/stat.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "company.h"
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
#include "ssl_client_lib.h"

#include "global_debug.h" // Mazieri's debug facilities

extern int sock;
extern char tar_option;
extern char reg_email1[];

/* Functions to initialize data */

/* Allocate TRANS structure and return index for the structure */
int trans_index_find(void)
{
    DBG();

    int index;

    for (index = 0; trans[index] != NULL; ++index)
    {
	/* If index out of range give error */
/*
	if (index >= 100)
	strcpy(rtn.msg, "index_err");
	return(-1);
*/
    }

    DBG("will return index = '%d'", index);
    return(index);
} /* end trans_index_find */


void get_web_client_information()
{
     extern USER_PARAMS user;
     char client_hostname[sizeof(user.mach)];
     char *local_web_client_host =  getenv("REMOTE_ADDR");
     if (local_web_client_host != 0 && isdigit(local_web_client_host[0]))
     {
         struct sockaddr_in sa;
         char client_hostname [ sizeof(user.mach) ];
         sa.sin_family = AF_INET;
         inet_pton(AF_INET, local_web_client_host, &sa.sin_addr);
         // NI_NUMERICSERV | NI_NUMERICHOST
         int ret = getnameinfo((struct sockaddr*)&sa, sizeof(sa), client_hostname, sizeof(user.mach) -1, NULL, 0, 0);
         DBG("ret=%d => hostname web client = '%s'", ret, user.mach);
         if (ret == 0 && client_hostname[0] != '\0')
         {
             strcpy(user.addr, local_web_client_host);
             strcpy(user.mach, client_hostname);
             strcpy(user.machine_type, "Web");
         }
     }
}

/* Initialize user options i.e. login machine, etc. */
/* this function is called before other functions are called */
/* This function is called only once per session */
/* This is for all non Windows platforms */
#ifndef WINDOWS_NT4
void trans_user_params_init(char *core_id)
{
    extern USER_PARAMS user;
    struct passwd *pw_entry;
    unsigned long uid;
    int cnt;

    DBG();

    /* Overwrite any file creation mode user has set */
    umask(0);

    /* Get the user's machine name */
    gethostname(user.mach, sizeof(user.mach) -1);

    /* Get the user-s login name */
    /* Get the User ID */
    uid = getuid();

    /* Open the password file */
    setpwent();

    /* Retrieve data from the password file */
    pw_entry = getpwuid(uid);
    DBG("\npw_entry->pw_name(aka user_login) = %s\npw_entry->pw_dir(aka home_dir) = %s\npw_entry->pw_gecos(aka user_name) = %s", pw_entry->pw_name, pw_entry->pw_dir, pw_entry->pw_gecos);

    if (pw_entry != NULL)
    {
	    /* Copy the user's login name into the user_id string */
	    strcpy(user.login, pw_entry->pw_name);

	    /* Initialize the home directory */
	    strcpy(user.home_dir, pw_entry->pw_dir);

	    /* Copy the user's name into correct field */
	    strcpy(user.name, pw_entry->pw_gecos);

	    /* Check for spaces and commas in the user's name */
	    password_name_check(user.name);
    } /* End if */
    else
    {
	    strcpy(user.login, "unknown");
	    strcpy(user.name, "Unknown Name");
    }

    /* Close the password file */
    endpwent();

    /* Get the IP node address */
    ip_find(user.addr, user.mach);

    /* Initialize the current working directory */
    getcwd(user.cwd, PATH_SIZE);

    /* If no home directory present, make current directory home directory */
    if (user.home_dir[0] == '\0') { strcpy(user.home_dir, user.cwd); }

    /* Reads machine type */
    strcpy(user.machine_type, MACHINE_TYPE);

    /* If core ID passed in, use it in place of the login name */
    if (core_id != NULL)
    {
        strcpy(user.login, core_id);
    }

    /* Set all of the transaction pointers to NULL */
    for (cnt = 0; cnt < 100; ++cnt)
        trans[cnt] = NULL;


#ifdef CGWIN_BUILD
     strcpy(user.home_dir, "/cygdrive/c/Users/");
     strcat(user.home_dir, user.login);
#endif

} /* End trans_user_params_init */
#endif

/* this is the initialization for Windows */
#ifdef WINDOWS_NT4
#include <Windows.h>

void trans_user_params_init(char *core_id)
{
    DBG();

    extern USER_PARAMS user;
    unsigned long uid;
    int cnt;
    DWORD nameLen;


    /* Overwrite any file creation mode user has set */
    umask(0);

    /* Get the user's machine name */
    gethostname(user.mach, 256);

    /* Get the user-s login name */
    nameLen = sizeof(user.login);

    if (!GetUserName(user.login,&nameLen))
    {
        strcpy(user.login, "unknown");
        strcpy(user.name, "Unknown Name");
    }
    else
    {
	    strcpy(user.name, user.login);
	    strexc(user.login, ' ', '-');
    }

    /* Get the IP node address */
    ip_find(user.addr, user.mach);

    /* Exchange any spaces in machine name for '_' */
    strexc(user.mach, ' ', '_');

    /* Initialize the current working directory */
    getcwd(user.cwd, PATH_SIZE);

    /* If no home directory present, make current directory home directory */
    if (user.home_dir[0] == '\0') strcpy(user.home_dir, user.cwd);
        strcpy(user.machine_type, MACHINE_TYPE);

    /* If core ID passed in, use it in place of the login name */
    if (core_id != NULL)
        strcpy(user.login, core_id);
    /* Set all of the transaction pointers to NULL */
    for (cnt = 0; cnt < 100; ++cnt)
        trans[cnt] = NULL;
} /* End trans_user_params_init */
#endif


/* Get a pointer to a transaction structure and set default values */
/* This function is called before any actions or options are set */
int trans_init(int overwrite_files, int do_background_transfer, int suppress_prompt_output, int suppress_transfer_status)
{
    DBG( "overwrite_files='%d', do_background_transfer='%d', suppress_prompt_output='%d', suppress_transfer_status='%d'", overwrite_files, do_background_transfer, suppress_prompt_output, suppress_transfer_status);

    extern int perform_background_transfer;
    int index = trans_index_find();

    trans[index] = malloc(sizeof(TRANS_PARAMS));

    DBG("trans[index]=%p", trans[index]);

    trans_clear(index);
    trans[index]->overwrite_files = overwrite_files;

    /* extern for perform_background_transfer used in tfile_hash */
    /* Do background transfer temporarily disabled */
    do_background_transfer = DBNO;
    trans[index]->perform_background_transfer = perform_background_transfer = do_background_transfer;
    trans[index]->suppress_prompt_output = suppress_prompt_output;

    if (suppress_transfer_status == DBYES) {
        trans[index]->display_transfer_status = 0;
    } else {
        trans[index]->display_transfer_status = 1;
    }

    return(index);
} /* end trans_init */


/* Set default values for a transaction structure */
void trans_clear(int index)
{
    DBG("will set default values for the index = '%d'", index);

    /* Set default values */
    trans[index]->transaction_status = -1;
    trans[index]->req_type = -1;
    trans[index]->transaction_type = -1;
    trans[index]->authen = DBNO;
    trans[index]->transfer_type = UNSPECIFIED_TRANSFER;
    trans[index]->follow_links = DBNO;
    trans[index]->zip_file = DBNO;
    trans[index]->use_gtar = DBYES;
    trans[index]->is_tar = DBNO;
    trans[index]->is_itar = DBNO;
    trans[index]->compress_during_transfer = DBYES;
    trans[index]->uncompress_during_transfer = DBYES;
    trans[index]->send_rtv_email = DBNO;
    trans[index]->company_list_rtvd = DBNO;
    trans[index]->num_days = 0;
    trans[index]->email_list = NULL;
    trans[index]->description = NULL;
    trans[index]->repository = 0;
    trans[index]->compressed_size = 0;
    trans[index]->original_size = 0;
    trans[index]->file_mode = 0;
    trans[index]->eccn_number = 0;
    trans[index]->file_type = 'e';
    trans[index]->fp = NULL;
    trans[index]->files = NULL;
    trans[index]->client_time = time(0);
    trans[index]->symbolic_dir = NULL;

    strcpy(trans[index]->keyword, NONE);
    strcpy(trans[index]->file_name, NONE);
    strcpy(trans[index]->email1, NONE);
    strcpy(trans[index]->email2, NONE);

    /* Initialization of extra parameters */
    strcpy(trans[index]->par1, NONE);
    strcpy(trans[index]->par2, NONE);
    strcpy(trans[index]->par3, NONE);
    strcpy(trans[index]->par4, NONE);
    strcpy(trans[index]->par5, NONE);
    strcpy(trans[index]->par6, NONE);
    strcpy(trans[index]->from_company, NONE);
    strcpy(trans[index]->to_company, NONE);

#ifdef EXTERNAL
        strcpy(trans[index]->icap_class, "CP");
#else
        strcpy(trans[index]->icap_class, "FCP");
#endif

    trans[index]->alternate_file = NULL;
    trans[index]->alternate_dir = NULL;
    trans[index]->tar_type[0] = '\0';
} /* end trans_clear */


/* Allocate space for a field and assign value */
int trans_field_allocate( char **field, char *value)
{
    DBG();

    *field = malloc(strlen(value)+1);
    strcpy(*field, value);

    return(SUCCESS);
} /* end trans_field_allocate */


/* Set the tool type */
/* This function is called just once */
int trans_tool_set(int index, int tool_type)
{
    extern TRANS_TOOL tool;
    extern char tool_name[7][10];
    extern int repository_index[7][2];
    extern int site_index[7][2];

    DBG("index='%d' and tool_type='%d'", index, tool_type);

    switch ((int)tool_type)
    {
	    case INTERNAL_TRANSCEND:
	    {
	        DBG("tool_type is INTERNAL_TRANSCEND");
	        strcpy(user.from_company, FREE_COMPANY);
	        trans_rcread(".transrc");
	    }
	    break;

	    case INTERNAL_INTERCOM:
	    {
	        DBG("tool_type is INTERNAL_INTERCOM");
	        strcpy(user.from_company, FREE_COMPANY);
	        trans_rcread(".icomrc");
	    }
	    break;

	    case EXTERNAL_INTERCOM:
	    {
	        DBG("tool_type is EXTERNAL_INTERCOM");
	        strcpy(user.from_company, EXTERNAL_INTERCOM_COMPANY);
	        trans_rcread(".icomrc");

	        /* Indicate authentication needed */
	        trans[index]->authen = DBYES;
	    }
	    break;

	    case INTERNAL_TRANSWEB:
	    {
	        DBG("tool_type is INTERNAL_TRANSWEB");
	        strcpy(user.from_company, FREE_COMPANY);
	        trans_rcread(".icomrc");
	    }
	    break;

	    case EX_TRANSWEB:
	    {
	        DBG("tool_type is EX_TRANSWEB");
	        strcpy(user.from_company, EXTERNAL_TRANSWEB_COMPANY);
	        trans_rcread(".icomrc");

	        /* Indicate authentication needed */
	        trans[index]->authen = DBYES;
	    }
	    break;

	    case TRANSCEND_INTERCOM:
	    {
	        DBG("tool_type is TRANSCEND_INTERCOM");
	        strcpy(user.from_company, FREE_COMPANY);
	        trans_rcread(".transrc");
	    }
	    break;

	    default:
	    {
	        DBG("tool_type is INVALID_TOOL");
	        strcpy(rtn.msg, invalid_tool);
	        return(INVALID_TOOL);
	    }
	    break;
    } /* end switch */

    /* Set the tool type */
    tool.type = tool_type;

    /* Set the repository and site indexes */
    tool.repository_index[0] = repository_index[tool.type][0];
    tool.repository_index[1] = repository_index[tool.type][1];

    tool.site_index[0] = site_index[tool.type][0];
    tool.site_index[1] = site_index[tool.type][1];

    strcpy(tool.name, tool_name[tool.type]);
    DBG("tool.name='%s'", tool.name);

    /* Set the default repository */
    trans_default_repository_set();

    return(SUCCESS);
} /* end trans_tool_set */

/* Set the iCAP class for data being transferred */
/* This is valid for put and get requests */
#ifdef garbage
int icap_class_set(int index, int icap_class)
{
    DBG();

    switch(icap_class)
    {
	    /* NXP confidential proprietary */
	    case FCP:  strcpy(trans[index]->icap_class, "fcp"); return(SUCCESS);

	    /* NXP internal use only */
	    case FIUO:  strcpy(trans[index]->icap_class, "fiou"); return(SUCCESS);

	    /* Public information */
	    case PUBI:  strcpy(trans[index]->icap_class, "pubi"); return(SUCCESS);

	    /* General business information */
	    case GBI:  strcpy(trans[index]->icap_class, "gbi"); return(SUCCESS);
	    default:
	    {
	        strcpy(rtn.msg, invalid_icap_class);
	        return(INVALID_ICAP_CLASS);
	    } /* End default */
    } /* end switch */
} /* end icap_class_set */
#endif


/* Initialize an option. Options include repository,
 * transfer type, file name, file keyword, secure keyword,
 * delete after retrieval etc
 * iCAP class: Options remain set until they are cleared or changed */
int  trans_option_set(int index, OPTION_TYPE option, char *value)
{
    DBG();

    int ret;

    /* The action must be set before options are set */
    if (trans[index]->req_type == -1)
    {
	    strcpy(rtn.msg, req_type_not_set);
	    return(REQ_TYPE_NOT_SET);
    }

    switch(option)
    {
	    case KEYWORD:
	    {
	        /* Verify this is a valid keyword */
	        if ((ret = is_keyword(trans[index]->req_type, value)) == SUCCESS)
	            strcpy(trans[index]->keyword, value);
	        else
	            return(ret);

	        /* Look up repository for keyword */
	        trans_repository_find(trans[index]->keyword[8]);
	    }
	    break;

	    /* Primary e-mail address */
	    case EMAIL:
	    {
	        strcpy(trans[index]->email1, value);
	    }
	    break;
	    case NUM_DAYS:
	    {
	        if (trans[index]->req_type != DBEXTEND_DATA) {
		        strcpy(rtn.msg, extend_req_err);
		        return(EXTEND_REQ_ERR);

	        } else if ((value < 0) || (value > 3)) {
		        sprintf(rtn.msg, invalid_num_days, value);
		        return(INVALID_NUM_DAYS);

	        } else {
		        trans[index]->original_size = value;
		        return(SUCCESS);
	        }
	    }
	    break;
	    /* Can set repository only for puts and site requests */
	    case FOLLOW_LINKS:
	    {
	        /* If this is not a put, return error */
		    if (trans[index]->transaction_type != PUT_REQUEST)
	        {
	            strcpy(rtn.msg, put_req_err);
	            return(PUT_REQ_ERR);
	        } /* end if */
	        else trans[index]-> follow_links = atoi(value);
	    }
	    break;
	    case SITE:
	    {
			if (trans[index]->transaction_type != PUT_REQUEST)
            {
		        strcpy(rtn.msg, put_req_err);
			    return(PUT_REQ_ERR);
            } /* end if */
	        else return(trans_site_find(index, value));
	    }
	    break;
	    case DESCRIPTION: trans_description_set(index, value); break;
	    case EMAIL_LIST: trans_email_list_set(index, value); break;
	    case EMAIL_STRING: trans_email_string_set(index, value); break;
	    case FILE_NAME:  trans_file_name_set(index, value); break;
	    case FILE_NAMES:  trans_files_set(index, value); break;
	    case SEND_RTV_EMAIL:  trans[index]->send_rtv_email = DBYES; break;
	    case PAR1: strcpy(trans[index]->par1, value); break;
	    case PAR2: strcpy(trans[index]->par2, value); break;
	    case PAR3: strcpy(trans[index]->par3, value); break;
	    case PAR4: strcpy(trans[index]->par4, value); break;
	    case PAR5: strcpy(trans[index]->par5, value); break;
	    case PAR6: strcpy(trans[index]->par6, value); break;
	    default:
	    {
	        strcpy(rtn.msg, invalid_option_type);
	        return(INVALID_OPTION_TYPE);
	    }
    } /* end switch */

    strcpy(rtn.msg, "Option set.");
    return(SUCCESS);
} /* end trans_option_set */


/* Initialize an action */
/* Actions include put, get, set e-mail,  extend, info, history, delete */
int transaction_set(int index, int action)
{
    DBG();

    trans[index]->req_type = action;
    /* Set the transaction type based on the action being performed */
    switch(action)
    {
	case DBPUT_DATA:
	case DBPUT_TAR_FILE:
	{
	    trans[index]->transaction_type = PUT_REQUEST;
	}
	break;
	case DBPUTDLT_DATA:
	case DBPUTDLT_TAR_FILE:
	{
	    trans[index]->transaction_type = PUT_REQUEST;
	}
	break;
	case DBPUT_PARALLEL:
	{
	    trans[index]->transaction_type = PUT_REQUEST;
	    tar_option = 'l';
	}
	break;
	case DBPUTDLT_PARALLEL:
	{
	    trans[index]->transaction_type = PUT_REQUEST;
	    tar_option = 'l';
	}
	break;
	case DBGET_DATA:
	{
	    trans[index]->transaction_type = GET_REQUEST;
	}
	break;
	case DBGETDLT_DATA:
	{
	    trans[index]->transaction_type = GET_REQUEST;
	}
	break;
	case DBGET_SERIAL:
	{
	    trans[index]->transaction_type = GET_REQUEST;
	    tar_option = 's';
	}
	break;
	case DBGET_ZIPPED:
	{
	    trans[index]->transaction_type = GET_REQUEST;
	    tar_option = 'z';
	}
	break;
	case DBGETDLT_ZIPPED:
	{
	    trans[index]->transaction_type = GET_REQUEST;
	    tar_option = 'z';
	}
	break;
	case DBGET_PARALLEL:
	{
	    trans[index]->transaction_type = GET_REQUEST;
	    tar_option = 'l';
	}
	break;
	case DBGETDLT_PARALLEL:
	{
	    trans[index]->transaction_type = GET_REQUEST;
	    tar_option = 'l';
	}
	break;
	default:
	{
	    trans[index]->transaction_type = SIMPLE_REQUEST;
	}
	break;
    } /* End switch */
    return(SUCCESS);
}


/* Perform pending actions */
short trans_perform(int index)
{
    short ret;

    DBG( "index=%d", index );

    /* If request not successfully set, return */
    switch(trans[index]->transaction_type)
    {
	    /* Simple requests are delete, info, size, status, lock, unlock */
	    /* Set file e-mail, cancel file e-mail, owner set, and owner unset */
        /* Extend expiration */
	    case KEYWORD_REQUEST:
	    {
	        DBG("KEYWORD_REQUEST");
	        ret = trans_simple_request(index);
        }
	    break;

	    /* Simple requests are ping, encrypt password, etc */
	    case SIMPLE_REQUEST:
	    {
	        DBG("SIMPLE_REQUEST");
	        ret = trans_simple_request(index);
        }
	    break;

	    /* Pform customer request i.e. history, extend, etc. */
	    case CUSTOM_REQUEST:
	    {
	        DBG("CUSTOM_REQUEST");
	        ret = trans_custom_request(index);
        }
	    break;

	    case PUT_REQUEST:
	    {
	        DBG("PUT_REQUEST");
	        ret = trans_put_request(index);
	    }
	    break;

	    case GET_REQUEST:
	    {
	        DBG("GET_REQUEST");
	        ret = trans_get_request(index);
	    }
	    break;

	    default:
	    {
	        DBG("INVALID_REQUEST_TYPE");
	        strcpy(rtn.msg, invalid_request_type);
	        ret = INVALID_REQUEST_TYPE;
	    }
	    break;
    } /* end switch */

    return(ret);
} /* end trans_perform */


/* Deallocate the TRANS structure */
void trans_cleanup(int index)
{
    DBG();

    /* Deallocate fields dynamically allocated */
    if (trans[index]->files != NULL) {
        DBG("trans[index]->files != NULL, so free it");
        free(trans[index]->files);
        trans[index]->files = NULL;
    }

    if (trans[index]->email_list != NULL) {
        DBG("trans[index]->email_list!=NULL, so free it");
        free(trans[index]->email_list);
        trans[index]->email_list = NULL;
    }

    if (trans[index]->description != NULL) {
        DBG("trans[index]->description!=NULL, so free it");
        free(trans[index]->description);
        trans[index]->description = NULL;
    }

    /* Now deallocate the structure */
    free(trans[index]);
    trans[index] = NULL;
} /* end trans_cleanup */


void trans_header_build(int index, char *buffer)
{
    DBG("index '%d', buffer '%s'", index, buffer);

    extern USER_PARAMS user;
    char tmp_password[90];

    /* If not the GUI, put login name in lower case */
#ifndef TRANSCEND_GUI
    str_lower(user.login);
#endif

    /* Build header to send to Transcend/InterCom server */
    /* Put transfer type if par5 */
    sprintf(trans[index]->par5, "%d", trans[index]->transfer_type);

    /* Put password in message string */
    strcpy2(tmp_password, user.login, TRANS_KEY);
    DBG("tmp_password is '%s'", tmp_password);

    password_encrypt(buffer, tmp_password, PWD_LEN);

    /* Build the header */
    /* global_par6 holds indication that subnet must be checked for encrypted password */
    sprintf(buffer+12," %d %s;%s;%s;%s;%s;%s;%s;%ld %lld %lld %ld %c %s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;",
        trans[index]->req_type, trans[index]->keyword,
        user.login, user.mach, user.addr, user.cwd, TRANSAPI_VERSION,
        user.machine_type, trans[index]->client_time,
        trans[index]->original_size, (unsigned long long)0, trans[index]->file_mode, trans[index]->file_type,
        trans[index]->email1, trans[index]->email2,
        user.from_company, server.site,
        trans[index]->par1, trans[index]->par2, trans[index]->par3, trans[index]->par4, trans[index]->par5, trans[index]->par6, user.name, trans[index]->icap_class);
} /* end trans_header_build */


/* Sends header to the server */
int trans_header_send(char *buffer)
{
    DBG("buffer\n%s", buffer);

    /* Send the header to the server */
    int ret;

    if ((ret = put_ssl_binary_string(buffer, (strlen(buffer+12)+12))) != SUCCESS)
    {
	    close(sock);

	    DBG("Unable to put_ssl_binary_string() string %d", ret);

	    return(ret);
    } /* End if for unable to put string */

    /* Get the response from the server */
    ret = get_ssl_short();
	DBG("get_ssl_short() response %d", ret);

    /* Perform registration */
    if (ret == DBEMAIL)
        return(user_autoreg());

    if (ret >= SUCCESS) return(ret);

    /* If ret out of range, error getting data from the data base */
    if (ret == BAD_GET) {
        DBG("error getting data from the database sisnce ret == BAD_GET (ret='%d')", ret);
        return(ret);
    }

    rtn.len += get_ssl_string(rtn.msg + rtn.len);
	close(sock);

    return(ret);
} /* end trans_header_send */


short trans_socket_init(int index)
{
    DBG("index '%d'", index);

    char buffer[BUFFER_SIZE] = {0};

    /* Build header to send to Transcend/InterCom server */
    trans_header_build(index, buffer);
    DBG("header built");

    return( intermediate_ssl_init(trans[index]->req_type, buffer) );
} /* End trans_socket_init */


int trans_keyword_request(int index, REQ_TYPE req_type, char *keyword)
{
    DBG();

    int ret;

    /* Set the transaction type */
    trans[index]->transaction_type = SIMPLE_REQUEST;

    /* Set the action */
    trans[index]->req_type = req_type;

    /* Set the keyword */
    strcpy(trans[index]->keyword, keyword);
    ret = trans_perform(index);

    return(ret);
} /* end trans_keyword_request */


/* Send list of strings to the server */
int trans_list_send(int index, char *list[])
{
    DBG();

    int cnt;
    short ret;

    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS) return(ret);

    /* Send list of names */
    for (cnt = 0; list[cnt] != NULL; ++cnt)
        put_ssl_string((char *)list[cnt]);

    /* Send indication that all list sent */
    put_ssl_short(SUCCESS);

    /* Get the return code */
    return(trans_ret_code_get());
} /* End trans_list_send */


/* Retrieve list of strings from the server */
int trans_list_rtv(int index, char **list[])
{
    DBG();

    char string[FILE_SIZE];
    int cnt;
    int len;
    short ret;

    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS) return(ret);

    *list = malloc(sizeof(char *) *(ret+1));

    /* Retrieve list in loop */
    for (cnt = 0; cnt < ret; ++cnt)
    {
	    len = get_ssl_string(string);
	    (*list)[cnt] = malloc(len+1);
	    strcpy((*list)[cnt], string);
    } /* End for */

    /* Point last pointer at NULL */
    (*list)[cnt] = NULL;

    /* Get the return code */
    return(trans_ret_code_get());
} /* End trans_list_rtv */


/* Send a simple request to the server */
int trans_request_send(REQ_TYPE req_type, int index)
{
    short ret;

    DBG("req_type='%d', index='%d'", (REQ_TYPE)req_type, index);

    /* Connect to the database server */
	/* Get the return code */
    trans[index]->req_type = req_type;

    if ((ret = trans_socket_init(index)) < SUCCESS) {
        DBG("ERROR when call trans_socket_init(), returned ret='%d'", ret);
        return(ret);
    } else {
        DBG("SUCCESS when call trans_socket_init(), returned ret='%d'", ret);
        return( trans_big_ret_code_get() );
    }

    DBG("If we hit this point, means the function did not work as expected and there is no return value to match the function signature");
} /* end trans_request_send */


/* Verify user entered correct user ID and password */
int dbx500authen(char *x500id, char *x500pwd)
{
    DBG();

    extern char reg_email1[];
    short ret;
    char int_pwd[128];
    char *ptr;
    int index = trans_init(DBNO, DBNO, DBNO, DBNO);


    /* Encrypt password and change to integer format */
    encrypt_ascii_pwd(int_pwd, x500pwd);

    strcpy(user.email, x500id);
    strcpy(user.name, x500id);
    strcpy(reg_email1, x500id);

    /* Connect to the database server */
    trans[index]->req_type = DBENCRYPT_PASSWORD_LOG;

    if ((ret = trans_socket_init(index)) != SUCCESS)
        return(ret);

    /* Get the return code */
    ret = trans_ret_code_get();

    if ((ptr = strchr(rtn.msg, ';')) != NULL)
        strcpy(user.name, ptr+1);

    return(ret);
} /* End dbx500authen */


/* Set the return message */
char *trans_rtn_msg_rtv(void)
{
    DBG();

    int cnt;

    /* Make sure there are two carriage returns on the string */
    /* Get length of the string */
    rtn.len =  strlen(rtn.msg) - 1;

    /* Find first character before carriage return */
    for (cnt = rtn.len; cnt > 0 && rtn.msg[cnt] == '\n'; --cnt)
        /* Loop off all of the carriage returns */
        /* We want to put them in ourselves to keep them consistent */
        rtn.msg[cnt] = '\0';

    /* Now add the line feed */
    strcat(rtn.msg, "\n\n");

    return(rtn.msg);
}


/* Retrieve the Transcend keyword for a put request */
char *trans_keyword_rtv(int index)
{
    DBG();

    extern char api_keyword[];

    strcpy(api_keyword, trans[index]->keyword);

    DBG("api_keyword='%s'", api_keyword);
    return(api_keyword);
}

/* Return total size of data being transferred */
unsigned long long trans_total_size_rtv(int index)
{
    DBG();

    return(trans[index]->original_size);
} /* end trans_total_size_rtv */


/* Return status of transfer */
int trans_transfer_status_rtv(int index)
{
    DBG();

/* Disabling ability to read shared memory */
#ifdef garbage
/* #ifndef WINDOWS_NT4 */
    unsigned long long transfer_size;
    int status = shared_status_read(&transfer_size);
    return(status);
#else
    return(0);
#endif
} /* end trans_transfer_status_rtv */


/* Return amount of data transferred */
unsigned long long trans_transfer_size_rtv(int index)
{
    DBG();

    unsigned long long transfer_size = 0;
    return(transfer_size);
} /* end trans_transfer_size_rtv */


int dblist_request(char req_type, char *primary_key, char *secondary_key, char **list[]) {
    DBG();
    return(0);
}


short trans_ret_code_get(void)
{
    DBG();

    /* Get the return code from the server */
    short ret = get_ssl_short();

    /* Get the return string from the server */
    get_ssl_string(rtn.msg);
    close(sock);

    DBG("rtn.msg='%s', ret='%d'", rtn.msg, ret);
    return(ret);
} /* End trans_ret_code_get */


/* Used to get extra large return code */
short trans_big_ret_code_get(void)
{
    DBG();

    short get_ssl_big_string(char *buffer);
    short ret;

    /* Get the return code from the server */
    ret = get_ssl_short();

    /* Get the return string from the server */
    get_ssl_big_string(rtn.msg);

    close(sock);

    DBG("ret='%d', rtn.msg=%s", ret, rtn.msg);

    return(ret);
} /* End trans_ret_code_get */

