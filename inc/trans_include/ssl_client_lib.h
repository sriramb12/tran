#ifndef ssl_client_lib_h
#define ssl_client_lib_h

int send_longlong(unsigned long long data);

short get_ssl_string(char *buffer);

short get_ssl_dynamic_string(char **buffer);

char get_ssl_char(void);
short get_ssl_short(void);
short put_ssl_short(short data);

short put_ssl_long(unsigned long data);


short put_ssl_string(char *string);

short put_ssl_binary_string(char *string, short len);

long get_ssl_long(void);

short put_ssl_longlong(unsigned long long data);

long long get_ssl_longlong(void);

#endif
