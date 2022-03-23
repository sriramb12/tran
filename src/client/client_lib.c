#ifndef TRANSAPI
#ifndef SSL_ENCRYPTION
/****************************************************************************
 *
 * Name:	client_lib
 *
 * Description:	Low level get routines used by database clientss.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <malloc.h>
#include <math.h>
#ifdef WINDOWS_NT4
#define WIN32
  #define sleep
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "global_defines.h"
#include "global_client_errs.h"
#include "global_macros.h"
#include "global_params.h"
#include <sys/types.h>
     #include <stdlib.h>
#ifndef WINDOWS_NT4
     #include <alloca.h>
#endif
#include "rtn.h"
#include "client_lib.h"

#include "global_debug.h" // Mazieri's debug facilities


/* Needed for the user interface */
int server_num;

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
    return(send(sock, &buffer, 8, 0) ) ;
} /* end send_longlong */
#endif

short get_string(char *buffer)
{
    DBG();

    short len;
    short ret;
    int rcvd = 0;
    int cnt = 0;

  /* Get the length of the string that is coming */
    len = get_short();
    if (len > BLOCK_SIZE)
    {
	    sprintf(rtn.msg, bad_string_len, errno, len,   HELP_MSG);
	    return(BAD_GET);
    }

    if (len < 0)
    {
	    /* Indicate to user interface error was found */
	    rtn.flag = DB_ERR;

	    /* Construct the error message */
	    sprintf(rtn.msg, server_err, len);

	    return(BAD_GET);
    } /* End if for length field less than 0 */

    /* Read until that many bytes have been received */
    while (rcvd < len)
    {
	    ret = recv(sock, buffer+rcvd, len-rcvd, 0);
	    if (ret < 0)
	        sleep(2);
	    else
	        rcvd += ret;

	    ++cnt;

	    /* Only try to get the string 50 times */
	    if (cnt > 50)
	    {
	        RETURN_BAD_GET();
	    }
    } /* End while */

    /* Terminate the string */
    buffer[len] = '\0';

    return(len);
}  /* End get_string */

short get_big_string(char *buffer)
{
    DBG();
  short len;
    short ret;
  int rcvd = 0;
    int cnt = 0;

  /* Get the length of the string that is coming */
    len = get_short();
    if (len > BLOCK_SIZE)
    {
	sprintf(rtn.msg, bad_string_len, errno, len,   HELP_MSG);
	return(BAD_GET);
    }
    if (len < 0)
    {
	/* Indicate to user interface error was found */
	rtn.flag = DB_ERR;
	/* Construct the error message */
	sprintf(rtn.msg, server_err, len);
	return(BAD_GET);
    } /* End if for length field less than 0 */
  /* Read until that many bytes have been received */
    while (rcvd < len) 
    {
	ret = recv(sock, buffer+rcvd, len-rcvd, 0);
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
    buffer[len] = '\0';
    return(len);
}  /* End get_string */

short get_dynamic_string(char **buffer)
{
    DBG();
  short len;
    short ret;
  int rcvd = 0;
    int cnt = 0;

  /* Get the length of the string that is coming */
    len = get_short();
    if (len < 0)
    {
	/* Indicate to user interface error was found */
	rtn.flag = DB_ERR;
	/* Construct the error message */
	sprintf(rtn.msg, server_err, len);
	return(BAD_GET);
    } /* End if for length field less than 0 */
  /* Read until that many bytes have been received */
    *buffer = malloc(len+1);
    while (rcvd < len) 
    {
	ret = recv(sock, *buffer+rcvd, len-rcvd, 0);
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
}  /* End get_string */

char get_char(void)
{
    DBG();
    char buffer[1];
    short ret;
    short len = 1;
    int rcvd = 0;
    int cnt = 0;

    while (rcvd < len) 
    {
	ret = recv(sock, buffer+rcvd, len-rcvd, 0);
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
} /* End get_char */

short get_short(void)
{
    DBG();
    char buffer[2];
    short ret;
    short len = 2;
    int rcvd = 0;
    int cnt = 0;

    while (rcvd < len) 
    {
	ret = recv(sock, buffer+rcvd, len-rcvd, 0);
	if (ret < 0)
	sleep(1);
	else rcvd += ret;
	++cnt;
	/* Only try to get the string 50 times */
	if (cnt > 50)
	{
	    RETURN_BAD_GET();
	}
    } /* End while */
    return(ntohs(*(short *)buffer));
} /* End get_short */


short put_short(short data)
{
    DBG();
  short wr_data;
    short ret;

  /* Put the word into network byte order and send it */
  wr_data = htons((short)data);
  if ((ret = send(sock, &wr_data, 2, 0)) < 0) {
    RETURN_BAD_PUT();
  }
    return(SUCCESS);
}

short put_long(unsigned long data)
{
    DBG();
  long wr_data;

  wr_data = htonl((long)data);
    if (send(sock, &wr_data, 4, 0) < 0) 
    {
	sprintf(rtn.msg, bad_put, errno, HELP_MSG);
	rtn.flag = DB_ERR; 
	close(sock); 
	return(BAD_PUT); 
}
    return(SUCCESS);
} /* end put_long */


short put_string(char *string)
{
    DBG();
  int len;

    /* Get the length of the string */
    len = strlen(string);
    /* Send the length */
    if (put_short(len) != 0)
    {
	RETURN_BAD_PUT();
    }
    /* And send the string */
    if ( len > 0 )
    {
        if (send(sock, string, len, 0) < 0) {
          RETURN_BAD_PUT();
        }
    }
    return(SUCCESS);
} /* end put_string */

short put_binary_string(char *string, short len)
{
    DBG();

    /* Send the length */
    if (put_short(len) != 0)
    {
	RETURN_BAD_PUT();
    }
    /* And send the string */
    if ( len > 0 )
    {
        if (send(sock, string, len, 0) < 0) {
          RETURN_BAD_PUT();
        }
    }
    return(SUCCESS);
} /* end put_binary_string */

long get_long(void)
{
    DBG();
    char buffer[4];
    short ret;
    short len = 4;
    int rcvd = 0;
    int cnt = 0;

    while (rcvd < len) 
    {
	ret = recv(sock, buffer+rcvd, len-rcvd, 0);
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
} /* End get_long */

short put_longlong(unsigned long long data)
{
    DBG();
    int ret;

#ifdef BYTESWAP
    ret = send_longlong(data);
#else
    ret = send(sock, &data, 8, 0);
#endif
    if (ret < 0)
    {
	sprintf(rtn.msg, bad_put, errno, HELP_MSG);
	rtn.flag = DB_ERR; 
	close(sock); 
	return(BAD_PUT); 
    } /* End if for bad send */
    else return(SUCCESS);
} /* End put_longlong */

long long get_longlong(void)
{
    DBG();
    char buffer[8];
    short ret;
    short len = 8;
    int rcvd = 0;
    int cnt = 0;

    while (rcvd < len) 
    {
	ret = recv(sock, buffer+rcvd, len-rcvd, 0);
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
} /* End get_longlong */
#endif
#endif /* For not TRANSAPI */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
