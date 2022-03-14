#ifndef global_macros_h
#define global_macros_h
#include "global_client_errs.h"
#include "rtn.h"
/* This file holds macros used by the client and user interface. */

#define CLOSE_DOWN(ret)\
{\
    rtn.flag = DB_ERR;\
    close(sock);\
    DB_ERRurn(ret);\
} /* End close_down */

#define CLOSE_DOWN_ERASE(cwd, ret)\
{\
    if (overwrite_file == DBNO) \
    erase_files(cwd, num, gu.dest_files);\
    rtn.flag = DB_ERR;\
    close(sock);\
    return(ret);\
} /* End CLOSE_DOWN_ERASE */

/* Inform calling function that bad data received from database server */
#define RETURN_BAD_PUT(); {\
    sprintf(rtn.msg, bad_put, errno, HELP_MSG);\
    rtn.flag = DB_ERR; \
    close(sock); \
    return(BAD_PUT); \
}
#define RETURN_BAD_GET(); {\
    sprintf(rtn.msg, bad_get, errno, HELP_MSG);\
    rtn.flag = DB_ERR; \
    close(sock); \
    return(BAD_GET); \
}

#endif /* ifdef for global_macros_h */
