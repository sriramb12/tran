/****************************************************************************
 *
 * Function:	process_request
 *
 * Description:	Reads and logs client requests
 *		from the database client.  Calls other functions
 *		to perform specific transactions.
 *
 ***************************************************************************/

#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "customer_table.h"
#include "server_lib.h"
#include "close_down.h"
#include "global_defines.h"
#include "global_reg_prototypes.h"
#include "global_report.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "lock_file.h"
#include "mm_email_msg.h"
#include "registry_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_arrays.h"
#include "trans_defines.h"
#include "trans_server_errs.h"
#include "trans_server_defines.h"
#include "trans_server_paths.h"
#include "reg_globals.h"
#include "trans_server_macros.h"
#include "trans_server_defines.h"
#include "trans_server_globals.h"
#include "user_params.h"
#include "rtn.h"
#include "rmt_server.h"
#include "strcpy.h"
#include "trans_server_prototypes.h"
#include "x500_defines.h"
#include "global_debug.h"



 int gl_socket;   // global variable to log in request_log() request.c

/* Set iCAP class for internal customer */
void internal_icap_class_set(char *icap_string)
{
    DBG();
    if (gu.icap_class[0] == '\0') strcpy(icap_string, "NXP Confidential Proprietary");
    else if (strcasecmp(gu.icap_class, "fcp") == 0) strcpy(icap_string, "NXP Confidential Proprietary");
    else if (strcasecmp(gu.icap_class, "pubi") == 0) strcpy(icap_string, "Public Information");
    else if (strcasecmp(gu.icap_class, "fiuo") == 0) strcpy(icap_string, "NXP Internal Use Only");
    else if (strcasecmp(gu.icap_class, "gbi") == 0) strcpy(icap_string, "General Business Information");
    else strcpy(icap_string, "NXP confidential Proprietary");
} /* end internal-icap_class_set */

/* Make the class the abbreviation again */
void external_icap_class_set(char *icap_string)
{
    DBG();
    if (gu.icap_class[0] == '\0') strcpy(icap_string, "Confidential Proprietary");
    else if (strcasecmp(gu.icap_class, "cp") == 0) strcpy(icap_string, "Confidential Proprietary");
    else if (strcasecmp(gu.icap_class, "pubi") == 0) strcpy(icap_string, "Public Information");
    else if (strcasecmp(gu.icap_class, "gbi") == 0) strcpy(icap_string, "General Business Information");
    else if (strcasecmp(gu.icap_class, "iuo") == 0) strcpy(icap_string, "Internal Use Only");
    else strcpy(icap_string, "confidential Proprietary");
} /* end external_icap_class_set */

/* Set the iCAP classification */
void icap_class_set(char *icap_string)
{
    extern int customer_type;

    DBG();

    if (customer_type == INTERNAL_CUSTOMER)
    internal_icap_class_set(icap_string);
    else external_icap_class_set(icap_string);
} /* end icap_class_set */

/* Make the class the abbreviation again */
void icap_class_rtv(char *icap_class, char *icap_string, char *from_company)
{
    DBG();

    if (icap_string[0] == '\0')
    strcpy(icap_class, "fcp");
    else if (strcasecmp(icap_string, "NXP Confidential Proprietary") == 0)
    strcpy(icap_class, "fcp");
    else if (strcasecmp(icap_string, "Confidential Proprietary") == 0) 
    strcpy(icap_class, "cp");
    else if (strcasecmp(icap_string, "Public Information") == 0) 
    strcpy(icap_class, "pubi");
    else if (strcasecmp(icap_string, "General Business Information") == 0) 
    strcpy(icap_class, "gbi");
    else if (strcasecmp(icap_string, "NXP Internal Use Only") == 0) 
    strcpy(icap_class, "fiuo");
    else if (strcasecmp(icap_string, "Internal Use Only") == 0) 
    strcpy(icap_class, "iuo");
    else strcpy(icap_class, "fcp");
} /* end icap_class_rtv */

void request_read(void)
{
    int fields;
    int cnt;  

    DBG("BEFORE get_string() bf.buffer='%s', gu.key1='%s'", bf.buffer, gu.key1);

    get_string(bf.buffer);

    /* Set data classification to a default value */
    strcpy(gu.icap_class, "FCP");   

    fields = sscanf(bf.buffer+13, "%d %[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%ld %lld %lld  %ld %c %[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]",
        &gu.req_type, gu.key1, user.login, user.mach, user.addr, user.cwd, user.client_version,
        gu.machine_type, &gu.client_time,  &gu.original_size, &gu.compressed_size, &gu.file_mode,
        &gu.file_type, gu.email1, gu.email2, gu.from_company, gu.to_company, gu.par1, gu.par2,
        gu.par3, gu.par4, gu.par5, gu.par6, user.name, gu.icap_class
    );  

    if (fields == 1 && gu.key1[0] == 0)
    {
        fields = sscanf(bf.buffer+13, "%d%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%ld %lld %lld  %ld %c %[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]",
            &gu.req_type, user.login, user.mach, user.addr, user.cwd, user.client_version,
            gu.machine_type, &gu.client_time,  &gu.original_size, &gu.compressed_size, &gu.file_mode,
            &gu.file_type, gu.email1, gu.email2, gu.from_company, gu.to_company, gu.par1, gu.par2,
            gu.par3, gu.par4, gu.par5, gu.par6, user.name, gu.icap_class
        );
    }

    DBG("AFTER sscanf() bf.buffer='%s', gu.key1='%s'", bf.buffer, gu.key1);
    str_lower(user.login);

    /* Pull off the password  */    
    /* Must get it by hand in order to copy null characters */
    for (cnt = 0; cnt < 12; ++cnt)
    {
        tr.crypt_pwd[cnt] = bf.buffer[cnt];   
    }

    tr.crypt_pwd[12] = '\0';   

    DBG("fields=%d, gu.original_size='%d'", fields, gu.original_size);
    DBG("gu.original_size='%d' user.login='%s'  gu.email1='%s'  gu.email2='%s'", gu.original_size, user.login, gu.email1, gu.email2);
    DBG("gu.from_company=%s gu.to_company=%s", gu.from_company, gu.to_company);
    DBG("gu.par1='%s' gu.par2='%s' gu.par3='%s' gu.par4='%s' gu.par5='%s' gu.par6='%s'", gu.par1, gu.par2, gu.par3, gu.par4, gu.par5, gu.par6);
    DBG("gu.req_type=%d gu.original_size=%llu gu.compressed_size=%llu", gu.req_type, gu.original_size, gu.compressed_size);

} /* end request_read */

void request_log(void)
{
    char msg[BUFFER_SIZE];
    extern int request_logged;  

    DBG();

    request_logged = DBYES;
    /* first make sure this request should be logged */
    if (log_request(LOG_REQUEST_FILE) != SUCCESS) return;

    sprintf(msg, "[%s on port %d] processing request %s from %s for %s\n\
Site %s\t%s Machine: %s \n\
Directory %s\n\
Client version %s Machine type %s Client time %s\
PID %d From company %s to company %s Server time %s\
File size %llu file mode %u file type %s  email1 %s Email2 %s\n\
Par1-6 %s %s %s %s %s %s data class %s\n",
            so.program, gl_socket,
	req_array[gu.req_type],  user.name, gu.key1, 
	user.login, user.addr, user.mach,  user.cwd, 
	user.client_version, gu.machine_type, ctime(&gu.client_time),
	tr.pid, gu.from_company, gu.to_company, ctime(&tr.upd_time),
	gu.original_size, gu.file_mode, trans_filetype_array[gu.file_type-'a'], gu.email1, gu.email2,
    gu.par1, gu.par2, gu.par3, gu.par4, gu.par5, gu.par6, gu.icap_class);
    
    data_log(tr.log_file, msg);
    DBG("\n%s", msg);

} /* end request_log */


#define REQUESTS_LOG "db_admin/request_information.txt"

/* Carlos Mazieri */

/*! save_last_request()
 *
 *  Save a request, it can be used later to reproduce failures in development mode
 *
 *  Saves information from TRANS_PARAMS and USER_PARAMS data structures
 */
void save_last_request()
{
    DBG();
    char msg[BUFFER_SIZE];
    char current_time[128];
    time_convert(current_time);

    FILE *log = fopen(REQUESTS_LOG, "w");
    if (log != NULL)
    {
        const char *format = "\n# %s\n%s";
        sprintf(msg, format, "current time", current_time);
        fputs(msg, log);

        sprintf(msg, format, "USER_PARAMS.server_num = %d", user.server_num);
        fputs(msg, log);

 /*
        sprintf(msg, format, "USER_PARAMS.client_version = %s", client_version);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, "USER_PARAMS.client_version = %s",);
        fputs(msg, log);


        sprintf(msg, format, );
        fputs(msg, log);
        */
        fclose(log);
    }
}
