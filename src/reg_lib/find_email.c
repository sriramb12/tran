/****************************************************************************
 *
 * Function:	find_email
 *
 * Description:	Finds user's Email address in registration file.
 *
 * Author:	Renee Carter
 *
 * Parameters:
 *      char *user_site   - site of user looking for
 *      char *email_addr  - Email address returned to calling function
 *      char address_type - Indicates official or unofficial Email address
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "reg_globals.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

int find_email(char *user_site, char *email_addr, char address_type)
{
    char email_addr1[48];
    char email_addr2[48];
    FILE *fp;
    int len;
    char fnd = DBNO;
    
    DBG("user_site='%s', email_addr='%s', address_type='%s'", user_site, email_addr, address_type);

    /* build name for mail registration file */
    /* Path has the final dot at the end */
    reg_name_build(bf.file_name, user_site);

    /* Open the mail registration file */
    if ((fp = fopen(bf.file_name, "r")) == NULL)
    {
	    /* Put in my Email and the user's name */
	    strcpy(email_addr, "transbug@nxp.com");
	    strcpy2(rtn.msg, rd_open_err, bf.file_name);
	    data_log(tr.log_file, rtn.msg);
	    return(FAILURE);
    }

    /* Get length for comparison in following loop */
    len = strlen(user_site);

    /* Pull in lines of text until find match or reach end of file */
    while (fscanf(fp, "%s %*d %s %s", bf.buffer, email_addr1, email_addr2)!= EOF)
    {
	    /* See if user site matches site from table */
	    if (strncmp(user_site, bf.buffer, len) == 0)
	    {
	        fnd = DBYES;
	        /* Copy address to give to calling function */
	        /* if address type is 'd', official Email is used */
	        if (address_type == 'd') len = strcpy1(email_addr, email_addr1);
	        else len = strcpy1(email_addr, email_addr2);

	        /* If Email address not tacked on, put it on */
	        if (len < 8)
	            strcpy1(email_addr+len, "@nxp.com");
	    
	        break;
	    } /* end if */
    } /* end while */

    /* Close the mail registration file */
    fclose(fp);

    if (fnd == DBNO)
    {
	    /* Put in my Email and the user's name */
	    strcpy(email_addr, "transbug@nxp.com");
	    sprintf(rtn.msg, usr_not_fnd, user_site);
	    data_log(tr.log_file, rtn.msg);
	    
	    return(FAILURE);
    }
    else 
        return(SUCCESS);
} /* End find_email */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
