#ifndef _GLOBAL_INCLUDE_H
#define _GLOBAL_INCLUDE_H

#include <sys/types.h>
#include <unistd.h>

#define RETURN_INT_DBG(x)         {DBG("returning %d", x)   ; return x;}
#define RETURN_UINT_DBG(x)        {DBG("returning %u", x)   ; return x;}
#define RETURN_LONG_DBG(x)        {DBG("returning %ld", x)  ; return x;}
#define RETURN_ULONG_DBG(x)       {DBG("returning %lu", x)  ; return x;}
#define RETURN_LONGLONG_DBG(x)    {DBG("returning %lld", x) ; return x;}
#define RETURN_ULONGLONG_DBG(x)   {DBG("returning %llu", x) ; return x;}

void _dbg_wait_longer();

/**
 *  Use -DDEBUG compile option to activate DBG() and DBG_WAIT_LONGER() macros
 *
 *  DBG_WAIT_LONGER() is used where it is necessary to have a break point for debugging in gdb
 *                    once the message appears it is possible to attach de debug at the current process ID
 *
 *  to bypass this DBG_WAIT_LONGER() macro used -DAVOID_DBG_WAIT
 **/

#if defined(DEBUG) || defined(ENABLE_LOGS)
void _trace_log(const char *fileName, int lineNumber, const char *function, const char *fmt, ...);
void _dbg_wait_longer();
# define DBG(fmt, ...)         _trace_log(__FILE__, __LINE__, __func__, fmt " ", ##__VA_ARGS__)
# ifndef AVOID_DBG_WAIT
# define DBG_WAIT_LONGER()     _dbg_wait_longer()
# else
# define DBG_WAIT_LONGER() /* */
#endif //AVOID_DBG_WAIT
#else
# define DBG(fmt, ...) /* */
# define DBG_WAIT_LONGER() /* */
#endif // defined(DEBUG)

#if !defined(DEBUG) && defined(TRANS_FTP)
# define FORCE_DEBUG
#endif

/**  use FORCE_DBG() instead of DBG() for messages intended to be present at release version
 *
 *   compile with -DFORCE_DEBUG
 */

#if defined(FORCE_DEBUG) || defined(DEBUG)
void _trace_log(const char *fileName, int lineNumber, const char *function, const char *fmt, ...);
# define FORCE_DBG(fmt, ...)         _trace_log(__FILE__, __LINE__, __func__, fmt " ", ##__VA_ARGS__)
#else
# define FORCE_DBG(fmt, ...) /* */
#endif

pid_t FAKE_fork();
void  FAKE_exit(int);
void  print_build_information(int argc, char *argv[]);


#endif //_GLOBAL_INCLUDE_H
