/****************************************************************************
 *
 * Function:	strip_email_trailors
 *
 * Description:	Strips extra text off end of Email address
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "global_server_errs.h"

#include "global_debug.h" // Mazieri's debug facilities


void strip_email_trailors(char *entry)
{
    char *strchr();
    char *ptr;

    DBG("entry='%s'", entry);

    ptr = strchr(entry, '.');
    
    if (ptr != NULL)
        ptr[0] = '\0';
}
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
