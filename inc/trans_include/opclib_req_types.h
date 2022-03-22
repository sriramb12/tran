#ifndef opclib_req_types_h
#define opclib_req_types_h

/* Poseidon request types */
typedef enum  
{
    CREATE,
    	CLONE,
    READ,
    LOCK,
    CHECKOUT,
    CANCEL_LOCK,
    CHECKIN,
    DELETE,
    ADD,
    RTV_LIST,
    RTV_HISTORY,
    RTV_FORMS,
    RTV_PACKAGE,
    RTV_VERSION,
    UPDATE_VERSION,
CHECKOUT_VERSION,
    CHANGE_PIN,
    CHECK_PIN,
} POSEIDON_REQ_TYPE;

/* Data types */
typedef enum
{
    LIBRARY,
    FORMSGROUP,
    PACKAGE,
    PACKAGE_FILE,
    FORMS_FILE,
    GLOBAL,
} POSEIDON_DATA_TYPE;

/* Poseidon request type arrays */
static char POSEIDON_REQ_ARRAY[18][16] =
{
    "CREATE",
    "	CLONE",
    "READ",
    "LOCK",
    "CHECKOUT",
    "CANCEL_CHECKOUT",
    "CHECKIN",
    "DELETE",
    "Add",
    "RTV_LIST",
    "RTV_HISTORY",
    "RTV_FORMS",
    "RTV_PACKAGE",
    "RTV_VERSION",
    "UPDATE_VERSION",
    "CHKOUT_VERSION",
     "CHANGE PIN",
    "CHECK_PIN",
};

/* Data type arrays */
static char POSEIDON_DATA_ARRAY[6][20] =
{
    "LIBRARY",
    "FORMSGROUP",
    "PACKAGE",
    "FORMS_PACKAGE_FILE",
    "FORMS_FILE",
    "GLOBAL",
};

#endif /* define for opclib_req_type */
