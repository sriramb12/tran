#ifndef m2globals_h
#define m2globals_h

#define NONE "0"
#define DBYES 1
#define DBNO 2
#define SUCCESS 0
#define AUTOSAVE_PRESENT -840
#define AUTOSAVE_NOT_PRESENT -841

#define RTNMSG_LEN 2048

/* Define for programs that update the reticle layout in the SQL database */
#define RETICLE_BUILDER 0
#define IMPORT_COORDS 1


typedef enum 
{
    ALL_MASKS,
    VALID_MASKS,
} M2LIST_TYPE;

/* Defines for different types of read transactions */
typedef enum
{
    M2APPROVED,	/* Retrieve approved version */
    M2LATEST,	/* Retrieve latest version */
    M2BY_VERSION,	/* Retrieve specified version */
    M2BY_VERSION_AUTOMERGE	/* by version but warn if not approved version */
} RTV_TYPE;

/* Status of database transaction returned to the GUI */
typedef enum
{
    M2DB_UNAVAIL = -7,
    M2BADPIN = -6,	/* User entered an incorrect PIN */
    M2NOTREG = -5,	/* User is not registered */
    M2REG_WRN = -4,
    M2WRN  = -3,
    M2OVR = -2,
    M2ERR = -1,
    M2SUCCESS = 0
} M2ERRS;

/* Indicates if date passed is prior, since, on or range */
typedef enum  
{
    ON = 1,
    PRIOR,
    RANGE,
    SINCE
} M2RANGE;

/* Indicates if time is create time, modify time or none */
/* Also specifies if user is creator, owner, or last modified by */
typedef enum 
{
    M2NONE,
    M2CREATE,
    M2MODIFY,
    M2OWN
} M2TIME;

/* Maximum number of fabs and technologies client expecting */
#define MAX_FAB_NUM 100
#define MAX_TECH_NUM 50

/* Mask shop names passed to the database while getting a contract number */
#define DUPONT "dupont"
#define PHOTRONICS "photronics"
#define RTC "rtc"

/* Data needed to send data to the mask shop */
typedef struct {
    int transfer_type;
    int maskshop;
    char login[16];
    char password[16];
    char maniac_addr[64];
    char dest_machine[64];
    char auth_account[16];	/* ESE authorized account i.e. phot */
} MASKSHOP_DATA;

/* Enums for different programs which access the database */
typedef enum 
{
    MASK2000,
    FABINT,
    RETSETUP,
    DATABASE,
    BUMPMASK,
    HERCULES,
    PADLOC,
    SHOTMAP,
    RPC
} DMI_PROGRAM;


#endif
