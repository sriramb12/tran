#ifndef client_lib_h
#define client_lib_h


short get_string(char *buffer);

/* Retrieves a string up to 6144 characters in length */
short get_big_string(char *buffer);

short get_short(void);

char get_char(void);

short put_short(short data);

short put_long(unsigned long data);

short put_longlong(unsigned long long data);

short put_string(char *string);

short put_binary_string(char *string, short len);

int get_time(void);

long get_long(void);

long long get_longlong(void);

#endif
