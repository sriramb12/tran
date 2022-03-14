#ifdef SSL_ENCRYPTION
#include "common.h"
#include <openssl/err.h>
#include "global_defines.h"
#include "rtn.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "transapi_defines.h"

#include "global_debug.h" // Mazieri's debug facilities


BIO *bio_err=0;

/*static char *pass;*/
/*static int password_cb(char *buf,int num, int rwflag,void *userdata);*/

#ifndef SOLARIS10
static void sigpipe_handle(int x);
#endif

/* work around for intercom_lib.c */
#if !defined(KEYFILE)
#define KEYFILE "trans.ssl.pem"
#endif


/*The password code is not thread safe*/
/*static int password_cb(char *buf,int num, int rwflag,void *userdata)
{
    DBG();

    if(num<strlen(pass)+1)
        return(0);

    strcpy(buf,pass);
    return(strlen(pass));
}*/

static void sigpipe_handle(int x){
    DBG();
}

/*
 * Aug 2018, Carlos Jose Mazieri
 * Clients must not use Keyfile,  it receives NULL right now to keep old compatibility of Servers
 */
SSL_CTX *initialize_ctx(char *not_used_parameter /* it used to be keyfile*/, char *password)
{
    SSL_METHOD *meth;
    SSL_CTX *ctx;

    if (!bio_err) {
        DBG("!bio_err");
        /* Global system initialization*/
	    /* Initialize the library */
        SSL_library_init();

        /* Load the error strings for printing when error occurs */
        SSL_load_error_strings();

        /* An error write context */
        bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
    }

    /* Set up a SIGPIPE handler */
#ifndef SOLARIS10
    signal(SIGPIPE, sigpipe_handle);
#endif

    DBG("forcing SSLv23");
    /* Create our context*/
    meth = SSLv23_method();
    //meth = TLSv1_method();

    /* Get a context object */
    ctx = SSL_CTX_new(meth);
    DBG("ctx=%p", ctx);

    /*
     * Changing the SSL context to not include export ciphers
     */
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);
    if (SSL_CTX_set_cipher_list(ctx, "HIGH:!EXPORT") != 1) {
       DBG("ERROR on SSL_CTX_set_cipher_list() ctx=%p", ctx);
       ERR_print_errors_fp(stderr);
       exit(1);
    }

    DBG("return ctx=%p", ctx);
    return ctx;
  }

     
void destroy_ctx(SSL_CTX *ctx)
{
    DBG();
    SSL_CTX_free(ctx);
}

/* Build the name for the certificate key file */
/* If this was set in the .icomrc file, use this name */
/* Otherwise check home dir, bin dir, and current working dirrectory */
void ssl_keyfile_name_build(char *keyfile)
{
    extern char certificate_dir[];
    extern char certificate_file[];
    char root_file[FILE_SIZE];

    DBG();

    /* Build the root file name first */
    if (certificate_file[0] != '\0') {
        strcpy(root_file, certificate_file);
    } else {
        strcpy(root_file, KEYFILE);
    }

    /* Now set the directory name */
    if (certificate_dir[0] != '\0') 
    {
	    strcpy3(keyfile, certificate_dir, "/", root_file);

	    /* Now try accessing the file */
	    if (access(keyfile, 0) == 0) return;
    } /* end if */

    /* If could not find the file, next try getting it from the home directory */
    strcpy3(keyfile, user.home_dir, "/", root_file);
    if (access(keyfile, 0) == 0) return;

    /* Next try getting it from the local bin directory */
    strcpy3(keyfile, user.home_dir, "/bin/", root_file);
    if (access(keyfile, 0) == 0) return;

    /* If still failed to find it, use the copy from the current working directory */
    else strcpy(keyfile, root_file);

} /* end keyfile_name_build */
#endif
