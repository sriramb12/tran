/****************************************************************************
 *
 * Name:	ip_lookup, ip_copy, ip_find
 *
 * Description:	Converts IP address to format used by socket call.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#ifdef WINDOWS_NT4
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
extern int errno;
#define closesocket close
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#endif
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "global_defines.h"
#include "global_client_errs.h"
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "trans_prototypes.h"

#include "global_debug.h" // Mazieri's debug facilities

#define HELP_MSG "For assistance, contact Intercom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"


/* Used to change IP address to network format */
int ip_copy(char *string, char *address)
{
    DBG();
    unsigned int net[4];
    int cnt;
    unsigned char *dest = (unsigned char *)string;
    if (sscanf(address, "%u.%u.%u.%u",
               &net[0], &net[1], &net[2], &net[3])  != 4)
    {
	    strcpy7(rtn.msg, bad_ip_addr, RET2TAB, "'", address, "'",
	    DOUBLE_RET, HELP_MSG);
	    rtn.flag = DB_ERR;
	return(BAD_IP_ADDR);
    } /* End if for invalid address */

    for (cnt = 0; cnt < 4; ++cnt)
     dest[cnt] = (unsigned char)net[cnt];

    return(SUCCESS);
} /* End ip_copy */


/* Used to get IP node address of server */
/* If cannot find address using gethostbyname, uses default specified */
int ip_lookup(char *string, char *address, char *name)
{
    DBG("string='%s', address='%s', name='%s'", string, address, name);

    struct hostent *host;

    /* If this is the localhost, do not do the look-up */
    /* because it will be wrong */
    /* copy the hard coded IP node address */
    if (strcmp(name, "localhost") == 0 || strcmp(name, "127.0.0.1") == 0)
    {
        return(ip_copy(string, address));
    }

    /* check if name is an IP number */
    if (isdigit(name[0]) && isdigit(name[strlen(name)-1]) && strchr(name, '.') != NULL)
    {
        return(ip_copy(string, name));
    }

/*
    host = getipnodebyname(name, AF_INET, AI_DEFAULT, &errno);
*/
    DBG("before call gethostbyname(%s);", name);
    host = gethostbyname(name);
    DBG("after call gethostbyname(%s);", name);

    /* if for address found with gethostbyname */
    if (host != NULL)
    {
        DBG("host!=NULL, host->h_name='%s'", host->h_name);

	    /* Copy the address found into the address field */
	    strncpy(string, host->h_addr_list[0], 4);
        sprintf(address, "%u.%u.%u.%u", (unsigned char)string[0], (unsigned char)string[1], (unsigned char)string[2], (unsigned char)string[3]);

        DBG("address='%s'", address);

	    return(SUCCESS);
	    DBG("returned");

    /* Otherwise just copy in the hard coded IP node address */
    } else {

        DBG("host is null, calling ip_copy(string, address) = ('%s', '%s')", string, address);
        return(ip_copy(string, address));
    }
} /* End ip_lookup */


/* Looks up IP node address using gethostbyname */
/* Used to get IP node address of client machine */
/* If not found, uses default address */
void ip_find(char *addr, char *name)
{
    DBG("addr='%s', name='%s'", addr, name);

    struct hostent *host;
    host = gethostbyname(name);

    if (host != NULL) {
	    sprintf(addr, "%u.%u.%u.%u", (unsigned char)host->h_addr_list[0][0], (unsigned char)host->h_addr_list[0][1], (unsigned char)host->h_addr_list[0][2], (unsigned char)host->h_addr_list[0][3]);
    } else {
        strcpy(addr, "000.000.000.000");
    }

    DBG("function write to addr = '%s'", addr);

} /* End ip_find */
