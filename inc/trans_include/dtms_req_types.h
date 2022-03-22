#ifndef dtms_req_types_h
#define dtms_req_types_h

/* Server types */
typedef enum
{
    DFM_SERVER,
    DTMS_SERVER,
    GDS_SERVER,
    PATHCHECK_SERVER,
} SERVER_TYPE;

/* Server types in an array */
static char server_type_array[4][12] =
{
    "DFM_SERVER",
    "DTMS_SERVER",
    "GDS_SERVER",
    "Pathcheck",
};

/* Request types */
typedef enum
{
    DFM_JOB_SUBMIT,
    DTMS_JOB_SUBMIT,
    GDS_JOB_SUBMIT,
    REGISTER,
    GTAR_GET,
    PATHCHECK_SUBMIT,
    Q_GET,
} DTMS_REQ_TYPE;

static char request_type_array[6][16] =
{
    "DFM job submit",
    "DTMS job submit",
    "GDS job submit",
    "register",
    "Gtar get",
    "Job submit"
};

#endif
