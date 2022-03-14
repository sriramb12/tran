#ifndef server_lib_h
#define server_lib_h

/* Used by the data base server and mask shop server */
short get_string(char *buffer);

short get_short();

void put_char(char data);

void put_short(short data);

/* Used to send data to the client */
int put_string(char *string);

void put_long(long data);

/* Used to send data to the client */

void put_binary_string(char *string, int len);
/* Concatinates two strings and sends to client */
/* All messages sent via put_string2 are logged */
void put_string2(char *string1, char *string2);

/* Concatinates three strings and sends to client */
/* All messages sent via put_string3 are logged */
void put_string3(char *string1, char *string2, char *string3);

long get_long();

long long get_longlong();

#endif
