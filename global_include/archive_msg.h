archive_req_types.h                                                                                 0000777 �    �Ԧ0120355 00000000635 12042342513 014313  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef archive_req_types_h
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
                                                                                                   bits.h                                                                                              0000777 �    �Ԧ0120355 00000007752 12042342513 011547  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   /* Macros to swap the order of bytes in integer values.
   Copyright (C) 1997, 1998, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#if !defined _BYTESWAP_H && !defined _NETINET_IN_H
# error "Never use <bits/byteswap.h> directly; include <byteswap.h> instead."
#endif

/* Swap bytes in 16 bit value.  */
#define __bswap_constant_16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#ifdef BYTESWAP
# define __bswap_16(x) \
     (__extension__							      \
      ({ register unsigned short int __v;				      \
	 if (__builtin_constant_p (x))					      \
	   __v = __bswap_constant_16 (x);				      \
	 else								      \
	   __asm__ __volatile__ ("rorw $8, %w0"				      \
				 : "=r" (__v)				      \
				 : "0" ((unsigned short int) (x))	      \
				 : "cc");				      \
	 __v; }))
#else
/* This is better than nothing.  */
# define __bswap_16(x) __bswap_constant_16 (x)
#endif


/* Swap bytes in 32 bit value.  */
#define __bswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#ifdef BYTESWAP
/* To swap the bytes in a word the i486 processors and up provide the
   `bswap' opcode.  On i386 we have to use three instructions.  */
# if !defined __i486__ && !defined __pentium__ && !defined __pentiumpro__
#  define __bswap_32(x) \
     (__extension__							      \
      ({ register unsigned int __v;					      \
	 if (__builtin_constant_p (x))					      \
	   __v = __bswap_constant_32 (x);				      \
	 else								      \
	   __asm__ __volatile__ ("rorw $8, %w0;"			      \
				 "rorl $16, %0;"			      \
				 "rorw $8, %w0"				      \
				 : "=r" (__v)				      \
				 : "0" ((unsigned int) (x))		      \
				 : "cc");				      \
	 __v; }))
# else
#  define __bswap_32(x) \
     (__extension__							      \
      ({ register unsigned int __v;					      \
	 if (__builtin_constant_p (x))					      \
	   __v = __bswap_constant_32 (x);				      \
	 else								      \
	   __asm__ __volatile__ ("bswap %0"				      \
				 : "=r" (__v)				      \
				 : "0" ((unsigned int) (x)));		      \
	 __v; }))
# endif
#else
# define __bswap_32(x) __bswap_constant_32 (x)
#endif


#ifdef BYTESWAP
/* Swap bytes in 64 bit value.  */
#define __bswap_constant_64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)				      \
      | (((x) & 0x00ff000000000000ull) >> 40)				      \
      | (((x) & 0x0000ff0000000000ull) >> 24)				      \
      | (((x) & 0x000000ff00000000ull) >> 8)				      \
      | (((x) & 0x00000000ff000000ull) << 8)				      \
      | (((x) & 0x0000000000ff0000ull) << 24)				      \
      | (((x) & 0x000000000000ff00ull) << 40)				      \
      | (((x) & 0x00000000000000ffull) << 56))

# define __bswap_64(x) \
     (__extension__							      \
      ({ union { __extension__ unsigned long long int __ll;		      \
		 unsigned long int __l[2]; } __w, __r;			      \
         if (__builtin_constant_p (x))					      \
	   __r.__ll = __bswap_constant_64 (x);				      \
	 else								      \
	   {								      \
	     __w.__ll = (x);						      \
	     __r.__l[0] = __bswap_32 (__w.__l[1]);			      \
	     __r.__l[1] = __bswap_32 (__w.__l[0]);			      \
	   }								      \
	 __r.__ll; }))
#else
# define __bswap_64(x) x
#endif
                      bytes.h                                                                                             0000777 �    �Ԧ0120355 00000002666 12042342513 011733  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   /* Copyright (C) 1997 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef _BYTESWAP_H
#define _BYTESWAP_H	1

/* Get the machine specific, optimized definitions.  */
#include "bits.h"


/* The following definitions must all be macros since otherwise some
   of the possible optimizations are not possible.  */

/* Return a value with all bytes in the 16 bit argument swapped.  */
#define bswap_16(x) __bswap_16 (x)

/* Return a value with all bytes in the 32 bit argument swapped.  */
#define bswap_32(x) __bswap_32 (x)

/* Return a value with all bytes in the 64 bit argument swapped.  */
# define bswap_64(x) __bswap_64 (x)

#endif /* bytes.h */
                                                                          client_close_down.h                                                                                 0000777 �    �Ԧ0120355 00000000320 12221415620 014257  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef client_close_down_h
#define client_close_down_h

int close_down1f(char *err_msg, char *file);

int close_down1m(
    char *err_msg,
    char *file,
    char *x500,
    register short err_no);

#endif
                                                                                                                                                                                                                                                                                                                client_defines.h                                                                                    0000777 �    �Ԧ0120355 00000006531 12042342513 013553  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #define BADGE_LEN 48
#define FABINT_CLIENT_VERSION "1.0"
#define DBNO_REQ 'm'
/* Following two defines help server run as daemon under BSD */
#define DAEMON   /* become a daemon */
#define BSD
/* Defines used to print error messages */
#define LATEST 'l'
#define STARS "**********************************************************************\n\n"
#define HELP_MSG "For assistance, contact DMI database administrator:\n\n\
               Email Address:  transbug@nxp.com"
#define TAB "               "
#define RET_TAB "\n               "
#define RET_TAB8 "\n        "
#define RET_TAB_QUOTE "\n               '"
#define RET2_TAB "\n\n                "
#define RET2_TAB_QUOTE "\n\n                '"
#define DOUBLE_RET "\n\n"
#define RET_TO "'\n            to '"
#define IN_TABLE "In table:      "

/* Lengths for global parameters */
#define ADDRESS_LEN 16
#define MACHINE_LEN 64
#define USER_LEN 32
#define FILE_SIZE 256
#define PATH_SIZE 1024
#define MSG_SIZE 4096	/* Maximum size for sending message */
#define PWD_SIZE 32	/* Length of temporary encryption password */
#define NULL_PIN "000000"
#define MASK_LEN 8
#define MIN_PIN_LEN 6
#define MAX_PIN_LEN 12
#define KEY_LEN 64
#define PAR_LEN 64
#define TOTAL_FILES 50 	/* Number of files database can manipulate at one time */
#define NUM_SERVERS 5
#define SERVER_INDEX NUM_SERVERS-1
#define MAX_SERVERS 5

#define DBNO 2
#define DBEMAIL 3
#define FAILURE -1
#define SUCCESS 0
#define NONE "0"

/*
typedef struct
{
/* This structure holds variables shared by the database client and the GUI */
/* Default values for these variables are set by calling dbinit_default_globals */
/* The GUI must set appropriate values in this structure before performing database transactions */
typedef struct
{
    float vnum;
    float sec_vnum;
    char version_flag;
    char sec_version_flag;
    char override_flag; /* Indicates whether transaction continues under abnormal conditions */
} ;

/* Structure used for retrieving or passing file  */
typedef struct
{
    /* Array holding list of files  allocated and deallocated by the GUI */
    char *name[FILE_SIZE];
	/* Number of files retrieved or passed */
    int num;
    /* Current working directory files placed in or taken from */
char cwd[PATH_SIZE];
} GUI_FILES;

/* Variables set with function call */
typedef struct
{
    char pin1[MAX_PIN_LEN+1];	/* Primary PIN */
char pin2[MAX_PIN_LEN+1];	/* Secondary PIN number */
} GU_PINS;

typedef struct
{
char machine[MAX_SERVERS][MACHINE_LEN];
char address[MAX_SERVERS][ADDRESS_LEN];
int socket;
int server_num;	/* Holds number of alternate servers to connect to */
};

typedef struct
{
char password[PWD_LEN+4];	/* User's encrypted password */
    char client_version[12];	/* Version of database software client using */
char user_id[USER_LEN];	/* User ID */
char user_mach[MACHINE_LEN];	/* Machine user is working on */
char user_addr[ADDRESS_LEN]; /*   IP node address of the machine user is on. */
char user_name[64];
} DB_PARAMS;

/* Holds flags returned from a Mask Prep or Fabint transsaction */
typedef struct
{
    int wrn_flag;
    int info_flag;
    int local_wrn_flag;
    int dsgn_file_num;
    int tmpl_file_num;
    int tmp_file_offset;
    int file_offset;
    short rtn_msg_len;
char rtn_msg[1500];	/* Status message returned with each DB primitive */
} DB_FLAGS;
                                                                                                                                                                       client_lib.h                                                                                        0000777 �    �Ԧ0120355 00000000757 12154177031 012715  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef client_lib_h
#define client_lib_h


short get_string(char *buffer);

/* Retrieves a string up to 6144 characters in length */
short get_big_string(char *buffer);

short get_short(void);

char get_char(void);

short put_short(short data);

short put_long(unsigned long data);

short put_longlong(unsigned long long data);

short put_string(char *string);

short put_binary_string(char *string, short len);

int get_time(void);

long get_long(void);

long long get_longlong(void);

#endif
                 client_prototypes.h                                                                                 0000777 �    �Ԧ0120355 00000003604 12270505451 014371  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef client_prototypes_h
#define client_prototypes_h

#include <stdio.h>

/* line_get.c */
/* Gets line up to 260 characters in length */
    int line_get(FILE *fp, char *line);

/* line_get.c */
/* Gets line up to 2048 characters in length */
    int long_line_get(FILE *fp, char *line);

/* password_encrypt.c */
    void password_encrypt(char *password, char *pwd, int len);

/* password_crypt.c */
int password_crypt(char *new_password, char *old_password);
short get_ret_code(void);

short init_socket (char *buffer);

/* List functions used in all of the clients */

/* Get a list of file list */
int list_put(char *files[]);

/* Send list of list to client */
int list_get(char *files[]);

/* Delete files no longer needed */
void files_dlt(char *dir, char *files[]);

/* Read list of list from specified file */

/* Add an entry to the list */
/* Names are kept in alphabetical order */
/* Returns 1 if entry added */
/* Returns 0 if current entry replaced */
/* Returns SUCCESS if name added */
int list_add(char *list[], char *entry, char *key, int total_cnt);

/* Delete name from  list */
/* Returns SUCCESS if name deleted */
/* Returns FAILURE if name not present */
int list_delete(char *list[],  char *key, int total_cnt);

/* Verify specified name is in the list array */
int list_find(char *list[], char *key);



/* Expand a technology list */
int tech_list_expand(char *dir, char *old_files[], char *new_files[]);

/* Convert list from string format to list format */
int string_list_convert(char *string, char *list[]);

/* convert list to string */
void list_string_convert(char *list[], char *string);


void static_list_string_convert(char *list[], char *string);

/* Convert string to list format */
/* Items on list are separated by the character specified */
int string2double_list_convert(char *string, char *extension_list[], char *ratio_list[]);

#endif /* define for client_prototypes_h */
                                                                                                                            close_down.h                                                                                        0000777 �    �Ԧ0120355 00000006162 12042342513 012734  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef close_down_h
#define close_down_h


/* This file contains different close down routines */
/* These routines are used by DB server and Mask Shop server */

/* Builds help message from data in global_params */
void  help_msg_print(char *help_msg);

/* Prints error num, error message, bad value, and help message */
void close_down1d(
/* Used with bad_req_type, bad_data_type, bad_version_flag */
    char *err_msg,
    char bad_val,
    register short err_no);

/* Prints error code and one string */
void close_down2d(
    char *err_msg,
    int bad_val,
    char *string,
    register short err_no);

/* Sends formatted message with one parameter and no help message */
void close_down1f(
    char *format,
    char *par1,
    register short err_no);

/* Sends formatted string with two parameters */
void close_down2f(
    char *err_msg,
    char *par1,
    char *par2,
    register short err_no);

/* Sends a formatted message with 3 parameters and no help message */
void close_down3f(
    char *format,
    char *par1,
    char *par2,
    char *par3,
    register short err_no);

/* Sends a formatted message with 4 parameters and no help message */
void close_down4f(
    char *format,
    char *par1,
    char *par2,
    char *par3,
    char *par4,
    register short err_no);

/* Sends a formatted message with 5 parameters and no help message */
void close_down5f(
    char *format,
    char *par1,
    char *par2,
    char *par3,
    char *par4,
    char *par5,
    register short err_no);

/* Sends formatted message with one parameters and a help message */
void close_down1p(
    char *format,
    char *par1,
    register short err_no);

/* Sends formatted message with two parameters and a help message */
void close_down2p(
    char *format,
    char *par1,
    char *par2,
    register short err_no);

/* Sends user error code, error message, quoted string, and help message */
void close_down1q(
    char *err_msg,
    char *file,
    register short err_no);

void close_down1m(
    char *err_msg,
    char *file,
    register short err_no);

/* Send e-mail to sys admin and user */
void close_down1t(
    char *err_msg,
    char *file,
    register short err_no,
    char *user_email);

/* Prints one error message, 2 quoted strings and help message */
void close_down2q(
    char *err_msg,
    char *file1,
    char *file2,
    register short err_no);

/* Sends one message and help message */
void close_down(
    char *err_msg,
    register short err_no);

/* Sends user error number, one message and help message */
void close_down1s(
    char *err_msg,
    register short err_no);

/* Sends error number and 2 messages on the same line */
void close_down2s(
    char *msg1,
    char *msg2,
    register short err_no);

/* Sends error code, 3 messages, and help message */
void close_down3s(
    char *err_msg,
    char *file,
    char *help,
    register short err_no);


/* Prints one error message, 2 quoted strings and help message */
/* Also sends the database administrator E-mail concerning the problem */
void close_down2m(
    char *err_msg,
    char *file1,
    char *file2,
    register short err_no);

void help_msg_append(char *msg);
#endif
                                                                                                                                                                                                                                                                                                                                                                                                              common.h                                                                                            0000777 �    �Ԧ0120355 00000001557 12042342513 012073  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef _common_h
#define _common_h

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#ifndef SOLARIS10
#include <signal.h>
#endif
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <openssl/ssl.h>

#define CA_LIST "ssl.pem"
#ifdef TRANSCEND
#define KEYFILE "trans.ssl.pem"
#endif
#ifdef INTERCOM
#define KEYFILE "icom.ssl.pem"
#endif
#define RANDOM  "random.pem"
#define PASSWORD "password"
#define BUFSIZZ 1024

extern BIO *bio_err;
int berr_exit (char *string);
int err_exit(char *string);

SSL_CTX *initialize_ctx(char *keyfile, char *password);
void destroy_ctx(SSL_CTX *ctx);

#ifndef ALLOW_OLD_VERSIONS
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
#error "Must use OpenSSL 0.9.6 or later"
#endif
#endif

#endif
                                                                                                                                                 db_paths.h                                                                                          0000777 �    �Ԧ0120355 00000010375 12230015775 012373  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   /* Defines for mask and template paths */
#define DB_DATA_DIR "db_data"
#define CONTRACT_TABLE_DIR "contract_tables"
#define CONTRACT_TABLE_PATH "contract_tables/"
#define DISK_FILE "db_data/disk_usage"
#define EMAIL_REG_PATH "registry/email_reg."
#define SITE_REG_FILE "registry/site_reg."
#define COM1SERIAL_FILE "db_data/com1serial_file"
#define COM1SERIAL_LEAF "com1serial_file"
#define COM1SERIAL_LOG "db_data/com1serial_log"
#define EMAIL_REG_FILE "registry/email_reg."
#define TMPL_OWNERS "tmpl_owners"
#define MASKPREPSTATE ".MaskPrepState"
#define BIG_FILE_INDEX_SIZE 4
#define RESEND_FILE "rmt_log/resend_file"
#define TMP_RESEND_FILE "log/tmp_resend_file"
#define GLBL_MASKPREPSTATE "db_data/mask.MaskPrepState"
#define CNB_FILE "db_data/maskshop.cnb"
#define GLOBAL_DATA "global_data/global"
#define SYSADMIN_AUTH "db_data/sysadmin.auth"
#define TOKYO_AUTH "db_data/tokyo.auth"
#define DB_ADMIN_FILE "db_data/db_admin_file"
#define SYSADMIN_INFO "db_data/sysadmin.info"
#define APPROVE_AUTH "db_data/approve.auth"
#define APPROVE_INFO "db_data/approve.info"
#define GLOBAL_MASK_FILE "db_data/mask.sets"
#define TMP_MASK_FILE "mask.sets"
#define TMP_TMPL_FILE "templates"
#define BACKUP_MASK_FILE "backup.sets"
#define OLD_MASKFILE "mask.MaskPrepState"
#define MASKFILE ".MaskPrepState"
#define BIG_FILE_INDEX "big_files/big_file_index"
#define BIG_FILE_TABLE "big_files/big_file_table"
#define BIG_INDEX_SIZE 4
/*
#define BIG_FILE_PATH "big_files/"
#define BIG_FILE_DIR "big_files"
#define OFF_REL_PATH "official_rel/"
#define BETA_PATH "beta_rel/"
#define OFF_REL_INDEX "official_rel/index"
*/
#define WLFL_PATH "wafer_design/"
#define CONTROL_GROUP_DIR "control_groups"
#define WLFL_DIR "wafer_design"
#define WLFL_DIR "wafer_design"
#define GLBL_WL_FILE "wafer_design/wafer.lines"
#define GLBL_DSGN_FILE "wafer_design/design.groups"
/* File with names of all closure, orient, option and driver files to send */
#define GLBL_FILE "mask_shop/"

/* Defines for global design and wafer line paths  */
#define DESIGN_GROUPS "wafer_design/design.groups"
#define DSGN_DIR "wafer_design"
#define DSGN_DATA_DIR "design_data"
#define DSGN_DLTA_DIR "design_dlta"
#define DSGNO_DLTA_DIR "off_design_dlta"
#define DSGN_PATH "design_data/"
#define DSGN_DLTA "design_dlta/"
#define DSGNO_DIR "off_design_data"
#define DSGNO_PATH "off_design_data/"
#define DSGNO_DLTA "off_design_dlta/"

#define PC_PATH "pc_data/"
#define PC_DIR "pc_data"
#define PC_DLTA_DIR "pc_dlta"
#define TMP_PC_PATH "pc_tmp/"
#define PC_DLTA "pc_dlta/"
#define PC_LEN 8
#define PC_BACKUP "pc_backup/"

#define TMPL_DIR "wafer_data"
#define TMPL_PATH "wafer_data/"
#define TMP_TMPL_PATH "tmpl_tmp/"
#define TMPL_DLTA_DIR "wafer_dlta"
#define TMPL_DLTA "wafer_dlta/"
#define TMPLO_DIR "off_wafer_data"
#define TMPLO_PATH "off_wafer_data/"
#define TMPLO_DLTA_DIR "off_wafer_dlta"
#define TMPLO_DLTA "off_wafer_dlta/"
#define MASK_DIR "mask_data"
#define MASK_PATH "mask_data/"
#define TMP_MASK_PATH "mask_tmp/"
#define MASK_DLTA "mask_dlta/"
#define MASK_DLTA_DIR "mask_dlta"

/* Defines for PC library names */
#define PC_LIB ".DB"
#define PC_USR ".pc_usr"
#define PC_STATE ".pc_state"
#define GLBL_PATH_LEN strlen(GLBL_PATH)
#define DSGN_PATH_LEN strlen(DSGN_PATH)
#define TMPL_PATH_LEN strlen(TMPL_PATH)
#define SHOP_PATH "mask_shop/"
#define MASK_PATH_LEN 20
#define TMPL_DLTA_LEN strlen(TMPL_DLTA);
#define MASK_DLTA_LEN strlen(MASK_DLTA);
#define SHOP_PATH_LEN strlen(SHOP_PATH);
#define CREATE ".create"
#define CRT_SUFFIX ".crt"
#define STATE ".state"
/* File holding authorized system administrators */
#define SYSADMIN_AUTH "db_data/sysadmin.auth"
#define APPROVE_AUTH "db_data/approve.auth"
#define REMOTE_SERVER_TABLE "db_data/remote_servers"
#define GLOBAL_DRIVER_TABLE "db_data/driver.files"
#define RMT_SERVER_TABLE "db_data/rmt_servers"
#define GLOBAL_PARAMS "db_data/global_params"
#define DRIVER_PATH "global_data/"
#define DRIVER_DIR "global_data"
#define WAFER_LINES "wafer_design/wafer.lines"
#define DRIVER.FILES "db_data/driver.files"
#define PC_FILE "db_data/pc_libs"
#define MOS6SERIAL_FILE "mos6.tbl"
#define MOS16SERIAL_FILE "mos16.tbl"
#define MOS6LOG_FILE "serial_tables/mos6.log"
#define MOS16LOG_FILE "serial_tables/mos16.log"
#define CONTRACT_FILE "db_data/contract_num"
#define AUDIT_FILE "db_data/audit_time"
#define PCNUM_FILE "db_data/pc_num"
                                                                                                                                                                                                                                                                   dir.h                                                                                               0000777 �    �Ԧ0120355 00000002263 12042342513 011354  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef dir_h
#define dir_h

int dir_archive_dlt(char *directory, char *mask);

/* Creates new directory */
/* If directory already present, deletes and re-creates */
void dir_create(char *dir);

void dir_make(char *dir);

int dir_delete(char *directory);

int dir_recursive_dlt(char *directory);

/* Remove specified directory */
/* If delete fails, client sent error message */
int dir_remove(char *directory);

void dir_rename(
    char *dir1,	/* Old directory */
    char *dir2);	/* New directory */

void dir_recursive_rename(
    char *dir1,	/* Old directory */
    char *dir2);	/* New directory */

/* Make list of files in directory except for those beginning with '.' */
int dir1list(char *directory, char *files[]);


/* Makes new directory and copies files from old directory to new directory */
/* Makes list of files in source directory */
/* and copies files to destination directory */
/* List of files copied returned to calling program */
/* The directory name passed in must not have an ending slash */
int dir_copy(char *new_dir, char *old_dir, char *files[]);

/* Make list of files in directory except for those beginning with '.' */
int dir_list(char *dir, char *files[]);

#endif
                                                                                                                                                                                                                                                                                                                                             disk_usage.h                                                                                        0000777 �    �Ԧ0120355 00000001325 12042342513 012712  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef disk_usage_h
#define disk_usage_h

#include <stdio.h>
struct DIR_INFO 
{
    long st_ino;
    long st_dev;
    struct DIR_INFO *next;
};

long long big_disk_usage(char *directory, int local_dir_cnt);
long long disk_usage(char *directory);
long long ldisk_usage(char *directory, struct DIR_INFO*dir_ptr);
long long big_ldisk_usage(char *directory, struct DIR_INFO*dir_ptr);

int dir_info_traverse(struct DIR_INFO *dir_ptr, long st_ino, long st_dev);

struct DIR_INFO *dir_info_add(struct DIR_INFO *dir_ptr, long st_ino, long st_dev);

/* Calculate directory size */
/* Also calculate estimated compressed size */
long long big_random_disk_usage(char *directory, int local_dir_cnt);

#endif /* Define for disk_usage */
                                                                                                                                                                                                                                                                                                           double_pin.h                                                                                        0000777 �    �Ԧ0120355 00000000714 12042342513 012715  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef mask_pin_h
#define mask_pin_h

#include "global_defines.h"

typedef struct
{
    char pin1[MAX_PIN_LEN+1];
    char pin2[MAX_PIN_LEN+1];
    char override_flag;
    float vnum;
    float sec_vnum;
    char version_flag;
    char sec_version_flag;
} DOUBLE_PIN;

extern DOUBLE_PIN tmp_pin;

#ifdef ANSI_C
void
mask_pin_set(
    char *pin1,
    char *pin2);

void
mask_pin_init(void);

#endif /* define for ANSI_C */
#endif /* Define for double_pin_h */
                                                    edc_defines.h                                                                                       0000777 �    �Ԧ0120355 00000003133 12321605654 013033  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef edc_defines_h
#define edc_defines_h

#define PENDING_APPROVAL_MSG \
"You have transferred data to one or more unapproved external customers.    External customers must be approved before they can retrieve data from Freescale.  Approvals are performed by your manager or a designated approver.\n\n\
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


static char reg_array[6][20] =
{
    "not registered",
    "registered",
    "approved",
    "registered_approved",
    "black listed",
    "not approved",
};

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                     edc_prototypes.h                                                                                    0000666 �    �Ԧ0120355 00000006342 12154412722 013644  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef edc_prototypes_h
#define edc_prototypes_h

void edc_sites_build(char *final_sites, char *new_sites, char *old_sites);

/* Builds name for EDC registry file */
void edc_name_build(char *file_name, char *user_id, int edc_type);

/* Builds a new entry when existing entry being modified */
void new_entry_build(char *query, char *final_entry, char *new_entry, char *old_entry);

/* See if the domain name is correct */
int edc_domain_lookup(char *entry, char *user_site, int save_email);

/* If entry not present, 3 things that can change are the status, expiration time, and sites */
int edc_entry_add(char *login, int status, unsigned long expirationtime, char *email1, char *email2, char *name, char *sites, int edc_type);

int edc_transweb_entry_add(char *login, int status, unsigned long expirationtime, char *email1, char *email2, char *name, char *sites, int edc_type);

int edc_transweb_entry_lookup(char *login, char *site, int edc_type);

int edc_entry_lookup(char *login, char *user_domain, int edc_type, int save_email);

/* Verify user is registered to use Transcend, Intercom, or TransWeb  */
int edc_reg_check(int edc_type);

int edc_reg_auth_user(char *email, unsigned long expirationtime, int new_status);

int edc_white_list_entry_add(
    char *domain,
    int status,
    unsigned long expirationtime);

/* Check the approved status of an external customer */
/* This function is called as the secure keywords are being generated */
int edc_auth_check(char *buffer, char *email_addr, char *keyword, int trusted_sender);

/* Verify the external customers are authorized to retrieve data */
int edc_pre_auth_check(char *email_addr[], int edc_type);

/* Approve specified user */
void edc_auth_user(char *email, char *req_type, char *expirationtime_string);

/* Authorize a domain */
void edc_auth_domain(char *email, char *req_type, char *expirationtime_string);

/* Retrieve pending authorization requests */
void edc_auth_rtv(void);

void edc_commandline_rtv(void);

/* Release keywords pending the approval */
void edc_keywords_release(char *email);

/* Process keyword for deletion block */
void edc_keyword_process(char *keyword, char *email);

/* Make needed log entries when approval is needed */
void edc_entries_log(char *keyword, char *email);

/* Perform user registration */
/* Register the user */
int edc_reg_user(
    char *login,
    char *domain,
    char *user_email1,	/* User's primary Email address */
    char *user_email2,	/* User's secondary Email address */
    char *user_name1,
    char *user_name2,
    char *user_name3,
    char *user_pwd,
    int edc_type);

int autoreg_user(int edc_type);

/* Submit an approval request */
void edc_commandline_approve(char *domain, char *justification, char *requestor, char *approver, time_t expiration_time);

/* This function is called by Transcend when a user uses Transcend to keep track of usage */
int edc_reg_timestamp_update(char *login, char *sites, int edc_type);

/* See if customer's domain is approved */
int domain_table_check(char *email_addr);

/* Request EDC approval */
void edc_request_approval(char *email);

int edc_status_calc(char *user_login, char *user_site, int edc_type, int save_email);

int edc_file_index_read(char *keyword);

int edc_registry_entries_count(char *path);

#endif
                                                                                                                                                                                                                                                                                              fabint_defines.h                                                                                    0000777 �    �Ԧ0120355 00000001035 12042342513 013532  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef fabint_defines_h
#define fabint_defines_h
#define FABINT_KEY "fab-integration"
#define FABINT_SOCK 6192
#define MAX_FABINT_SERVERS 4

/* Default Fabint machine names */
static char fabint_machine[MAX_SERVERS][MACHINE_LEN] =
{
    "maskdb1",
    "maskdb2",
    "maskdb3",
    "waccdb",
};

/* Holds IP addresses for default Fabint servers */
static char fabint_address[MAX_SERVERS][MACHINE_LEN] =
{
    "223.188.10.121",
    "223.13.249.31",
    "223.fabint_defines.h",
    "192.54.83.17",
};

#endif /* ifdef for fabint_defines_h */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   file.h                                                                                              0000777 �    �Ԧ0120355 00000000153 12042342513 011511  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef file_h
#define file_h

void file_lock(char *lock_file);
void file_unlock(char *lock_file);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                     files.h                                                                                             0000777 �    �Ԧ0120355 00000001077 12042342513 011702  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef files_h
#define files_h

#include "global_defines.h"

typedef struct
{
    char name[TOTAL_FILES][FILE_SIZE];
    char dir[FILE_SIZE];
    int num;
} DB_FILES;


int files_compress(
    DB_FILES *files,
    char *compress_string,
    char compress_flag);


int files_list(DB_FILES *files);

void files_copy(
    DB_FILES *files,
    char *old_dir);

void
files_dlt(DB_FILES *files);
void 
file_copy(
    char *old_file,
    char *new_file);

int 
file_rename(
    char *old_file,
    char *new_file);

void files_sort(char *files[]);

#endif /* Define for files_h */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                 gcrypt.h                                                                                            0000666 �    �Ԧ0120355 00000212773 12073102565 012121  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   /* gcrypt.h -  GNU Cryptographic Library Interface              -*- c -*-
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2006
                 2007, 2008, 2009, 2010  Free Software Foundation, Inc.
  
   This file is part of Libgcrypt.
  
   Libgcrypt is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.
  
   Libgcrypt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.
  
   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, see <http://www.gnu.org/licenses/>.

   File: src/gcrypt.h.  Generated from gcrypt.h.in by configure. */

#ifndef _GCRYPT_H
#define _GCRYPT_H

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <gpg-error.h>

#include <sys/types.h>

#if defined _WIN32 || defined __WIN32__
# include <winsock2.h>
# include <ws2tcpip.h>
# include <time.h>
# ifndef __GNUC__
  typedef long ssize_t;
  typedef int  pid_t;
# endif /*!__GNUC__*/
#else
# include <sys/socket.h>
# include <sys/time.h>
#endif /*!_WIN32*/

typedef socklen_t gcry_socklen_t;


/* This is required for error code compatibility. */
#define _GCRY_ERR_SOURCE_DEFAULT GPG_ERR_SOURCE_GCRYPT

#ifdef __cplusplus
extern "C" {
#if 0 /* (Keep Emacsens' auto-indent happy.) */
}
#endif
#endif

/* The version of this header should match the one of the library. It
   should not be used by a program because gcry_check_version() should
   return the same version.  The purpose of this macro is to let
   autoconf (using the AM_PATH_GCRYPT macro) check that this header
   matches the installed library.  */
#define GCRYPT_VERSION "1.4.6"

/* Internal: We can't use the convenience macros for the multi
   precision integer functions when building this library. */
#ifdef _GCRYPT_IN_LIBGCRYPT
#ifndef GCRYPT_NO_MPI_MACROS
#define GCRYPT_NO_MPI_MACROS 1
#endif
#endif

/* We want to use gcc attributes when possible.  Warning: Don't use
   these macros in your programs: As indicated by the leading
   underscore they are subject to change without notice. */
#ifdef __GNUC__

#define _GCRY_GCC_VERSION (__GNUC__ * 10000 \
                             + __GNUC_MINOR__ * 100 \
                             + __GNUC_PATCHLEVEL__)

/* While enum foo {FOO} __attribute__ ((__deprecated__)); is ok with
 * gcc-3.1.x and later, it doesn't work with g++-3.2.3, but seems to
 * be okay with g++-3.4.6 */
#ifndef __GNUG__
#if _GCRY_GCC_VERSION >= 30100
#define _GCRY_GCC_ATTR_DEPRECATED __attribute__ ((__deprecated__))
#endif
#else
#if _GCRY_GCC_VERSION >= 30400
#define _GCRY_GCC_ATTR_DEPRECATED __attribute__ ((__deprecated__))
#endif
#endif

#if _GCRY_GCC_VERSION >= 29600
#define _GCRY_GCC_ATTR_PURE  __attribute__ ((__pure__))
#endif

#if _GCRY_GCC_VERSION >= 30200
#define _GCRY_GCC_ATTR_MALLOC  __attribute__ ((__malloc__))
#endif

#endif /*__GNUC__*/

#ifndef _GCRY_GCC_ATTR_DEPRECATED
#define _GCRY_GCC_ATTR_DEPRECATED
#endif
#ifndef _GCRY_GCC_ATTR_PURE
#define _GCRY_GCC_ATTR_PURE
#endif
#ifndef _GCRY_GCC_ATTR_MALLOC
#define _GCRY_GCC_ATTR_MALLOC
#endif

/* Some members in a public type should only be used internally.
   There is no "internal" attribute, so we abuse the deprecated
   attribute to discourage external use.  */
#ifdef _GCRYPT_IN_LIBGCRYPT
#define _GCRY_ATTR_INTERNAL
#else
#define _GCRY_ATTR_INTERNAL	_GCRY_GCC_ATTR_DEPRECATED
#endif

/* Wrappers for the libgpg-error library.  */

typedef gpg_error_t gcry_error_t;
typedef gpg_err_code_t gcry_err_code_t;
typedef gpg_err_source_t gcry_err_source_t;

static GPG_ERR_INLINE gcry_error_t
gcry_err_make (gcry_err_source_t source, gcry_err_code_t code)
{
  return gpg_err_make (source, code);
}

/* The user can define GPG_ERR_SOURCE_DEFAULT before including this
   file to specify a default source for gpg_error.  */
#ifndef GCRY_ERR_SOURCE_DEFAULT
#define GCRY_ERR_SOURCE_DEFAULT  GPG_ERR_SOURCE_USER_1
#endif

static GPG_ERR_INLINE gcry_error_t
gcry_error (gcry_err_code_t code)
{
  return gcry_err_make (GCRY_ERR_SOURCE_DEFAULT, code);
}

static GPG_ERR_INLINE gcry_err_code_t
gcry_err_code (gcry_error_t err)
{
  return gpg_err_code (err);
}


static GPG_ERR_INLINE gcry_err_source_t
gcry_err_source (gcry_error_t err)
{
  return gpg_err_source (err);
}

/* Return a pointer to a string containing a description of the error
   code in the error value ERR.  */
const char *gcry_strerror (gcry_error_t err);

/* Return a pointer to a string containing a description of the error
   source in the error value ERR.  */
const char *gcry_strsource (gcry_error_t err);

/* Retrieve the error code for the system error ERR.  This returns
   GPG_ERR_UNKNOWN_ERRNO if the system error is not mapped (report
   this).  */
gcry_err_code_t gcry_err_code_from_errno (int err);

/* Retrieve the system error for the error code CODE.  This returns 0
   if CODE is not a system error code.  */
int gcry_err_code_to_errno (gcry_err_code_t code);

/* Return an error value with the error source SOURCE and the system
   error ERR.  */
gcry_error_t gcry_err_make_from_errno (gcry_err_source_t source, int err);

/* Return an error value with the system error ERR.  */
gcry_err_code_t gcry_error_from_errno (int err);


/* This enum is deprecated; it is only declared for the sake of
   complete API compatibility.  */
enum gcry_thread_option
  {
    _GCRY_THREAD_OPTION_DUMMY
  } _GCRY_GCC_ATTR_DEPRECATED;


/* Constants defining the thread model to use.  Used with the OPTION
   field of the struct gcry_thread_cbs.  */
#define GCRY_THREAD_OPTION_DEFAULT  0
#define GCRY_THREAD_OPTION_USER     1
#define GCRY_THREAD_OPTION_PTH      2
#define GCRY_THREAD_OPTION_PTHREAD  3

/* The version number encoded in the OPTION field of the struct
   gcry_thread_cbs.  */
#define GCRY_THREAD_OPTION_VERSION  0

/* Wrapper for struct ath_ops.  */
struct gcry_thread_cbs
{
  /* The OPTION field encodes the thread model and the version number
     of this structure.   
       Bits  7 - 0  are used for the thread model
       Bits 15 - 8  are used for the version number.
  */
  unsigned int option;

  int (*init) (void);
  int (*mutex_init) (void **priv);
  int (*mutex_destroy) (void **priv);
  int (*mutex_lock) (void **priv);
  int (*mutex_unlock) (void **priv);
  ssize_t (*read) (int fd, void *buf, size_t nbytes);
  ssize_t (*write) (int fd, const void *buf, size_t nbytes);
#ifdef _WIN32
  ssize_t (*select) (int nfd, void *rset, void *wset, void *eset,
		     struct timeval *timeout);
  ssize_t (*waitpid) (pid_t pid, int *status, int options);
  int (*accept) (int s, void  *addr, int *length_ptr);
  int (*connect) (int s, void *addr, gcry_socklen_t length);
  int (*sendmsg) (int s, const void *msg, int flags);
  int (*recvmsg) (int s, void *msg, int flags);
#else
  ssize_t (*select) (int nfd, fd_set *rset, fd_set *wset, fd_set *eset,
		     struct timeval *timeout);
  ssize_t (*waitpid) (pid_t pid, int *status, int options);
  int (*accept) (int s, struct sockaddr *addr, gcry_socklen_t *length_ptr);
  int (*connect) (int s, struct sockaddr *addr, gcry_socklen_t length);
  int (*sendmsg) (int s, const struct msghdr *msg, int flags);
  int (*recvmsg) (int s, struct msghdr *msg, int flags);
#endif
};

#ifdef _WIN32
# define _GCRY_THREAD_OPTION_PTH_IMPL_NET				      \
static ssize_t gcry_pth_select (int nfd, void *rset, void *wset,	      \
				void *eset, struct timeval *timeout)	      \
  { return pth_select (nfd, rset, wset, eset, timeout); }		      \
static ssize_t gcry_pth_waitpid (pid_t pid, int *status, int options)	      \
  { return pth_waitpid (pid, status, options); }			      \
static int gcry_pth_accept (int s, void *addr,                                \
			    gcry_socklen_t *length_ptr)			      \
  { return pth_accept (s, addr, length_ptr); }				      \
static int gcry_pth_connect (int s, void *addr,		                      \
			     gcry_socklen_t length)			      \
  { return pth_connect (s, addr, length); }
#else /*!_WIN32*/
# define _GCRY_THREAD_OPTION_PTH_IMPL_NET				      \
static ssize_t gcry_pth_select (int nfd, fd_set *rset, fd_set *wset,	      \
				fd_set *eset, struct timeval *timeout)	      \
  { return pth_select (nfd, rset, wset, eset, timeout); }		      \
static ssize_t gcry_pth_waitpid (pid_t pid, int *status, int options)	      \
  { return pth_waitpid (pid, status, options); }			      \
static int gcry_pth_accept (int s, struct sockaddr *addr,		      \
			    gcry_socklen_t *length_ptr)			      \
  { return pth_accept (s, addr, length_ptr); }				      \
static int gcry_pth_connect (int s, struct sockaddr *addr,		      \
			     gcry_socklen_t length)			      \
  { return pth_connect (s, addr, length); }
#endif /*!_WIN32*/



#define GCRY_THREAD_OPTION_PTH_IMPL					      \
static int gcry_pth_init (void)						      \
{ return (pth_init () == FALSE) ? errno : 0; }				      \
static int gcry_pth_mutex_init (void **priv)				      \
{									      \
  int err = 0;								      \
  pth_mutex_t *lock = malloc (sizeof (pth_mutex_t));			      \
									      \
  if (!lock)								      \
    err = ENOMEM;							      \
  if (!err)								      \
    {									      \
      err = pth_mutex_init (lock);					      \
      if (err == FALSE)							      \
	err = errno;							      \
      else								      \
	err = 0;							      \
      if (err)								      \
	free (lock);							      \
      else								      \
	*priv = lock;							      \
    }									      \
  return err;								      \
}									      \
static int gcry_pth_mutex_destroy (void **lock)				      \
  { /* GNU Pth has no destructor function.  */ free (*lock); return 0; }      \
static int gcry_pth_mutex_lock (void **lock)				      \
  { return ((pth_mutex_acquire (*lock, 0, NULL)) == FALSE)		      \
      ? errno : 0; }							      \
static int gcry_pth_mutex_unlock (void **lock)				      \
  { return ((pth_mutex_release (*lock)) == FALSE)			      \
      ? errno : 0; }							      \
static ssize_t gcry_pth_read (int fd, void *buf, size_t nbytes)		      \
  { return pth_read (fd, buf, nbytes); }				      \
static ssize_t gcry_pth_write (int fd, const void *buf, size_t nbytes)	      \
  { return pth_write (fd, buf, nbytes); }				      \
_GCRY_THREAD_OPTION_PTH_IMPL_NET                                              \
									      \
/* Note: GNU Pth is missing pth_sendmsg and pth_recvmsg.  */		      \
static struct gcry_thread_cbs gcry_threads_pth = {                            \
  (GCRY_THREAD_OPTION_PTH | (GCRY_THREAD_OPTION_VERSION << 8)),               \
  gcry_pth_init, gcry_pth_mutex_init, gcry_pth_mutex_destroy,		      \
  gcry_pth_mutex_lock, gcry_pth_mutex_unlock, gcry_pth_read, gcry_pth_write,  \
  gcry_pth_select, gcry_pth_waitpid, gcry_pth_accept, gcry_pth_connect,       \
  NULL, NULL }


#define GCRY_THREAD_OPTION_PTHREAD_IMPL					      \
static int gcry_pthread_mutex_init (void **priv)			      \
{									      \
  int err = 0;								      \
  pthread_mutex_t *lock = (pthread_mutex_t*)malloc (sizeof (pthread_mutex_t));\
									      \
  if (!lock)								      \
    err = ENOMEM;							      \
  if (!err)								      \
    {									      \
      err = pthread_mutex_init (lock, NULL);				      \
      if (err)								      \
	free (lock);							      \
      else								      \
	*priv = lock;							      \
    }									      \
  return err;								      \
}									      \
static int gcry_pthread_mutex_destroy (void **lock)			      \
  { int err = pthread_mutex_destroy ((pthread_mutex_t*)*lock);                \
    free (*lock); return err; }                                               \
static int gcry_pthread_mutex_lock (void **lock)			      \
  { return pthread_mutex_lock ((pthread_mutex_t*)*lock); }		      \
static int gcry_pthread_mutex_unlock (void **lock)			      \
  { return pthread_mutex_unlock ((pthread_mutex_t*)*lock); }		      \
									      \
static struct gcry_thread_cbs gcry_threads_pthread = {			      \
  (GCRY_THREAD_OPTION_PTHREAD | (GCRY_THREAD_OPTION_VERSION << 8)),           \
  NULL, gcry_pthread_mutex_init, gcry_pthread_mutex_destroy,		      \
  gcry_pthread_mutex_lock, gcry_pthread_mutex_unlock,                         \
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }


/* The data object used to hold a multi precision integer.  */
struct gcry_mpi;
typedef struct gcry_mpi *gcry_mpi_t;

#ifndef GCRYPT_NO_DEPRECATED
typedef struct gcry_mpi *GCRY_MPI _GCRY_GCC_ATTR_DEPRECATED;
typedef struct gcry_mpi *GcryMPI _GCRY_GCC_ATTR_DEPRECATED;
#endif



/* Check that the library fulfills the version requirement.  */
const char *gcry_check_version (const char *req_version);

/* Codes for function dispatchers.  */

/* Codes used with the gcry_control function. */
enum gcry_ctl_cmds 
  {
    GCRYCTL_SET_KEY  = 1,
    GCRYCTL_SET_IV   = 2,
    GCRYCTL_CFB_SYNC = 3,
    GCRYCTL_RESET    = 4,   /* e.g. for MDs */
    GCRYCTL_FINALIZE = 5,
    GCRYCTL_GET_KEYLEN = 6,
    GCRYCTL_GET_BLKLEN = 7,
    GCRYCTL_TEST_ALGO = 8,
    GCRYCTL_IS_SECURE = 9,
    GCRYCTL_GET_ASNOID = 10,
    GCRYCTL_ENABLE_ALGO = 11,
    GCRYCTL_DISABLE_ALGO = 12,
    GCRYCTL_DUMP_RANDOM_STATS = 13,
    GCRYCTL_DUMP_SECMEM_STATS = 14,
    GCRYCTL_GET_ALGO_NPKEY    = 15,
    GCRYCTL_GET_ALGO_NSKEY    = 16,
    GCRYCTL_GET_ALGO_NSIGN    = 17,
    GCRYCTL_GET_ALGO_NENCR    = 18,
    GCRYCTL_SET_VERBOSITY     = 19,
    GCRYCTL_SET_DEBUG_FLAGS   = 20,
    GCRYCTL_CLEAR_DEBUG_FLAGS = 21,
    GCRYCTL_USE_SECURE_RNDPOOL= 22,
    GCRYCTL_DUMP_MEMORY_STATS = 23,
    GCRYCTL_INIT_SECMEM       = 24,
    GCRYCTL_TERM_SECMEM       = 25,
    GCRYCTL_DISABLE_SECMEM_WARN = 27,
    GCRYCTL_SUSPEND_SECMEM_WARN = 28,
    GCRYCTL_RESUME_SECMEM_WARN  = 29,
    GCRYCTL_DROP_PRIVS          = 30,
    GCRYCTL_ENABLE_M_GUARD      = 31,
    GCRYCTL_START_DUMP          = 32,
    GCRYCTL_STOP_DUMP           = 33,
    GCRYCTL_GET_ALGO_USAGE      = 34,
    GCRYCTL_IS_ALGO_ENABLED     = 35,
    GCRYCTL_DISABLE_INTERNAL_LOCKING = 36,
    GCRYCTL_DISABLE_SECMEM      = 37,
    GCRYCTL_INITIALIZATION_FINISHED = 38,
    GCRYCTL_INITIALIZATION_FINISHED_P = 39,
    GCRYCTL_ANY_INITIALIZATION_P = 40,
    GCRYCTL_SET_CBC_CTS = 41,
    GCRYCTL_SET_CBC_MAC = 42,
    GCRYCTL_SET_CTR = 43,
    GCRYCTL_ENABLE_QUICK_RANDOM = 44,
    GCRYCTL_SET_RANDOM_SEED_FILE = 45,
    GCRYCTL_UPDATE_RANDOM_SEED_FILE = 46,
    GCRYCTL_SET_THREAD_CBS = 47,
    GCRYCTL_FAST_POLL = 48,
    GCRYCTL_SET_RANDOM_DAEMON_SOCKET = 49,
    GCRYCTL_USE_RANDOM_DAEMON = 50,
    GCRYCTL_FAKED_RANDOM_P = 51,
    GCRYCTL_SET_RNDEGD_SOCKET = 52,
    GCRYCTL_PRINT_CONFIG = 53,
    GCRYCTL_OPERATIONAL_P = 54,
    GCRYCTL_FIPS_MODE_P = 55,
    GCRYCTL_FORCE_FIPS_MODE = 56,
    GCRYCTL_SELFTEST = 57
    /* Note: 58 .. 62 are used internally.  */
  };

/* Perform various operations defined by CMD. */
gcry_error_t gcry_control (enum gcry_ctl_cmds CMD, ...);


/* S-expression management. */ 

/* The object to represent an S-expression as used with the public key
   functions.  */
struct gcry_sexp;
typedef struct gcry_sexp *gcry_sexp_t;

#ifndef GCRYPT_NO_DEPRECATED
typedef struct gcry_sexp *GCRY_SEXP _GCRY_GCC_ATTR_DEPRECATED;
typedef struct gcry_sexp *GcrySexp _GCRY_GCC_ATTR_DEPRECATED;
#endif

/* The possible values for the S-expression format. */
enum gcry_sexp_format
  {
    GCRYSEXP_FMT_DEFAULT   = 0,
    GCRYSEXP_FMT_CANON     = 1,
    GCRYSEXP_FMT_BASE64    = 2,
    GCRYSEXP_FMT_ADVANCED  = 3
  };

/* Create an new S-expression object from BUFFER of size LENGTH and
   return it in RETSEXP.  With AUTODETECT set to 0 the data in BUFFER
   is expected to be in canonized format.  */
gcry_error_t gcry_sexp_new (gcry_sexp_t *retsexp,
                            const void *buffer, size_t length,
                            int autodetect);

 /* Same as gcry_sexp_new but allows to pass a FREEFNC which has the
    effect to transfer ownership of BUFFER to the created object.  */
gcry_error_t gcry_sexp_create (gcry_sexp_t *retsexp,
                               void *buffer, size_t length,
                               int autodetect, void (*freefnc) (void *));

/* Scan BUFFER and return a new S-expression object in RETSEXP.  This
   function expects a printf like string in BUFFER.  */
gcry_error_t gcry_sexp_sscan (gcry_sexp_t *retsexp, size_t *erroff,
                              const char *buffer, size_t length);

/* Same as gcry_sexp_sscan but expects a string in FORMAT and can thus
   only be used for certain encodings.  */
gcry_error_t gcry_sexp_build (gcry_sexp_t *retsexp, size_t *erroff,
                              const char *format, ...);

/* Like gcry_sexp_build, but uses an array instead of variable
   function arguments.  */
gcry_error_t gcry_sexp_build_array (gcry_sexp_t *retsexp, size_t *erroff,
				    const char *format, void **arg_list);

/* Release the S-expression object SEXP */
void gcry_sexp_release (gcry_sexp_t sexp);

/* Calculate the length of an canonized S-expresion in BUFFER and
   check for a valid encoding. */
size_t gcry_sexp_canon_len (const unsigned char *buffer, size_t length, 
                            size_t *erroff, gcry_error_t *errcode);

/* Copies the S-expression object SEXP into BUFFER using the format
   specified in MODE.  */
size_t gcry_sexp_sprint (gcry_sexp_t sexp, int mode, void *buffer,
                         size_t maxlength);

/* Dumps the S-expression object A in a format suitable for debugging
   to Libgcrypt's logging stream.  */
void gcry_sexp_dump (const gcry_sexp_t a);

gcry_sexp_t gcry_sexp_cons (const gcry_sexp_t a, const gcry_sexp_t b);
gcry_sexp_t gcry_sexp_alist (const gcry_sexp_t *array);
gcry_sexp_t gcry_sexp_vlist (const gcry_sexp_t a, ...);
gcry_sexp_t gcry_sexp_append (const gcry_sexp_t a, const gcry_sexp_t n);
gcry_sexp_t gcry_sexp_prepend (const gcry_sexp_t a, const gcry_sexp_t n);

/* Scan the S-expression for a sublist with a type (the car of the
   list) matching the string TOKEN.  If TOKLEN is not 0, the token is
   assumed to be raw memory of this length.  The function returns a
   newly allocated S-expression consisting of the found sublist or
   `NULL' when not found.  */
gcry_sexp_t gcry_sexp_find_token (gcry_sexp_t list,
                                const char *tok, size_t toklen);
/* Return the length of the LIST.  For a valid S-expression this
   should be at least 1.  */
int gcry_sexp_length (const gcry_sexp_t list);

/* Create and return a new S-expression from the element with index
   NUMBER in LIST.  Note that the first element has the index 0.  If
   there is no such element, `NULL' is returned.  */
gcry_sexp_t gcry_sexp_nth (const gcry_sexp_t list, int number);

/* Create and return a new S-expression from the first element in
   LIST; this called the "type" and should always exist and be a
   string. `NULL' is returned in case of a problem.  */
gcry_sexp_t gcry_sexp_car (const gcry_sexp_t list);

/* Create and return a new list form all elements except for the first
   one.  Note, that this function may return an invalid S-expression
   because it is not guaranteed, that the type exists and is a string.
   However, for parsing a complex S-expression it might be useful for
   intermediate lists.  Returns `NULL' on error.  */
gcry_sexp_t gcry_sexp_cdr (const gcry_sexp_t list);

gcry_sexp_t gcry_sexp_cadr (const gcry_sexp_t list);


/* This function is used to get data from a LIST.  A pointer to the
   actual data with index NUMBER is returned and the length of this
   data will be stored to DATALEN.  If there is no data at the given
   index or the index represents another list, `NULL' is returned.
   *Note:* The returned pointer is valid as long as LIST is not
   modified or released.  */
const char *gcry_sexp_nth_data (const gcry_sexp_t list, int number,
                                size_t *datalen);

/* This function is used to get and convert data from a LIST.  The
   data is assumed to be a Nul terminated string.  The caller must
   release the returned value using `gcry_free'.  If there is no data
   at the given index, the index represents a list or the value can't
   be converted to a string, `NULL' is returned.  */
char *gcry_sexp_nth_string (gcry_sexp_t list, int number);

/* This function is used to get and convert data from a LIST. This
   data is assumed to be an MPI stored in the format described by
   MPIFMT and returned as a standard Libgcrypt MPI.  The caller must
   release this returned value using `gcry_mpi_release'.  If there is
   no data at the given index, the index represents a list or the
   value can't be converted to an MPI, `NULL' is returned.  */
gcry_mpi_t gcry_sexp_nth_mpi (gcry_sexp_t list, int number, int mpifmt);



/*******************************************
 *                                         *
 *  Multi Precision Integer Functions      *
 *                                         *
 *******************************************/

/* Different formats of external big integer representation. */
enum gcry_mpi_format 
  {
    GCRYMPI_FMT_NONE= 0,
    GCRYMPI_FMT_STD = 1,    /* Twos complement stored without length.  */
    GCRYMPI_FMT_PGP = 2,    /* As used by OpenPGP (unsigned only).  */
    GCRYMPI_FMT_SSH = 3,    /* As used by SSH (like STD but with length).  */
    GCRYMPI_FMT_HEX = 4,    /* Hex format. */
    GCRYMPI_FMT_USG = 5     /* Like STD but unsigned. */
  };

/* Flags used for creating big integers.  */
enum gcry_mpi_flag 
  {
    GCRYMPI_FLAG_SECURE = 1,  /* Allocate the number in "secure" memory.  */
    GCRYMPI_FLAG_OPAQUE = 2   /* The number is not a real one but just
                                 a way to store some bytes.  This is
                                 useful for encrypted big integers.  */
  };


/* Allocate a new big integer object, initialize it with 0 and
   initially allocate memory for a number of at least NBITS. */
gcry_mpi_t gcry_mpi_new (unsigned int nbits);

/* Same as gcry_mpi_new() but allocate in "secure" memory. */
gcry_mpi_t gcry_mpi_snew (unsigned int nbits);

/* Release the number A and free all associated resources. */
void gcry_mpi_release (gcry_mpi_t a);

/* Create a new number with the same value as A. */
gcry_mpi_t gcry_mpi_copy (const gcry_mpi_t a);

/* Store the big integer value U in W. */
gcry_mpi_t gcry_mpi_set (gcry_mpi_t w, const gcry_mpi_t u);

/* Store the unsigned integer value U in W. */
gcry_mpi_t gcry_mpi_set_ui (gcry_mpi_t w, unsigned long u);

/* Swap the values of A and B. */
void gcry_mpi_swap (gcry_mpi_t a, gcry_mpi_t b);

/* Compare the big integer number U and V returning 0 for equality, a
   positive value for U > V and a negative for U < V. */
int gcry_mpi_cmp (const gcry_mpi_t u, const gcry_mpi_t v);

/* Compare the big integer number U with the unsigned integer V
   returning 0 for equality, a positive value for U > V and a negative
   for U < V. */
int gcry_mpi_cmp_ui (const gcry_mpi_t u, unsigned long v);

/* Convert the external representation of an integer stored in BUFFER
   with a length of BUFLEN into a newly create MPI returned in
   RET_MPI.  If NSCANNED is not NULL, it will receive the number of
   bytes actually scanned after a successful operation. */
gcry_error_t gcry_mpi_scan (gcry_mpi_t *ret_mpi, enum gcry_mpi_format format,
                            const void *buffer, size_t buflen, 
                            size_t *nscanned);

/* Convert the big integer A into the external representation
   described by FORMAT and store it in the provided BUFFER which has
   been allocated by the user with a size of BUFLEN bytes.  NWRITTEN
   receives the actual length of the external representation unless it
   has been passed as NULL. */
gcry_error_t gcry_mpi_print (enum gcry_mpi_format format,
                             unsigned char *buffer, size_t buflen,
                             size_t *nwritten,
                             const gcry_mpi_t a);

/* Convert the big integer A int the external representation described
   by FORMAT and store it in a newly allocated buffer which address
   will be put into BUFFER.  NWRITTEN receives the actual lengths of the
   external representation. */
gcry_error_t gcry_mpi_aprint (enum gcry_mpi_format format,
                              unsigned char **buffer, size_t *nwritten,
                              const gcry_mpi_t a);

/* Dump the value of A in a format suitable for debugging to
   Libgcrypt's logging stream.  Note that one leading space but no
   trailing space or linefeed will be printed.  It is okay to pass
   NULL for A. */
void gcry_mpi_dump (const gcry_mpi_t a);


/* W = U + V.  */
void gcry_mpi_add (gcry_mpi_t w, gcry_mpi_t u, gcry_mpi_t v);

/* W = U + V.  V is an unsigned integer. */
void gcry_mpi_add_ui (gcry_mpi_t w, gcry_mpi_t u, unsigned long v);

/* W = U + V mod M. */
void gcry_mpi_addm (gcry_mpi_t w, gcry_mpi_t u, gcry_mpi_t v, gcry_mpi_t m);

/* W = U - V. */
void gcry_mpi_sub (gcry_mpi_t w, gcry_mpi_t u, gcry_mpi_t v);

/* W = U - V.  V is an unsigned integer. */
void gcry_mpi_sub_ui (gcry_mpi_t w, gcry_mpi_t u, unsigned long v );

/* W = U - V mod M */
void gcry_mpi_subm (gcry_mpi_t w, gcry_mpi_t u, gcry_mpi_t v, gcry_mpi_t m);

/* W = U * V. */
void gcry_mpi_mul (gcry_mpi_t w, gcry_mpi_t u, gcry_mpi_t v);

/* W = U * V.  V is an unsigned integer. */
void gcry_mpi_mul_ui (gcry_mpi_t w, gcry_mpi_t u, unsigned long v );

/* W = U * V mod M. */
void gcry_mpi_mulm (gcry_mpi_t w, gcry_mpi_t u, gcry_mpi_t v, gcry_mpi_t m);

/* W = U * (2 ^ CNT). */
void gcry_mpi_mul_2exp (gcry_mpi_t w, gcry_mpi_t u, unsigned long cnt);

/* Q = DIVIDEND / DIVISOR, R = DIVIDEND % DIVISOR,
   Q or R may be passed as NULL.  ROUND should be negative or 0. */
void gcry_mpi_div (gcry_mpi_t q, gcry_mpi_t r,
                   gcry_mpi_t dividend, gcry_mpi_t divisor, int round);

/* R = DIVIDEND % DIVISOR */
void gcry_mpi_mod (gcry_mpi_t r, gcry_mpi_t dividend, gcry_mpi_t divisor);

/* W = B ^ E mod M. */
void gcry_mpi_powm (gcry_mpi_t w,
                    const gcry_mpi_t b, const gcry_mpi_t e,
                    const gcry_mpi_t m);

/* Set G to the greatest common divisor of A and B.  
   Return true if the G is 1. */
int gcry_mpi_gcd (gcry_mpi_t g, gcry_mpi_t a, gcry_mpi_t b);

/* Set X to the multiplicative inverse of A mod M.
   Return true if the value exists. */
int gcry_mpi_invm (gcry_mpi_t x, gcry_mpi_t a, gcry_mpi_t m);


/* Return the number of bits required to represent A. */
unsigned int gcry_mpi_get_nbits (gcry_mpi_t a);

/* Return true when bit number N (counting from 0) is set in A. */
int      gcry_mpi_test_bit (gcry_mpi_t a, unsigned int n);

/* Set bit number N in A. */
void     gcry_mpi_set_bit (gcry_mpi_t a, unsigned int n);

/* Clear bit number N in A. */
void     gcry_mpi_clear_bit (gcry_mpi_t a, unsigned int n);

/* Set bit number N in A and clear all bits greater than N. */
void     gcry_mpi_set_highbit (gcry_mpi_t a, unsigned int n);

/* Clear bit number N in A and all bits greater than N. */
void     gcry_mpi_clear_highbit (gcry_mpi_t a, unsigned int n);

/* Shift the value of A by N bits to the right and store the result in X. */
void     gcry_mpi_rshift (gcry_mpi_t x, gcry_mpi_t a, unsigned int n);

/* Shift the value of A by N bits to the left and store the result in X. */
void     gcry_mpi_lshift (gcry_mpi_t x, gcry_mpi_t a, unsigned int n);

/* Store NBITS of the value P points to in A and mark A as an opaque
   value.  WARNING: Never use an opaque MPI for anything thing else then 
   gcry_mpi_release, gcry_mpi_get_opaque. */
gcry_mpi_t gcry_mpi_set_opaque (gcry_mpi_t a, void *p, unsigned int nbits);

/* Return a pointer to an opaque value stored in A and return its size
   in NBITS.  Note that the returned pointer is still owned by A and
   that the function should never be used for an non-opaque MPI. */
void *gcry_mpi_get_opaque (gcry_mpi_t a, unsigned int *nbits);

/* Set the FLAG for the big integer A.  Currently only the flag
   GCRYMPI_FLAG_SECURE is allowed to convert A into an big intger
   stored in "secure" memory. */
void gcry_mpi_set_flag (gcry_mpi_t a, enum gcry_mpi_flag flag);

/* Clear FLAG for the big integer A.  Note that this function is
   currently useless as no flags are allowed. */
void gcry_mpi_clear_flag (gcry_mpi_t a, enum gcry_mpi_flag flag);

/* Return true when the FLAG is set for A. */
int gcry_mpi_get_flag (gcry_mpi_t a, enum gcry_mpi_flag flag);

/* Unless the GCRYPT_NO_MPI_MACROS is used, provide a couple of
   convenience macros for the big integer functions. */
#ifndef GCRYPT_NO_MPI_MACROS
#define mpi_new(n)          gcry_mpi_new( (n) )
#define mpi_secure_new( n ) gcry_mpi_snew( (n) )
#define mpi_release(a)      \
  do \
    { \
      gcry_mpi_release ((a)); \
      (a) = NULL; \
    } \
  while (0)

#define mpi_copy( a )          gcry_mpi_copy( (a) )
#define mpi_set( w, u)         gcry_mpi_set( (w), (u) )
#define mpi_set_ui( w, u)      gcry_mpi_set_ui( (w), (u) )
#define mpi_cmp( u, v )        gcry_mpi_cmp( (u), (v) )
#define mpi_cmp_ui( u, v )     gcry_mpi_cmp_ui( (u), (v) )
                              
#define mpi_add_ui(w,u,v)      gcry_mpi_add_ui((w),(u),(v))
#define mpi_add(w,u,v)         gcry_mpi_add ((w),(u),(v))
#define mpi_addm(w,u,v,m)      gcry_mpi_addm ((w),(u),(v),(m))
#define mpi_sub_ui(w,u,v)      gcry_mpi_sub_ui ((w),(u),(v))
#define mpi_sub(w,u,v)         gcry_mpi_sub ((w),(u),(v))
#define mpi_subm(w,u,v,m)      gcry_mpi_subm ((w),(u),(v),(m))
#define mpi_mul_ui(w,u,v)      gcry_mpi_mul_ui ((w),(u),(v))
#define mpi_mul_2exp(w,u,v)    gcry_mpi_mul_2exp ((w),(u),(v))
#define mpi_mul(w,u,v)         gcry_mpi_mul ((w),(u),(v))
#define mpi_mulm(w,u,v,m)      gcry_mpi_mulm ((w),(u),(v),(m))
#define mpi_powm(w,b,e,m)      gcry_mpi_powm ( (w), (b), (e), (m) )
#define mpi_tdiv(q,r,a,m)      gcry_mpi_div ( (q), (r), (a), (m), 0)
#define mpi_fdiv(q,r,a,m)      gcry_mpi_div ( (q), (r), (a), (m), -1)
#define mpi_mod(r,a,m)         gcry_mpi_mod ((r), (a), (m))
#define mpi_gcd(g,a,b)         gcry_mpi_gcd ( (g), (a), (b) )
#define mpi_invm(g,a,b)        gcry_mpi_invm ( (g), (a), (b) )

#define mpi_get_nbits(a)       gcry_mpi_get_nbits ((a))
#define mpi_test_bit(a,b)      gcry_mpi_test_bit ((a),(b))
#define mpi_set_bit(a,b)       gcry_mpi_set_bit ((a),(b))
#define mpi_set_highbit(a,b)   gcry_mpi_set_highbit ((a),(b))
#define mpi_clear_bit(a,b)     gcry_mpi_clear_bit ((a),(b))
#define mpi_clear_highbit(a,b) gcry_mpi_clear_highbit ((a),(b))
#define mpi_rshift(a,b,c)      gcry_mpi_rshift ((a),(b),(c))
#define mpi_lshift(a,b,c)      gcry_mpi_lshift ((a),(b),(c))

#define mpi_set_opaque(a,b,c)  gcry_mpi_set_opaque( (a), (b), (c) )
#define mpi_get_opaque(a,b)    gcry_mpi_get_opaque( (a), (b) )
#endif /* GCRYPT_NO_MPI_MACROS */



/************************************
 *                                  *
 *   Symmetric Cipher Functions     *
 *                                  *
 ************************************/

/* The data object used to hold a handle to an encryption object.  */
struct gcry_cipher_handle;
typedef struct gcry_cipher_handle *gcry_cipher_hd_t;

#ifndef GCRYPT_NO_DEPRECATED
typedef struct gcry_cipher_handle *GCRY_CIPHER_HD _GCRY_GCC_ATTR_DEPRECATED;
typedef struct gcry_cipher_handle *GcryCipherHd _GCRY_GCC_ATTR_DEPRECATED;
#endif

/* All symmetric encryption algorithms are identified by their IDs.
   More IDs may be registered at runtime. */
enum gcry_cipher_algos
  {
    GCRY_CIPHER_NONE        = 0,
    GCRY_CIPHER_IDEA        = 1,
    GCRY_CIPHER_3DES        = 2,
    GCRY_CIPHER_CAST5       = 3,
    GCRY_CIPHER_BLOWFISH    = 4,
    GCRY_CIPHER_SAFER_SK128 = 5,
    GCRY_CIPHER_DES_SK      = 6,
    GCRY_CIPHER_AES         = 7,
    GCRY_CIPHER_AES192      = 8,
    GCRY_CIPHER_AES256      = 9,
    GCRY_CIPHER_TWOFISH     = 10,

    /* Other cipher numbers are above 300 for OpenPGP reasons. */
    GCRY_CIPHER_ARCFOUR     = 301,  /* Fully compatible with RSA's RC4 (tm). */
    GCRY_CIPHER_DES         = 302,  /* Yes, this is single key 56 bit DES. */
    GCRY_CIPHER_TWOFISH128  = 303,
    GCRY_CIPHER_SERPENT128  = 304,
    GCRY_CIPHER_SERPENT192  = 305,
    GCRY_CIPHER_SERPENT256  = 306,
    GCRY_CIPHER_RFC2268_40  = 307,  /* Ron's Cipher 2 (40 bit). */
    GCRY_CIPHER_RFC2268_128 = 308,  /* Ron's Cipher 2 (128 bit). */
    GCRY_CIPHER_SEED        = 309,  /* 128 bit cipher described in RFC4269. */
    GCRY_CIPHER_CAMELLIA128 = 310,
    GCRY_CIPHER_CAMELLIA192 = 311,
    GCRY_CIPHER_CAMELLIA256 = 312
  };

/* The Rijndael algorithm is basically AES, so provide some macros. */
#define GCRY_CIPHER_AES128      GCRY_CIPHER_AES    
#define GCRY_CIPHER_RIJNDAEL    GCRY_CIPHER_AES    
#define GCRY_CIPHER_RIJNDAEL128 GCRY_CIPHER_AES128 
#define GCRY_CIPHER_RIJNDAEL192 GCRY_CIPHER_AES192 
#define GCRY_CIPHER_RIJNDAEL256 GCRY_CIPHER_AES256 

/* The supported encryption modes.  Note that not all of them are
   supported for each algorithm. */
enum gcry_cipher_modes 
  {
    GCRY_CIPHER_MODE_NONE   = 0,  /* Not yet specified. */
    GCRY_CIPHER_MODE_ECB    = 1,  /* Electronic codebook. */
    GCRY_CIPHER_MODE_CFB    = 2,  /* Cipher feedback. */
    GCRY_CIPHER_MODE_CBC    = 3,  /* Cipher block chaining. */
    GCRY_CIPHER_MODE_STREAM = 4,  /* Used with stream ciphers. */
    GCRY_CIPHER_MODE_OFB    = 5,  /* Outer feedback. */
    GCRY_CIPHER_MODE_CTR    = 6,  /* Counter. */
    GCRY_CIPHER_MODE_AESWRAP= 7   /* AES-WRAP algorithm.  */
  };

/* Flags used with the open function. */ 
enum gcry_cipher_flags
  {
    GCRY_CIPHER_SECURE      = 1,  /* Allocate in secure memory. */
    GCRY_CIPHER_ENABLE_SYNC = 2,  /* Enable CFB sync mode. */
    GCRY_CIPHER_CBC_CTS     = 4,  /* Enable CBC cipher text stealing (CTS). */
    GCRY_CIPHER_CBC_MAC     = 8   /* Enable CBC message auth. code (MAC). */
  };


/* Create a handle for algorithm ALGO to be used in MODE.  FLAGS may
   be given as an bitwise OR of the gcry_cipher_flags values. */
gcry_error_t gcry_cipher_open (gcry_cipher_hd_t *handle,
                              int algo, int mode, unsigned int flags);

/* Close the cioher handle H and release all resource. */
void gcry_cipher_close (gcry_cipher_hd_t h);

/* Perform various operations on the cipher object H. */
gcry_error_t gcry_cipher_ctl (gcry_cipher_hd_t h, int cmd, void *buffer,
                             size_t buflen);

/* Retrieve various information about the cipher object H. */
gcry_error_t gcry_cipher_info (gcry_cipher_hd_t h, int what, void *buffer,
                              size_t *nbytes);

/* Retrieve various information about the cipher algorithm ALGO. */
gcry_error_t gcry_cipher_algo_info (int algo, int what, void *buffer,
                                   size_t *nbytes);

/* Map the cipher algorithm whose ID is contained in ALGORITHM to a
   string representation of the algorithm name.  For unknown algorithm
   IDs this function returns "?".  */
const char *gcry_cipher_algo_name (int algorithm) _GCRY_GCC_ATTR_PURE;

/* Map the algorithm name NAME to an cipher algorithm ID.  Return 0 if
   the algorithm name is not known. */
int gcry_cipher_map_name (const char *name) _GCRY_GCC_ATTR_PURE;

/* Given an ASN.1 object identifier in standard IETF dotted decimal
   format in STRING, return the encryption mode associated with that
   OID or 0 if not known or applicable. */
int gcry_cipher_mode_from_oid (const char *string) _GCRY_GCC_ATTR_PURE;

/* Encrypt the plaintext of size INLEN in IN using the cipher handle H
   into the buffer OUT which has an allocated length of OUTSIZE.  For
   most algorithms it is possible to pass NULL for in and 0 for INLEN
   and do a in-place decryption of the data provided in OUT.  */
gcry_error_t gcry_cipher_encrypt (gcry_cipher_hd_t h,
                                  void *out, size_t outsize,
                                  const void *in, size_t inlen);

/* The counterpart to gcry_cipher_encrypt.  */
gcry_error_t gcry_cipher_decrypt (gcry_cipher_hd_t h,
                                  void *out, size_t outsize,
                                  const void *in, size_t inlen);

/* Set KEY of length KEYLEN bytes for the cipher handle HD.  */
gcry_error_t gcry_cipher_setkey (gcry_cipher_hd_t hd,
                                 const void *key, size_t keylen);


/* Set initialization vector IV of length IVLEN for the cipher handle HD. */
gcry_error_t gcry_cipher_setiv (gcry_cipher_hd_t hd,
                                const void *iv, size_t ivlen);


/* Reset the handle to the state after open.  */
#define gcry_cipher_reset(h)  gcry_cipher_ctl ((h), GCRYCTL_RESET, NULL, 0)

/* Perform the OpenPGP sync operation if this is enabled for the
   cipher handle H. */
#define gcry_cipher_sync(h)  gcry_cipher_ctl( (h), GCRYCTL_CFB_SYNC, NULL, 0)

/* Enable or disable CTS in future calls to gcry_encrypt(). CBC mode only. */
#define gcry_cipher_cts(h,on)  gcry_cipher_ctl( (h), GCRYCTL_SET_CBC_CTS, \
                                                                   NULL, on )

/* Set counter for CTR mode.  (CTR,CTRLEN) must denote a buffer of
   block size length, or (NULL,0) to set the CTR to the all-zero block. */
gpg_error_t gcry_cipher_setctr (gcry_cipher_hd_t hd,
                                const void *ctr, size_t ctrlen);

/* Retrieved the key length in bytes used with algorithm A. */
size_t gcry_cipher_get_algo_keylen (int algo);

/* Retrieve the block length in bytes used with algorithm A. */
size_t gcry_cipher_get_algo_blklen (int algo);

/* Return 0 if the algorithm A is available for use. */
#define gcry_cipher_test_algo(a) \
            gcry_cipher_algo_info( (a), GCRYCTL_TEST_ALGO, NULL, NULL )

/* Get a list consisting of the IDs of the loaded cipher modules.  If
   LIST is zero, write the number of loaded cipher modules to
   LIST_LENGTH and return.  If LIST is non-zero, the first
   *LIST_LENGTH algorithm IDs are stored in LIST, which must be of
   according size.  In case there are less cipher modules than
   *LIST_LENGTH, *LIST_LENGTH is updated to the correct number.  */
gcry_error_t gcry_cipher_list (int *list, int *list_length);


/************************************
 *                                  *
 *    Asymmetric Cipher Functions   *
 *                                  *
 ************************************/

/* The algorithms and their IDs we support. */
enum gcry_pk_algos 
  {
    GCRY_PK_RSA   = 1,
    GCRY_PK_RSA_E = 2,      /* (deprecated) */
    GCRY_PK_RSA_S = 3,      /* (deprecated) */
    GCRY_PK_ELG_E = 16,
    GCRY_PK_DSA   = 17,
    GCRY_PK_ELG   = 20,
    GCRY_PK_ECDSA = 301
  };

/* Flags describing usage capabilities of a PK algorithm. */
#define GCRY_PK_USAGE_SIGN 1   /* Good for signatures. */            
#define GCRY_PK_USAGE_ENCR 2   /* Good for encryption. */            
#define GCRY_PK_USAGE_CERT 4   /* Good to certify other keys. */
#define GCRY_PK_USAGE_AUTH 8   /* Good for authentication. */        
#define GCRY_PK_USAGE_UNKN 128 /* Unknown usage flag. */          

/* Encrypt the DATA using the public key PKEY and store the result as
   a newly created S-expression at RESULT. */
gcry_error_t gcry_pk_encrypt (gcry_sexp_t *result,
                              gcry_sexp_t data, gcry_sexp_t pkey);

/* Decrypt the DATA using the private key SKEY and store the result as
   a newly created S-expression at RESULT. */
gcry_error_t gcry_pk_decrypt (gcry_sexp_t *result,
                              gcry_sexp_t data, gcry_sexp_t skey);

/* Sign the DATA using the private key SKEY and store the result as
   a newly created S-expression at RESULT. */
gcry_error_t gcry_pk_sign (gcry_sexp_t *result,
                           gcry_sexp_t data, gcry_sexp_t skey);

/* Check the signature SIGVAL on DATA using the public key PKEY. */
gcry_error_t gcry_pk_verify (gcry_sexp_t sigval,
                             gcry_sexp_t data, gcry_sexp_t pkey);

/* Check that private KEY is sane. */
gcry_error_t gcry_pk_testkey (gcry_sexp_t key);

/* Generate a new key pair according to the parameters given in
   S_PARMS.  The new key pair is returned in as an S-expression in
   R_KEY. */
gcry_error_t gcry_pk_genkey (gcry_sexp_t *r_key, gcry_sexp_t s_parms);

/* Catch all function for miscellaneous operations. */
gcry_error_t gcry_pk_ctl (int cmd, void *buffer, size_t buflen);

/* Retrieve information about the public key algorithm ALGO. */
gcry_error_t gcry_pk_algo_info (int algo, int what,
                                void *buffer, size_t *nbytes);

/* Map the public key algorithm whose ID is contained in ALGORITHM to
   a string representation of the algorithm name.  For unknown
   algorithm IDs this functions returns "?". */
const char *gcry_pk_algo_name (int algorithm) _GCRY_GCC_ATTR_PURE;

/* Map the algorithm NAME to a public key algorithm Id.  Return 0 if
   the algorithm name is not known. */
int gcry_pk_map_name (const char* name) _GCRY_GCC_ATTR_PURE;

/* Return what is commonly referred as the key length for the given
   public or private KEY.  */
unsigned int gcry_pk_get_nbits (gcry_sexp_t key) _GCRY_GCC_ATTR_PURE;

/* Please note that keygrip is still experimental and should not be
   used without contacting the author. */
unsigned char *gcry_pk_get_keygrip (gcry_sexp_t key, unsigned char *array);

/* Return 0 if the public key algorithm A is available for use. */
#define gcry_pk_test_algo(a) \
            gcry_pk_algo_info( (a), GCRYCTL_TEST_ALGO, NULL, NULL )

/* Get a list consisting of the IDs of the loaded pubkey modules.  If
   LIST is zero, write the number of loaded pubkey modules to
   LIST_LENGTH and return.  If LIST is non-zero, the first
   *LIST_LENGTH algorithm IDs are stored in LIST, which must be of
   according size.  In case there are less pubkey modules than
   *LIST_LENGTH, *LIST_LENGTH is updated to the correct number.  */
gcry_error_t gcry_pk_list (int *list, int *list_length);



/************************************
 *                                  *
 *   Cryptograhic Hash Functions    *
 *                                  *
 ************************************/

/* Algorithm IDs for the hash functions we know about. Not all of them
   are implemnted. */
enum gcry_md_algos
  {
    GCRY_MD_NONE    = 0,  
    GCRY_MD_MD5     = 1,
    GCRY_MD_SHA1    = 2,
    GCRY_MD_RMD160  = 3,
    GCRY_MD_MD2     = 5,
    GCRY_MD_TIGER   = 6,   /* TIGER/192 as used by GnuPG <= 1.3.2. */
    GCRY_MD_HAVAL   = 7,   /* HAVAL, 5 pass, 160 bit. */
    GCRY_MD_SHA256  = 8,
    GCRY_MD_SHA384  = 9,
    GCRY_MD_SHA512  = 10,
    GCRY_MD_SHA224  = 11,
    GCRY_MD_MD4     = 301,
    GCRY_MD_CRC32         = 302,
    GCRY_MD_CRC32_RFC1510 = 303,
    GCRY_MD_CRC24_RFC2440 = 304,
    GCRY_MD_WHIRLPOOL = 305,
    GCRY_MD_TIGER1  = 306, /* TIGER (fixed).  */
    GCRY_MD_TIGER2  = 307  /* TIGER2 variant.   */
  };

/* Flags used with the open function.  */
enum gcry_md_flags
  {
    GCRY_MD_FLAG_SECURE = 1,  /* Allocate all buffers in "secure" memory.  */
    GCRY_MD_FLAG_HMAC   = 2   /* Make an HMAC out of this algorithm.  */
  };

/* (Forward declaration.)  */
struct gcry_md_context;

/* This object is used to hold a handle to a message digest object.
   This structure is private - only to be used by the public gcry_md_*
   macros.  */
typedef struct gcry_md_handle 
{
  /* Actual context.  */
  struct gcry_md_context *ctx;
  
  /* Buffer management.  */
  int  bufpos;
  int  bufsize;
  unsigned char buf[1];
} *gcry_md_hd_t;

/* Compatibility types, do not use them.  */
#ifndef GCRYPT_NO_DEPRECATED
typedef struct gcry_md_handle *GCRY_MD_HD _GCRY_GCC_ATTR_DEPRECATED;
typedef struct gcry_md_handle *GcryMDHd _GCRY_GCC_ATTR_DEPRECATED;
#endif

/* Create a message digest object for algorithm ALGO.  FLAGS may be
   given as an bitwise OR of the gcry_md_flags values.  ALGO may be
   given as 0 if the algorithms to be used are later set using
   gcry_md_enable.  */
gcry_error_t gcry_md_open (gcry_md_hd_t *h, int algo, unsigned int flags);

/* Release the message digest object HD.  */
void gcry_md_close (gcry_md_hd_t hd);

/* Add the message digest algorithm ALGO to the digest object HD.  */
gcry_error_t gcry_md_enable (gcry_md_hd_t hd, int algo);

/* Create a new digest object as an exact copy of the object HD.  */
gcry_error_t gcry_md_copy (gcry_md_hd_t *bhd, gcry_md_hd_t ahd);

/* Reset the digest object HD to its initial state.  */
void gcry_md_reset (gcry_md_hd_t hd);

/* Perform various operations on the digest object HD. */
gcry_error_t gcry_md_ctl (gcry_md_hd_t hd, int cmd,
                          void *buffer, size_t buflen);

/* Pass LENGTH bytes of data in BUFFER to the digest object HD so that
   it can update the digest values.  This is the actual hash
   function. */
void gcry_md_write (gcry_md_hd_t hd, const void *buffer, size_t length);

/* Read out the final digest from HD return the digest value for
   algorithm ALGO. */
unsigned char *gcry_md_read (gcry_md_hd_t hd, int algo);

/* Convenience function to calculate the hash from the data in BUFFER
   of size LENGTH using the algorithm ALGO avoiding the creating of a
   hash object.  The hash is returned in the caller provided buffer
   DIGEST which must be large enough to hold the digest of the given
   algorithm. */
void gcry_md_hash_buffer (int algo, void *digest,
                          const void *buffer, size_t length);

/* Retrieve the algorithm used with HD.  This does not work reliable
   if more than one algorithm is enabled in HD. */
int gcry_md_get_algo (gcry_md_hd_t hd);

/* Retrieve the length in bytes of the digest yielded by algorithm
   ALGO. */
unsigned int gcry_md_get_algo_dlen (int algo);

/* Return true if the the algorithm ALGO is enabled in the digest
   object A. */
int gcry_md_is_enabled (gcry_md_hd_t a, int algo);

/* Return true if the digest object A is allocated in "secure" memory. */
int gcry_md_is_secure (gcry_md_hd_t a);

/* Retrieve various information about the object H.  */
gcry_error_t gcry_md_info (gcry_md_hd_t h, int what, void *buffer,
                          size_t *nbytes);

/* Retrieve various information about the algorithm ALGO.  */
gcry_error_t gcry_md_algo_info (int algo, int what, void *buffer,
                               size_t *nbytes);

/* Map the digest algorithm id ALGO to a string representation of the
   algorithm name.  For unknown algorithms this function returns
   "?". */
const char *gcry_md_algo_name (int algo) _GCRY_GCC_ATTR_PURE;

/* Map the algorithm NAME to a digest algorithm Id.  Return 0 if
   the algorithm name is not known. */
int gcry_md_map_name (const char* name) _GCRY_GCC_ATTR_PURE;

/* For use with the HMAC feature, the set MAC key to the KEY of
   KEYLEN bytes. */
gcry_error_t gcry_md_setkey (gcry_md_hd_t hd, const void *key, size_t keylen);

/* Start or stop debugging for digest handle HD; i.e. create a file
   named dbgmd-<n>.<suffix> while hashing.  If SUFFIX is NULL,
   debugging stops and the file will be closed. */
void gcry_md_debug (gcry_md_hd_t hd, const char *suffix);


/* Update the hash(s) of H with the character C.  This is a buffered
   version of the gcry_md_write function. */
#define gcry_md_putc(h,c)  \
            do {                                          \
                gcry_md_hd_t h__ = (h);                   \
                if( (h__)->bufpos == (h__)->bufsize )     \
                    gcry_md_write( (h__), NULL, 0 );      \
                (h__)->buf[(h__)->bufpos++] = (c) & 0xff; \
            } while(0)

/* Finalize the digest calculation.  This is not really needed because
   gcry_md_read() does this implicitly. */
#define gcry_md_final(a) \
            gcry_md_ctl ((a), GCRYCTL_FINALIZE, NULL, 0)

/* Return 0 if the algorithm A is available for use. */
#define gcry_md_test_algo(a) \
            gcry_md_algo_info( (a), GCRYCTL_TEST_ALGO, NULL, NULL )

/* Return an DER encoded ASN.1 OID for the algorithm A in buffer B. N
   must point to size_t variable with the available size of buffer B.
   After return it will receive the actual size of the returned
   OID. */
#define gcry_md_get_asnoid(a,b,n) \
            gcry_md_algo_info((a), GCRYCTL_GET_ASNOID, (b), (n))

/* Enable debugging for digest object A; i.e. create files named
   dbgmd-<n>.<string> while hashing.  B is a string used as the suffix
   for the filename.  This macro is deprecated, use gcry_md_debug. */
#ifndef GCRYPT_NO_DEPRECATED
#define gcry_md_start_debug(a,b) \
            gcry_md_ctl( (a), GCRYCTL_START_DUMP, (b), 0 )

/* Disable the debugging of A.  This macro is deprecated, use
   gcry_md_debug.  */
#define gcry_md_stop_debug(a,b) \
            gcry_md_ctl( (a), GCRYCTL_STOP_DUMP, (b), 0 )
#endif

/* Get a list consisting of the IDs of the loaded message digest
   modules.  If LIST is zero, write the number of loaded message
   digest modules to LIST_LENGTH and return.  If LIST is non-zero, the
   first *LIST_LENGTH algorithm IDs are stored in LIST, which must be
   of according size.  In case there are less message digest modules
   than *LIST_LENGTH, *LIST_LENGTH is updated to the correct
   number.  */
gcry_error_t gcry_md_list (int *list, int *list_length);



/* Alternative interface for asymmetric cryptography.  This interface
   is deprecated.  */

/* The algorithm IDs. */
typedef enum gcry_ac_id
  {
    GCRY_AC_RSA = 1,
    GCRY_AC_DSA = 17,
    GCRY_AC_ELG = 20,
    GCRY_AC_ELG_E = 16
  }
gcry_ac_id_t;

/* Key types.  */
typedef enum gcry_ac_key_type
  {
    GCRY_AC_KEY_SECRET,
    GCRY_AC_KEY_PUBLIC
  }
gcry_ac_key_type_t;

/* Encoding methods.  */
typedef enum gcry_ac_em
  {
    GCRY_AC_EME_PKCS_V1_5,
    GCRY_AC_EMSA_PKCS_V1_5
  }
gcry_ac_em_t;

/* Encryption and Signature schemes.  */
typedef enum gcry_ac_scheme
  {
    GCRY_AC_ES_PKCS_V1_5,
    GCRY_AC_SSA_PKCS_V1_5
  }
gcry_ac_scheme_t;

/* AC data.  */
#define GCRY_AC_FLAG_DEALLOC     (1 << 0)
#define GCRY_AC_FLAG_COPY        (1 << 1)
#define GCRY_AC_FLAG_NO_BLINDING (1 << 2)

/* This type represents a `data set'.  */
typedef struct gcry_ac_data *gcry_ac_data_t;

/* This type represents a single `key', either a secret one or a
   public one.  */
typedef struct gcry_ac_key *gcry_ac_key_t;

/* This type represents a `key pair' containing a secret and a public
   key.  */
typedef struct gcry_ac_key_pair *gcry_ac_key_pair_t;

/* This type represents a `handle' that is needed by functions
   performing cryptographic operations.  */
typedef struct gcry_ac_handle *gcry_ac_handle_t;

typedef gpg_error_t (*gcry_ac_data_read_cb_t) (void *opaque,
					       unsigned char *buffer,
					       size_t *buffer_n);

typedef gpg_error_t (*gcry_ac_data_write_cb_t) (void *opaque,
						unsigned char *buffer,
						size_t buffer_n);

typedef enum
  {
    GCRY_AC_IO_READABLE,
    GCRY_AC_IO_WRITABLE
  }
gcry_ac_io_mode_t;

typedef enum
  {
    GCRY_AC_IO_STRING,
    GCRY_AC_IO_CALLBACK
  }
gcry_ac_io_type_t;

typedef struct gcry_ac_io
{
  /* This is an INTERNAL structure, do NOT use manually.  */
  gcry_ac_io_mode_t mode _GCRY_ATTR_INTERNAL;
  gcry_ac_io_type_t type _GCRY_ATTR_INTERNAL;
  union
  {
    union
    {
      struct
      {
	gcry_ac_data_read_cb_t cb;
	void *opaque;
      } callback;
      struct
      {
	unsigned char *data;
	size_t data_n;
      } string;
      void *opaque;
    } readable;
    union
    {
      struct
      {
	gcry_ac_data_write_cb_t cb;
	void *opaque;
      } callback;
      struct
      {
	unsigned char **data;
	size_t *data_n;
      } string;
      void *opaque;
    } writable;
  } io _GCRY_ATTR_INTERNAL;
}
gcry_ac_io_t;

/* The caller of gcry_ac_key_pair_generate can provide one of these
   structures in order to influence the key generation process in an
   algorithm-specific way.  */
typedef struct gcry_ac_key_spec_rsa
{
  gcry_mpi_t e;                 /* E to use.  */
} gcry_ac_key_spec_rsa_t;

/* Structure used for passing data to the implementation of the
   `EME-PKCS-V1_5' encoding method.  */
typedef struct gcry_ac_eme_pkcs_v1_5
{
  size_t key_size;
} gcry_ac_eme_pkcs_v1_5_t;

typedef enum gcry_md_algos gcry_md_algo_t;

/* Structure used for passing data to the implementation of the
   `EMSA-PKCS-V1_5' encoding method.  */
typedef struct gcry_ac_emsa_pkcs_v1_5
{
  gcry_md_algo_t md;
  size_t em_n;
} gcry_ac_emsa_pkcs_v1_5_t;

/* Structure used for passing data to the implementation of the
   `SSA-PKCS-V1_5' signature scheme.  */
typedef struct gcry_ac_ssa_pkcs_v1_5
{
  gcry_md_algo_t md;
} gcry_ac_ssa_pkcs_v1_5_t;

/* Returns a new, empty data set in DATA.  */
gcry_error_t gcry_ac_data_new (gcry_ac_data_t *data);

/* Destroy the data set DATA.  */
void gcry_ac_data_destroy (gcry_ac_data_t data);

/* Create a copy of the data set DATA and store it in DATA_CP.  */
gcry_error_t gcry_ac_data_copy (gcry_ac_data_t *data_cp,
                               gcry_ac_data_t data);

/* Return the number of named MPI values inside of the data set
   DATA.  */
unsigned int gcry_ac_data_length (gcry_ac_data_t data);

/* Destroy any values contained in the data set DATA.  */
void gcry_ac_data_clear (gcry_ac_data_t data);

/* Add the value MPI to DATA with the label NAME.  If FLAGS contains
   GCRY_AC_FLAG_DATA_COPY, the data set will contain copies of NAME
   and MPI.  If FLAGS contains GCRY_AC_FLAG_DATA_DEALLOC or
   GCRY_AC_FLAG_DATA_COPY, the values contained in the data set will
   be deallocated when they are to be removed from the data set.  */
gcry_error_t gcry_ac_data_set (gcry_ac_data_t data, unsigned int flags,
                               const char *name, gcry_mpi_t mpi);

/* Store the value labelled with NAME found in DATA in MPI.  If FLAGS
   contains GCRY_AC_FLAG_COPY, store a copy of the MPI value contained
   in the data set.  MPI may be NULL.  */
gcry_error_t gcry_ac_data_get_name (gcry_ac_data_t data, unsigned int flags,
                                    const char *name, gcry_mpi_t *mpi);

/* Stores in NAME and MPI the named MPI value contained in the data
   set DATA with the index IDX.  If FLAGS contains GCRY_AC_FLAG_COPY,
   store copies of the values contained in the data set. NAME or MPI
   may be NULL.  */
gcry_error_t gcry_ac_data_get_index (gcry_ac_data_t data, unsigned int flags,
                                     unsigned int idx,
                                     const char **name, gcry_mpi_t *mpi);

/* Convert the data set DATA into a new S-Expression, which is to be
   stored in SEXP, according to the identifiers contained in
   IDENTIFIERS.  */
gcry_error_t gcry_ac_data_to_sexp (gcry_ac_data_t data, gcry_sexp_t *sexp,
				   const char **identifiers);

/* Create a new data set, which is to be stored in DATA_SET, from the
   S-Expression SEXP, according to the identifiers contained in
   IDENTIFIERS.  */
gcry_error_t gcry_ac_data_from_sexp (gcry_ac_data_t *data, gcry_sexp_t sexp,
				     const char **identifiers);

/* Initialize AC_IO according to MODE, TYPE and the variable list of
   arguments.  The list of variable arguments to specify depends on
   the given TYPE.  */
void gcry_ac_io_init (gcry_ac_io_t *ac_io, gcry_ac_io_mode_t mode,
		      gcry_ac_io_type_t type, ...);

/* Initialize AC_IO according to MODE, TYPE and the variable list of
   arguments AP.  The list of variable arguments to specify depends on
   the given TYPE.  */
void gcry_ac_io_init_va (gcry_ac_io_t *ac_io, gcry_ac_io_mode_t mode,
			 gcry_ac_io_type_t type, va_list ap);

/* Create a new ac handle.  */
gcry_error_t gcry_ac_open (gcry_ac_handle_t *handle,
                           gcry_ac_id_t algorithm, unsigned int flags);

/* Destroy an ac handle.  */
void gcry_ac_close (gcry_ac_handle_t handle);

/* Initialize a key from a given data set.  */
gcry_error_t gcry_ac_key_init (gcry_ac_key_t *key, gcry_ac_handle_t handle,
                               gcry_ac_key_type_t type, gcry_ac_data_t data);

/* Generates a new key pair via the handle HANDLE of NBITS bits and
   stores it in KEY_PAIR.  In case non-standard settings are wanted, a
   pointer to a structure of type gcry_ac_key_spec_<algorithm>_t,
   matching the selected algorithm, can be given as KEY_SPEC.
   MISC_DATA is not used yet.  */
gcry_error_t gcry_ac_key_pair_generate (gcry_ac_handle_t handle,
                                        unsigned int nbits, void *spec,
                                        gcry_ac_key_pair_t *key_pair,
                                        gcry_mpi_t **misc_data);

/* Returns the key of type WHICH out of the key pair KEY_PAIR.  */
gcry_ac_key_t gcry_ac_key_pair_extract (gcry_ac_key_pair_t key_pair,
                                        gcry_ac_key_type_t which);

/* Returns the data set contained in the key KEY.  */
gcry_ac_data_t gcry_ac_key_data_get (gcry_ac_key_t key);

/* Verifies that the key KEY is sane via HANDLE.  */
gcry_error_t gcry_ac_key_test (gcry_ac_handle_t handle, gcry_ac_key_t key);

/* Stores the number of bits of the key KEY in NBITS via HANDLE.  */
gcry_error_t gcry_ac_key_get_nbits (gcry_ac_handle_t handle,
                                    gcry_ac_key_t key, unsigned int *nbits);

/* Writes the 20 byte long key grip of the key KEY to KEY_GRIP via
   HANDLE.  */
gcry_error_t gcry_ac_key_get_grip (gcry_ac_handle_t handle, gcry_ac_key_t key,
                                   unsigned char *key_grip);

/* Destroy a key.  */
void gcry_ac_key_destroy (gcry_ac_key_t key);

/* Destroy a key pair.  */
void gcry_ac_key_pair_destroy (gcry_ac_key_pair_t key_pair);

/* Encodes a message according to the encoding method METHOD.  OPTIONS
   must be a pointer to a method-specific structure
   (gcry_ac_em*_t).  */
gcry_error_t gcry_ac_data_encode (gcry_ac_em_t method,
				  unsigned int flags, void *options,
				  gcry_ac_io_t *io_read,
				  gcry_ac_io_t *io_write);

/* Decodes a message according to the encoding method METHOD.  OPTIONS
   must be a pointer to a method-specific structure
   (gcry_ac_em*_t).  */
gcry_error_t gcry_ac_data_decode (gcry_ac_em_t method,
				  unsigned int flags, void *options,
				  gcry_ac_io_t *io_read,
				  gcry_ac_io_t *io_write);

/* Encrypt the plain text MPI value DATA_PLAIN with the key KEY under
   the control of the flags FLAGS and store the resulting data set
   into DATA_ENCRYPTED.  */
gcry_error_t gcry_ac_data_encrypt (gcry_ac_handle_t handle,
                                   unsigned int flags,
                                   gcry_ac_key_t key,
                                   gcry_mpi_t data_plain,
                                   gcry_ac_data_t *data_encrypted);

/* Decrypt the decrypted data contained in the data set DATA_ENCRYPTED
   with the key KEY under the control of the flags FLAGS and store the
   resulting plain text MPI value in DATA_PLAIN.  */
gcry_error_t gcry_ac_data_decrypt (gcry_ac_handle_t handle,
                                   unsigned int flags,
                                   gcry_ac_key_t key,
                                   gcry_mpi_t *data_plain,
                                   gcry_ac_data_t data_encrypted);

/* Sign the data contained in DATA with the key KEY and store the
   resulting signature in the data set DATA_SIGNATURE.  */
gcry_error_t gcry_ac_data_sign (gcry_ac_handle_t handle,
                                gcry_ac_key_t key,
                                gcry_mpi_t data,
                                gcry_ac_data_t *data_signature);

/* Verify that the signature contained in the data set DATA_SIGNATURE
   is indeed the result of signing the data contained in DATA with the
   secret key belonging to the public key KEY.  */
gcry_error_t gcry_ac_data_verify (gcry_ac_handle_t handle,
                                  gcry_ac_key_t key,
                                  gcry_mpi_t data,
                                  gcry_ac_data_t data_signature);

/* Encrypts the plain text readable from IO_MESSAGE through HANDLE
   with the public key KEY according to SCHEME, FLAGS and OPTS.  If
   OPTS is not NULL, it has to be a pointer to a structure specific to
   the chosen scheme (gcry_ac_es_*_t).  The encrypted message is
   written to IO_CIPHER. */
gcry_error_t gcry_ac_data_encrypt_scheme (gcry_ac_handle_t handle,
					  gcry_ac_scheme_t scheme,
					  unsigned int flags, void *opts,
					  gcry_ac_key_t key,
					  gcry_ac_io_t *io_message,
					  gcry_ac_io_t *io_cipher);

/* Decrypts the cipher text readable from IO_CIPHER through HANDLE
   with the secret key KEY according to SCHEME, @var{flags} and OPTS.
   If OPTS is not NULL, it has to be a pointer to a structure specific
   to the chosen scheme (gcry_ac_es_*_t).  The decrypted message is
   written to IO_MESSAGE.  */
gcry_error_t gcry_ac_data_decrypt_scheme (gcry_ac_handle_t handle,
					  gcry_ac_scheme_t scheme,
					  unsigned int flags, void *opts,
					  gcry_ac_key_t key,
					  gcry_ac_io_t *io_cipher,
					  gcry_ac_io_t *io_message);

/* Signs the message readable from IO_MESSAGE through HANDLE with the
   secret key KEY according to SCHEME, FLAGS and OPTS.  If OPTS is not
   NULL, it has to be a pointer to a structure specific to the chosen
   scheme (gcry_ac_ssa_*_t).  The signature is written to
   IO_SIGNATURE.  */
gcry_error_t gcry_ac_data_sign_scheme (gcry_ac_handle_t handle,
				       gcry_ac_scheme_t scheme,
				       unsigned int flags, void *opts,
				       gcry_ac_key_t key,
				       gcry_ac_io_t *io_message,
				       gcry_ac_io_t *io_signature);

/* Verifies through HANDLE that the signature readable from
   IO_SIGNATURE is indeed the result of signing the message readable
   from IO_MESSAGE with the secret key belonging to the public key KEY
   according to SCHEME and OPTS.  If OPTS is not NULL, it has to be an
   anonymous structure (gcry_ac_ssa_*_t) specific to the chosen
   scheme.  */
gcry_error_t gcry_ac_data_verify_scheme (gcry_ac_handle_t handle,
					 gcry_ac_scheme_t scheme,
					 unsigned int flags, void *opts,
					 gcry_ac_key_t key,
					 gcry_ac_io_t *io_message,
					 gcry_ac_io_t *io_signature);

/* Store the textual representation of the algorithm whose id is given
   in ALGORITHM in NAME.  This function is deprecated; use
   gcry_pk_algo_name. */
#ifndef GCRYPT_NO_DEPRECATED
gcry_error_t gcry_ac_id_to_name (gcry_ac_id_t algorithm,
                                 const char **name) 
     /* */                      _GCRY_GCC_ATTR_DEPRECATED;
/* Store the numeric ID of the algorithm whose textual representation
   is contained in NAME in ALGORITHM.  This function is deprecated;
   use gcry_pk_map_name. */
gcry_error_t gcry_ac_name_to_id (const char *name,
                                 gcry_ac_id_t *algorithm)
     /* */                      _GCRY_GCC_ATTR_DEPRECATED;
#endif


/************************************
 *                                  *
 *   Random Generating Functions    *
 *                                  *
 ************************************/

/* The possible values for the random quality.  The rule of thumb is
   to use STRONG for session keys and VERY_STRONG for key material.
   WEAK is usually an alias for STRONG and should not be used anymore
   (except with gcry_mpi_randomize); use gcry_create_nonce instead. */
typedef enum gcry_random_level
  {
    GCRY_WEAK_RANDOM = 0,
    GCRY_STRONG_RANDOM = 1,
    GCRY_VERY_STRONG_RANDOM = 2
  }
gcry_random_level_t;

/* Fill BUFFER with LENGTH bytes of random, using random numbers of
   quality LEVEL. */
void gcry_randomize (void *buffer, size_t length,
                     enum gcry_random_level level);

/* Add the external random from BUFFER with LENGTH bytes into the
   pool. QUALITY should either be -1 for unknown or in the range of 0
   to 100 */
gcry_error_t gcry_random_add_bytes (const void *buffer, size_t length,
                                    int quality);

/* If random numbers are used in an application, this macro should be
   called from time to time so that new stuff gets added to the
   internal pool of the RNG.  */
#define gcry_fast_random_poll()  gcry_control (GCRYCTL_FAST_POLL, NULL)


/* Return NBYTES of allocated random using a random numbers of quality
   LEVEL. */
void *gcry_random_bytes (size_t nbytes, enum gcry_random_level level)
                         _GCRY_GCC_ATTR_MALLOC;

/* Return NBYTES of allocated random using a random numbers of quality
   LEVEL.  The random numbers are created returned in "secure"
   memory. */
void *gcry_random_bytes_secure (size_t nbytes, enum gcry_random_level level)
                                _GCRY_GCC_ATTR_MALLOC;


/* Set the big integer W to a random value of NBITS using a random
   generator with quality LEVEL.  Note that by using a level of
   GCRY_WEAK_RANDOM gcry_create_nonce is used internally. */
void gcry_mpi_randomize (gcry_mpi_t w,
                         unsigned int nbits, enum gcry_random_level level);


/* Create an unpredicable nonce of LENGTH bytes in BUFFER. */
void gcry_create_nonce (void *buffer, size_t length);





/*******************************/
/*                             */
/*    Prime Number Functions   */
/*                             */
/*******************************/

/* Mode values passed to a gcry_prime_check_func_t. */
#define GCRY_PRIME_CHECK_AT_FINISH      0
#define GCRY_PRIME_CHECK_AT_GOT_PRIME   1
#define GCRY_PRIME_CHECK_AT_MAYBE_PRIME 2

/* The function should return 1 if the operation shall continue, 0 to
   reject the prime candidate. */
typedef int (*gcry_prime_check_func_t) (void *arg, int mode,
                                        gcry_mpi_t candidate);

/* Flags for gcry_prime_generate():  */

/* Allocate prime numbers and factors in secure memory.  */
#define GCRY_PRIME_FLAG_SECRET         (1 << 0)

/* Make sure that at least one prime factor is of size
   `FACTOR_BITS'.  */
#define GCRY_PRIME_FLAG_SPECIAL_FACTOR (1 << 1)

/* Generate a new prime number of PRIME_BITS bits and store it in
   PRIME.  If FACTOR_BITS is non-zero, one of the prime factors of
   (prime - 1) / 2 must be FACTOR_BITS bits long.  If FACTORS is
   non-zero, allocate a new, NULL-terminated array holding the prime
   factors and store it in FACTORS.  FLAGS might be used to influence
   the prime number generation process.  */
gcry_error_t gcry_prime_generate (gcry_mpi_t *prime,
                                  unsigned int prime_bits,
                                  unsigned int factor_bits,
                                  gcry_mpi_t **factors,
                                  gcry_prime_check_func_t cb_func,
                                  void *cb_arg,
                                  gcry_random_level_t random_level,
                                  unsigned int flags);

/* Find a generator for PRIME where the factorization of (prime-1) is
   in the NULL terminated array FACTORS. Return the generator as a
   newly allocated MPI in R_G.  If START_G is not NULL, use this as
   teh start for the search. */
gcry_error_t gcry_prime_group_generator (gcry_mpi_t *r_g,
                                         gcry_mpi_t prime,
                                         gcry_mpi_t *factors,
                                         gcry_mpi_t start_g);


/* Convenience function to release the FACTORS array. */
void gcry_prime_release_factors (gcry_mpi_t *factors);


/* Check wether the number X is prime.  */
gcry_error_t gcry_prime_check (gcry_mpi_t x, unsigned int flags);



/************************************
 *                                  *
 *     Miscellaneous Stuff          *
 *                                  *
 ************************************/

/* Log levels used by the internal logging facility. */
enum gcry_log_levels 
  {
    GCRY_LOG_CONT   = 0,    /* (Continue the last log line.) */
    GCRY_LOG_INFO   = 10,
    GCRY_LOG_WARN   = 20,
    GCRY_LOG_ERROR  = 30,
    GCRY_LOG_FATAL  = 40,
    GCRY_LOG_BUG    = 50,
    GCRY_LOG_DEBUG  = 100
  };

/* Type for progress handlers.  */
typedef void (*gcry_handler_progress_t) (void *, const char *, int, int, int);

/* Type for memory allocation handlers.  */
typedef void *(*gcry_handler_alloc_t) (size_t n);

/* Type for secure memory check handlers.  */
typedef int (*gcry_handler_secure_check_t) (const void *);

/* Type for memory reallocation handlers.  */
typedef void *(*gcry_handler_realloc_t) (void *p, size_t n);

/* Type for memory free handlers.  */
typedef void (*gcry_handler_free_t) (void *);

/* Type for out-of-memory handlers.  */
typedef int (*gcry_handler_no_mem_t) (void *, size_t, unsigned int);

/* Type for fatal error handlers.  */
typedef void (*gcry_handler_error_t) (void *, int, const char *);

/* Type for logging handlers.  */
typedef void (*gcry_handler_log_t) (void *, int, const char *, va_list);

/* Certain operations can provide progress information.  This function
   is used to register a handler for retrieving these information. */
void gcry_set_progress_handler (gcry_handler_progress_t cb, void *cb_data);


/* Register a custom memory allocation functions. */
void gcry_set_allocation_handler (
                             gcry_handler_alloc_t func_alloc,
                             gcry_handler_alloc_t func_alloc_secure,
                             gcry_handler_secure_check_t func_secure_check,
                             gcry_handler_realloc_t func_realloc,
                             gcry_handler_free_t func_free);

/* Register a function used instead of the internal out of memory
   handler. */
void gcry_set_outofcore_handler (gcry_handler_no_mem_t h, void *opaque);

/* Register a function used instead of the internal fatal error
   handler. */
void gcry_set_fatalerror_handler (gcry_handler_error_t fnc, void *opaque);

/* Register a function used instead of the internal logging
   facility. */
void gcry_set_log_handler (gcry_handler_log_t f, void *opaque);

/* Reserved for future use. */
void gcry_set_gettext_handler (const char *(*f)(const char*));

/* Libgcrypt uses its own memory allocation.  It is important to use
   gcry_free () to release memory allocated by libgcrypt. */
void *gcry_malloc (size_t n) _GCRY_GCC_ATTR_MALLOC;
void *gcry_calloc (size_t n, size_t m) _GCRY_GCC_ATTR_MALLOC;
void *gcry_malloc_secure (size_t n) _GCRY_GCC_ATTR_MALLOC;
void *gcry_calloc_secure (size_t n, size_t m) _GCRY_GCC_ATTR_MALLOC;
void *gcry_realloc (void *a, size_t n);
char *gcry_strdup (const char *string) _GCRY_GCC_ATTR_MALLOC;
void *gcry_xmalloc (size_t n) _GCRY_GCC_ATTR_MALLOC;
void *gcry_xcalloc (size_t n, size_t m) _GCRY_GCC_ATTR_MALLOC;
void *gcry_xmalloc_secure (size_t n) _GCRY_GCC_ATTR_MALLOC;
void *gcry_xcalloc_secure (size_t n, size_t m) _GCRY_GCC_ATTR_MALLOC;
void *gcry_xrealloc (void *a, size_t n);
char *gcry_xstrdup (const char * a) _GCRY_GCC_ATTR_MALLOC;
void  gcry_free (void *a);

/* Return true if A is allocated in "secure" memory. */
int gcry_is_secure (const void *a) _GCRY_GCC_ATTR_PURE;

/* Return true if Libgcrypt is in FIPS mode.  */
#define gcry_fips_mode_active()  !!gcry_control (GCRYCTL_FIPS_MODE_P, 0)


/* Include support for Libgcrypt modules.  */
#include <gcrypt-module.h>

#if 0 /* (Keep Emacsens' auto-indent happy.) */
{
#endif
#ifdef __cplusplus
}
#endif
#endif /* _GCRYPT_H */
     global_arrays.h                                                                                     0000777 �    �Ԧ0120355 00000011555 12042342513 013423  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_arrays_h
#define global_arrays_h
#define ARRAY_OFFSET 27  
/* Data types understood by the DMI database */
static char data_array[50][50] = 
{
/* Data types */
    "MASK",
    "DSGN",
    "TMPL",
    "PC",
    "TMPL_DSGN",
    "DSGN_TMPL",
    "DSGN_GROUP",
    "WAFER_LINE",
    "GLBL",
    "GLOBAL_TABLES",
    "WL_PC",
    "OFF_TMPL",
    "OFF_DSGN",
    "OFF_TMPL_OFF_DSGN",
    "OFF_TMPL_DSGN",
    "TMPL_OFF_DSGN",
    "OFF_TMPL_AUDIT",
    "OFF_DSGN_AUDIT",
    "APP_TMPL_DSGN"
};

/* Transaction types */
static char transaction_array[50][50] = 
{
    "UPD_REQ ",
    "DLT_REQ",
    "READ_REQ",
    "MOD_REQ",
    "RTV_DATA_REQ",
    "RTV_FILES_REQ",
    "CRT_REQ",
    "REMOTE_REQ",
    "GLOBAL_REQ",
    "VERSION_REQ",
    "APPROVE_REQ",
    "ALIVE_REQ",
    "RELEASE_REQ",
    "NETWORK_REQ",
    "PORT_REQ",
    "REGISTER_REQ",
    "LIST_REQ",
    "DB_ADMIN_REQ",
    "RVUPORT_REQ"
};

/* Request types */
static char req_array[200][50] = 
{
    /* Update requests */
    "DBOVRWR_DATA  ",
    "DBCHKIN_DATA ",
    "DBUPD_DATA ",
    "DBCHKOFF_DATA ",
    "DBDLT_DATA ",

/* Check out and read transactions */
    "DBCHKOUT_DATA ",
    "DBAPP_DATA ",

/* Modify transactions */
    "DBCANCHKOUT_DATA ",
    "DBCANAPP_DATA ",
    "DBTRANSFER_DATA ",

/* Read data", retrieve versions */
    "DBREAD_DATA ",
    "DBREAD_FILE ",
    "DBRTV_VERSIONS ",
    "DBUNAPP_DATA ",
    "DBBACKOUT_DATA ",
    "DBDLT_VERSIONS ",
    "DBPORT_RVU_DATA ",

/* Create", copy, and port data */
    /* Action class is create */
    "DBCRT_DATA ",
    "DBCOPY_DATA ",
    "DBCOPY_MASK2TMPL",
/* Port mask to template", done in Hercules */
    "DBPORT_MASK2TMPL",
    /* Port specified data to the database */
    "DBPORT_DATA ",
    /* Create a group of files with specified data type */
    "DBCRT_FILES",
    /* Create a speicified file */
    "DBCRT_FILE",

    /* Modify transactions */
    "DBMOD_PIN ",
    "DBMOD_OWNER ",
    /* Grant overwrite for one transaction */
    "DBGRANT_OVRWR ",
/* Set C4 mask */
    "DBSET_C4MASK ",
    /* Empty requests for future use */
    "DBSET_EMPTY1",
    "DBSET_EMPTY2",

    /* Retrieve small pieces of data */
    /* Uses transaction type DBRTV_DATA */
/* Retrieve the mask set PIN */
    "DBRTV_PIN",
    "DBRTV_CWD",
    /* Retrieve serial number for Mesa Mask Shop */
    "DBRTV_SERIAL ",
    "DBRTV_COM1SERIAL",
/* Retrieve annual contract number */
    "DBRTV_CNB",
    "DBRTV_CONTRACT_NUM ",
/* Retrieve value of C4 mask */
    "DBRTV_C4MASK",

    /* Registration requests */
    /* Transaction type of REGISTER_REQ */
    "DBCHK_REG",
    "DBAUTO_REG",
    "DBMAN_REG",
    "DBFORCE_REG",
    "DBRMT_MAN_REG",
    "DBRMT_FORCE_REG",
/* Resend registration entries */
    "DBRESEND_REG",

    /* List requests */
    /* Have transaction type LILST_REQ */
    /* Pull out file with a list of items */
    /* List PC links */
    "DBLIST_LINKS ",
    /* Pull down list of PC libraries */
    "DBLIST_PCS ",
    /* List masks with specified search criteria */
    "DBLIST_MASKS ",
    /* Send list of all masks */
    "DBLIST_ALL_MASKS ",
    /* Send list of templates with specified criteria */
    /* Currently being used by Hercules */
    "DBLIST_TMPLS",
   /* Currently not being used */
    "DBLIST_SERVERS ",
    /* Unused list request */
    "DBLIST_EMPTY1",
    "DBLIST_EMPTY2",

    /* Retrieve one or more files */
    /* Have transaction type RTV_FILES */
    "DBRTV_FILES ",
    "DBRTV_FILE",
    "EMPTY_REQ ",

/* Remote request types */
    "DBRMTCRT_DATA ",
    "DBRMTDLT_DATA ",
    "DBMARK_DATA ",
    "DBREFRESH_DATA ",
    "DBRMTPORT_RVU ",
    "DBRMT_AUDIT_DATA ",

/* PC functions */
    "DBADD_LINK ",
    "DBMOD_LINK ",
    "DBDLT_LINK ",
    "DBREPLACE_DATA ",
    "DBRENAME_DATA ",

    "DBREAD_CONTROL_GROUP ",

    /* Network requests */

    "DBRESEND_DATA ",
    "DBUPD_SERVER ",
    "DBPING_REQ ",
    "DBREAD_DIR ",

    /* Audit actions */
    "DBRMT_AUDIT_GLOBAL ",
    "DBAUDIT_DATA ",
    "DBAUDIT_GLOBAL ",
    "DBAUDIT_FAB ",
    "DBAUDIT_DESIGN ",
    "DBAUDIT_MASK ",
    "DBAUDIT_PC ",
    "DBAUDIT_OFF_FAB",
/* Audit the global file entries against the actual directories */
    "DBAUDIT_DIR2GLOBAL",
/* Audit the global wafer line and design templates */
    "DBAUDIT_WL_DSGN_FILES",
/* Audit entire directory structure */
    "DBRMT_AUDIT_DIRECTORIES",
    "DBAUDIT_DIRECTORIES",
    "DBAUDIT_OFF_DESIGN",
    "DBREAD_RVU_DATA",
    "DBRMV_DATA",
/* Perform df and ps on database */
    "DBPS",
    "DBPS_DF",
/* Resend global files to remote servers */
    "DBRESEND_FILES",
/* Copy directory to remote servers */
    "DBCOPY_RMT_FILES ",
/* Force data to be shadowed to remote databases */
    "DBSHADOW_DATA",

    /* Database administrative functions */
/* Add template builder and template approver */
    "DBADD_TMPLBLD",
    "DBADD_APPROVER",
    "DBADD_TMPLBLD_APPROVER",
    "DBPUT_GLOBAL_FILE ",
    "DBMOD_PASSWORD "
};


#endif /* ifdef for global_arrays_h */
                                                                                                                                                   global_caution_errs.h                                                                               0000777 �    �Ԧ0120355 00000007775 12042342513 014630  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef server_caution_errs_h
#define server_caution_errs_h
/* Caution messages */
#define mask_read_cau "\n\nCaution -470:  While listing masks unable to read state file:"
#define MASK_READ_CAU -470
#define rd_open_cau "\n\nCaution -471:  Unable to open for read file '"
#define RD_OPEN_CAU -471
#define connect_cau "\n\nCaution -472:  Received error code '%d' while trying to connect to remote server '%s'"
#define CONNECT_CAU -472
#define CONNECT_CAU -472
#define upd_server_cau "\n\nCaution -473:  Unable to find global entry for database server '"
#define UPD_SERVER_CAU -473
#define no_glbl_table_cau "\n\nCaution -474:  Entry for '%s' not present in global table:%s.\n\
               Missing entry added."
#define NO_GLBL_TABLE_CAU -474
#define rd_parent_cau "\n\nCaution -475:  Unable to open for read parent file '"
#define RD_PARENT_CAU -475
#define mkdir_cau "Error -476:  Unable to create directory '"
#define MKDIR_CAU -476
#define mkdir_audit "Audit -474:  Created missing directory '"
#define MKDIR_AUDIT -474
#define wr_open_cau "\n\nCaution -477:  Unable to open for write file '"
#define WR_OPEN_CAU -477
#define rename_cau "\n\nCaution -478:  Unable to rename '%s' to '%s'."
#define RENAME_CAU -478
#define open_dir_cau "\n\nCaution -479:  Unable to open for read directory '"
#define OPEN_DIR_CAU -479
#define read_cau "\n\nCaution -480:  Unable to read file '"
#define READ_CAU -480
#define extra_dir "\n\nCaution -481:    Found and deleted unexpected database directory:\n\
               '"
#define EXTRA_DIR -481
#define bad_toc_cau "\n\nCaution -482:    Unable to find and delete:\n\n\
               Global entry:  "
#define BAD_TOC_CAU -482
#define no_glbl_entry_cau "\n\nCaution -483:  Entry for '%s' not present in global table:%s.\n\
               Missing entry added."
#define NO_GLBL_ENTRY -483
#define global_table_cau "\n\nCaution -484:  Duplicate global table entry found for:\n\n\
               Table:  '%s'\n\
          Entry:  '%s'\n"
#define GLOBAL_TABLE_CAU -484
#define dup_entry_cau "\n\nCaution -485:  Duplicate entry found for:\n\n\
               Global entry:  "
#define DUP_ENTRY_CAU -485
#define invalid_dir_name "\n\nCaution -486:  Unable to delete directory with 0 length name."
#define INVALID_DIR_NAME -486
#define invalid_dir_open "\n\nCaution -487:  Unable to open for deletion directory:\n\
               '"
#define INVALID_DIR_OPEN -487
#define invalid_rmdir "\n\nCaution -488:  Unable to remove directory:\n\
               '"
#define INVALID_RMDIR -488
#define invalid_unlink "\n\nCaution -489:  Unable to unlink file:\n\
               '"
#define INVALID_UNLINK -489
#define no_mask_delta "\n\nCaution -490:  No delta files present for mask set '"
#define NO_MASK_DELTA -490
#define extra_mask_entry "\n\nCaution -491:  Extra entry found in global table for mask set '"
#define EXTRA_MASK_ENTRY -491
#define missing_mask_entry "\n\nCaution -492:  Added missing entry in global table for mask set '"
#define MISSING_MASK_ENTRY -492
#define parse_cau "\n\nCaution -493:  Unable to parse line '%s'\n\
               in file '%s'"
#define PARSE_CAU -493
#define builder_cau "\n\nCaution -494:  %d template builders have been defined.  Database manager only allows %d builders.\n\
              Increase this limit defined in 'pc_data.h' and re-compile the database server."
#define BUILDER_CAU -494
#define index_cau "\n\nCaution -495:  Invalid index found while parsing table '"
#define INDEX_CAU -495
#define mod_state_cau "\n\nCaution -496:  Changed approval state to '%s' in file '%s'"
#define MOD_STATE_CAU -496
#define mod_table_cau "\n\nCaution -497:  Modified approval state for '%s' to '%c' in table '%s'"
#define MOD_TABLE_CAU -497
#define split_cau "\n\nCaution -498:  Unable to split '%s' into fab and flow."
#define SPLIT_CAU -498
#define rmt_update_cau "\n\nCaution 499:  Remote update unable to open for read file "
#define RMT_UPDATE_CAU -499
#define extra_entry_cau "Caution -519:  Extra entry '%s' found and deleted from global table  '%s'"
#define EXTRA_ENTRY_CAU -519

#endif /* ifdef for server_caution_errs_h */
   global_client_errs.h                                                                                0000777 �    �Ԧ0120355 00000030006 12315311020 014413  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_client_errs_h
#define global_client_errs_h
/* This file holds global errors used by all clients */
#define binary_err "Error -415:  Unable to set FTP transfer type to binary."
#define BINARY_ERR -415
#define NOT_REG -300
#define BAD_PIN -301

#define connect_err "Error -110:    Received error code '%d' while trying\n\
               to connect to database server:\n\n\
               Machine:  %s\n\
               Address:  %s\n\n"
#define ssl_connect_err "Error -110:    Received error code '%d' while trying\n\
               to connect to database server:\n\n\
               Machine:  %s\n\
               Address:  %s\n\
               Port:  %d\n\n"
#define DIAGNOSTICS "Diagnostics:\n\n\
               Verify the correct IP node address for the\n\
               database is in your '.MaskPreprc' file.\n\
               Verify that the database machine is accessible.\n\n"
#define CONNECT_ERR -110
#define bad_sock_open "Error -111:      Received error code '%d' while trying to create \n\
              local socket structure."
#define BAD_SOCK_OPEN -111
#ifdef INTERCOM
#define bad_get \
"Error -112:   Received error code '%d' while retrieving data from the\n\
               server.  This error is usually caused by a temporary network\n\
               problem.  Please try your transaction again.\n\n\
               %s"
#else
#define bad_get "Error -112:  Received error code '%d' while retrieving data \n\
               from the database server.\n\n\
          This error is usually caused by a temporary network\n\
               problem.  Please wait a few minutes and try your \n\
               database request again.\n\n\
               %s"
#endif
#define BAD_GET -112
#ifdef INTERCOM
#define bad_put \
"Error -113:   Received error code '%d' while attempting to send data to the\n\
               server.  This error is usually caused by a temporary network\n\
               problem.  If you are uploading data, try transferring the\n\
               data using the -ps option.  If you are downloading data, try\n\
               retrieving data using the -gs option.  These options perform\n\
               serial transfer so time outs are less likely to occur.\n\n\
               %s"
#else
#define bad_put "Error -113:  Received error code '%d' while attempting to send\n\
               data to the database server.\n\n\
          This error is usually caused by a temporary network\n\
               problem.  Please wait a few minutes and try your \n\
               database request again.\n\n\
               %s"
#endif
#define BAD_PUT -113
#define bad_create "Error -114:    Received error code %d while trying to create file:\n\n\
               '%s'\n\n\
               This error usually occurs when you are running \n\
               %s in a directory where you do not have \n\
               write permission.  It may also occur if your \n\
               disk is full.\n\n%s"
#define BAD_CREATE -114
#define bad_rd_open "Error -115:    Unable to open for read file:"
#define bad_rd_open_msg \
"               This error usually occurs when the file being\n\
               accessed does not exist or when you do not\n\
               have read access on the file.\n\n\
For assistance contact the DMI database administrator:\n\n\
               Email:  transbug@nxp.com"
#define BAD_RD_OPEN -115
#define bad_write "Error -116:    Unable to write to file:"
#define BAD_WRITE -116
#define erase_wrn "\n\nWarning -117:  Unable to erase the following files(s) from your local disk:\n\n\
               '"
#define ERASE_WRN -117
#define overwrite_dsgn "\n\nWarning -118:  The design files in your working directory were over-\n\
               written with the latest version from the database."
#define OVERWRITE_DSGN -118
#define overwrite_tmpl "\n\nWarning -119:  The template files in your working directory were over-\n\
               written with the latest version from the database."
#define OVERWRITE_TMPL -119
#define bad_string_len "Error -120:  Received error code '%d' with retrieved string length of '%d'\n\n%s"
#define BAD_STRING_LEN -120
#define bad_file_get "Error -121:    Unable to retrieve file name from the database server."
#define BAD_FILE_GET -121
#define bad_hostname "Error -122:    Machine name not found in /etc/hosts."
#define BAD_HOSTNAME -122
#define dir_open_err "Error -123:    Unable to open directory:"
#define DIR_OPEN_ERR -123
#define tmpl_wrn "Warning -124:  Template created, but latest copy\n\
               of template list not retrieved."
#define TMPL_WRN -124
#define chkin_wrn "Warning -125:  Unable to find file:\n\n\
               '%s'\n\n\
Data was checked in without this file."
#define CHKIN_WRN -125
#define upd_wrn "Warning -126:  Unable to find file:\n\n\
               '%s'\n\n\
Data was updated without this file."
#define UPD_WRN -126
#define bad_request "Error -127:    Unable to read local file:\n\n\
               '%s'\n\n\
               Bad request type:  '%d'\n\n\
               %s"
#define BAD_REQUEST -127
#define malloc_err "Error -128:  Unable to allocate dynamic memory."
#define MALLOC_ERR -128
#define bad_ip_addr "Error -129:  Invalid IP node address for server machine:"
#define BAD_IP_ADDR -129
#define no_files_err "Error -130:  No files found to upload."
#define NO_FILES_ERR -130
#define nonexist_err "Warning -131:  The following file no longer exists and\n\
               will not be checked into the database.\n\n\
               '%s'\n\n\
               Press 'Continue' to proceed with check-in.\n\
               Press 'Cancel' to abort."
#define NONEXIST_ERR -131
#define nonexist_wrn "Warning -132:  The following file(s) no longer exist and\n\
               were not checked into the database.\n\n\
               '%s'\n\n"
#define NONEXIST_WRN -132
#define bad_prefix "Error -134:  Database Client received file name with invalid prefix:"
#define BAD_PREFIX -134
#define bad_filelist "Overwrite -135:  File list does not contain required file:\n\n\
               '%s%s%s'\n\n\
Press 'Continue' to put blank file in database.\n\n\
Press 'cancel' to abort action."
#define BAD_FILELIST -135
#define bad_suffix "Warning -136:  File with invalid suffix not transferred to the database."
#define BAD_SUFFIX -136
#ifdef INTERCOM
#define server_err \
"Error -112:   Received error code '%d' while retrieving data from the\n\
               server.  This error is usually caused by a temporary network\n\
               problem.  If you are uploading data, try transferring the\n\
               data using the -ps option.  If you are downloading data, try\n\
               retrieving data using the -gs option.  These options force\n\
               serial data transfer.\n\n\
               %s"
#else
#define server_err "Error -137:  Error code '%d' received from the database server."
#endif
#define SERVER_ERR -137
#define network_err "Error -138:  Network to database machine '%s' is unreachable.\n\n%s"
#define NETWORK_ERR -138
#define run_err "Error -139:  Database server on machine '%s' is not running.\n\n%s"
#define RUN_ERR -139
#define big_connect_err "Error -140:  Unable to connect to database servers:\n"
#define BIG_CONNECT_ERR -140
#define sec_server_wrn "\n\nWarning -141:  Data retrieved from secondary database on machine:\n\n\
               '"
#define SEC_SERVER_WRN -141
#define sec_own_wrn "Warning -142:  Data accessed owned by secondary database on machine:\n\n\
               '"
#define SEC_OWN_WRN -142
#define bad_rename "Error -143:  Unable to rename file:\n\n\
               '%s'\n\n               to \n\n               '%s'\n\n%s"
#define BAD_RENAME -143
#define db_write_err "Error -144:  Database server which owns the data is currently inaccessible.\n\
               Data owned by server on machine '%s'.  Read\n\
               transactions may be performed on alternate database servers."
#define DB_WRITE_ERR -144
#define db_own_err "Error -145:  None of the databases accessed own the requested data."
#define DB_OWN_ERR -145
#define crt_dir_err "Error -146:    Unable to create directory \n\n               '"
#define CRT_DIR_ERR -146
#define bad_wr_open "Error -147:    Unable to open for write file:"
#define BAD_WR_OPEN -147
#define bad_read "Error -148:    Unable to read file:"
#define BAD_READ -148
#define essential_err "Error -149:    Essential file '%s' is not present.\n\
               This file must be present to proceed with check-in."
#define ESSENTIAL_ERR -149
#define ovr_wrn1 "Warning -150:  The following files are already present on your local disk:"
#define ovr_wrn2 "\n               Press 'Continue to overwrite files.\n\
               Press 'Cancel' to abort."
#define OVR_WRN -150
#define no_fab_files "Error -151:  No fab files found to send to database."
#define NO_FAB_FILES -151
#define command_open4write_err "Error -152:    Unable to open for write command:"
#define COMMAND_OPEN4WRITE_ERR -152
#define command_open4read "Error -153:    Unable to open for read command:"
#define COMMAND_OPEN4READ -153
#define command_write_err "Error -154:    Unable to write to command:"
#define COMMAND_WRITE_ERR -154
#define command_read_err "Error -155:    Unable to read from command:"
#define COMMAND_READ_ERR -155
#define bad_files "\n\nWarning -156:  The following objects were not found and were not send:\n\n"
#define BAD_FILES -156
#define format_err "Error -157:  Invalid format entered.\n\n"
#define FORMAT_ERR -157
#define invalid_pin "Error -158:  Two entries enter for new PIN do not match."
#define INVALID_PIN -158
#define no_valid_files "Error -159:  No valid file names specified."
#define NO_VALID_FILES -159
#define dlt_dir_err "Error -160:    Received error code %d while deleting directory:\n\n\
               '%s'\n\n%s"
#define DLT_DIR_ERR -160
#define file_dlt_err "Error -161:    Unable to delete file:"
#define FILE_DLT_ERR -161
#define dir_access_err "Error -162:  Unable to access directory:"
#define DIR_ACCESS_ERR -162
#define access_err "Error -162:  Unable to access:"
#define ACCESS_ERR -162
#define bad_write_open "Error -163:    Unable to open for write remote file:"
#define bad_rmt_write "Error -164:    Unable to write to remote FTP file:"
#define maskshop_err "Error -165:    Unable to find information for Mask Shop:"
#define ftp_connect_err "Error -166:    Unable to FTP to Mask Shop:"
#define bad_stat \
"Error -167:    Received error code %d while performing stat on\n\
               specified file.  This error usually occurs when \n\
               the file is not present.\n\n\
File name:     '%s'"
#define BAD_STAT -167
#define bad_link "Error -168:  Received error code %d while linking %s to %s"
#define BAD_LINK -168
#define bad_tar "Error -169:    Unable to open tar command:"
#define bad_rmt_delete "Error -170:    Unable to delete remote FTP file:"
#define bad_rmt_chdir "Error -171:    Unable to change to remote FTP directory:"

#define array_alloc_err "Error -172:    Array allocation error has occurred.\n\
               Array holds %d elements, but %d entries were added."
#define ARRAY_ALLOC_ERR -172
#define dlt_dir_wrn "\n\nWarning -173:    Received error code %d while deleting directory:\n\n\
               '%s'\n\n%s"
#define DLT_DIR_WRN -173
#define file_dlt_wrn "Warning -174:    Unable to delete file:"
#define FILE_DLT_WRN -174
#define invalid_send2maskshop_input  "Error -175:  Invalid input passed to 'send2maskshop'.\n\
               Parameters passed in:\n\n"

#define path_env_err "Error -175:  No environment variable present for IcomSend"
#define permission_create_err "Error -176:    Received error code %d while trying to create file:\n\n\
               '%s'\n\n\
               This error occurs when you are running \n\
               %s in a directory where you do not have \n\
               write permission.\n\n%s"
#define PERMISSION_CREATE_ERR -176
#define permission_write_err "Error -177:    Received error code %d while trying to write to file:\n\n\
               '%s'\n\n\
               This error occurs when you are running \n\
               %s in a directory where you do not have \n\
               write permission.\n\n%s"
#define PERMISSION_WRITE_ERR -177

#endif /* ifdef for global_client_errs_h */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          global_client_params.h                                                                              0000777 �    �Ԧ0120355 00000000114 12042342513 014730  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   /* Holds parameters the client passes to the server */
extern GU_PARAMS gu;
                                                                                                                                                                                                                                                                                                                                                                                                                                                    global_defines.h                                                                                    0000777 �    �Ԧ0120355 00000023106 12315355760 013545  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_defines_h
#define global_defines_h

#define DTMS_DB "dtmsdb.freescale.net"
#define EXTERNAL_DTMS_DB "10.81.199.118"
#define TOTAL_SERVERS 10
#define EMAIL_SUFFIX "@freescale.com"
#define  ENCRYPT  1
#define  DECRYPT  2

/* Define the machine type */
#ifdef AIX
#define MACHINE_TYPE "AIX"
#endif
#ifdef MACINTOSH
#define MACHINE_TYPE "macOS X"
#endif
#ifdef SOLARIS
#define MACHINE_TYPE "Solaris"
#endif
#ifdef SOLARIS5
#define MACHINE_TYPE "Solaris5"
#endif
#ifdef SOLARIS7
#define MACHINE_TYPE "Solaris7"
#endif
#ifdef SOLARIS8
#define MACHINE_TYPE "Solaris8"
#endif
#ifdef SOLARIS9
#define MACHINE_TYPE "Solaris9"
#endif
#ifdef SOLARIS10
#define MACHINE_TYPE "Solaris10"
#endif
#ifdef SOLARIS_X86
#define MACHINE_TYPE "Solaris_X86"
#endif
#ifdef SUNOS
#define MACHINE_TYPE "SunOS"
#endif
#ifdef LINUX 
#define MACHINE_TYPE "Linux"
#endif
#ifdef LINUX64 
#ifdef TRANSCEND_GUI
#define MACHINE_TYPE  "TransGUI"
#else
#define MACHINE_TYPE "Linux64"
#endif
#endif
#ifdef LINUX32 
#define MACHINE_TYPE "Linux32"
#endif
#ifdef LINUX_GUI
#define MACHINE_TYPE  "Linux GUI"
#endif
#ifdef WINDOWS_NT4
#ifdef TRANSCEND_GUI
#define MACHINE_TYPE  "Windows GUI"
#else
#define MACHINE_TYPE "Windows 7"
#endif /* for Not GUI */
#endif /* for WINDOWS_NT4 */

#define EMAIL_LEN 64
#define SITE_LEN 64
#define EMAIL_LEN 64
#define BADGE_LEN 48
#define SECPERDAY 86400
#define FABINT_CLIENT_VERSION "1.0"
#define DBNO_REQ 'm'
/* Following two defines help server run as daemon under BSD */
#define DAEMON   /* become a daemon */
#define BSD
/* Defines used to print error messages */
#define LATEST 'l'
#define BY_DATE 'd'
#define BY_VNUM 'v'
#define STARS "**********************************************************************\n\n"
#ifdef MASKPREP
#ifdef FREESCALE
#define HELP_MSG "For assistance, contact DMI database administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#endif
#ifdef M2
#define HELP_MSG "For assistance, contact Mask Designer Database administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef OPCTRANS
#define HELP_MSG "For assistance, contact D/MI database administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef DFM
#define HELP_MSG "For assistance, contact DMI database administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef TRANSWEB
#define PROGRAM "TransWeb"
#define HELP_MSG "For assistance, contact TransWeb administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif /* for TransWeb */
#ifdef TRANSNET
#define PROGRAM "TransNet"
#define HELP_MSG "For assistance, contact TransNet administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif /* for TransNet */
#ifdef TRANS_FTP
#define PROGRAM "TransFTP"
#define HELP_MSG "For assistance, contact TransFTP administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif /* for TransFTP */
#ifdef TRANSCEND
#define PROGRAM "Transcend"
#define HELP_MSG "For assistance, contact Transcend administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#ifndef TRANSNET
#ifdef INTERCOM
#define PROGRAM "InterCom"
#ifdef FREESCALE
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif

/* This is for the external client ran without the server */
#ifdef EXTERNAL
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef VPN
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef FSS
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef GENERIC
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#ifdef LOCAL
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Email Address:  transbug@nxp.com"
#endif
#endif /* for InterCom */
#endif /* For not TransNet */

#define TAB "               "
#define RET_TAB "\n               "
#define RET_TAB8 "\n        "
#define RET_TAB_QUOTE "\n               '"
#define RET2TAB "\n\n               "
#define RET_TAB_QUOTE "\n               '"
#define RET2TAB_QUOTE "\n\n                '"
#define DOUBLE_RET "\n\n"
#define RET_TO "'\n            to '"
#define IN_TABLE "In table:      "

/* Lengths for global parameters */
#define ADDRESS_LEN 16
#define MACHINE_LEN 64
#define USER_LEN 32
#define PWD_LEN 12
#define FILE_SIZE 256
#define PATH_SIZE 1024
#define BLOCK_SIZE 4096	/* Maximum size for file blocks */
#define BIG_BLOCK_SIZE 8192	/* Maximum size for big file blocks */
#define MSG_SIZE 1024	/* Maximum size for sending message */
#define BUFFER_SIZE BLOCK_SIZE + 4 /* Length of buffer to hold file blocks */
#define PWD_SIZE 32	/* Length of temporary encryption password */
#define NULL_PIN "000000"
#define MIN_PIN_LEN 6
#define MAX_PIN_LEN 12
#define KEY_LEN 256
#define PAR_LEN 256
#define TOTAL_FILES 100 	/* Number of files database can manipulate at one time */

#define DBYES 1
#define DBNO 2
#define NOT_SPECIFIED 3
#define DBEMAIL 3
#define FAILURE -1
#define SUCCESS 0
#define NONE "0"
#define SLASH "/"
#define OPC_RMT_REG 20
#define PRINTF printf
#define DASHES "------------------------------------------------------------\n\n"

/* Used to sort file lists */
typedef struct {
	char * key;         /* the key used in the comparison */
	int balance;			/* info about the state of the tree at this node */
							/* 0 if balanced, -1 if heavy to less, 1 if heavy to more */
	struct tree_node *less;
	struct tree_node *more;
}tree_node;

/* Holds temporary buffer space */
typedef struct
{
char files[TOTAL_FILES][FILE_SIZE]; /* Temporary name */
char buffer[BUFFER_SIZE];	/* Temporarily holds messages, file names, etc. */
char header_buffer[MSG_SIZE];	/* Temporarily holds messages, file names, etc. */
char msg[MSG_SIZE];	/* Temporarily holds local messages */
char file_name[FILE_SIZE]; /* Holds local file names */
char dir_name[FILE_SIZE]; /* Holds local file names */
char tmp_data[FILE_SIZE]; /* Holds temporary local data */
char tmp_file[FILE_SIZE]; /* Holds temporary local file names */
char entry[FILE_SIZE];	/* Holds local entry name being manipulated */
} TMP_BUFFER;

/* Variables used for the database transacction */
typedef struct
{
    long upd_time;
    char log_file[FILE_SIZE];
    char upd_time_string[16];	/* Used to generate unique file names */
    char log_date[32];
    char user_site[SITE_LEN];
    char crypt_pwd[PWD_LEN+1];
    char user_email[EMAIL_LEN];
    char pid_string[8];
    int pid;
    char user_pin[16];
} TRANSACTION_PARAMS;

/* Administrator's login and Email */
typedef struct
{
    char name1[16];	/* Administrator's first name */
    char name2[16];	/* Administrator's last name */
    char phone[20];
    char from_email[EMAIL_LEN];	/* Holds address where email is coming from */	/* Account email came from */
    char err_email[EMAIL_LEN];	/* Email address to send error messages to */
    char reg_email[EMAIL_LEN];	/* Email address to send registration requests to */
    char login1[32];	/* Login name of database administrator */
    char login2[32];	/* Login name of database administrator */
    char program[16];
} DBADMIN_INFO;


/* Set default values for keys, data type, PIN, etc. */
/* Default values in global structure GU_PARAMS gu are set */
void dbinit_default_globals();


/* Holds software name, version number and password */
typedef struct
{
    char version[16];
    char program[32];
} DB_SOFTWARE;
/* Status of database transaction returned to the GUI */
typedef enum
{
    DB_REG_WRN = -3,
    DB_OVR = -2,
    DB_ERR = -1,
    DB_SUCCESS = 0,
    DB_WRN  = 1,
    DB_INFO = 2,
    DB_PRV = 3
} DB_ERRS;


typedef struct 
{
    int index;
    char type;	/* Keep, discard or no action */
    char value[KEY_LEN]; /* Holds value of string to keep or discard */
} FILTER_DATA;

#endif /* ifdef for global_defines_h */
                                                                                                                                                                                                                                                                                                                                                                                                                                                          global_macros.h                                                                                     0000777 �    �Ԧ0120355 00000001577 12042342513 013411  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_macros_h
#define global_macros_h
#include "global_client_errs.h"
#include "rtn.h"
/* This file holds macros used by the client and user interface. */

#define CLOSE_DOWN(ret)\
{\
    rtn.flag = DB_ERR;\
    close(sock);\
    DB_ERRurn(ret);\
} /* End close_down */

#define CLOSE_DOWN_ERASE(cwd, ret)\
{\
    if (overwrite_file == DBNO) \
    erase_files(cwd, num, gu.dest_files);\
    rtn.flag = DB_ERR;\
    close(sock);\
    return(ret);\
} /* End CLOSE_DOWN_ERASE */

/* Inform calling function that bad data received from database server */
#define RETURN_BAD_PUT(); {\
    sprintf(rtn.msg, bad_put, errno, HELP_MSG);\
    rtn.flag = DB_ERR; \
    close(sock); \
    return(BAD_PUT); \
}
#define RETURN_BAD_GET(); {\
    sprintf(rtn.msg, bad_get, errno, HELP_MSG);\
    rtn.flag = DB_ERR; \
    close(sock); \
    return(BAD_GET); \
}

#endif /* ifdef for global_macros_h */
                                                                                                                                 global_params.h                                                                                     0000777 �    �Ԧ0120355 00000000532 12042342513 013376  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_params_h
#define global_params_h

#include "global_defines.h"

/* Holds login and Email for database administrator */
extern DBADMIN_INFO admin;

/* Holds program name, version number, and password */
extern DB_SOFTWARE so;


extern int sock;

extern TMP_BUFFER bf;

extern char help_msg[];

#endif /* ifdef for global_params_h */
                                                                                                                                                                      global_reg_prototypes.h                                                                             0000777 �    �Ԧ0120355 00000017751 12232564371 015224  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_reg_prototypes_h
#define global_reg_prototypes_h

     #include <sys/types.h>
     #include <sys/stat.h>
     #include <fcntl.h>
#include "reg_globals.h"
#include "rmt_server.h"
#include "whobu.h"
#include "ldap.h"

int add_email_entry(char *entry);	/* Name of entry to add */

/* Adds entry to any file */
int add_file_entry(
    char *path,
    char *file_name,	/* Complete path name of file entry being added to */
    char *entry);	/* Name of entry to be added */

int add_file_line(
    char *file_name,	/* Complete path name of file entry being added to */
    char *entry);	/* Name of entry to be added */
int add_site_entry(
    char *new_site,
    char *new_email1,
    char *new_email2);

/* Signal handler for timeouts (SIGALRM);.
 * This function is called when the alarm(void); value that was set
 * counts down to zero. This indicates that we haven't received a
 * response. The function sets timeout_flag to indicate a timeout has occurred.
 * The signal interrupts the read system call which sets errno = EINTR.
 */
void timeout( int rcv_signal );

int read_with_timeout( int fd,  char *buf, int num_bytes );



int bsearch_registry(
    char *user_email1,
    char *user_email2,
    char *registry,
    REG_PARAMS *reg);	/* Holds user's name and Email address if found */

int compare_names(
    char *servername1,
    char *servername2,
    char *username1,
    char *username2,
    char *username3);

int compare_sites(
    char *site,	/* User's new site */
    char *email,	/* Address given by user */
    REG_PARAMS *reg);	/* Holds user's email if match found */

int breakup_site(
    char *site,
    char *login,
    char *octet1,
    char *octet2,
    char *octet3);

int dlt_file_entry(
    char *file_path,	/* Complete path name of file entry being deleted from */
    char *entry);	/* Name of entry to be deleted */

int dlt_file_line(
    char *file_name,	/* Complete path name of file entry being deleted from */
    char *entry);


int dlt_site_entry(char *new_site);

int find_email(
    char *user_site,	/* site of user looking for */
    char *email_addr,	/* Email address returned to calling function */
    char address_type);	/* Indicates official or unofficial Email address */

int find_reg_entry(
    char *user_email1,
    char *user_email2,
    char *user_name1,
    char *user_name2,
    char *user_name3,
    REG_PARAMS *reg);	/* Holds user's name and Email address if found */

int new_reg(
    char *site,	/* Needed only to add entry to site table */
    char *user_email1,	/* User's primary Email address */
    char *user_email2,	/* User's secondary Email address */
    char *user_name1,
    char *user_name2,
    char *user_name3,
    char *user_pwd,
    REG_PARAMS *reg);	/* Holds user's name and Email address if found */

/* Connect to the remote socket */
/* If the connect is unsuccessful */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
short init_rmt_socket(
    char *header_buffer,
    RMT_SERVERS *rmt,
    int indx);		/* Index into the server table */

int lookup_reg_user(
    char *new_email,
    char *email_addr,	/* Email address of registered user */
    char *append);	/* Indicates if contact info is needed */

int reg_lookup(void);

void rmt_trans_reg(char *login, char *site, char *email1, char *email2,
    char *name, unsigned long expiration_time);
void rmt_server_reg(RMT_SERVERS *rmt_trans, char *site, char *email1, char *email2);

void rmt_transweb_reg(RMT_SERVERS *rmt_trans, char *site, char *email1, char *email2);

#ifndef OPCTRANS
#ifdef PHOTRONICS
/* Sends registration entry only to remote Intercom servers */
void rmt_dmi_reg(char *site, char *email1, char *email2);
#endif
#endif /* For not OPCtrans */


/* Build name for registry file */
void reg_name_build(char *file_name, char *user_id);

void
rmt_server_read(void);

/* Find index for specified server site */
int
rmt_server_find(char *site);


short get_rmt_short(void);

void get_timed_short(char *buffer);


short put_rmt_short(short data);

short get_rmt_string(char *buffer);

/* Used to send data to the remote server */
short put_rmt_string(char *string);

short put_rmt_binary_string(
  char *string,
    register int len);


void resend_reg(void);

int strcmp1err(
    char *name1,
    char *name2);

void strip_email_trailors(char *entry);

void rmt_force_reg(
    char *key1,
    char *key2,
    char *key3,
    char *dpt,
    char *par1, 
    char *par2,
    char *par3,
    char *par4,
    char *par5);

void rmt_man_reg(char req_type, char *site, char *email1, char *email2);



/*
 * Host name of LDAP server
 */

/*
 * Port number where LDAP server is running
 */


/*
 * Subtree to search
 */

/*
 * These variables must reside independant of the runnin of the routines.
 * attrs is an array of pointers to the attribute names to search for.
 * value is the value for each attribute.
 */

int whobu_rtv_all(char *user_id, char *commerce_id, char value[USER_ROWS][USER_COLS]);


/* Reformat the phone number if ten digits */
void phone_format(char *phone);

int whobu_reg(
    char *user_email,
    char *user_name1,
    char *user_name2,
    char *user_name3,
    char *user_email1,
    char *user_email2,
    int use_secondary_email);

/* Retrieve user's name and phone number */
/* Name and phone number returned in rtn.msg */
int whobu_name_rtv(char *x500);

int whobu_name(char *x500, char *name);

int whobu_name_phone(char *x500, char *name, char *phone);

int whobu_name_dept(char *x500, char *commerce_id, char *user_id, char *name, char *dept);

int whobu_name_phone_dept(char *x500, char *name, char *phone, char *dept);


/* Verify user is in the X500 registry */
int whobu_user(char *x500, char *commerce_id, char *user_id);
int whobu_check(char *user_id);

int whois_reg(
    char *user_email1,
    char *user_email2,
    char *user_name1,
    char *user_name2,
    char *user_name3,
    REG_PARAMS *reg);

int whois_check(char *user_email);

int x500vfypwd(
    char *user_email,
    char *user_pwd,
    char *user_email1,
    char *user_email2,
    int use_secondary_email);

int strcmp1err(

    char *name1,
    char *name2);

void strip_email_trailors(char *entry);

void strdlt_quotes(char *string);

int bsearch_registry(
    char *user_email1,
    char *user_email2,
    char *registry,
    REG_PARAMS *reg);	/* Holds user's name and Email address if found */


/* Connect to the remote socket */
/* If the connect is unsuccessful */
/* this function puts reason for failure in rtn.msg */
/* It also closes the socket */
short init_rmt_socket(
    char *header_buffer,
    RMT_SERVERS *rmt,
    int indx);		/* Index into the server table */

int whobu2name(char *core_id, char *name);

int test_authenticate(char *user_id, char *password, char *first_name, char *last_name);

int fss_intranet_authenticate(char *user_id, char *password, char *first_name, char *last_name);

/* Called from Intercom server running on SecNet */
/* Used to authenticate FSS customer with LDAP */
/* Uses SSL authentication */
int extranet_authenticate(char *user_id, char *password, char *email, char *name);

/* Used to authenticate FSS customer with LDAP */
/* Uses SSL authentication */
int fss_authenticate(char *user_id, char *password, char *host, int port,
    char *searchbase, char *service_dn, char *service_password, char *first_name, char *last_name);



/* Authenticate with Freescale test server */
int freescale_test_authenticate(char *user_id, char *password, char *corporate_email, char *friendly_email, char *name);

/* Authenticate Freescale user with internal LDAP */
int freescale_authenticate(char *user_id, char *password, char *corporate_email, char *friendly_email, char *name);

/* Authenticate user with internal LDAP */
int intranet_authenticate(char *user_id, char *password, char *host, int port,
    char *searchbase, char *corporate_email, char *friendly_email, char *name,
    char *service_dn, char *service_pwd);

void ldap_name_get(LDAP *ld, LDAPMessage *user_result, char *name);

void ldap_company_get(LDAP *ld, LDAPMessage *user_result, char *company);

int extranet_name_lookup(char *user_id, char *name);

#endif
                       global_report.h                                                                                     0000777 �    �Ԧ0120355 00000002550 12042342513 013430  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_report_h
#define global_report_h

#define FILTER_NUM 11
#ifdef M2
#define DATE_FIELD 8
#define REPORT_ARRAY_SIZE 9
#define USERID_FIELD 5
#define NAME_FIELD 7
#define REPORT_DIR "event_log/"
#define REPORT_SCAN_SIZE 6
#else
#define REPORT_ARRAY_SIZE 20
#define DATE_FIELD 17
#define REPORT_DIR "report_log/"
#define REPORT_SCAN_SIZE 17
#endif


/* General function for building report lists */
void report_list(
    int req_type,
    char *date1, 
    char *date2, 
    char *keep_filter1, 	/* Holds keep_filter index and keep_filter value */
    char *keep_filter2,	/* Holds keep_filter index and keep_filter value */
    char *discard_filter1, 	/* Holds discard_filter index and discard_filter value */
    char *discard_filter2,	/* Holds discard_filter index and discard_filter value */
 char *scan_fields);


void report_header_print(FILE *fo, char *report_title, char *date1, char *date2);


#ifdef TRANSWEB
int report_fields_print(char *buffer, char rep[REPORT_ARRAY_SIZE][256], int fields[]);
#endif

void elapsed_time_convert(char *string);
void size_convert(char *size_string);

void long_time_convert(char *time_string);


/* Pad with spaces a single field */
void report_field_trim(char *field, int index);

int report_fields_scan(char *scan_fields, int fields[]);

int report_title_build(char *string, char *report_title, char *date1, char *date2);

#endif
                                                                                                                                                        global_req_types.h                                                                                  0000777 �    �Ԧ0120355 00000013262 12042342513 014132  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_req_types_h
#define global_req_types_h
/* Data types understood by the DMI database */
typedef enum
{
/* Data types */
    MASK = 'a',
    DSGN = 'b',
    TMPL = 'c',
    PC_LIB = 'd',
    TMPL_DSGN = 'e',
    DSGN_TMPL = 'f',
    DSGN_GROUP = 'g',
    WAFER_LINE = 'h',
    GLBL = 'i',
    VERSION = 'j',
    GLOBAL_TABLES = 'k',
    WL_PC = 'l',
    OFF_TMPL = 'm',
    OFF_DSGN = 'n',
    OFF_TMPL_OFF_DSGN = 'o',
    OFF_TMPL_DSGN = 'p',
    TMPL_OFF_DSGN = 'q',
    OFF_TMPL_AUDIT = 'r',
    OFF_DSGN_AUDIT = 's',
    APP_TMPL_DSGN = 't'
} DATA_TYPE;

/* Transaction types */
typedef enum
{
    UPD_REQ = 'd'  ,
    DLT_REQ = 'i',
    READ_REQ = 'b',
    MOD_REQ = 'c',
    RTV_DATA_REQ= 'q',
    RTV_FILES_REQ = 'r',
    CRT_REQ = 'a',
    REMOTE_REQ = 'e',
    GLOBAL_REQ = 'f',
    VERSION_REQ = 'g',
    APPROVE_REQ = 'h',
    ALIVE_REQ = 'j',
    RELEASE_REQ = 'k',
    NETWORK_REQ = 'l',
    PORT_REQ= 'p',
    REGISTER_REQ = 'o',
    LIST_REQ = 's',
    DB_ADMIN_REQ = 'n',
    RVUPORT_REQ = 'm'
} TRANSACTION;

/* Request types */
typedef enum
{
    /* Update requests */
    DBOVRWR_DATA = 'A'  ,
    DBCHKIN_DATA = 'B' ,
    DBUPD_DATA = 'C' ,
    DBCHKOFF_DATA = 'D' ,
    DBDLT_DATA = 'E' ,

/* Check out and read transactions */
    DBCHKOUT_DATA = 'F' ,
    DBAPP_DATA = 'G' ,

/* Modify transactions */
    DBCANCHKOUT_DATA = 'H' ,
    DBCANAPP_DATA = 'I' ,
    DBTRANSFER_DATA = 'J' ,

/* Read data, retrieve versions */
    DBREAD_DATA = 'K' ,
    DBREAD_FILE = '^' ,
    DBRTV_VERSIONS = 'L' ,
    DBUNAPP_DATA = 'M' ,
    DBBACKOUT_DATA = 'N' ,
    DBDLT_VERSIONS = 'O' ,
    DBPORT_RVU_DATA = 'P' ,

/* Create, copy, and port data */
    /* Action class is create */
    DBCRT_DATA = 'U' ,
    DBCOPY_DATA = 'V' ,
DBCOPY_MASK2TMPL = 'R',
/* Port mask to template, done in Hercules */
    DBPORT_MASK2TMPL = 50,
    /* Port specified data to the database */
    DBPORT_DATA = 'W' ,
    /* Create a group of files with specified data type */
    DBCRT_FILES,
    /* Create a speicified file */
    DBCRT_FILE,

    /* Modify transactions */
    DBMOD_PIN = 'X' ,
    DBMOD_OWNER = 'Y' ,
    /* Grant overwrite for one transaction */
    DBGRANT_OVRWR = 'Z' ,
/* Set C4 mask */
    DBSET_C4MASK = '_' ,
    /* Empty requests for future use */
    DBSET_EMPTY1,
    DBSET_EMPTY2,

    /* Retrieve small pieces of data */
    /* Uses transaction type DBRTV_DATA */
/* Retrieve the mask set PIN */
    DBRTV_PIN = 55,
    DBRTV_CWD = 56,
    /* Retrieve serial number for Mesa Mask Shop */
    DBRTV_MOS6SERIAL = 'n' ,
    DBRTV_MOS16SERIAL = 42 ,
    DBRTV_COM1SERIAL = 59,
/* Retrieve annual contract number */
    DBRTV_CNB = 57,
    DBRTV_CONTRACT_NUM = 'l' ,
/* Retrieve value of C4 mask */
    DBRTV_C4MASK = 58,

    /* Registration requests */
    /* Transaction type of REGISTER_REQ  */
    DBCHK_REG = 60,
    DBAUTO_REG = 61,
    DBMAN_REG = 62,
/*
    DBFORCE_REG = 63,
    DBRMT_MAN_REG = 64,
*/
    DBRMT_FORCE_REG = 39,
/* Resend registration entries */
    DBRESEND_REG = 54,

    /* List requests */
    /* Have transaction type LIST_REQ */
    /* Pull out file with a list of items */
    /* List PC links */
    DBLIST_LINKS = '[' ,
    /* Pull down list of PC libraries */
    DBLIST_PCS = '\\' ,
    /* List masks with specified search criteria */
    DBLIST_MASKS = ']' ,
    /* Send list of all masks */
    DBLIST_ALL_MASKS ,
    /* Send list of templates with specified criteria */
    /* Currently being used by Hercules */
    DBLIST_TMPLS = 127,
   /* Currently not being used */
    DBLIST_SERVERS = 'o' ,
    /* Unused list request */
    DBLIST_EMPTY1,
    DBLIST_EMPTY2,

    /* Retrieve one or more files */
    /* Have transaction type RTV_FILES */
    DBRTV_FILES ,
    DBRTV_FILE,
    EMPTY_REQ = '_' ,

/* Remote request types */
    DBRMTCRT_DATA = 'a' ,
    DBRMTDLT_DATA = 'b' ,
    DBRMTUNAPP_DATA = 35,
    DBMARK_DATA = 'c' ,
    DBREFRESH_DATA = 'd' ,
    DBRMTPORT_RVU = 'e' ,
    DBRMT_AUDIT_DATA = '`' ,

/* PC functions */
    DBADD_LINK = 'f' ,
    DBMOD_LINK = 'g' ,
    DBDLT_LINK = 'h' ,
    DBREPLACE_DATA = 'i' ,
    DBRENAME_DATA = 'j' ,

    DBREAD_CONTROL_GROUP = 'k' ,

    /* Network requests */

    DBRESEND_DATA = 'p' ,
    DBUPD_SERVER = 'q' ,
    DBPING_REQ = 'r' ,
    DBREAD_DIR = 's' ,

    /* Audit actions */
    DBRMT_AUDIT_GLOBAL = 't' ,
    DBAUDIT_DATA = 'u' ,
    DBAUDIT_GLOBAL = 'v' ,
    DBAUDIT_FAB = 'w' ,
    DBAUDIT_DESIGN = 'x' ,
    DBAUDIT_MASK = 'y' ,
    DBAUDIT_PC = 'z' ,
    DBAUDIT_OFF_FAB = 123,
/* Audit the global file entries against the actual directories */
    DBAUDIT_DIR2GLOBAL = 40,
/* Audit the global wafer line and design templates */
    DBAUDIT_WL_DSGN_FILES = 41,
     DBAUDIT_EXTRA_GLOBAL_FILES = 39,
/* Audit entire directory structure */
    DBRMT_AUDIT_DIRECTORIES = 46,
    DBAUDIT_DIRECTORIES = 47,
    DBAUDIT_OFF_DESIGN = 124,
    DBREAD_RVU_DATA = 125,
    DBRMV_DATA = 126,
/* Perform df and ps on database */
    DBPS = 43,
    DBPS_DF = 44,
/* Resend global files to remote servers */
    DBRESEND_FILES = 45,
/* Copy directory to remote servers */
    DBCOPY_RMT_FILES = '0' ,
/* Force data to be shadowed to remote databases */
    DBSHADOW_DATA = 49,

    /* Database administrative functions */
/* Add template builder and template approver */
    DBADD_TMPLBLD = 51,
    DBADD_APPROVER = 52,
    DBADD_TMPLBLD_APPROVER = 53,
    DBRMT_TMPLBLD = 36,
    DBRMT_APPROVER = 37,
    DBRMT_TMPLBLD_APPROVER= 38,
    DBPUT_GLOBAL_FILE = '0' ,
    DBMOD_PASSWORD = '1' 
} REQ_TYPE;


/* Secondary request type used when data type and request type */
/* cannot adequately describe the transaction */
typedef enum
{
    /* Different types of retrieve */
    CNB,
    SERIAL_NUM,
    CONTRACT_NUM,
    cwd
} SEC_REQ_TYPE;
#endif /* ifdef for global_req_types_h */
                                                                                                                                                                                                                                                                                                                                              global_server_errs.h                                                                                0000777 �    �Ԧ0120355 00000053646 12326253254 014502  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_server_errs_h
#define global_server_errs_h
/* The error messages defined in this file are used by all of the DMI databases. */
/* These errors are between -300 and -399 */
#define BAD_GET -112
/* Defines for database errors follow */

#define not_reg "Error -300:    Registration entry not found for '%s'"
#define NOT_REG -300
#define bad_pin "Error -301:    Incorrect PIN entered."
#define BAD_PIN -301
#define bad_arg "Error -302:    Invalid option '%s'\n\n\
               Use '-p' for production, '-t' for test,\n\
               and '-d' for development DMI server.\n\n"
#define BAD_ARG -302
#define bad_rmt_sock_open "Error -304:    Unable to establish communication with remote database server."
#define BAD_RMT_SOCK_OPEN -304
#define bad_rmt_get "Error -305:    Unable to retrieve data from the database server."
#define BAD_RMT_GET -305
#define bad_rmt_put "Error -306:    Unable to send data to the database server."
#define BAD_RMT_PUT -306
#define malloc_err "Error -307:  Unable to allocate dynamic memory."
#define MALLOC_ERR -307
#define bad_rmt_ip_addr "Error -308:  Invalid IP node address for server machine:"
#define BAD_RMT_IP_ADDR -308
#define bad_rmt_connect "Error -309:  Received error code %d while trying to connect to remote server '%s'"
#define BAD_RMT_CONNECT -309
#define rmt_connect_err "Error -310:    Received error code '%d' while trying\n\
               to connect to database server:\n\n\
               Machine:  %s\n\
               Address:  %s\n\n"
#define RMT_CONNECT_ERR -310

/* Database server network errors */
#define put_short_err "Error -311:    Database server received error code '%d'\n\
               while sending length field to the client or remote server."
#define PUT_SHORT_ERR -311
#define put_char_err "Error -312:    Database server received error code '%d'\n\
               while sending character to the client or remote server."
#define PUT_CHAR_ERR -312
#define put_err "Error -313:    Database server received error code '%d'\n\
               while sending data to the client or remote server.\n\n\
               Attempted to send '%d' bytes.\n\
               But sent only '%d' bytes."
#define PUT_ERR -313
#define bad_ack "Error -314:    Database server did not receive final\n\
               acknowledgement from the client."
#define BAD_ACK -314
#define bad_file_ack "Error -315:    Database server did not receive end-of-file\n\
               acknowledgement from the client."
#define BAD_FILE_ACK -315
#define get_short_err "Error -316:    Database server retrieved invalid\n\
               length field of '%d' with error code '%d' from the client."
#define GET_SHORT_ERR -316
#define get_err "Error -317:    Database server received error code '%d'\n\
               while trying to retrieve data from the user."
#define GET_ERR -317
#define bad_rmt_ack "Error -318:  Server unable to retrieve acknowledgement\
               from the remote server."
#define BAD_RMT_ACK -318

#define no_disk_space "Error -319:    There is not enough disk space to perform\
               database transaction."
#define NO_DISK_SPACE -319
#define bad_vnum "Error -320:    Invalid database client version number:\n\n\
               '%s'\n\n\
               Lowest version of client that can run with database server '%s' is '%s'.%s%s"
#define BAD_VNUM -320

/* Invalid data sent by user */
#define bad_req_type  "Error -321:    Invalid request type"
#define BAD_REQ_TYPE -321
#define bad_trans_type  "Error -322:    Invalid transaction type"
#define BAD_TRANS_TYPE -322
#define bad_data_type  "Error -323:    Invalid data type"
#define BAD_DATA_TYPE -323
/* Empty slot -324 */
#define bad_version_flag "Error -325:    Invalid version flag"
#define BAD_VERSION_FLAG -325
#define bad_row_index "Error -326:    Invalid state table row index value"
#define BAD_ROW_INDEX -326
#define bad_col_index "Error -327:    Invalid state table column index value"
#define BAD_COL_INDEX -327
#define bad_state_val "Error -328:    Invalid state table value"
#define BAD_STATE_VAL -328
#define bad_addr "Error -329:    Invalid IP node address:"
#define BAD_ADDR -329
#define bad_password "Error -330:    Internal password corrupted during data transmission.  "
#define bad2password "Error -330:    Internal password corrupted during data transmission.  \n\
               User password:  %s\n\
               Server password:  %s"
#define BAD_PASSWORD -330
/*
#define corrupted_data "Error -331:  Corrupted data found in file:"
#define CORRUPTED_DATA -331
*/
#define checksum_err "Error -332:  Inconsistent check sums found on the %s database for directory:\n\n\
             '%s'"
#define CHECKSUM_ERR -332
#define not_auth_err "Error -333:    You are not authorized to perform database \n\
               administrative functions."
#define NOT_AUTH_ERR -333
#define invalid_password "Error -334:  The password you entered is incorrect."
#define INVALID_PASSWORD -334
#define alr_own_err "Error -334:  %s already owned by:\n\n\
               Site:  '%s'"
#define ALR_OWN_ERR -334
/* Empty slot -335 */

#define origin_blacklist_err "Error -336:    You are not authorized to perform this request."
#define ORIGIN_BLACKLIST_ERR -336

#define pc_flow_err "Error -337:    Process flow %s must be created before %s\n\
               can be created."
#define PC_FLOW_ERR -337
/* Empty slot -338 -339 */

/* Server directory and table of contents  errors */
#define mkdir_err "Error -340:    Received err code %d while trying to create directory:\n\n\
              '%s'"
#define MKDIR_ERR -340
#define dlt_dir_err "Error -341:    Received err code '%d' while trying to delete directory:\n\n\
               '%s'"
#define DLT_DIR_ERR -341
#define open_dir_err "Error -342:    Received err code %d while trying  to open directory:\n\n\
               '%s'"
#define OPEN_DIR_ERR -342
#define bad_dir_name "Error -343:    Invalid directory name of 0 length."
#define BAD_DIR_NAME -343
/* Empty slot 344 */
#define rmt_ovr "\n\nWarning -345:  %s retrieved, but still being updated by the remote\n\
               server.  Thus, you might not have the latest copy of the data.\n\n%s"
#define RMT_OVR -345
#define toc_err "Error -346:    Table of contents entry already present for:"
#define TOC_ERR -346
#define sys_write_err "Error -347:    Unable to execute system command:"
#define SYS_WRITE_ERR -347
#define lock_err "Error -348:    Unable to create lock file:"
#define LOCK_ERR -348
#define unlock_err "Error -349:    Unable to delete lock file:"
#define UNLOCK_ERR -349
#define no_valid_files "Error -349:    No valid files present to send to the database."
#define NO_VALID_FILES -349

/* Server file errors */
#define rd_open_err "Error -350:    Unable to open for read database file:"
#define RD_OPEN_ERR -350
#define wr_open_err "Error -351:    Unable to open for write database file:"
#define WR_OPEN_ERR -351
#define read_err "Error -352:    Unable to read database file:"
/* Currently not being used */
#define READ_ERR -352
#define write_err "Error -353:    Unable to write to database file:"
#define WRITE_ERR -353
#define seek_err "Error -354:  Unable to perform seek on database file:"
#define SEEK_ERR -354
#define rename_err "Error -355:    Unable to rename database file:"
#define RENAME_ERR -355
#define dlt_err "Error -356:    Received error '%d' while deleting file:\n\n\
               '%s'"
#define DLT_ERR -356
/* global_server_errs.h Not currently being used */
#define reg_req "Warning -359:  Requesting registration on %s for:\n\n\
               %IP node address:  %s\n\
               Login:  %s\n\
               Email address:  %s\n\
               User ID:  %s\n\
               Name:  %s\n"
#define REG_REQ -359
#define reg_core_err "Error -358:  Your registration request failed because no entry was  found\n\
             for  you in the Freescale email core directory.  For security\n\
             reasons, all users must be  registered  in  the  email  core\n\
             directory before they can register to use this tool.  If you\n\
             are already registered in the email core  directory,  please\n\
             try  registering  again and be sure to type in your official\n\
             Email address at the prompt.\n\n\
             If you are not  registered  in  the  email  core  directory,\n\
please contact the help desk.\n\n\
             After you are registered in the email  core  directory,  you\n\
             can  register  to use this tool by running the tool.  If you\n\
             have any questions or  problems  with  registration,  please\n\
             contact Transcend team - transbug@nxp.com."
#define REG_CORE_ERR -358
#define reg_match_err "Error -360:  Registration failed because your name in the  password  file\n\
             does  not  match  your  name  in  the  Freescale  email  core\n\
             directory.   For  security  reasons,  the  DMI  registration\n\
             software  requires  these  names to match.  \n\n\
                 Name in password file:  %s %s\n\
                 Name in core email directory:  %s %s\n\n\
             If your name in the password file is incorrect, contact your\n\
             system  administrator  to correct the problem.  If your name\n\
             in the email core directory is  wrong,  contact  your  local\n\
             email  administrator  to  fix  the  problem.   If  you  have\n\
             questions, contact Transcend Team - transbug@nxp.com."
#define REG_MATCH_ERR -360
/* Empty slots    364 365 366 367 368 369 */
#define reg_verify_err "Error -361:  Unable to verify registration for %s."
#define REG_VERIFY_ERR -361
#define auto_reg_err "Error -362:    Unable to perform registration for %s\n\
               on the %s database.\n\n\
               Your registration request has been sent to the\n\
               DMI Database administrator.\n\n\
To insure that your registration request is processed, please\n\
send to Transcend team (transbug@nxp.com) the following information:\n\n\
-   Your full name\n\
-   Your official Freescale ID or badge\n\
-   Your telephone number\n\
-   Your department number\n\
-   Your location code\n\
-   Your mail drop"
#define AUTO_REG_ERR -362
#define man_reg_err "Error -363:    Unable to register user '%s %s'\n\
               on the '%s' database."
#define MAN_REG_ERR -363
#define invalid_subdir "Error -365:  Invalid directory '%s' found"
#define INVALID_SUBDIR -365
#define empty_dir "Error -366:  Empty directory '%s' found"
#define EMPTY_DIR -366
#define extra_entry_err "Error -367:  Entry '%s' found and deleted from '%s'"
#define EXTRA_ENTRY -367
#define accept_err "Error -368:  Received error code %d while attempting to accept a connection."
#define ACCEPT_ERR -368
#define ssl_accept_err "Error -369:  Received error code %d while attempting to accept a connection."
#define SSL_ACCEPT_ERR -369

#define crt_err "Error -380:    %s already exists."
#define CRT_ERR -380
#define non_err "Error -381:    %s does not exist."
#define machine_non_err "Error -381:    %s do not exist."
#define NON_ERR -381
/* Empty slots 382 383 384 385 386 387 388 389 390 */
#define no_archive_space "Error -446:  No disk space present for archive."
#define NO_ARCHIVE_SPACE -446
#define usr_not_fnd "Error -447:    Unable to find registration entry \n\
               for '%s'"
#define USR_NOT_FND -702
#define ftp_connect_err "Error -710:    Received error code %d while connecting to \n\n\
               %s"
#define FTP_CONNECT_ERR -710
#define invalid_reg_format "Error -411:    Invalid format found in registration file '%s\n\
               for line %d which reads '%s'"
#define bad_file_sock "Error -412:  Unable to obtain socket for file transfer."
#define BAD_FILE_SOCK -412
#define INVALID_REG_FORMAT -411
#define reg_entry_wrn "Warning -413:  Neither core ID nor Transcend registration entry\n\
               present for requested user.  No email sent to:"
#define REG_ENTRY_WRN -413
#define ascii_err "Error -414:  Unable to set FTP transfer type to ASCII."
#define ASCII_ERR -414
#define binary_err "Error -415:  Unable to set FTP transfer type to binary."
#define BINARY_ERR -415
#define abort_err "Error -416:    Data transfer aborted for file:\n\n\
               '%s'"
#define ABORT_ERR -416
#define df_err "Error -417:  Unable to determin disk usage using 'df' command"
#define DF_ERR -417
#define disk_space_err "Error -418:  There is not enough room in the %s repository for the data.\n\n\
               File Size:  %d bytes\n\
               Available space:  %d bytes\n\
               Percent used:  %d%%"
#define not_fnd_err "Error -418:    Entry for %s not found."
#define NOT_FND_ERR -418

#define dup_entry "Error -420:    Duplicate entry found for:"
#define DUP_ENTRY -420
#define bad_toc "\n\nCaution -422:    Unable to find and delete:\n\n\
               Global entry:  "
#define BAD_TOC -422
#define own_wrn "Warning -424:  Data base request sent to owner: '"
#define OWN_WRN -424
#define ARCHIVE_WRN -624


#define VERSION_NOT_FND -327

#define bad_len_err "Error -445:  Bad error code of '%d' received from the client or remote server."
#define BAD_LEN_ERR -445
#define dlt_dir_cau "\n\nCaution -444:  Unable to delete directory '%s'\n\
               This directory has been renamed '%s'"
#define DLT_DIR_CAU -444
#define not_owner "Error -414:  You cannot modify data you do not own.  Data is owned by:\n\n\
               Login:  '%s'\n\
               Email address:  '%s'\n\
               IP node Address: '%s'"
#define NOT_OWNER -414

#define no_data_err "Error -425:   No data found in database for version '%2.3f' of:\n\n\
               %s"
#define NO_DATA_ERR -425
#define timeout_err "Error -426:  Timed out while trying to connect to remote server."
#define TIMEOUT_ERR -426
#define mask_present \
"Error -480:    %s is already present in the MaskPrep database.\n\
               You must disable the maskset in the MaskPrep database before\n\
               creating it in the Mask Designer database.  Use the 'Disable Mask\n\
               Set' function found under the 'MaskPrep Access' menu!"
#define MASK_PRESENT -480
#define invalid_user_id "Error -485:    Invalid Freescale user ID of '%s' entered.  Freescale\n\
               user ID's must be 5 or more characters in length."
#define INVALID_USER_ID -485
#define date_err "Error -486:    Invalid date entered.  Dates must\n\
               follow the format mm/dd/yy."
#define DATE_ERR -486
#define month_range_err "Error -487:    Specified month '%0.2d' out of range.\n\
               Valid months are 01-12."
#define MONTH_RANGE_ERR -487
#define day_range_err "Error -488:    Specified day '%0.2d' out of range.\n\
               Valid days are 01-31."
#define DAY_RANGE_ERR -488
#define year_range_err "Error -489:    Specified day '%0.2d' out of range.\n\
               Valid days are 01-31."
#define YEAR_RANGE_ERR -489
#define year_err "Error -490:    The system currently only allows reports \n\
               with the same year on both the start and end date."
#define YEAR_ERR -490
#define bad_dates "Error -491:  NO data for specified dates found."
#define BAD_DATES -841
#define month_err "Error -492:  Ending month must be greater than or equal\n\
               to starting month.\n\n\
               Starting month:  %0.2d\n\
               Ending month:  %0.2d"
#define MONTH_ERR -492
#define YEAR_RANGE_ERr -839
#define no_weekly_data "Error -493:  No data present for specified dates."
#define NO_WEEKLY_DATA -493
#define delete_err "Error -495:    Received error %d while trying to delete:\n\n\
               %s"
#define DELETE_ERR -495
#define fss_err "Error -496:  You are not authorized to transfer, retrieve, or access  Foundry Services Customer data"
#define FSS_ERR -496

#define no_data "Error -725:  No files found for specified version of data."
#define NO_DATA -725
#define    authen_err "Error -726:    OneIT authentication failed."
#define AUTHEN_ERR -726
#define convert_ovr "Override -727: Received error code %d while trying to convert \n\
               data to specified version.\n\n\
                Press 'Continue' to retrieve unusable data for evaluation.\n\
                Press 'Cancel' to abort."
#define CONVERT_OVR -727
#define chmod_err "Error -728:  Unable to set execute permission on job script"
#define CHMOD_ERR -728
#define convert_err "Error -729:  %s was not converted correctly \n\
               and this data is unusable.  Conversion\n\
               output is stored in file '.db/convert.output'"
#define CONVERT_ERR -729
#define convert_wrn "Warning -730:  Received return status %d while trying to convert \n\
               data to specified version."
#define CONVERT_WRN -730
#define pclose_err "Error -731:  Received error %d while closing tar command"
#define PCLOSE_ERR -731
#define decrypt_err "Error -732:    Unable to decrypt file:"
#define DECRYPT_ERR -732
#define file_sent_err \
"Error -733:    File '%s' has already been sent to the\n\
               mask shop.  Duplicate file names cannot be sent.  Because of\n\
               this problem, data transfer to the mask shop was aborted."
#define FILE_SENT_ERR -733
#define no_version_history "Error -734:  No version history present for %s"
#define NO_VERSION_HISTORY -734
#define bad_link "Error -735:  Received error code %d while linking %s to %s"
#define BAD_LINK -735
#define tech_err "Error -736:  Technology %s is not defined"
#define TECH_ERR -736
#define sql_err "Error -737:    Received error code %d from mySql query:\n\n\
               '%s'\n\n\
               Error string returned is:\n\n\
               '%s'"
#define SQL_ERR -737
#define mask_project_list_err "Error -738:    Unable to find project name for mask set '%s'"
#define MASK_PROJECT_LIST_ERR -837
#define mwr_signoff_err "Warning -739:  Final sign off not completed for MWR number '%s'\n\n"
#define MWR_SIGNOFF_ERR -739
#define no_dtms_signoff_wrn "Warning -740:  No DTMS projects defined for mask set '%s'.\n\n"
#define command_open_err "Error -741:    Received error code %d while executing command:\n\n\
               '%s'"
#define COMMAND_OPEN_ERR -741
#define mwr_signoff_done "Final sign off for MWR number %s was completed on %s.\n\n"
#define dtms_not_needed "DTMS sign off not needed for this mask set."
#define DTMS_NOT_NEEDED -741
#define certificate_err "Error -742:    Unable to  read certificate file:\n\n\
               %s"
#define CERTIFICATE_ERR -742
#define key_file_err "Error -743:   Received error code '%d' while reading certificate file:\n\n\
               '%s'"
#define KEY_FILE_ERR -743
#define ca_list_err "Error -744:  Unable to  read CA list"
#define CA_LIST_ERR -744
#define expired_core_id \
"Error -745:    The authentication token for your e-mail address has expired.\n\
               Please perform authentication again to renew the token."
#define EXPIRED_CORE_ID  -745
#define extract_err \
"Error -980:  Unable to extract data_transfer registry"
#define EXTRACT_ERR -980
#define domain_approve_err \
"Error -981:    Approval request not performed because the domain for\n\
               '%s' is already approved."
#define DOMAIN_APPROVE_ERR -981
#define pending_request_err \
"Error -982:    Pending request not present for:\n\n\
               '%s'"
#define PENDING_REQUEST_ERR -982
#define key_mismatch_err \
"Error -983:    The file keyword does not match the secure keyword:\n\n\
               Secure keyword:  %s\n\
               File keyword:  %s"
#define KEY_MISMATCH_ERR -983
#define unsupported_transaction_err \
"Error -984:    this transaction is not supported for external customers."
#define UNSUPPORTED_TRANSACTION_ERR -984
#define already_approved_err \
"Error -985:  '%s' is already approved."
#define ALREADY_APPROVED_ERR -985
#define bad_encrypt_size \
"Error -986:    Transfer was aborted because the encrypted file \n\
               is 0 bytes in length."
#define BAD_ENCRYPT_SIZE -986
#define approver_mismatch_err \
"Error -987:    The approver Core ID does not match the registered Core ID:\n\n\
               Approver Core ID:  %s\n\
               Registered Core ID:  %s"
#define APPROVER_MISMATCH_ERR -987
#define version_err  \
"Error -988:  This version of Transcend is no longer supported.  Please \n\
               use the latest version present in /usr/fsl/bin."
#define VERSION_ERR -988
#define version_wrn \
"\n\nWarning -989:  This version of Transcend is no longer supported\n\
               and will be retired on %s.  Please upgrade to the\n\
               latest Transcend release present in /usr/fsl/bin."
#define VERSION_WRN -989
#define domain_not_approved_err \
"Error -990:    Approval was not revoked because \n\
               domain '%s' is not approved."
#define DOMAIN_NOT_APPROVED_ERR -990
#define user_not_approved_err \
"Error -991:    Approval was not revoked because \n\
               '%s' is not approved."
#define USER_NOT_APPROVED_ERR -991
#define domain_blacklist_err  \
"Error -992:    This transaction was aborted because \n\
               domain '%s' is black listed."
#define DOMAIN_BLACKLIST_ERR -992
#define user_blacklist_err  \
"Error -993:    This transaction was aborted because \n\
               user '%s' is black listed."
#define USER_BLACKLIST_ERR -993
#define timestamp_err "Error -995:    The approval timestamp is less than the current time.\n\
               Current time:  %ld\n\
               Approval time:  %ld\n"
#define not_reg_err "Error -994:    Your transaction was aborted because you are not registered."
#define NOT_REG_ERR -994
#define TIMESTAMP_ERR -995
#define not_approved_err  \
"Error -996:    Data retrieval was aborted because you are not approved to\n\
               retrieve data from Freescale.  You must be registered and\n\
               approved to retrieve data from Freescale.  Please contact\n\
               %s (%s) concerning this\n\
               problem.  %s's manager can perform approval.\n\
               For additional details about the approval process, retrieve\n\
               documentation using Intercom by typing 'icom -r approval.txt'."
#define NOT_APPROVED_ERR -996
#define blacklist_err "Error -997:  Your transaction was aborted because you are black listed."
#define BLACKLIST_ERR -997
#define no_customer_approvals "Error -998: No pending customer approval requests present."
#define NO_CUSTOMER_APPROVALS -998
#define no_commandline_approvals "Error -999: No command line approval requests present."
#define NO_COMMANDLINE_APPROVALS -999
#endif /* ifdef for global_server_errs_h */
                                                                                          global_server_params.h                                                                              0000777 �    �Ԧ0120355 00000001216 12042342513 014764  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_server_params_h
#define global_server_params_h
#include "global_params.h"

/* Holds program name, version number, and password */
extern DB_SOFTWARE so;

/* Holds global parameters used only by the server */
extern TRANSACTION_PARAMS tr;

/* Holds temporary buffer space used by the database manager */
extern TMP_BUFFER bf;

int sock;

/* Indicates if email error message   sent for the current transaction */
/* Only one email error  message can be sent for each transaction */
extern int email_cnt;

/* Indicates how many log entries made for the current transaction */
extern int log_cnt;
#endif /* ifdef for global_server_params_h */
                                                                                                                                                                                                                                                                                                                                                                                  global_server_prototypes.h                                                                          0000777 �    �Ԧ0120355 00000013566 12042342513 015744  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_server_prototypes_h
#define global_server_prototypes_h

#include <stdio.h>

#define EQUAL '='
#define NOT_EQUAL '!'

/* Following structure used when creating file names within date range */
typedef struct
{
    int day;
    int month;
    int year;
} FILE_DATE;

int line_get(FILE *fp, char *line);

int long_line_get(FILE *fp, char *line);

/* Logs current transaction */
/*
int log(
     char *log_file,
       char *string);
*/

int data_log(
     char *log_file,	/* Holds name of log file where string will be put */
       char *string);

void log3(
    char *str1, char *str2, char *str3);

/* Build the log file name */
void bld_log_name(void);

/* The following allows me to use month '01' instead of 'Jan' */
void log_convert(char *log_date, char *time_stamp);

/* Encrypt client/server password */
/* Encryption is symetrical */
void password_encrypt(char *new_password, char *old_password, int len);

/* Build the user's site */
void get_site(char *ip_addr, char *site);

/* Send email using SMTP */
void email_smtp_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *body);

/* Send email using sendmail */
void email_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *body);

void email_file_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *file);

void email_msg_file_send(
    char *replyto,
    char *recipients,
    char *subject,
    char *msg,
    char *file);

/* Sends error message to user and system administrator */
/* Also exits current transaction */
void email_send_user(char *user_email, char *subject, char *body, int errno);

void jdc_sendmail(
    char *sender,
    char *replyto,
    char *recipients,
    char *subject,
    char *body);

/* Build a time stamp consisting of date and time */
/* Time stamp returned in 'time_stamp' variable */
void time_stamp_build(char *time_stamp);

/* Build names of all files that fall within date range */
int names_build(char *date1, char *date2, char *files[]);

int date_calc(FILE_DATE *d, char *date);

/* Scan through the file offset number of lines and log new message after offset */
int data_log_offset(char *log_file, char *string, int offset);

/* Log error for put short or put string */
void log_err(int ret, char *calling_functions);
void shut_down(char *msg, short ret_code);

void clean_up(char *msg);
void lock_files_init(void);

/* Lock specified file */
/* Allocate space for the lock file name and increment index */
int lock_file_set(char *file_name);

/* Unlock file and free memory */
void lock_file_clear(int index);

/* Unlock all files locked and free memory */
/* Called when exiting with error condition */
void lock_files_clear(void);

/* Wait to lock file for specified number of seconds */
int lock_file_wait(char *file_name, int wait_time);

/* Initialize socket on start-up */
int init_socket(int socket);

void startup_daemon(void);

int ip_addr_copy(char *string, char *address);

int ip_addr_lookup(
    char *string,
    char *address,
    char *name);

/* Looks up IP node address using gethostbyname */
/* Used to get IP node address of client machine */
/* If not found, uses default address */
void ip_addr_find(char *addr, char *name);

int add_entry(char *file_name, char *entry);
int dlt_entry(char *file_name, char *entry);

/* convert month i.e. Jan goes to 01 */
void month_convert(char *log_date, char *time_stamp);


/* Get a list of file list */
int list_put(char *files[]);

/* Send list of list to client */
int list_get(char *files[]);

/* Sends list but does not free list members */
/* Returns number of items sent */
int static_list_get(char *files[]);

/* Read list of list from specified file */
/* Returns number of elements found */
int list_read(char *file_name, char *list[]);

int x500list_read(char *file_name, char *list[]);

/* Write list of list to specified file */
int list_write(char *file_name, char *list[]);

/* Add an entry to the list */
/* Names are kept in alphabetical order */
/* Returns 1 if entry added */
/* Returns 0 if current entry replaced */
/* Returns SUCCESS if name added */
int m2list_add(char *list[], char *entry, char *key, int total_cnt);

/* Delete name from  list */
/* Returns SUCCESS if name deleted */
/* Returns FAILURE if name not present */
int m2list_delete(char *list[],  char *key, int total_cnt);

/* Verify specified name is in the list array */
int list_find(char *list[], char *key);

/* Look for element in list using binary search */
/* Returns SUCCESS if element found */
int list_binary_search(char *list[], char *name, int num);

/* Add the user's name before adding to the list */
int userlist_add(char *list[], char *key, int total_cnt);


/* Expand a technology list */
int tech_list_expand(char *dir, char *old_files[], char *new_files[]);

/* Convert list from string format to list format */
int string2list_convert(char *string, char *list[]);

/* convert list to string */
void list2string_convert(char *list[], char *string);

/* Expand the list */
void list_whobu_expand(char *list[]);

/* Put items on list in reverse order */
void m2list_reverse (char ***new_list, char **old_list, int num);

/* Compare elements in two lists */
/* Returns FAILURE if number of elements in two lists different */
/* Also returns FAILURE  if any two elements not the same */
int list_compare(char *list1[], char *list2[]);

/*Free members of list and set first element to NULL */
void m2list_free(char *list[]);

/* Generate usage and summary reports */
void report_usage(int req_type, char *time1, char *time2, char *fields);

/* Decrypt/encrypt user password */
int password_crypt(char *new_pwd, char *old_pwd);

/* SEnd log for previous day for transactions  for Intercom, Transcend, MARS, and Event_log */
void mettracks_log(char *log_prefix);

int entry_find(char *file_name, char *entry);

/* Free list of files */
void files_free(char *files[]);

int remote_email_send(void);

int add_line(char *file_name, char *entry);

#endif
                                                                                                                                          global_sql_lib.h                                                                                    0000777 �    �Ԧ0120355 00000001254 12223612465 013551  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef sql_lib_h
#define sql_lib_h

#include <mysql.h>


/* Get result from an SQL query */
MYSQL_RES * result_get(char *site, char *db, char *query1, char *query2);

int insert_data(char *site, char *db, char *query, int report_err_flag);

/* Initialize a connection in preparation for multiple queries */
int sql_connect(MYSQL *sql, char *db, char *dtms_db);

/* Perform a query to an open connection */
int sql_query(MYSQL *sql, char *query);

/* Retrieve name of projects associated with a mask set */
int sql_mask_projects_rtv(char *mask, char *projects[]);

/* Get DTMS status for a project */
int sql_dtms_status_rtv(char *project, char *mwr_num, char *project_date);

#endif
                                                                                                                                                                                                                                                                                                                                                    hash.h                                                                                              0000777 �    �Ԧ0120355 00000002245 12172312506 011524  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef hash_h
#define hash_h

#include "zlib.h"
#include "trans_defines.h"

gzFile trans_open ( char *mode, long long hash_size, int hash_enable, TRANS_FILE_TYPE file_type); 

int trans_read (gzFile file, voidp buf, char get_header); 

/* ------------------------------------------------------------ trans_write */
void trans_write (gzFile file, const voidp buf, unsigned len); 

/* ------------------------------------------------------------ trans_close */
long long trans_close (gzFile file , char file_type); 

/* ---------------------------------------------------------------- num_bytes */
char* num_bytes (long long bytes); 

/* ---------------------------------------------------------- trans_hash_init */
void trans_hash_init (long long fsize, TRANS_FILE_TYPE file_type, 
char *mode, int parallel); 

/* --------------------------------------------------------------- trans_hash */
long long trans_hash (int new_bytes_xfered, int transfer_type); 

void trans_hash_close(void);

long long transweb_hash (int new_bytes_xfered, FILE *fp_output); 

/* Print estimated file size */
void total_size_print(long long fsize, char *mode, TRANS_FILE_TYPE  file_type, int parallel);

#endif
                                                                                                                                                                                                                                                                                                                                                           line.h                                                                                              0000777 �    �Ԧ0120355 00000000213 12042342513 011516  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef line_h
#define line_h

void
line_add(
    char *global_path,
    char *line,
    char *file_name);

#endif /* define for line_h */
                                                                                                                                                                                                                                                                                                                                                                                     lock_file.h                                                                                         0000777 �    �Ԧ0120355 00000001655 12042342513 012531  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef lock_file_h
#define lock_file_h

#define TOTAL_LOCK_FILES 10
int lock_index;

char *lock_files[TOTAL_LOCK_FILES];

/* Initialize index to be 0 */
/* Point all lock files to NULL */
void lock_files_init(void);

/* Lock specified file */
/* Allocate space for the lock file name and increment index */
int lock_file_set(char *file_name);

/* Unlock file and free memory */
void lock_file_clear(int index);

/* Unlock all files locked and free memory */
/* Called when exiting with error condition */
void lock_files_clear(void);
/* Tries to access the file specified number of times */
/* If the PID for the start period is the same as for the end time */
/* assumes process has aborted and locks the file */
/* If the PID is not the same, gives error */
int lock_file_wait(char *lock_file, int wait_time);

int lock_file_read(char *lock_file);

/* Change the PID stored in the lock file */
int lock_file_mod(char *lock_file);

#endif
                                                                                   mask_msg.h                                                                                          0000777 �    �Ԧ0120355 00000004603 12042342513 012377  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef mask_msg_h
#define mask_msg_h

#include "global_defines.h"

/* Holds definition for mask message */
/* This structure is used by the Mask Prep server */
/* and the Archive server */
typedef struct
{
    char transaction_type;
    char req_type;
    char sec_req_type;
    char data_type;
    char sec_data_type;
    float vnum;
    float sec_vnum;
    char version_flag;
    char sec_version_flag;
    char override_flag; /* Indicates whether transaction continues under abnormal conditions */
    char key1[KEY_LEN]; /* Primary key for data accessing */
    char key2[KEY_LEN]; /* Secondary key for data accessing */
    char key3[KEY_LEN]; /* Key for data accessing */
    char par1[PAR_LEN];	/* First extra parameter, Holds miscellaneous data */
    char par2[PAR_LEN];	/* Second extra parameter */
    char par3[PAR_LEN];	/* Third extra parameter */
    char par4[512];	/* Fourth extra parameter */
    char par5[PAR_LEN];	/* Fifth extra parameter */
    long transaction_num;
char swd[PATH_SIZE];		/* Current working directory */
    char pin1[MAX_PIN_LEN+1];	/* Primary PIN */
char pin2[MAX_PIN_LEN+1];	/* Secondary PIN number */
    char password[32];
} MASK_MSG;

extern MASK_MSG mask_msg;

#ifdef ANSI_C
void
mask_msg_init(
    MASK_MSG *mask_msg,
    char transaction_type,
    char req_type,
    char data_type,
    char *key1,
    char *key2,
    char *key3);

/* Initializes the mask structure */
/* Takes values from data passed in */
void
mask_msg_init_all(
    MASK_MSG *mask_msg,
    char transaction_type,
    char req_type,
    char sec_req_type,
    char data_type,
    char *key1,
    char *key2,
    char *key3,
    char *par1,
    char *par2,
    char *par3,
    char *par4,
    char *par5);

/* Initializes mask message structure using default values */
void
mask_msg_init_default(MASK_MSG *mask_msg);

short
mask_msg_send(
    MASK_MSG *mask_msg,
    int index);

void
mask_header_init(
    char *mask_header,
    char remote_req_type,
    char remote_transaction_type,
    char *mask,
    char *partition);

/* Resends a mask message to the specified remote server */
short
mask_msg_resend(
    MASK_MSG *mask_msg,
    int index);

/* Used by archive server to send mask messages */
short
mask_msg_archive_send(
    char transaction_type,
    char req_type,
    char data_type,
    char *key1,
    char *key2,
    char *master_site);

#endif /* define for ANSI_C */

#endif /* ifdef for mask_msg_h */
                                                                                                                             mask_pin.h                                                                                          0000777 �    �Ԧ0120355 00000000447 12042342513 012401  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef mask_pin_h
#define mask_pin_h

#include "global_defines.h"

typedef struct
{
    char pin1[MAX_PIN_LEN+1];
    char pin2[MAX_PIN_LEN+1];
} MASK_PIN;

extern MASK_PIN pin;

void
mask_pin_set(
    char *pin1,
    char *pin2);

void
mask_pin_init(void);

#endif /* Define for mask_pin_h */
                                                                                                                                                                                                                         mask_req_types.h                                                                                    0000777 �    �Ԧ0120355 00000014104 12042342513 013621  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef global_req_types_h
#define global_req_types_h
/* Data types understood by the DMI database */
typedef enum
{
/* Data types */
    MASK = 'a',
    DSGN = 'b',
    TMPL = 'c',
    PC_LIB = 'd',
    TMPL_DSGN = 'e',
    DSGN_TMPL = 'f',
    DSGN_GROUP = 'g',
    WAFER_LINE = 'h',
    GLBL = 'i',
    VERSION = 'j',
    GLOBAL_TABLES = 'k',
    WL_PC = 'l',
    OFF_TMPL = 'm',
    OFF_DSGN = 'n',
    OFF_TMPL_OFF_DSGN = 'o',
    OFF_TMPL_DSGN = 'p',
    TMPL_OFF_DSGN = 'q',
    OFF_TMPL_AUDIT = 'r',
    OFF_DSGN_AUDIT = 's',
    APP_TMPL_DSGN = 't'
} OLD_DATA_TYPE;

/* Transaction types */
typedef enum
{
    UPD_REQ = 'd'  ,
    DLT_REQ = 'i',
    READ_REQ = 'b',
    MOD_REQ = 'c',
    RTV_DATA_REQ= 'q',
    RTV_FILES_REQ = 'r',
    CRT_REQ = 'a',
    REMOTE_REQ = 'e',
    GLOBAL_REQ = 'f',
    VERSION_REQ = 'g',
    APPROVE_REQ = 'h',
    ALIVE_REQ = 'j',
    RELEASE_REQ = 'k',
    NETWORK_REQ = 'l',
    PORT_REQ= 'p',
    REGISTER_REQ = 'o',
    LIST_REQ = 's',
    DB_ADMIN_REQ = 'n',
    RVUPORT_REQ = 'm',
    CRON_REQ = 't',
    OPC_REQ = 'u'
} TRANSACTION;

/* Request types */
typedef enum
{
    /* Update requests */
    DBM2READ = 61,
    DBM2CHECKOUT = 62,
    DBM2CONVERT = 63,
    DBM2CANCHECKOUT = 64,
    DBOVRWR_DATA = 'A'  ,
    DBCHKIN_DATA = 'B' ,
    DBUPD_DATA = 'C' ,
    DBCHKOFF_DATA = 'D' ,
    DBDLT_DATA = 'E' ,

/* Check out and read transactions */
    DBCHKOUT_DATA = 'F' ,
    DBAPP_DATA = 'G' ,

/* Modify transactions */
    DBCANCHKOUT_DATA = 'H' ,
    DBCANAPP_DATA = 'I' ,
    DBTRANSFER_DATA = 'J' ,

/* Read data, retrieve versions */
    DBREAD_DATA = 'K' ,
    DBREAD_FILE = '^' ,	/* 94 */
    DBRTV_VERSIONS = 'L' ,
    DBUNAPP_DATA = 'M' ,
    DBBACKOUT_DATA = 'N' ,
    DBDLT_VERSIONS = 'O' ,
    DBPORT_RVU_DATA = 'P' ,
    DBRESTORE1MASK = 22,
DBLOCK4ARCHIVE = 'Q',
DBCOMPLETE_ARCHIVE = 'R',
DBM2DISABLE = 'S',
DBM2ENABLE = 'T',
DBSEND_DAILY = 19,
    DBSEND_MONTHLY = 20,

/* Create, copy, and port data */
    /* Action class is create */
    DBCRT_DATA = 'U' ,
    DBCOPY_DATA = 'V' ,
DBCOPY_MASK2TMPL = 'R',
/* Port mask to template, done in Hercules */
    DBPORT_MASK2TMPL = 50,
    /* Port specified data to the database */
    DBPORT_DATA = 'W' ,

    /* Modify transactions */
    DBMOD_PIN = 'X' ,
    DBMOD_OWNER = 'Y' ,
    /* Grant overwrite for one transaction */
    DBGRANT_OVRWR = 'Z' ,
/* Set C4 mask */
    DBSET_C4MASK = '_' ,	/* 95 */
    DBRTV_MASK_FILES = 'a',
    DBRTV_MASKSET_NAME = 'b',
    DBRTV_PC_NAME = 'c',
    DBRTV_H_NAME = 'd',
    /* Empty requests for future use */

    /* Retrieve small pieces of data */
    /* Uses transaction type DBRTV_DATA */
/* Retrieve the mask set PIN */
    DBRTV_PIN = 55,
    DBRTV_CWD = 56,
    /* Retrieve serial number for Mesa Mask Shop */
    DBRTV_MOS17SERIAL = 33 ,
    DBRTV_MOS21SERIAL = 'm' ,
    DBRTV_MOS6SERIAL = 'n' ,
    DBRTV_MOS16SERIAL = 42 ,
    DBRTV_COM1SERIAL = 59,
/* Retrieve annual contract number */
    DBRTV_CNB = 57,
    DBRTV_CONTRACT_NUM = 'l' ,
/* Retrieve value of C4 mask */
    DBRTV_C4MASK = 58,

    /* Registration requests */
    /* Transaction type of REGISTER_REQ  */
    DBCHK_REG = 60,
    DBAUTO_REG = 61,
    DBMAN_REG = 62,
    DBFORCE_REG = 63,
    DBRMT_MAN_REG = 64,
    DBRMT_FORCE_REG = 24,
    DBMOVE2NEW = 23,
/* Resend registration entries */
    DBRESEND_REG = 54,

    /* List requests */
    /* Have transaction type LIST_REQ */
    /* Pull out file with a list of items */
    /* List PC links */
    DBLIST_LINKS = '[' ,
    /* Pull down list of PC libraries */
    DBLIST_PCS = '\\' ,
    /* List masks with specified search criteria */
    DBLIST_MASKS = ']' ,
    /* Send list of all masks */
    DBLIST_ALL_MASKS ,
    /* Send list of templates with specified criteria */
    /* Currently being used by Hercules */
    DBLIST_TMPLS = 127,
   /* Currently not being used */
    DBLIST_SERVERS = 'o' ,
    /* Unused list request */
    DBLIST_EMPTY1,
    DBLIST_EMPTY2,

    /* Retrieve one or more files */
    /* Have transaction type RTV_FILES */
    DBRTV_FILES ,
    DBRTV_FILE,
    EMPTY_REQ = '_' ,

/* Remote request types */
    DBRMT_COMPLETE_ARCHIVE = 25,
    DBRMT_AUDIT_DATA =  26,
    DBRMTCRT_DATA = 27 ,
    DBRMTDLT_DATA = 28 ,
    DBMARK_DATA = 29 ,
    DBREFRESH_DATA = 30 ,
    DBRMTPORT_RVU = 31 ,

/* PC functions */
    DBADD_LINK = 'f' ,
    DBMOD_LINK = 'g' ,
    DBDLT_LINK = 'h' ,
    DBREPLACE_DATA = 'i' ,
    DBRENAME_DATA = 'j' ,

    DBREAD_CONTROL_GROUP = 'k' ,

    /* Network requests */

    DBRESEND_DATA = 'p' ,
    DBUPD_SERVER = 'q' ,
    DBPING_REQ = 'r' ,
    DBREAD_DIR = 's' ,

    /* Audit actions */
    DBRMT_AUDIT_GLOBAL = 't' ,
    DBAUDIT_DATA = 'u' ,
    DBAUDIT_GLOBAL = 'v' ,
    DBAUDIT_FAB = 'w' ,
    DBAUDIT_DESIGN = 'x' ,
    DBAUDIT_MASK = 'y' ,
    DBAUDIT_PC = 'z' ,
    DBAUDIT_OFF_FAB = 123,
/* Audit the global file entries against the actual directories */
    DBAUDIT_DIR2GLOBAL = 40,
/* Audit the global wafer line and design templates */
    DBAUDIT_WL_DSGN_FILES = 41,
     DBAUDIT_EXTRA_GLOBAL_FILES = 39,
/* Audit entire directory structure */
    DBRMT_AUDIT_DIRECTORIES = 46,
    DBAUDIT_DIRECTORIES = 47,
    DBAUDIT_OFF_DESIGN = 124,
    DBREAD_RVU_DATA = 125,
    DBRMV_DATA = 126,
/* Perform df and ps on database */
    DBPS = 43,
    DBPS_DF = 44,
/* Resend global files to remote servers */
    DBRESEND_FILES = 45,
/* Copy directory to remote servers */
    DBCOPY_RMT_FILES = '0' ,
/* Force data to be shadowed to remote databases */
    DBSHADOW_DATA = 49,

    /* Database administrative functions */
/* Add template builder and template approver */
    DBADD_TMPLBLD = 51,
    DBADD_APPROVER = 52,
    DBADD_TMPLBLD_APPROVER = 53,
    DBRMT_TMPLBLD = 36,
    DBRMT_APPROVER = 37,
    DBRMT_TMPLBLD_APPROVER= 38,
    DBPUT_GLOBAL_FILE = '0' ,
    DBMOD_PASSWORD = '1' ,

	    /* Extra requests */
    DBMAKE_TMP_DIR = 34,
    DBRMTUNAPP_DATA = 35
} REQ_TYPE;


/* Secondary request type used when data type and request type */
/* cannot adequately describe the transaction */
typedef enum
{
    /* Different types of retrieve */
    CNB,
    SERIAL_NUM,
    CONTRACT_NUM,
    cwd
} SEC_REQ_TYPE;
#endif /* ifdef for global_req_types_h */
                                                                                                                                                                                                                                                                                                                                                                                                                                                            mask_server_macros.h                                                                                0000777 �    �Ԧ0120355 00000004060 12042342513 014460  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef mask_server_macros_h
#define mask_server_macros_h
#define STATE_INPUT "%d %s %s %d %d %d %s %s %s %s %s %s %s %s %s %s %s %d %f %d %s"
#define STATE_AUDIT_INPUT "%d %s %*s %d %d %d %s %s %s %s %s %s %s %s %s %s %s %d %f %d %s"
#define STATE_CWD_INPUT "%*d %*s %*s %*d %*d %*d %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s"
#define STATE_PIN_INPUT "%*d %*s %*s %*d %*d %*d %*s %s"
#define STATE_OWNER "%*d %*s %*s %*d %*d %*d %*s %*s %*s %s"
#define C4MASK_INPUT "%*d %*s %s"
#define SITE_INPUT "%*d %*s %*s %*d %*d %*d %s"
#define STATE5_INPUT "%*d %*s %*s %d %*d %*d %s"
#define AUDIT_INPUT "%d %*s %*s %d %*d %*d %s %*s %*s %*s %*s %*s %*s %s %*s %s %*s %d %*f %*d %*s"
#define AUDIT_TMPL_INPUT "%d %*s %s %d %*d %*d %s %*s %*s %*s %*s %*s %*s %s %*s %s %*s %d %*f %d %s"
#define TMPL_INPUT "%d %s"
#define PC_INPUT "%*d %s"
#define PIN_INPUT "%*d %*s %*s %*d %*d %*d %s %s"
#define DSGN_INPUT "%d"
#define STATE3INPUT "%*d %*s %*s%*d %*d %*d %*s %*s %*s %*s %*s %*s %*s %s %s %s"
#define STATE4INPUT "%d %*s %*s %*d %*d %*d %*s %*s %*s %*s %s %*s %s"
#define STATE2INPUT "%*d %s %s %d %d %d %s %s %s %s %s %s %s %s %s %s %s %d %*f %d %s"

#define STATE_OUTPUT "%d %s %s\n%d %d %d %s %s\n%s %s %s %s %s\n\
%s %s %s %s\n%d %2.3f     \n%d %s\n"

#define RENAME_DIR(dir1, dir2) \
    if (rename(dir1, dir2) != SUCCESS) \
    {\
	dlt_dir(dir2);\
	if ((rename(dir1, dir2)) != SUCCESS)\
	close_down2q(rename_err, dir1, dir2, RENAME_ERR);\
    }

#define RENAME_FILE(file1, file2) \
    if (rename(file1, file2) != SUCCESS) \
    { \
	unlink(file2); \
	if (rename(file1, file2) != SUCCESS) \
	close_down2m(rename_err, file1, file2, RENAME_ERR); \
    }

#define MAKE_DIR(dir) \
{ \
    if (mkdir(dir, 0700) != SUCCESS) \
    { \
	dir_remove(dir); \
	if (mkdir(dir, 0700) != SUCCESS) close_down1q(mkdir_err, dir, MKDIR_ERR); \
    } \
}

#define MAKE_SOURCE_DIR(dir) \
{ \
    if (mkdir(dir, 0700) != SUCCESS) \
    { \
	dir_remove(dir); \
	if (mkdir(dir, 0700) != SUCCESS) close_down1q(mkdir_err, dir, MKDIR_ERR); \
	fl.source_flag = DBYES;\
    } \
}
#endif /* ifdef for mask_server_macros_h */
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                maskshop_prototypes.h                                                                               0000777 �    �Ԧ0120355 00000004057 12042342513 014736  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef maskshop_prototypes_h
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 mmbdsql.h                                                                                           0000777 �    �Ԧ0120355 00000014771 12042342513 012244  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef mmbdsql_h
#define mmbdsql_h

/* This file contains definitions required to interface MMBD software to
 * the MySQL interface library.
 *
 * This is a joint development effort between Renee Carter and Pavel Ivanov.
 *    Renee Carter - development of MySQL Interface code.
 *    Pavel Ivanov - development of XL reading and MDS Implemention.
 *
 * Copyright: 11 November 2008
 */

#include <mysql.h>

/* This enum defines the sheets of the MMBD that are captured in MySQL. */
typedef enum mmbdSheetNumber{
    MMBD_SHEET_BOOLEANS,
    MMBD_SHEET_SIZING,
    MMBD_NumMMBDSheets,       /* Real sheets must go above this entry */
    MMBD_SHEET_NONE,
    MMBD_SHEET_UNKNOWN
} mmbdSheetType;

/* This enum defines each record in the Boolean Sheet.
 *
 *    Each element is stored as a string. This allows maximum flexibility.
 *    The string will be either a text string or a 255-character string MySQL.
 *
 */
typedef enum _mmbdsql_booleans_sheet_record {
				/* Begin General Information */
    BooleanProcessStep,		/* 255 */
    BooleanDescription,		/* text */
    BooleanOptions,		/* 255 */
    BooleanRETTech,		/* 255 */
    BooleanRETProcessStep,	/* 255 */
    BooleanProxLayer,		/* 255 */
    BooleanDummy1,		/* Begin OPC Boolean Information */
    BooleanOPCBoolean,		/* text */
    BooleanOPCSGBoolean,	/* text */
    BooleanOPCPassThrough,	/* text */
    BooleanNoOPCLayer,		/* text */
    BooleanOPCOutputLayer,	/* 255 */
    BooleanDummy2,		/* Begin RET Tiling Boolean Information */
    BooleanTilingBoolean,	/* text */
    BooleanRETTilingBoolean,	/* text */
    BooleanTilingExclusion,	/* text */
    BooleanTilingPassThrough,	/* text */
    BooleanNoTilingLayer,	/* text */
    BooleanTilingOutputLayer,	/* text */
    BooleanDummy3,		/* Begin FIN Boolean Information */
    BooleanFINGrid,		/* 255 */
    BooleanFINFractureBoolean,	/* text */
    BooleanFINSGBoolean,	/* text */
    BooleanDummy4,		/* Begin SGPC Boolean Information */
    BooleanSGPCGrid,		/* 255 */
    BooleanSGPCFrctureBoolean,	/* text */
    BooleanSGPCSGBoolean,	/* text */
    BooleanDummy5,		/* Begin Key Boolean Information */
    BooleanKeyGrid,		/* 255 */
    BooleanInputGDSFiles,	/* text */
    BooleanKeyFractureBoolean,	/* text */
    BooleanDummy6,		/* Begin CD Boolean Information */
    BooleanCDFracturePanelGrid, /* 255 */
    BooleanCDInputGDSFiles,	/* text */
    BooleanCDFractureBoolean,	/* text */
    BooleanDummy7,		/* Begin OPC General Information */
    BooleanOPCFormName,		/* 255 */
    BooleanOPCType,		/* 255 */
    BooleanDummy8,		/* Begin Tiling General Information */
    BooleanTilingFormName,	/* 255 */
    BooleanTilingType,		/* 255 */
    BooleantilingVersions,	/* 255 */
    BooleanDummy9,		/* Begin Comments Section*/
    Booleancomments,		/* text */
    NumBooleanFields
} mmbdBooleansSheetFields;

/* This enum defines each record in the Sizing Sheet. *
 *
 *    Each element is stored as a string. This allows maximum flexibility.
 *    The string will be either a text string or a 255-character string MySQL.
 *
 */
typedef enum _mmbdsql_sizing_sheet_record {
    SizingProcessStep,		/* 255 */
    SizingDescription,		/* text */
    SizingOptions,		/* 255 */
    SizingBarcodeRevision,	/* 255 */
    SizingBuildPriority,	/* 255 */
    SizingDensity,		/* 255 */
    SizingChangeType,		/* 255 */
    SizingBaseLayerRef,		/* 255 */
    SizingMaskMag,		/* 255 */
    SizingCDFeature,		/* 255 */
    SizingOverlayAlignsTo,	/* 255 */
    SizingOverlayMeasuresTo,	/* 255 */
    SizingCDMinArea,		/* 255 */
    SizingCDMinWidth,		/* 255 */
    SizingCDKeySubstrate,	/* 255 */
    SizingDummy1,
    SizingTotalSizingDrawn,	/* 255 */
    SizingTotalSizingApp,	/* 255 */
    SizingTotalSizingWafer,	/* 255 */
    SizingPhotoBias,		/* 255 */
    SizingEtchBias,		/* 255 */
    SizingTotalBias,		/* 255 */
    SizingDummy2,
    SizingCDDrawnLineBefore,	/* 255 */
    SizingCDDrwanSpaceBefore,	/* 255 */
    SizingCDDrawnPitchAfter,	/* 255 */
    SizingCDDrawnLineAfter,	/* 255 */
    SizingCDDrwanSpaceAfter,	/* 255 */
    SizingCDDrawnPitchBefore,	/* 255 */
    SizingCDDenseLine,		/* 255 */
    SizingCDIsoLine,		/* 255 */
    SizingCDDenseSpace,		/* 255 */
    SizingCDDensePitch,		/* 255 */
    SizingReticleCDDenseLine,	/* 255 */
    SizingReticleCDIsoLine,	/* 255 */
    SizingReticleCDDenseSpace,	/* 255 */
    SizingReticleCDDensePitch,	/* 255 */
    SizingDummy3,
    SizingADICDWaferScale,	/* 255 */
    SizingADIPlusMinus,		/* 255 */
    SizingADITolerance,		/* 255 */
    SizingACICDWaferScale,	/* 255 */
    SizingACIPlusMinus,		/* 255 */
    SizingACITolerance,		/* 255 */
    SizingDummy4,
    Sizingcomments,		/* text */
    NumSizingFields
} mmbdSizingSheetFields;

typedef struct _mmbdsql_record {
    mmbdSheetType type;
    int field;
    char **fields;
    struct _mmbdsql_record *next;
} mmbdRecord;

/* Prototyes for routines used to manipulate records. */
mmbdSheetType mmbd_record_get_type( mmbdRecord *record );
char *mmbd_record_get_type_string( mmbdRecord *record );
char *mmbd_record_get_table_name( mmbdRecord *record );
int mmbd_record_get_num_fields( mmbdRecord *record );
char **mmbd_record_get_fields( mmbdRecord *record );
int mmbd_record_get_max_fields( mmbdRecord *record );
mmbdRecord *mmbd_record_new( mmbdSheetType type );
int mmbd_record_add_field( mmbdRecord *record, char *value );
void mmbd_record_initialize( mmbdRecord *record );
int mmbd_record_dump( mmbdRecord *record );
void mmbd_record_delete( mmbdRecord *record );
mmbdRecord *mmbd_record_get_next( mmbdRecord *record );

/* Prototypes for inserting records from MySQL. */

/* The following 2 prototypes will probably soon be deleted */
int mmbd_boolean_record_insert(char *record[NumBooleanFields], int num_fields);
int mmbd_sizing_record_insert(char *record[NumSizingFields], int num_fields);

/* Prototypes for retrieving records from MySQL. */
MYSQL_RES * mmbd_result_get(char *db, char *query1);

/* Prototypes from mmbd_sql_lib.c */
/* Get the maximum index from the mmbd_table */
int mmbd_max_index_rtv(void);

/* Map the mmbd name and user version to the mmbd index */
int mmbd_index_rtv(char *mmbd_name, char *user_version);

void core_id_rtv(char *core_id);

/* Retrieve a record from the mmbd database */
mmbdRecord *mmbd_record_rtv( mmbdSheetType type, char *process_step, char *mmbd_name, char *user_version);

/* Retrieve all records from specified table */
int mmbd_records_rtv(char *table, int total_fields);

/* Insert record into any MMBD table that is defined */
int mmbd_record_insert(mmbdRecord *record);

/* Insert info for a new MMBD */
int new_mmbd_insert(char *mmbd_name, char *user_version);

#endif
       password.h                                                                                          0000777 �    �Ԧ0120355 00000001200 12042342513 012426  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef password_h
#define password_h

static char password_array[92][2] =
{
"`?",
"1>",
"2<",
"3M",
"4N",
"5B",
"6V",
"7C",
"8X",
"9Z",
"0/",
"-.",
";	",
", ",
"~m",
"!n",
"@b",
"#v",
"$c",
"%x",
"^z",
"&:",
"*L",
"(K",
")J",
"_H",
"+G",
"qF",
"wD",
"eS",
"rA",
"t=",
"yl",
"uk",
"ij",
"oh",
"pg",
"[f",
"]d",
"Qs",
"Wa",
"E}",
"R{",
"TP",
"YO",
"UI",
"IU",
"OY",
"PT",
"{R",
"}E",
"aW",
"sQ",
"d]",
"f[",
"gp",
"ho",
"ji",
"ku",
"ly",
"=t",
"Ar",
"Se",
"Dw",
"Fq",
"G+",
"H_",
"J)",
"K(",
"L*",
":&",
"z^",
"x%",
"c$",
"v#",
"b@",
"n!",
"m~",
"	;",
" ,",
".-",
"/0",
"Z9",
"X8",
"C7",
"V6",
"B5",
"N4",
"M3",
"<2",
">1",
"?`",
};

#endif
                                                                                                                                                                                                                                                                                                                                                                                                rmt_server.h                                                                                        0000777 �    �Ԧ0120355 00000001176 12042342513 012770  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef rmt_servers_h
#define rmt_servers_h

#include "global_defines.h"


/* Info about remote servers */
typedef struct
{
    int server_num;	/* Number of remote servers */
    int server_index;	/* Index of first server to update */
    char log_file[20];	/* Holds log name for remote transactions */
    char site[TOTAL_SERVERS][32];
    char mach[TOTAL_SERVERS][32];
    char addr[TOTAL_SERVERS][ADDRESS_LEN];
    int sock[TOTAL_SERVERS];
} RMT_SERVERS;


extern RMT_SERVERS rmt_mpd;

int rmt_server_find(char *master_site);

void rmt_server_read(void);

void rmt_server_send(char *master_site);

#endif /* ifdef for rmt_servers_h */
                                                                                                                                                                                                                                                                                                                                                                                                  rtn.h                                                                                               0000777 �    �Ԧ0120355 00000000325 12202155334 011377  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef rtn_h
#define rtn_h

#include "global_defines.h"

typedef struct
{
    char msg[BIG_BLOCK_SIZE];
    int len;
    int flag;
    int transfer_status;
} RTN;

extern RTN rtn;


#endif /* define for rtn_h */
                                                                                                                                                                                                                                                                                                           scan_prototypes.h                                                                                   0000777 �    �Ԧ0120355 00000000460 12042342513 014027  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef scan_prototypes_h
#define scan_prototypes_h

/* Get an input line */
void line_scan(char *line);

/* Get an argument from the user */
/* This function allows the user to enter a carriage return */
/* and not specify any argument */
int arg_scan(char *line);

int string_scan(char *line);

#endif
                                                                                                                                                                                                                server_defines.h                                                                                    0000777 �    �Ԧ0120355 00000010467 12153723010 013604  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef server_defines_h
#define server_defines_h

#define BADGE_LEN 48
#define FABINT_CLIENT_VERSION "1.0"
#define DBNO_REQ 'm'
/* Following two defines help server run as daemon under BSD */
#define DAEMON   /* become a daemon */
#define BSD
/* Defines used to print error messages */
#define LATEST 'l'
#define STARS "**********************************************************************\n\n"
#define HELP_MSG "For assistance, contact DMI database administrator:\n\n\
               Email Address:  transbug@nxp.com"
#define TAB "               "
#define RET_TAB "\n               "
#define RET_TAB8 "\n        "
#define RET_TAB_QUOTE "\n               '"
#define RET2_TAB "\n\n                "
#define RET2_TAB_QUOTE "\n\n                '"
#define DOUBLE_RET "\n\n"
#define RET_TO "'\n            to '"
#define IN_TABLE "In table:      "

/* Lengths for global parameters */
#define ADDRESS_LEN 16
#define MACHINE_LEN 16
#define USER_LEN 32
#define SITE_LEN 64
#define FILE_SIZE 256
#define PATH_SIZE 1024
#define MSG_SIZE 4096	/* Maximum size for sending message */
#define PWD_SIZE 32	/* Length of temporary encryption password */
#define NULL_PIN "000000"
#define MASK_LEN 8
#define MIN_PIN_LEN 6
#define MAX_PIN_LEN 12
#define KEY_LEN 64
#define PAR_LEN 64
#define TOTAL_FILES 50 	/* Number of files database can manipulate at one time */
#define NUM_SERVERS 5
#define SERVER_INDEX NUM_SERVERS-1
#define MAX_SERVERS 5

#define DBNO 2
#define DBEMAIL 3
#define FAILURE -1
#define SUCCESS 0

/* This structure holds variables passed from the client to the server */
typedef struct
{
    char transaction_type;
    char req_type;
    char sec_req_type;
    char data_type;
    char sec_data_type;
    float vnum;
    float sec_vnum;
    char version_flag;
    char sec_version_flag;
    char override_flag; /* Indicates whether transaction continues under abnormal conditions */
    char key1[KEY_LEN]; /* Primary key for data accessing */
    char key2[KEY_LEN]; /* Secondary key for data accessing */
    char key3[KEY_LEN]; /* Key for data accessing */
    char key4[KEY_LEN]; /* Key for data accessing */
    char key5[KEY_LEN]; /* Key for data accessing */
    char par1[PAR_LEN];	/* First extra parameter, Holds miscellaneous data */
    char par2[PAR_LEN];	/* Second extra parameter */
    char par3[PAR_LEN];	/* Third extra parameter */
    char par4[PAR_LEN];	/* Fourth extra parameter */
    char par5[PAR_LEN];	/* Fifth extra parameter */
char cwd[PATH_SIZE];		/* Current working directory */
    char pin1[MAX_PIN_LEN+1];	/* Primary PIN */
char pin2[MAX_PIN_LEN+1];	/* Secondary PIN number */
} GU_PARAMS;

typedef struct
{
char machine[MAX_SERVERS][MACHINE_LEN];
char address[MAX_SERVERS][ADDRESS_LEN];
int socket;
int server_num;	/* Holds number of alternate servers to connect to */
char password[PWD_LEN+4];	/* User's encrypted password */
    char client_version[12];	/* Version of database software client using */
char user_id[USER_LEN];	/* User ID */
char user_mach[MACHINE_LEN];	/* Machine user is working on */
char user_addr[ADDRESS_LEN]; /*   IP node address of the machine user is on. */
char user_name[64];
} DB_PARAMS;

/* Holds flags returned from a Mask Prep or Fabint transsaction */
typedef struct
{
    int wrn_flag;
    int info_flag;
    int local_wrn_flag;
    int dsgn_file_num;
    int tmpl_file_num;
    int tmp_file_offset;
    int file_offset;
    short rtn_msg_len;
char rtn_msg[1500];	/* Status message returned with each DB primitive */
} DB_FLAGS;

/* Miscellaneous globals */
typedef struct
{
    long upd_time;
    char log_file[FILE_SIZE];
    char upd_time_string[16];	/* Used to generate unique file names */
} SERVER_PARAMS;

typedef struct
{
    int server_num;	/* Number of remote servers */
    long transaction_num;
    int server_index;	/* Index of first server to update */
    char req_type;		/* Request type */
    char transaction_type;
    char log_file[20];	/* Holds log name for remote transactions */
    char site[TOTAL_SERVERS][32];
    char mach[TOTAL_SERVERS][32];
    char addr[TOTAL_SERVERS][ADDRESS_LEN];
    int sock[TOTAL_SERVERS];
} RMT_PARAMS;


typedef struct
{
    char name[16];	/* Name of disk partition */
    int used;	/* Percent of disk being used */
    int allowed;	/* Percent of disk allowed to be used */
} DB_DISK;
#endif
                                                                                                                                                                                                         server_hash.h                                                                                       0000777 �    �Ԧ0120355 00000002162 12042342513 013105  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef hash_h
#define hash_h

#include "zlib.h"
#include "trans_defines.h"

gzFile trans_open ( char *mode, long long hash_size, int hash_enable, TRANS_FILE_TYPE file_type); 

int trans_read (gzFile file, voidp buf, char get_header); 

/* ------------------------------------------------------------ trans_write */
void trans_write (gzFile file, const voidp buf, unsigned len); 

/* ------------------------------------------------------------ trans_close */
long long trans_close (gzFile file , char file_type); 

/* ---------------------------------------------------------------- num_bytes */
char* num_bytes (long long bytes); 

/* ---------------------------------------------------------- trans_hash_init */
void trans_hash_init (long long fsize, int enable, TRANS_FILE_TYPE file_type, 
char *mode); 

/* --------------------------------------------------------------- trans_hash */
long long trans_hash (int new_bytes_xfered); 

long long transweb_hash (int new_bytes_xfered, FILE *fp_output); 

/* Print estimated file size */
void total_size_print(long long fsize, char *mode, TRANS_FILE_TYPE  file_type, int parallel);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                              server_lib.h                                                                                        0000777 �    �Ԧ0120355 00000001371 12042342513 012731  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef server_lib_h
#define server_lib_h

/* Used by the data base server and mask shop server */
short get_string(char *buffer);

short get_short();

void put_char(char data);

void put_short(short data);

/* Used to send data to the client */
int put_string(char *string);

void put_long(long data);

/* Used to send data to the client */

void put_binary_string(char *string, int len);
/* Concatinates two strings and sends to client */
/* All messages sent via put_string2 are logged */
void put_string2(char *string1, char *string2);

/* Concatinates three strings and sends to client */
/* All messages sent via put_string3 are logged */
void put_string3(char *string1, char *string2, char *string3);

long get_long();

long long get_longlong();

#endif
                                                                                                                                                                                                                                                                       server_params.h                                                                                     0000777 �    �Ԧ0120355 00000000645 12042342513 013451  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef server_params_h
#define server_params_h

#include "global_defines.h"

/* Holds info needed to connect to the database server */
/* The GUI never assigns values to these variables */
/* They are set by the database client */
typedef struct
{
char machine[MACHINE_LEN];
char address[ADDRESS_LEN];
    char site[48];
int socket;
} SERVER_PARAMS;

extern SERVER_PARAMS server;

#endif /* ifdef for server_params_h */
                                                                                           socket_includes.h                                                                                   0000777 �    �Ԧ0120355 00000000442 12042342513 013751  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef socket_includes_h
#define socket_includes_h
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef SOLARIS10
#include <netdb.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <sys/fcntl.h>

#endif /* ifdef for socket_includes_h */
                                                                                                                                                                                                                              solaris_stat.h                                                                                      0000777 �    �Ԧ0120355 00000002002 12042342513 013274  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef solaris_stat_h
#define solaris_stat_h

#define	_ST_FSTYPSZ 16		/* array size for file system type name */

typedef struct  timespec {              /* definition per POSIX.4 */
        time_t          tv_sec;         /* seconds */
        long            tv_nsec;        /* and nanoseconds */
} timespec_t;

typedef struct timespec timestruc_t;    /* definition per SVr4 */

struct  solaris_stat {
        unsigned long   st_dev;
        long    st_pad1[3];     /* reserved for network id */
        ino_t   st_ino;
        unsigned long  st_mode;
        unsigned long st_nlink;
        long   st_uid;
        long   st_gid;
        unsigned long   st_rdev;
        long    st_pad2[2];
        off_t   st_size;
        long    st_pad3;        /* future off_t expansion */
        timestruc_t st_atim;
        timestruc_t st_mtim;
        timestruc_t st_ctim;
        long    st_blksize;
        long    st_blocks;
        char    st_fstype[_ST_FSTYPSZ];
        long    st_pad4[8];     /* expansion area */
};

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              sort.h                                                                                              0000777 �    �Ԧ0120355 00000000422 12042342513 011560  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   typedef struct {
	char * key;         /* the key used in the comparison */
	int balance;			/* info about the state of the tree at this node */
							/* 0 if balanced, -1 if heavy to less, 1 if heavy to more */
	struct tree_node *less;
	struct tree_node *more;
}tree_node;

                                                                                                                                                                                                                                              ssl_client_lib.h                                                                                    0000777 �    �Ԧ0120355 00000001047 12326517766 013605  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef ssl_client_lib_h
#define ssl_client_lib_h


short get_ssl_string(char *buffer);

/* Retrieves a string up to 6144 characters in length */
short get_ssl_big_string(char *buffer);

short get_ssl_short(void);

char get_ssl_char(void);

short put_ssl_short(short data);

short put_ssl_long(unsigned long data);

short put_ssl_longlong(unsigned long long data);

short put_ssl_string(char *string);

short put_ssl_binary_string(char *string, short len);

int get_ssl_time(void);

long get_ssl_long(void);

long long get_ssl_longlong(void);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         state_file.h                                                                                        0000777 �    �Ԧ0120355 00000003105 12042342513 012711  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef state_file_h
#define state_file_h

/* Used by the Mask Prep server and the Mask Archive server */

#define NO_STATE 0 	/* State of data unknown */

#include "global_defines.h"

/* Holds data found in the state file */
typedef struct
{
    char pin[MAX_PIN_LEN+1];
    char master_site[32];
    char own_id[32];
    char own_email[MACHINE_LEN];
    char own_addr[ADDRESS_LEN];
    char tmp_id[USER_LEN];
    char tmp_email[MACHINE_LEN];
    char user_id[USER_LEN];	/* User ID of requester */
    char user_email[EMAIL_LEN];	/* Machine name of last user to access data */
    char user_addr[ADDRESS_LEN];	/* IP node address of last user to access data */
    char user_cwd[PATH_SIZE];	/* Last user's working directory */
    char pc_link[FILE_SIZE];
int read_cnt;
int lock_cnt;		/* Number of times user tried to access mask set */
int state;		/* Holds current state of the mask set or template */
int pre_state;		/* Holds previous state of mask set or template */
    int transaction_num;
    char app_state[5];
    long upd_time;
    long touch_time;
    float vnum;
    char archive_dir[FILE_SIZE];
} STATE_FILE;

extern STATE_FILE st;

int
state_file_read(char *state_file);


void
state_file_set(
    int new_state,
    short increment);


void
sec_state_file_init(void);

/* Set approved state */
void
state_file_set_app(void);

void
state_file_crt(
    int state,
    char *state_file,
    char *pin,
    char *pc_link,
    float *vnum);

int 
state_get_off(
    char *delta_dir,
    char *key);

void
state_file_get(void);

void
state_file_get_sec(void);

#endif /* ifdef for state_file_h */
                                                                                                                                                                                                                                                                                                                                                                                                                                                           strcpy.h                                                                                            0000777 �    �Ԧ0120355 00000012155 12326521534 012132  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef strcpy_h
#define strcpy_h

/* Copies source string into destination string returning the number */
/* of characters returned. */
int strcpy1(char *result, char *string);

/* Copy contents of two strings into destination string */
int strcpy2(char *result, char *str1, char *str2);

/* Copy contents of three strings into destination string */
int strcpy3(char *result, char *str1, char *str2, char *str3);

/* Copy contents of four strings into destination string */
int strcpy4(char *result, char *str1, char *str2, char *str3, char *str4);

/* Copy contents of 5 strings into destination string */
int strcpy5(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5);

/* Copy contents of six strings into destination string */
int strcpy6(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6);

/* Copy contents of seven strings into destination string */
int strcpy7(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7);

/* Copy contents of eight strings into destination string */
int strcpy8(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7,
    char *str8);

int strcpy9(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7,
    char *str8,
    char *str9);

int strcpy10(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7,
    char *str8,
    char *str9,
    char *str10);

int strcpy12(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7,
    char *str8,
    char *str9,
    char *str10,
    char *str11,
    char *str12);

/* Copy string exchanging old character for new character */
int strcpy_ex(
    char *new_string,
    char *old_string,
    char new_char,
    char old_char);

/* Looks for pattern at end of string */
/* If the pattern is found, it is stripped off */
void str_strip(char *string, char *pattern);

/* Strip off leading spaces */
void str_strip_leading_spaces(char *string);

/* Strip leading path off of file name */
/* It strips off paths with / or \ in it */
void str_path_strip(char *file_name);

/* Convert the specified string to lower case */
void str_lower(char *string);

/* Converts the specified string to upper case */
void str_upper(char *string);

/* Copies subset of source string to destination string */
/* Stops when specified character is found or buffer size for */
/* destination string has been reached */
/* Returns the length of the string if copy is successful */
int strcpy_substr(char *dest_string, char *src_string, char c, int len);

/* Finds position of specified character in string */
/* Returns -1 if character not found */
int strfind_char(char *string, char c, int upper_bound);

/* Finds specified substring in main string */
/* Returns a pointer to the substring if it is found */
/* Returns NULL if substring is not found in main string */
char *
strfind_substring(
    char *string,
    char *substring);

/* Make passed in string specified length */
/* If string not long enough, pad with spaces */
/* If string too long, truncate it */
void str_pad(char *string, int len);

/* Pad string with specified number of spaces to the left */
void strlpad(char *string, int len);

/* Exchanges all occurrences of old character to new character in string */
/* Returns length of string parsed */
int strexc(char *string, char old_char, char new_char);

/* Copy string changing occurrences of one character to two characters */
/* Returns length of new string */
int strexc2(char *new_string, char *old_string, char old_char, char *new_char);

/* Copies string1 into string2 */
/* Does not check for the null string */
/* Emulates actions of bcopy */
void str_bcopy(char *string1, char *string2, int len);

/* Converts a string to a long long */
long long str2ll(char *string);

/* Reads in line from buffer and returns number of characters found */
int str_line_get(char *buffer, char *line);

/* Version of strncpy which puts NULL at the end of the string */
void strn_cpy(char *result, char *string, int num);

/* Convert long long size to size string */
void str2size(char *size_string, long long size);

/* Verifies string contains onlyt numeric characters */
int str_numeric_only(char *string);

int str_is_alpha(char *string);

/* Strip off quotes, and brackets from beginning and end */
int str_quotes_strip(char *string);

/* Determines if string only contains digits */
/* Returns SUCCESS if all numeric characters */
/* Otherwise returns -1 */

int str_digits(char *string);
void str_move(char *string1, char *string2, char *string3);

/* See if file is a zipped file */
int is_zipped_file(char *file_name);

/* Reverse the characters in a string, must be NULL terminated */
int str_reverse(char *result, char *string);

/* count number of occurrences of character in string */
int str_numchar(char *string, char c);

#endif /* Define for strcpy_h */
                                                                                                                                                                                                                                                                                                                                                                                                                   tmp.h                                                                                               0000777 �    �Ԧ0120355 00000004115 12042342513 011374  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef maskshop_prototypes_h
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                   trans_prototypes_2.h                                                                                0000777 �    �Ԧ0120355 00000000373 12042342513 014456  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef trans_prototypes_2_h
#define trans_prototypes_2_h

/* line_get.c */
    int line_get(FILE *fp, char *line);

/* password_encrypt.c */
    void password_encrypt(char *password, char *pwd, int len);

#endif /* define for trans_prototypes_2_h */
                                                                                                                                                                                                                                                                     transrc.h                                                                                           0000777 �    �Ԧ0120355 00000000553 12042342513 012252  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef transrc_h
#define transrc_h
extern int rcread (char *rcfile);

struct transrc_t {
  int hashBySize;
  int hashByTime;
  char logDir[1024];
  char historyDir[1024];
    char machine[32];
    char address[20];
    int socket;
    unsigned long long parallel_zip_size;
    unsigned long long separate_block_size;
};

extern struct transrc_t transrc;

#endif
                                                                                                                                                     user_params.h                                                                                       0000777 �    �Ԧ0120355 00000001422 12042342513 013113  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef user_params_h
#define user_params_h

#include "global_defines.h"
typedef struct
{
int server_num;	/* Holds number of alternate servers to connect to */
    char client_version[12];	/* Version of database software client using */
char login[256];	/* User ID */
    char home_dir[PATH_SIZE];	/* User's home directory */
char mach[MACHINE_LEN];	/* Machine user is working on */
char addr[ADDRESS_LEN]; /*   IP node address of the machine user is on. */
char name[64];
char cwd[PATH_SIZE];		/* Current working directory */
char swd[PATH_SIZE];		/* System working directory */
    char machine_type[16];
    char pin[33];
    char email[64];
    char company[64];
} USER_PARAMS;

extern USER_PARAMS user;


/* Initializes the data about the user */
void user_params_init();


#endif
                                                                                                                                                                                                                                              whobu.h                                                                                             0000777 �    �Ԧ0120355 00000002020 12042342513 011711  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef whobu_h
#define whobu_h


#define USER_ROWS 16
#define USER_COLS 64
#define CN 0
#define LAST_NAME 1
#define FRIENDLY_EMAIL 2
#define PHONE 3
#define LOC 4
#define DEPT 5
#define MAIL_DROP 6
#define USER_ID 7
#define COMMERCE_ID 8
#define FIRST_NAME 9
#define NAME 15



/* USER_ROWS and WHOU_SIZE defined in global_defines.h */
int whobu_rtv_all(char *user_id, char *commerce_id, char value[USER_ROWS][USER_COLS]);

/* Reformat the phone number if ten digits */
void phone_format(char *phone);

/* Retrieve user's name and phone number */
/* Name and phone number returned in rtn.msg */
int whobu_name_rtv(char *x500);

int whobu_name_phone(char *x500, char *name, char *phone);

int whobu_name_dept(char *x500, char *commerce_id, char *user_id, char *name, char *dept);

int whobu_name_phone_dept(char *x500, char *name, char *phone, char *dept);


/* Returns user_id if user found */
int whobu_user(char *x500, char *commerce_id, char *user_id);

/* Verify user is in the X500 registry */
int whobu_check(char *user_id);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                x500_defines.h                                                                                      0000777 �    �Ԧ0120355 00000004606 12042342513 012772  0                                                                                                    ustar   rxfh80                          dtms                                                                                                                                                                                                                   #ifndef x500_defines_h
#define x500_defines_h

/* Return values from x500 functions */
#define VALID_LOGIN 200
#define PWD_CHANGED 200
#define X500INVALID_USERID 410
#define X500NONUNIQUE_USERID 415
#define X500PWD_INCORRECT 420
#define X500PWD_EXPIRED 421
#define X500SUSPENDED_ACCOUNT 422
#define X500PWD_STATUS_UNKNOWN 429
#define PWD_SAME 430
#define PWD_ILLEGAL 431
#define PWD_LEN_ERR 432

/* Defines for x500 errors */
#define pwd_incorrect "Error -720:    Incorrect One IT applications directory password entered.  Please  \n\
               try again.\n\n\
               You can set up your One IT applications directory password at web site\n\
               http://password.freescale.net.\n\n\
               If you have questions, contact Transcend team, \n\
               transbug@nxp.com"
#define PWD_INCORRECT -720
#define pwd_authen_wrn "Warning -721:  The One IT applications directory password you entered was incorrect.\n\
               However, authentication was done by verifying\n\
               your name in the email core directory matches \n\
               your name in your local password file.\n\n"
#define PWD_AUTHEN_WRN -721
#define invalid_userid   "Error -722:    Invalid core ID entered.  Please try again.\n\
               If you do not have a core ID, contact\n\
               your local system administrator or email\n\
               administrator.  If you have additional questions, \n\
               contact Transcend team, transbug@nxp.com."
#define INVALID_USERID -722
#define nonunique_userid "Error -723:  Non-unique core ID entered."
#define NONUNIQUE_USERID -723
#define pwd_expired "Error -724:    Your One IT applications directory password has expired and must be changed.\n\
               Please change your One IT applications directory password on the SPS Directory  \n\
               Services Web Page at http://password.freescale.net."
#define PWD_EXPIRED -724
#define suspended_account "Error -725:  Your OneIT user account has been suspended."
#define SUSPENDED_ACCOUNT -725

/* Verify if x500 User ID and password are correct */
char *vfypwd(
    char *server1,
    char *server2,
    int time_out,
    char *user_id,
    char *pwd);

/* Change One IT applications directory password */
char *
/* No longer supported by IS */
chgpwd(
    char *server1,
    char *server2,
    int time_out,
    char *user_id,
    char *old_pwd,
    char *new_pwd);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
