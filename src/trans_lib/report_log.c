/****************************************************************************
 *
 * Function:	report_lob
 *
 * Description:	Logs statistics from transaction in report log
 *		from the database client.  Calls other functions
 *		to perform specific transactions.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "maskshop.h"
#include "report_log.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_lib_prototypes.h"
#include "trans_server_arrays.h"
#include "trans_defines.h"
#include "trans_server_defines.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "global_debug.h"

void report_field_convert(char *field, int cnt)
{
    DBG();

    char type[REPORT_ARRAY_SIZE] =
{
  's', 's', 's', 's', 's', 's', 's', 's', 's', 's', 's',  'i', 'i', 't', 't', 'e', 's', 's'
};

    switch (type[cnt])
    {
	case 's': return;
	case 'i': size_convert(field); break;
	case 'e': elapsed_time_convert(field); break;
	case 't':  time_convert(field); break;
    } /* End switch */
} /* End report_field_convert */

int trim_size[REPORT_ARRAY_SIZE] = 
{ 12, 10, 25, 12, 20, 10,  20, 20, 10, 32, 8, 20, 20, 20, 20, 20, 6, 10 };

    char report_header[REPORT_ARRAY_SIZE][32] = 
{
    "Keyword     ",
    "User ID   ",
    "User name           ",
    "Platform  ",
    "IP node address     ",
    "Version",
    "From Company          ",
    "To Company          ",
    "Request   ",
    "File/directory      ",
    "Mask Set",
    "Size",
    "Compressed",
    "Start time",
    "Complete Time",
    "Transfer time",
    "Status",
    "Date    ",
};


#ifdef TRANSWEB
void report_header_build(char *header, int fields[])
{
    int cnt;
    int len = 0;

    DBG();

    for (cnt = 0; fields[cnt] != -1; ++cnt)
    {
	len += strcpy2(header+len, report_header[fields[cnt]], ";");
     } /* End for */
    /* Chop off trailing semicolon */
    header[len-1] = '\0';
} /* report_header_build */
#else /* For TRANSCEND and INTERCOM */
void report_header_build(char *header, int fields[])
{
    int cnt;
    char tmp_field[32];
    int len = 0;

    DBG();

    for (cnt = 0; fields[cnt] != -1; ++cnt)
    {
	strcpy(tmp_field, report_header[fields[cnt]]);
	str_pad(tmp_field, trim_size[fields[cnt]]);
    len += strcpy1(header+len, tmp_field);
    /* Chop off trailing semicolon */
     } /* End for */
} /* report_header_build */
#endif /* for TRANSCEND and INTERCOM */
void transaction_log(char *keyword, char *file_name, char *mask,
    char *status, char *msg,  char *log_prefix)
{   
    extern TRANS_FILE_STAT fs;
    long current_time = time(0);
    FILE *fp;
    char report_file[32];
    char local_msg[BUFFER_SIZE] = {0};
    char *pt = local_msg;

    /* convert line feed to * for log */
    snprintf(local_msg, sizeof(local_msg) -1, "%s", msg);
    for (; *pt != '\0'; ++pt)
    {
        if (*pt == '\n') *pt = '*';
    }
    /* Build log file name */
    strcpy2(report_file, log_prefix, tr.log_file);
    DBG("local_msg=%s report_file=%s", local_msg, report_file);
    if ((fp = fopen(report_file, "a")) == NULL) return;
    int bytes  = fprintf(fp, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%lld;%lld;%ld;%ld;%ld;%s;%s;%s\n",
     /* User ID platform, IP node address */
    keyword, user.email, user.name, gu.machine_type, user.addr,
     user.client_version, gu.from_company, 
	/* External company specified only for Intercom transactions */
#ifndef TRANSCEND
	gu.to_company,
#else
	gu.from_company,
#endif
    /* Request type, file name, mask, original size */
    req_array[gu.req_type],  file_name, mask, fs.original_size,
     fs.compressed_size, tr.upd_time, current_time, current_time-tr.upd_time, status, gu.icap_class, local_msg);
    fclose(fp);
} /* End transaction log */

void report_log(char *keyword, char *file_name, char *mask, char *msg)
{
    char status[4];

    DBG();

    /* See if this is the first time mask set sent */
    mask_status_find(mask, status);
    strcpy(status, "NA");
    /* First log in report log where all transactions logged */
    transaction_log(keyword, file_name, mask, status, msg, "report_");
    /* If this is an FSS transaction, log to FSS log */
#ifndef TRANSCEND
    if ((strncmp(gu.from_company, "Foundry", 7) == 0) 
    || (strncmp(gu.to_company, "Foundry", 7) == 0))
    fss_report_log(NONE, file_name, mask);
    else
#endif
    /* If this is a put or get request, also log in transfer log */
    /* Do not log any of the FSS transactions in this log */
    if (((gu.req_type == DBGET_DATA)
    || (gu.req_type == DBGETDLT_DATA)
    || (gu.req_type == DBPUTDLT_DATA)
    || (gu.req_type == DBPUT_DATA)
    || (gu.req_type == DBPUTDLT_TAR_FILE)
    || (gu.req_type == DBPUT_TAR_FILE)
    || (gu.req_type == DBPUTDLT_FILES)
    || (gu.req_type == DBPUT_FILES))
    && (strncmp(user.email, "b36283", 6) != 0))
    transaction_log(NONE, file_name, mask, status, msg, "transfer_");
} /* End report_log */

int report_input_scan(char *line, char rep[REPORT_ARRAY_SIZE][256])
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
    if (strcmp(rep[TO_COMPANY], "Photronics") == 0)
    strcat(rep[TO_COMPANY], " (Mesa AZ)");
    return(ret);
} /* End input_scan */

void report_fields_pad(char rep[REPORT_ARRAY_SIZE][256])
{
    char tmp_company[256];
    int cnt;

    DBG();

    /* Do some special conversions for company names */
    if (strncmp(rep[TO_COMPANY], "Photronics", 10) == 0)
    {
	strcpy2(tmp_company, "Phot", rep[TO_COMPANY]+10);
	strcpy(rep[TO_COMPANY], tmp_company);
    }
    if (strncmp(rep[TO_COMPANY], "DuPont", 6) == 0)
    {
	strcpy2(tmp_company, "Dpt", rep[TO_COMPANY]+6);
	strcpy(rep[TO_COMPANY], tmp_company);
    }
    for (cnt = 0; cnt <REPORT_ARRAY_SIZE; ++cnt)
    {
	report_field_convert(rep[cnt], cnt);
	str_pad(rep[cnt], trim_size[cnt]);
	/* Make sure at least one blank space at end of name */
	rep[cnt][trim_size[cnt]-1] = ' ';
    } /* End for */
} /* End report_fields_pad */



void num_gmb_calc(char *size_string, unsigned long long size)
{
    DBG();
  if (size < 1024 * 1024)
	sprintf (size_string, "%llu bytes", size);
    else if (size < 1024*1024 * 1024)
	sprintf (size_string, "%0.2f MB", 1.0 * size / 1024 / 1024);
	else sprintf (size_string, "%0.4f GB", 1.0 * size / 1024/ 1024/1024);
} /* End num_gmb_calc */

void num_bytes_calc(char *byte_string, long long bytes)
{
    DBG();
	sprintf (byte_string, "%lld bytes", bytes);
} /* End num_bytes_calc */

void time_convert(char *time_string)
{
    long time = atol(time_string);
    char *time_ptr = ctime(&time);
    DBG();
    strncpy(time_string, time_ptr+4, 15);
}

/* Determine if first time mask set sent to the mask shop */
void mask_status_find(char *mask, char *status)
{
    FILE *po;
    char command[32];
    int num;

    DBG();

    if (((gu.req_type != DBPUT_DATA) || ( gu.req_type != DBPUT_TAR_FILE)
    || (gu.req_type != DBPUTDLT_DATA) && ( gu.req_type != DBPUTDLT_TAR_FILE))
    || (strcmp(mask, "none") == 0))
    {
	strcpy(status, "na");
	return;
    }
    /* Look for mask set in mask set list */
    strcpy3(command, "grep ", mask, " report_log/masks | wc -l");
    po = popen(command, "r");
    fscanf(po, "%d", &num);
    pclose(po);
    if (num == 0) 
    {
	strcpy(status, "New");
	data_log("report_log/masks", mask);
    } /* End if */
    else strcpy(status, "Old");
} /* End mask_status_find */
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
