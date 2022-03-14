#ifndef maskshop_prototypes_h
#define maskshop_prototypes_h

/* Prototypes for functions used by Send to Mask Shop */

/* DB_ERRS is defined in global_req_types.h */
#include "global_req_types.h"

/* Prevent C++ compiler from mangling the function names */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Initialize globals used by the database client */
void init_globals( void );

/* Retrieve specified file from the Mask Prep database */
int dbrtv_global_file( char* input_file[],
                       char* output_file[] );

/* Sends specified files to the Mesa Mask Shop */
DB_ERRS dbsend_mesa(
    char *mask,	/* Name of mask set being sent to Mesa */
    char *email_addr,	/* User's Email address */
    char *file_list[]);	/* List of files to send */

/* Sends Mask Shop files to ESE machine using FTP */
/* Currently used to send data to Dupont and Photronics via dock */
DB_ERRS dbsend_ese(
    char *mask,		/* Mask set name */
    char *phone,		/* Phone number put in tickler file */
    char *ese_login,	/* User's login name on the ESE machine */
    char *ese_password,	/* User's password on the ESE machine */
    char *email_addr,	/* User's Email address on the ESE machine */
    char *file_list[],	/* List of files to send to mask shop */
    char *maniac_addr, /* Holds IP node address for maniac machine */
    char *maskshop_mach, /* Holds full name of mask shop machine */
    char *auth_login);	/* Name of authorized account */

/* Send Mask Shop files to Mask Shop via FTP */
DB_ERRS dbsend_ftp(
    char *mask,		/* Mask set name */
    char *phone,		/* Phone number put in tickler file */
    char *ese_login,	/* User's login name on the ESE machine */
    char *ese_password,	/* User's password on the ESE machine */
    char *email_addr,	/* User's Email address on the ESE machine */
    char *file_list[],	/* List of files to send to mask shop */
    char *maniac_addr, /* Holds IP node address for maniac machine */
    char *maskshop_mach); /* Holds full name of mask shop machine */


#ifdef __cplusplus
}
#endif /* __cplusplus */
   
#endif /* ifdef for maskshop_prototypes_h */
