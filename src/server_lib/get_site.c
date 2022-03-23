/****************************************************************************
 *
 * Function:	get_site
 *
 * Description:	Takes the last octet off the IP node address.
 *		All users are identified by their login name
 *		and the first three octets of their IP node address.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *ip_addr -	IP address to be converted to site
 *  char *site) - Site returned to calling function
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_debug.h" // Mazieri's debug facilities

void get_site(char *ip_addr, char *site)
{
    DBG("ip_addr='%s' and site='%s'", ip_addr, site);

    char *site_ptr;

    strcpy(site, ip_addr);
    site_ptr = strrchr(site, '.');

    if (site_ptr != NULL)
        site_ptr[0] = '\0';
    else 
        log3(bad_addr, " ", site);
} /* End get_site */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
