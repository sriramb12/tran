
/****************************************************************************
 *
 * Functions:	get_short, get_string, put_short, put_string
 *
 * Description:	Holds basic routines needed to read and write to a socket.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <openssl/ssl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "global_defines.h"
#include <string.h>
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h"

extern SSL *ssl;

/* Used by all data transfer tools */
#ifdef BYTESWAP
int send_longlong(unsigned long long data)
{
    unsigned long long buffer;

    DBG("data=%llu", data);

  buffer =    (((data) >> 56) & 0x00000000000000FFll) \
      +  (((data) >> 40) & 0x000000000000FF00ll) \
      +  (((data) >> 24) & 0x0000000000FF0000ll) \
      +  (((data) >> 8)  & 0x00000000FF000000ll) \
      +  (((data) << 8)  & 0x000000FF00000000ll) \
      +  (((data) << 24) & 0x0000FF0000000000ll) \
      +  (((data) << 40) & 0x00FF000000000000ll) \
      +  (((data) << 56) & 0xFF00000000000000ll) \
  ;
  return (SSL_write(ssl, &buffer, sizeof(unsigned long long)));
} /* end send_longlong */
#endif


static void show_ssl_error(SSL* ssl, int ret)
{
    int ssl_error = SSL_get_error(ssl, ret);
    DBG("return from SSL_get_error()=%d errno=%d", ssl_error, errno);
}

short get_string(char *buffer)
{
    short len;
    int rcvd = 0;
    register short cnt;
    int ret = 0;


    len = get_short();
    DBG("len=%d ", len);
    if ((len < SUCCESS) || (len > BLOCK_SIZE))
    {
        DBG("Error! rtn.msg='%s' BAD_LEN_ERR", rtn.msg);
	    close(sock);
	    sprintf(rtn.msg, bad_len_err, len);
	    shut_down(rtn.msg, BAD_LEN_ERR);
    }

    cnt = 0;
    while (rcvd < len)
    {
	    ret = SSL_read(ssl, buffer+rcvd, len-rcvd);
	    /* If didn't get anything, wait for a second and try again */
	    if (ret < 0) sleep(1);
	    else rcvd += ret;
	    ++cnt;

	    if (cnt > 50)
        {
            DBG("Error! rtn.msg='%s' GET_ERR", rtn.msg);
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, GET_ERR);
	    }
    } /* End while */

    buffer[len] = '\0';

#if defined(DEBUG) && defined(SAVE_PROTOCOL_DATA)
    {
           static int counter = 0;
           char file[64];
           int fd = 0;
           sprintf (file, "recvdata%03d" , counter++);
           if ((fd = open(file, O_WRONLY | O_CREAT, 0666)) > 0)
           {
                write(fd, buffer, len);
                close(fd);
           }
    }
#endif
    return(len);
} /* End get_string */


short get_short()
{
    char buffer[2];
    short ret;
    short len = 2;
    int rcvd = 0;
    int cnt = 0;

    DBG();
    while (rcvd < len)
    {
	    ret = SSL_read(ssl, buffer+rcvd, len-rcvd);
	    if (ret < 0)
	    {
            show_ssl_error(ssl, ret);
	        DBG("Error! ret='%d', rtn.msg='%s' GET_ERR", ret, rtn.msg);
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, GET_ERR);
	    }
	    rcvd += ret;
	    ++cnt;
	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        DBG("Error! rtn.msg='%s' GET_ERR", rtn.msg);
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, GET_ERR);
	    }
    } /* End while */

    ret = htons(*(short *)buffer);
    DBG("word=%d" , ret);
    return ret;
} /* End get_short */


void put_char(char data)
{
    int ret;

    DBG("data=%c data hex=0x%02x", data, data);

    if ((ret = SSL_write(ssl, &data, 1)) != 1)
    {
        show_ssl_error(ssl, ret);
        DBG("Error calling SSL_write() to send data='%s', rtn.msg='%s'", data, rtn.msg);
	    close(sock);
	    sprintf(rtn.msg, put_char_err, errno);
	    data_log(tr.log_file, rtn.msg);
        log3("Closing transaction ", tr.pid_string, ".");
	    exit(0);
    }

    DBG("hits the end of put_char(data='%s')", data);
}


void put_short(short data)
{
    int ret;
    int cnt;
    short wr_data;

    DBG("data='%d' for data in hex='0x%04x'", data, data);
    wr_data = htons((short)data);
    DBG("after call htons((short)data) wr_data='%d'", wr_data);

	if ((ret = SSL_write(ssl, &wr_data, sizeof(short))) != sizeof(short))
    {
        show_ssl_error(ssl, ret);

        DBG("first attempt to SSL_write() data='%d' fails with ret='%d', will try 50 more", data, ret);

        for (cnt = 0; cnt < 50; ++cnt)
        {
	        sleep(1);
                if ((ret = SSL_write(ssl, &wr_data, sizeof(short))) == sizeof(short)) {
                DBG("attempt cnt='%d' to call SSL_write() to send data='%d' fails again with ret='%d'", cnt, data, ret);
	            break;
            }
        } /* End for */

        if (ret != 2)
        {
            DBG("if ret('%d') != 2 then close transaction '%s'", ret, tr.pid_string);

	        close(sock);
	        log3("Closing ", "transaction ", tr.pid_string);
	        exit(0);
        }
    } /* End if for bad return */

    DBG("exiting put_short() with last ret='%d'", ret);
} /* End put_short */


/* Used to send data to the client */
int put_string(char *string)
{
    int len;
    int cnt;
    int ret;

    len = strlen(string);
    DBG("string='%s' and len=%d", string, len);

    put_short(len);

    if ((ret = SSL_write(ssl, string, len)) != len)
    {
        show_ssl_error(ssl, ret);
	    for (cnt = 0; cnt < 50; ++cnt)
	    {
	        sleep(1);
	        if ((ret = SSL_write(ssl, string, len)) == len)  break;
	    } /* End for */

        if (ret != len)
        {
	        sprintf(rtn.msg, put_err, errno, len, ret);
	        shut_down(rtn.msg, -1);
        }
    } /* End if for bad return */

    return(SUCCESS);
}


void put_long(long data)
{
    long wr_data;
    int ret;

    DBG();

    wr_data = htonl((long)data);
    if ((ret = SSL_write(ssl, &wr_data, 4)) != 4)
    {
        show_ssl_error(ssl, ret);
	    sprintf(rtn.msg, put_short_err, errno);
	    shut_down(rtn.msg, -1);
    }
}


short put_longlong(unsigned long long data)
{
    int ret = 0;
    DBG("data=%llu", data);

#ifdef LINUX
    if (send_longlong(data) < 0)
#else
    if ((ret = SSL_write(ssl, &data, 8)) < 0)
#endif
    {
        show_ssl_error(ssl, ret);
        sprintf(rtn.msg, put_short_err, errno);
        shut_down(rtn.msg, -1);
    } /* End if for bad send */
    else return(SUCCESS);
} /* End put_longlong */


/* Used to send data to the client */
void put_binary_string(char *string, int len)
{
    int ret;
    DBG();

    put_short(len);
    if ((ret = SSL_write(ssl, string, len)) != len)
    {
        show_ssl_error(ssl, ret);
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }
}


/* Concatinates two strings and sends to client */
/* All messages sent via put_string2 are logged */
void put_string2(char *string1, char *string2)
{
    int len;
    int ret;
    char string[BUFFER_SIZE];

    DBG();

    len = strcpy2(string, string1, string2);
    data_log(tr.log_file, string);
    put_short(len);

    if ((ret = SSL_write(ssl, string, len)) != len)
    {
        show_ssl_error(ssl, ret);
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }
}


/* Concatinates three strings and sends to client */
/* All messages sent via put_string3 are logged */
void put_string3(char *string1, char *string2, char *string3)
{
    int len;
    int ret;
    char string[BUFFER_SIZE];

    DBG();

    len = strcpy3(string, string1, string2, string3);
    put_short(len);
    if ((ret = SSL_write(ssl, string, len)) != len)
    {
        show_ssl_error(ssl, ret);
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }
}


long get_long()
{
    short ret;
    short len = 4;
    int rcvd = 0;
    int cnt = 0;
    char buffer[4];

    DBG();

    while (rcvd < len)
    {
	    ret = SSL_read(ssl, buffer+rcvd, len-rcvd);
        if (ret < 0)
        {
            show_ssl_error(ssl, ret);
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
	        shut_down(rtn.msg, GET_ERR);
	    }
    } /* End while */

    return(ntohl(*(long *)buffer));
} /* End get_long */


long long get_longlong(void)
{
    short ret;
    short len = 8;
    int rcvd = 0;
    int cnt = 0;
    static char buffer[8];

    DBG();

    while (rcvd < len)
    {
	    ret = SSL_read(ssl, buffer+rcvd, len-rcvd);
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
	        shut_down(rtn.msg, GET_ERR);
	    }
    } /* End while */
#ifdef BYTESWAP
{
    int cnt;
    int cnt1;
    char buffer1[8];
    for (cnt = 0, cnt1 = 7; cnt < 8; ++cnt, --cnt1)
    buffer1[cnt1] = buffer[cnt];
    return(*(long long *)buffer1);
}
#else
    return(*(long long *)buffer);
#endif
} /* End get_longlong */

