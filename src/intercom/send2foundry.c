#ifndef INTERCOM
#ifndef TRANSWEB
/****************************************************************************
 *
 * Function:	get_gds
 *
 * Description:	Routines to get data from Design Sync and send data to foundry
 *
 ***************************************************************************/

#include <stdlib.h>
#include <alloca.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>

#include "close_down.h"
#include "external_intercom_prototypes.h"
#include "ftp_info.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "trans_defines.h"
#include "trans_lib_prototypes.h"
#include "trans_server_arrays.h"
#include "trans_server_defines.h"
#include "trans_server_errs.h"
#include "trans_server_globals.h"
#include "trans_server_macros.h"
#include "trans_server_paths.h"
#include "user_params.h"
#include "global_debug.h"

extern COMPANY_DATA cp;
extern TRANS_FILE_STAT fs;
extern FTP_TRANSFER_PARAMS ft;


#include <curl.h>



#if defined(DEBUG)
# define TRANSFTP_CURL_VERBOSE 1L
#else
# define TRANSFTP_CURL_VERBOSE 0L
#endif

#define   SFTP_NOT_SUPPORTED_BY_SERVER  0
#define   SFTP_IS_SUPPORTED_BY_SERVER   1


#define LOCAL_CURL_UPLOAD_BUFFER_SIZE 2048000L  // buffer to upload


#define SFTP_PROBE_CONTENT "transfer to probe SFTP, please discard this file\n"

static int gl_is_sftp_supported                      = SFTP_NOT_SUPPORTED_BY_SERVER;
static int gl_curl_return_CURLE_REMOTE_ACCESS_DENIED = 0;

struct tr_data
{
    FILE *fp;
    size_t total_size;
    size_t offset;
    size_t transferred_size;
    size_t block_size;
    int cnt;
};


typedef size_t (*Curl_read_callback) (void *ptr, size_t size, size_t mem, void *data);



void _priv_set_transfer_error_msg(const char *reason)
{
    sprintf(rtn.msg, "Error:       %s", reason);
    ftp_status_update(fs.key, rtn.msg);
}


void s2f_close_down(const char *reason, const char *item, int error)
{
    _priv_set_transfer_error_msg(reason);
    data_log(tr.log_file, reason);
    close_down1m(reason, item, error);
}


static size_t sftp_probe_read_callback(void *ptr, size_t size, size_t mem, void *data)
{
    size_t ret;

    DBG();

    ret = strlen(SFTP_PROBE_CONTENT);
    strcpy(ptr, SFTP_PROBE_CONTENT);
    return ret;
}

/*! \fn curl_set_ssh_authorization_type() sets the type of SFTP authentication
 *
 *  Just to avoid duplicated code
 */
static CURLcode curl_set_ssh_authorization_type(CURL *curl)
{
    CURLcode result = -1;
    if (cp.password[0] != '\0')
    {
        result = curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD | CURLSSH_AUTH_KEYBOARD);
     FORCE_DBG("curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD | CURLSSH_AUTH_KEYBOARD)=%d", result);
     if (result == CURLE_UNKNOWN_OPTION)
     {
          result = curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD )=%d", result);
     }
    }
    else
    {
        char ssh_file[1024];
        char ssh_priv_file[1024];
        unsigned long uid = getuid();
        struct passwd *pw_entry;
        setpwent();
        pw_entry = getpwuid(uid);
        if (pw_entry != NULL)
        {
            char ssh_file[512];
            result = curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES , CURLSSH_AUTH_PUBLICKEY);
            FORCE_DBG("curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES , CURLSSH_AUTH_PUBLICKEY)=%d" , result);
            strcpy2(ssh_file, pw_entry->pw_dir, "/.ssh/id_rsa.pub");
            strcpy2(ssh_priv_file, pw_entry->pw_dir, "/.ssh/id_rsa");
            curl_easy_setopt(curl, CURLOPT_KEYPASSWD, "");
            FORCE_DBG("curl_easy_setopt(curl, CURLOPT_KEYPASSWD, NULL)=%d" , result);
            result = curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, ssh_file);
            FORCE_DBG("curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, %s)=%d" , ssh_file, result);
            result = curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE, ssh_priv_file);
            FORCE_DBG("curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE , %s)=%d" , ssh_priv_file, result);
        }
    }
     return result;
}



/*! \fn set_generic_CURL_options() sets common LibCurl options for both parallel and setrial transfers
 *
 */
static CURLcode set_generic_CURL_options(CURL *curl,             /* in */
                                         char *url,              /* in */
                                         char *login_pwd,        /* in */
                                         int  is_sftp_supported, /* in */
                                         struct tr_data * tr_data_pt, /* in */
                                         char* error_CURL,       /* in */
                                         Curl_read_callback  read_function_callback,
                                         unsigned long long block_size_to_read  /* in */)
{
     CURLcode result = 0;
     result  = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_CURL);
     FORCE_DBG("curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_CURL)=%d", result);
     if(result == CURLE_OK)
     {
          /* enable verbose for easier tracing */
          long int CURL_VERBOSE = TRANSFTP_CURL_VERBOSE;
          if(CURL_VERBOSE != 1L){
            if(access("curl_debug", F_OK) == 0){
                CURL_VERBOSE = 1L;
                /* Send curl verbose to a file*/
                char* curl_debug_file = "curl_debug.log";
                FILE *filec = fopen(curl_debug_file, "a");
                fprintf(filec,"\n%s\n", "===================================================");
                result = curl_easy_setopt(curl, CURLOPT_STDERR, filec); // send verbose messages to curl_verbose.log
                FORCE_DBG("curl_easy_setopt(curl, CURLOPT_STDERR, '%s')=%d", curl_debug_file, result);
            }
          }
          if(result == CURLE_OK){
            result = curl_easy_setopt(curl, CURLOPT_VERBOSE, CURL_VERBOSE); // when DEBUG is defined we activate verbose
            FORCE_DBG("curl_easy_setopt(curl, CURLOPT_VERBOSE, %ld)=%d", CURL_VERBOSE, result);
          }
     }
     if(result == CURLE_OK)
     {
          /* Indicate progress of transfer not checked */
          result = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)=%d", result);
     }
     if(result == CURLE_OK)
     {
          /* Set the URL for upload */
          result = curl_easy_setopt(curl, CURLOPT_URL, url);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_URL, url)=%d", result);
     }
     if(result == CURLE_OK)
     {
          /* Indicate extended passive not used, does not disable passive */
          result = curl_easy_setopt(curl, CURLOPT_FTP_USE_EPSV, 0L);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_FTP_USE_EPSV, 0L)=%d", result);
     }
     if(result == CURLE_OK)
     {
          /* Use the following read call back */
          result = curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_function_callback);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_function_callback)=%d", result);
     }
     if(result == CURLE_OK)
     {
          /* Set reading from the defined buffer */
          result = curl_easy_setopt(curl, CURLOPT_READDATA, tr_data_pt);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_READDATA, tr_data_pt)=%d", result);
     }
     if (result == CURLE_OK)
     {
          result = curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_RETRY);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_RETRY)=%d", result);
     }
     if(result == CURLE_OK)
     {
          result = curl_easy_setopt(curl, CURLOPT_USERPWD, login_pwd);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_USERPWD, '%s')=%d", login_pwd,  result);
     }
     if (is_sftp_supported == SFTP_IS_SUPPORTED_BY_SERVER)
     {
         result = curl_set_ssh_authorization_type(curl);
     }
     if(result == CURLE_OK)
     {
          /* Indicate an upload will be performed */
          result = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L)=%d", result);
     }
     if(result == CURLE_OK)
     {
          curl_off_t  cur_size = (curl_off_t)block_size_to_read;
          FORCE_DBG("total_size=%llu sizeof(curl_off_t)=%d cur_size=%ld sizeof(block_size_to_read)=%d", block_size_to_read, sizeof(curl_off_t), cur_size, sizeof(block_size_to_read));
          /* Indicate the size of the block will be sent */
          result = curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, cur_size);
          FORCE_DBG("curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,block_size_to_read=%llu)=%d;", block_size_to_read, result);
     }
     return result;
}



/*! \fn build_full_remote_pathname()  builds a full pathname (directory + filename) from remote server for FTP or SFTP
 *
 *  \param is_sftp               must be either SFTP_IS_SUPPORTED_BY_SERVER or SFTP_NOT_SUPPORTED_BY_SERVER
 *  \param full_remote_pathname is the output and will contain that full remote pathname
 */
static void  build_full_remote_pathname(char *remote_dir,  /* in */
                                        char *remote_file, /* in */
                                        int  is_sftp,      /* in */
                                        char *full_remote_pathname /*out */)
{
    char *root_dir = 0;
    int  cp_login_length = strlen(cp.login);
    /**
    *  Oct 29 2014 Carlos Mazieri
    *
    *  Changed the approach related to comment above
    *
    *  For TSMC the cp.root is "/home/motola"  which is the home of the user NXP
    *  in this case a url like:  ftp://@ftp.tsmc.com/home/motola/test/BAD_GDS_TEST_20MB.gds.gz.gpg
    *  will be saved in /home/motola/home/motola/test  because the path is relative to the user path
    *
    *  Other foundries like: gfm, altis and dalsa the home is "/"
    *
    *  So now we check for the login in the /home/<login>
    */
    if (cp.root_dir[0] == 0)
    {
         strcpy (cp.root_dir, "/");
    }
    root_dir = cp.root_dir;
    if ( strncmp(cp.root_dir, "/home/", 6) == 0
         && strncmp(&cp.root_dir[6], cp.login, cp_login_length ) == 0)
    {
         /*
         * if the cp.root_dir is;  /home/my-login/someelsedir/
         * we want to get only /someelsedir/
         */
         root_dir += 6 + cp_login_length;
    }
    if (*root_dir == '/')
    {
        root_dir++;
    }
    // handle SFPT x FTP
    if (is_sftp == SFTP_IS_SUPPORTED_BY_SERVER)
    {
        /*
         * Carlos Mazieri, March 2018
         * if  cp.root_dir is equal "/"  -> root directory
         *   normal FTP account controls where it is going to be saved, local home or root directory
         * for SFP it does not happend, we need to have some control to make it fail in case it cannot write into root directory
         */
        if (*root_dir == '\0' )
        {
             // try root directory and expect it to fail in case cannot write into it
             strcpy4(full_remote_pathname, root_dir, remote_dir, "/", remote_file);
        }
        else
        {
             // use user home
             strcpy5(full_remote_pathname,  "~/", root_dir, remote_dir, "/", remote_file);
        }
    }
    else
    {
        strcpy4(full_remote_pathname, root_dir, remote_dir, "/", remote_file);
    }
}



/*! \fn check_if_server_supports_preferred_protocol()  checks if that server supports the preferred protocol (SFTP x FTP)
 *
 *  Prepares the SFTP full URL and tries to authenticate
 *
 *  \return SFTP_IS_SUPPORTED_BY_SERVER   in case server supports
 *          SFTP_NOT_SUPPORTED_BY_SERVER  otherwise
 */
static int check_if_server_supports_preferred_protocol(char *ftp_url, char *login_password, int is_sftp)
{
    int ret = !is_sftp;
    CURL *curl = 0;

    DBG("is_sftp=%d", is_sftp);

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl)
    {
        char url[1024];
        CURLcode result = 0;
        const char *probe_file = "sftp.probe.txt";
        strcpy(url, ftp_url);
        char *save_file = strrchr(url, '/');
        if (save_file)
        {
            strcpy(++save_file, probe_file);
        }
        char error_CURL[CURL_ERROR_SIZE + 256];
        unsigned long long total_size  = strlen(SFTP_PROBE_CONTENT);
        struct tr_data tr;
        tr.transferred_size = 0;
        tr.offset = tr.total_size  = total_size;
        tr.fp = fopen("db_admin/company_email", "r");
        if (tr.fp == NULL)
        {
            tr.fp = fopen("db_admin/subnet_table", "r");
        }

        FORCE_DBG("Probe preferred protocol url=%s %p", url,  tr.fp);
        result = set_generic_CURL_options(curl, url, login_password, is_sftp, &tr, error_CURL,
                                          sftp_probe_read_callback, total_size);
        FORCE_DBG("AFTER set_generic_CURL_options() result=%d", result);

        /* Perform the transactions */
        if(result == CURLE_OK)
        {
            struct curl_slist *slist = NULL;
            char string[1024];
            if (is_sftp == SFTP_NOT_SUPPORTED_BY_SERVER)
            {
                strcpy2(string, "DELE ", probe_file);
            }
            else
            {
                // "sftp://"  position 7
                char *path_begins = strchr(&url[7], '/');
                strcpy2(string,"rm ", path_begins);
            }
            slist = curl_slist_append(slist, string);
            {
                int post_quote = curl_easy_setopt(curl, CURLOPT_POSTQUOTE, slist);
                FORCE_DBG("string=%s curl_easy_setopt(curl, CURLOPT_POSTQUOTE, slist)=%d", string, post_quote);
            }
            result = curl_easy_perform(curl);
            /* ----------------------------------------------------------------------
               This define is for debugging purposes
             -----------------------------------------------------------------------*/
#if defined(DEBUG) && defined(SIMULATE_ERROR_CURLE_REMOTE_ACCESS_DENIED)
            if (gl_curl_return_CURLE_REMOTE_ACCESS_DENIED == 0)
            {
                FORCE_DBG("simulating CURLE_REMOTE_ACCESS_DENIED ...");
                result = CURLE_REMOTE_ACCESS_DENIED;
            }
#endif
            FORCE_DBG("curl_easy_perform(curl)=%d is_sftp=%d CURLE_OK=%d", result, is_sftp, CURLE_OK);
            if (result == CURLE_REMOTE_ACCESS_DENIED)
            {
                /* -------------------------------------------------------------------------------------------------------------
                   May 2019, Carlos Mazieri
                CURLE_REMOTE_ACCESS_DENIED (9)
                We were denied access to the resource given in the URL. For FTP, this occurs while trying to change to the remote directory.
                ----------------------------------------------------------------------------------------------------------------
                */
                gl_curl_return_CURLE_REMOTE_ACCESS_DENIED = 1;
            }
            // some FTP sites give an error on QUOTE commands. Synopys gives error on creating directories
            curl_slist_free_all(slist);
            if (result == CURLE_OK || result == CURLE_QUOTE_ERROR || result == CURLE_REMOTE_FILE_NOT_FOUND)
            {
                ret = is_sftp;
                if (is_sftp == SFTP_IS_SUPPORTED_BY_SERVER)
                {
                   FORCE_DBG("SFTP is supported sleeping 1 second(s) to let server created all subdirs ...");
                }
                sleep(1);
            }
        }
        fclose(tr.fp);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        // restore original URL
    }

    FORCE_DBG("ftp_url=%s ret=%d", ftp_url, ret);
    return ret;
}


/*!
 * \brief build_full_remote_url()
 * \param remote_dir
 * \param remote_file
 * \param is_sftp
 * \param url
 */
static void build_full_remote_url(char *remote_dir,  /* in */
                           char *remote_file, /* in */
                           int  is_sftp,      /* in */
                           char *url          /* out*/)
{
    char  remote_pathname[1024];
    if (is_sftp == SFTP_IS_SUPPORTED_BY_SERVER)
    {
        //  SFTP url
        build_full_remote_pathname(remote_dir, remote_file, SFTP_IS_SUPPORTED_BY_SERVER, remote_pathname);
        strcpy4(url, "sftp://",  cp.machine, "/", remote_pathname);
    }
    else
    {
        build_full_remote_pathname(remote_dir, remote_file, SFTP_NOT_SUPPORTED_BY_SERVER, remote_pathname);
         // set normal FTP url when SFTP is not supported
        strcpy4(url, "ftp://@", cp.machine, "/", remote_pathname);
    }
}



/*! \fn set_generic_CURL_url() Sets the full URL to be used in the transfer
 *
 * It calls \a check_if_server_supports_preferred_protocol() to check SFTP in the server
 *
 *  \return SFTP_IS_SUPPORTED_BY_SERVER   in case server supports
 *          SFTP_NOT_SUPPORTED_BY_SERVER  otherwise
 */
static int set_generic_CURL_url(char *remote_dir,  /* in */
                                char *remote_file, /* in */
                                char *login_pwd,   /* in */
                                char *url          /* out*/)
{


     build_full_remote_url(remote_dir, remote_file, gl_is_sftp_supported, url);

     FORCE_DBG("cp.machine=%s cp.root_dir=%s cp.login=%s cp.password=%s", cp.machine, cp.root_dir, cp.login, cp.password);
     FORCE_DBG("remote_dir=%s remote_file=%s", remote_dir, remote_file);
     FORCE_DBG("url=%s login_pwd=%s", url, login_pwd);
     return gl_is_sftp_supported;
}



void choose_SFTP_or_FTP(char *remote_dir)
{
    char login_pwd[128] = {0};
    char url[1024] = {0};
    int   attempt_CURLE_REMOTE_ACCESS_DENIED = 0;
    int   sftp_supported = SFTP_IS_SUPPORTED_BY_SERVER;   /* default test if SFTP is supported */

    strcpy3(login_pwd, cp.login, ":", cp.password);

    /* allow a file to set FTP as preferred, modifying default */
    if (access("db_admin/sftp_disable.config", 0) == 0)
    {
        sftp_supported = SFTP_NOT_SUPPORTED_BY_SERVER;
    }
    build_full_remote_url(remote_dir, "dummy", sftp_supported, url);
    gl_is_sftp_supported = check_if_server_supports_preferred_protocol(url, login_pwd, sftp_supported);
    if (gl_curl_return_CURLE_REMOTE_ACCESS_DENIED == 1 && ++attempt_CURLE_REMOTE_ACCESS_DENIED == 1)
    {
        FORCE_DBG("got CURLE_REMOTE_ACCESS_DENIED, trying it again after sleep, perhaps it works ...");
        sleep(1);
        gl_is_sftp_supported = check_if_server_supports_preferred_protocol(url, login_pwd, sftp_supported);
    }
    gl_curl_return_CURLE_REMOTE_ACCESS_DENIED = 0;
    FORCE_DBG("gl_is_sftp_supported=%d", gl_is_sftp_supported);
}


static size_t parallel_read_callback(void *ptr, size_t size, size_t nmem, void *data)
{
    struct tr_data *tr;
    curl_off_t nread;
    size_t ret;

    tr = data;
    if (tr->transferred_size >= tr->total_size)     return(0);
    if (tr->block_size > tr->offset)
    {
        tr->block_size = tr->offset;
    }
    ret = fread(ptr, size, tr->block_size, tr->fp);
    /* FTP grabs the data from ptr and writes it to the remote file */
    nread = (curl_off_t)ret;
    tr->transferred_size += nread;
    tr->offset -= nread;
/*
  fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
          " bytes from file\n", nread);
*/
  return nread;
} /* end parallel_read_callback */


static size_t serial_read_callback(void *ptr, size_t size, size_t nmem, void *data)
{
    struct tr_data *tr;
    size_t ret;

    tr = data;
    ret = fread(ptr, size, nmem, tr->fp);
    tr->transferred_size += ret;

    ft.last_read_return = ret;
    ft.transfer_size    = tr->transferred_size;

    /* Print out the transferred size every 20 meg */
    ++tr->cnt;
    if (tr->cnt == 1 || tr->cnt >= 400)
    {
        FORCE_DBG("ret=%lu size=%lu nmem=%lu tr->transferred_size=%llu tr->block_size=%lu", ret, size, nmem, tr->transferred_size, tr->block_size);
        tr->cnt = 2;
        sprintf(rtn.msg, "Status:      Transferred %llu bytes", tr->transferred_size);
        ftp_status_update(fs.key, rtn.msg);
    } /* end for */
    /* FTP grabs the data from ptr and writes it to the remote file */
  return ret;
} /* end serial_read_callback */


unsigned long long put_finale_file(char *local_file, char *remote_file, char *remote_dir)
{
    char url[1024];
    char login_pwd[128];
    char subject[80];
    CURL *curl;
    CURLcode result;
    const double filesize;
    struct stat file_info;
    unsigned long long speed_upload, total_time;
    FILE *fp;

     DBG();
     if ((fp = fopen(local_file, "rb")) == NULL)
     {
          /* Send error to sys admin and user */
          strcpy5(rtn.msg, rd_open_err, RET2TAB, "'", local_file, "'");
          strcpy2(subject, "Error transferring data to ", cp.name);
          email_send_user(gu.email1, subject, rtn.msg, RD_OPEN_ERR);
          return(FAILURE);
     } /* End if for unable to open local file */
     /* to get the file size */
     if(fstat(fileno(fp), &file_info) != 0)
     {
          return 1; /* can't continue */
     }

    /* Initialize global flags */
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl)
    {
         char error_CURL[CURL_ERROR_SIZE + 256];
         char remote_file_extended[1024];
         int   sftp_supported = 0;  /* default is not supported */
         error_CURL[0] = 0;
         strcpy3(login_pwd, cp.login, ":", cp.password);
         /* set url */
         strcpy2(remote_file_extended, remote_file, ".finale");
         sftp_supported = set_generic_CURL_url(remote_dir, remote_file_extended, login_pwd, url);
         result = set_generic_CURL_options(curl, url, login_pwd, sftp_supported, fp, error_CURL, NULL,  file_info.st_size);
         if(result == CURLE_OK)
         {
              struct curl_slist *slist = NULL;
              char string[2048];
              char remote_pathname[1024];
              // rename from
              if (sftp_supported == SFTP_NOT_SUPPORTED_BY_SERVER)
              {
                    strcpy2(remote_file_extended, remote_file, ".incomplete");
                    //  build_full_remote_pathname(remote_dir, remote_file_extended, sftp_supported, remote_pathname);
                    strcpy2 (string, "RNFR ", remote_file_extended);
                    FORCE_DBG("curl_slist_append(slist, string) with string=%s", string);
                    slist = curl_slist_append(slist, string);
                    // rename to
                    strcpy(string, "RNTO ");
                    strcpy2(remote_file_extended, remote_file, ".split");
                    strcpy2 (string, "RNTO ", remote_file_extended);
                    FORCE_DBG("curl_slist_append(slist, string) with string=%s", string);
                    slist = curl_slist_append(slist, string);
              }
              else
              {
                   strcpy(string, "rename ");
                   // build origin name
                   strcpy2(remote_file_extended, remote_file, ".incomplete");
                   build_full_remote_pathname(remote_dir, remote_file_extended, sftp_supported, remote_pathname);
                   strcat(string, &remote_pathname[2]);  // avoid ~/
                   strcat(string, " ");
                   // build destination name
                   strcpy2(remote_file_extended, remote_file, ".split");
                   build_full_remote_pathname(remote_dir, remote_file_extended, sftp_supported, remote_pathname);
                   strcat (string, &remote_pathname[2]);  // avoid ~/
                   FORCE_DBG("curl_slist_append(slist, string) with string=%s", string);
                   slist = curl_slist_append(slist, string);
              }
              // turn the finale file executable
              strcpy2(remote_file_extended, remote_file, ".finale");
              if (sftp_supported == SFTP_NOT_SUPPORTED_BY_SERVER)
              {
                   strcpy2(string, "site chmod 0777 ", remote_file_extended);
              }
              else
              {
                   build_full_remote_pathname(remote_dir, remote_file_extended, sftp_supported, remote_pathname);
                   strcpy2(string, "chmod 0777 ", &remote_pathname[2]); // avoid ~/
              }
              FORCE_DBG("curl_slist_append(slist, string) with string=%s", string);
              slist = curl_slist_append(slist, string);
              // perform the action
              curl_easy_setopt(curl, CURLOPT_POSTQUOTE, slist);
              result = curl_easy_perform(curl);
              FORCE_DBG("curl_easy_perform(curl)=%d", result);
              /* Free the list no longer needed */
              curl_slist_free_all(slist);
              /* Check for errors */
              if(result != CURLE_OK)
              {
                   fprintf(stderr, "curl_easy_perform() failed: %s\n",
                           curl_easy_strerror(result));
              }
         }
         /* always cleanup */
         curl_easy_cleanup(curl);
         curl_global_cleanup();
    } /* end if for big if */
    return(file_info.st_size);
} /* end put_finale_file */


int put_parallel_block(char *local_file, char *remote_file, char *remote_dir,
    unsigned long long offset, unsigned long long total_size, int block_num)
{
    struct tr_data tr;
    char url[1024];
    char subject[64];
    char login_pwd[128];
    unsigned long long block_size;
    CURL *curl;
    CURLcode result;
    double filesize;
    struct stat file_info;
    double speed_upload, total_time;

    DBG();

    /* Open file to upload */
    if ((tr.fp = fopen(local_file, "rb")) == NULL)
    {
        /* Send error to sys admin and user */
        strcpy5(rtn.msg, rd_open_err, RET2TAB, "'", local_file, "'");
        strcpy2(subject, "Error transferring data to ", cp.name);
        email_send_user(gu.email1, subject, rtn.msg, RD_OPEN_ERR);
        return(FAILURE);
    } /* End if for unable to open local file */
    /* Move to the place where transfer should start */
    if ( fseeko(tr.fp, offset, SEEK_SET) != 0)
    {
        strcpy5(rtn.msg, seek_err, RET2TAB, "'", local_file, "'");
        shut_down(rtn.msg, SEEK_ERR);
        return(FAILURE);
    }

    tr.offset = tr.total_size = total_size;
    tr.transferred_size = 0;
    tr.block_size = 16000;

    strcpy3(login_pwd, cp.login, ":", cp.password);

    /* Initialize global flags */
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl)
    {
         char error_CURL[CURL_ERROR_SIZE + 256];
         int   sftp_supported = 0;  /* default is not supported */
         error_CURL[0] = 0;
         sftp_supported = set_generic_CURL_url(remote_dir, remote_file, login_pwd, url);
         result = set_generic_CURL_options(curl, url, login_pwd, sftp_supported, &tr, error_CURL, parallel_read_callback, total_size);
         if(result == CURLE_OK)
         {
             unsigned long buffer_size = 256000L; // 250 KB
             FORCE_DBG("set buffer=%lu curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, buffer_size)= %d", buffer_size, curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, buffer_size));
             result = curl_easy_perform(curl);
             FORCE_DBG("curl_easy_perform(curl)=%d", result);
         }
          /* Check for errors */
          if(result != CURLE_OK)
          {
               sprintf(rtn.msg, curl_easy_strerror(result));
               data_log("out", rtn.msg);
               filesize = 0;
               FORCE_DBG("ERROR: transfer failed curl code = %d", result);
          }
          else
          {
                    /* Extract uploaded size */
                    result = curl_easy_getinfo(curl,  CURLINFO_SIZE_UPLOAD,  &filesize);
                    if (result != CURLE_OK)
                    {
                              filesize = 0;
                              /* build error message */
                              sprintf(rtn.msg, curl_easy_strerror(result));
                              FORCE_DBG("ERROR: transfer size does not match");
                    } /* end inner else */
           } /* end else for transfer completed */
           /* always cleanup */
          curl_easy_cleanup(curl);
          curl_global_cleanup();
     }

    fclose(tr.fp);
    if (total_size == (unsigned long long)filesize)
    {
        DBG("Will return SUCCESS");
        return(SUCCESS);
    }
    else
    {
        DBG("Will return FAILURE");
        return(FAILURE);
    }
} /* end put_parallel_block */


int put_serial_block(char *local_file, char *remote_file, char *remote_dir,
    FILE *fp, unsigned long long total_size, int block_num)
{
    struct tr_data tr;
    char url[1024];
    char login_pwd[128];
    CURL *curl;
    CURLcode result;
    double filesize = 0;

    FORCE_DBG("block_num=%d", block_num);
    FORCE_DBG("remote_dir=%s remote_file=%s", remote_dir, remote_file);

    /* Point to the stream pointer for the file */
    tr.fp = fp;

    tr.offset = tr.total_size = total_size;
    tr.transferred_size = 0;
    tr.block_size = 16000;

    strcpy3(login_pwd, cp.login, ":", cp.password);
    rtn.msg[0] = 0;
    /* Initialize global flags */
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    DBG("curl_easy_init()=%p", curl);
    if(curl)
    {
        char error_CURL[CURL_ERROR_SIZE + 256];
        int   sftp_supported = 0;  /* default is not supported */
        error_CURL[0] = 0;
        /* set url */
        sftp_supported = set_generic_CURL_url(remote_dir, remote_file, login_pwd, url);
        tr.cnt = 0;
        result = set_generic_CURL_options(curl, url, login_pwd, sftp_supported, &tr, error_CURL, serial_read_callback, total_size);
        /*
         * Carlos Mazieri March 2018
         *  This option CURLOPT_FTPAPPEND  seems to not work for SFTP even it is not necessary
        if(result == CURLE_OK)
        {
                result = curl_easy_setopt(curl, CURLOPT_FTPAPPEND, 1L);
                FORCE_DBG("curl_easy_setopt(curl, CURLOPT_FTPAPPEND, 1L)=%d", result);

        }
        */
        if(result == CURLE_OK)
        {
            unsigned long buffer_size = (long)LOCAL_CURL_UPLOAD_BUFFER_SIZE;
            DBG("LIBCURL_VERSION_NUM= %x", LIBCURL_VERSION_NUM);
            DBG("LIBCURL_VERSIONN= %s", LIBCURL_VERSION);
            FORCE_DBG("LIBCURL_VERSION= %s", curl_version());
            FORCE_DBG("CURL_MAX_WRITE_SIZE= %d", CURL_MAX_WRITE_SIZE);

#if LIBCURL_VERSION_NUM >= 0x073E00
            DBG("LIBCURL_VERSION_NUM >= 7.62.0");
            FORCE_DBG("set buffer=%lu curl_easy_setopt(curl, CURLOPT_UPLOAD_BUFFERSIZE, buffer_size)= %d", buffer_size, curl_easy_setopt(curl, CURLOPT_UPLOAD_BUFFERSIZE, buffer_size));
#else
            /*
             * versions prior of 7.62.0. can use  CURLOPT_BUFFERSIZE, it was set using CURL from TWW
             *     set buffer=4096000 curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, buffer_size)= 0
             */
             DBG("LIBCURL_VERSION_NUM < 7.62.0");
             FORCE_DBG("set buffer=%lu curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, buffer_size)= %d", buffer_size, curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, buffer_size));
#endif
            /* Perform the transactions */
            // we do not set result here due to Curl Lib may have some problems,
            // we trust in curl_easy_getinfo(curl,  CURLINFO_SIZE_UPLOAD,  &filesize);
            CURLcode perform_result =  curl_easy_perform(curl);
            FORCE_DBG("curl_easy_perform(curl)=%d", perform_result);
            result = perform_result;

        }
        /* Check for errors */
        if(result == CURLE_OK)
        {
            /* Extract uploaded size */
            filesize = 0;
            result = curl_easy_getinfo(curl,  CURLINFO_SIZE_UPLOAD,  &filesize);
            FORCE_DBG("curl_easy_getinfo(curl,  CURLINFO_SIZE_UPLOAD,  &filesize)=%d filesize=%lf", result, filesize);
        } /* end else for transfer completed */
        //now check for any error
        if(result != CURLE_OK)
        {
            if (result != CURLE_REMOTE_FILE_NOT_FOUND || strlen(remote_dir) == 0)
            {
                sprintf(rtn.msg, curl_easy_strerror(result));
            }
            FORCE_DBG("Curl ERROR: %s", curl_easy_strerror(result));
            FORCE_DBG("extended ERROR: %s", error_CURL);

            /*
             * we do not set filesize = 0  because libCurl has some problems
             *
             * we trust in curl_easy_getinfo(curl,  CURLINFO_SIZE_UPLOAD,  &filesize)
             **/
           // filesize = 0;
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }//end curl ok

    // Jan 2020, Synopys has blocked creating directories on their server,
    //
    if (result == CURLE_REMOTE_FILE_NOT_FOUND && strlen(remote_dir) > 0)
    {
        FORCE_DBG("receiving CURLE_REMOTE_FILE_NOT_FOUND, trying to work around ...");
        return result;
    }

    if (total_size == (unsigned long long)filesize)
    {
        DBG("Will return SUCCESS");
        return(SUCCESS);
    }
    if (rtn.msg[0] == 0) // no message
    {
        _priv_set_transfer_error_msg("set DEBUG to see libCurl verbose messages");
    }
    DBG("Will return FAILURE");
    return(FAILURE);
} /* end put_serial_block */

      /* now extract transfer info */
/*
      curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed_upload);
      curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
      fprintf(stderr, "Speed: %.3f bytes/sec during %.3f seconds\n",
              speed_upload, total_time);
        printf("filesize %s: %0.0f bytes\n", "readme", filesize);
*/


/* This function prints the in the ds script the command to download alternative GDS name*/
void print_download_expected_gds_name(FILE *fp, char* tag_name, char* stored_gdb_name, const char* expected_gds_name)
{
    fprintf(fp, "\n# -- if could not get the GDS yet, try using the name '%s'\n", expected_gds_name);
    fprintf(fp, "if [ ! -f \"%s\" ]\n", stored_gdb_name);
    fprintf(fp, "then\n");
    fprintf(fp, "    dssc co -version %s -get %s\n", tag_name, expected_gds_name);
    fprintf(fp, "fi\n");
    fprintf(fp, "# -- if succeeded getting '%s'\n", expected_gds_name);
    fprintf(fp, "if [ -f \"%s\" ]\n", expected_gds_name);
    fprintf(fp, "then\n");
    fprintf(fp, "    /bin/mv %s %s\n", expected_gds_name, stored_gdb_name);
    fprintf(fp, "fi\n");
}



static int priv_ds_checkout(char *tag_name, char *file_keyword, char *mask, char *technology, char *design, char *gdb_extension, char *file_name)
    {
    char stored_gdb_name[24];
    char script_file[64];
    char command[FILE_SIZE];
    time_t tmp_time;
    FILE *fp;

    strcpy3(stored_gdb_name, "GDB.", gdb_extension, ".gz");

    /* Build script to retrieve data from Dessync */
    /* Build script name */
    sprintf(script_file, "ds_link_script%d.sh", tr.pid);
    DBG("script_file=%s" , script_file);
    /* Open the script for writing */
    if ((fp = fopen(script_file, "w")) == NULL)
    {
          s2f_close_down(wr_open_err, script_file, WR_OPEN_ERR);
    }
   
    fprintf (fp,  "#! /bin/sh\n");
    fprintf (fp,  "#\n");
    fprintf (fp,  "# this script is auto generated \n");
    fprintf (fp,  "#\n");
    fprintf(fp, "dssc setvault sync://sync-15056:15056/Projects/repository/%s/%s_%s/design .\n",
    technology, mask, design);
    fprintf(fp, "dssc co -version %s -get %s\n", tag_name, stored_gdb_name);

    fprintf(fp, "\n## --  June 2019, let's handle gds_out which can be encrypted with gpg\n");

    print_download_expected_gds_name(fp, tag_name, stored_gdb_name, "GDB.gds.gpg");
    print_download_expected_gds_name(fp, tag_name, stored_gdb_name, "GDB.gds.gpg.gz");
    print_download_expected_gds_name(fp, tag_name, stored_gdb_name, "GDB.gds.gz.gpg");
    print_download_expected_gds_name(fp, tag_name, stored_gdb_name, "GDB.gpg.gz");
    print_download_expected_gds_name(fp, tag_name, stored_gdb_name, "GDB.gpg");

    fprintf(fp, "\n\nif [ ! -f \"%s\" ]\n", stored_gdb_name);
    fprintf(fp, "then\n");
    fprintf(fp, "    exit 1\n");
    fprintf(fp, "fi\n\n");

    fprintf (fp,  "\n");
    fprintf (fp,  "#\n");
    fclose(fp);
    /* Make script executable */
    chmod(script_file, 0700);
    /* Build command to execute */
    strcpy3(command, "./", script_file, " >ds_output 2>&1");
    tmp_time = time(0);
    FORCE_DBG("dssc setvault sync://sync-15056:15056/Projects/repository/%s/%s_%s/design", technology, mask, design);
    FORCE_DBG("dssc co -version %s -get %s", tag_name, stored_gdb_name);
    FORCE_DBG("command=%s", command);
    system(command);
    sprintf(rtn.msg, "Ran %s in %d seconds\n", command, time(0) - tmp_time);
    FORCE_DBG("%s" , rtn.msg);
    /* Move file to real name */
    /* Build name for file */
    return rename(stored_gdb_name, file_name);
}

/*!
 * \brief priv_get_gds_from_pool() if the file exists in the pool creates a link to the file such as:
 *                                 /bin/ln -sf /home/dtms_pool/pool/2/124658_196509 ../ghba6925t.gz
 * \param gdb_id
 * \param out_file_name
 * \return SUCESS or FAILURE
 */
static int priv_get_gds_from_pool(char *gdb_id, char *out_file_name)
{   
    char query[1024];
    char pool_uid[24];
    char jobnumber_a[24];
    char jobnumber_b[24];
    char subpool_uid[24];
    int ret = FAILURE;
    sprintf(query, "SELECT pool_master_uid, subPoolUid, a.jobnumber, b.jobnumber "
                   "FROM   repository.repository_intoThePool as a, "
                   "       pool.pool_master as b "
                   "WHERE   repository_uid = %s "
                   "AND (file = '%s' or type = 'oas.gz' or type = 'oasis.gz') "
                   "AND masterUid=pool_master_uid AND a.active AND b.active limit 1"
                , gdb_id, fs.real_name);
    DBG("query=%s", query);
    ret = sql4column_rtv(query, pool_uid, subpool_uid, jobnumber_a, jobnumber_b);
    FORCE_DBG("sql4column_rtv()=%d", ret);
    if (ret == SUCCESS)
    {       
        char script_file[64];
        FILE *fp;
        char command[FILE_SIZE];
        sprintf(script_file, "cp_from_pool_script%d.sh", tr.pid);
        DBG("script_file=%s" , script_file);
        if ((fp = fopen(script_file, "w")) == NULL)
        {
              s2f_close_down(wr_open_err, script_file, WR_OPEN_ERR);
        }
        fprintf (fp,  "#!/bin/sh\n");
        fprintf (fp,  "#\n");
        fprintf (fp,  "# this script is auto generated, it gets a GDS from Data Pool\n");
        fprintf (fp,  "#\n");
        fprintf(fp, "/bin/cp -v /home/dtms_pool/pool/%s/%s_%s %s\n\n",
                 subpool_uid, jobnumber_a, pool_uid, out_file_name);
        fclose(fp);
        /* Make script executable */
        chmod(script_file, 0700);
        sprintf(command, "./%s > cp_gds_ouptut.txt 2>&1", script_file);
        FORCE_DBG("command=%s", command);
        ret = system(command);
        ret = WEXITSTATUS(ret);
    }
    FORCE_DBG("ret=%d", ret);
    return ret;
}

/* gets the GDS either from Data Pool or Design Sync 
       returns gds_size
*/
unsigned long long get_gds(char *gdb_id, char *file_keyword)
{   
    struct stat buf;
    
    char file_name[24];
   
    char tag_name[64];
    char mask[8];
    char design[64];
    char technology[64];   
    char gdb_extension[12];   

    DBG();

    /* Retrieve the file name, mask, etc. */
    sql_mtd_rtv(gdb_id, fs.real_name, tag_name, mask, technology, design, gdb_extension);

    
#if defined(DEBUG)  
    {
        DBG("cwd=%s", get_current_dir_name());
    }
#endif    
    
    /* If file not present, return failure */
    strcpy3(file_name, "../", file_keyword, ".gz");
    unlink(file_name);
    
    /* use fs.real_name as it is*/
    priv_get_gds_from_pool(gdb_id, file_name);
    if (stat(file_name, &buf) != 0)
    {
       priv_ds_checkout(tag_name, file_keyword, mask, technology, design, gdb_extension, file_name);
    }
     
    if (stat(file_name, &buf) != 0)
    {
        return(0);
    }

    /* Add the .gz suffix to fs.real_name if not already present */
    if (strcasestr(fs.real_name, ".gz") == NULL && strcasestr(fs.real_name, ".gpg") == NULL)
    {
        strcat(fs.real_name, ".gz");
    }
    return(buf.st_size);
}



/* Send data to the foundry */
/* Data is checked out from Data Pool or DesignSync by calling get_gds() */
/* GDB ID comes over in the key */
/* It is then put in global_mask */
int gdb_checkout(char *remote_dir, char *keyword)
{
    extern TRANS_FILE_STAT fs;
    FILE *fp;
    extern char lines[TRANS_LINE_NUM][TRANS_LINE_LEN];
    char data_dir[32];
    char local_gdb_id[8];

    FORCE_DBG("fs.original_size=%lld ft.gdb_id=%d fs.real_name=%s", fs.original_size, ft.gdb_id, fs.real_name);

    /* Read in the global data */
    fp = read_big_file(keyword, lines, &fs);
    /* See if check out is needed */
    /* Only do check out if file size is 0, GDB ID is non zero */
    /* and file name is GDB.gds.gz */
    if ((fs.original_size != 0)
    || (ft.gdb_id == 0)
    || (strcasecmp(fs.real_name, "GDB.gds.gz") != 0))
    {
	sprintf(rtn.msg, "Check out not performed for the following data:\n\n\
GDB ID:  %d\nFile name:  %s\nFile size %llu\n",
	ft.gdb_id, fs.real_name, fs.original_size);
	data_log(tr.log_file, rtn.msg);
	fclose(fp);
	return(DBNO);
    }
    /* Check the data out from DesignSync */
    /* Build the directory name */
    sprintf(data_dir, "data_files%d", fs.disk_index);
    chdir(data_dir);
    /* Make a directory under the keyword */
    mkdir(keyword, 0700);
    chdir(keyword);
    /* Convert GDB ID to string format */
    sprintf(local_gdb_id, "%d", ft.gdb_id);
    fs.original_size = get_gds(local_gdb_id, keyword);
    
    /* Move back to server directory */
    chdir("../..");
    /* If file size is 0, check out failed */
    if (fs.original_size == 0)
    {
        const char *failure = "File not correctly checked out";
        FORCE_DBG("check out failed");
        s2f_close_down("GDS check out from repository failed", "", -1);
    }
    else
    {
    /* rtn.msg was initialized in previous function */
	data_log(tr.log_file, rtn.msg);
	data_log(tr.log_file, "file successfully checked out.");
    } /* end else */
    /* Indicate this is a compressed file */
    fs.compressed_size = fs.original_size;
    /* Must move the GDS file to the keyword */
    /* Update the file name in the global file */
    strcpy(lines[1]+13, fs.real_name);
    /* Update the file size in the global data file */
sprintf(lines[2], "User size:   %llu bytes", fs.original_size);
sprintf(lines[3], "Compressed:  %llu bytes", fs.compressed_size);
    DBG("Before call write_big_file()");
    write_big_file(fp, lines, &fs);
    DBG("after call write_big_file()");
} /* end gdb_checkout */
#endif
#endif
