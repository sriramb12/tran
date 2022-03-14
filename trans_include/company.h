#ifndef company_h
#define company_h

#include <sys/types.h>
#include <sys/stat.h>
#include "global_defines.h"


#define TOTAL_COMPANY_CNT 150

#ifdef EXTERNAL_TRANSWEB
#define COMPANY_NAME "External TransWeb Customer"
#endif

#ifndef COMPANY_NAME
#define COMPANY_NAME "NXP Semiconductor"
#endif

#define FTP_TRANSFER_MSG \
"Enter the directory name where files will be placed on the FTP machine.\n\
The home directory is: %s.\n\
If you wish to transfer the files to this directory simply enter a carriage\n\
return.  If you want to transfer the files to a subdirectory, enter the \n\
relative path for the subdirectory.  For example, if you are transferring\n\
the files to %s/test,\n\
type 'test'.  >  "

#define FTP_RTV_MSG \
"Enter the directory name where files are stored on the FTP machine.\n\
The home directory is: %s.\n\
If you wish to retrieve the files from this directory simply enter \n\
a carriage return.  If the files reside in a subdirectory, enter the \n\
relative path for the subdirectory.  For example, if you are retrieving\n\
the files from %s/test,\n\
type 'test'.  > "

#ifndef SQL_SERVER_TYPE
#define SQL_SERVER_TYPE  // to use as SELECT CAST(city AS UNSIGNED) FROM your_table;
   enum
   {
       SERVER_TRANSCEND = 1,
       SERVER_INTERCOM  = 2,
       SERVER_TRANSFTP  = 3,
       SERVER_TRANSWEB  = 4,  // no longer used
   };
#endif


typedef struct
{
    char name[48];
    char abbreviation[8];         // Abbreviation
    char machine[MACHINE_LEN];
    char address[ADDRESS_LEN];
    char socket[8];
    char server_type[SITE_LEN];
    char email[1024];
    char is_foundry[4];
    char is_maskshop[4];
    char compression[4];
    char extension[2];         // file keyword last letter
    char secure_extension[2];  // secure  keyword last letter

} TRANS_COMPANY;

typedef struct
{
    char name[48];
    char extension;          // file keyword last letter
    char secure_extension;   // secure  keyword last letter
    char machine[MACHINE_LEN];
    char address[ADDRESS_LEN];
    int socket;
    char abbreviation[48];  // temporary field just to print
} TRANS_REPOSITORY;


extern TRANS_COMPANY  gl_site_destinations[];
extern int            gl_site_destinations_counter;


extern TRANS_COMPANY site[];
extern TRANS_REPOSITORY repository[];
extern char remote_dir[];
extern char remote_company_email[];

void repository_find(char extension);

#ifdef TRANSCEND
/*int company_lookup(int argc, char **argv);*/

void company_print(void);
#endif /* for Transcend */

#ifndef INTERCOM

int icom_company_lookup(int argc, char **argv, int *is_itar);

/* Get and look for company for put commands */
void company_get(char *extension, char *argv[]);

/* Used for retrieve options */
void repository_get(char *extension);

/* Used for getting release, registering, user information, etc. */
void local_repository_get(char *extension);


/* Companies only defined for InterCom */
/* Print list of defined companies */

void company_print(void);


#endif /* For INTERCOM */

int extension_calc(char *extension, int *search, int argc, char **argv);

/* Finds information needed to send data to specified remote company */
int company_find(char *extension, char *remote_dir);

//void default_company_set();


/* Finds address of server where request is sent to */
/*
int company_lookup(int argc, char **argv);
*/

/* Sets company name: Puts company name in swd (system working directory) */
void swd_set(void);

#ifdef INTERCOM
#ifdef NXP

int icom_company_lookup(int argc, char **argv, int *is_itar);


/* Used for retrieve options */
void repository_get(char *extension);

/* Used for getting release, registering, user information, etc. */
void local_repository_get(char *extension);


int extension_calc(char *extension, int *search, int argc, char **argv);


int company_find(char *extension, char *transnet_access);




int repository_print(void);

/* Sets default site for transaction */
int default_repository_set(void);

int default_site_set(void);



/* Get the latest company table from the data repository */
void company_table_rtv(void);

/* Prompt user for mask set if not decuded from file name */
int mask_info_get(char *file_name, char *mask);

int is_mask(char *mask);

void maskshop_info_build(void);

void tsmc_info_get(char command);

/* Logs info in send2maskshop_log file */
void maskshop_file_log(char *file_name, struct stat *buf, int get_buf);

#endif /* for NXP Semiconductor */
#ifdef SINGLE_CONNECT
int generic_company_lookup(int argc, char **argv);
#endif /* for SINGLE_CONNECT */
#endif /* For InterCom */

#endif /* Define for company_h */
