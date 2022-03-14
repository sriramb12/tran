#ifndef maskshop_prototypes_h
#define maskshop_prototypes_h

/* Prototypes for functions used by Send to Mask Shop */

/* where DB_ERRS is defined */
#include "global_defines.h"

/* Initialize globals used by the database client */
void init_globals( void );

/* Retrieve specified file from the Mask Prep database */
int dbrtv_global_file( char* input_file[],
                       char* output_file[] );


/* Sends Mask Shop files to ESE machine using FTP */
/* Currently used to send data to Dupont and Photronics via dock */
DB_ERRS dbsend_ese(
    char *mask,		/* Mask set name */
    char *phone,		/* Phone number put in tickler file */
    char *ese_login,	/* User's login name on the ESE machine */
    char *ese_password,	/* User's password on the ESE machine */
    char *ese_addr,
    char *maniac_addr,
    char *auth_account,
    char *email_addr,	/* Email address where notification sent */
     char *file_list[],	/* List of files to send, last entry points to NULL */
    char *maniac_addr, /* Holds IP node address for maniac machine */
    char *maskshop_mach, /* Holds full name of mask shop machine */
    char *auth_account,	/* Name of authorized account */
    char *maskshop_email,	/* Mask shop  address for email notification */
    char batch_flag);	/* Indicates if job is to run in background */

/* Send Mask Shop files to Mask Shop via FTP */
DB_ERRS dbsend_ftp(
    char *mask,		/* Mask set name */
    char *phone,		/* Phone number put in tickler file */
    char *ftp_login,	/* User's login name on the external machine */
    char *ftp_password,	/* User's password on the external machine */
    char *email_addr,	/* Email address where notification sent */
     char *file_list[],	/* List of files to send, last entry points to NULL */
    char *maniac_addr, /* Holds IP node address for maniac machine */
    char *maskshop_mach, /* Holds full name of mask shop machine */
    char *maskshop_email,	/* Mask shop  address for email notification */
    char batch_flag);	/* Indicates if job is to be in backgroun */

#endif /* ifdef for maskshop_prototypes_h */
