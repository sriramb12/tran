#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	get_ret_code
 *
 * Description:	Get return code from the server
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "client_prototypes.h"
#include "global_params.h"
#include "rtn.h"
#include "client_lib.h"

#include "global_debug.h" // Mazieri's debug facilities



short get_ret_code(void)
{
    DBG();

    /* Get the return code from the server */
    short ret = get_short();
    /* Get the return string from the server */
    get_string(rtn.msg);
    close(sock);
    return(ret);
} /* End get_ret_code */

/* Used to get extra large return code */
#ifndef TRANSWEB
short get_big_ret_code(void)
{
    short ret;

    DBG();


    /* Get the return code from the server */
    ret = get_short();
    /* Get the return string from the server */
    get_big_string(rtn.msg);
    close(sock);
    return(ret);
} /* End get_ret_code */
#endif
#endif /* For not TRANSAPI */
