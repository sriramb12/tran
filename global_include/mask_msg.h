#ifndef mask_msg_h
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
