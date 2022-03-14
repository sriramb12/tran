#ifndef global_arrays_h
#define global_arrays_h
#define ARRAY_OFFSET 27  
/* Data types understood by the DMI database */
static char data_array[50][50] = 
{
/* Data types */
    "MASK",
    "DSGN",
    "TMPL",
    "PC",
    "TMPL_DSGN",
    "DSGN_TMPL",
    "DSGN_GROUP",
    "WAFER_LINE",
    "GLBL",
    "GLOBAL_TABLES",
    "WL_PC",
    "OFF_TMPL",
    "OFF_DSGN",
    "OFF_TMPL_OFF_DSGN",
    "OFF_TMPL_DSGN",
    "TMPL_OFF_DSGN",
    "OFF_TMPL_AUDIT",
    "OFF_DSGN_AUDIT",
    "APP_TMPL_DSGN"
};

/* Transaction types */
static char transaction_array[50][50] = 
{
    "UPD_REQ ",
    "DLT_REQ",
    "READ_REQ",
    "MOD_REQ",
    "RTV_DATA_REQ",
    "RTV_FILES_REQ",
    "CRT_REQ",
    "REMOTE_REQ",
    "GLOBAL_REQ",
    "VERSION_REQ",
    "APPROVE_REQ",
    "ALIVE_REQ",
    "RELEASE_REQ",
    "NETWORK_REQ",
    "PORT_REQ",
    "REGISTER_REQ",
    "LIST_REQ",
    "DB_ADMIN_REQ",
    "RVUPORT_REQ"
};

/* Request types */
static char req_array[200][50] = 
{
    /* Update requests */
    "DBOVRWR_DATA  ",
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
    "DBREAD_FILE ",
    "DBRTV_VERSIONS ",
    "DBUNAPP_DATA ",
    "DBBACKOUT_DATA ",
    "DBDLT_VERSIONS ",
    "DBPORT_RVU_DATA ",

/* Create", copy, and port data */
    /* Action class is create */
    "DBCRT_DATA ",
    "DBCOPY_DATA ",
    "DBCOPY_MASK2TMPL",
/* Port mask to template", done in Hercules */
    "DBPORT_MASK2TMPL",
    /* Port specified data to the database */
    "DBPORT_DATA ",
    /* Create a group of files with specified data type */
    "DBCRT_FILES",
    /* Create a speicified file */
    "DBCRT_FILE",

    /* Modify transactions */
    "DBMOD_PIN ",
    "DBMOD_OWNER ",
    /* Grant overwrite for one transaction */
    "DBGRANT_OVRWR ",
/* Set C4 mask */
    "DBSET_C4MASK ",
    /* Empty requests for future use */
    "DBSET_EMPTY1",
    "DBSET_EMPTY2",

    /* Retrieve small pieces of data */
    /* Uses transaction type DBRTV_DATA */
/* Retrieve the mask set PIN */
    "DBRTV_PIN",
    "DBRTV_CWD",
    /* Retrieve serial number for Mesa Mask Shop */
    "DBRTV_SERIAL ",
    "DBRTV_COM1SERIAL",
/* Retrieve annual contract number */
    "DBRTV_CNB",
    "DBRTV_CONTRACT_NUM ",
/* Retrieve value of C4 mask */
    "DBRTV_C4MASK",

    /* Registration requests */
    /* Transaction type of REGISTER_REQ */
    "DBCHK_REG",
    "DBAUTO_REG",
    "DBMAN_REG",
    "DBFORCE_REG",
    "DBRMT_MAN_REG",
    "DBRMT_FORCE_REG",
/* Resend registration entries */
    "DBRESEND_REG",

    /* List requests */
    /* Have transaction type LILST_REQ */
    /* Pull out file with a list of items */
    /* List PC links */
    "DBLIST_LINKS ",
    /* Pull down list of PC libraries */
    "DBLIST_PCS ",
    /* List masks with specified search criteria */
    "DBLIST_MASKS ",
    /* Send list of all masks */
    "DBLIST_ALL_MASKS ",
    /* Send list of templates with specified criteria */
    /* Currently being used by Hercules */
    "DBLIST_TMPLS",
   /* Currently not being used */
    "DBLIST_SERVERS ",
    /* Unused list request */
    "DBLIST_EMPTY1",
    "DBLIST_EMPTY2",

    /* Retrieve one or more files */
    /* Have transaction type RTV_FILES */
    "DBRTV_FILES ",
    "DBRTV_FILE",
    "EMPTY_REQ ",

/* Remote request types */
    "DBRMTCRT_DATA ",
    "DBRMTDLT_DATA ",
    "DBMARK_DATA ",
    "DBREFRESH_DATA ",
    "DBRMTPORT_RVU ",
    "DBRMT_AUDIT_DATA ",

/* PC functions */
    "DBADD_LINK ",
    "DBMOD_LINK ",
    "DBDLT_LINK ",
    "DBREPLACE_DATA ",
    "DBRENAME_DATA ",

    "DBREAD_CONTROL_GROUP ",

    /* Network requests */

    "DBRESEND_DATA ",
    "DBUPD_SERVER ",
    "DBPING_REQ ",
    "DBREAD_DIR ",

    /* Audit actions */
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

    /* Database administrative functions */
/* Add template builder and template approver */
    "DBADD_TMPLBLD",
    "DBADD_APPROVER",
    "DBADD_TMPLBLD_APPROVER",
    "DBPUT_GLOBAL_FILE ",
    "DBMOD_PASSWORD "
};


#endif /* ifdef for global_arrays_h */
