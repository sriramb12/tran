#ifndef TRANSAPI
#ifndef M2
#ifndef VPN
/****************************************************************************
 *
 * Function:	init_trans_socket
 *
 * Description:	Initializes connection between Transcend/InterCom client and server.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "company.h"
#include "global_defines.h"
#include "global_params.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "transapi_defines.h"
#include "x500_defines.h"
#include "trans_prototypes_2.h"

#include "global_debug.h" // Mazieri's debug facilities

/* Holds server name, address, and socket */
extern SERVER_PARAMS server;

/* Connect to the server socket */
/* Send the header information and get response from server */
/* If the connect fails or the response from the server is not SUCCESS, */
/* this function puts reason for failure in rtn_msg */
/* It also closes the socket */

#include "trans_prototypes.h"
#include "client_lib.h"

int init_trans_socket(
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
    char *par6)
{
    DBG();

    extern int compression;
    short ret;
    char buffer[MSG_SIZE];

    /* build header to send */
    header_build(buffer, req_type, key1,   email1, email2, size, file_mode, file_type, par1, par2, par3, par4, par5, par6, user.login);

     /* First try to connect to server set up in lookup_company */
    ret = init_socket(buffer);

    /* If sent to wrong server, get address of right server */
    if (ret == -535)
    {
	    /* At this point correct site is in rtn.msg */
	    if (sscanf(rtn.msg, "%s %s %s %d", server.site, server.machine, server.address, &server.socket) == 4)
        {
	        server.socket = htons(server.socket);

	        /* Must rewrite header with new site in it */
	        /* Build the header */
            /* Set message length back to 0 */
            sprintf(buffer+12," %d %s;%s;%s;%s;%s;%s;%s;%ld %lld %d %ld %c %s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;",
                req_type, key1, user.login, user.mach, user.addr, user.cwd, TRANS_VERSION, "Web_browser", 
                time(0), size, compression, file_mode, file_type, user.email, email2, user.from_company, server.site, 
                par1, par2, par3, par4, par5, par6, user.name);
	
	        rtn.len = 0;
	        ret  = init_socket(buffer);
	    } /* End if for able to scan in alternate server */
    } /* End if */

    rtn.len = 0;
    return(ret);
} /* End init_trans_socket */
#endif
#endif /* for not M2 */
#endif /* For not TRANSAPI */
