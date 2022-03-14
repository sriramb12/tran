#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	dbsend_request
 *
 * Description:	Sends the Big file server a big file.
 *
 ***************************************************************************/

     #include <stdlib.h>
#ifndef WINDOWS_NT4
     #include <alloca.h>
#endif
#include "client_files.h"
#include "global_client_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include <sys/stat.h>
#include "trans_prototypes.h"
#include "client_lib.h"
#include "transapi_defines.h"
#include <string.h>
#include <netinet/in.h>
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

#ifndef NXP
int is_transweb_keyword(REQ_TYPE req_type, char *secure_key, char *file_key)
{
    int ret;

    if ((ret = is_keyword(req_type, secure_key)) != SUCCESS) return(ret);
    return(is_keyword(req_type, file_key));
} /* End if_transweb_key */
#endif

#ifdef NXP
int is_transweb_keyword(REQ_TYPE req_type, char *secure_key, char *file_key)
{

    return(is_keyword(req_type, secure_key));
} /* End if_transweb_key */
#endif

int dbsend_transweb_keyword_request(char req_type, char *secure_key, char *file_key)
{
    DBG();

    short ret;
    /* Verify this is a valid keyword */
    if ((ret = is_transweb_keyword(req_type, secure_key, file_key)) < 0) return(ret);
#ifndef SINGLE_CONNECT
/* If this is TransWeb, set up the server */
    repository_find(secure_key[8]);
#endif
    /* Connect to the database server */
    if ((ret = init_trans_socket(req_type, secure_key, NONE, NONE, 0, 0, 
    UNSPECIFIED, file_key, NONE, NONE, NONE, NONE, NONE)) != SUCCESS)
    return(ret);
    /* Get the return code */
    return(get_ret_code());
} /* End dbsend_transweb_keyword_request */

int extend_transweb_file(char *secure_key, char *file_key, int days)
{
    short ret;
    DBG();

    if ((ret = is_transweb_keyword(DBEXTEND_DATA, secure_key, file_key)) < 0) return(ret);
    
    repository_find(secure_key[8]);

    if ((ret = init_trans_socket(DBEXTEND_DATA, secure_key, NONE, NONE, days, 0, UNSPECIFIED, file_key, NONE, NONE, NONE, NONE, NONE)) < SUCCESS) return(ret);
        return(get_ret_code());
} /* End extend_transweb_file */

#endif /* For not TRANSAPI */
