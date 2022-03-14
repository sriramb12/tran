
/****************************************************************************
 *
 * Name:	ssl_client_globals
 *
 * Description:	Global variables used by the SSL TransAPI client.
 *		        Must also include variables from trans_client_globals.c
 *
 ***************************************************************************/

#include <openssl/ssl.h>
#include "company.h"
#include "server_params.h"
#include "trans_defines.h"
#include "transapi_defines.h"
#include "transrc.h"
#include "repositories_defines.h"


char tool_name[7][10] =
{
    "TransWeb",
    "InterCom",
    "TransWeb",
    "Transcend",
    "InterCom",
    "Transcend",
    "Transcend",
};


/* Tool structure */
TRANS_TOOL tool;

/* Indexes for tools into the repository array */
int repository_index[7][2] =
{
    { 0, 0, },   /* External TransWeb */
    { 1, 1, },   /* External InterCom */
    { 2, 6, },   /* Internal TransWeb */
    { 2, 6, },   /* Transcend */
    { 7, 8,},   /* InterCom */
    { 2, 8, },   /* Transcend and InterCom */
    { 0, 8  },    /* ALL */
};

TRANS_REPOSITORY repository[14] =
{
/* External TransWeb */
/*0*/    { COMPANY_NAME, 'e', 'E', INTERCOM_ADDR, INTERCOM_IP, EXTERNAL_INTERCOM_PORT, NULL},    // abbreviation NULL means to not be printed

/* External InterCom */
/*1*/    { COMPANY_NAME, 'e', 'E', INTERCOM_ADDR, INTERCOM_IP, EXTERNAL_INTERCOM_PORT, NULL},    // abbreviation NULL means to not be printed


/* Transcend servers */
/*2*/   { MASTER_REP, 'p', 'P',            MASTER_REP_ADDR,           MASTER_REP_IP,     TRANSCEND_PORT,    "phx"},
/*3*/   { "Amsterdam", 'i', 'I',          "adam-trans.nxdi.nxp.com",  "92.120.72.14",    TRANSCEND_PORT,    "adam"},
/*4*/   { "Noida", 'n', 'N',              "idc-trans.nxdi.nxp.com",   "10.224.148.151",  TRANSCEND_PORT,    "idc"},
/*5*/   { "DTMS", 'd', 'D',               "phx-dtms.nxdi.nxp.com",    "92.121.240.169",  TRANSCEND_PORT,    "dtms"},
/*6*/   { "Bangalore", 'b', 'B',          "blr-trans.nxdi.nxp.com",   "165.114.116.154", TRANSCEND_PORT,    "blr"},

/*  Intercom server and Intercom alternatives in case   INTERCOM_ADDR changes */
/*7*/   { "External Transweb", 'e', 'E',  INTERCOM_ADDR,   INTERCOM_IP,    INTERNAL_INTERCOM_PORT, "etc"},
/*8*/   { "Send2Foundry", 's', 'S',       MASTER_REP_ADDR, MASTER_REP_IP,  SEND2FOUNDY_PORT,   NULL},    // abbreviation NULL means to not be printed

};

int site_index[7][2] = // matches TOOL_TYPE
{
    { 0, 0,  },  /* External TransWeb   EX_TRANSWEB */
    { 1, 1,  },  /* External InterCom   EXTERNAL_INTERCOM */
    { 2, 7,  },  /* Internal TransWeb   INTERNAL_TRANSWEB */
    { 2, 7,  },  /* Transcend           INTERNAL_TRANSCEND */
    { 8, LAST_SITE_DESTINATION_INDEX, },  /* InterCom            INTERNAL_INTERCOM  */
    { 2, LAST_SITE_DESTINATION_INDEX, },  /* Transcend_InterCom  TRANSCEND_INTERCOM */
    { 0, LAST_SITE_DESTINATION_INDEX, },  /* All                 ALL_TOOLS */
};

/* Extra records are present so additional companies can be added */
TRANS_COMPANY site[MAX_SITE_DESTINATIONS] =
{
/* External TransWeb */
/*00*/   { "InterCom", "etc", INTERCOM_ADDR, INTERCOM_IP, STR_P(EXTERNAL_INTERCOM_PORT), "Tramsweb",  "0", "no", "no",  "yes", },

/* External InterCom */
/*01*/   { "InterCom", "etc", INTERCOM_ADDR, INTERCOM_IP, STR_P(EXTERNAL_INTERCOM_PORT), "intercom",  "0", "no", "no",  "yes", },

/* Transcend */
/*02*/   { "Phoenix",   "phx",  MASTER_REP_ADDR,            MASTER_REP_IP,     STR_P(TRANSCEND_PORT), "Transcend", NONE, "no", "no", "yes", },
/*03*/   { "Amsterdam", "adam", "adam-trans.nxdi.nxp.com",  "92.120.72.14",    STR_P(TRANSCEND_PORT), "Transcend", NONE, "no", "no", "yes", },
/*04*/   { "Bangalore", "blr",  "blr-trans.nxdi.nxp.com",   "165.114.116.154", STR_P(TRANSCEND_PORT), "TransWeb",  NONE, "no", "no", "yes", },
/*05*/   { "Noida",     "idc",  "idc-trans.nxdi.nxp.com",   "10.224.148.151",  STR_P(TRANSCEND_PORT), "Transcend", NONE, "no", "no", "yes", },
/*06*/   { "DTMS",      "dtms", "phx-dtms.nxdi.nxp.com",    "92.121.240.169",  STR_P(TRANSCEND_PORT), "Transcend", NONE, "no", "no", "yes", },
/*07*/   { "DTMS",      "dtm",  "phx-dtms.nxdi.nxp.com",    "92.121.240.169",  STR_P(TRANSCEND_PORT), "Transcend", NONE, "no", "no", "yes", },

/* Internal InterCom */

    // Nov 29 Carlos Mazieri, removed more than 30 items, everything comes from the server

};

SSL *tmp_ssl;
SSL *ssl;
char certificate_dir[PATH_SIZE];
RTN rtn;
struct transrc_t transrc;
char tmp_transdirlist[64];
char certificate_file[FILE_SIZE];
char icap_class[8];
int shm_id;
int perform_background_transfer;
char global_par6[8];
int global_file_cnt;
int global_dir_cnt;
char global_project[64];
long long global_final_size;
int global_counter;
int global_dir_offset; /* Offset for directory if slashes are in original file name */
unsigned long long global_compressed_size;
char global_keyword[256];
char api_keyword[10];
TRANS_FILE_TYPE global_file_type;
TRANS_FILE_TYPE global_file_type;
int compression;
float compression_ratio;
char *extension_list[500];
char *ratio_list[500];
char reg_email1[256];
char reg_email2[256];
char remote_company_email[1024];
char tar_option;
char icomrc_path[PATH_SIZE];
int log_files;
int size_printed;
SERVER_PARAMS server;
USER_PARAMS user;
DB_SOFTWARE so;

/* List of pointers for TRANS structures */
TRANS_PARAMS *trans[100];

