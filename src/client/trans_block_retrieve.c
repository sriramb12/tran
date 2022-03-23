#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	trans_block_retrieve
 *
 * Description:	Retrieve a compressed file from the Transcend/InterCom repository.
 *		Also retrieves history of big files.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "global_defines.h"
#include "global_params.h"
#include "ssl_client_lib.h"
#include "trans_hash.h"
#include "trans_defines.h"
#include "trans_client_errs.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "client_lib.h"
#include "transapi_prototypes.h"
#include <zlib.h>

#include "global_debug.h"

#define HELP_MSG "For assistance, contact Intercom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"


int trans_block_retrieve(gzFile file, FILE *fp,
    TRANS_FILE_TYPE file_type,
    char *file_name,
    char get_block,
    int index)
{
    DBG();

    char buffer[BUFFER_SIZE];
    int ret;
    int len;

	    while ( (ret = tfile_read(file, buffer, get_block, index)) > 0) 
	    {
		if ((len = fwrite(buffer, 1, ret, fp))!= ret)
		{
		    char *err_ptr = strerror(errno);
		    char err_string[8];
		    sprintf(err_string, "%d", errno);
		    rtn.len = strcpy5(rtn.msg, bad_write, RET2TAB, "'", 
		    file_name, "'.");
		    strcpy7(rtn.msg+rtn.len,DOUBLE_RET, "Error message:  ", err_ptr,
		    "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
		    put_ssl_short(BAD_WRITE);
		    close(sock);
	#ifdef do_tar
		    if (file_type == DIRECTORY) pclose(fp);
		    else
	#endif
		    fclose(fp);
		        unlink(file_name);
		    return(BAD_WRITE);
		}
    if (get_block == DBYES) break;
		} /* end while */
    return(SUCCESS);
} /* end trans_block_retrieve */

#endif
