#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "close_down.h"
#include "customer_table.h"
#include "dir.h"
#include "mm_email_msg.h"
#include "files.h"
#include "edc_defines.h"
#include "edc_prototypes.h"
#include "external_intercom_prototypes.h"
#include "ftp_info.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "trans_lib_prototypes.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_msg.h"
#include "trans_server_paths.h"
#include "reg_globals.h"
#include "registry_prototypes.h"
#include "user_params.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "ldapfsl.h"
#include "global_debug.h"

#define ATTEMPTS  10



/* Add new keyword file */
static int keyword_add(char *keyword, char *entry)
{
    char keyword_file[32];

    DBG();

    /* build name of keyword file to create */
    strcpy2(keyword_file, KEYWORD_DIR, keyword);
    return(data_log(keyword_file, entry));
}
// 

int create_secure_keyword(char *file_keyword,        /* IN */
                          char *destination_email,   /* IN */
                          char *description,         /* IN */
                          int initial_index,         /* IN */
                          char *secure_keyword)      /* OUT */
{
    extern char secure_keyword_extension[];
    char line[256];
    int  attemp = 0;
    int  status = FAILURE;
    
    DBG();

    for (attemp = 0; status != SUCCESS && attemp < ATTEMPTS; ++attemp)
    {
        DBG("calling keyword_create() attemp=%d", attemp);
        /* Build the keyword to send to the user */
        keyword_create(secure_keyword, destination_email, initial_index + attemp);
        /* Make the ninth letter upper case */
        secure_keyword[8] = secure_keyword_extension[0];
        /* Build entry to add to keyword file */
        int len = strcpy6(line, secure_keyword, " ", file_keyword, " ", destination_email, " 0 ");
        /* Put in the time */
        sprintf(line+len, "%ld", tr.upd_time);
        /* Put an entry in the keyword file for the intermediate keyword */        
        status = sql_keyword_mapping_insert(file_keyword, secure_keyword, user.email, destination_email, description, 0);         
    }
    DBG("status=%d", status);
    if (status == SUCCESS)
    {
         keyword_add(secure_keyword, line);
    }
    return status;
}

