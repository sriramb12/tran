#ifndef rmt_state_table_h
#define rmt_state_table_h

#define RMT_REQ_OFFSET DBRMT_COMPLETE_ARCHIVE
/* States remote data can be in */
#define PRESENT 0
#define NOT_PRESENT 1
#define CORRUPTED 2

/* Remote return messages */
#define RMT_REF "Remote %s refreshed on %s."
#define RMT_CRT "Remote %s created on %s."
#define COR_REF "Corrupted %s refreshed on %s."
#define RMT_DLT "Remote %s deleted on %s."
#define COR_DLT "Corrupted %s deleted on %s."
#define NON_DLT "Nonexistent %s not deleted on %s."
#define RMT_MRK "Remote %s marked on %s."
#define RMT_ARC "Archive completed for remote %s on %s."
#define RMT_AUD "Check sums for %s correct on %s."
#define RMT_RVU_PORT "Remote RVU files for %s ported on %s."

static char *rmt_state_msg[7][3] =
{
    /* Present, Not present, Corrupted */
    RMT_ARC, RMT_ARC, RMT_ARC,	/* Remote complte archive */
    RMT_AUD, RMT_CRT, COR_REF,
    RMT_REF, RMT_CRT, COR_REF,
    RMT_DLT, NON_DLT, COR_DLT,
    RMT_MRK, RMT_CRT, RMT_REF, /* Mark */
    RMT_REF, RMT_CRT, COR_REF, /* Refresh */
    RMT_RVU_PORT, RMT_CRT, RMT_RVU_PORT, /* Port RVU data */
};


static short rmt_state_table[7][3] =
{
    /* Present, not present, corrupted */
    DBRMT_COMPLETE_ARCHIVE, DBRMT_COMPLETE_ARCHIVE, DBRMT_COMPLETE_ARCHIVE, /* Remote complete archive */
    DBRMT_AUDIT_DATA, DBRMTCRT_DATA, DBREFRESH_DATA, /* Audit data */
    DBREFRESH_DATA, DBRMTCRT_DATA, DBREFRESH_DATA, /* Remote create */
    DBRMTDLT_DATA, DBRMTDLT_DATA, DBRMTDLT_DATA, /* Delete data */
    DBMARK_DATA, DBRMTCRT_DATA, DBREFRESH_DATA, /* Mark data */
    DBREFRESH_DATA, DBRMTCRT_DATA, DBREFRESH_DATA, /* Refresh */
    DBRMTPORT_RVU, DBRMTCRT_DATA, DBRMTPORT_RVU, /* Port RVU files */
};
#endif /* Define for rmt_state_table_h */
