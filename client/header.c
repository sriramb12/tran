#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	header_send and header_build
 *
 * Description:	Builds and sends headers
 *
 ***************************************************************************/

#include <fcntl.h>
#ifdef WINDOWS_NT4
#define WIN32
#include <windows.h>
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
#include <time.h>
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

/* Holds server name, address, and socket */
extern SERVER_PARAMS server;
extern char icap_class[];


#ifndef TRANSWEB
/* Sends header to the server */
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
    /* Perform registration */
    if ((ret = get_short()) == DBEMAIL)
        return(user_autoreg());

    /* If ret out of range, error getting data from the data base */
    if (ret >= SUCCESS) return(ret);

    if (ret == BAD_GET)
        return(ret);

    rtn.len += get_string(rtn.msg+rtn.len);

	close(sock);

    return(ret);
} /* end header_send */
#endif


/* Build header sent to server */
/* This is used by all the data transfer servers */
void header_build(char *buffer, REQ_TYPE req_type, char *key1, char *email1,
    char *email2, unsigned long long size, long file_mode, char file_type,
    char *par1, char *par2, char *par3, char *par4, char *par5, char *par6,
    char *user_login)
{
    DBG();
    extern char global_par6[];
    char *par6_ptr;
    long time_int;
    char tmp_password[90];

    /* If this is the GUI, use encrypted core ID */

    str_lower(user.login);

    DBG("user.login='%s', user.addr='%s', user.mach='%s'", user.login, user.addr, user.mach);

    /* Build header to send to Transcend/InterCom server */
    /* Put password in message string */
    strcpy2(tmp_password, user.login, TRANS_KEY);
    password_encrypt(buffer, tmp_password, PWD_LEN);

    /* Find the current time */
    time_int = time(0);

    /* Build the header */
    /* global_par6 holds indication that subnet must be checked for encrypted password */
    if (global_par6[0] != '\0') 
        par6_ptr = global_par6;
    else 
        par6_ptr = par6;

    ip_find(user.addr, user.mach);
    DBG("after ip_find: user.mach='%s', user.addr='%s' AND user.cwd='%s'", user.mach, user.addr, user.cwd);


    sprintf(buffer+12," %d %s;%s;%s;%s;%s;%s;%s;%ld %lld %lld %ld %c %s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;",
        req_type, key1, user.login, user.mach, user.addr, "TransWeb dir", TRANS_VERSION,
        user.machine_type, time_int, (unsigned long long)size, (unsigned long long)0, file_mode, file_type, 
        email1, email2, user.from_company, server.site, par1, par2, par3, par4, par5, par6_ptr, user.name, icap_class);

    DBG("buffer after this function will be\n %s", buffer);
} /* end header_build */
#endif /* For not TRANSAPI */
