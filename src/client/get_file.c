#ifndef TRANSAPI
/****************************************************************************
 *
 * Function:	get_file
 *
 * Description:	Retrieve a big file from the Transcend/InterCom database.
 *		Also retrieves history of big files.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#ifdef WINDOWS_NT4
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <netinet/in.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include "global_defines.h"
#include "global_params.h"
#include "hash.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "client_lib.h"
#include "global_debug.h"


/* routine for everything except Transcend GUI */
#ifndef TRANSCEND_GUI
int get_file(
    REQ_TYPE req_type,
    char *file_name,
    unsigned long mode,
     char file_type)
{
    DBG();

    short ret;
    FILE *fp;
    char buffer[BUFFER_SIZE];

     if (file_type == BINARY)
    fp = fopen(file_name, "wb");
    else fp = fopen(file_name, "w");
     if (fp == NULL)
    {
#ifdef TRANSCEND
	sprintf(rtn.msg, bad_create, errno, file_name, "Transcend", HELP_MSG);
#endif
#ifdef INTERCOM
	sprintf(rtn.msg, bad_create, errno, file_name, "InterCom", HELP_MSG);
#endif
	/* Tell server unable to open file */
	put_short(BAD_CREATE);
	get_ret_code();
	/* Send error message to server */
		return(BAD_CREATE);
    } /* End if for failure to open file */
    put_short(SUCCESS);
    while ((ret = get_string(buffer)) > 0)
    {
	if (fwrite(buffer, 1, ret, fp) != ret)
	{
	    strcpy7(rtn.msg, bad_write, RET2TAB, "'", file_name, "'.", DOUBLE_RET, HELP_MSG);
	    put_short(BAD_WRITE);
	    close(sock);
    fclose(fp);
	        unlink(file_name);
	    return(BAD_WRITE);
	}
	if (req_type != DBHIST_DATA)
	tfile_hash (ret, 0);
	} /* end while */
    fclose(fp);

    /* Set the file mode */
    if (file_type != DIRECTORY) {
        DBG("chmod(file_name, mode)", file_name, mode);
        chmod(file_name, mode);
    }

    ret = get_ret_code();
    if (req_type != DBHIST_DATA)
  PRINTF("\n");
    return(ret);
} /* end get_file */
#endif

/* Version for Transcend GUI */
#ifdef TRANSCEND_GUI
int get_file(
    REQ_TYPE req_type,
    char *file_name,
    unsigned long mode,
     char file_type)
{
    DBG();

    short ret;
    char buffer[BUFFER_SIZE];

    put_short(SUCCESS);
    while ( 0 != (ret = get_string(buffer)) )
    {
	if (fwrite(buffer, 1, ret, stdout) != ret)
	{
	    strcpy7(rtn.msg, bad_write, RET2TAB, "'", file_name, "'.", DOUBLE_RET, HELP_MSG);
	    put_short(BAD_WRITE);
	    close(sock);
	    return(BAD_WRITE);
	}
	} /* end while */

    ret = get_ret_code();
    if (ret == 0) return(0);
    if (req_type != DBHIST_DATA)
  PRINTF("\n");
    return(ret);
} /* end get_file */
#endif
#endif /* For not TRANSAPI */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
