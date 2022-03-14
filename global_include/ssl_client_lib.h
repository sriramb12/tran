#ifndef ssl_client_lib_h
#define ssl_client_lib_h


short get_ssl_string(char *buffer);

/* Retrieves a string up to 6144 characters in length */
short get_ssl_big_string(char *buffer);

short get_ssl_short(void);

char get_ssl_char(void);

short put_ssl_short(short data);

short put_ssl_long(unsigned long data);

short put_ssl_longlong(unsigned long long data);

short put_ssl_string(char *string);

short put_ssl_binary_string(char *string, short len);

int get_ssl_time(void);

long get_ssl_long(void);

long long get_ssl_longlong(void);

#endif
