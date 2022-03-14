#ifndef TRANSAPI
#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	company_lookup
 *
 * Description:	Looks up IP node address, machine name, and socket 
 *		        for specified company.
 *
 ***************************************************************************/

#include "scan_prototypes.h"
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include "global_params.h"
#include "server_params.h"
#include "global_defines.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "company.h"
#include "transapi_defines.h"

#include "global_debug.h" // Mazieri's debug facilities

/* Finds machine name IP address and socket for specified company */
/* Used with the TransWeb web page */
int transweb_company_find(char *company, char *is_foundry)
{
#ifdef NXP
#ifdef INTERCOM
    extern char remote_company_email[];
#endif
#endif
    int cnt;

    /* Look for specified company */
    /* Read through the table looking for specified company */
    for (cnt = 0; site[cnt].abbreviation[0] != '\0'; ++cnt)
	{
	    if (strcmp(company, site[cnt].name) == 0)
	    {
		strcpy(server.address, site[cnt].address);
	strcpy(server.machine, site[cnt].machine);
	server.socket = htons(site[cnt].socket);
		strcpy(server.site, site[cnt].name);
#ifdef INTERCOM
#ifdef NXP 
	strcpy(remote_company_email, site[cnt].email);
	strcpy(is_foundry, site[cnt].is_foundry);
#endif
#endif
		return(1);
	    } /* End if for company found */
	} /* End for */
     /* If reach this point, company not found */
    return(0);
} /* end transweb_company_find */

void repository_find(char extension)
{
    int cnt;
    extern TRANS_REPOSITORY repository[];
    DBG();

    /* Look for specified site */
    /* Read through the table looking for specified site */
    for (cnt = 0; repository[cnt].extension != '\0'; ++cnt)
	{
	    if ((extension == repository[cnt].extension) 
	    || (extension == repository[cnt].secure_extension))
	    {
		strcpy(server.address, repository[cnt].address);
	strcpy(server.machine, repository[cnt].machine);
	server.socket = htons(repository[cnt].socket);
		strcpy(server.site, repository[cnt].name);
		return(1);
	    } /* End if for site found */
	} /* End for */
     /* If reach this point, site not found */
    return(0);
} /* end repository_find */

int company_find(char *extension, char *is_foundry)
{
    int len;
    int cnt;

    /* Look for specified company */
    /* Read through the table looking for specified company */
    len = strlen(extension);
    if (len > 0)
    {
	str_lower(extension);
    for (cnt = 0; site[cnt].abbreviation[0] != '\0'; ++cnt)
	{
	    if ((strncmp(extension, site[cnt].abbreviation, 3) == 0)
	    || (strcasecmp(extension, site[cnt].name) == 0))
	    {
		strcpy(server.address, site[cnt].address);
	strcpy(server.machine, site[cnt].machine);
	server.socket = htons(site[cnt].socket);
		strcpy(server.site, site[cnt].name);
#ifdef INTERCOM
#ifdef NXP 
	strcpy(remote_company_email, site[cnt].email);
	strcpy(is_foundry, site[cnt].is_foundry);
#endif
#endif
		return(1);
	    } /* End if for company found */
	} /* End for */
     } /* End if */
     /* If reach this point, company not found */
    return(0);
} /* end company_find */
#endif /* for not TRANSAPI */
#endif /* For not TRANSAPI */
