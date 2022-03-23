#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	TransAPI functions
 *
 * Description:	Functions the API can call
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <malloc.h>
#include <pwd.h>
#include <sys/stat.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "ssl_client_lib.h"
#include "global_defines.h"
#include "global_params.h"
#include "rtn.h"
#include "strcpy.h"
#include "server_params.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_msg.h"
#include "transapi_prototypes.h"
/****************************************************************************
 *
 * Function:	client
 *
 * Description:	Main routine for Transcend, JDCtrans, and InterCom
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ssl_client_lib.h"
#include "global_client_errs.h"
#include "global_params.h"
#include "server_params.h"
#include "strcpy.h"
#include "trans_client_msg.h"
#include "trans_prototypes.h"

#include "global_debug.h"


int  trans_support_request(int index, int argc, char **argv, char *email_list, char *admin_list)
{
    DBG();

    int ret;

    /* Set the repository for the request */
    trans_repository_set(2);
    switch(argv[1][2])
    {
	case 'a': 
	{
	    /* Set the request type */
	    trans_simple_basic_request_set(index, DBSUPPORT_ADD, email_list);
	    ret = trans_list_send( trans, argv+2); 
	}
	break;
	case 'd': 
	{
	    trans_simple_basic_request_set(index, DBSUPPORT_DELETE, email_list);
	    ret = trans_list_send( trans, argv+2); 
	}
	break;
	case 'v': 
	{
	    ret = trans_simple_file_get(index, DBSUPPORT_VIEW, email_list); 
	}
	break;
	default:
	{
/*
	    char *ptr = "icom";
	    PRINTF(CUST_FORMAT_MSG);
	    PRINTF(CUST_MSG, ptr, ptr, ptr, ptr, ptr, ptr);
*/
	    exit(101);
	}
    } /* End switch */
    return(ret);
} /* end support_request */
#endif
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
