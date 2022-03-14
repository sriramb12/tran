#ifndef global_req_types_h
#define global_req_types_h
/* Data types understood by the DMI database */
typedef enum
{
/* Data types */
    MASK = 'a',
    DSGN = 'b',
    TMPL = 'c',
    PC_LIB = 'd',
    TMPL_DSGN = 'e',
    DSGN_TMPL = 'f',
    DSGN_GROUP = 'g',
    WAFER_LINE = 'h',
    GLBL = 'i',
    VERSION = 'j',
    GLOBAL_TABLES = 'k',
    WL_PC = 'l',
    OFF_TMPL = 'm',
    OFF_DSGN = 'n',
    OFF_TMPL_OFF_DSGN = 'o',
    OFF_TMPL_DSGN = 'p',
    TMPL_OFF_DSGN = 'q',
    OFF_TMPL_AUDIT = 'r',
    OFF_DSGN_AUDIT = 's',
    APP_TMPL_DSGN = 't'
} OLD_DATA_TYPE;

/* Transaction types */
typedef enum
{
    UPD_REQ = 'd'  ,
    DLT_REQ = 'i',
    READ_REQ = 'b',
    MOD_REQ = 'c',
    RTV_DATA_REQ= 'q',
    RTV_FILES_REQ = 'r',
    CRT_REQ = 'a',
    REMOTE_REQ = 'e',
    GLOBAL_REQ = 'f',
    VERSION_REQ = 'g',
    APPROVE_REQ = 'h',
    ALIVE_REQ = 'j',
    RELEASE_REQ = 'k',
    NETWORK_REQ = 'l',
    PORT_REQ= 'p',
    REGISTER_REQ = 'o',
    LIST_REQ = 's',
    DB_ADMIN_REQ = 'n',
    RVUPORT_REQ = 'm',
    CRON_REQ = 't',
    OPC_REQ = 'u'
} TRANSACTION;

/* Request types */
typedef enum
{
    /* Update requests */
    DBM2READ = 61,
    DBM2CHECKOUT = 62,
    DBM2CONVERT = 63,
    DBM2CANCHECKOUT = 64,
    DBOVRWR_DATA = 'A'  ,
    DBCHKIN_DATA = 'B' ,
    DBUPD_DATA = 'C' ,
    DBCHKOFF_DATA = 'D' ,
    DBDLT_DATA = 'E' ,

/* Check out and read transactions */
    DBCHKOUT_DATA = 'F' ,
    DBAPP_DATA = 'G' ,

/* Modify transactions */
    DBCANCHKOUT_DATA = 'H' ,
    DBCANAPP_DATA = 'I' ,
    DBTRANSFER_DATA = 'J' ,

/* Read data, retrieve versions */
    DBREAD_DATA = 'K' ,
    DBREAD_FILE = '^' ,	/* 94 */
    DBRTV_VERSIONS = 'L' ,
    DBUNAPP_DATA = 'M' ,
    DBBACKOUT_DATA = 'N' ,
    DBDLT_VERSIONS = 'O' ,
    DBPORT_RVU_DATA = 'P' ,
    DBRESTORE1MASK = 22,
DBLOCK4ARCHIVE = 'Q',
DBCOMPLETE_ARCHIVE = 'R',
DBM2DISABLE = 'S',
DBM2ENABLE = 'T',
DBSEND_DAILY = 19,
    DBSEND_MONTHLY = 20,

/* Create, copy, and port data */
    /* Action class is create */
    DBCRT_DATA = 'U' ,
    DBCOPY_DATA = 'V' ,
DBCOPY_MASK2TMPL = 'R',
/* Port mask to template, done in Hercules */
    DBPORT_MASK2TMPL = 50,
    /* Port specified data to the database */
    DBPORT_DATA = 'W' ,

    /* Modify transactions */
    DBMOD_PIN = 'X' ,
    DBMOD_OWNER = 'Y' ,
    /* Grant overwrite for one transaction */
    DBGRANT_OVRWR = 'Z' ,
/* Set C4 mask */
    DBSET_C4MASK = '_' ,	/* 95 */
    DBRTV_MASK_FILES = 'a',
    DBRTV_MASKSET_NAME = 'b',
    DBRTV_PC_NAME = 'c',
    DBRTV_H_NAME = 'd',
    /* Empty requests for future use */

    /* Retrieve small pieces of data */
    /* Uses transaction type DBRTV_DATA */
/* Retrieve the mask set PIN */
    DBRTV_PIN = 55,
    DBRTV_CWD = 56,
    /* Retrieve serial number for Mesa Mask Shop */
    DBRTV_MOS17SERIAL = 33 ,
    DBRTV_MOS21SERIAL = 'm' ,
    DBRTV_MOS6SERIAL = 'n' ,
    DBRTV_MOS16SERIAL = 42 ,
    DBRTV_COM1SERIAL = 59,
/* Retrieve annual contract number */
    DBRTV_CNB = 57,
    DBRTV_CONTRACT_NUM = 'l' ,
/* Retrieve value of C4 mask */
    DBRTV_C4MASK = 58,

    /* Registration requests */
    /* Transaction type of REGISTER_REQ  */
    DBCHK_REG = 60,
    DBAUTO_REG = 61,
    DBMAN_REG = 62,
    DBFORCE_REG = 63,
    DBRMT_MAN_REG = 64,
    DBRMT_FORCE_REG = 24,
    DBMOVE2NEW = 23,
/* Resend registration entries */
    DBRESEND_REG = 54,

    /* List requests */
    /* Have transaction type LIST_REQ */
    /* Pull out file with a list of items */
    /* List PC links */
    DBLIST_LINKS = '[' ,
    /* Pull down list of PC libraries */
    DBLIST_PCS = '\\' ,
    /* List masks with specified search criteria */
    DBLIST_MASKS = ']' ,
    /* Send list of all masks */
    DBLIST_ALL_MASKS ,
    /* Send list of templates with specified criteria */
    /* Currently being used by Hercules */
    DBLIST_TMPLS = 127,
   /* Currently not being used */
    DBLIST_SERVERS = 'o' ,
    /* Unused list request */
    DBLIST_EMPTY1,
    DBLIST_EMPTY2,

    /* Retrieve one or more files */
    /* Have transaction type RTV_FILES */
    DBRTV_FILES ,
    DBRTV_FILE,
    EMPTY_REQ = '_' ,

/* Remote request types */
    DBRMT_COMPLETE_ARCHIVE = 25,
    DBRMT_AUDIT_DATA =  26,
    DBRMTCRT_DATA = 27 ,
    DBRMTDLT_DATA = 28 ,
    DBMARK_DATA = 29 ,
    DBREFRESH_DATA = 30 ,
    DBRMTPORT_RVU = 31 ,

/* PC functions */
    DBADD_LINK = 'f' ,
    DBMOD_LINK = 'g' ,
    DBDLT_LINK = 'h' ,
    DBREPLACE_DATA = 'i' ,
    DBRENAME_DATA = 'j' ,

    DBREAD_CONTROL_GROUP = 'k' ,

    /* Network requests */

    DBRESEND_DATA = 'p' ,
    DBUPD_SERVER = 'q' ,
    DBPING_REQ = 'r' ,
    DBREAD_DIR = 's' ,

    /* Audit actions */
    DBRMT_AUDIT_GLOBAL = 't' ,
    DBAUDIT_DATA = 'u' ,
    DBAUDIT_GLOBAL = 'v' ,
    DBAUDIT_FAB = 'w' ,
    DBAUDIT_DESIGN = 'x' ,
    DBAUDIT_MASK = 'y' ,
    DBAUDIT_PC = 'z' ,
    DBAUDIT_OFF_FAB = 123,
/* Audit the global file entries against the actual directories */
    DBAUDIT_DIR2GLOBAL = 40,
/* Audit the global wafer line and design templates */
    DBAUDIT_WL_DSGN_FILES = 41,
     DBAUDIT_EXTRA_GLOBAL_FILES = 39,
/* Audit entire directory structure */
    DBRMT_AUDIT_DIRECTORIES = 46,
    DBAUDIT_DIRECTORIES = 47,
    DBAUDIT_OFF_DESIGN = 124,
    DBREAD_RVU_DATA = 125,
    DBRMV_DATA = 126,
/* Perform df and ps on database */
    DBPS = 43,
    DBPS_DF = 44,
/* Resend global files to remote servers */
    DBRESEND_FILES = 45,
/* Copy directory to remote servers */
    DBCOPY_RMT_FILES = '0' ,
/* Force data to be shadowed to remote databases */
    DBSHADOW_DATA = 49,

    /* Database administrative functions */
/* Add template builder and template approver */
    DBADD_TMPLBLD = 51,
    DBADD_APPROVER = 52,
    DBADD_TMPLBLD_APPROVER = 53,
    DBRMT_TMPLBLD = 36,
    DBRMT_APPROVER = 37,
    DBRMT_TMPLBLD_APPROVER= 38,
    DBPUT_GLOBAL_FILE = '0' ,
    DBMOD_PASSWORD = '1' ,

	    /* Extra requests */
    DBMAKE_TMP_DIR = 34,
    DBRMTUNAPP_DATA = 35
} REQ_TYPE;


/* Secondary request type used when data type and request type */
/* cannot adequately describe the transaction */
typedef enum
{
    /* Different types of retrieve */
    CNB,
    SERIAL_NUM,
    CONTRACT_NUM,
    cwd
} SEC_REQ_TYPE;
#endif /* ifdef for global_req_types_h */
