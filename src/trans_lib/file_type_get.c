/****************************************************************************
 *
 * Function:	file_type_get_gzip_file
 *
 * Description:	Determine file type based on file suffix.
 *
 ***************************************************************************/

#include <string.h>

#include "global_defines.h"
#include "trans_defines.h"
#include "trans_server_globals.h"
#include "trans_lib_prototypes.h"
#include "file_type_arrays.h"
#include "global_debug.h"


char file_type_get(char *file_name)
{
    int cnt;
    int len;
    char *file_ptr;

    DBG();

    /* Get string length of file name */
    len = strlen(file_name);

    /* If name less than or equal to two characters, simply return */
    if (len <= 2) return(UNSPECIFIED);

    /* Point at last two characters of file name */
    file_ptr = file_name+len-2;

    /* Go through list of text files with two letter extension */
    for (cnt = 0; text2array[cnt][0] != '\0'; ++cnt)
        if (strcmp(file_ptr, text2array[cnt]) == 0) return(ASCII);

    /* Go through list of binary files with two letter extension */
    for (cnt = 0; binary2array[cnt][0] != '\0'; ++cnt)
        if (strcmp(file_ptr, binary2array[cnt]) == 0) return(BINARY);

    if (len <= 3) return(UNSPECIFIED);

    /* Point to last three characters of file name */
     --file_ptr;

    /* Go through three letter text extensions */
    for (cnt = 0; text3array[cnt][0] != '\0'; ++cnt)
        if (strcmp(file_ptr, text3array[cnt]) == 0) return(ASCII);

    /* Go through three letter binary extensions */
    for (cnt = 0; binary3array[cnt][0] != '\0'; ++cnt)
        if (strcmp(file_ptr, binary3array[cnt]) == 0) return(BINARY);

    if (len <= 4) return(UNSPECIFIED);
    
    /* Point to last four characters of file name */
    --file_ptr;

    /* Go through four letter text extensions */
    for (cnt = 0; text4array[cnt][0] != '\0'; ++cnt)
        if (strcmp(file_ptr, text4array[cnt]) == 0) return(ASCII);

    /* Go through four letter binary extensions */
    for (cnt = 0; binary4array[cnt][0] != '\0'; ++cnt)
        if (strcmp(file_ptr, binary4array[cnt]) == 0) return(BINARY);

    if (len <= 5) return(UNSPECIFIED);

    /* Go through five letter binary extensions */
    for (cnt = 0; binary5array[cnt][0] != '\0'; ++cnt)
        if (strcmp(file_ptr, binary5array[cnt]) == 0) return(BINARY);

    if (len <= 6) return(UNSPECIFIED);

    /* Go through six letter binary extensions */
    for (cnt = 0; binary6array[cnt][0] != '\0'; ++cnt)
        if (strcmp(file_ptr, binary6array[cnt]) == 0) return(BINARY);

    /* If reach this point, do not know file type */
    return(UNSPECIFIED);
} /* End file_type_get */

/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
