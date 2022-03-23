#include <alloca.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_lib_prototypes.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h"

void transweb_history(char *key)
{
    char tmp_key[16];
    short ret;
    FILE *fp;

    DBG("key='%s'", key);

    strcpy(tmp_key, key);

    /* If the key is found, it is returned in variable 'key' */
    keyword_find(tmp_key, key);

    /* Send the history file to the user */
    /* Build complete name of file */
    strcpy3(bf.file_name, HISTORY_FILE_PATH, key, ".history");

    /* Obtain a file descriptor to file */
    fp = fopen(bf.file_name, "r");
    if (fp == NULL) return_non_err(key);

    /* Scan first the first line, it contains the delete date */
    if (line_get(fp, bf.buffer) <= 0)
        close_down1m(read_err, bf.file_name, READ_ERR);

    /* Indicate that file wil be sent */
    put_short(SUCCESS);

    ret = fread(rtn.msg, sizeof(*rtn.msg), 4096, fp);

    rtn.msg[ret] = '\0';
    fclose(fp);

    /* Send the return code */
    put_success_msg(key);

    /* If the user's lock count is not set to 0, set it back to 0 */
    if (user_lock_cnt > 0)
        mod_lock_cnt(0);
} /* end transweb_history */


/* Read list of secure keywords from internal server */
int secure_keys_read(char *list[1000], char *status)
{
    char global_file[80];
    FILE *fi;
    char line[260];
    char secure_key[10];
    char file_key[10];
    char email[EMAIL_LEN];
    int cnt = 0;

    DBG();

    if ((fi = fopen(KEYWORD_FILE, "r")) == NULL)
        close_down1m(rd_open_err, KEYWORD_FILE, RD_OPEN_ERR);

    /* Strip off nxp.com */
	company_email_strip(gu.email1);
	company_email_strip(gu.email2);

    while (line_get(fi, line) != EOF)
    {
	    if (sscanf(line, "%s %s %s", secure_key, file_key, email) != 3) continue;

	    /* Strip off nxp.com */
	    company_email_strip(email);

	    if ((strcmp(email, gu.email1) == 0) || (strcmp(email, gu.email2) == 0))
	    {
	        list[cnt] = malloc(10);
	        strcpy(list[cnt], secure_key);

	        /* Get the status for the key */
	        strcpy2(global_file, "global_data/", file_key);
	        if (access(global_file, 0) == 0)
	            status[cnt] = '+';
	        else
	            status[cnt] = '-';

	        ++cnt;
	    } /* End if */
    } /* End while */

    /* Point last element on list to NULL */
    list[cnt] = NULL;

    /* Close the file */
    fclose(fi);

    return(cnt);
} /* end secure_keys_read */


/* Read list of secure keywords from external server */
int external_secure_keys_read(char *list[1000], char *status)
{
    extern char secure_keyword_extension[];
    char global_file[80];
    FILE *po;
    char line[260];
    char secure_key[10];
    char file_key[10];
    char email[EMAIL_LEN];
    char command[80];
    int cnt = 0;

    DBG();

    /* Strip off nxp.com from e-mail address */
	company_email_strip(gu.email1);

    /* Build command to run */
    strcpy4(command,  "egrep -ih ", gu.email1, " db_admin/keywords/*", secure_keyword_extension);

    if ((po = popen(command, "r")) == NULL)
        close_down1m(rd_open_err, KEYWORD_FILE, RD_OPEN_ERR);

    while (line_get(po, line) != EOF)
    {
	    if (sscanf(line, "%s %s %s", secure_key, file_key, email) != 3) continue;

	    list[cnt] = malloc(10);
	    strcpy(list[cnt], secure_key);

	    /* Get the status for the key */
	    strcpy2(global_file, "global_data/", file_key);

	    if (access(global_file, 0) == 0)
	        status[cnt] = '+';
	    else
	        status[cnt] = '-';

	    ++cnt;
    } /* End while */

    /* Point last element on list to NULL */
    list[cnt] = NULL;

    /* Close the pipe */
    pclose(po);

    return(cnt);
} /* end external_secure_keys_read */


void short_keys_send(char *list[1000], char *status, int list_cnt)
{

    DBG();

    if (list_cnt > 0)
    {
	    put_short(list_cnt);
	    list_get(list);
	    put_string(status);

        strcpy3(rtn.msg, "List of personal secure keywords shown below.\n", "Status '+' indicates data is present in repository.\n", "status of '-' indicates data has been deleted from repository.\n\n");
        put_success_msg(NONE);
    }
    else
    {
	    sprintf(rtn.msg, no_keyword_wrn, gu.email1, server.site);
	    close_down1f(rtn.msg, gu.email1, NO_KEYWORD_WRN);
    }
} /* End short_keys_send */


void user_keys_rtv(void)
{
    char *status;
    char file[128];
    char *file_ptr = file + strcpy1(file, "global_data/");
    char **new_list;
    char *list[5000];
    char file_name[128];
    int cnt;
    int cnt1;

    DBG();

    format_user_keys_file_name(file_name);

    if (access(file_name, 0) != 0)
    {
        sprintf(rtn.msg, no_file_key_wrn, user.email, server.site);
	    shut_down(rtn.msg, NO_FILE_KEY_WRN);

	    exit(0);
    }

    cnt = list_read(file_name, list);
    if (cnt <= 0)
    {
        sprintf(rtn.msg, no_file_key_wrn, user.email, server.site);
	    shut_down(rtn.msg, NO_FILE_KEY_WRN);

	    exit(0);
    }

    /* Put the list in reverse cronological order */
    m2list_reverse(&new_list, list, cnt);
    new_list[cnt] = NULL;

    /* Now collect the status */
    status = malloc(cnt+1);

    for (cnt1 = 0; cnt1 < cnt; ++cnt1)
    {
	    /* Build global file name */
	    strcpy(file_ptr, new_list[cnt1]);

	    if (access (file, 0) == 0) status[cnt1] = '+';
	    else status[cnt1] = '-';
    }

    status[cnt] = '\0';

    /* Tell client ready to send data */
    put_short(cnt);

    /* Send the list */
    list_get(new_list);

	/* Send status for the keywords */
    put_string(status);

    strcpy3(rtn.msg, "List of user keywords shown below.\n", "Status '+' indicates data is present in repository.\n", "status of '-' indicates data has been deleted from repository.\n\n");

    put_success_msg(NONE);

    free(new_list);
} /* End user_keys_rtv */


/* Retrieve list of secure keywords from internal server */
void secure_keys_rtv(void)
{
    char *list[1000];
    char **reverse_list;
    char status[1000];
    char *reverse_status;
    int list_cnt = 0;

    DBG();

    list_cnt = secure_keys_read(list, status);

    /* Put list in reverse chronilogical order */
    m2list_reverse(&reverse_list, list, list_cnt);

    /* Reverse status also */
    reverse_status = malloc(list_cnt+1);
    str_reverse(reverse_status, status);
    short_keys_send(reverse_list, reverse_status, list_cnt);
} /* end secure_keys_rtv */


/* Retrieve list of secure keywords from external server */
void external_secure_keys_rtv(void)
{
    char *list[1000];
    char **reverse_list;
    char status[1000];
    char *reverse_status;
    int list_cnt = 0;

    DBG();

    list_cnt = external_secure_keys_read(list, status);

    /* Put list in reverse chronilogical order */
    m2list_reverse(&reverse_list, list, list_cnt);
 
    /* Reverse status also */
    reverse_status = malloc(list_cnt+1);
    str_reverse(reverse_status, status);
    short_keys_send(reverse_list, reverse_status, list_cnt);
} /* end external_secure_keys_rtv */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
