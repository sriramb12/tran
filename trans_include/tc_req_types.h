#ifndef tc_req_types_h 
#define tc_req_types_h

/* List of defined TC request types */
typedef enum
{
    REGISTER,
    JOB_SUBMIT,	/* Submit a new TC job */
    JOB_CANCEL,	/* Cancel an existing generic job */
    Q_GET,
    Q_AUDIT,
    RUN_NEW_JOB,
    AUTOREG,
    CHECKREG,
    CLEANUP,
    ANDY_CLEANUP,
    AUTH_CHECK,
    CONTROL_FILE_GET,
    CONTROL_FILE_LIST,
    CONTROL_FILE_PUT,
    CONTROL_FILE_GET_PRIOR,
    TECHNOLOGY_GET,
    TECHNOLOGY_FILE_PUT,
    TECHNOLOGY_FILE_GET,
    TECHNOLOGY_FILE_LIST,
    TECHNOLOGY_GET_PRIOR,
    FILE_GET,
    MKDIR,	/* Send an DFM directory */
    RMDIR,	/* Delete an existing directory on the RET machine */
    AUDIT,
    GTAR_GET,
    DP_JOB_SUBMIT,
} TC_REQ_TYPE;


#endif /* Define for tc_req_types_h */
