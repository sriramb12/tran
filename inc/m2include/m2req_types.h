#ifndef m2_req_types_h
#define m2_req_types_h
/* Data types understood by the DMI database */
typedef enum
{
/* Data types */
    MASK_SET = 0,
    FAB,
    OFF_TEMPLATE,
    TEMPLATE,
    PC,
    PARTITION,
    PC_HISTORY,
    METRICS,
    MACHINE,
    GLOBAL,
    OFF_GLOBAL,
    REGISTER,
    USERGROUP,
    NETWORK,
    M2FILES,
    CRON,
    VERSION,
    GLOBAL_TABLES,
    OFF_TMPL,
    DD_ADMIN,
    SERIAL,
    SQL_QUERY,
    CAP_WAFER,
} DATA_TYPE;


/* Request types */
typedef enum
{
    /* Update requests */
    OVERWRITE,
    CHECKIN,
    UPDATE,
    SAVE,
    DELETE,
    CHECKOUT,
    APPROVE,
    CANCHECKOUT,
    TRANSFER,
    READ,
    RTV_VERSIONS,
    UNAPPROVE,
    M2ARCHIVE,
    UNARCHIVE,
    RETRIEVE,
    READ_ANY,
    RETRIEVE1FILE,
    RESTORE1MASK,
    C4SET,
    TRANSACTION_LOG_RTV,
    MOD_OWNER ,
    GRANTOVERWRITE,
    CREATE,
    COPY,
    RENAME,
    SHADOW,
    VERSIONS,
    HISTORY,
    M2LIST,
    LISTALL,
    DELETE_ALL,
    MOD_USERGROUP,
    M2STATUS,
    /* Retrieve data for MARS */
    RETRIEVE_MASK_NAME,
    RETRIEVE_H_NAME,
    RETRIEVE_PC_NAME,
    /* Retrieve serial numbers */
    RETRIEVE_MOS6SERIAL ,
    RETRIEVE_MOS16SERIAL  ,
    RETRIEVE_MOS17SERIAL ,
    RETRIEVE_MOS21SERIAL ,
/* Retrieve annual contract number */
    RETRIEVE_ANNUAL ,
    RETRIEVE_CONTRACT_NUM ,
	EVENT_LOG_METRICS,
    MARS_METRICS,

    /* Registration requests */
    CHECK_EMAIL,
    CHECK_PIN,
    CHECK_X500,
    REG_PERFORM,
    MANUAL_REG,
    REMOTE_REG,
    RESEND_REG,
    PIN_SET,
    PIN_GET,
    X500QUERY,
    /* Remote request types */
    AUDIT,
    OFF_AUDIT,
    REFRESH,
    MARK,
    RMT_CREATE,
    RMT_DELETE,
    RMT_RENAME,
    RMT_REG,
    RMT_AUDIT,
    RMT_AUDIT_STATE,
    RMT_LOCK4ARCHIVE,
    RMT_COMPLETE_ARCHIVE,
    RMT_SHADOW_CHECK,


    /* Retrieve one or more files */
    /* Have transaction type RTV_FILES */
    LIST_MEMBERS,
    RETRIEVE_FILES ,
    RTV_NAME_PHONE,
    RTV_MASK_INFO,
    RTV_PC_INFO,
    RTV_RET_PASSWORD,
    /* MDSadmin request types */
    ADD_USERGROUP,
    ADD_USER,
	UNREGISTER,
    ADD_APPROVER,
    ADD_BUILDER,
    ADD_PC_APPROVER,
    ADD_PC_BUILDER,
    VIEW_CWD,
    VIEW_USERS,
    VIEW_USERGROUPS,
    ADMIN_CHECK,
    EVENT_LOG,

    /* Network requests */
    AUDIT_DATA,
    RESEND_DATA ,
    SEND_WEEKLY,
    SEND_GROUP,
    SEND_ADMIN,
    REPORT_LIST,
    REPORT_JOIN,
    APPEND,
    RTV_RET_MSO_PASSWORDS,

/* Perform df and ps on database */
    DBPS_DF,
/* Copy directory to remote servers */
    COPY_RMT_FILES ,
    /* Database administrative functions */
/* Add template builder and template approver */
    MASK_PROJECTS_RTV,
    DTMS_STATUS_RTV,
    ADD,
    RETICLE_LAYOUT_UPDATE,
    RMT_TMPLBLD_APPROVER,
    PUT_GLOBAL_FILE ,
    AUDIT_FILE,
    AUDIT_DIR,
    RMT_AUDIT_FILE,
    RMT_AUDIT_DIR,
    CHECKOUT_STATUS_RTV,
    COMMON_LAYERS_UPDATE,
} M2REQ_TYPE;



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
#endif /* ifdef for mask_req_types_h */
