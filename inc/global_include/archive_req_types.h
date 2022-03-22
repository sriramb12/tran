#ifndef archive_req_types_h
#define archive_req_types_h


/* Holds request types the archive server can process */
typedef enum 
{
    SAVE2ARCHIVE = 40,
    COMPLETE_ARCHIVE,
    MARK_META,
    RVU_READ,
    READ_MASK,
    LOCK1MASK,
    READ_HISTORY,
    RESTORE_MASK,
   SYS_RESTORE,
    RESET_CONFIG	/* Reset configurable archive parameters */
} ARCHIVE_REQ_TYPE;

#endif /* Define for archive_req_types_h */
