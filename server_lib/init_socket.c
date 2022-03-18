/***************************************************************************
 *
 * Function:	init_socket
 *
 * Description:	Initialize the DRC database server socket that
 *		continuously waits for database requests from the 
 *		database clients.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include <sys/errno.h>
#include <strings.h>
#include <string.h>
#include "socket_includes.h"
#include <stdlib.h>
#include <unistd.h>
#include "global_debug.h" // Mazieri's debug facilities

/* Retrieves the exit status from the dying chilren */
void sig_child()
{
    struct rusage usage; 
    int status = 0; 

    DBG();

    /* Set up the signal again */
    /* This call is needed when using system V libraries */
    signal(SIGCLD, sig_child);

    /* Call wait3() on SIGCHLD until no more zombie processes remain.  */
    /* wait3 returns positive number if gets a signal from a dying child */
    /* It returns -1 if there are no more signals from dying children */
    while( wait3(&status, WNOHANG, &usage) > 0); 
        return;
} /* End sig_child */


void startup_daemon()
{
    DBG();

    int fd;
    int childpid;	/* process id of forked child */

    /* Ignore the SIGTTOU, SIGTTIN, and SIGTSTP signals */
    /* Do not accept terminal output */
#ifdef   SIGTTOU
    if (signal(SIGTTOU, SIG_IGN) == SIG_ERR)
    {
	    data_log("log/startup_errs", "startup_daemon:  signal(SIGTTOU, SIG_IGN) failed.\n");
	    exit(0);
    }
#endif

    /* Do not accept terminal input */
#ifdef   SIGTTIN
    if (signal(SIGTTIN, SIG_IGN) == SIG_ERR)
	{
	    data_log("log/startup_errs", "startup_daemon:  signal(SIGTTIN, SIG_IGN) failed.\n");
	    exit(0);
    }
#endif

    /* Do not accept control-C character */
#ifdef   SIGTSTP
    if (signal(SIGTSTP, SIG_IGN) == SIG_ERR)
    {
	    data_log("log/startup_errs", "startup_daemon:  signal(SIGTSTP, SIG_IGN) failed.\n");
	    exit(0);
    }
#endif

    /* Fork a child and let the parent exit. This guarantees the first child */
    /* is not a process group leader. */
    if ((childpid = fork()) < 0)
    {
	    data_log("log/startup_errs", "startup_daemon:  Unable to fork the first child.\n");
	    exit(0);
    }
    else if (childpid > 0)
        exit(0);			/* parent exits */

    /* First child process: */
    /* Disassociate from controlling terminal and process group. Ensure the */
    /* process can't reacquire a new controlling terminal. */
    /* SunOS is BSD */
    /* Solaris is system 5 */
#ifdef SUNOS
    if (setpgrp(0, getpid()) == -1)
    {
	    data_log("log/startup_errs", "startup_daemon:  Can't change process group.\n");
	    exit(0);
    }
    /* to lose controlling tty. */
    if ((fd = open("/dev/tty", O_RDWR)) >= 0)
    /*
      ioctl(fd, TIOCNOTTY, (char *) NULL);
    */

    /* First child continues as BSD daemon. */
#else				/* System V */
    if (setpgrp() == -1)
    {
	    data_log("log/startup_errs", "startup_daemon:  Can't change process group.\n");
	    exit(0);
    }

    if (signal(SIGHUP, SIG_IGN) == SIG_ERR)
    {
	    data_log("log/startup_errs", "startup_daemon:  signal(SIGHUP, SIG_IGN) failed.\n");
	    exit(0);
    }
    if ((childpid = fork()) < 0)
    {
	    data_log("log/startup_errs", "startup_daemon:  Unable to fork the second child.\n");
	    exit(0);
    }
    else if (childpid > 0)

    exit(0);			/* first child exits */
    /* Second child continues as a System V daemon. */
#endif

    /* Close all open file descriptors */
    for (fd = 3; fd < 16; fd++)
        close(fd);

    /* Clear any inherited file mode creation mask. */
    umask(0);
/* SunOS is BSD and Solaris is system 5 */
#ifdef SUNOS
   /*
    * Ignore the exit status of the children. With System V, all we need to
    * do is to ignore the signal.  With BSD, we have to catch each signal and
    * execute the wait3() syscall.
    */

    /* Ignore the return status of the zombie children */
    if (signal(SIGCLD, sig_child) == SIG_ERR)
    {
	    data_log("log/startup_errs", "startup_daemon:  signal(SIGCLD, sig_child) failed.\n");
	    exit(0);
    }
#else
    if (signal(SIGCLD, SIG_IGN) == SIG_ERR)
    {
	    data_log("log/startup_errs", "startup_daemon:  signal(SIGCLD, SIG_IGN) failed.\n");
	    exit(0);
    }
#endif
} /* End startup_daemon */
