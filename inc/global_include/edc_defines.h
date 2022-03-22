#ifndef edc_defines_h
#define edc_defines_h

#define PENDING_APPROVAL_MSG \
"You have transferred data to one or more unapproved external customers.    External customers must be approved before they can retrieve data from NXP.  Approvals are performed by your manager or a designated approver.\n\n\
Your manager can perform the following steps to approve a customer:\n\n\
    1.  Go to http://nww.edc.nxp.com and sign in.\n\
    2.  Search for the customer.\n\
    3.  Click on the customer's name.\n\
    4.  Select Approve this Email.\n\
    5.  Click on submit.\n\n\
A message indicating the customer has been approved is then displayed.\n\n\
Pending approvals are present for:\n\n"

#define REG_UPDATE_MSG \
"EDC registry updated on %s.\n\n\
    Previous number of registry entries:  %d\n\
    Current number of registry entries:  %d"

#define ONE_YEAR 31536000
#define SIX_MONTHS 15768000
#define DOMAIN_TABLE  "ext_registry/domain_table"
#define TRUSTED_SENDER_TABLE "ext_registry/trusted_sender_table"
#define CUSTOMER_TABLE "user_approvals/pending_approvals"
#define COMMANDLINE_TABLE "user_approvals/commandline_approvals"
#define EXT_REGISTRY "ext_registry/site_reg.xx"
#define EDC_REGISTRY "edc_registry/site_reg.xx"

#define INTERNAL_CUSTOMER 0
#define EXTERNAL_CUSTOMER 1
#define TRANSCEND_CUSTOMER 2

typedef enum
{
    INACTIVE,
    ACTIVE,
    NOT_REGISTERED,
    APPROVED,
    PENDING,
    REJECTED,
    REVOKED,
} APPROVE_STATUS;


/*static char reg_array[6][20] =
{
    "not registered",
    "registered",
    "approved",
    "registered_approved",
    "black listed",
    "not approved",
};*/

#endif
