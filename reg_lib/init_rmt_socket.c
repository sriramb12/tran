#ifndef SSL_ENCRYPTION
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
#include <unistd.h>
#include <string.h>
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "socket_includes.h"
#include "rtn.h"
#include "rmt_server.h"
#include "server_lib.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

/* Connect to the remote socket */
/* If the connect is unsuccessful */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
/* This function does not use an SSL connection */
/* int indx is the index into the server table */
short init_rmt_socket(char *header_buffer, RMT_SERVERS *rmt, int indx)
{
    struct sockaddr_in client;
    int ret;
    short action;

    DBG("header_buffer='%s', indx='%d'", header_buffer, indx);

    /* Get a socket (Internet type, tcp protocol */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
	    data_log(tr.log_file, bad_rmt_sock_open);
	    return(BAD_RMT_CONNECT);
    }

    /* Null out the structure before assigning values */
    memset(&client, 0, sizeof(client));

    /* Build the character representation for the IP node address */
    /* Also does a DNS look-up using the machine name */
    printf("address is %s and %s\n", rmt->addr[indx], rmt->mach[indx]);
    if ((ret = ip_addr_lookup((char*)&client.sin_addr, rmt->addr[indx], rmt->mach[indx])) != SUCCESS)
    {
	    data_log(tr.log_file, rtn.msg);
	    return(ret);
    }

    /* Create the socket address structure to connect to */
    client.sin_family = AF_INET;
    client.sin_port = htons(rmt->sock[indx]);

    /* Try to connect to the address */
    if (( ret = connect(sock, (struct sockaddr *)&client, sizeof(client))) < 0)
    {
	    close(sock);
	    rtn.len += sprintf(rtn.msg+rtn.len, bad_rmt_connect,
	    errno, rmt->mach[indx]);
	    return(BAD_RMT_CONNECT);
    }

    /* Send the header */
    /* Calculate the length of the string */
    /* The internal password might contain the null character */
    ret = strlen(header_buffer+13) +13;
    if ((ret = put_rmt_binary_string(header_buffer, ret)) != SUCCESS)
    {
	    return(PUT_ERR);
    }

    /* Get action from the remote server */
    get_timed_short((char *)&action);

    /* Convert from network order to host order */
    action = ntohs(action);
    if (action < SUCCESS)
    {
	    if (action == BAD_GET)
	        log3("bad_get ", " from ", rmt->site[indx]);
	    else if (action == TIMEOUT_ERR)
	        log3(timeout_err, rmt->site[indx], tr.pid_string);
	    else
	    {
	        /* First get the specific return code */
	        ret = get_short();

	        /* Get reason for failure */
	        if (get_rmt_string(rtn.msg) <= 0) return(GET_ERR);
	        else data_log(tr.log_file, rtn.msg);

	        /* Log the reason for failure */
	        log3(rtn.msg, " from ", rmt->site[indx]);
	    } /* End if */

	    /* Close the connection to the remote server */
	    close(sock);
    }

    return(action);
} /* End init_rmt_socket */
#endif
