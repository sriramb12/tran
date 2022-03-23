/****************************************************************************
 *
 * Function:	Group of functions that operates on a file list.
 *		The file list is an array of pointers.
 *		The last pointer points to NULL.
 *
 * Description:	See below.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "global_defines.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities



static int compareStrings (const void * a, const void * b)
{
    return strcmp (*(const char **) a, *(const char **) b);
}


static int compareCaseStrings (const void * a, const void * b)
{
    return strcasecmp (*(const char **) a, *(const char **) b);
}


void files_remove(char *files[], char *dir)
{
    char file_name[FILE_SIZE];
    char *file_ptr;
    int cnt;

    DBG();

    file_ptr = file_name + strcpy2(file_name, dir, "/");

    /* Read through the list of file names */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    /* Build complete file name */
	    strcpy(file_ptr, files[cnt]);

	    /* Delete the file */
	    unlink(file_name);
    } /* end for */
} /* end files_remove */


/* Puts file names in a string */
/* Each file name is separated by a space */
void files_convert2string(char *files[], char *string)
{
    int cnt;
    int len = 0;

    DBG();

    for (cnt = 0; files[cnt] != NULL; ++cnt)
        len += strcpy2(string+len, files[cnt], " ");
} /* End files_convert2string */


/* Puts file names in string into a file list */
void files_convert_from_string(char *files[], char *string)
{
    int cnt;
    int tmp_len;
    int len = 0;
    char tmp_string[32];

    DBG();

    while (sscanf(string+len, "%s", tmp_string) > 0)
    {
	    tmp_len = strlen(tmp_string) +1;
	    files[cnt] = malloc(tmp_len);
	    strcpy(files[cnt], tmp_string);
	    len += tmp_len;
    } /* End for */

    /* Point the pointer at the end of the list to NULL */
    files[cnt] = NULL;
} /* End files_convert_from_string */


void temp_files_sort(char *files[])
{
    int cnt;
    int balance = 0;
    tree_node *tree_top = NULL;

    DBG();

    /* Build a sorted binary tree holding file names */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
	    add_node(&tree_top,files[cnt],&balance);
    } /* end for */

    /* At this point, a sorted balanced binary tree with file names exists */
    /* Free the nodes and build the sorted list */
    cnt = 0;
    free_bin_tree(&tree_top, files, &cnt);
} /* end files_sort */


void files_sort(char *files[])
{
    int total_cnt = 0;
    DBG();
    for (total_cnt = 0; files[total_cnt] != NULL; ++total_cnt);
    if (total_cnt > 1)
    {
        qsort (files, total_cnt, sizeof (char *), compareStrings);
    }
}  /* End files_sort */


void files_case_sort(char *files[])
{
    int total_cnt = 0;
    DBG();
    for (total_cnt = 0; files[total_cnt] != NULL; ++total_cnt);
    if (total_cnt > 1)
    {
        qsort (files, total_cnt, sizeof (char *), compareCaseStrings);
    }
}  /* End files_case_sort */


/* Free null terminate file list */
void files_free(char *files[])
{
    int cnt;
    DBG();

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
        free(files[cnt]);
        files[cnt] = 0;
    }
}
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
