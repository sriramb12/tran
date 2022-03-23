#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include "global_debug.h"


#if defined(TRANS_FTP)
#include "trans_server_globals.h"     //it defines extern TRANS_FILE_STAT fs;
#else
#include "trans_server_defines.h"
static TRANS_FILE_STAT fs;
#endif


static char trace_file_path[2048]  = {0};

static const char *current_time()
{
    struct timeb   timeb;
    static char m_currentDatetime[64];

    m_currentDatetime[0] = 0;
    timeb.time         = 0;
    if (ftime(&timeb) == 0)
    {
        struct tm * tm = localtime(&timeb.time);
        if (tm)
        {
            unsigned len = sprintf(m_currentDatetime, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                              tm->tm_year + 1900,
                              tm->tm_mon  + 1,
                              tm->tm_mday,
                              tm->tm_hour,
                              tm->tm_min,
                              tm->tm_sec,
                              timeb.millitm
                              );
            if(len >= sizeof(m_currentDatetime))
            {
                strcpy(m_currentDatetime, "buffer overflow");
            }
        }
    }

    return m_currentDatetime;
}


/*! \fn pid_t FAKE_fork()  Used for debuggng purposes, does not fork
 *
 *  please compile with -Dfork=FAKE_fork
 */
#if defined(fork)
# undef fork
#endif
pid_t FAKE_fork()
{
    return 0; // simulates a child process
}

/*!
     please compile with -Dexit=FAKE_exit
*/
#if defined(exit)
#undef exit
#endif
void FAKE_exit(int _exit_code)
{
}

/*! \fn _trace_log() generates log for macros DGB() anb FORCE_DBG
 *
 */
void _trace_log(const char *fileName, int lineNumber, const char *function, const char *format, ...)
{
#if defined(DEVEL_USING_SINGLE_PROCESS)
    static int first_time = 1;
#endif
static char m_msg[9216];
    int len;

    va_list ap;
    va_start(ap, format);

#if 0    // leave full path
    char *name = (char*)strrchr(fileName, '/');
    if(name)
    {
        name++;
    }
    else
    {
        name = (char*)fileName;
    }
#else
     const char *name = fileName;
#endif

     len  = snprintf(m_msg, sizeof(m_msg) -2, "%s k=%s pid=%-5d ppid=%-5d [%s:%d:%s()] ",
                                            current_time(),
                                            fs.key[0] ? fs.key : "NoKey",
                                            getpid(), getppid(), name, lineNumber, function);
    m_msg[len]  = 0;
    len += vsnprintf(m_msg+len, sizeof(m_msg) -2 -len, format, ap);
    va_end(ap);

    FILE * debug_file = NULL;

    if (trace_file_path[0] == 0)
    {
        const char *name =  "trace_debug.log";
         /*
           check for an option of saving trace log into an specific directory
              -DTRACE_LOG_PATH=\\\"/tmp\\\"  will save in /tmp for instance
          */
#if !defined(TRACE_LOG_PATH)   // not defined save trace_debug.log in the current directory
        if ( getcwd(trace_file_path, sizeof(trace_file_path) - 1) == NULL)
        {
            strcpy(trace_file_path, name);
        }
        else
#else
        strcpy(trace_file_path, TRACE_LOG_PATH);
#endif
        {
            if ((strlen(trace_file_path) + strlen(name) + 1) < sizeof(trace_file_path))
            {
               strcat(trace_file_path, "/");
               strcat(trace_file_path, name);
            }
            else
            {
                strcpy(trace_file_path, name);
            }
        }
    }

#if defined(DEVEL_USING_SINGLE_PROCESS)
    if (first_time)
    {
        debug_file = fopen( trace_file_path, "w" );
        first_time = 0;
    }
    else
#endif
    {
        debug_file = fopen( trace_file_path, "a" );
    }
    if (debug_file)
    {
        fprintf(debug_file, "%s\n", m_msg );
        fflush(debug_file);
        fclose( debug_file );
    }
}


void _dbg_wait_longer()
{
     DBG("Waiting 90 seconds..");
#if !defined(DEVEL_USING_SINGLE_PROCESS)
     sleep(90);
#endif
     DBG("entry point to attach PID");
}


void print_build_information(int argc, char *argv[])
{
#if defined(GIT_SHA) && defined(BUILD_DATE)
   printf("Build info: date [%s] git sha [%s]", BUILD_DATE, GIT_SHA);
#if defined(TRANSAPI_VERSION)
    printf(" api [%s]", TRANSAPI_VERSION);
#endif
    printf("\n");
#else
  (void)argc;
  (void)argv;
#endif

}
/*ec.files.fileuse*/
/*es.files.fileuse*/
/*ic.files.fileuse*/
/*is.files.fileuse*/
/*tc.files.fileuse*/
/*ts.files.fileuse*/
