#ifdef SSL_ENCRYPTION
/****************************************************************************
 *
 * Functions:	get_ssl_short, get_string, put_ssl_short, put_string
 *
 * Description:	Holds basic routines needed to read and write to a socket.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bytes.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include "global_defines.h"
#include "global_params.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "intercom_server_prototypes.h"
#include "rtn.h"
#include "strcpy.h"
#include "server_lib.h"

#include "global_debug.h" // Mazieri's debug facilities

extern int trans_sock;
SSL *trans_ssl;

/* Used by the data base server and mask shop server */
short get_ssl_string(char *buffer)
{
    short get_ssl_short(void);
    short len;
    int rcvd = 0;
    register short cnt;
    int ret;

    DBG("buffer='%s'", buffer);

    len = get_ssl_short();
    if ((len < SUCCESS) || (len > BLOCK_SIZE))
    {
	    get_ssl_string(rtn.msg);
	    close(trans_sock);
	    shut_down(rtn.msg, ret);
    }

    cnt = 0;
    while (rcvd < len)
    {
	    ret = SSL_read(trans_ssl, buffer+rcvd, len-rcvd);
	    /* If didn't get anything, wait for a second and try again */
	    if (ret < 0) sleep(1);
	    else rcvd += ret;

	    ++cnt;

	    if (cnt > 50)
        {
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, GET_ERR);
	    }
    } /* End while */

    buffer[len] = '\0';
    return(len);
} /* End get_ssl_string */


short get_ssl_short(void)
{
    char buffer[2];
    short ret;
    short len = 2;
    int rcvd = 0;
    int cnt = 0;

    DBG();

    while (rcvd < len)
    {
	    ret = SSL_read(trans_ssl, buffer+rcvd, len-rcvd);
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


short get_timed_ssl_short(void)
{
    char buffer[2];
    short ret;
    short len = 2;
    int rcvd = 0;
    int cnt = 0;

    DBG();

    while (rcvd < len)
    {
	    ret = read_with_timeout(trans_sock, buffer+rcvd, len-rcvd);
	    if (ret < 0)
	    {
	        str_bcopy((char *)&ret, buffer, 2);
	        return(ret);
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
} /* End get_timed_ssl_short */


void put_ssl_char(char data)
{
    int ret;

    DBG("data='%s'", data);

    if ((ret = SSL_write(trans_ssl, &data, 1)) != 1)
    {
	    close(trans_sock);
	    sprintf(rtn.msg, put_char_err, errno);
	    data_log(tr.log_file, rtn.msg);
	    log3("Closing ", "transaction ", tr.pid_string);
	    exit(0);
    }
}


short put_ssl_short(short data)
{
    short ret;
    short wr_data;

    DBG("data='%d'", data);

    wr_data = htons((short)data);

    if ((ret = SSL_write(trans_ssl, &wr_data, 2)) != 2)
    {
	    close(trans_sock);
	    sprintf(rtn.msg, put_short_err, errno);
	    data_log(tr.log_file, rtn.msg);
	    log3("Closing ", "transaction ", tr.pid_string);
	    exit(0);
    }

    return(ret);
}


/* Used to send data to the client */
int put_ssl_string(char *string)
{
    int len;
    int ret;

    DBG("string='%s'", string);

    len = strlen(string);
    put_ssl_short(len);

    if ((ret = SSL_write(trans_ssl, string, len)) != len)
    {
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }

    return(SUCCESS);
}


void put_ssl_long(long data)
{
    long wr_data;
    int ret;

    DBG("data='%ld'", data);

    wr_data = htons((long)data);
    if ((ret = SSL_write(trans_ssl, &data, 4)) != 4)
    {
	    sprintf(rtn.msg, put_short_err, errno);
	    shut_down(rtn.msg, -1);
    }
}


void put_ssl_longlong(long long data)
{
    int ret;

    DBG("data='%lld'", data);

#ifdef BYTESWAP
    data = bswap_64(data);
#endif
    if ((ret = SSL_write(trans_ssl, &data, 8)) != 8)
    {
	    sprintf(rtn.msg, put_short_err, errno);
	    shut_down(rtn.msg, -1);
    }
}


/* Used to send data to the client */
short put_ssl_binary_string(char *string, int len)
{
    int ret;

    DBG("len='%d'", string);

    put_ssl_short(len);

    if ((ret = SSL_write(trans_ssl, string, len)) != len)
    {
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }

    return(SUCCESS);
}


/* Concatinates two strings and sends to client */
/* All messages sent via put_ssl_string2 are logged */
void put_ssl_string2(char *string1, char *string2)
{
    int len;
    int ret;
    char string[BUFFER_SIZE];

    DBG("string1='%s' and string2='%s'", string1, string2);

    len = strcpy2(string, string1, string2);
    data_log(tr.log_file, string);
    put_ssl_short(len);

    if ((ret = SSL_write(trans_ssl, string, len)) != len)
    {
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }
}


/* Concatinates three strings and sends to client */
/* All messages sent via put_ssl_string3 are logged */
short put_ssl_string3(char *string1, char *string2, char *string3)
{
    int len;
    int ret;
    char string[BUFFER_SIZE];

    DBG("string1='%s', string2='%s', string3='%s'", string1, string2, string3);

    len = strcpy3(string, string1, string2, string3);
    put_ssl_short(len);

    if ((ret = SSL_write(trans_ssl, string, len)) != len)
    {
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }

    return(SUCCESS);
}


long get_ssl_long(void)
{
    short ret;
    short len = 4;
    int rcvd = 0;
    int cnt = 0;
    char buffer[4];

    DBG();

    while (rcvd < len)
    {
	    ret = SSL_read(trans_ssl, buffer+rcvd, len-rcvd);
	    if (ret < 0) {
	        close(trans_sock);
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
} /* End get_ssl_long */
#endif
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
