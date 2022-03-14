#ifdef M2
/****************************************************************************
 *
 * Function:	mask_msg_server_send
 *
 * Description:	Sends remote server a mask message.
 *		Sends only the header and does not get the return code.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "socket_includes.h"
#include "rtn.h"
#include "rmt_server.h"
#include "mask_msg.h"
#include "user_params.h"
#include "global_debug.h" // Mazieri's debug facilities

/* Connect to the remote socket */
/* If the connect is unsuccessful */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
short mask_msg_server_send(MASK_MSG *mask_msg, int indx)
{
    DBG();

    char header_buffer[1500];
    char string[4];
    struct sockaddr_in client;
    int ret;
    short action;

    /* Look up the IP node address */
    ip_addr_lookup(string, rmt_mpd.addr[indx], rmt_mpd.mach[indx]);
  
    /* Get a socket (Internet type, tcp protocol */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
	    data_log(tr.log_file, bad_rmt_sock_open);
	    return(BAD_RMT_CONNECT);
    }

    /* Null out the structure before assigning values */
    memset(&client, 0, sizeof(client));

    /* Allow connection to any socket */
    client.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Create the socket address structure to connect to */
    client.sin_family = AF_INET;
    client.sin_port = rmt_mpd.sock[indx];
    bcopy(string, &client.sin_addr, 4);

    /* Try to connect to the address */
    if (( ret = connect(sock, (struct sockaddr *)&client, sizeof(client))) < 0) 
    {
	    sprintf(rtn.msg, bad_rmt_connect, errno, rmt_mpd.mach[indx]);
	    data_log(tr.log_file, rtn.msg);
	    return(BAD_RMT_CONNECT);
    }

    /* Build string to send to remote server */
    /* Put password in message string */
    /* Must put in by hand so nulls will be put in */
    for (ret = 0; ret < 12; ++ret)
        header_buffer[ret] = mask_msg->password[ret];
    sprintf(header_buffer+12, " %c %c %c %c %c %c %f %c %f %s %s %s %s\
 %s %s %s %s %s %s %s %s %s %s %d %s",
    mask_msg->req_type, mask_msg->transaction_type, mask_msg->data_type, mask_msg->override_flag, mask_msg->sec_req_type,
    mask_msg->version_flag, mask_msg->vnum, mask_msg-> sec_version_flag, mask_msg->sec_vnum, 
    user.login, user.mach, user.addr, user.cwd,
    mask_msg->key1, mask_msg->key2, mask_msg->key3, mask_msg->pin1, mask_msg->pin2,
    mask_msg->par1, mask_msg->par2, mask_msg->par3, mask_msg->par4, mask_msg->par5, mask_msg->transaction_num, user.name);
    
    /* Calculate the length of the string */
    ret = strlen(header_buffer+13) +13;
    if (put_rmt_binary_string(header_buffer, ret) != SUCCESS) return(PUT_ERR);

    /* Get action from the remote server */
    get_timed_short((char *)&action);
    if (action < SUCCESS)
    {
	    if (action == BAD_GET) log3("bad_get ", " from ", rmt_mpd.site[indx]);
	    else if (action == TIMEOUT_ERR)	log3(timeout_err, rmt_mpd.site[indx], tr.pid_string);
	    else
	    {
	        /* Get reason for failure */
	        if (get_rmt_string(rtn.msg) <= 0) return(GET_ERR);

	        /* Get the flags */
	        get_rmt_string(bf.tmp_data);

	        /* Log the reason for failure */
	        log3(rtn.msg, " from ", rmt_mpd.site[indx]);
	    } /* End if */
	
	    /* Close the connection to the remote server */
	    close(sock);
    } /* end if */

    return(action);
} /* End mask_msg_server_send */
#endif
