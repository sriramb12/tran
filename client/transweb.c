#ifndef TRANSAPI
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "client_lib.h"
#include "company.h"
#include "global_client_errs.h"
#include "global_defines.h"
#include "intercom_lib.h"
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"

#include "global_debug.h"

#define RET2_TAB_QUOTE "\n\n\t'"


/* TransWeb version comes from trans_include/trans_defines.h */
#define transweb_msg \
"Welcome to version %s of TransWeb\n\
All parameters for the following commands which contain spaces must be enclosed in double quotes\n\n\
Check user registration:           'transweb <COREID> -C'\n\
Authenticate a user:               'transweb <COREID> -a <password>'\n\
Put a file:                        'transweb <COREID> -p <tmp file> <real file> <repository> <delete>  <output_file> [<email>] [<description>] [<SR_num>]\n\
Get a file:                        'transweb <COREID> -g <secure key> <file key> <output_file>'\n\
Get/delete a file:                 'transweb <COREID> -G <secure key> <file key> <output_file>'\n\
Retrieve as a compressed file:     'transweb <COREID> -gz <secure key> <file key> <output_file>'\n\
Delete a file:                     'transweb <COREID> -d <secure key> <file key>'\n\
View file info:                    'transweb <COREID> -i <secure key> <file key>'\n\
View file mode:                    'transweb <COREID> -I <secure key> <file key>'\n\
View file history:                 'transweb <COREID> -h <secure key> <file key>'\n\
View TransWeb server sites:        'transweb <COREID> -x'\n\
View list of TransWeb companies:   'transweb <COREID> -X'\n\
View file status:                  'transweb <COREID> -S <keyword> '\n\
Extend file expiration 1-3 days:   'transweb <COREID> -e <keyword> <days>'\n\
Send e-mail when files retrieved:  'transweb <COREID> -s all '\n\
Cancel sending e-mail for files:   'transweb <COREID> -c all '\n\
Send e-mail when 1 file retrieved: 'transweb <COREID> -s <keyword> '\n\
Cancel sending e-mail for 1 file:  'transweb <COREID> -c <keyword> '\n\
View original/stored file length:  'transweb <COREID> -l <keyword>'\n\
Lock data to prevent retrieval:    'transweb <COREID> -L <keyword>'\n\
Unlock data and allow retrieval:   'transweb <COREID> -U <keyword>'\n\
Lock data to prevent deletion:     'transweb <COREID> -D <keyword>'\n\
Unlock data and allow deletion:    'transweb <COREID> -U <keyword>'\n\
Mail users secure keyword:         'transweb <COREID> -m <keyword> <email>[<description>]\n\
View list of secure keywords:      'transweb <COREID> -V [-<site>]'\n\
View list of file keywords:        'transweb <COREID> -v [-<site>]'\n"
/*
Delete data for all user keywords:        'transweb  <core_id> -d all'\n\
Set Email notification for all files:     'transweb  <core_id> -s [-<site>] all'\n\
Cancel Email notification for all files:  'transweb  <core_id> -c [-<site>] all'\n\
Set Email notification for one file:      'transweb  <core_id> -s <keyword>'\n\
Cancel Email notification for one file:   'transweb  <core_id> -c <keyword>'\n\
Delete data after the current retrieval:  'transweb  <core_id> -G <keyword>'\n\
Retrieve data as a compressed file:       'transweb  <core_id> -gz <keyword>'\n\
*/




char * transweb_company(void)
{
    char *ptr;
    char **list = NULL;

    DBG();

    list = icom_company_rtv(&ptr);
    if (list == NULL) return(NULL);
#ifdef NXP
    return(list[2]);
#endif
#ifdef EXTERNAL_TRANSWEB
    return(list[2]);
#endif
}

int transweb_put(char *tmp_file, char *real_file, char *company_ptr, char *delete, char *output_file, char *email_ptr, char *description)
{
    DBG();

    char keyword[12];
    unsigned long long size;
    struct stat buf;
    char buffer[BUFFER_SIZE];
    FILE *fp;
    FILE *output_fp;
    char *ptr;
    char *real_file_ptr;
    int delete_flag;
    int len;
    int local_dir_cnt = 0;
    TRANS_FILE_TYPE data_type;
    FILE *file = 0;

    DBG("tmp_file='%s', real_file='%s', company_ptr='%s', delete='%s', output_file='%s', email_ptr='%s', description='%s'", tmp_file, real_file, company_ptr, delete, output_file, email_ptr, description);

    /* Mike passes in the file name with the full path */
    /* Thus I need to strip off the full path */
    real_file_ptr = root_find( real_file, '/');

    /* If did not find a / or \, point to the file */
    if (real_file_ptr == NULL)
        real_file_ptr = real_file;

    /* Open the file for write */
    /* Mike puts the file in /tmp and gives it a new name */
    /* Thus, I open the temporary file but send in the real file name */
    /* Determine if the file is a directory */
    data_type = data_type_get(tmp_file);

    if (data_type < 1) return(data_type);
    else if (data_type == DIRECTORY)
    {
        char command[FILE_SIZE];
	    sprintf(command,  "gtar cf - \"%s\"", tmp_file);
	    fp = popen(command, "r");
    }
	else fp = fopen(tmp_file, "rb");


    if (fp == NULL)
    {
	    strcpy6(rtn.msg, bad_rd_open, RET2_TAB_QUOTE, real_file_ptr, "'\n\n", bad_rd_open_msg, "\n\n");
	    return(BAD_RD_OPEN);
    }

    /* Open the output file */
    /* This is where the amount of data transferred is written to */
    if ((output_fp = fopen(output_file, "w")) == NULL)
    {
	    strcpy6(rtn.msg, bad_wr_open, RET2_TAB_QUOTE, output_file, "'\n\n", bad_rd_open_msg, "\n\n");

	    if (data_type == DIRECTORY)
	        pclose(fp);
	    else
	        fclose(fp);

	    return(BAD_WR_OPEN);
    }

    /* change delete flag to integer value */
    delete_flag = atoi(delete);

    /* Initialize the size of the data */
    if (data_type == DIRECTORY)
        size = big_disk_usage(tmp_file, local_dir_cnt);
    else if (stat(tmp_file, &buf) == 0)
        size = buf.st_size;
    else
        size = 0;

#ifdef NXP
    file = icom_put_compress_init(real_file_ptr, company_ptr, description, email_ptr,  delete_flag);
#endif
#ifdef EXTERNAL_TRANSWEB
    file = icom_put_compress_init(real_file_ptr, company_ptr, description, email_ptr, DBNO);
#endif
    ptr = get_global_msg();
    if (file == 0)
        return(-1);

    /* Scan in the keyword */
    if (sscanf(ptr, "%*s %*s %*s %s", keyword) < 1)
        return(-1);

    /* Write out the file keyword */
    fprintf(output_fp, "%s\n", keyword);
    fflush(output_fp);

    /* Now write out the data */
    while ( 0 != (len = fread(buffer, 1, BLOCK_SIZE, fp)) )
    {
	    len = icom_write(file, buffer, len);
	    transweb_hash(len, output_fp);
    }

    if (data_type == DIRECTORY)
        pclose(fp);
    else
        fclose(fp);

    /* Now close connection */
    return(icom_put_compress_close(file, "description"));
}


/* Not currently being used */
int transweb_put_uncompressed(char *file_name, char *company_ptr)
{
    int ret;
    char *ptr;

    DBG("file_name='%s', company_ptr='%s'", file_name, company_ptr);

    ret = icom_put_init(file_name, company_ptr, "test", NONE, 'e', DBNO, DBNO, 0);
    ptr = get_global_msg();

    if (ret != 0)
        return(ret);

    /* Now write out the data */
    put_binary_string("testing\n", 8);
    ret = icom_put_close((long long)8, "description", 'e');

    PRINTF("ptr is %s\n", ptr);

    return(ret);
}

int transweb_get(int req_type, char *primary_key, char *secondary_key, int retrieve_compressed)
{
    int ret;
    int remote_compression;
    char file_name[FILE_SIZE];
    long mode;
    long long size;
    char file_type;
    char *ptr;

    DBG("req_type='%d', primary_key='%s', secondary_key='%s', retrieve_compressed='%d'", req_type, primary_key, secondary_key, retrieve_compressed);

    /* May 2018, Carlos Mazieri
        icom_get_init() used to be:
           icom_get_init(req_type, primary_key, secondary_key, DBNO)
    */
    ret = icom_get_init(primary_key, secondary_key, DBNO);
    ptr = get_global_msg();

    if (ret != 0)
        return(ret);

    /* Scan in file name, size, mode, and file type */
    sscanf(ptr, "%s %lld %ld %c %d", file_name, &size, &mode, &file_type, &remote_compression);

    /* Indicate ready to retrieve the data */
    put_short(SUCCESS);

    /* If requesting it uncompressed or already uncompressed get it */
    ret = icom_data_get(file_name, retrieve_compressed, remote_compression, size);

    if (ret != 0)
        PRINTF(rtn.msg);

    chmod(file_name, mode);

    exit(ret);
} /* End transweb_get */


void status_print(char **status)
{
    DBG();

    PRINTF("GLOBAL_EMAIL_SET: %s\n", status[0]);
    PRINTF("FILE_EMAIL_SET: %s\n", status[1]);
    PRINTF("RETRIEVAL_LOCK_SET: %s\n", status[2]);
    PRINTF("DELETE_SET: %s\n", status[3]);
    PRINTF("DAYS_EXTENDED: %s\n", status[4]);
    PRINTF("OWN_DATA: %s\n", status[5]);
    PRINTF("TRANSFER_COMPLETE: %s\n", status[6]);

    exit(0);
} /* End status_print */

int main(int argc, char *argv[])
{
    extern char icap_class[];
    char *email_ptr;
    char *description;
    char *company_ptr;
    char friendly_email[32];
    char name[32];
    char *ptr;
    char **key_ptr;
    char primary_key[12];
    char secondary_key[12];
    int cnt;
    int i;

    DBG("*** MAIN TRANSWEB CLIENT ***");

    if (argc <= 2)
    {
	    PRINTF(transweb_msg, TRANS_VERSION); // from trans_include/trans_defines.h
	    exit(0);
    }

    defaults_init("", argv[1], "", "");

    /* Read values from .transwebrc file */
    transrc_read(".transweb.rc");

    if ((argc < 4) && ((argv[2][1] != 'C') && (argv[2][1] != 'a') && (argv[2][1] != 'x') && (argv[2][1] != 'X')))
    {
	    fprintf(stderr, "Not enough parameters specified\n");
	    exit(-1);
    }

    /* copy keys for convenience */
    if (argc >= 5)
    {
	    strcpy(primary_key, argv[3]);
	    strcpy(secondary_key, argv[4]);
    }
    else if (argc >= 4)
    {
	    strcpy(primary_key, argv[3]);
	    strcpy(secondary_key, primary_key);
    }
/*
    company_ptr = transweb_company();
*/

#ifndef SINGLE_CONNECT
/*
    icom_repository_rtv(&ptr);
*/
#endif

    icom_repository_set(0);

    user_params_init();

    switch(argv[2][1])
    {
	    /* Check registration */
	    case 'C':
        {
            DBG("Check registration 'C' flag");
            i = dbsend_request(DBX500LOOKUP, user.login);
        }
        break;

	    /* Authenticate user */
	    case 'a':
	    {
            DBG("Authenticate user 'a' flag");

	        if ((i = icom_authen(argv[1], argv[3])) >= 0)
		{
		    ptr = get_global_msg();
	            sscanf(ptr, "%[^;]%*c%[^;]", friendly_email, name);
		}
	    }
	    break;

	    case 'x':
	    case 'X':
	    {
	        DBG("Company retrieve 'x' or 'X'");

	        char **company_ptr;

	        if (argv[2][1] == 'x')
	            company_ptr = icom_company_rtv(&ptr);
	        else
	            company_ptr = icom_repository_rtv(&ptr);

	        for (cnt = 0; company_ptr[cnt] != NULL; ++cnt)
	            PRINTF("%s\n", company_ptr[cnt]);

	        exit(0);
	    }
	    break;

	    case 'S':
        {
            DBG("Keyword status 'S'");

	        char **status = icom_status(primary_key, secondary_key);
		ptr = get_global_msg();

	        /* Print status returned */
	        if (status != NULL) status_print(status);
	    }
	    break;

	    /* View user keywords */
	    case 'v':
	    {
            DBG("View user keywords 'v'");

	        key_ptr = icom_user_keys_rtv( argv[3]);
		ptr = get_global_msg();

	        for (cnt = 0; key_ptr[cnt] != NULL; ++cnt)
	            PRINTF("%s\n", key_ptr[cnt]);

	        /* If keywords retrieved do not print return code */
	        /* Otherwise indicate an error was found */
	        if (cnt >= 0) exit(0);
	        else exit(-1);
	    }
	    break;

	    /* View secure keywords */
	    case 'V':
	    {
	        DBG("View secure keywords 'V'");

	        key_ptr = icom_secure_keys_rtv(argv[3]);
		ptr = get_global_msg();

	        for (cnt = 0; key_ptr[cnt] != NULL; ++cnt)
	            PRINTF("%s\n", key_ptr[cnt]);

	        /* If keywords retrieved do not print return code */
	        /* Otherwise indicate an error was found */
	        if (cnt >= 0) exit(0);
	        else exit(-1);
	    }
	    break;

	    case 'g':
	    {
	        DBG("retrieve key 'g'");

	        int retrieve_compressed = DBYES;
	        int req_type = DBGET_DATA;

	        /* See if user wants to retrieve the data compressed */
	        if (argv[2][2] == 'z')
            {
	            retrieve_compressed = DBNO;
	            req_type = DBGET_ZIPPED;

	            i = transweb_get(req_type, primary_key, secondary_key, retrieve_compressed);
            }
	        else
	            i = transweb_get(req_type, primary_key, secondary_key, retrieve_compressed);
	    }
	    break;

	    case 'G':
	    {
	        DBG("retrieve and delete key 'g'");

	        int retrieve_compressed = DBYES;
	        int req_type = DBGETDLT_DATA;

	        /* See if user wants to retrieve the data compressed */
	        if (argv[2][2] == 'z')
            {
	            retrieve_compressed = DBNO;
            }

	        i = transweb_get(req_type, primary_key, secondary_key, retrieve_compressed);
	    }
	    break;

	    case 'p':
	    {
	        DBG("put file 'p'");

	        char string[180];

	        /* See if data classification specified */
	        if ((argc >= 4) && (argv[3][0] == '-'))
	        {
	            strcpy(icap_class, argv[3]+1);
	            argv += 1;
	            argc -= 1;
	        }
#ifdef NXP
	        else strcpy(icap_class, "fcp");
#else
	        else strcpy(icap_class, "cp");
#endif

	        if (argc < 7)
	        {
		        strcpy(rtn.msg, "Not enough parameters");
		        exit(-1);
	        }

	        if (argc >= 9) email_ptr = argv[8];
	        else email_ptr = NULL;

	        if (argc >= 10) description = argv[9];
	        else description = NULL;

            /* If SR# specified, verify that data is going to a mass market customer */
	        if (argc >= 11)
	        {
	            char tla[32];

	            /* Scan in the tla */
	            sscanf(argv[5], "%[^;]%*c%[^;]", tla, tla);

	            if ((strcmp(tla, "imc") == 0) || (strcmp(tla, "emc") == 0))
	            {
		            strcpy4(string, "SR# ", argv[10], ", ", argv[9]);
		            description = string;
	            } /* end if */

	        } /* End big if */

	        i = transweb_put(argv[3], argv[4], argv[5], argv[6], argv[7], email_ptr, description);
	    }
	    break;

	    case 'P':
        {
            DBG("put compressed 'P'");
            i = transweb_put_uncompressed(argv[2], company_ptr);
        }
        break;
	    case 'i':
        {
            DBG("keyword info 'i'");
            i = icom_info(primary_key, secondary_key);
	    ptr = get_global_msg();
        }
        break;
	    case 'I':
        {
            DBG("Keyword mode 'I'");
            i = icom_mode(primary_key, secondary_key);
	    ptr = get_global_msg();
        }
        break;
	    case 'h':
        {
            DBG("Keyword history 'h'");
            i = icom_history(primary_key, secondary_key);
	    ptr = get_global_msg();
        }
        break;
 	    case 'l':
        {
            DBG("Keyword file size 'l'");
            i = icom_size(primary_key, secondary_key);
	    ptr = get_global_msg();
        }
        break;
	    case 'd':
        {
            DBG("Keyword delete 'd'");
            i = icom_delete(primary_key, secondary_key);
	    ptr = get_global_msg();
        }
        break;
	    case 'e':
        {
            DBG("Extend keyword 'e'");
            i = icom_extend(primary_key, secondary_key,  3);
	    ptr = get_global_msg();
        }
        break;
	    case 'm':
	    {
	        DBG("Mail keyword 'm'");
	        char *description;

	        if (argc >= 6) description = argv[5];
	        else description = NONE;

	        i = icom_mail_keyword(primary_key, secondary_key, argv[4], description);
		ptr = get_global_msg();
	    }
	    break;

	    case 'c':
        {
	        DBG("Another mail keyword 'c'");

	        if (strcasecmp(argv[3], "all") != 0)
	            i = icom_can_file_email(primary_key, secondary_key);
	        else i = icom_can_user_email();
		ptr = get_global_msg();
	    }
	    break;

	    case 's':
        {
            DBG("Set file 's'");

	        if (strcasecmp(argv[3], "all") != 0)
	            i = icom_set_file_email(primary_key, secondary_key);
	        else i = icom_set_user_email();
		ptr = get_global_msg();
	    }
	    break;
	    case 'L':
        {
            DBG("Lock keyword 'L'");
            i = icom_lock(primary_key, secondary_key);
	    ptr = get_global_msg();
        }
        break;
	    case 'U':
        {
            DBG("Unlock keyword 'U'");
            i = icom_unlock(primary_key, secondary_key);
	    ptr = get_global_msg();
        }
        break;
	    case 'D':
        {
            DBG("Delete locked key 'D'");
            i = icom_delete_lock(primary_key, secondary_key);
	    ptr = get_global_msg();
        }
        break;
	    case 'E':
        {
            DBG("Delete unlocked key 'E'");
            i = icom_delete_unlock(primary_key, secondary_key);
	    ptr = get_global_msg();
        }
        break;
    } /* end switch */

    PRINTF("%s\n", rtn.msg);

    exit(i);
} /* end main */

#endif /* for TRANSAPI */

/* dont remove it! or you will break the build (legacy, Renee's Build) */
shared_status_write() { }
short user_autoreg(void) { }
int dir_dlt() { }
int display_transfer_status() { }
trans_dirlist_log() { }
float list_ratio_find(char *string) { }
static_list_string_convert() { }
string2double_list_convert() { }
