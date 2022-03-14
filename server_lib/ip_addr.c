/****************************************************************************
 *
 * Name:	ip_addr_lookup, ip_addr_copy, ip_addr_find
 *
 * Description:	Converts IP address to format used by socket call.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include "global_defines.h"
#include "global_client_errs.h"
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities

/* Used to get IP node address of server */
/* If cannot find address using gethostbyname, uses default specified */
int ip_addr_lookup(char *string, char *address, char *name)
{
    DBG("string='%s', address='%s' and name='%s'", string, address, name);

    int ip_addr_copy(char *string, char *address);
    struct hostent *host;

    host = gethostbyname(name);

    /* Must hard code value for Azalea */
    if (host != NULL)
    {
	    strncpy(string, host->h_addr_list[0], 4);
        sprintf(server.address, "%u.%u.%u.%u", (unsigned char)string[0], (unsigned char)string[1], (unsigned char)string[2], (unsigned char)string[3]);
	    return(SUCCESS);
    } /* End if for address found with gethostbyname */
    else return(ip_addr_copy(string, address));
} /* End ip_addr_lookup */


/* Used to change IP address to network format */
int ip_addr_copy(char *string, char *address)
{
    DBG("string='%s' and address='%s'", string, address);

    int net[4];
    int cnt;
    if (sscanf(address, "%u.%u.%u.%u", &net[0], &net[1], &net[2], &net[3])  != 4)
    {
	    strcpy7(rtn.msg, bad_ip_addr, RET2TAB, "'", address, "'",
	    DOUBLE_RET, HELP_MSG);
	    rtn.flag = DB_ERR;
	    return(BAD_IP_ADDR);
    } /* End if for invalid address */

    bcopy(address, string, (size_t) 4);
/*
    for (cnt = 0; cnt < 4; ++cnt)
        string[cnt] = net[cnt];
*/

    return(SUCCESS);
} /* End ip_addr_copy */


/* Looks up IP node address using gethostbyname */
/* Used to get IP node address of client machine */
/* If not found, uses default address */
void ip_addr_find(char *addr, char *name)
{
    DBG("addr='%s' and name='%s'", addr, name);
    struct hostent *host;

    host = gethostbyname(name);
    if (host != NULL)
    {
	    sprintf(addr, "%u.%u.%u.%u", (unsigned char)host->h_addr_list[0][0], (unsigned char)host->h_addr_list[0][1], (unsigned char)host->h_addr_list[0][2], (unsigned char)host->h_addr_list[0][3]);
    }
    else strcpy(addr, "000.000.000.000");
} /* End ip_addr_find */
