/****************************************************************************
 *
 * Function:	close_down
 *
 * Description:	Routines to print error messages and and return failure.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "global_client_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "intercom_single_msg.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_prototypes.h"

#include "global_debug.h"

#define HELP_MSG "For assistance, contact Intercom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"


int trans_exit( char *msg, int err_code)
{
    DBG();
#ifdef TRANSCEND
    extern int shm_id;
#endif

    close(sock);
    /* Get rid of shared memory */
#ifdef TRANSCEND
#ifndef WINDOWS_NT4
    if (shm_id > 0)
    shared_memory_release();
#endif
#endif
#ifdef SEND2FOUNDRY
    status_log("transfer_msg", rtn.msg);
#else
    strcpy(rtn.msg, msg);
#endif
    return(err_code);
} /* end trans_exit */

int close_down1f(char *err_msg, char *file, int err_no)
{
    DBG();

    strcpy7(rtn.msg, err_msg, RET2TAB, "'", file, "'", DOUBLE_RET, HELP_MSG);
    rtn.flag = DB_ERR;

    return(err_no);
} /* End close_down1f */

int close_down1m(char *err_msg, char *file, char *x500, register short err_no)
{
    DBG();
    char msg[FILE_SIZE];
    char subject[80];

    strcpy5(msg, err_msg, RET2TAB, "'", file, "'");
	sprintf(subject, "Error %d from %s", err_no, so.program);
	email_send( "transbug@nxp.com", x500, subject, msg);
    return(err_no);
} /* End close_down1m */

int trans_data_log(char *log_file, char *string)
{
    DBG();
    FILE *log_ptr;

    if ((log_ptr = fopen(log_file, "a")) == NULL)
    {
	printf("Could not log message '%s' in file '%s'\n", string, log_file);
	return(FAILURE);
    }
    fprintf(log_ptr, "%s\n", string);
    fflush(log_ptr);
    fclose(log_ptr);
    return(SUCCESS);
} /* End trans_data_log */

int trans_data_log2(char *log_file, char *string1, char *string2)
{
    DBG();
    FILE *log_ptr;

    if ((log_ptr = fopen(log_file, "a")) == NULL)
    {
	printf("Could not log message '%s%s' in file '%s'\n", string1, string2, log_file);
	return(FAILURE);
    }
    fprintf(log_ptr, "%s%s\n", string1, string2);
    fflush(log_ptr);
    fclose(log_ptr);
    return(SUCCESS);
} /* End trans_data_log2 */

int status_log(char *log_file, char *string)
{
    DBG();
    FILE *log_ptr;

    if ((log_ptr = fopen(log_file, "w")) == NULL)
    {
	printf("Could not log message '%s' in file '%s'\n", string, log_file);
	return(FAILURE);
    }
    fprintf(log_ptr, "%s", string);
    fflush(log_ptr);
    fclose(log_ptr);
    return(SUCCESS);
} /* End status_log */

/* Build message when file could not be opened for read */
int bad_rd_msg_build(char *file_name)
    {
    DBG();
    char *err_ptr = strerror(errno);
    char err_string[8];

    sprintf(err_string, "%d", errno);
rtn.len = strcpy4(rtn.msg, bad_rd_open, RET2TAB_QUOTE,
file_name, "'\n\n");
    strcpy6(rtn.msg+rtn.len,"Error message: ", err_ptr,
    "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
return(BAD_RD_OPEN);
} /* End bad_rd_msg_build */

/* Print errors and return error*/
int shut_down(char *string, int err_code)
  {
    DBG();
    strcpy(rtn.msg, string);
    return(err_code);
} /* end shut_down */


/* Print output */
int trans_print(char *msg, int err_code)
{
    DBG("err_code='%d', msg='%s'", err_code, msg);
    /* TODO: this is just to calm down GCC. Please remove it in the future 
     * (we probably is not using trans_print() anyway)
     * */
    char *tmp; strncpy(tmp, msg, 1);

#ifdef garbage
    if (trans[index].display_transfer_status == 0)
        sprintf(rtn.msg, msg);
    else 
        PRINTF(msg);
#endif
    return(err_code);
} /* end trans_print */
