#ifndef global_req_types_h
#define global_req_types_h
/* Data types understood by the DMI database */
typedef enum
{
/* Data types */
    "MASK = 0",
    "DSGN",
    "TMPL",
    "PC_LIB",
    "DSGN_GROUP",
    "WAFER_LINE",
    "GLBL",
    "VERSION",
    "GLOBAL_TABLES",
    "OFF_TMPL",
    "OFF_DSGN",
    "OFF_TMPL_AUDIT",
    "OFF_DSGN_AUDIT",
    "APP_TMPL_DSG",
} DATA_TYPE;

/* Transaction types */
typedef enum
{
    "UPDATE",
    "DELETE",
    "READ",
    "MODIFY",
    "RTV_DATA",
    "RTV_FILES",
    "CREATE",
    "REMOTE",
    "GLOBAL",
    "VERSION",
    "APPROVE",
    "ALIVE",
    "RELEASE",
    "NETWORK",
    "PORT",
    "REGISTER",
    "LIST",
    "DB_ADMIN",
    "RVUPORT",
    "CRON",
} TRANSACTION;

/* Request types */
typedef enum
{
    "/* Update requests */
    "DBOVRWR_DATA = 0  ",
    "DBCHKIN_DATA ",
    "DBUPD_DATA ",
    "DBCHKOFF_DATA ",
    "DBDLT_DATA ",

/* Check out and read transactions */
    "DBCHKOUT_DATA ",
    "DBAPP_DATA ",

/* Modify transactions */
    "DBCANCHKOUT_DATA ",
    "DBCANAPP_DATA ",
    "DBTRANSFER_DATA ",

/* Read data", retrieve versions */
    "DBREAD_DATA ",
    "DBREAD_FILE ",	/* 94 */
    "DBRTV_VERSIONS ",
    "DBUNAPP_DATA ",
    "DBBACKOUT_DATA ",
    "DBDLT_VERSIONS ",
    "DBPORT_RVU_DATA ",
    "DBRESTORE1MASK ",
DBLOCK4ARCHIVE",
DBCOMPLETE_ARCHIVE",
DBSEND_DAILY",
    "DBSEND_MONTHLY",

/* Create", copy, and port data */
    "/* Action class is create */
    "DBCRT_DATA ",
    "DBCOPY_DATA ",
DBCOPY_MASK2TMPL",
/* Port mask to template", done in Hercules */
    "DBPORT_MASK2TMPL ",
    "/* Port specified data to the database */
    "DBPORT_DATA ",

    "/* Modify transactions */
    "DBMOD_PIN ",
    "DBMOD_OWNER ",
    "/* Grant overwrite for one transaction */
    "DBGRANT_OVRWR ",
/* Set C4 mask */
    "DBSET_C4MASK ",	/* 95 */
    "DBRTV_MASK_FILES",
    "DBRTV_MASKSET_NAME",
    "/* Empty requests for future use */

    "/* Retrieve small pieces of data */
    "/* Uses transaction type DBRTV_DATA */
/* Retrieve the mask set PIN */
    "DBRTV_PIN ",
    "DBRTV_CWD",
    "/* Retrieve serial number for Mesa Mask Shop */
    "DBRTV_MOS6SERIAL ",
    "DBRTV_MOS16SERIAL  ",
    "DBRTV_MOS17SERIAL ",
    "DBRTV_MOS21SERIAL ",
    "DBRTV_COM1SERIAL ",
/* Retrieve annual contract number */
    "DBRTV_CNB ",
    "DBRTV_CONTRACT_NUM ",
/* Retrieve value of C4 mask */
    "DBRTV_C4MASK ",

    "/* Registration requests */
    "/* Transaction type of REGISTER  */
    "DBCHK_REG",
    "DBAUTO_REG",
    "DBMAN_REG",
    "DBRMT_MAN_REG",
/* Resend registration entries */
    "DBRESEND_REG",

    "/* List requests */
    "/* Have transaction type LIST_REQ */
    "/* Pull out file with a list of items */
    "/* List PC links */
    "DBLIST_LINKS ",
    "/* Pull down list of PC libraries */
    "DBLIST_PCS",
    "/* List masks with specified search criteria */
    "DBLIST_MASKS ",
    "/* Send list of all masks */
    "DBLIST_ALL_MASKS ",
    "/* Send list of templates with specified criteria */
    "/* Currently being used by Hercules */
    "DBLIST_TMPLS",
   /* Currently not being used */
    "AUDIT",
    "/* Unused list request */
    "DBLIST_EMPTY1",
    "DBLIST_EMPTY2",

    "/* Retrieve one or more files */
    "/* Have transaction type RTV_FILES */
    "DBRTV_FILES ",
    "DBRTV_FILE",

/* Remote request types */
    "DBRMT_COMPLETE_ARCHIVE",
    "DBRMT_AUDIT_DATA",
    "DBRMTCRT_DATA",
    "DBRMTDLT_DATA",
    "DBMARK_DATA",
    "DBREFRESH_DATA",
    "DBRMTPORT_RVU",

/* PC functions */
    "DBADD_LINK ",
    "DBMOD_LINK ",
    "DBDLT_LINK ",
    "DBREPLACE_DATA ",
    "DBRENAME_DATA ",

    "DBREAD_CONTROL_GROUP ",

    "/* Network requests */

    "DBRESEND_DATA ",
    "DBUPD_SERVER ",
    "DBPING_REQ ",
    "DBREAD_DIR ",

    "/* Audit actions */
    "DBRMT_AUDIT_GLOBAL ",
    "DBAUDIT_DATA ",
    "DBAUDIT_GLOBAL ",
    "DBAUDIT_FAB ",
    "DBAUDIT_DESIGN ",
    "DBAUDIT_MASK ",
    "DBAUDIT_PC ",
    "DBAUDIT_OFF_FAB",
/* Audit the global file entries against the actual directories */
    "DBAUDIT_DIR2GLOBAL",
/* Audit the global wafer line and design templates */
    "DBAUDIT_WL_DSGN_FILES",
    " DBAUDIT_EXTRA_GLOBAL_FILES",
/* Audit entire directory structure */
    "DBRMT_AUDIT_DIRECTORIES",
    "DBAUDIT_DIRECTORIES",
    "DBAUDIT_OFF_DESIGN",
    "DBREAD_RVU_DATA",
    "DBRMV_DATA",
/* Perform df and ps on database */
    "DBPS",
    "DBPS_DF",
/* Resend global files to remote servers */
    "DBRESEND_FILES",
/* Copy directory to remote servers */
    "DBCOPY_RMT_FILES ",
/* Force data to be shadowed to remote databases */
    "DBSHADOW_DATA",

    "/* Database administrative functions */
/* Add template builder and template approver */
    "DBADD_TMPLBLD",
    "DBADD_APPROVER",
    "DBADD_TMPLBLD_APPROVER",
    "DBRMT_TMPLBLD",
    "DBRMT_APPROVER",
    "DBRMT_TMPLBLD_APPROVER",
    "DBPUT_GLOBAL_FILE ",
    "DBMOD_PASSWORD ",

	    "/* Extra requests */
    "DBMAKE_TMP_DIR",
    "DBRMTUNAPP_DATA",
} REQ_TYPE;


/* Secondary request type used when data type and request type */
/* cannot adequately describe the transaction */
typedef enum
{
    "/* Different types of retrieve */
    "CNB",
    "SERIAL_NUM",
    "CONTRACT_NUM",
    "cwd",
} SEC_REQ_TYPE;
#endif /* ifdef for mask_req_types_h */
