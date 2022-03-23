#ifndef SSL_ENCRYPTION
/****************************************************************************
 *
 * Functions:	get_short, get_string, put_short, put_string
 *
 * Description:	Holds basic routines needed to read and write to a socket.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include "bytes.h"
#include "global_defines.h"
#include <string.h>
#include "global_params.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "global_debug.h" // Mazieri's debug facilities


/* Used by the data base server and mask shop server */
short get_string(char *buffer)
{
    short len;
    int rcvd = 0;
    register short cnt;
    int ret;

    DBG("buffer='%d'", sizeof(buffer));

    len = get_short();
    if ((len < SUCCESS) || (len > BLOCK_SIZE))
    {
	    close(sock);
	    sprintf(rtn.msg, bad_len_err, len);
	    shut_down(rtn.msg, BAD_LEN_ERR);
    }

    cnt = 0;
    while (rcvd < len) 
    {
	    ret = read(sock, buffer+rcvd, len-rcvd);
	    /* If didn't get anything, wait for a second and try again */
	    if (ret < 0)  sleep(1);
	    else rcvd += ret;
	
	    ++cnt;
	    if (cnt > 50)
        {
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, GET_ERR);
	    }
    } /* End while */
    
    buffer[len] = '\0';

    DBG("len='%d' AND buffer='%d'", len, sizeof(buffer));
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
	    ret = read(sock, buffer+rcvd, len-rcvd);
	    if (ret < 0) 
	    {
	        sprintf(rtn.msg, get_err, errno);
	        shut_down(rtn.msg, GET_ERR);
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

    return(htons(*(short *)buffer));
} /* End get_short */


void put_char(char data)
{
    int ret;

    DBG("data='%s'", data);

    if ((ret = send(sock, &data, 1, 0)) != 1) 
    {
	    close(sock);
	    sprintf(rtn.msg, put_char_err, errno);
	    data_log(tr.log_file, rtn.msg);
        log3("Closing transaction ", tr.pid_string, ".");
	    exit(0);
    }
}


void put_short(short data)
{
    int ret;
    int cnt;
    short wr_data;

    DBG("data='%d'", data);

    wr_data = htons((short)data);

	if ((ret = send(sock, &wr_data, 2, 0)) != 2)  
    {
        for (cnt = 0; cnt < 50; ++cnt)
        {
	        sleep(1);
	        if ((ret = send(sock, &wr_data, 2, 0)) == 2)  break;
        } /* End for */
        
        if (ret != 2)
        {
	        close(sock);
	        log3("Closing ", "transaction ", tr.pid_string);
	        exit(0);
        }
    } /* End if for bad return */
} /* End put_short */


/* Used to send data to the client */
int put_string(char *string)
{
    int len;
    int cnt;
    int ret;

    DBG("string='%s'", string);

    len = strlen(string);
    put_short(len);

    if ((ret = send(sock, string, len, 0)) != len) 
    {
	    for (cnt = 0; cnt < 50; ++cnt)
	    {
	        sleep(1);
	        if ((ret = send(sock, string, len, 0)) == len)  break;
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

    DBG("data='%ld'", data);

    wr_data = htonl((long)data);
    if ((ret = send(sock, &wr_data, 4, 0)) != 4) 
    {
	    sprintf(rtn.msg, put_short_err, errno);
    	shut_down(rtn.msg, -1);
    }
}


short put_longlong(unsigned long long data)
{
    DBG("data='%llu'", data);

#ifdef LINUX64
    if (send_longlong(data) < 0)
#else
    if (send(sock, &data, 8, 0) < 0) 
#endif
    {
	    sprintf(rtn.msg, put_short_err, errno);
	    shut_down(rtn.msg, -1);
    } /* End if for bad send */
    else return(SUCCESS);
} /* End put_longlong */


/* Used to send data to the client */
void put_binary_string(char *string, int len)
{
    DBG("string='%s' and len='%d'", string, len);

    int ret;
    put_short(len);

    if ((ret = send(sock, string, len, 0)) != len) 
    {
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }
}


/* Concatinates two strings and sends to client */
/* All messages sent via put_string2 are logged */
void put_string2(char *string1, char *string2)
{
    DBG("string1='%s' and string2='%s'", string1, string2);

    int len;
    int ret;
    char string[BUFFER_SIZE];

    len = strcpy2(string, string1, string2);
    data_log(tr.log_file, string);
    put_short(len);

    if ((ret = send(sock, string, len, 0)) != len) 
    {
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }
}


/* Concatinates three strings and sends to client */
/* All messages sent via put_string3 are logged */
void put_string3(char *string1, char *string2, char *string3)
{
    DBG("string1='%s', string2='%s' and string3='%s'", string1, string2, string3);

    int len;
    int ret;
    char string[BUFFER_SIZE];

    len = strcpy3(string, string1, string2, string3);
    put_short(len);

    if ((ret = send(sock, string, len, 0)) != len) 
    {
	    sprintf(rtn.msg, put_err, errno, len, ret);
	    shut_down(rtn.msg, -1);
    }
}


long get_long()
{
    DBG();

    short ret;
    short len = 4;
    int rcvd = 0;
    int cnt = 0;
    char buffer[4];

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
	        shut_down(rtn.msg, GET_ERR);
	    }
    } /* End while */

    return(ntohl(*(long *)buffer));
} /* End get_long */


long long get_longlong(void)
{
    DBG();

    short ret;
    short len = 8;
    int rcvd = 0;
    int cnt = 0;
    static char buffer[8];

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
	        shut_down(rtn.msg, GET_ERR);
	    }
    } /* End while */
#ifdef BYTESWAP
#ifdef LINUX64
{
    int cnt;
    int cnt1;
    char buffer1[8];
    for (cnt = 0, cnt1 = 7; cnt < 8; ++cnt, --cnt1)
    buffer1[cnt1] = buffer[cnt];
    return(*(long long *)buffer1);
}
#else
/* This is for 32 bit Linux */
    return(bswap_64(*(long long *)buffer));
#endif
#else
    return(*(long long *)buffer);
#endif
} /* End get_longlong */
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
#endif
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
