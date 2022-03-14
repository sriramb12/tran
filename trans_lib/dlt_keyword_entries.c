/****************************************************************************
 *
 * Function:	dlt_keyword_entries
 *
 * Description:	Deletes entries for specified key from keyword file.
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>

#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "strcpy.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_server_macros.h"
#include "trans_lib_prototypes.h"
#include "user_params.h"
#include "trans_server_defines.h"
#include "global_debug.h"


int dlt_keyword_entries(void)
{
    int index;
    char line[260];
    int cnt = 0;
    char decrypted_keyword[12];
    char history_file[32];
    char *history_ptr;
    FILE *fi;
    FILE *fo;

    DBG();

    /* Build prefix for history file */
    history_ptr = strcpy1(history_file, HISTORY_FILE_PATH) + history_file;
    index = lock_file_set(KEYWORD_FILE);

    /* Open the keyword file */
    if ((fi = fopen(KEYWORD_FILE, "r")) == NULL)
    {
	    lock_file_clear(index);
	    return(0);
    }

    /* Build name for temporary file */
    strcpy5(bf.file_name, "tmp_files/", user.login, "_", "abcd1234", ".keyword");

    /* Open the temporary file */
    if ((fo = fopen(bf.file_name, "w")) == NULL)
    {
	    lock_file_clear(index);
        close_down1m(wr_open_err, bf.file_name, WR_OPEN_ERR);
    }

    while (line_get(fi, line) > 0)
    {
        if (sscanf(line, "%*s %s", decrypted_keyword) != 1) continue;
	
	    /* Build the history file name */
	    strcpy2(history_ptr, decrypted_keyword, ".history");

	    /* See if history file exists */
	    if (access(history_file, 0) == 0) {
	        fprintf(fo, "%s\n", line);
        } else {
	        /* Otherwise increment number of entries being deleted */
	        ++cnt;
        }
    } /* End while */

    /* Close the files */
    fclose(fi);
    fclose(fo);

    /* If an entry was deleted, write the new file over the old file */
    if (cnt > 0) {
        rename(bf.file_name, KEYWORD_FILE);
    } else {
        /* Otherwise simply delete temporary file created */
        unlink(bf.file_name);
    }

    lock_file_clear(index);

    /* Return the number of enntries deleted */
    return(cnt);
} /* End dlt_keyword_entries */
