/****************************************************************************
 *
 * Function:	compares_site
 *
 * Description:
 *  Compares two sites passed in.
 *  The sites are broken into 3 octets and 
 *	a login name. If three of the four things match,
 *	SUCCESS is returned.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *  char *site      - User's new site
 *  char *email     - Address given by user
 *  REG_PARAMS *reg - Holds user's email if match found
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_prototypes.h"
#include "reg_globals.h"
#include "line.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

int compare_sites(char *site, char *email, REG_PARAMS *reg)
{
    char reg_site[64];
    char *reg_ptr;
    FILE *po;
    char login1[20];
    char first_octet1[4];
    char sec_octet1[4];
    char third_octet1[4];
    char login2[48];
    char first_octet2[4];
    char sec_octet2[4];
    char third_octet2[4];
    char command[64];
    char line[120];
    int cnt ;	/* Number of matches found */

    DBG("site='%s', email='%s'", site, email);

    /* build command to grep for sites in registry */
    strcpy3(command, "grep ", email, " registry/site*");

    /* Direct output into pipe */
    if ((po = popen(command, "r")) == NULL) return(DBNO);

    /* Break first site into its components */
    if (breakup_site(site, login1, first_octet1, sec_octet1, third_octet1) != SUCCESS) return(DBNO);

    /* Scan in matching lines in a while loop */
    while (line_get(po, line) > 0)
    {
	    /* Pull out site and Email addresses */
	    if (sscanf(line, "%s %*d %s %s", reg_site, reg->email1, reg->email2) != 3) continue;

	    cnt = strfind_char(reg_site, ':', 64);
	    if (cnt > -1)
	        reg_ptr = reg_site+cnt+1;

	    /* Break site  into its components */
	    if (breakup_site(reg_ptr, login2, first_octet2, sec_octet2, third_octet2) != SUCCESS) return(FAILURE);

	    /* Indicate no matches found */
	    cnt = 0;
	    if (strcmp(login1, login2) == 0) ++cnt;
	    else continue;

	    if (strcmp(first_octet1, first_octet2) == 0) ++cnt;

	    if (strcmp(sec_octet1, sec_octet2) == 0) ++cnt;

	    if (strcmp(third_octet1, third_octet2) == 0) ++cnt;
	
	    if (cnt >= 3)
	    {
	        /* close the stream */
	        pclose(po);
	        return(DBYES);
	    } /* End if for match found */
    } /* End while */

    /* Close the stream and return failure */
    pclose(po);
    return(DBNO);
} /* End compare_sites */

int breakup_site(char *site, char *login, char *octet1, char *octet2, char *octet3)
{
    int len1;
    int len2;
    int len3;

    DBG("site='%s', login='%s', octet1='%s', octet2='%s' and octet3='%s'", site, login, octet1, octet2, octet3);

    /* Copy site into login up to first period */
    len1 = strcpy_substr(login, site, '.', strlen(site));

    /* If less than 4 characters before '.', site is invalid */
    if (len1 < 4) return(FAILURE);

    /* Copy the first octet */
    /* It is the last three characters before the first period */
    strncpy(octet1, site+len1-3, 3);
    octet1[3] = '\0';

    /* Strip first octet off of login name */
    login[len1-3] = '\0';

    /* Copy the second octet */
    len2 = strcpy_substr(octet2, site+len1+1, '.', 4);

    /* octet must be 1 to 3 characters in length */
    if ((len2 < 1) || (len2 > 3)) return(FAILURE);

    /* Copy in the third octet */
    len3 = strcpy1(octet3, site+len1+len2+2);

    /* Third octet must be 1 to 3 characters in length */
    if ((len3 < 1) || (len3 > 3)) return(FAILURE);
    
    return(SUCCESS);
} /* End breakup_site */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
