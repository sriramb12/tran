#ifndef customer_table_h
#define customer_table_h

#include "trans_server_globals.h"
#include "edc_defines.h"

#define XIO_COMPANY "External InterCom Only"
extern int customer;
extern int customer_type;

typedef enum
{
    FTP_TRANS,
    SECNET_INTERCOM,
    EBUS_INTERCOM,
    INTERNAL_FSS,
    EXTERNAL_FSS,
    INTERNAL_TRANSWEB,
    INTERNAL_INTERCOM,
    INTERNAL_MM,
    EXTERNAL_MM,
    EXTERNAL_TRANSWEB,
    EXTERNAL_INTERCOM,
    EXTERNAL_INTERCOM_ONLY,
} CUSTOMER;


typedef enum
{
    CUSTOMER_TYPE,
    SEND_FILE_KEY,       /* Send file keyword to sender to send to receiver */
    AUTHEN_TYPE,         /* Authenticate external or internal customer */
    SEND_SECURE_KEY,     /* Require user to send secure keyword to at least one customer */
    RETRIEVE_SECURE_KEY, /* Must enter secure key, but does not need file key also */
    RETRIEVE_FILE_KEY,   /* Require customer to enter both file and secure keyword */
    COMPANY_CHECK,
    TRANSWEB_CUSTOMER_ONLY,
    EMAIL_TYPE,
    EMAIL_CHECK,
    EMAIL_LOG,
    FILE_ACCESS_TYPE,
    EDC_CHECK,
} TRANSACTION;

typedef enum
{
    INTRANET,
    EXTRANET,
    FSS_AUTHEN,
    DIRECT,
    INTERNAL_SERVER,
    CRON_JOB,
    DIRECT_INTERNAL_SERVER,
} authen_type;

/* type of E-mail checks */
typedef enum
{
    LDAP_CHECK,
    FREE_CHECK,
} email_check;

/* Specifies type of access to files the user has */
typedef enum
{
    ALL,
    BY_COMPANY,
    BY_AUTHENTICATION,
    TO_NXP_ONLY,
} file_access_type;

#if defined(INTERCOM)
   static short customer_table[13][12] =
    {
    /* Authen type */ { INTRANET, INTRANET, INTRANET, FSS_AUTHEN, FSS_AUTHEN, FSS_AUTHEN, INTRANET, EXTRANET, INTRANET, EXTRANET, EXTRANET , EXTRANET } ,
    /* Send secure key to at least 1 customer */ {  DBNO, DBNO, DBNO, DBNO, DBYES, DBYES, DBNO, DBYES, DBNO, DBYES, DBNO , DBNO } ,
    /* Retrieve secure key */ {  DBNO, DBNO, DBNO, DBNO, DBYES, DBYES, DBNO, DBYES, DBNO, DBYES , DBYES , DBNO } ,
    /* Send file key */ {  DBNO, DBNO, DBYES, DBYES, DBNO, DBNO, DBYES, DBNO, DBYES, DBNO , DBNO , DBYES } ,
    /* Enter secure & file key */ {  DBNO, DBNO, DBNO, DBNO, DBNO, DBYES, DBNO, DBYES, DBNO, DBYES , DBNO , DBNO} ,
    /* Companies authorized */ {  DBNO, DBYES, DBYES, DBYES, DBYES, DBYES, DBYES, DBYES, DBYES, DBYES, DBYES , DBYES } ,
    /* External TransWeb only */ {  DBNO, DBNO, DBYES, DBNO, DBNO, DBNO, DBYES, DBNO, DBNO, DBNO , DBNO , DBNO } ,
    /* Email type */ { DIRECT, INTERNAL_SERVER, DIRECT, INTERNAL_SERVER, CRON_JOB, CRON_JOB, DIRECT, DIRECT , DIRECT, DIRECT , DIRECT , DIRECT },
    /* Email_check */ { DBNO, DBNO, LDAP_CHECK, DBNO, DBNO, DBNO, DBNO, FREE_CHECK , LDAP_CHECK, FREE_CHECK , DBNO , DBNO },
    /* EMAIL_LOG */ { DBNO, DBNO, DBYES, DBNO, DBNO, DBNO, DBYES, DBYES, DBYES, DBYES , DBNO , DBNO } ,
    /* File access type */ { ALL, BY_COMPANY, ALL, ALL, BY_COMPANY, BY_COMPANY, ALL, TO_NXP_ONLY , ALL, TO_NXP_ONLY , TO_NXP_ONLY , TO_NXP_ONLY },
    /* EDC check */ { EDC_CHECK, EDC_CHECK, EDC_CHECK, EDC_CHECK, EDC_CHECK, EDC_CHECK, EDC_CHECK, EDC_CHECK, EDC_CHECK, EDC_CHECK, EDC_CHECK, EDC_CHECK, },
    /* EDC type */ { INTERNAL_CUSTOMER, INTERNAL_CUSTOMER, INTERNAL_CUSTOMER, INTERNAL_CUSTOMER, EXTERNAL_CUSTOMER, INTERNAL_CUSTOMER, INTERNAL_CUSTOMER, EXTERNAL_CUSTOMER, INTERNAL_CUSTOMER, EXTERNAL_CUSTOMER, EXTERNAL_CUSTOMER, INTERNAL_CUSTOMER }
    } ;
#else
    static short customer_table[1][8] =
    {
            /* Customer type */
        {
            INTERNAL_CUSTOMER, INTERNAL_CUSTOMER, INTERNAL_CUSTOMER, INTERNAL_CUSTOMER, EXTERNAL_CUSTOMER, EXTERNAL_CUSTOMER, INTERNAL_CUSTOMER, INTERNAL_CUSTOMER,
        }
    };
#endif  //if defined(INTERCOM)

#endif //customer_table_h
