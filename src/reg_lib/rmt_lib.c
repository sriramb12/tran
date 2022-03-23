#ifndef SSL_ENCRYPTION
/****************************************************************************
 *
 * Function:	rmt_lib
 *
 * Description:	Routines to communicate with remote servers.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_errs.h"
#include "reg_globals.h"
#include <sys/types.h>
#include <netinet/in.h>
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

#define rmt_clean_up(message, ret, code) \
{ \
    sprintf(rtn.msg, message, ret); \
    data_log(tr.log_file, rtn.msg); \
    close(sock); \
    return(code); \
}
#define rmt2cleanup(message, ret1, ret2, ret3, code) \
{ \
    sprintf(rtn.msg, message, ret1, ret2, ret3); \
    data_log(tr.log_file, rtn.msg); \
    close(sock); \
    return(code); \
}

short get_ssl_short()
{
    char buffer[2];
    short ret;
    short len = 2;
    int rcvd = 0;
    int cnt = 0;

    DBG();

    while (rcvd < len) 
    {
	    ret = read(sock, buffer+rcvd, len-rcvd);

	    if (ret < 0) 
	    {
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, -1);
	    }
	
	    rcvd += ret;
	    ++cnt;

	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, -1);
	    }
    } /* End while */

    return(htons(*(short *)buffer));
} /* End get_ssl_short */


short get_rmt_short()
{
    char buffer[2];
    short ret;
    short len = 2;
    int rcvd = 0;
    int cnt = 0;

    DBG();

    while (rcvd < len) 
    {
	    ret = read(sock, buffer+rcvd, len-rcvd);
	    if (ret < 0) 
	    {
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, -1);
	    }
	
	    rcvd += ret;
	    ++cnt;

	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, -1);
	    }
    } /* End while */

    return(htons(*(short *)buffer));
} /* End get_ssl_short */


void get_timed_short(char *buffer)
{
    short ret;
    short len = 2;
    int rcvd = 0;
    int cnt = 0;

    DBG("buffer='%s'", buffer);

    while (rcvd < len) 
    {
	    ret = read_with_timeout(sock, buffer+rcvd, len-rcvd);
	    if (ret < 0) 
	    {
	        str_bcopy((char *)&ret, buffer, 2);
	        return;
	    }
	
	    rcvd += ret;
	    ++cnt;
	
	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, -1);
	    }
    } /* End while */
} /* End get_timed_short */


short put_ssl_short(short data)
{
    int ret;
    short wr_data;

    DBG("data='%d'", data);

    wr_data = htons((short)data);

    if ((ret = write(sock, &wr_data, 2)) == 2) 
    {
        return(SUCCESS);
    } else {
        /* Log the failure and close the socket */
        rmt_clean_up(put_short_err, ret, PUT_SHORT_ERR);
    }
} /* End put_ssl_short */


short get_ssl_string(char *buffer)
{
    short len;
    int rcvd = 0;
    short ret;

    DBG("buffer='%s'", buffer);

    len = get_ssl_short();

    if ((len < SUCCESS) || (len > 1024))
        return(GET_SHORT_ERR);

    while (rcvd < len) 
    {
	    ret = read(sock, buffer+rcvd, len-rcvd);

	    if (ret < 0) 
	        rmt_clean_up(get_err, ret, GET_ERR);

	    rcvd += ret;
    }

    buffer[len] = '\0';

    return(len);
} /* End get_ssl_string */


short get_rmt_string(char *buffer)
{
    short len;
    int rcvd = 0;
    short ret;

    DBG("buffer='%s'", buffer);

    len = get_ssl_short();
    if ((len < SUCCESS) || (len > 1024))
        return(GET_SHORT_ERR);
  
    while (rcvd < len) 
    {
	    ret = read(sock, buffer+rcvd, len-rcvd);

	    if (ret < 0) 
	        rmt_clean_up(get_err, ret, GET_ERR);

	    rcvd += ret;
    }

    buffer[len] = '\0';

    return(len);
} /* End get_ssl_string */


/* Used to send data to the remote server */
short put_ssl_string(char *string)
{
    int ret;
    int len;

    DBG("string='%s'", string);

    len = strlen(string);
    
    if (put_ssl_short(len) != SUCCESS) return(PUT_SHORT_ERR);

    if ((ret = write(sock, string, len)) != len) 
        rmt2cleanup(put_err, errno, len, ret, PUT_ERR);

    return(SUCCESS);
} /* End put_ssl_string */


short put_rmt_binary_string(char *string, register int len)
{
    register int ret;

    DBG("string='%s'i, len='%d'", string, len);

    if (put_ssl_short(len) != SUCCESS)
        return(PUT_SHORT_ERR);

    if ((ret = write(sock, string, len)) != len) 
        rmt2cleanup(put_err, errno, len, ret, PUT_ERR);

    return(SUCCESS);
} /* End put_ssl_binary_string */


void put_ssl_long(long data)
{
    long wr_data;
    int ret;

    DBG("data='%ld'", data);

    wr_data = htonl((long)data);

    if ((ret = send(sock, &wr_data, 4, 0)) != 4) 
    {
	    sprintf(rtn.msg, put_short_err, errno);
	    shut_down(rtn.msg, -1);
    }
}


short put_ssl_longlong(unsigned long long data)
{
    DBG("data='%llu'", data);

#ifdef LINUX
    if (send_longlong(data) < 0)
#else
    if (send(sock, &data, 8, 0) < 0) 
#endif
    {
	    sprintf(rtn.msg, put_short_err, errno);
	    shut_down(rtn.msg, -1);
    } /* End if for bad send */
    else
        return(SUCCESS);
} /* End put_longlong */


/* Used to send data to the client */
void put_ssl_binary_string(char *string, int len)
{
    int ret;

    DBG("string='%s', len='%d'", string, len);

    put_short(len);

    if ((ret = send(sock, string, len, 0)) != len) 
    {
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }
}


long get_ssl_long()
{
    short ret;
    short len = 4;
    int rcvd = 0;
    int cnt = 0;
    char buffer[4];

    DBG();

    while (rcvd < len) 
    {
	    ret = recv(sock, buffer+rcvd, len-rcvd, 0);

	    if (ret < 0) {
	        close(sock);
	        sprintf(rtn.msg, bad_len_err, len);
	        shut_down(rtn.msg, BAD_LEN_ERR);
	    }
	
	    rcvd += ret;
	    ++cnt;
	
	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, -1);
	    }
    } /* End while */

    return(ntohl(*(long *)buffer));
} /* End get_long */

#endif
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
