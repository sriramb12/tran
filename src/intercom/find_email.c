/****************************************************************************
 *
 * Function:	find_email
 *
 * Description:	Finds user's Email address in registration file.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include "external_intercom_prototypes.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_paths.h"
#include "rtn.h"
#include "strcpy.h"
#include "global_debug.h"

int find_email(user_site, email_addr, address_type)
    char *user_site;	/* site of user looking for */
    char *email_addr;	/* Email address returned to calling function */
    char address_type;	/* Indicates official or unofficial Email address */
{
    char email_addr1[48];
    char email_addr2[48];
    FILE *fp;
    int len;
    char fnd = DBNO;

    DBG();

    /* build name for mail registration file */
    /* Path has the final dot at the end */
    len = strcpy1(bf.file_name, SITE_REG_PATH);
    bf.file_name[len++] = user_site[0];
    bf.file_name[len++] = user_site[1];
    bf.file_name[len] = '\0';
    /* Open the mail registration file */
    if ((fp = fopen(bf.file_name, "r")) == NULL)
    {
	/* Put in my Email and the user's name */
	strcpy(email_addr, "transbug@nxp.com");
	strcpy2(rtn.msg, rd_open_err, bf.file_name);
    DBG("%s" , rtn.msg);
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
	    strcpy1(email_addr+len, "@email");
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
    DBG("%s", rtn.msg);
	return(FAILURE);
    }
    else return(SUCCESS);
} /* End find_email */
