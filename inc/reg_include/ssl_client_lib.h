#ifndef ssl_client_lib_h
#define ssl_client_lib_h

int send_longlong(unsigned long long data);

short get_string(char *buffer);

short get_dynamic_string(char **buffer);

char get_char(void);
short get_short(void);
short put_short(short data);

short put_long(unsigned long data);


short put_string(char *string);

short put_binary_string(char *string, short len);

long get_long(void);

short put_longlong(unsigned long long data);

long long get_longlong(void);

#endif
