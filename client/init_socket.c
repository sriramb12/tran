 /* Function:	init_socket
 *
 * Description:	Initializes socket
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#ifdef WINDOWS_NT4
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#define closesocket close
#include <netinet/in.h>
#endif
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "client_lib.h"
#include "global_defines.h"
#include "global_client_errs.h"
#include "global_params.h"
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "x500_defines.h"

#include "global_debug.h" // Mazieri's debug facilities


#define DEBUG 0

/* Holds server name, address, and socket */
extern SERVER_PARAMS server;

/* Connect to the server socket */
/* Send the header information and get response from server */
/* If the connect fails or the response from the server is not SUCCESS, */
/* this function puts reason for failure in rtn_msg */
/* It also closes the socket */
int sock_connect(void)
{
    DBG();
    int one = 1;
    int tcp_buf_size = 1024*256;
    int connect_sock;
    short ret;
    struct sockaddr_in client;

#ifdef WINDOWS_NT4
    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);
#endif
    /* Use the default host defined in the RC table */
#ifdef test
    strcpy(server.machine, "lvd0325");
    strcpy(server.address, "10.84.151.75");
    server.socket = htons(6927);
#endif
    //PRINTF("transweb server:  %s %s %d\n", server.machine, server.address, ntohs(server.socket));
    if ((ret = ip_lookup((char *)&client.sin_addr, server.address, server.machine)) != SUCCESS)
    return(ret);
  /* Get a socket (Internet type, tcp protocol */
  if ((connect_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  {
	sprintf(rtn.msg, bad_sock_open, errno);
	strcpy2(rtn.msg+strlen(rtn.msg), DOUBLE_RET, HELP_MSG);
    return(BAD_SOCK_OPEN);
  }
  /* Set the send and receive  buffer size to 256K */
       setsockopt ( connect_sock , SOL_SOCKET , SO_SNDBUF ,
			(char *)&tcp_buf_size , sizeof(tcp_buf_size));
       setsockopt ( connect_sock , SOL_SOCKET , SO_RCVBUF ,
			(char *)&tcp_buf_size , sizeof(tcp_buf_size));
       setsockopt ( connect_sock , SOL_SOCKET , SO_REUSEADDR ,
			(char *)&one , sizeof(one));
       setsockopt ( connect_sock , SOL_SOCKET , SO_REUSEADDR ,
			(char *)&one , sizeof(one));
      
    /* Create the socket address structure to connect to */
    client.sin_family = AF_INET;
    client.sin_port = server.socket;

    /* Try to connect to the address */
    if (connect(connect_sock, (struct sockaddr *)&client, sizeof(client)) < 0) 
    {
	close(connect_sock);
	/* Add connect error message only if buffer size not exceeded */
	if (rtn.len < (BUFFER_SIZE-256))
	{
	sprintf(rtn.msg+rtn.len, connect_err, errno, server.machine, server.address);
	rtn.len += strlen(rtn.msg+rtn.len);
	} /* End if for buffer size not exceeded */
	    return(CONNECT_ERR);
    } /* End bad return code */
    else return(connect_sock);
} /* End sock_connect */


short init_socket(char *buffer)
{
    DBG();
    extern int sock;
    short ret;

    /* Connect to the socket */
    if ((sock = sock_connect())< 0) 
    return(sock);
    return(header_send(buffer));
} /* End init_socket */


int header_send(char *buffer)
{
    DBG("buffer='%s'", buffer);

    /* Send the header to the server */
    int ret;

    if ((ret = put_binary_string(buffer, (strlen(buffer+12)+12))) != SUCCESS) 
    {
	    close(sock);
	    return(ret);
    } /* End if for unable to put string */

    /* Get the response from the server */
    if (((ret = get_short()) >= SUCCESS) || (ret == DBEMAIL)) 
        return(ret);

    /* If ret out of range, error getting data from the data base */
    if (ret == BAD_GET)
        return(ret);

    rtn.len += get_string(rtn.msg+rtn.len);

	close(sock);

    return(ret);
} /* end header_send */


/* Build header sent to server */
/* This is used by all the data transfer servers */
void header_bld(
    char *buffer,
    REQ_TYPE req_type,
    char *key1,
    char *email1,
    char *email2,
    unsigned long long size,
    long file_mode,
    char file_type,
    char *par1,
    char *par2,
    char *par3,
    char *par4,
    char *par5,
    char *par6,
    char *user_login)
{
    DBG();
    extern char icap_class[];
    long time_int;
    char tmp_password[64];

    /* Build header to send to Transcend/InterCom server */
    /* Put password in message string */
    strcpy2(tmp_password, user.login, TRANS_KEY);
    password_encrypt(buffer, tmp_password, PWD_LEN);
    /* Find the current time */
    time_int = time(0);
    /* Build the header */
    sprintf(buffer+12,
" %d %s;%s;%s;%s;%s;%s;%s;%ld %lld %lld %ld %c %s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;",
    req_type, key1,
   user.login, user.mach, user.addr, user.cwd, TRANS_VERSION,
    user.machine_type, time_int,
    (unsigned long long)size, (unsigned long long)0, file_mode, file_type, 
    email1, email2,
    user.from_company, server.site,
    par1, par2, par3, par4, par5, par6, user.name, icap_class);
} /* end header_build */
