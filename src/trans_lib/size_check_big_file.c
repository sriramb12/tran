/****************************************************************************
 *
 * Function:	size_check_big_file
 *
 * Description:	checks original size of file stored in global_data 
	against actual size of file
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <strings.h>
#include <string.h>
#include "server_lib.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_server_globals.h"
#include "trans_server_macros.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

void size_check_big_file(char *key)
{
    char file_size_string[32];
    char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    extern TRANS_FILE_STAT fs;
    struct stat buf;
    FILE *fp;

    DBG();

    /* Open and read global file */
    fp = read_big_file(key, lines, &fs);
    /* Close the file no longer needed */
    fclose(fp);
    /* Build real file name */
    strcpy3(bf.file_name, disk.path[fs.disk_index], key, ".gz");
    /* Get file status */
    if (stat(bf.file_name, &buf) != 0)
    return_non_err(key);
    if (strcmp(server.site, DTMS) != 0)
    sprintf(file_size_string, "%llu bytes\n", buf.st_size);
    else strcpy(file_size_string, lines[3]+13);
    /* Change title to upper case */
    fs.title[0] -= 32;
    /* Add the colon to title */
    strcat(fs.title, ":");
    /* Pad file title with blank space */
    str_pad(fs.title, 15);
    /* Build message to send to user */
    strcpy9(rtn.msg, fs.title, fs.real_name, 
    "\nOriginal size:    ", lines[2]+13,
    "\nCompressed size:  ", lines[3]+13,
    "\nStored size:      ", file_size_string, "\n");
    /* Indicate data will be sent */
    put_short(SUCCESS);
    /* Send the return code */
    put_success_msg(key);
} /* End size_check_big_file */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
