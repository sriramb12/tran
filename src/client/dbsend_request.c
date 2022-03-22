#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	dbsend_request
 *
 * Description:	Sends the Transcend/InterCom server transaction requests.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#ifdef WINDOWS_NT4
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
 extern int errno;
#define closesocket close
#include <netinet/in.h>
     #include <alloca.h>
#endif
#include <malloc.h>
     #include <stdlib.h>
#include "client_files.h"
#include "global_client_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include <sys/stat.h>
#include "trans_prototypes.h"
#include "client_lib.h"
#include "transapi_defines.h"
#include <string.h>
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

void repository_set(int index);
void repository_scan(char *line);

int dbsend_request(char req_type, char *info)
{
    extern char remote_dir[];
    short ret;

    DBG();

    /* Connect to the database server */
    if ((ret = init_trans_socket(req_type, info, NONE, remote_dir, 0, 0, UNSPECIFIED, NONE_STRING)) < SUCCESS)
        return(ret);

    /* Get the return code */
    return(get_ret_code());
} /* End dbsend_request */

#ifndef TRANSWEB
int dbsend_big_request(
    char req_type,
    char *info)
{
    extern char remote_dir[];
    short ret;

    /* Connect to the database server */
    if ((ret = init_trans_socket(req_type, info, NONE, remote_dir, 0, 0,
    UNSPECIFIED, NONE_STRING)) < SUCCESS)
    return(ret);
    /* Get the return code */
    return(get_big_ret_code());
} /* End dbsend_big_request */
#endif

int dbsend_list_extension_request(void)
{
    char extension_buffer[BUFFER_SIZE];
    extern char *extension_list[500];
    extern float*ratio_list[500];
    short ret;

    /* Connect to the database server */
#ifdef TRANSCEND
    if ((ret = init_trans_socket(DBLIST_EXTENSIONS_RTV, NONE, NONE, NONE, 0, 0,
    UNSPECIFIED, NONE_STRING)) == SUCCESS)
    {
	ret = get_string(extension_buffer);
	/* Get the return code */
	ret = get_ret_code();
    }
    /* If could not get the list, just use default list */
    else strcpy(extension_buffer, ". 1.0\n");
#endif
#ifdef INTERCOM
    strcpy(extension_buffer, ". 1.0\n");
#endif
    /* Convert to list format */
    ret = string2double_list_convert(extension_buffer, extension_list, ratio_list);
    return(ret);
} /* End dbsend_list_extension_request */

/* Retrieve a list from the server */
/* Returns the number of items retrieved if successful */
int dblist_request(
    char req_type,
    char *primary_key,
    char *secondary_key,
    char **list[])
{
    int cnt = 0;
    short ret = 0;

    /* Connect to the database server */
    if ((ret = init_trans_socket(req_type, primary_key, NONE, remote_dir, 0, 0,
                                 UNSPECIFIED, secondary_key, NONE, NONE, NONE,NONE,NONE)) <= SUCCESS)
    {
        *list = NULL;
        DBG("return less than 0");
        return(ret);
    }
    *list = malloc(sizeof(char *) *(ret+1));
    /* Get list of names */
    if ((cnt = names_get(*list)) < 0)
    {
        free(*list);
        *list = NULL;
        DBG("return 0");
        return(0);
    }
    else if (ret < cnt)
    {
        sprintf(rtn.msg, array_alloc_err, ret, cnt);
        DBG("return error");
        return(ARRAY_ALLOC_ERR);
    } /* End else */

    /* Get the return code */
    if ((ret = get_ret_code()) < 0)
    {
        free(*list);
        *list = NULL;
        DBG("return error");
        return(ret);
    }
    DBG("cnt=%d" , cnt);
    return(cnt);
} /* End dblist_request */

int dblist_send(
    char req_type,
    char *primary_key,
    char **list[])
{
    int cnt;
    short ret;

    /* Connect to the database server */
    if ((ret = init_trans_socket(req_type, primary_key, NONE, remote_dir, 0, 0,
    UNSPECIFIED, NONE_STRING)) < SUCCESS)
    return(ret);
    /* Send list of names */
    for (cnt = 0; list[cnt] != NULL; ++cnt)
    put_string((char *)list[cnt]);
    /* Send indication that all list sent */
    put_short(SUCCESS);
    /* Get the return code */
    return(get_ret_code());
} /* End dblist_send */

int dbmulti_list_request(
    char req_type,
    char *keyword,
    char **list[],
    char **status)
{
    int cnt;
    short ret;

    /* Connect to the database server */
    if ((ret = init_trans_socket(req_type, keyword, NONE, remote_dir, 0, 0,
    UNSPECIFIED, NONE_STRING)) < SUCCESS)
    {
	*list = NULL;
    return(ret);
    }
    *list = malloc(sizeof(char *) *(ret+2));
    /* Get list of names */
    if ((cnt = names_get(*list)) < 0)
    {
	free(*list);
	*list = NULL;
	return(cnt);
    }
    else if (ret < cnt)
    {
	sprintf(rtn.msg, array_alloc_err, ret, cnt);
	return(ARRAY_ALLOC_ERR);
    } /* End else */
    /* Get the additional string */
    cnt = get_string(rtn.msg);
    *status = malloc(cnt + 1);
    strcpy(*status, rtn.msg);
    if ((ret = get_ret_code()) < 0)
    {
	free(*status);
	free(*list);
	*list = NULL;
	return(ret);
    }
	else return(cnt);
} /* End dbmulti_list_request */

#ifndef M2
int dbsend_keyword_request(
    char req_type,
    char *info)
{
    short ret;
    /* Verify this is a valid keyword */
    if ((ret = is_keyword(req_type, info)) < 0) return(ret);
#ifndef SINGLE_CONNECT
/* If this is TransWeb, set up the server */
    repository_find(info[8]);
#endif
    /* Connect to the database server */
    if ((ret = init_trans_socket(req_type, info, NONE, NONE, 0, 0,
    UNSPECIFIED, NONE_STRING)) != SUCCESS)
    return(ret);
    /* Get the return code */
    return(get_ret_code());
} /* End dbsend_keyword_request */
#endif

/* Verify user entered correct user ID and Active directory password */
int dbx500authen(char *x500id, char *x500pwd)
{
    extern char reg_email1[];
    short ret;
    char int_pwd[128];
    char *ptr;

    DBG();

    /* Encrypt password and change to integer format */
    encrypt_ascii_pwd(int_pwd, x500pwd);

    strcpy(user.email, x500id);
    strcpy(user.name, x500id);
    strcpy(reg_email1, x500id);

    /* Connect to the database server */
    if ((ret = init_trans_socket(DBX500AUTHEN, int_pwd, x500id, NONE, 0, 0, UNSPECIFIED, NONE_STRING)) != SUCCESS)
        return(ret);

    /* Get the return code */
    ret =  get_ret_code();

    if ((ptr = strchr(rtn.msg, ';')) != NULL)
        strcpy(user.name, ptr+1);

    return(ret);
} /* End dbx500authen */


/* Verify specified user is registered */
int dbsite_check(char *extension)
{
    short ret;
    char is_foundry[8];

    /* Set the specified site */
    if (extension[0] == '-')
    company_find(extension+1, is_foundry);
    else company_find(extension, is_foundry);
    /* Connect to the database server */
    if ((ret = init_trans_socket(DBSITE_CHECK, NONE, NONE, NONE, 0, 0,
    UNSPECIFIED, NONE_STRING)) != SUCCESS)
    return(ret);
    /* Get the return code */
    return(get_ret_code());
} /* End dbsite_check */


int user_keys_rtv(void)
{
    char *status;
    int cnt;
    char **list;

    list = trans_keys_rtv(DBUSR_KEYS, "All;filler", &status);
    if (list == NULL)
    {
	printf("No user keys present.\n\n");
	free(list);
	free(status);
	return(0);
    }
    PRINTF(rtn.msg);
    PRINTF("\n\nStatus of '+' indicates data is present in repository.");
    PRINTF("\nStatus of '-' indicates data has been deleted.");
   PRINTF("\n\nSTATUS   KEYWORD\n");
    PRINTF("------   -------\n\n");
    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	printf("   %c     %s\n", status[cnt], list[cnt]);
	free(list[cnt]);
    }
    free(list);
    free(status);
	PRINTF("\n");
	return(0);
} /* End user_keys_rtv */

char **trans_keys_rtv(int req_type, char *site, char **global_status)
{
    char *local_status;
    char **local_ptr;
    int ret;
    static char **ptr;
    int cnt;
    int total_num = 0;
    int old_total, local_cnt, final_cnt;

    DBG();

	ptr = malloc(sizeof(char *)*5000);
    *global_status = malloc(5000);
    *global_status[0] = '\0';
    /* Do the following for a specific site */
    if (strncasecmp(site, "All", 3) != 0)
    {
	repository_scan(site);
	 total_num = dbmulti_list_request(req_type, NONE, &ptr, global_status);
    } else {
	/* Do the following for all of the sites */
	for (cnt = 0; repository[cnt].name[0] != '\0'; ++cnt)
	{
	/* Simply set the values for the current repository */
	    repository_set(cnt);
	    /* Keep track of old number */
	    old_total = total_num;
	    /* Grab results from the list */
	 ret  = dbmulti_list_request(req_type, NONE, &local_ptr, &local_status);
    if (ret < 0) continue;
    total_num += ret;
	    /* copy elemnts of list from local pointer to final pointer */
	    /* If nothing retrieved skip to next element */
	    if (old_total == total_num) continue;
	    for (local_cnt = 0, final_cnt = old_total; local_ptr[local_cnt] != NULL; ++local_cnt, ++final_cnt)
	    {
		ptr[final_cnt] = local_ptr[local_cnt];
	    } /* End for for doing copy */
    /* This is in place to verify keywords are really found */
    total_num = final_cnt;
	strcat(*global_status, local_status);
	    } /* End for for each site */
    /* If no keys returned, point first element to the empty string */
    } /* End else */
    if (total_num <= 0)
    {
	ptr[0] = NULL;
    } else {
	ptr[total_num] = NULL;
	strcpy(rtn.msg, "List of keywords retrieved.");
    } /* End else */
    return(ptr);
} /* End trans_keys_rtv */

void repository_set(int index)
{
    extern TRANS_REPOSITORY repository[];

	    strcpy(server.address, repository[index].address);
	    strcpy(server.machine, repository[index].machine);
	    strcpy(server.site, repository[index].name);
	server.socket = htons(repository[index].socket);
} /* End repository_set */


void repository_scan(char *line)
{

    /* Scan line into parameters */
    sscanf(line, "%[^;]%*c%*[^;]%*c%*c%*c %*c%*c %[^;]%*c %[^;]%*c %d",
    server.site, server.machine, server.address, &server.socket);
    server.socket = htons(server.socket);
} /* End repository_scan */

/* Used to delete both secure and user keys from one or all repositories */
int trans_keys_delete(int req_type, char *site)
{
    char msg[BUFFER_SIZE];
    char *msg_ptr = msg;
    int cnt;

    if (strncmp(site, "All", 3) != 0)
    {
	repository_scan(site);
    return(dbsend_request(req_type, NONE));
    } else {
	/* Do the following for all of the sites */
	for (cnt = 0; repository[cnt].name[0] != '\0'; ++cnt)
	{
	    repository_set(cnt);
	dbsend_request(req_type, NONE);
	msg_ptr += strcpy2(msg_ptr, rtn.msg, "\n\n");
    } /* End for */
    strcpy(rtn.msg, msg);
    return(0);
    } /* End else */
} /* End trans_keys_delete */
#endif /* For not TRANSAPI */
