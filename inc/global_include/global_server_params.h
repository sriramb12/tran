#ifndef global_server_params_h
#define global_server_params_h
#include "global_params.h"

/* Holds program name, version number, and password */
extern DB_SOFTWARE so;

/* Holds global parameters used only by the server */
extern TRANSACTION_PARAMS tr;

/* Holds temporary buffer space used by the database manager */
extern TMP_BUFFER bf;

int sock;

/* Indicates if email error message   sent for the current transaction */
/* Only one email error  message can be sent for each transaction */
extern int email_cnt;

/* Indicates how many log entries made for the current transaction */
extern int log_cnt;
#endif /* ifdef for global_server_params_h */
