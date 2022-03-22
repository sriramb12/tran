/****************************************************************************
 *
 * Function:	read_with_timeout
 *
 * Description:	Sets alarm and exits if alarm goes off.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/uio.h>
#include <string.h>
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "rtn.h"

#include "global_debug.h" // Mazieri's debug facilities

#define TIMEOUT_DEFAULT      90  /* seconds */
#define TIMEOUT_CANCEL        0  /* value for alarm() to cancel the current timeout */
#define ERR_TIMEOUT          -2
#define ERR_IO               -1

/* Used in this file only to indicate the read timed out */
static int timeout_flag;
extern int sock;

/*
 * Signal handler for timeouts (SIGALRM).
 * This function is called when the alarm() value that was set
 * counts down to zero. This indicates that we haven't received a
 * response. The function sets timeout_flag to indicate a timeout has occurred.
 * The signal interrupts the read system call which sets errno = EINTR.
 */
void timeout( int rcv_signal )
{
    DBG();

    timeout_flag = DBYES;
    strcpy(rtn.msg, timeout_err);
}

int read_with_timeout( fd,  buf, num_bytes )
    int fd;
    char *buf;
    int num_bytes;
{
   int ret;
      
   DBG();

   /* Setup the read with timeout mechanism
    * set signal handler for the alarm that is set to prevent a blocking
    * read that would never return. Reset the flag to indicate a timeout
    * occurred and set the alarm so the countdown begins.
    */
    /* Let system know we want read interrupted with the SIGALRM function */
/*
    siginterrupt(SIGALRM, 1);
*/
    signal( SIGALRM, timeout );
    timeout_flag = DBNO;
    alarm( TIMEOUT_DEFAULT );
    ret =  read( fd, buf, num_bytes );
    if (ret < 0)
    {
	if (timeout_flag == DBYES)
	ret = TIMEOUT_ERR;
    } /* End if */
    /* Cancel the alarm if it has not expired  */
    if (timeout_flag == DBNO)
	alarm( TIMEOUT_CANCEL );
   return ret;
}
