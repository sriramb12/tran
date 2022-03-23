/****************************************************************************
 *
 * Function:	files_dlt
 *
 * Description:	Deletes specified files.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "files.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities

void tmp_files_dlt(DB_FILES *file)
{
    DBG();

    char file_name[FILE_SIZE];
    char *file_ptr;
    int cnt;

    file_ptr = file_name + strcpy2(file_name, file->dir, "/");
    /* Read through the list of file names */

    for (cnt = 0; cnt <file->num; ++cnt)
    {
	    /* Build complete file name */
	    strcpy(file_ptr, file->name[cnt]);

	    /* Delete the file */
	    unlink(file_name);
    } /* end for */
} /* end files_dlt */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
