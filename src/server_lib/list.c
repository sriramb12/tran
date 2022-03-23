/****************************************************************************
 *
 * Function:	list_get, list_put, m2list_add, m2list_delete, etc.
 *
 * Description:	Basic functions to manipulate lists.
 *		Files to send are passed in a file array with one or more elements.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <unistd.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "strcpy.h"
#include "rtn.h"
#include "server_lib.h"
#include "global_debug.h" // Mazieri's debug facilities


/* Get a list of file list */
int list_put(char *files[])
{
    int cnt;
    short len;
    char file_name[PATH_SIZE];

    DBG();

    /* Retrieve list in loop */
    for (cnt = 0; (len = get_string(file_name)) > 0; ++cnt)
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
    int cnt;
    
    DBG();

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    put_string(files[cnt]);
	    free(files[cnt]);
    } /* End for */

    /* Send indication all list retrieved */
    put_short(SUCCESS);

    return(cnt);
} /* End list_get */


/* Send list of list to client */
/* End each item on list with carriage return */
int list_cr_get(char *files[])
{
    char string[260];
    int cnt;

    DBG();

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    strcpy2(string, files[cnt], "\n");
	    put_string(string);
	    free(files[cnt]);
    } /* End for */

    /* Send indication all list retrieved */
    put_short(SUCCESS);
    
    return(cnt);
} /* End list_get */


int static_list_get(char *files[])
{
    int cnt;

    DBG();

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    put_string(files[cnt]);
    } /* End for */

    /* Send indication all list retrieved */
    put_short(SUCCESS);

    return(cnt);
} /* End list_get */


/* Delete files no longer needed */
void files_dlt(char *dir, char *files[])
{
    int cnt;
    char file_name[FILE_SIZE];

    DBG();

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    strcpy3(file_name, dir, "/", files[cnt]);
	    unlink(file_name);
	    free(files[cnt]);
        files[cnt] = 0;
    } /* End for */
} /* End files_dlt */


/* Read list of list from specified file */
/* Returns number of elements found */
int list_read(char *file_name, char *list[])
{
    FILE *fp;
    int cnt;
    int ret;
    char line[260];

    DBG();

    /* Open the file for reading */
    if ((fp = fopen(file_name, "r")) == NULL)
        close_down1m(rd_open_err, file_name, RD_OPEN_ERR);

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


/* Write list of list to specified file */
int list_write(char *file_name, char *list[])
{
    FILE *fp;
    int cnt;

    DBG();

    /* Open the file for write */
    if ((fp = fopen(file_name, "w")) == NULL)
        close_down1m(wr_open_err, file_name, WR_OPEN_ERR);

    /* Get list in for loop */
    for (cnt =0; list[cnt] != NULL; ++cnt)
    {
	    fprintf(fp, "%s\n", list[cnt]);
	    free(list[cnt]);
    }

    /* Close file no longer needed */
    fclose(fp);

    return(cnt);
} /* end list_write */


/* Add an entry to the list */
/* Names are kept in alphabetical order */
/* Returns 1 if entry added */
/* Returns 0 if current entry replaced */
/* Returns SUCCESS if name added */
int m2list_add(char *list[], char *entry, char *key, int total_cnt)
{
    char fnd = DBNO;
    int ret;
    int cnt;
    int tmp_cnt;
    int len = strlen(key);

    DBG();

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
	    return(1);
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
int m2list_delete(char *list[],  char *key, int total_cnt)
{
    int tmp_cnt;
    int cnt;
    int fnd = DBNO;

    DBG();

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
/* Returns SUCCESS if item found, otherwise returns FAILURE */
int list_find(char *list[], char *key)
{
    int cnt;
    int len = strlen(key);

    DBG();

    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	    /* If entry greater than one searching for found, entry is not present */
	    if (strncmp(list[cnt], key, len)  == 0) return(SUCCESS);
    }

    /* If reach this point name not found */
    return(FAILURE);
} /* end list_find */


/* Verify specified name is in the list array */
/* Returns SUCCESS if item found, otherwise returns FAILURE */
int list_find_complete(char *list[], char *key)
{
    int cnt;

    DBG();

    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	    /* If entry greater than one searching for found, entry is not present */
	    if (strcmp(list[cnt], key)  == 0) return(SUCCESS);
    }

    /* If reach this point name not found */
    return(FAILURE);
} /* end list_find_complete */


/* Expand a technology list */
int tech_list_expand(char *dir, char *old_files[], char *new_files[])
{
    char db_dir[FILE_SIZE];
    int len;
    int cnt;
    int cnt1 = 0;
    int dir_len = strlen(dir)+2;

    DBG();

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


/* convert string to list format */
/* Items on string are separated by spaces */
int string2list_convert(char *string, char *list[])
{
    int cnt;
    int len;
    char list_item[1024];
    char *ptr = string;

    DBG();

    /* Add a space at the end of the e-mail address */
    strcat(string, " ");

    for (cnt = 0; sscanf(ptr, "%s", list_item) == 1; ++cnt)
    {
	    len = strlen(list_item)+1;
	    list[cnt] = malloc(len);
	    strcpy(list[cnt], list_item);
	    ptr += len;
	    
	    if (ptr == NULL) break;
    }

    /* Put null at end of string */
    list[cnt] = NULL;

    return(cnt);
} /* End string2list_convert */


/* Convert string to list format */
/* Items on list are separated by the character specified */
int string2list2convert(char *string, char *list[], char c)
{
    int cnt;
    int len;
    char list_item[64];
    char *ptr = string;

    DBG();

    for (cnt = 0; sscanf(ptr, "%[^,]", list_item) == 1; ++cnt)
    {
	    len = strlen(list_item)+1;
	    list[cnt] = malloc(len+1);
	    strcpy2(list[cnt], list_item, ",");
	    ptr += len;
    }

    /* Put null at end of string */
    list[cnt] = NULL;

    return(cnt);
} /* End string2list2convert */


/* convert list to string */
/* Items in new string are separated by spaces */
void list2string_convert(char *list[], char *string)
{
    char *ptr = string;
    int cnt;

    DBG();

    for (cnt = 0; list[cnt] != NULL; ++cnt)
    {
	    ptr += strcpy2(ptr, list[cnt], " ");
	    free(list[cnt]);
    } /* End for */
} /* End list2string_convert */


/* Reverse order of items on the list */
void m2list_reverse(char ***new_list, char **old_list, int num)
{
    int new_cnt;
    int old_cnt;

    DBG();

    /* Allocate space for items on new list */
    *new_list = malloc(sizeof(char *)*(num+1));

    for (new_cnt = 0, old_cnt = num-1; old_cnt > -1; ++new_cnt, --old_cnt)
	    (*new_list)[new_cnt] = old_list[old_cnt];

    (*new_list)[num] = NULL;
} /* End list_reverse */


/* Compares two lists to see if equal */
int list_compare(char *list1[], char *list2[])
{
    int ret = SUCCESS;
    int cnt1;
    int cnt2;

    DBG();

    /* First make sure lists have same number of elements */
    for (cnt1 = 0; list1[cnt1] != NULL; ++cnt1);
        for (cnt2 = 0; list2[cnt2] != NULL; ++cnt2);
            if (cnt1 != cnt2) 
            {
	            m2list_free(list1);
	            m2list_free(list2);
	            return(FAILURE);
            } /* End if */

    /* Now compare each item in two lists */
    for (cnt1 = 0; list1[cnt1] != NULL; ++cnt1)
    {
	    if (strcmp(list1[cnt1], list2[cnt1]) != 0) 
	    {
	        ret = FAILURE;
	        break;
	    } /* End if */
    }

    m2list_free(list1);
    m2list_free(list2);

    return(ret);
} /* End list_compare */


void m2list_free(char *list[])
{
    int cnt;

    DBG();

    for (cnt = 0; list[cnt] != NULL; ++cnt)
        free(list[cnt]);

    list[0] = NULL;
} /* End m2list_free */


/* Does binary search on list */
int list_binary_search(char *list[], char *name, int num)
{
    int ret;
    int center;
    int lower = 0;
    int upper = num-1;

    DBG();

    /* Do binary search until just 3 elements left */
    while (num > 3)
    {
        center = num/2 + num%2;
	    center += lower;
	    ret = strcmp(list[center], name);
	
	    /* If found, break out of loop */
	    if (ret == 0) return(SUCCESS);
	    /* If less than center, make center new lower limit */
	    else if (ret < 0) lower = center + 1;
	    /* If greater just modify center */
	    else upper = center-1;

	    num = upper-lower;
    } /* end while */
    
    for (center = lower; center <= upper; ++center)
    {
	    if (strcmp(list[center], name) == 0)
	        return(SUCCESS);
    } /* end for */

    return(FAILURE);
} /* End list_binary_search */

#ifndef M2
#ifndef OPCTRANS
void list_extensions_rtv(void)
{
    int fd;
    int ret;

    DBG();

    if ((fd = open("db_admin/file_extension_ratios", O_RDONLY)) < 0)
        strcpy(rtn.msg, "default 3\n");
    else {
	    ret = read(fd, rtn.msg, BLOCK_SIZE);
	    rtn.msg[ret] = '\0';
	    close(fd);
    }

    /* Send data to client */
    put_short(SUCCESS);
    put_string(rtn.msg);

    strcpy(rtn.msg, "List retrieved");

    put_success_msg(NONE);
} /* End list_extensions_rtv */
#endif
#endif
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
