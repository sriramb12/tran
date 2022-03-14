#ifndef m2msg_h
#define m2msg_h

#include "global_defines.h"

/* Holds definition for mask message */
/* This structure is used by the Mask Prep server */
/* and the Archive server */
typedef struct
{
    int req_type;
    int data_type;
    int override_flag; /* Indicates whether transaction continues under abnormal conditions */
    int program;
    int num;
    long transaction_time;
    char key1[KEY_LEN]; /* Primary key for data accessing */
    char key2[KEY_LEN]; /* Secondary key for data accessing */
    char key3[KEY_LEN]; /* Key for data accessing */
    char par1[PAR_LEN];	/* First extra parameter, Holds miscellaneous data */
    char par2[PAR_LEN];	/* Second extra parameter */
    char par3[PAR_LEN];	/* Third extra parameter */
    char par4[PAR_LEN];	/* Fourth extra parameter */
    char par5[PAR_LEN];	/* Fifth extra parameter */
char swd[PATH_SIZE];		/* Current working directory */
    char password[32];
} M2MSG;

extern M2MSG m2msg;

#endif /* ifdef for m2msg_h */
