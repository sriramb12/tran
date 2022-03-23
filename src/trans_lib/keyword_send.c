/****************************************************************************
 *
 * Function:keyword_send
 *
 * Description:Sends secure keyword to specified users.
 *
 ***************************************************************************/

#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_defines.h"
#include "trans_server_defines.h"
#include "trans_server_macros.h"
#include "trans_server_globals.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "reg_globals.h"

#include "global_debug.h"


void keyword_send(void)
{
    char email[BUFFER_SIZE];
    char *email_addr[1000];
    FILE *fp;
    extern TRANS_FILE_STAT fs;
    char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char description[160];

    DBG();

    /* Read in information about the file */
    fp = read_big_file(gu.key1, lines, &fs);

    /* Let client know successful up to this point */
    put_short(SUCCESS);

    /* Get the description */
    description[0] = '\0';
    get_string(description);

    /* If there is no description set it back to the NULL string */
    if (strcmp(description, NONE) == 0)
        description[0] = '\0';

    get_string(email);

    /* Convert commas to spaces */
    strexc(email, ',', ' ');

    /* Convert semicolons to spaces */
    strexc(email, ';', ' ');

    /* Convert from string format to list format */
    string2list_convert(email, email_addr);
    rtn.msg[0] = '\0';

    send_secure_keyword(gu.key1, email_addr, fs.real_name, lines[2]+13, fs.file_type, fs.dlt_time, lines[8]+13, lines[9]+13, description);

    /* Tell the user the send was successful */
    /* Return message already initialized */
    /* Send the return message */
    put_success_msg(gu.key1);

    /* If description not already present in file add it */
    if ((lines[TRANS_LINE_NUM-1][0] == '\0') && (description[0] != '\0'))
    {
	    strcpy2(lines[TRANS_LINE_NUM-1], "Description: ", description);
        DBG("Before call write_big_file()");
	    write_big_file(fp, lines, &fs);
        DBG("after call write_big_file()");
    } /* End if for description not set */

    DBG("Living keyword_send()");
} /* End keyword_send */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
