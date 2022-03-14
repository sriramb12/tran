#ifndef rmt_server_h
#define rmt_server_h

#include "rmt_server.h"

short get_rmt_short();

void get_timed_short(char *buffer);


short put_rmt_short(short data);

short get_rmt_string(char *buffer);

/* Used to send data to the remote server */
short put_rmt_string(char *string);

short put_rmt_binary_string(
  char *string,
    register int len);


/* Connect to the remote socket */
/* If the connect is unsuccessful */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
short init_rmt_socket(
    char *header_buffer,
    RMT_SERVERS *rmt,
    int indx);		/* Index into the server table */

#endif
