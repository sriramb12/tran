/****************************************************************************
 *
 * Function:	tech_entry_add, tech_entry_delete
 *
 * Description:	Adds entry to global fab table when a technology is created.
 *		Deletes entry from fab table when technology deleted 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include "close_down.h"
#include "entry.h"
#include "lock_file.h"
#include "files.h"
#include "global_defines.h"
#include "m2req_types.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_prototypes.h"
#include "m2server_paths.h"
#include "mti.h"
#include "strcpy.h"

void tech_entry_add(char *fab, char *tech)
{
    char new_entry[260];

    /* Build the entry to add */
    strcpy3(new_entry, fab, ";", tech);
    entry_add(GLOBAL_TECH_FILE, new_entry, new_entry);
} /* end tech_entry_add */

/* Delete entry from fab table */
void tech_entry_delete(char *fab, char *tech)
{
    char key[KEY_LEN];

    strcpy3(key, fab, ";", tech);
    entry_delete(GLOBAL_TECH_FILE, key);
} /* End tech_entry_delete */

