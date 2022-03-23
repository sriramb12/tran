#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	edc_request
 *
 * Sends EDC request to server
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include "edc_defines.h"
#include "global_defines.h"
#include "global_params.h"
#include "rtn.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include <stdlib.h>
#include "trans_prototypes.h"
#include "ssl_client_lib.h"

#include "global_debug.h"


/* The trans structure is passed in with key1, par1, and par2 initialized */
int trans_edc_request(int index, REQ_TYPE req_type, char *key1, char *par2, char *par1)
{
    DBG();
    int ret;

    /* Initialize structure */
    trans_simple_basic_request_set(index, req_type, key1);
    /* Initialize extra parameters */
    strcpy(trans[index]->par1, par1);
    strcpy(trans[index]->par2, par2);
    /* Set the default repository */
    trans_default_repository_set();
    if ((ret = trans_socket_init(index)) < SUCCESS)
    return(ret);
    /* If this is for pending approvals, retrieve the approvals & display  */
    ret = trans_ret_code_get();
    return(ret);
} /* end trans_edc_request */

/* Send approval request to InterCom server */
/* Pass in e-mail address and description */
int trans_commandline_approve(int index, REQ_TYPE req_type, int argc, char **argv)
{
    DBG();

    short ret;
    int cnt = 0;
    DBG("index=%d, req_type=%i(APPROVAL=%i, STATUS=%i), argc=%d, argv[1]=%s, argv[2]=%s", index, req_type, DBREQUEST_APPROVAL, DBREQUEST_STATUS, argc, argv[1], argv[2]);

    /* Verify valid site name */
    if ((argc < 3)
    || (strchr(argv[2], '.')) == NULL)
    {
	sprintf(rtn.msg, domain_err, argv[2]);
	return(DOMAIN_ERR);
    }


    if (req_type == DBREQUEST_APPROVAL) 
	{
	    /* If requesting approval must have description */
            /* If description not entered, prompt user for it */
	    if (argc < 4)
	    {
		PRINTF("Enter justification for approval.\n> ");
		while (((trans[index]->par1[cnt] = getchar()) != '\n') 
		&& (trans[index]->par1[cnt] != EOF)
		&& (cnt < 159)) ++cnt;
		trans[index]->par1[cnt] = '\0';
		/* Put in carriage return for output separation */
		PRINTF("\n");
	      } /* end if for description needed */
	    else 
		strcpy(trans[index]->par1, argv[3]);
	}

    else if (req_type == DBREQUEST_STATUS)
    {
        /*STATUS do not have justification, just copy email addr*/
	strcpy(trans[index]->par1, argv[2]);
    }
    else {
	strcpy(trans[index]->par1, argv[3]);
    }

    DBG("after");

    /* Set the default repository */
    trans_default_repository_set();
    trans_simple_basic_request_set(index, req_type, argv[2]);
    if ((ret = trans_socket_init(index)) < SUCCESS)
    return(ret);
    trans_ret_code_get();
    return(ret);
} /* end edc_approve */
#endif /* for TRANSAPI */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
