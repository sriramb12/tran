/****************************************************************************
 *
 * Function:	is_keyword
 *
 * Description:	Verifies keyword is valid.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rtn.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"

#include "global_debug.h"


/* Returns 0 for valid keywords */
int is_keyword (REQ_TYPE  req_type, char *key) 
{
    DBG();

    short ret;

    /* Return success for "all" and NONE */
    if ((strcasecmp(key, "all") == 0)
    || (strcmp(key, NONE) == 0))
    return(SUCCESS);
    if ((req_type == 'g')
    && ((strncmp(key, "trans", 5) == 0)
    || (strcmp(key+strlen(key)-5, "trans") == 0)))
    return(SUCCESS);
    ret = strlen(key);
    if ((ret < 8) || (ret > 10)
    || ((ret == 9) && ((key[8] < 'a') && (key[8] > 'z')))
    || (key[0] < 'a') || (key[0] > 'z')
    || (key[1] < 'a') || (key[1] > 'z')
    || (key[2] < 'a') || (key[2] > 'z')
    || (key[3] < 'a') || (key[3] > 'z')
    || (key[4] < '0') || (key[4] > '9')
    || (key[5] < '0') || (key[5] > '9')
    || (key[6] < '0') || (key[6] > '9')
    || (key[7] < '0') || (key[7] > '9'))
    {
	sprintf(rtn.msg,invalid_trans_keyword, key);
	strcat(rtn.msg, "\n\n");
	return(INVALID_TRANS_KEYWORD);
    } /* End if */
    else return(SUCCESS);
} /* End is_keyword */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
