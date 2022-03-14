#ifndef m2_req_types_h
#define m2_req_types_h
/* Data types understood by the DMI database */
typedef enum
{
/* Data types */
    MASK_SET = 0,
    FAB,
    FABFILE,
    TEMPLATE,
    PC,
    PARTITION,
    PC_HISTORY,
    METRICS,
    MACHINE,
    GLOBAL,
    REMOTE,
    REGISTER,
    USERGROUP,
    NETWORK,
    FILES,
    CRON,
    VERSION,
    GLOBAL_TABLES,
    OFF_TMPL,
    DD_ADMIN,
    SERIAL,
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
    LOCK4ARCHIVE,
    COMPLETE_ARCHIVE,
    RETRIEVE,
    READ_ANY,
    RETRIEVE1FILE,
    RESTORE1MASK,
    C4SET,
    C4GET,
    MOD_OWNER ,
    GRANTOVERWRITE,
    CREATE,
    COPY,
    RENAME,
    SHADOW,
    VERSIONS,
    HISTORY,
    LIST,
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
    SEND_DAILY,
    SEND_MONTHLY,

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
    RMT_SHADOW,
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
   RTV_H_INFO,
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

/* PC functions */
    SEND_WEEKLY,

    READ_CONTROL_GROUP ,

    /* Network requests */
    RESEND_DATA ,
    AUDIT_DATA,
    UPD_SERVER ,
    PING_REQ ,
    EVENT_LOG,
    APPEND,

/* Perform df and ps on database */
    DBPS,
    DBPS_DF,
/* Copy directory to remote servers */
    COPY_RMT_FILES ,
    /* Database administrative functions */
/* Add template builder and template approver */
    ADD_TMPLBLD,
    ADD_TMPLBLD_APPROVER,
    RMT_TMPLBLD,
    RMT_APPROVER,
    RMT_TMPLBLD_APPROVER,
    PUT_GLOBAL_FILE ,

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
#endif /* ifdef for mask_req_types_h */
