#ifdef SSL_ENCRYPTION
#include <errno.h>
#include "common.h"
#include <openssl/err.h>
#include "global_defines.h"
#include "global_server_errs.h"
#include "global_debug.h" // Mazieri's debug facilities


BIO *bio_err=0;
static char *pass;
static int password_cb(char *buf,int num,
int rwflag,void *userdata);
static void sigpipe_handle(int x);

/* A simple error and exit routine*/
int err_exit(string)
  char *string;
  {
    DBG();
    fprintf(stderr,"%s\n",string);
    exit(0);
  }

/* Print SSL errors and exit*/
int berr_exit(string)
  char *string;
  {
    DBG();
    BIO_printf(bio_err,"%s\n",string);
    ERR_print_errors(bio_err);
    exit(0);
  }

/*The password code is not thread safe*/
static int password_cb(char *buf,int num,
  int rwflag,void *userdata)
  {
    DBG();
    if(num<strlen(pass)+1)
      return(0);

    strcpy(buf,pass);
    return(strlen(pass));
  }

static void sigpipe_handle(int x){
}

SSL_CTX *initialize_ctx(char *keyfile, char *password)
{
    extern char ca_list[];
    SSL_METHOD *meth;
    SSL_CTX *ctx;
    int ret;

    DBG();

    if(!bio_err){
        /* Global system initialization*/
	    /* Initialize the library */
        SSL_library_init();

        /* Load the error strings for printing when error occurs */
        SSL_load_error_strings();

        /* An error write context */
        bio_err = BIO_new_fp(stderr,BIO_NOCLOSE);
    }

    /* Set up a SIGPIPE handler */
    signal(SIGPIPE,sigpipe_handle);

    /* Create our context*/
    meth = SSLv23_method();

    /* Get a context object */
    ctx = SSL_CTX_new(meth);

/*
 * Jan 2020,  Carlos Mazieri
 *
 * As required by HCL_NXP_IT_Security@nxp.com it is necessary to reduce the Vulnerabilities of our servers
 *
 * The ideal would be leaving only  TLSv1_2 as recomended, but, it also requires client changes
 */
{
    long mask_method  = 0;
#if defined(SSL_OP_NO_SSLv2)
    mask_method = SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
    DBG("mask_method=%ld", mask_method);
#endif
#if defined(SSL_OP_NO_SSLv3)
    mask_method = SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv3);
    DBG("mask_method=%ld", mask_method);
#endif

#if defined (INTERCOM) && defined(EXTERNAL)
#if defined(SSL_OP_NO_TLSv1)
    mask_method = SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1);
    DBG("mask_method=%ld", mask_method);
#endif
#if defined(SSL_OP_NO_TLSv1_1)
    mask_method = SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1_1);
    DBG("mask_method=%ld", mask_method);
#endif
#endif
}


    /* Load our keys and certificates*/
    if (!(SSL_CTX_use_certificate_chain_file(ctx, keyfile)))
    {
	    printf(certificate_err, keyfile);
	    exit(CERTIFICATE_ERR);
    }

    pass = password;

    /* If the password is encrypted, this decrypts the password */
    SSL_CTX_set_default_passwd_cb(ctx, password_cb);

    if ((ret = SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM)) < 1)
    {
	    printf(key_file_err, errno, keyfile);
	    exit(KEY_FILE_ERR);
    }

    /* Load the CAs we trust*/
    /* Load the certificate authorities we trust */
    if (!(SSL_CTX_load_verify_locations(ctx, ca_list, 0)))
    {
	    PRINTF(ca_list_err);
	    exit(CA_LIST_ERR);
    }

#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(ctx, 1);
#endif

    DBG("will return a ctx");
    return ctx;
  }

void destroy_ctx(ctx)
  SSL_CTX *ctx;
  {
    DBG();
    SSL_CTX_free(ctx);
  }
#endif
