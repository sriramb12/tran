
/****************************************************************************
 *
 * Functions:	report_list, used to generate reports
*		Currently not being used
 *
 * Description:	Various report functions
 *
 *		key1 holds list of fields to print for list requests
 *		par1 holds starting date
 *		par2 holds ending date
 *		par3 holds first keep_filter, name and value
 *		par4 holds second keep_filter, name and value
 *		par5 holds first join field
 *		par6 holds second join field
 * Author:	Renee Carter
 *
 ***************************************************************************/
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <alloca.h>
#include <unistd.h>
#include "close_down.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "intercom_server_prototypes.h"
#include "maskshop.h"
#include "report_log.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"
#include "global_debug.h"


/* General function for building report lists */
void report_list(
    char *date1, 
    char *date2, 
    char *keep_filter1, 	/* Holds keep_filter index and keep_filter value */
    char *keep_filter2,	/* Holds keep_filter index and keep_filter value */
    char *discard_filter1, 	/* Holds discard_filter index and discard_filter value */
    char *discard_filter2,	/* Holds discard_filter index and discard_filter value */
 char *scan_fields)
{
    char buffer[BUFFER_SIZE];
    char discard_filter1value[64];
    int discard_filter1index;
    int discard_filter1len;
    char discard_filter2value[64];
    int discard_filter2index;
    int discard_filter2len;
    char keep_filter1value[64];
    int keep_filter1index;
    int keep_filter1len;
    char keep_filter2value[64];
    int keep_filter2index;
    int keep_filter2len;
    int fields[REPORT_ARRAY_SIZE];
    char line[260];
     char header[512];
    char rep[REPORT_ARRAY_SIZE][256];
    char file_name[FILE_SIZE];
    char tmp_file[64];
    char *file_ptr;
    int cnt;
    int cnt1;
    char *files[1000];
    FILE *fi;
    FILE *fo;

    DBG();

    /* first build list of files to retrieve information from */
    if (names_build(date1, date2, files) <= 0)
    shut_down(bad_dates, BAD_DATES);
    /* Build first part of file name */
    file_ptr = file_name + strcpy1(file_name, "report_log/");
    /* Build name for temporary file */
     strcpy(tmp_file, "/tmp/tmp_file");
    /* Open temporary file */
    if ((fo = fopen(tmp_file, "w")) == NULL)
    close_down1m(wr_open_err, tmp_file, WR_OPEN_ERR);
    /* Print header for file */
    /* Scan out fields to print */
    report_fields_scan(scan_fields, fields);
    /* Scan out values for discard_filters */
    sscanf(discard_filter1, "%d%s", &discard_filter1index, discard_filter1value);
    sscanf(discard_filter2, "%d%s", &discard_filter2index, discard_filter2value);
    discard_filter1len = strlen(discard_filter1value);
    discard_filter2len = strlen(discard_filter2value);
    /* Scan out values for keep_filters */
    sscanf(keep_filter1, "%d%s", &keep_filter1index, keep_filter1value);
    sscanf(keep_filter2, "%d%s", &keep_filter2index, keep_filter2value);
    keep_filter1len = strlen(keep_filter1value);
    keep_filter2len = strlen(keep_filter2value);
    report_header_build(header, fields);
    fprintf(fo, "%s\n", header);
    for (cnt = 0,cnt1 = 0; files[cnt] != NULL; ++cnt)
    {
        strcpy(file_ptr, files[cnt]);
        free(files[cnt]);
        /* Open specified file */
        if ((fi = fopen(file_name, "r")) == NULL) 
        continue;
	/* Build date field for following loop */
	str_pad(file_ptr, 10);
	file_ptr[2] = file_ptr[5] = '/';
	/* copy value into fields array */
	strcpy(rep[DATE], file_ptr);
        while(line_get(fi, line) > 0)
        {
	if (  maskshop_input_scan(line, rep) != 17) continue;
	    /* See if this request meets the keep_filter criteria */
	if ((keep_filter1len > 0) &&
    (strncmp(rep[keep_filter1index], keep_filter1value, keep_filter1len)) != 0) continue;
	if ((keep_filter2len > 0) &&
    (strncmp(rep[keep_filter2index], keep_filter2value, keep_filter2len)) != 0) continue;
	    /* See if this request meets the discard_filter criteria */
	if ((discard_filter1len > 0) &&
    (strncmp(rep[discard_filter1index], discard_filter1value, discard_filter1len)) == 0) continue;
	if ((discard_filter2len > 0) &&
    (strncmp(rep[discard_filter2index], discard_filter2value, discard_filter2len)) == 0) continue;
	    /* Pad all the fields */
	    report_fields_pad(rep);
	    report_fields_print(buffer, rep, fields);
	    fprintf(fo, buffer);
	    ++cnt1;
        } /* End while */
        fclose(fi);
    } /* End for */
    /* Close the temp file */
    fclose(fo);
     if (cnt1 == 0) 
    {
        unlink(tmp_file);
        shut_down(no_weekly_data, NO_WEEKLY_DATA);
    } /* End if */
    /* Otherwise send file */
    else
    {
        /* Indicate ready to send data */
        put_short(SUCCESS);
        file_get("/tmp", tmp_file+5);
        /* Send return message */
        strcpy(rtn.msg, "Weekly report shown below:");
        return_success(rtn.msg, DBNO);
     } /* End else */
    /* Delete file no longer needed */
    unlink(tmp_file);
    exit(0);
} /* end report_list */

int maskshop_input_scan(char *line, char rep[REPORT_ARRAY_SIZE][256])
{
    DBG();
    int ret = sscanf(line, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%s",
    /* User ID platform IP node address */
    rep[KEYWORD], rep[USER_ID], rep[USER_NAME], rep[PLATFORM], rep[ADDRESS],  
    /* client version, from company, to company */
    rep[CLIENT_VERSION], rep[FROM_COMPANY], rep[TO_COMPANY],
    /* Request type, file name, original size */
     rep[REQUESTTYPE], rep[FILENAME], rep[MASKSET],
    rep[ORIGINALSIZE],
     rep[COMPRESSEDSIZE], rep[STARTTIME], rep[COMPLETETIME], rep[TRANSFERTIME], rep[ORDER_STATUS]);
    return(ret);
} /* End input_scan */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
