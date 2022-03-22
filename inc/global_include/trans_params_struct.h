#ifndef _INCLUDE_TRANS_PARAMS_STRUCT
#define _INCLUDE_TRANS_PARAMS_STRUCT

#include "global_defines.h"

/* this structure is allocated and initialized by trans_transaction_init */
/* It is deallocated by trans_cleanup */
/* Holds data passed from the GUI to the server */
typedef struct
{
    int transaction_status;
    /* Transaction type, keyword, simple, custom, put, or get */
    int transaction_type;
    /* Request type, put, get, delete, etc */
    int req_type;
    /* Determines if files automatically deleted  */
    int overwrite_files;
    /* Transfer files in background, DBYES or DBNO */
    int perform_background_transfer;
    int suppress_prompt_output; /* Prompt and display output */
    /* Display transfer status */
    int display_transfer_status;
    /* Set the transfer type to serial or parallel */
    int transfer_type;
    /* Follow symbolic links during data upload */
    int follow_links;
    int send_rtv_email;
    /* Specifies if authentication is needed - DBYES or DBNO */
    int authen;
    int is_tar;
    int is_itar; /* Is this ITAR data */
    int is_maskshop; /* Is this a mask shop */
    char tar_type[32];
    /* Use gtar */
    int use_gtar;
    /* Zip the data during transfer */
    int zip_file;
    /* Compress during transfer */
    int compress_during_transfer;
    /* Uncompress during transfer */
    int uncompress_during_transfer;
    /* Indicates if company list has been retrieved */
    int company_list_rtvd;
    /* Array of e-mail addresses to receive secure keywords */
    char *email_list;
    char *alternate_file;
    char *alternate_dir;
    /* Description of data used when secure keywords sent */
    char *description;
    int repository;
    /* Number of days to extend file expiration */
    int num_days;
    /* Currently not used, will be used in the future */
    int eccn_number;
    /* Holds file descriptor for file */
    FILE *fp;
    /* List of files to transfer when group of files being uploaded */
    char *files;
    /* Name of file to upload */
    char file_name[PATH_SIZE];
    char c_option[4];
    char *symbolic_dir;
    char remote_dir[PATH_SIZE];
    char mask[8];
    /* Primary keyword, can be the file name or file or secure keyword */
    char keyword[FILE_SIZE];
    char file_keyword[12];
    char machine_type[KEY_LEN];
    char key1[KEY_LEN];
    char email1[KEY_LEN]; /* Secondary key for data accessing */
    char email2[KEY_LEN]; /* Key for data accessing */
    /* Extra parameters present in header */
    char par1[PATH_SIZE];	/* First extra parameter */
    char par2[PATH_SIZE];	/* Second extra parameter */
    char par3[PAR_LEN];	/* Third extra parameter */
    char par4[PAR_LEN];	/* Fourth extra parameter */
    char par5[PAR_LEN];	/* Fifth extra parameter */
    char par6[PAR_LEN];	/* Fifth extra parameter */
    /* time when transaction initiated */
    time_t client_time;
    char from_company[COMPANY_LEN];
    char to_company[COMPANY_LEN];
    char icap_class[80];
    unsigned long long original_size;
    /* compressed size of data being transferred */
    unsigned long long compressed_size;
    char pid[KEY_LEN];
    /* Read write mode for file copied from st_mode */
    long file_mode;
    /* File type ie File, group of files, directory, etc */
    char file_type;
} TRANS_PARAMS;


#endif // _INCLUDE_TRANS_PARAMS_STRUCT
