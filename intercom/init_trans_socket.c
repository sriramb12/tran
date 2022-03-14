/****************************************************************************
 *
 * Function:	init_trans_socket
 *
 * Description:	Initializes connection between Transcend/InterCom client and server.
 *
 ***************************************************************************/

#define closesocket close
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "company.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_defines.h"
#include "trans_server_globals.h"
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "user_params.h"
#include "x500_defines.h"
#include "global_debug.h"

/* Holds server name, address, and socket */
extern SERVER_PARAMS server;

short init_transweb_socket(char *buffer, char *machine, char *address, int port)
{
    struct sockaddr_in client;
    short ret;
    short action;

    DBG("machine='%s', addresss='%s', port='%d'", machine, address, port);

    /* Get a socket (Internet type, tcp protocol */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
	    data_log(tr.log_file, bad_rmt_sock_open);
	    return(-110);
    }

    /* Null out the structure before assigning values */
    memset(&client, 0, sizeof(client));

    /* Do DNS lookup on machine */
    /* Also convert address to correct 4 character format */
    ip_addr_copy((char *)&client.sin_addr, address);

    /* Create the socket address structure to connect to */
    client.sin_family = AF_INET;
    client.sin_port = htons(port);

    /* Try to connect to the address */
    if (( ret = connect(sock, (struct sockaddr *)&client, sizeof(client))) < 0) 
    {
	    sprintf(rtn.msg, rmt_connect_err, errno, machine, address);
	    data_log(tr.log_file, rtn.msg);
	    return(-110);
    }

    /* Send the header */
    /* Calculate the length of the string */
    /* The internal password might contain the null character */
    ret = strlen(buffer+12) +12;
    put_binary_string(buffer, ret);

    /* Get action from the remote server */
    if ((action = get_short()) < SUCCESS)
    {
	    if (action == BAD_GET)
	        log3("bad_get ", " from ", "server.site");
	    else if (action == TIMEOUT_ERR)
	        log3(timeout_err, " ", tr.pid_string);
	    else
	    {
	        /* Get reason for failure */
	        if (get_string(rtn.msg) <= 0) return(GET_ERR);

	        /* Log the reason for failure */
	        log3(rtn.msg, " from ", server.site);
	    } /* End if */

	/* Close the connection to the remote server */
	close(sock);
    }

    DBG("will return action");
    return(action);
} /* End init_transweb_socket */


/* Connect to the server socket */
/* Send the header information and get response from server */
/* If the connect fails or the response from the server is not SUCCESS, */
/* this function puts reason for failure in rtn_msg */
/* It also closes the socket */
int init_ftp_transweb_socket(char req_type, char *key1, char *to_company)
{
    int cnt;
    short ret;
    char buffer[BUFFER_SIZE];

    DBG("key='%s', to_company='%s'", key1, to_company);

    /* Will always try to connect to dropbox8 */
    strcpy(server.machine, "dropbox8");
    strcpy(server.address, "192.88.157.190");
    strcpy(server.site, "External");
    server.socket = htons(6927);

    /* Build header to send to Transcend/InterCom server */
    /* Build the header */
    /* First put in the password passed from the client */
    for (cnt = 0; cnt < 12; ++cnt)
        buffer[cnt] = tr.crypt_pwd[cnt];

    sprintf(buffer+12,
        " %d %s;%s;%s;%s;%s;%s;%s;%ld %lld %lld %ld %c %s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;",
        req_type, key1, user.login, user.mach, user.addr, user.cwd, TRANS_VERSION,
        gu.machine_type, gu.client_time, gu.original_size, gu.compressed_size, gu.file_mode, gu.file_type, 
        gu.email1, gu.email2, gu.from_company, to_company, gu.par1, gu.par2, gu.par3, gu.par4, gu.par5, gu.par6, user.name);

    ret =  init_transweb_socket(buffer, server.machine, server.address, server.socket);

    DBG("will return");
    return(ret);
} /* End init_trans_socket */
