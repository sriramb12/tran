#ifdef old
#ifdef SSL_ENCRYPTION
/****************************************************************************
 *
 * Function:	init_rmt_socket
 *
 * Description:	Initializes socket for remote server.
 *
 Returns:	Returns response from remote server
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "socket_includes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "rmt_server.h"
#include "user_params.h"
#include "reg_globals.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "trans_lib_prototypes.h"
#include "intercom_server_prototypes.h"

#include "global_debug.h" // Mazieri's debug facilities

/* Connect to the remote socket */
/* If the connect is unsuccessful */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
short init_rmt_socket(char *header_buffer, RMT_SERVERS *rmt, int index)
{
    extern int trans_sock;
    int ret;
    short action;

    DBG("header_buffer='%s', index='%d'", header_buffer, index);

    /* connect to the SSL socket */
    sprintf(bf.msg, "Connecting to %s%s %d",  rmt->mach[index], rmt->addr[index], rmt->sock[index]);

    data_log(tr.log_file, bf.msg);
label: if ((ret = ssl_init(rmt->mach[index], rmt->addr[index], htons(rmt->sock[index]))) < 0)
     return(ret); 

    /* Send the header */
    /* Calculate the length of the string */
    /* The internal password might contain the null character */
    ret = strlen(header_buffer+12) +12;
    if ((ret = put_ssl_binary_string(header_buffer, ret)) != SUCCESS) 
    {
	    return(PUT_ERR);
    }
    /* Get action from the remote server */
    if ((action = get_ssl_short()) < SUCCESS)
    {
	    if (action == BAD_GET)
	        log3("bad_get ", " from ", "server.site");
	    else if (action == TIMEOUT_ERR)
	        log3(timeout_err, " ", tr.pid_string);
	    else
	    {
	        /* Get reason for failure */
	        if (get_ssl_string(rtn.msg) <= 0) return(GET_ERR);

	        /* Log the reason for failure */
	        log3(rtn.msg, " from ", server.site);
	    } /* End if */

	    /* Close the connection to the remote server */
	    close(trans_sock);
    }

    return(action);
} /* End init_rmt_socket */
#endif
#endif
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
