#ifndef transapi_defines_h
#define transapi_defines_h

#include <stdio.h>
#include <time.h>
#include "global_defines.h"
#include "rtn.h"
#include "user_params.h"
#include "trans_params_struct.h"


#ifdef INTERCOM
#define TRANSAPI_VERSION CURRENT_ICOM_VERSION
#endif
#ifdef TRANSCEND
#if !defined(TRANSAPI_VERSION)
#define TRANSAPI_VERSION CURRENT_TRANS_VERSION
#endif
#endif
#define NXP_COMPANY "NXP Semiconductor"
#define TRANSAPI_PUT_OPTIONS "fFkKnNOpPtTzZ"

/* Enumerations for supported servers */
/* One of these values can be passed to function customer_set */
typedef enum
{
    AUSTIN_SERVER,
    MUNICH_SERVER,
    BRAZIL_SERVER,
    INDIA_SERVER,
    ISRAEL_SERVER,
    DTMS_SERVER,
    EXTERNAL_SERVER,
    SEND2FOUNDRY_SERVER,
    FTPGATE_SERVER,
} TRANS_SERVERS;

/* Tool types */
/* One of these values is passed to trans_tool_set  */
/* This specifies the tool being used */
typedef enum
{
    EX_TRANSWEB,
    EXTERNAL_INTERCOM,
    INTERNAL_TRANSWEB,
    INTERNAL_TRANSCEND,
    INTERNAL_INTERCOM,
    TRANSCEND_INTERCOM,
    ALL_TOOLS,
} TOOL_TYPE;

/* Any enumerated value listed below can be passed to trans_option_set */
typedef enum
{
    KEYWORD,
    /* Specifies if links followed when uploading a file */
    /* DBYES used to follow links, DBNO used to not follow links */
    /* The default is to not follow links */
    FOLLOW_LINKS,
    SEND_RTV_EMAIL,
    /* Zip the data during transfer */
    ZIP_DATA,
    /* Uncompress during transfer */
    UNCOMPRESS_DURING_TRANSFER,
    /* List of e-mail addresses to send secure keywords to */
    EMAIL_LIST,
    EMAIL_STRING,
    EMAIL,
    /* Short description supplied when sending secure keywords */
    DESCRIPTION,
    FILE_NAME,
    /* Name of file being uploaded */
    /* List of files being uploaded */
    FILE_NAMES,
    /* ICAP classification */
    ICAP_CLASS,
    /* Site ie aus, brzl, mun, idc, isrl, dtms, etc, pat, tsm */
    SITE,
    /* Number of days to extend file expiration */
    /* This can be set to 1, 2, or 3 */
    NUM_DAYS,
    /* Extra parameters passed in the Transcend header */
    /* These have different meanings in different transactions */
    PAR1,
    PAR2,
    PAR3,
    PAR4,
    PAR5,
    PAR6,
} OPTION_TYPE;

/* Types of supported requests */
typedef enum
{
    /* Upload a file, directory, or group of files */
    /* A file descriptor or file stream can be used in place of the file name */
    PUT_REQUEST,
    /* Download data from Transcend/Intercom repository */
    GET_REQUEST,
    /* Simple request requiring a valid file or secure keyword */
    KEYWORD_REQUEST,
    /* Simple request which may or may ot have an additional parameter */
    SIMPLE_REQUEST,
    /* custom request requiring custom code to perform the request */
    CUSTOM_REQUEST
} TRANSACTION_TYPE;

/* Types of transfer */
/* These are set in trans_put_request_set or trans_put_transfer_type_set */
#define UNSPECIFIED_TRANSFER 'U'
#define SERIAL_TRANSFER 's'
#define PARALLEL_TRANSFER 'l'
#define SEPARATE_BLOCK_TRANSFER 'R'
#define ZIP_DURING_TRANSFER 'z'
#define BUNDLE_WITH_TAR 't'

/* Defined ICAP classifications */
/* The iCAP class is set with the function icap_class_set */
typedef enum
{
    FCP,
    /* NXP confidential Proprietary */
    /* NXP Internal Use Only */
    FIUO,
    /* Public information */
    PUBI,
    /* General business information */
    GBI,
} ICAP_TYPE;

/* Feb 2018 Carlos Mazieri
   struct TRANS_USER_PARAMS removed
   using  struct USER_PARAMS from  global_include/user_params.h
   //extern TRANS_USER_PARAMS user;
 */
extern USER_PARAMS  user;

/* Tool structure */
typedef struct
{
    char name[16];
    int type;
    int index;
    int repository_index[2];
    int site_index[2];
} TRANS_TOOL;

/* List of pointers for TRANS structures */
extern TRANS_PARAMS *trans[100];

/* Holds defined transfer types */
typedef enum
{
    M2INTERCOM,
    M2FTP,
    M2ESE,
}  TRANSFER_TYPE;

/* Holds names, adresses, accounts for remote mask shop */
typedef struct
{
    char tla[8];
    TRANSFER_TYPE transfer_type;
     char company[32];
    char machine[64];
    char address[64];
    char remote_email[64];
    char account[64];
    int socket;
} MASKSHOP;

typedef struct
{
    char mask[8];
    char tickler_file[32];
    char script_file[32];
    char stamp[16];
     FILE *tickler_fp;
    FILE *script_fp;
    char user_email[32];
    char user_name[24];
    char user_phone[20];
} TICKLER;

#endif
