/****************************************************************************
 *
 * Function:	keyword_read
 *
 * Description:	Retrieves list of secure keywords sent to specified user
 *		        from the database client. Calls other functions
 *		        to perform specific transactions.
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_defines.h"
#include "trans_server_arrays.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_macros.h"
#include "trans_server_paths.h"
#include "trans_server_prototypes.h"
#include "reg_globals.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "x500_defines.h"
#include "global_debug.h"


typedef struct
{
    long time;
    char string[5000];
} LIST;

LIST *list[5000];


/* Strip off @nxp.com */
int company_email_strip(char *email_addr)
{
    int len;

    DBG("email_addr='%s'", email_addr);

    /* If length too short just return */
    if ((len = strlen(email_addr)) < 9)
        return(len);

    /* If is @nxp.com, strip it off */
    if (strcasecmp(email_addr+len-8, "@nxp.com") == 0)
    {
	    email_addr[len-8] = '\0';
	    return(len-8);
    } /* end if */
    else 
        return(len);
} /* end company_email_strip */


int email_replace_at_character_by_dot(char *email_addr)
{
    int len = strlen(email_addr);
    char *at = strchr(email_addr, '@');
    if (at != NULL)
    {
        *at = '.';
    }
}

void format_user_keys_file_name(char *filename /*OUT*/)
{
    char *id = user.email;
    if (id[0] == '\0')
    {
        id = user.login;
    }
    str_lower(id);
    strcpy2(filename, "user_keys/", id);
    email_replace_at_character_by_dot(filename);
}

/* Reads last transaction from history file */
int history_read(char *line, char *file_key, long *time)
{
    FILE *fp;
    int get = DBNO;
    int len= 0;
    int ret;
    char history_file[36];

    DBG("line='%s', file_key='%s'", line, file_key);

    strcpy3(history_file, "history_data/", file_key, ".history");

    if ((fp = fopen(history_file, "r")) == NULL)
    {
	    line[0] = '\0';
	    return (0);
    }

    /* Skip the first line of the file */
    line_get(fp, line);

    /* Get the expiration time */
    sscanf(line, "%ld", time);

    /* Read through the history file until end reached */
    while (len < 950)
    {
        len += strcpy1(line+len, "\n    ");
        if ((ret = line_get(fp, line+len)) == EOF) break;

	    /* If this is a get and doing only unretrieved keys, skip it */
	    if ((gu.req_type == DBQUERY_KEYWORD) && (strncmp(line+len, "Get", 3) == 0))
	    {
	        printf("found a get\n");
	        get = DBYES;
	        break;
	    } /* End if */

	    len += ret;
    } /* End while */

    /* Put on trailing carriage return */
    strcpy(line+len-4, "\n");

    /* Close history file no longer needed */
    fclose(fp);

    if (get == DBYES) return(FAILURE);
    else return(len);
} /* End history_read */


/* Read long list of file keywords for internal or external user */
/* These are present in a file called user_keys/core_ID */
int long_file_keywords_read(void)
{
    long new_time = 0;
    char history_line[4096];
    char file_key[260];
    int len;
    int list_cnt = 0;
    char fnd = DBNO;
    char keyword_file[128];
    FILE *fi;

    DBG();

    format_user_keys_file_name(keyword_file);

    if ((fi = fopen(keyword_file, "r")) == NULL)
    {
	    sprintf(rtn.msg, no_file_key_wrn, gu.email1, server.site);
	    shut_down(rtn.msg, NO_FILE_KEY_WRN);
	    exit(0);
    }

    /* Initialize return message */
    rtn.len = strcpy3(rtn.msg, "File keywords sent to ", gu.email1, " include:\n\n");

    while (line_get(fi, file_key) != EOF)
    {
	    /* Retrieve history for this keyword */
        file_key[9] = '\0';

	    len = history_read(history_line, file_key, &new_time);
	    list[list_cnt] = malloc(sizeof(LIST));

	    if (len > 0)
	        strcpy3(list[list_cnt]->string, file_key, ":\n", history_line);
	    else
	        strcpy2(list[list_cnt]->string, file_key, "\n\n");

	    list[list_cnt++]->time = new_time;
	    fnd = DBYES;
    } /* End while */

    /* Close the keyword file */
    fclose(fi);

    if (fnd == DBNO)
    {
	    sprintf(rtn.msg, no_file_key_wrn, gu.email1, server.site);
	    shut_down(rtn.msg, NO_FILE_KEY_WRN);
	    exit(0);
    }

    return(list_cnt);
} /* end long_file_keywords_read */


/* Read long list of secure keywords for internal customer */
/* These are present in a file called keyword_file */
int long_secure_keywords_read(void)
{
    long new_time = 0;

    char history_line[5024];
    char file_key[16];
    int len;
    int list_cnt = 0;
    int len1;
    int len2;
    int rtv_num;
    char fnd = DBNO;
    char line[260];
    char key[10];
    char email[32];
    FILE *fi;

    DBG();

    if ((fi = fopen(KEYWORD_FILE, "r")) == NULL)
        close_down1m(rd_open_err, KEYWORD_FILE, RD_OPEN_ERR);

    /* Initialize return message for entries found */
    if (gu.req_type == DBQUERY_KEYWORD)
        rtn.len = strcpy3(rtn.msg, "Personal secure keywords not retrieved sent to ",  gu.email1, " include:\n\n");
    else
        rtn.len = strcpy3(rtn.msg, "Personal secure keywords sent to ", gu.email1, " include:\n\n");

    /* If this is an '@nxp.com' form, strip off following part of email */
    len1 = company_email_strip(gu.email1);
    len2 = company_email_strip(gu.email2);

    while (line_get(fi, line) != EOF)
    {
	    if ((sscanf(line, "%s %s %s %d", key, file_key, email, &rtv_num) == 4) && ((strncmp(email, gu.email1, len1) == 0) || (strncmp(email, gu.email2, len2) == 0)))
	    {
	        /* If data already retrieve and want only unretrieved keys, skip this key */
	        if ((gu.req_type == DBQUERY_KEYWORD) && (rtv_num > 0)) continue;

	        /* Get transactions from history file */
	        /* Skip those already retrieved if retrieving only unretrieved keywords */
	        if ((len = history_read(history_line, file_key, &new_time)) == FAILURE) continue;

	        list[list_cnt] = malloc(sizeof(LIST));

	        if (len > 0)
	            strcpy3(list[list_cnt]->string, key, ":\n", history_line);
	        else
	            strcpy2(list[list_cnt]->string, key, "\n\n");

	        list[list_cnt++]->time = new_time;
	        fnd = DBYES;
	    } /* End if */
    } /* End while */

    /* Close the keyword file */
    fclose(fi);

    if (fnd == DBNO)
    {
	    if (gu.req_type == DBQUERY_KEYWORD)
	        strcpy3(rtn.msg, "All data for personal secure keywords sent to ", gu.email1, " retrieved.");
	    else
	        sprintf(rtn.msg, no_keyword_wrn, gu.email1, server.site);

	    shut_down(rtn.msg, NO_KEYWORD_WRN);
	    exit(0);
    }

    return(list_cnt);
} /* end long_secure_keywords_read */


/* Read long list of secure keywords for external customer */
/* These are present in a directory called keywords */
int long_external_secure_keywords_read(void)
{
    extern char secure_keyword_extension[];
    FILE *po;
    long new_time = 0;
    char history_line[1024];
    char file_key[16];
    int len;
    int list_cnt = 0;
    int len1;
    int len2;
    int rtv_num;
    char fnd = DBNO;
    char line[260];
    char key[10];
    char email[32];
    char command[260];

    DBG();

    /* Strip off @nxp.com from e-mail address */
    company_email_strip(gu.email1);

    /* Build command to run */
    strcpy4(command,  "egrep -ih ", gu.email1, " db_admin/keywords/*", secure_keyword_extension);

    if ((po = popen(command, "r")) == NULL)
        close_down1m(rd_open_err, KEYWORD_FILE, RD_OPEN_ERR);

    /* Initialize return message for entries found */
    if (gu.req_type == DBQUERY_KEYWORD)
        rtn.len = strcpy3(rtn.msg, "Personal secure keywords not retrieved sent to ", gu.email1, " include:\n\n");
    else
        rtn.len = strcpy3(rtn.msg, "Personal secure keywords sent to ", gu.email1, " include:\n\n");

    len1 = company_email_strip(gu.email1);
    len2 = company_email_strip(gu.email2);

    while (line_get(po, line) != EOF)
    {
	    if ((sscanf(line, "%s %s %s %d", key, file_key, email, &rtv_num) == 4) && ((strncmp(email, gu.email1, len1) == 0) || (strncmp(email, gu.email2, len2) == 0)))
	    {
	        /* If data already retrieve and want only unretrieved keys, skip this key */
	        if ((gu.req_type == DBQUERY_KEYWORD) && (rtv_num > 0)) continue;

	        /* Get transactions from history file */
	        /* Skip those already retrieved if retrieving only unretrieved keywords */
	        if ((len = history_read(history_line, file_key, &new_time)) == FAILURE) continue;

	        list[list_cnt] = malloc(sizeof(LIST));

	        if (len > 0)
	            strcpy3(list[list_cnt]->string, key, ":\n", history_line);
	        else
	            strcpy2(list[list_cnt]->string, key, "\n\n");

	        list[list_cnt++]->time = new_time;
	        fnd = DBYES;
	    } /* End if */
    } /* End while */

    /* Close the pipe command */
    pclose(po);

    if (fnd == DBNO)
    {
	    if (gu.req_type == DBQUERY_KEYWORD)
	        strcpy3(rtn.msg, "All data for personal secure keywords sent to ", gu.email1, " retrieved.");
	    else
	        sprintf(rtn.msg, no_keyword_wrn, gu.email1, server.site);

	    shut_down(rtn.msg, NO_KEYWORD_WRN);
	    exit(0);
    }
    return(list_cnt);
} /* end long_external_secure_keywords_read */


/* Sort list of keywords retrieved */
void trans_keywords_sort(int list_cnt)
{
    LIST *tmp_ptr;
    int cnt;
    int cnt1;
    int large_cnt;
    int top_cnt = 0;

    DBG();

    for (cnt = 0; cnt <list_cnt; ++cnt)
    {
	    /* Point to the top of the list to start */
	    large_cnt = top_cnt;

	    for (cnt1 = top_cnt; cnt1 < list_cnt; ++cnt1)
	    {
	        if (list[cnt1]->time > list[large_cnt]->time)
	            large_cnt = cnt1;
	    } /* end of inner for */

	/* At this point we have an index to the next highest time */
	/* We must put the new large value at the top of the list */
	tmp_ptr = list[top_cnt];
	list[top_cnt] = list[large_cnt];
	list[large_cnt] = tmp_ptr;

	/* Point to the new top of the list */
	++top_cnt;

    } /* End big for */
} /* end trans_keywords_sort */


void trans_keywords_send(char *file_name, int list_cnt)
{
    int fnd = DBYES;
    int cnt;
    int ack = DBNO;
    int large_cnt;
    int top_cnt;

    DBG("file_name='%s'", file_name);

    /* Tell user data will be sent */
    put_short(SUCCESS);
    put_string(file_name);

    /* Get ack from client */
    if (get_short() == SUCCESS)
    {
	    ack = DBYES;

        /* Put as much data as possible in rtn.msg */
        if (list_cnt > 4) large_cnt = 4;
        else large_cnt = list_cnt;

        top_cnt = rtn.len;

        /* First put on a header */
	    for (cnt = 0; cnt < large_cnt; ++cnt)
        {
	        rtn.len += strcpy1(rtn.msg+rtn.len, list[cnt]->string);
	        free(list[cnt]);
        }

        put_binary_string(rtn.msg+top_cnt, rtn.len-top_cnt);
        for (cnt = large_cnt; cnt < list_cnt; ++cnt)
        {
	        put_string(list[cnt]->string);
	        free(list[cnt]);
        }

	    /* Tell client all data for this file sent */
	    put_short(SUCCESS);

    if (ack == DBNO);
    if ((fnd == DBYES) && (ack == DBYES))
    {
	    if (list_cnt >= 4)
	        sprintf(rtn.msg+rtn.len, "\nComplete list of keywords saved in file '%s'.", file_name);
	    else
	        sprintf(rtn.msg+rtn.len, "\nList of keywords found saved in file '%s'.", file_name);
    } /* End if for keywords found in file */

    put_success_msg(NONE);
    } else {
	    strcpy3(rtn.msg, "Retrieval of '", file_name, "' aborted.");
        put_success_msg(NONE);
	    exit(0);
    } /* end else */
} /* end trans_keywords_send */


/* Retrieve long listing of file keywords */
void long_file_keys_rtv(void)
{
    int list_cnt = 0;

    DBG();

    list_cnt = long_file_keywords_read();

    /* Sort list of keywords retrieved */
    trans_keywords_sort(list_cnt);

    /* Send keywords to the user */
    trans_keywords_send("file_keywords", list_cnt);
} /* end long_file_keys_rtv */


/* Retrieve long list of secure keywords for internal customer */
void long_secure_keys_rtv(void)
{
    int list_cnt = 0;

    DBG();

    list_cnt = long_secure_keywords_read();

    /* Sort list of keywords retrieved */
    trans_keywords_sort(list_cnt);

    /* Send keywords to the user */
    trans_keywords_send("secure_keywords", list_cnt);

} /* end long_secure_keys_rtv */


/* Retrieve long listing of secure keywords for external customers */
void long_external_secure_keys_rtv(void)
{
    int list_cnt = 0;

    DBG();

    list_cnt = long_external_secure_keywords_read();

    /* Sort list of keywords retrieved */
    trans_keywords_sort(list_cnt);

    /* Send keywords to the user */
    trans_keywords_send("secure_keywords", list_cnt);
} /* end long_external_secure_keys_rtv */


/* Modify keyword file to indicate data retrieved */
void keyword_mod(void)
{
    int index;
    extern char global_secure_keyword[];
    FILE *fi;
    FILE *fo;
    int ret;
    int fnd = DBNO;
    int rtv_num;
    long time_stamp;
    char secure_keyword[10];
    char file_keyword[10];
    char email[64];
    char tmp_file[64];
    char line[260];

    DBG();

    index = lock_file_set(KEYWORD_FILE);

    /* Open keyword file */
    if ((fi = fopen(KEYWORD_FILE, "r")) == NULL)
    {
	    lock_file_clear(index);
	    close_down1m(rd_open_err, KEYWORD_FILE, RD_OPEN_ERR);
    }

    /* build tmp file name */
    strcpy5(tmp_file, "db_admin/", user.login, "_", tr.pid_string, ".tmp_keywords");

    /* Open tmp file */
    if ((fo = fopen(tmp_file, "w")) == NULL)
    {
	    lock_file_clear(index);
	    close_down1m(wr_open_err, tmp_file, WR_OPEN_ERR);
    }

    /* Read through keyword until keyword found */
    while (line_get(fi, line) > EOF)
    {
	    /* if match found modify entry and indicate match found */
	    if (strncmp(global_secure_keyword, line, 9) == 0)
	    {
	        fnd = DBYES;

	        /* Scan line into parameters */
	        sscanf(line, "%s %s %s %d %ld", secure_keyword, file_keyword, email, &rtv_num, &time_stamp);

	        /* Increment number of times data retrieved */
	        ++rtv_num;

	        /* Write out new parameters to output file */
	        fprintf(fo, "%s %s %s %d %ld\n", secure_keyword, file_keyword, email, rtv_num, time_stamp);

	        break;
	    } /* End if for match found */
	    else fprintf(fo, "%s\n", line);

    } /* End while */

    /* Write rest of data out to file */
	while ((ret = fread(bf.buffer, sizeof(*bf.buffer), BUFFER_SIZE, fi)) != 0)
	    fwrite(bf.buffer, sizeof(*bf.buffer), ret, fo);

    /* Close files */
    fclose(fi);
    fclose(fo);

    /* If match found overwrite keyword file */
    if (fnd == DBYES)
    {
	    rename(KEYWORD_FILE, "db_admin/tmp_files/keyword_file");
        rename(tmp_file,  KEYWORD_FILE);
    }
    else unlink(tmp_file);

    lock_file_clear(index);
} /* End keyword_mod */

