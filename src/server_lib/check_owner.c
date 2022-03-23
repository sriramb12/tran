/****************************************************************************
 *
 * Function:	check_owner
 *
 * Description:	Checks to see if user owns data he is manipulating.
 *		Ownership verified with user ID and site.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "global_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h" // Mazieri's debug facilities

int check_owner(char *own_id, char *own_mach, char *own_addr, char *err_msg)
{
    char own_site[20];
    char user_site[20];

    DBG();

    /* If the user is the database administrator, he may perform the transaction */
    if ((strcmp(user.login, admin.login1) == 0) || (strcmp(user.login, admin.login2) == 0)) 
        return(SUCCESS);

    /* First verify the USER ID's are the same */
    if (strcmp(own_id, user.login) != 0)
    {
	    sprintf(bf.buffer, err_msg, own_id, own_mach, own_addr);
	    shut_down(bf.buffer, FAILURE);
    }

    /* Make sure the sites match */
    get_site(user.addr, user_site);
    get_site(own_addr, own_site);

    if (strcmp(own_site, user_site) != 0)
    {
	    sprintf(bf.buffer, err_msg, own_id, own_mach, own_addr);
	    shut_down(bf.buffer, FAILURE);
    } /* End if for owner and user site mismatch */

    return(SUCCESS);
} /* End check_owner */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
