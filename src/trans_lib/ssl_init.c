#ifdef SSL_ENCRYPTION
/****************************************************************************
 *
 * Function:	init_socket
 *
 * Description:	Initializes SSL socket
 *
 ***************************************************************************/

#include <errno.h>
#include "common.h"
#include "rtn.h"
#include "server_params.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "intercom_server_prototypes.h"
#include "trans_server_params.h"
#include "global_debug.h"

/* yes, a hack to make it build again :P */
#define HELP_MSG "For assistance, contact InterCom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com\n\n\
To submit a help ticket, go to:\n\n\
https://servicedesk.nxp.com "


static int s_server_session_id_context = 1;
SSL_CTX *ctx;
extern int trans_sock;
BIO *sbio;

void check_cert(SSL *trans_ssl, char *host)
{
    int ret;
    X509 *peer;
    char peer_cn[256];

    DBG("host='%s'", host);

    if(((ret = SSL_get_verify_result(trans_ssl)) != X509_V_OK) && (ret != 18))
        /* 18 means that it is a self signed certificate */
        shut_down(invalid_certificate, INVALID_CERTIFICATE);

    /*Check the cert chain. The chain length is automatically checked by OpenSSL when
      we set the verify depth in the ctx */

    /*Check the common name*/
    peer = SSL_get_peer_certificate(trans_ssl);
    X509_NAME_get_text_by_NID(X509_get_subject_name(peer), NID_commonName, peer_cn, 256);

    if(strcasecmp(peer_cn, host))
    {
        DBG("FAIL to strcasecmp() peer_cn='%s' and host='%s'", peer_cn, host);
	    sprintf(rtn.msg, host_mismatch_err, peer_cn, host);
        /* 	shut_down(rtn.msg, -1); */
    }
} /* End check_cert */


int ssl_init(char *machine, char *address, int socket)
{
    extern char key_file[];
    int ret;
    extern SSL *trans_ssl;

    DBG("machine='%s', address='%s', socket='%d'", machine, address, socket);

    /* Build our SSL key */
    /* Build our SSL context*/
    /* this function sets up the library and the ctx structure */
    /* It is found in ssl_common.c */
    /* the key_file is set when global params are passed in */
    ctx=initialize_ctx(key_file,PASSWORD);

    /* Set our cipher list */
    SSL_CTX_set_session_id_context(ctx, (void*)&s_server_session_id_context, sizeof s_server_session_id_context);

    /* Connect the TCP socket*/
    trans_sock=tcp_connect(machine,address, socket);

    /* Allocate structure needed for the SSL connection */
    /* the structure gets its attributes from the underlying ctx structure */
    trans_ssl = SSL_new(ctx);

    /* Create a new BIO with a new socket */
    sbio=BIO_new_socket(trans_sock,BIO_NOCLOSE);

    /* Connect the SSL structure with the BIO */
    /* this function cannot fail */
    SSL_set_bio(trans_ssl,sbio,sbio);

    if ((ret = SSL_connect(trans_ssl)) != 0)
    {
        DBG("SSL_connect() ret = '%d'", ret);
        sprintf(rtn.msg, connect_err, errno, server.machine, server.address);
	    return(CONNECT_ERR);
    }

    /* Check the SSL certificate */
    check_cert(trans_ssl,server.machine);

    return(SUCCESS);
} /* end ssl_init */


/* Call this when exiting connection */
void ssl_exit(void)
{
    DBG();
    destroy_ctx(ctx);

    exit(0);
}


/* This is the connect that is similar to that in init_socket */
int tcp_connect(char *host, char *address, int port)
{
    struct sockaddr_in addr;
    extern int sock;

    DBG("host='%s', address='%s', port='%d'", host, address, port);

    memset(&addr,0,sizeof(addr));

    /* Copy in the IP node address */
    ip_addr_copy((char *)&addr.sin_addr, address);
    addr.sin_family=AF_INET;
    addr.sin_port=port;

    if ((sock=socket(AF_INET,SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
	    sprintf(rtn.msg, bad_sock_open, errno);
	    strcpy2(rtn.msg+strlen(rtn.msg), DOUBLE_RET, HELP_MSG);
        return(BAD_SOCK_OPEN);
    }

    if( connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
	    sprintf(rtn.msg, connect_err, errno, server.machine, server.address);
	    return(CONNECT_ERR);
    } /* End if */

    return sock;
} /* End tcp_connect */

/* Check that the common name matches the host name*/
/* this verifies that the server name and certificate are valid */
#endif
/*es.files.fileuse*/
/*is.files.fileuse*/
/*ts.files.fileuse*/
