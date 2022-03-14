/****************************************************************************
 *
 * Function:	block_retrieve
 *
 * Description:	Retrieve a compressed file from the DFM client.
 *		Written to work with version 1.1.3 of the gzip library.
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
#include <strings.h>
#include <string.h>
#include "global_client_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "gzio.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "server_lib.h"
#include "zlib.h"
#include "global_debug.h"


long long block_retrieve(gzFile file, FILE *fp, TRANS_FILE_TYPE file_type, char *file_name, char get_block)
{
    char buffer[BUFFER_SIZE];
    int ret;
    long long size = 0;

    DBG("file_name='%s', get_block='%s'", file_name, get_block);

    while ( (ret = trans_read(file, buffer, get_block)) != 0) 
    {
        if (fwrite(buffer, 1, ret, fp)!= ret)
        {
            char *err_ptr = strerror(errno);
            char err_string[8];

            sprintf(err_string, "%d", errno);
            rtn.len = strcpy5(rtn.msg, bad_write, RET2TAB, "'", file_name, "'.");

            strcpy7(rtn.msg+rtn.len,DOUBLE_RET, "Error message:  ", err_ptr, "\nError No:      ", err_string, DOUBLE_RET, help_msg_print());

            put_short(BAD_WRITE);
            close(sock);

	        /* These are actually tar files and parts of directories */
            if (file_type == DIRECTORY) pclose(fp);
            else fclose(fp);
                
            unlink(file_name);
            return(BAD_WRITE);
        }
    
        size += ret;
        if (get_block == DBYES) break;
    } /* end while */

    return(size);
} /* end block_retrieve */

