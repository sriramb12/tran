
/****************************************************************************
 *
 * Function:	trans_put_request and supporting functions
 *
 * Description:	Functions the API can call to upload data
 *
 ***************************************************************************/

#include <arpa/inet.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "client_dir.h"
#include "client_files.h"
#include "ssl_client_lib.h"
#include "client_maskshop.h"
#include "client_prototypes.h"
#include "company.h"
#include "global_defines.h"
#include "trans_hash.h"
#include "rtn.h"
#include "strcpy.h"
#include "server_params.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"


#include "global_debug.h" // Mazieri's debug facilities

#define HELP_MSG "For assistance, contact Intercom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"

extern int sock;

static int  is_file_encrypted(const char *file)
{
     char command[FILE_SIZE];
     int status = 0;
     int ret    = 0;  // default returns false
     sprintf(command, "gpg --batch --list-only %s > /dev/null 2>&1", file);
     status  = system(command);
     if (status == 0)
     {
         ret = 1;
     }
     DBG("command=%s status=%d ret=%d", command, status, ret);
     return ret;
}

/* Build command to execute for transferring a directory */
char *trans_dir_command_build(int index)
{
    char *tar_ptr;
    char tar_command[256];
    char follow_links_char;
    int len;

    DBG();

    /* Determine if links will be followed */
    if (trans[index]->follow_links == DBYES)
        follow_links_char = 'h';
    else follow_links_char = ' ';
        whereis_gzip_tar(tar_command);

    /* See if relative or absolute path specified */
    if (trans[index]->symbolic_dir == NULL)
    {
        len     = strlen(tar_command) + strlen(trans[index]->file_name) + 8;
        tar_ptr = malloc(len);

        sprintf(tar_ptr,"%s cf%c - %s", tar_command, follow_links_char, trans[index]->keyword);
    } else {
        len     = strlen(tar_command) + strlen(trans[index]->symbolic_dir) + strlen(trans[index]->keyword) + 64;
        tar_ptr = malloc(len);

        sprintf(tar_ptr, "%s cf%c - -C %s %s", tar_command, follow_links_char, trans[index]->symbolic_dir, trans[index]->keyword);
    } /* end else */

    return(tar_ptr);
} /*end trans_dir_command_build */


/* Build tar command for transfer of group of files */
char *trans_files_command_build(int index)
{
    char *tar_ptr;
    int len;
    char tar_command[256];

    DBG();

    whereis_gzip_tar(tar_command);
    /* Get length of strings needed for the tar command */
    len = strlen(tar_command) + strlen(trans[index]->files) + 8;
    /* Allocate space for tar command */
    tar_ptr = malloc(len);
    sprintf(tar_ptr, "%s  cf - %s", tar_command, trans[index]->files);
    return(tar_ptr);
} /* end trans_files_command_build */


/* Set a put request */
int trans_put_request_set(int index, REQ_TYPE req_type, char transfer_type)
{

/* to force serial transfer
   2014-10-27 16:27:56.371 k=NoKey pid=19159 ppid=19158 [../../client/trans_put.c:100:trans_put_request_set()] index=0 req_type=1, transfer_type=0

   2014-10-28 08:06:25.630 k=NoKey pid=24537 ppid=24536 [../../client/trans_put.c:100:trans_put_request_set()] index=0 req_type=1, transfer_type=115
*/


#if ICOM_FORCE_SERIAL_TRANSFER
    transfer_type = 's';
#endif
    DBG( "index='%d', req_type='%d', transfer_type='%d'", index, (int)req_type, (int)transfer_type );

    /* Indicate this is a put transaction for the perform function */
    trans[index]->transaction_type = PUT_REQUEST;
    trans[index]->req_type = req_type;

    /* Store the requested transfer type */
    /* This will be used later when the file name is set */
    /* The transfer type comes from the command line */
    /* Values are l r s z u */
    return(trans_put_transfer_type_set(index, transfer_type));
} /* end trans_put_request_set */

/* Encrypt ITAR data before sending to the maskshop */
/* This function must run on medusa */
/* A server running on medusa encrypts the data */
/* This is needed because the GPG keyring is on an old account on medusa */
/* After the data is encrypted, it is transferred to the maskshop using InterCom */
short trans_itar_data_encrypt(int index)
{
    DBG();
    int encrypt_index;
    struct stat buf;

    /* Verify file to encrypt is present */
    if (stat(trans[index]->file_name, &buf) == -1)
    {
        strcpy8(rtn.msg, bad_rd_open, RET2TAB, "'", trans[index]->file_name, "'", DOUBLE_RET, bad_rd_open_msg, "\n\n");
        DBG("BAD_RD_OPEN");
        return(BAD_RD_OPEN);
    }

    /* If this is a directory, it first needs to be tarred */
    if (S_ISDIR(buf.st_mode) || is_file_encrypted(trans[index]->file_name) == 0)
    {
        sprintf(rtn.msg, file_must_be_encrypted_err, trans[index]->file_name);
        return FILE_MUST_BE_ENCRYPTED;
    }

    /* Indicate data will not be compressed during transfer */
    trans[index]->compress_during_transfer = DBNO;

    DBG("SUCCESS");

    return(SUCCESS);
} /* end trans_itar_data_encrypt */


int list_extensions_rtv(int index)
{
    char extension_buffer[BUFFER_SIZE];
    extern char *extension_list[500];
    /*extern float *ratio_list[500];*/
    extern char *ratio_list[500];
    short ret;

    DBG();

    /* Connect to the database server */
    trans[index]->req_type = DBLIST_EXTENSIONS_RTV;

    if (trans_socket_init(index) == SUCCESS)
    {
        ret = get_ssl_string(extension_buffer);

        /* Get the return code */
        ret = trans_ret_code_get();

        /* Convert to list format */
        ret = string2double_list_convert(extension_buffer, extension_list, ratio_list);
    }

    /* If could not get the list, just use default list */
    else
    {
    ret = 1;
    strcpy(extension_buffer, ". 1.0\n");
    }
    DBG("ret='%d'", ret);
    return(ret);
} /* end list_extensions_rtv */


static void set_parallel_transfer_mode(index)
{
    switch(trans[index]->req_type)
    {
    case DBPUTDLT_DATA:
        trans[index]->transfer_type = DBPUT_LARGE_PARALLEL;
        trans[index]->req_type      = DBPUTDLT_LARGE_PARALLEL;
        break;
    case DBPUT_DATA:
        trans[index]->transfer_type = DBPUT_LARGE_PARALLEL;
        trans[index]->req_type      = DBPUT_LARGE_PARALLEL;
        break;
    default:
        break;
    }

}


/* Set the transfer type */
/* this simply stores the requested type, but does not guarantee it will be used */
int trans_put_transfer_type_set(int index, char transfer_type)
{
    DBG( "index='%d', transfer_type='%c'", index, transfer_type );

    switch(transfer_type)
    {
    case UNSPECIFIED_TRANSFER:  trans[index]->transfer_type = UNSPECIFIED_TRANSFER; break;
    case 'l':  set_parallel_transfer_mode(index); break;
    case 's':  trans[index]->transfer_type = SERIAL; break;
    case 'r':  set_parallel_transfer_mode(index);  break;
    case 'u':  trans[index]->compress_during_transfer = DBNO; break;
    case 'z':  trans[index]->zip_file = DBYES; break;
    case 't':  trans[index]->use_gtar = DBNO; break;
    default:
    {
        DBG( "Invalid transfer type for transfer_type=%d", (int)transfer_type );
        strcpy(rtn.msg, invalid_transfer_type);
        return(INVALID_TRANSFER_TYPE);
    }
    break;
    } /* end switch */
    DBG("SUCCESS");
    return(SUCCESS);
} /* end trans_put_transfer_type_set */

/* Determine the request type to use */
/* this is done after transfer type file name and size set */
void trans_request_type_set(int index)
{
    DBG();

    /* If serial transfer specified, simply return */
    if (trans[index]->transfer_type == SERIAL) return;
/* Parallel data transfer not supported for Windows */
#ifdef WINDOWS_NT4
    /* Do not change the request type */
    /* Give warning if parallel or separate block specified */
    if ((trans[index]->transfer_type == DBPUT_LARGE_PARALLEL)
    || 	(trans[index]->transfer_type == DBPUT_SEPARATE))
    PRINTF(ll_platform_wrn);
    return;
#endif
/* Parallel data transfer not supported for AIX */
#ifdef AIX
    /* Give warning if parallel or separate block specified */
    if ((trans[index]->transfer_type == DBPUT_LARGE_PARALLEL)
    || 	(trans[index]->transfer_type == DBPUT_SEPARATE))
    PRINTF(aix_ll_platform_wrn);
    return;
#endif
    if (trans[index]->file_type == DIRECTORY)
    {
    if (trans[index]->transfer_type == DBPUT_LARGE_PARALLEL)
    {
        PRINTF(ll_dir_wrn);
        return;
    }
    else if 	(trans[index]->transfer_type == DBPUT_SEPARATE)
    {
        PRINTF(ll_dir_wrn);
        return;
    }
    } /* end if */
    if (trans[index]->file_type == FILES)
    {
    if (trans[index]->transfer_type == DBPUT_LARGE_PARALLEL)
    PRINTF(ll_files_wrn);
    if 	(trans[index]->transfer_type == DBPUT_SEPARATE)
    PRINTF(files_retry_wrn);
    return;
    } /* end if */
    /* If file less than 5 Mb, give warning if parallel or separate specified */
    if (trans[index]->original_size < 1024*1024*5)
    {
    if (trans[index]->transfer_type == DBPUT_LARGE_PARALLEL)
    PRINTF(ll_5mb_wrn);
    else if (trans[index]->transfer_type == DBPUT_SEPARATE)
    {
        if (trans[index]->file_type == DIRECTORY)
        PRINTF(retry_dir_wrn);
        else PRINTF(retry5mb_wrn);
    } /* end else */
    return;
    } /* end if */
    /* If this is a zipped file, use parallel */
    if ((trans[index]->compress_during_transfer == DBNO)
    && (trans[index]->original_size > 100000000))
    {
    if (trans[index]->transfer_type == DBPUT_SEPARATE)
    {
        trans[index]->req_type = DBPUT_SEPARATE;
    } /* End if */
    else
    {
    /* Client always specifies large parallel */
    /* Server decides if parallel or large parallel used */
        if (trans[index]->req_type == DBPUTDLT_DATA)
        trans[index]->req_type = DBPUTDLT_PARALLEL;
        else trans[index]->req_type = DBPUT_PARALLEL;
    } /* end else */
    return;
    } /* end if */
    /* If have an unpressed file greater than 10 Mb, choose request type */
    if (trans[index]->transfer_type == DBPUT_LARGE_PARALLEL)
    {
    /* If this is a zipped file, use parallel transfer */
    if (is_zipped_file(trans[index]->file_name) == DBYES)
    {
        trans[index]->req_type = DBPUT_PARALLEL;
        sprintf(trans[index]->par5, "%d", DBPUT_PARALLEL);
    } else {
        trans[index]->req_type = DBPUT_RANDOM_PARALLEL;
        sprintf(trans[index]->par5, "%d", DBPUT_RANDOM_PARALLEL);
    } /* end else */
    /* Give a warning that there is some risk with random parallel */
    } /* end if */
} /* end trans_request_type_set */

/* Build list of e-mail addresses to send */
void commandline_email_set(int index, int arg_cnt, int argc, char **argv)
{
    int len = 0;
    int cnt;

    DBG( "index=%d, arg_cnt=%d, argc=%d, argv=%s", index, arg_cnt, argc, (char*) argv );

    if (argc <= arg_cnt) return;
    /* Otherwise calculate length of string needed for the e-mail addresses */
    for (cnt = arg_cnt; cnt < argc; ++cnt)
    len += strlen(argv[cnt])+1;
    /* Malloc space for the email list */
    trans[index]->email_list = malloc(len+10);
    /* Set len back to 0 for next loop */
    len = 0;
    /* Copy in the e-mail addresses */
    for (cnt = arg_cnt; cnt < argc; ++cnt)
    len += strcpy2(trans[index]->email_list+len, argv[cnt], " ");
    strcpy(trans[index]->par1, argv[arg_cnt]);
} /* end commandline_email_set */

/* Set description sent to receivers when secure keywords are generated */
int trans_description_set(int index, char *description)
{
    DBG( "index=%d, description=%s", index, description );

    if ((trans[index]->transaction_type != PUT_REQUEST)
    && (trans[index]->transaction_type != CUSTOM_REQUEST))
    {
    strcpy(rtn.msg, put_req_err);
    return(PUT_REQ_ERR);
    } else {
    trans[index]->description = malloc(strlen(description) + 1);
    strcpy(trans[index]->description, description);
    return(SUCCESS);
    } /* end else */
} /* end trans_description_set */

/* Scan description sent to receivers when secure keywords are generated */
int trans_description_scan(int index)
{
    DBG( "index=%d", index );

    char description[260];
    int cnt = 0;

    PRINTF("Enter a one line description for the data.\n> ");
    while (((description[cnt] = getchar()) != '\n')
    && (description[cnt] != EOF)
    && (cnt < 159)) ++cnt;
    description[cnt] = '\0';
    /* Put in carriage return for output separation */
    PRINTF("\n");
    return(trans_description_set(index, description));
} /* end trans_description_scan */

/* Set the email addresses for transaction from an array of pointers */
int trans_email_list_set(int index, char **email_list)
{
    void static_list_string_convert(char *list[], char *string);
    int cnt;
    int len = 0;

    /* Make sure this is a put request */
    if ((trans[index]->transaction_type != PUT_REQUEST)
    && (trans[index]->req_type != DBSEND_KEYWORD))
    {
    strcpy(rtn.msg, put_req_err);
    return(PUT_REQ_ERR);
    } /* end if */
    /* Calculate the size for the e-mail addresses */
    for (cnt = 0; email_list[cnt] != NULL; ++cnt)
    len += strlen(email_list[cnt]) +1;
    /* If no e-mail found, simply return */
    if (len == 0) return(SUCCESS);
    /* If company e-mail previous set, free it */
    if (trans[index]->email_list != NULL)
    {
        free(trans[index]->email_list);
        trans[index]->email_list = NULL;
    }
    /* Allocate space for the e-mail addresses */
    trans[index]->email_list = malloc(len+10);
    /* Convert e-mail addresses from list format to a string */
    static_list_string_convert(email_list, trans[index]->email_list);
    /* Indicate e-mail is being sent */
    strcpy(trans[index]->par1, email_list[0]);
    return(SUCCESS);
} /* end trans_email_list_set */

/* Set the email addresses for transaction from a string */
int trans_email_string_set(int index, char *email_string)
{
    int len;

    /* Make sure this is a put request */
    if ((trans[index]->transaction_type != PUT_REQUEST)
    && (trans[index]->req_type != DBSEND_KEYWORD))
    {
    strcpy(rtn.msg, put_req_err);
    return(PUT_REQ_ERR);
    } /* end if */
    /* Replace commas and semicolons with space */
    strexc(email_string, ',', ' ');
    strexc(email_string, ';', ' ');
    /* Calculate the size for the e-mail addresses */
    len = strlen(email_string) + 1;
    /* If company e-mail previous set, free it */
    if (trans[index]->email_list != NULL)
    {
        free(trans[index]->email_list);
        trans[index]->email_list = NULL;
    }
    /* Allocate space for the e-mail addresses */
    trans[index]->email_list = malloc(len+10);
    strcpy(trans[index]->email_list, email_string);
    /* Indicate e-mail is being sent */
    strcpy(trans[index]->par1, email_string);
    return(SUCCESS);
} /* end trans_email_string_set */

/* Set e-mail notification to remote company */
/* Maskshops receive e-mail notification automatically when data transferred to them */
int trans_company_email_set(int index)
{
    if (strcmp(remote_company_email, NONE) != 0)
    {
    /* Malloc space for the e-mail address */
    trans[index]->email_list = malloc(strlen(remote_company_email) + 10);
        strcpy(trans[index]->email_list, remote_company_email);
    } /* end if */
    return(SUCCESS);
} /* end trans_company_email_set */


/* Initialize file name for data being transferred */
/* Verifies file is present */
/* Store full file name in TRANS structure in field file_name */
/* Stores root file name in TRANS structure in keyword field */
/* also stores file mode and file type */
int trans_file_name_set(int index, char *file_name)
{
    extern int global_file_cnt;
    char *root_ptr;
    struct stat buf;
    int len;
    int offset;
    int is_absolute_path;
    int dir_levels = 0;

    DBG( "index=%d, file_name=%s", index, file_name );

    /* Make sure this is a put request */
    if (trans[index]->transaction_type != PUT_REQUEST)
    {
        strcpy(rtn.msg, put_req_err);
        return(PUT_REQ_ERR);
    } /* end if */

    /* If file name not specified, give error */
    if (file_name == NULL)
    {
        sprintf(rtn.msg, no_filename_err);
        return(NO_FILENAME_ERR);
    }

    /* Set the full file path */
    /* Exchange \ for / */
    strexc(file_name, '\\', '/');

    /* If there is a slash or backslash on the end of the name, get rid of it */
    len = strlen(file_name);

    if ((file_name[len-1] == '/') || (file_name[len-1] == '\\'))
        file_name[len-1] = '\0';

    /* Keep the full Unix path */
    /* Also keep the full Windows path */
    if ((file_name[0] == '/') || ((file_name[1] == ':') && (strncmp(user.machine_type, "Windows", 7) == 0)))
    {
        is_absolute_path = DBYES;
        strcpy(trans[index]->file_name, file_name);

        /* See if multiple levels present in absolute path */
        root_ptr = root_name_find(file_name+1, '/');

        /* If other slashes present in path, split it out */
        /* This is the case where multiple levels present in absolute path */
        if (root_ptr != NULL)
        {
            root_ptr[0] = '\0';
            offset = strlen(file_name)+1;
            trans[index]->symbolic_dir = malloc(offset);

            if (trans[index]->symbolic_dir == NULL)
                close_down1f(malloc_err, trans[index]->file_name, MALLOC_ERR);

            strcpy(trans[index]->symbolic_dir, file_name);
            strcpy(trans[index]->keyword, root_ptr+1);

            dir_levels = str_numchar(trans[index]->symbolic_dir, '/') + 1;
        } /* end if for absolute path with multiple levels present */

        /* This is the case when only one level for absolute path specified */
        else
        {
            strcpy(trans[index]->keyword, file_name+1);
            offset = 0;
            dir_levels = 0;
        } /* end else */
    } /* end big if */
    /* This is the case when a relative directory is specified */
    else
    {
        is_absolute_path = DBNO;
        dir_levels = 0;
        strcpy(trans[index]->file_name, file_name);

        /* Check for multiple levels in file name */
        if ((root_ptr = root_name_find(file_name, '/')) == NULL)
        {
            strcpy(trans[index]->keyword, file_name);
            offset = 0;
        } /* end if */
        /* This is the case where multiple levels present in relative path */
        else
        {
            root_ptr = root_name_find( file_name, '/');
            root_ptr[0] = '\0';
            offset = strlen(file_name) + 1;

            if ((trans[index]->symbolic_dir = malloc( offset)) == NULL)
                close_down1f(malloc_err, trans[index]->file_name, MALLOC_ERR);

            strcpy(trans[index]->symbolic_dir, file_name);
            strcpy(trans[index]->keyword, root_ptr+1);

            dir_levels = str_numchar(trans[index]->symbolic_dir, '/') + 1;

            /* Restore the original file name */
            root_ptr[0] = '\0';
        } /* end else */
    } /* end big else */

    /* Now calculate the file size */
    DBG("will call the stat() function for file '%s'", trans[index]->file_name);
    if (stat(trans[index]->file_name, &buf) == 0)
    {
        DBG("called the stat() function wich returns 0");
        /* Check to see if this is a directory */
        if (S_ISDIR(buf.st_mode))
        {
            DBG("Enter if S_ISDIR(buf.st_mode)");
            trans[index]->is_tar = DBYES;
            trans[index]->file_type = DIRECTORY;

            /* Initialize the dirlist file name */
            trans_dirlist_set();

            /* If sending data to the mask shop, build maskshop log */
            if (trans[index]->is_maskshop == DBYES)
                trans_maskshop_file_open();

            /* Calculate directory size */
            /* If this is an absolute path, pass in full file name plus offset */
            if (offset > 0)
                trans[index]->original_size = trans_disk_usage(trans[index]->file_name, trans[index]->follow_links, offset, dir_levels);
            else
                trans[index]->original_size = trans_disk_usage(trans[index]->keyword, trans[index]->follow_links, offset, dir_levels);

        /* XXX: Normal File, size is read here */
        } else {
            DBG("Enter else S_ISDIR(buf.st_mode)");
            DBG("trans[%d]->original_size='%llu' (from buf.st_size)\n", index, buf.st_size);
            trans[index]->original_size = buf.st_size;

            /* Indicate that one file is being sent */
            global_file_cnt = 1;
            trans[index]->file_type = UNSPECIFIED;

            /* If sending data to mask shop, append to log */
            if (trans[index]->is_maskshop == DBYES)
            {
                trans_maskshop_file_open();
                trans_maskshop_file_log(file_name, &buf, DBNO);
            }
        } /* end else for file */

        /* Store the file permission for later use */
        trans[index]->file_mode = buf.st_mode;
    } /* end if for stat on file successful */
    else
        return( bad_rd_msg_build(file_name));

    /* If file already zipped, do not compress it during transfer */
    if ((trans[index]->file_type == UNSPECIFIED) && (is_zipped_file(trans[index]->file_name) == DBYES))
        trans[index]->compress_during_transfer = DBNO;

    /* Set the request type */
    trans_request_type_set(index);

    return(SUCCESS);
} /* end trans_file_name_set */


/* Initialize a file stream for an already opened file */
int trans_file_stream_set(int index, FILE *fp)
{
    if (fp == NULL)
    {
        strcpy(rtn.msg, invalid_stream_ptr);
        return(INVALID_STREAM_PTR);
    }

    trans[index]->fp = fp;
    return(SUCCESS);
} /* end trans_file_stream_set */


/* Initialize a file descriptor for an already opened file */
int trans_file_descriptor_set(int index, int fd)
{
    if (fd == 0)
    {
        strcpy(rtn.msg, invalid_file_descriptor);
        return(INVALID_FILE_DESCRIPTOR);
    }

    /* Convert file descriptor to file stream */
    if ((trans[index]->fp = fdopen(fd, "r")) == NULL)
        return( bad_rd_msg_build(trans[index]->file_name));
    else return(SUCCESS);
} /* end trans_file_descriptor_set */


/* Initialize a list of files */
/* this is also where disk space is calculated */
/* Used with the -f and -F options */
int trans_files_set(int index, char **files)
{
    struct stat buf;
    int cnt;
    int len = 0;
    int len1;
    int done = DBNO;

    /* Make sure this is a put request */
    if (trans[index]->transaction_type != PUT_REQUEST)
    {
        strcpy(rtn.msg, put_req_err);
        return(PUT_REQ_ERR);
    } /* end if */

    /* Initialize the dirlist file name */
    trans_dirlist_set();

    /* Set the file type to files */
    trans[index]->file_type = FILES;
    trans[index]->is_tar = DBYES;

    /* See how much room needed for files */
    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
        /* Calculate length of string needed */
        len += strlen(files[cnt])+1;
    } /* end for */

    /* Build the maskshop log file */
    if (trans[index]->is_maskshop == DBYES)
        /* Build file with information of files transferred */
        trans_maskshop_file_open();

    /* Malloc space for the files */
    trans[index]->files = malloc(len+1);

    /* Set len back to 0 for next loop */
    len1 = len = 0;
    done = DBNO;

    for (cnt = 0; files[cnt] != NULL; ++cnt)
    {
        len += strcpy2(trans[index]->files+len, files[cnt], " ");
        /* Add file name to names sent to server */
        if (done == DBNO)
        {
            if (len1 < 40) len1 += strcpy2(trans[index]->keyword+len1, files[cnt], " ");
            else
            {
                strcpy1(trans[index]->keyword+len1, "etc.");
                done = DBYES;
            } /* End else */
        } /* End if */

        /* Add size of file to total size */
        /* If this is a directory, calculate directory size */
        if (S_ISDIR(buf.st_mode))
        {
            trans[index]->original_size += trans_disk_usage(files[cnt], DBNO, 0, 0);
        } else {
            if (stat(files[cnt], &buf) != 0)
            {
                sprintf(rtn.msg, bad_stat, errno, files[cnt]);
                return(BAD_STAT);
            }

            if (buf.st_size >= 1024)
                trans[index]->original_size  += buf.st_size;
            else
                trans[index]->original_size += 1024;

            trans_dirlist_add(files[cnt], buf.st_size);

            /* If sending data to mask shop, append to log */
            if (trans[index]->is_maskshop == DBYES)
                trans_maskshop_file_log(files[cnt], &buf, DBNO);
        } /* End else for file */
    } /* end for */

    /* Set the request type */
    trans_request_type_set(index);
    return(SUCCESS);
} /* end trans_files_set */


/* Build list of files stored in a file */
/* Used with the -n and -N options */
int trans_named_files_set(int index, char *file_name)
{
    FILE *fp;
    struct stat buf;
    int len = 0;
    int len1;
    int done = DBNO;
    char string[FILE_SIZE];

    /* Make sure this is a put request */
    if (trans[index]->transaction_type != PUT_REQUEST)
    {
        strcpy(rtn.msg, put_req_err);
        return(PUT_REQ_ERR);
    } /* end if */

    /* Initialize the dirlist file name */
    trans_dirlist_set();

    /* Set the file type to files */
    trans[index]->file_type = FILES;
    trans[index]->is_tar = DBYES;

    /* See how much room needed for files */
    if (stat(file_name, &buf) != 0)
    {
        sprintf(rtn.msg, bad_stat, errno, file_name);
        return(BAD_STAT);
    }

    trans[index]->files = malloc(buf.st_size+1);

    /* open the maskshop log file */
    if (trans[index]->is_maskshop == DBYES)
        trans_maskshop_file_open();

    /* Set len back to 0 for next loop */
    len1 = len = 0;
    done = DBNO;

    /* Open the file for reading */
    if ((fp = fopen(file_name, "r")) == NULL)
        return( bad_rd_msg_build(file_name));

    while(fscanf(fp, "%s", string) != EOF)
    {
        /* Get size of file */
        if (stat(string, &buf) != 0)
        {
            sprintf(rtn.msg, bad_stat, errno, string);
            return(BAD_STAT);
        }

        len += strcpy2(trans[index]->files+len, string, " ");

        /* Add file name to names sent to server */
        if (done == DBNO)
        {
            if (len1 < 40) len1 += strcpy2(trans[index]->keyword+len1, string, " ");
            else
            {
                strcpy1(trans[index]->keyword+len1, "etc.");
                done = DBYES;
            } /* End else */
        } /* End if */

        /* Add size of file to total size */
        /* If this is a directory, calculate directory size */
        if (S_ISDIR(buf.st_mode))
        {
            trans[index]->original_size += trans_disk_usage(string, DBNO, 0, 0);
        } else {
            if (buf.st_size >= 1024)
                trans[index]->original_size  += buf.st_size;
            else trans[index]->original_size += 1024;

            trans_dirlist_add(string, buf.st_size);
            trans_maskshop_file_log(string, &buf, DBNO);
        } /* End else for file */
    } /* end while */

    return(SUCCESS);
} /* end trans_named_files_set */


/* Read file names from a file */
/* Then set symbolic links */
int trans_dir_files_read(int index, char *file_name)
{
    int ret;
    int cnt = 0;
    FILE *fp;
    char tmp_file[FILE_SIZE];
    char *file_list[1000];

    fp = fopen(file_name, "r");
    while (fscanf(fp, "%s", tmp_file) != EOF)
    {
    file_list[cnt] = malloc(strlen(tmp_file)+1);
    strcpy(file_list[cnt], tmp_file);
    ++cnt;
    } /* end while */
    file_list[cnt] = NULL;
    /* Call function to set symbolic links */
    ret = trans_dir_files_link(index, file_list);
    files_free(file_list);
    return(ret);
} /* end trans_dir_files_read */

/* Set symbolic directory for the -k and -z options */
int trans_symbolic_dir_set(int index, char *symbolic_dir)
{
    int len;

    if (trans[index]->transaction_type != PUT_REQUEST)
    {
        strcpy(rtn.msg, put_req_err);
        return(PUT_REQ_ERR);

    } else {
        /* Set the file type */
        trans[index]->file_type = DIRECTORY;
        trans[index]->follow_links = DBYES;
        trans[index]->is_tar = DBYES;
        len = strlen(symbolic_dir);

        /* Allocate space for directory name */
        trans[index]->symbolic_dir = malloc(len+len+11);
        sprintf(trans[index]->symbolic_dir, "%s%ld", symbolic_dir, (long)time(0));
        strcpy(trans[index]->keyword, symbolic_dir);
        strcpy(trans[index]->file_name, symbolic_dir);

        return(SUCCESS);
    } /* end else */
} /* end trans_symbolic_dir_set */

int print_open_file_err(char *file_name)
{
            char *err_ptr = strerror(errno);
            char err_string[256];

            sprintf(err_string, "%d", errno);
    rtn.len = strcpy4(rtn.msg, bad_rd_open, RET2TAB_QUOTE,
    file_name, "'\n\n");
            strcpy6(rtn.msg+rtn.len,"Error message: ", err_ptr,
            "\nError No:      ", err_string, DOUBLE_RET, HELP_MSG);
    return(BAD_RD_OPEN);
} /* End print_open_file_err */
/* Link files to symbolic directory before transfer performed */
int trans_dir_files_link(int index, char **file_list)
{
    extern int global_dir_cnt;
    int dir_len = 0;
     int ret;
    int cnt;
    char *root_ptr;
    char *file_ptr;
    char tmp_dir[FILE_SIZE];
    char new_file[FILE_SIZE];
    long long size = 1024;
    struct stat buf;

    /* Make the symbolic directory */
#ifdef WINDOWS_NT4
    if (mkdir(trans[index]->symbolic_dir, 0700) != 0)
#else
    if (mkdir(trans[index]->symbolic_dir, 0700) != 0)
#endif
    {
    close_down1f(crt_dir_err, tmp_dir, CRT_DIR_ERR);
    return(CRT_DIR_ERR);
    }
    /* Now make the real directory */
    strcpy3(tmp_dir, trans[index]->symbolic_dir, "/", trans[index]->file_name);
#ifdef WINDOWS_NT4
    if (mkdir(tmp_dir, 0700) != 0)
#else
    if (mkdir(tmp_dir, 0700) != 0)
#endif
    {
    close_down1f(crt_dir_err, tmp_dir, CRT_DIR_ERR);
    return(CRT_DIR_ERR);
    }
    /* Initialize place where file names placed */
    file_ptr = strcpy2(new_file, tmp_dir, "/") +new_file;
    /* Initialize the name for the dirlist file */
    trans_dirlist_set();
    /* If this is a maskshop, build the maskshop log */
    if (trans[index]->is_maskshop == DBYES)
    {
    trans_maskshop_file_open();
    dir_len = strlen(tmp_dir+1);
    }
    /* Indicate only one directory level present */
    global_dir_cnt = 1;
    /* Link each file into the temporary directory */
    for (cnt = 0; file_list[cnt] != NULL; ++cnt)
    {
    /* First find the root file name */
    root_ptr = root_find(file_list[cnt], '/');
    strcpy2(file_ptr, "/", root_ptr);
    /* Now perform the link */
    if ((ret = link( file_list[cnt], new_file)) != 0)
    ret = symlink( file_list[cnt], new_file);
    if (ret != 0)
    {
        close_down1f(bad_link, file_list[cnt], BAD_LINK);
        dir_dlt(tmp_dir);
        dir_dlt(trans[index]->symbolic_dir);
        return(BAD_LINK);
    } /* End if */
    /* Add size of file to total size */
    if (stat(file_list[cnt], &buf) != 0)
    {
        dir_dlt(tmp_dir);
        dir_dlt(trans[index]->symbolic_dir);
    sprintf(rtn.msg, bad_stat, errno, file_list[cnt]);
        return(BAD_STAT);
    } /* End if */
    /* Size of 1024 added if file size less than 1024 */
    /* because tar makes files less than 1024 bytes 1024 bytes */
    if (buf.st_size >= 1024)
    size += buf.st_size;
    else size += 1024;
    trans_dirlist2add(trans[index]->file_name, root_ptr, buf.st_size);
        /* If sending data to mask shop, append to log */
    if (trans[index]->is_maskshop == DBYES)
    trans_maskshop_file_log(new_file+dir_len, &buf, DBNO);
    } /* End for */
    trans[index]->original_size = size;
    return(cnt);
} /* end trans_dir_files_link */

/* Transfer data using serial transfer */
int trans_serial_put(int index, FILE *fp)
{
    extern int size_printed;
    int ret;
    int len;
    char buffer[BLOCK_SIZE];
    unsigned long long final_size = 0;
    gzFile file;

    DBG();

    /* Prepare to write compressed data to socket */
    size_printed = DBNO;

    if (trans[index]->compress_during_transfer == DBYES)
    {
        file = tfile_open("w", 1, index);

        if (file == NULL)
        {
            strcpy(rtn.msg, gz_sockopen_err);
            close(sock);

            if (trans[index]->is_tar == DBYES) {
                pclose(fp);
            } else {
                fclose(fp);
            }

            return(GZ_SOCKOPEN_ERR);
        }

        /* Now send the file data itself */
        /* Write out data for as long as can read it in */
        while ( 0 != (len = fread(buffer, 1, BLOCK_SIZE, fp)) )
        {
            tfile_write(file, buffer, len, index);
            final_size += len;
            DBG("Sending %llu bytes (total='%llu')", len, final_size);
        }

        /* Flush any left over data to buffer */
        final_size = tfile_close(file, trans[index]->file_type);

        /* Tell client all data sent */
        put_ssl_short(SUCCESS);
        ret = put_ssl_longlong(final_size);

    } else {
        /* Indicate 0 bytes transferred */
        tfile_hash_init(trans[index]->original_size, trans[index]->file_type, "w", SERIAL, index);

        /* Write out data for as long as can read it in */
        final_size = 0;

        while ( 0 != (len = fread(buffer, 1, BLOCK_SIZE, fp)) )
        {
            put_ssl_binary_string(buffer, len);
            final_size += len;
            tfile_hash(len, 0, index);

            DBG("Sending %llu bytes (total='%llu')", len, final_size);
        } /* End while */

        /* Print final size */
        tfile_hash_close();

        /* Tell server all data sent */
        if (put_ssl_short(0) != SUCCESS)
        {
            sprintf(rtn.msg, bad_put, errno, HELP_MSG);
            close(sock);
            return(BAD_PUT);
        }

        ret = put_ssl_longlong(final_size);
    } /* end else for data already compressed */

    /* Close local file */
    if (trans[index]->is_tar == DBYES) {
        pclose(fp);
    } else {
        fclose(fp);
    }

    /* Put in carriage return for output separation */
#ifndef TRANSCEND_GUI
    if (size_printed == DBYES) PRINTF("\n");
#endif

    /* Get the return code */
    /* Also get the string to put in the log */
    ret = get_log_string();
    close(sock);

    return(ret);
} /* end trans_serial_put */

/* Perform a put */
/* This is called from trans_perform */
int trans_put_request(int index)
{
    char *tar_ptr;
    short ret;
    int mem_key;
    FILE *fp;

    DBG();

    /* See if ITAR data being sent */
    if (trans[index]->is_itar == DBYES)
    {
        DBG("its ITAR transfer");
        if ((ret = trans_itar_data_encrypt(index)) < 0) {
            DBG("after call trans_itar_data_encrypt() ret='%d'", ret);
            return(ret);
        }
    } /* end if */


    /* If this is a directory, build the tar command */
    if (trans[index]->file_type == DIRECTORY)
    {
        DBG("Transfer of a directory, because trans[index]->file_type == DIRECTORY");
#ifdef tar_problem
        sprintf(rtn.msg, nt_put_wrn, trans[index]->keyword, trans[index]->keyword, trans[index]->keyword);
        DBG("tar problem detected, will return NT_PUT_WRN from here");
        return(NT_PUT_WRN);
#else
        tar_ptr = trans_dir_command_build(index);
        DBG("tar command will be tar_ptr='%s'", tar_ptr);
#endif
    } /* end if for directory */
    else
    /* If this is a group of files, build the tar command */
    if (trans[index]->file_type == FILES)
    {
        DBG("Transfer of a group of files, because trans[index]->file_type == FILES");
#ifdef tar_problem
        sprintf(rtn.msg, nt_put_files_wrn, trans[index]->files, trans[index]->files);
        DBG("tar problem detected, will return NT_PUT_FILES_WRN from here");
        return(NT_PUT_FILES_WRN);
#else
        tar_ptr = trans_files_command_build(index);
        DBG("tar command will be tar_ptr='%s'", tar_ptr);
#endif
    } /* end if for files */


    /* If sending data to the maskshop, reset the request type */
    if (trans[index]->is_maskshop == DBYES)
    {
        DBG("Sending file to the maskshop, because trans[index]->is_maskshop == DBYES");
        if ((ret = trans_mask_name_set(trans[index]->keyword, trans[index]->mask)) < SUCCESS) {
            DBG("transfer to maskshop returns ret='%d'", ret);
            return(ret);
        }

        trans_maskshop_req_type_reset(index);
    } /* end if */


    /* Connect to the database server */
    if ((ret = trans_socket_init(index)) < SUCCESS) {
        DBG("trans_socket_init(index) returns ret='%d'", ret);
        return(ret);
    }


    /* If sending data to the maskshop, send list of files being transferred */
    if (trans[index]->is_maskshop == DBYES)
    {
        DBG("sending data to the maskshop");

        /* Close the file just being created */
        trans_maskshop_file_close();

        if (trans[index]->req_type != DBPUT_DUPLICATE_FILES)
        {
            if ((ret = trans_maskshop_filelist_send()) != SUCCESS)
                return(ret);
        } /* end if */

    } /* end if for sending data to the maskshop */


    /* See if email notification is being sent */
    if (trans[index]->email_list != NULL)
    {
        DBG("List of email notification is being sent '%s'", trans[index]->email_list);

        /* If description set, send it, otherwise send blank description */
        if (trans[index]->description != NULL) {
            DBG("there is a description '%s'", trans[index]->description);
            put_ssl_string(trans[index]->description);
        } else {
            put_ssl_string("");
        }

        /* Send the list of e-mail addresses */
        put_ssl_string(trans[index]->email_list);

    } /* end if for e-mail notification */

    DBG("Server return\n%s", rtn.msg);

    /* Get keyword */
    get_ssl_string(rtn.msg);

    /* If output not suppressed, print keyword */
    if (trans[index]->display_transfer_status == DBYES)
        PRINTF("%s\n\n", rtn.msg);

    /* Copy keyword into trans structure */
    sscanf(rtn.msg, "%*s %*S %*s %s", trans[index]->file_keyword);
    strcpy(trans[index]->keyword, trans[index]->file_keyword);

    /* Get indication that server ready to receive data */
    if ((ret = get_ssl_short()) < SUCCESS)
    {
        get_ssl_string(rtn.msg);
        close(sock);
        DBG("inside `if ((ret = get_ssl_short()) < SUCCESS)`, where ret='%d'", ret);
        return(ret);
    }

    /* If background transfer performed, spawn off separate process */
#ifndef WINDOWS_NT4
    if (trans[index]->perform_background_transfer == DBYES)
    {
        DBG("inside if for background transfer");

        /* Initialize the shared memory */
        mem_key = shared_status_create(trans[index]->keyword);

        /* Allocate memory */
        shared_status_allocate(mem_key);

        /* Indicate not finished and 0 bytes transferred */
        shared_status_write(1, 0);

        /* If this is the parent return */
        if (fork() != 0)
            return(SUCCESS);
    } /* end if for background transfer */
#endif

    /* Choose transfer type based on parallel type */
    /* The server tells client if it can do a parallel transfer */
    trans[index]->req_type = (REQ_TYPE)ret;
    DBG("The server indicates to the client if it can do parallel transfer, saved at ret='%d'", ret);

#ifndef WINDOWS_NT4
    switch(trans[index]->req_type)
    {
        case DBPUT_LARGE_PARALLEL:
        case DBPUTDLT_LARGE_PARALLEL:
        case DBPUT_PARALLEL:
        case DBPUTDLT_PARALLEL:

        DBG("request type indicates we will call trans_parallel_put");
        ret = trans_parallel_put(index);
        break;

        case DBPUT_RANDOM_PARALLEL:
        {
            DBG("request type indicates we will call trans_uncompressed_parallel_put");
            ret = trans_uncompressed_parallel_put(index);
        }
        break;

        case DBPUT_SEPARATE:
            DBG("request type indicates we will call trans_put_separate_compressed_data");
            ret = trans_put_separate_compressed_data(index);
        break;

        default:
        {
#endif
            DBG("default, will call trans_serial_put");
            /* If file descriptor already open, copy to local value */
            if (trans[index]->fp != NULL) {
                /* This is used for file stream or file descriptor */
                fp = trans[index]->fp;

            } else if (trans[index]->is_tar == DBYES) {
                /* If this is a directory or group of files, open the tar command */
                fp = popen(tar_ptr, "r");

            } else {
            /* This is for a single file */
                fp = fopen(trans[index]->file_name, "r");
            }

            if (fp == NULL)
                return(print_open_file_err(trans[index]->file_name));

            ret = trans_serial_put(index, fp);
#ifndef WINDOWS_NT4
        } /* end default */
    } /* End switch */

    DBG("after the switch, ret='%d'", ret);
    /* If sent a directory or group of files, send the transdirlist file */
    if (ret == SUCCESS)
    {
        DBG("after the switch, ret='%d' is SUCCESS", ret);
        if (trans[index]->is_tar == DBYES)
        trans_dirlist_put(index);

        /* If this is a mask shop, send list of files sent */
        if (trans[index]->is_maskshop == DBYES)
            trans_intercom_file_send(index, DBYES);
    } /* end if for successful return code */

    /* Indicate status complete */
    if (trans[index]->perform_background_transfer == DBYES)
        shared_status_finish(0);

    /* Release the shared memory */
    if (trans[index]->perform_background_transfer == DBYES)
        shared_status_release();
#endif

    /* Delete symbolic dir if present */
    /* Clean up symbolic directory no longer needed */
    if ((trans[index]->symbolic_dir != NULL) && ((trans[index]->req_type == DBPUT_FILES) || (trans[index]->req_type == DBPUTDLT_FILES)))
    {
        DBG("Delete symbolic dir if present. Clean up symbolic directory no longer needed");

        char command[PATH_SIZE];
        strcpy2(command, "rm -r -f ", trans[index]->symbolic_dir);
        system(command);
    }

    DBG("we hit the end of trans_put_request(), will return ret='%d'", ret);
    return(ret);
} /* end trans_put_request */

/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
