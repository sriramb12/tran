#ifndef trans_server_defines_h
#define trans_server_defines_h

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "global_defines.h"
#include "trans_defines.h"
#include "trans_params_struct.h"

#define APPROVAL_REQUEST_MSG \
"Your request to approve '%s' has been\n\
submitted, and e-mail was sent to your manager for the approval.\n\
Your manager can perform the approval on the EDC web site: \n\
    - http://nww.edc.nxp.com"

#define STATUS_REQUEST_MSG \
"Status of request for '%s' : '%s'"

/* Used in array when deleting history and dirlist files */
#define HISTORY 0
#define DIRLIST 1
#define TWO_WEEKS 1209600
#define NXP_SEMI "NXP Semiconductor N.V."
#define WEB_BROWSER "Web browser"
#define EXTERNAL_EMAIL "transbug@nxp.com"

#ifdef INTERCOM
#define EXTERNAL_REPOSITORY "InterCom"
#define EXTERNAL_SOFTWARE "intercom_exec"
#endif /* For InterCom */


/* Defines for keyword extensions for various servers */

typedef struct
{
    char cleanup_flag;
    char source_flag;
} TRANS_FLAGS;


/* Disk information initialized when the server is initialized */
typedef struct
{
    char path[10][16];
         long lower_size[10];
        long upper_size[10];
} TRANS_DISK;


/* Holds status information for Transcend files */
typedef struct
{
    char file_type;	/* directory, binary, text, or unknown */
    int disk_index;	/* Index of disk where file stored */
    time_t dlt_time;	/* Time when file can be deleted in sec. since 1970 */
    char own_id[EMAIL_LEN];
    int email_flag;	/* Indicates where owner will receive mail when file retrieved */
    int extension;	/* Number of working days file extended */
    int dlt_flag;	/* Indicates if file will be deleted after first retrieval */
    int compression;	/* Indicates if file is compressed */
    char lock_flag;	/* Indicates if file is still being transcended */
    long mode;	/* File permission mode */
    char real_name[FILE_SIZE];	/* Name of actual file from user */
    char title[16];
    char tar_type[8];
    char icap_class[80];
    unsigned long long original_size;	/* Original size in bytes */
    unsigned long long compressed_size;
#ifdef INTERCOM
    char owner_site[SITE_LEN];
#endif
    char key[64];
} TRANS_FILE_STAT;

/* Holds fields for the meta data present in the files in global_data */
typedef struct
{
    char current_time_string [32];
    char current_gmt_time_string [32];
    char dlt_time_string[32];
    char gmt_time_string[32];
    char expiration_time_string [60];
    char icap_class [40];
    char description[260];
    char send_keyword ;
} METADATA;
typedef struct
{
    char file_type[32];	/* File type, big_files, med_files, etc. */
    char path[64];
    long avail;	/* Number of bytes available */
    long increment;	/* Number of bytes that must not be used */
    int percent;
} TRANS_USAGE_DISK;

#endif /* Define for trans_server_defines_h */
