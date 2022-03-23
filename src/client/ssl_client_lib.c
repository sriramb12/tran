#ifdef SSL_ENCRYPTION
/****************************************************************************
 *
 * Name:	ssl_client_lib
 *
 * Description:	Low level SSL get routines used by database clientss.
 *
 ***************************************************************************/

#include <malloc.h>
#include <math.h>
#define closesocket close
#ifndef WINDOWS_NT4
     #include <alloca.h>
#endif /* End if not Windows */
#include <openssl/ssl.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <stdlib.h>
#include "global_defines.h"
#include "global_params.h"
#include "rtn.h"
#include "ssl_client_lib.h"

#include "global_client_errs.h"
#include "global_macros.h"
#include "common.h"
#include "trans_client_errs.h"

#include "global_debug.h" // Mazieri's debug facilities

#define HELP_MSG "For assistance, contact Intercom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"


/* Needed for the user interface */
int server_num;
extern SSL *ssl;

#ifdef BYTESWAP

int send_longlong(unsigned long long data)
{
    DBG();
    unsigned long long buffer;

  buffer =    (((data) >> 56) & 0x00000000000000FFll) \
      +  (((data) >> 40) & 0x000000000000FF00ll) \
      +  (((data) >> 24) & 0x0000000000FF0000ll) \
      +  (((data) >> 8)  & 0x00000000FF000000ll) \
      +  (((data) << 8)  & 0x000000FF00000000ll) \
      +  (((data) << 24) & 0x0000FF0000000000ll) \
      +  (((data) << 40) & 0x00FF000000000000ll) \
      +  (((data) << 56) & 0xFF00000000000000ll) \
  ;
    return(SSL_write(ssl, &buffer, 8)) ;
} /* end send_longlong */
#endif /* for BYTESWAP */

short get_ssl_string(char *buffer)
{
    short len;
    short ret;
    int rcvd = 0;
    int cnt = 0;

    DBG("buffer size '%u'", (unsigned)strlen(buffer));
    DBG("rtn.msg\n'%s'\n", rtn.msg);

    /* Get the length of the string that is coming */
    len = get_ssl_short();

    if (len > BLOCK_SIZE)
    {
	    sprintf(rtn.msg, bad_string_len, errno, len,   HELP_MSG);

        DBG("Error len > BLOCK_SIZE where len='%d' and rtn.msg='%s'", len, rtn.msg);
	    return(BAD_GET);
    }

    if (len < 0)
    {
	    /* Indicate to user interface error was found */
	    rtn.flag = DB_ERR;

	    /* Construct the error message */
	    sprintf(rtn.msg, server_err, errno, HELP_MSG);

        DBG("Error len < 0 where len='%d' and rtn.msg='%s'", len, rtn.msg);
	    return(BAD_GET);
    } /* End if for length field less than 0 */

    /* Read until that many bytes have been received */
    while (rcvd < len)
    {
	    ret = SSL_read(ssl, buffer+rcvd, len-rcvd);
	    if (ret < 0)
	        sleep(2);
	    else
	        rcvd += ret;

	    ++cnt;
	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        DBG("will return bad get");
	        RETURN_BAD_GET();
	    }
    } /* End while */

    /* Terminate the string */
    buffer[len] = '\0';

    DBG("SUCCESS, will return len='%d'", len);
    return(len);
}  /* End get_ssl_string */


short get_ssl_big_string(char *buffer)
{
    DBG();
    short len;
    short ret;
    int rcvd = 0;
    int cnt = 0;

    /* Get the length of the string that is coming */
    len = get_ssl_short();
    if (len > BIG_BLOCK_SIZE)
    {
        DBG("the length of the string that is coming is big the BIG_BLOCK_SIZE, len='%d'", len);
	    sprintf(rtn.msg, bad_string_len, errno, len, HELP_MSG);
	    DBG("will return BAD_GET, with rtn.msg='%s'", rtn.msg);
	    return(BAD_GET);
    }

    if (len < 0)
    {
        DBG("the length of the string that is coming is less then 0, len='%d'", len);
	    /* Indicate to user interface error was found */
	    rtn.flag = DB_ERR;

	    /* Construct the error message */
	    sprintf(rtn.msg, server_err, errno, HELP_MSG);
	    DBG("will return BAD_GET, with rtn.msg='%s'", rtn.msg);
	    return(BAD_GET);
    } /* End if for length field less than 0 */

    /* Read until that many bytes have been received */
    while (rcvd < len) 
    {
	    ret = SSL_read(ssl, buffer+rcvd, len-rcvd);
	    
	    if (ret < 0) sleep(2);
	    else rcvd += ret;
	
	    ++cnt;
	
	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        DBG("tried to get SSL_read() 50 times without success, calling return_bad_get");
	        RETURN_BAD_GET();
	    }
    } /* End while */

    /* Terminate the string */
    buffer[len] = '\0';
    DBG("SUCCESS, len='%d'", len); 
    return(len);
}  /* End get_ssl_big_string */


short get_ssl_dynamic_string(char **buffer)
{
    DBG();
  short len;
    short ret;
  int rcvd = 0;
    int cnt = 0;

  /* Get the length of the string that is coming */
    len = get_ssl_short();
    if (len < 0)
    {
	/* Indicate to user interface error was found */
	rtn.flag = DB_ERR;
	/* Construct the error message */
	sprintf(rtn.msg, server_err, errno, HELP_MSG);
	return(BAD_GET);
    } /* End if for length field less than 0 */
  /* Read until that many bytes have been received */
    *buffer = malloc(len+1);
    while (rcvd < len) 
    {
	ret = SSL_read(ssl, *buffer+rcvd, len-rcvd);
	if (ret < 0)
	sleep(2);
	else rcvd += ret;
	++cnt;
	/* Only try to get the string 50 times */
	if (cnt > 50)
	{
	    RETURN_BAD_GET();
	}
    } /* End while */
    /* Terminate the string */
    (*buffer)[len] = '\0';
    return(len);
}  /* End get_ssl_dynamic_string */

char get_ssl_char(void)
{
    DBG();
    char buffer[1];
    short ret;
    short len = 1;
    int rcvd = 0;
    int cnt = 0;

    while (rcvd < len) 
    {
	ret = SSL_read(ssl, buffer+rcvd, len-rcvd);
	if (ret < 0)
	sleep(1);
	else rcvd += ret;
	++cnt;
	/* Only try to get the character 50 times */
	if (cnt > 50)
	{
	    RETURN_BAD_GET();
	}
    } /* End while */
    return(buffer[0]);
} /* End get_ssl_char */

short get_ssl_short(void)
{
    DBG();

    char buffer[2];
    short ret;
    short len = 2;
    int rcvd = 0;
    int cnt = 0;

    while (rcvd < len) 
    {
	    ret = SSL_read(ssl, buffer+rcvd, len-rcvd);

	    if (ret < 0) sleep(1);
	    else rcvd += ret;

	    ++cnt;

	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        DBG("tried to get SSL_read() 50 times without success, calling return_bad_get");
	        RETURN_BAD_GET();
	    }
    } /* End while */

    DBG("buffer='%d'", buffer);
    return(ntohs(*(short *)buffer));
} /* End get_ssl_short */


short put_ssl_short(short data)
{
    short wr_data;

    DBG("data='%d'", data);

    /* Put the word into network byte order and send it */
    wr_data = htons((short)data);

    if (SSL_write(ssl, &wr_data, 2) < 0) {
        DBG("SSL_write() failed to send wr_data='%d'", wr_data);
        RETURN_BAD_PUT();
    }

    DBG("SUCCESS transfer data='%d', wr_data='%d'", data, wr_data);
    return(SUCCESS);
}

short put_ssl_long(unsigned long data)
{
    DBG("data='%lu'", data);
    long wr_data;

    wr_data = htonl((long)data);

    if (SSL_write(ssl, &wr_data, 4) < 0) 
    {
        DBG("SSL_write() error sending data='%lu', wr_data='%lu', rtn.msg='%s'", data, wr_data, rtn.msg);
	    sprintf(rtn.msg, bad_put, errno, HELP_MSG);
	    rtn.flag = DB_ERR; 
	    close(sock); 
	    return(BAD_PUT); 
    }
    
    DBG("SUCCESS transfer data='%lu', wr_data='%lu'", data, wr_data);
    return(SUCCESS);
} /* end put_ssl_long */


short put_ssl_string(char *string)
{
    int len;

    /* Get the length of the string */
    len = strlen(string);

    DBG("string='%s' (len='%d')", string, len);

    /* Send the length */
    if (put_ssl_short(len) != 0)
    {
        DBG("Failed to send string len='%d'", len);
	    RETURN_BAD_PUT();
    }

    /* And send the string */
    if ( len > 0 )
    {
        if ( SSL_write(ssl, string, len) < 0) {
            DBG("Failed to send string='%s'", string);
            RETURN_BAD_PUT();
        }
    }

    DBG("SUCCESS transfer string='%s' of len='%d'", string, len);
    return(SUCCESS);
} /* end put_ssl_string */


short put_ssl_binary_string(char *string, short len)
{
    DBG("string of len='%d'", len);

    /* Send the length */
    if (put_ssl_short(len) != 0)
    {
	    RETURN_BAD_PUT();
    }

    /* And send the string */
    if ( len > 0 )
    {
        if (SSL_write(ssl, string, len) < 0) {
            RETURN_BAD_PUT();
        }
    }

    DBG("SUCCESS");
    return(SUCCESS);
} /* end put_ssl_binary_string */

long get_ssl_long(void)
{
    DBG();

    char buffer[4];
    short ret;
    short len = 4;
    int rcvd = 0;
    int cnt = 0;

    while (rcvd < len) 
    {
	    ret = SSL_read(ssl, buffer+rcvd, len-rcvd);
	    if (ret < 0) {
	        RETURN_BAD_GET();
	    }
	    rcvd += ret;
	    ++cnt;
	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        RETURN_BAD_GET();
	    }
    } /* End while */

    return(ntohl(*(long *)buffer));
} /* End get_ssl_long */

short put_ssl_longlong(unsigned long long data)
{
    DBG();
    int ret;

#ifdef BYTESWAP
    ret = send_longlong(data);
#else
    ret = SSL_write(ssl, &data, 8);
#endif
    if (ret < 0)
    {
	sprintf(rtn.msg, bad_put, errno, HELP_MSG);
	rtn.flag = DB_ERR; 
	close(sock); 
	return(BAD_PUT); 
    } /* End if for bad send */
    else return(SUCCESS);
} /* End put_ssl_longlong */

long long get_ssl_longlong(void)
{
    DBG();
    char buffer[8];
    short ret;
    short len = 8;
    int rcvd = 0;
    int cnt = 0;

    while (rcvd < len) 
    {
	ret = SSL_read(ssl, buffer+rcvd, len-rcvd);
	if (ret < 0) {
	    RETURN_BAD_GET();
	}
	rcvd += ret;
	++cnt;
	/* Only try to get the string 50 times */
	if (cnt > 50)
	{
	    RETURN_BAD_GET();
	}
    } /* End while */
    return(*(long long *)buffer);
} /* End get_ssl_longlong */

#endif /* For SSL encryption */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
