#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	list_get, list_put, list_add, list_delete, etc.
 *
 * Description:	Basic functions to manipulate lists.
 *		Files to send are passed in a file array with one or more elements.
 *		It is assumed that the list is NULL terniated and that
 *		the number of elements on the list is known.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#ifndef WINDOWS_NT4
#include <alloca.h>
#endif
#include <string.h>
#include <unistd.h>
#include "client_prototypes.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "strcpy.h"
#include "rtn.h"
#include "server_lib.h"
#include "ssl_client_lib.h"
#include "trans_defines.h"
#include "trans_prototypes.h"

#include "global_debug.h"



/* Get a list of file list */
int list_put(char *files[])
{
    DBG();
    int cnt;
    short len;
    short get_ssl_string();
     char file_name[64];

    /* Retrieve list in loop */
    for (cnt = 0; (len = get_ssl_string(file_name)) > 0; ++cnt)
    {
	files[cnt] = malloc(len+1);
	strcpy(files[cnt], file_name);
    } /* End for */
    /* Point last pointer at NULL */
    files[cnt] = NULL;
    return(cnt);
} /* End list_put */

/* Send list of list to client */
int list_get(char *files[])
{
    DBG();
    int cnt;

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	put_ssl_string(files[cnt]);
    free(files[cnt]);
    } /* End for */
    /* Send indication all list retrieved */
    put_ssl_short(SUCCESS);
    return(cnt);
} /* End list_get */

int static_list_get(char *files[])
{
    DBG();
    int cnt;

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	put_ssl_string(files[cnt]);
    } /* End for */
    /* Send indication all list retrieved */
    put_ssl_short(SUCCESS);
    return(cnt);
} /* End static_list_get */

int static_root_list_get(char *files[])
{
    DBG();
    char *ptr;
    int cnt;

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	if ((ptr = strrchr(files[cnt], '/')) != NULL)
	    put_ssl_string(ptr+1);
	else put_ssl_string(files[cnt]);
    } /* End for */
    /* Send indication all list retrieved */
    put_ssl_short(SUCCESS);
    return(cnt);
} /* End static_list_get */

/* Delete files no longer needed */
void files_dlt(char *dir, char *files[])
{
    DBG();
    int cnt;
    char file_name[FILE_SIZE];

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	strcpy3(file_name, dir, "/", files[cnt]);
	unlink(file_name);
	free(files[cnt]);
    } /* End for */
} /* End files_dlt */

/* Read list of list from specified file */

/* Add an entry to the list */
/* Names are kept in alphabetical order */
/* Returns 1 if entry added */
/* Returns 0 if current entry replaced */
/* Returns SUCCESS if name added */
int list_add(char *list[], char *entry, char *key, int total_cnt)
{
    DBG();
    char fnd = DBNO;
    int ret;
    int cnt;
    int tmp_cnt;
    int len = strlen(key);

    /* Allocate space for new name */
    char *ptr = malloc(strlen(entry) +1);
    strcpy(ptr, entry);
    /* Find position for new name */
    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	if ((ret = strncmp(key, list[cnt], len)) < 0) 
	{
	    fnd = DBYES;
	    break;
	} /* End if */
	/* If equal, just clear list */
	else if (ret == 0)
	{
	    /* Replace existing entry with new entry */
	    free(list[cnt]);
	    list[cnt] = ptr;
	return(0);
	} /* end if for entry the same */
    } /* End for */
    if (fnd == DBNO)
    {
	list[cnt] = ptr;
	list[cnt+1] = NULL;
    }
    /* Move all entries down and insert new entry */
    else
    {
	for (tmp_cnt = total_cnt; tmp_cnt >= cnt; --tmp_cnt)
    {
	list[tmp_cnt] = list[tmp_cnt-1];
    }
	/* Fill hole with new entry */
	list[cnt] = ptr;
	/* Put NULL at new end of list */
	list[total_cnt+1] = NULL;
    } /* end else */
    return(1);
} /* End list_add */

/* Delete name from  list */
/* Returns SUCCESS if name deleted */
/* Returns FAILURE if name not present */
int list_delete(char *list[],  char *key, int total_cnt)
{
    DBG();
    int tmp_cnt;
    int cnt;
    int fnd = DBNO;

    /* Search for entry in list */
    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	if (strcmp(list[cnt], key) == 0) 
	{
	    fnd = DBYES;
	    break;
	}
    } /* end for */
    /* Delete entry from list */
    /* If entry not found, simply return */
    if (fnd != DBYES) return(-1);
    /* Set total cnt at new end of list */
    --total_cnt;
    /* Free the entry no longer needed */
    free(list[cnt]);
    for (tmp_cnt = cnt; tmp_cnt < total_cnt; ++tmp_cnt)
    list[tmp_cnt] = list[tmp_cnt+1];
    /* Put NULL at end of new list */
    list[total_cnt] = NULL;
    return(total_cnt);
} /* End list_delete */

/* Verify specified name is in the list array */
int list_find(char *list[], char *key)
{
    DBG();
    int cnt;
    int len = strlen(key);

    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	/* If entry greater than one searching for found, entry is not present */
	if (strncmp(list[cnt], key, len)  > 0) return(SUCCESS);
    }
    /* If reach this point name not found */
    return(FAILURE);
} /* end list_find */

/* Find the extension ratio in the list */
float list_ratio_find(char *file_name)
{
    DBG();
    extern char *extension_list[];
    extern char *ratio_list[];
    int cnt;
    int ret;
    float ratio;
    float default_ratio;
    char *file_ptr;

    return(1);
    /* Default ratio is at the top of the list */
    sscanf(ratio_list[0], "%f", &default_ratio);
    if ((file_ptr = strrchr(file_name, '.')) == NULL)
    return(default_ratio);
    for (cnt = 0; extension_list[cnt] != NULL; ++cnt)
    {
	if ((ret = strcasecmp(extension_list[cnt], file_ptr)) == 0)
	{
	    sscanf(ratio_list[cnt], "%f", &ratio);
	    return(ratio);
	} /* End if */
	/* If entry greater than one searching for found, entry is not present */
	else if (ret > 0)
	return(default_ratio);
    }
    /* If reach this point name not found */
    return(default_ratio);
} /* end list_ratio_find */

/* Expand a technology list */
int tech_list_expand(char *dir, char *old_files[], char *new_files[])
{
    DBG();
    char db_dir[FILE_SIZE];
    int len;
    int cnt;
    int cnt1 = 0;
    int dir_len = strlen(dir)+2;

     for (cnt = 0; old_files[cnt] != NULL; ++cnt)
    {
	len = strlen(old_files[cnt]) + dir_len;
	new_files[cnt1] = malloc(len);
	strcpy3(new_files[cnt1++], dir, "/", old_files[cnt]);
	/* Make sure the .db directory is present before adding it */
	strcpy4(db_dir, dir, "/", old_files[cnt], "/.db");
	if (access(db_dir, 0) == 0) 
	{
	    new_files[cnt1] = malloc(len+4);
	    strcpy4(new_files[cnt1++], dir, "/", old_files[cnt], "/.db");
	} /* End if for .db directory present */
	free(old_files[cnt]);
    } /* End for */
     new_files[cnt1] = NULL;
    return(cnt1);
} /* End tech_list_expand */

/* Convert list from string format to list format */
int string_list_convert(char *string, char *list[])
{
    DBG();
    int cnt;
    int len;
    char list_item[32];
    char *ptr = string;

    for (cnt = 0; sscanf(ptr, "%s", list_item) == 1; ++cnt)
    {
	len = strlen(list_item)+1;
	list[cnt] = malloc(len);
	strcpy(list[cnt], list_item);
	ptr += len;
    }
    /* Put null at end of string */
    list[cnt] = NULL;
    return(cnt);
} /* End string_list_convert */

/* convert list to string */
void list_string_convert(char *list[], char *string)
{
    DBG();
    char *ptr = string;
    int cnt;

    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	ptr += strcpy2(ptr, list[cnt], " ");
	free(list[cnt]);
    } /* End for */
} /* End list_string_convert */

void static_list_string_convert(char *list[], char *string)
{
    DBG();
    char *ptr = string;
    int cnt;

    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	ptr += strcpy2(ptr, list[cnt], " ");
    } /* End for */
} /* End list_string_convert */

/* Read list of list from specified file */
int list_read(char *file_name, char *list[])
{
    DBG();
    FILE *fp;
    int cnt;
    int ret;
    char line[260];

    /* Open the file for reading */
    if ((fp = fopen(file_name, "r")) == NULL)
    {
	list[0] = NULL;
	return(0);
    }
    /* Get list in for loop */
    for (cnt = 0; (ret = line_get(fp, line)) > 0; ++cnt)
    {
	list[cnt] = malloc(ret +1);
	strcpy(list[cnt], line);
    }
    /* Terminate list with NULL */
    list[cnt] = NULL;
    /* Close file no longer needed */
    fclose(fp);
    return(cnt);
} /* end list_read */

/* Count elements on a list */
/* Returns number of elements found */
int list_cnt(char *list[])
{
    DBG();
    int cnt;

    for (cnt = 0; list[cnt] != NULL; ++cnt);
    return(cnt);
}

/* Frees all entries on specified list */
void list_free(char *list[])
{
    DBG();
    int cnt;

    for (cnt = 0; list[cnt] != NULL; ++cnt)
    free(list[cnt]);
    list[0] = NULL;
} /* End list_free */

/* Convert string to list format */
/* Items on list are separated by the character specified */
int string2double_list_convert(char *string, char *extension_list[], char *ratio_list[])
{
    DBG();
    int cnt;
    int len = 0;
    char item1[20];
    char item2[16];

    for (cnt = 0; (sscanf(string+len, "%s %s", item1, item2) > 0); ++cnt)
    {
	extension_list[cnt] = malloc(strlen(item1) + 1);
	ratio_list[cnt] = malloc(strlen(item2) + 1);
	len += strcpy1(extension_list[cnt], item1);
	len += strcpy1(ratio_list[cnt], item2);
	/* Add two more characters for the space and carriage return */
	len += 2;
    }
    /* Put null at end of lists */
    extension_list[cnt] = NULL;
    ratio_list[cnt] = NULL;
    return(cnt);
} /* End string2double_list_convert */

/* find ratio for specified file type */
#ifndef TRANSAPI
float list_ratio_rtv(char *file_name)
{
    DBG();
    char *ptr;
    float ratio;

    /* First find the file extension */
        ptr = strrchr(file_name, '.');
    if (ptr == NULL)
    {
	printf("no extension present\n");
	exit(1);
    }
    /* Now get the list from the server */
    dbsend_list_extension_request();
    /* Now look for the extension in the list */
    ratio = list_ratio_find(ptr);
    exit(0);
}
#endif
#endif /* For not TRANSAPI */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
