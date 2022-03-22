#ifndef user_params_h
#define user_params_h

#include "global_defines.h"


#define USER_ROWS 20
#define USER_COLS 128
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

typedef struct
{
  int  server_num;   /* Holds number of alternate servers to connect to */
  char client_version[12];	/* Version of database software client using */
  char login[256];	/* User ID */
  char home_dir[PATH_SIZE];	/* User's home directory */
  char mach[MACHINE_LEN];	/* Machine user is working on */
  char addr[ADDRESS_LEN]; /*   IP node address of the machine user is on. */
  char name[256];
  char cwd[PATH_SIZE];		/* Current working directory */
  char swd[PATH_SIZE];		/* System working directory */
  char machine_type[16];        /* Machine type ie Linux, Windows, Solaris, Web browser, etc */
  char pin[33];
  char email[256];
  char from_company[64];
  char to_company[64];         /* Company data being sent to, This is often External TransWeb Customer */
  char phone[128];
  char dept[128];
  char site[128];
  int  ldap_ok;                /* flag to indicate the user was already found in LDAP */ 
} USER_PARAMS;

extern USER_PARAMS user;


/* Initializes the data about the user */
void user_params_init();


#endif
