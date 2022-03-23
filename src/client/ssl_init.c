
/****************************************************************************
*
* Function:	ssl_init
*
* Description:	Initializes SSL socket
*
***************************************************************************/
#include <errno.h>
#include "common.h"
#include "rtn.h"
#include "server_params.h"
#include "ssl_client_lib.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "trans_server_errs.h"
#include "x500_defines.h"
#include "openssl/err.h"

#include "global_debug.h" // Mazieri's debug facilities

#define HELP_MSG "For assistance, contact Intercom administrator:\n\n\
               Name:  Transcend Team\n\
               Email Address:  transbug@nxp.com"


static int s_server_session_id_context = 1;
SSL_CTX *ctx;
int ssl_sock;
BIO *sbio;

extern int gl_is_web;  // special flag for transweb

void ssl_keyfile_name_build(char *keyfile);
int ip_lookup(char *string, char *address, char *name);


#ifdef EXTERNAL
void check_cert( SSL *ssl, char *host)
{
    DBG();
    int ret;
    X509 *peer;
    char peer_cn[256];

    if((ret = SSL_get_verify_result(ssl))!=X509_V_OK)
       shut_down(invalid_certificate, INVALID_CERTIFICATE);
    /*Check the cert chain. The chain length
      is automatically checked by OpenSSL when
      we set the verify depth in the ctx */
    /*Check the common name*/
    peer=SSL_get_peer_certificate(ssl);
    X509_NAME_get_text_by_NID
      (X509_get_subject_name(peer),
      NID_commonName, peer_cn, 256);
    if(strcasecmp(peer_cn,host))
     shut_down("err", -1);
} /* End check_cert */

#else
/*This is for Transcend and internal InterCom */
void check_cert(SSL *ssl, char *host)
{
    DBG("host = '%s'", host);
    int ret;

    /* 18 means that it is a self signed certificate */
    /* 19 means that the root cannot be found locally */
    /* 20 means unable to get local issuer or certificate */
    /* This occurs when the client does not have a copy of the certificate */
    /* The NXP clients to not keep a copy of the certificate */
    if ( ( (ret = SSL_get_verify_result(ssl) ) != X509_V_OK) && (ret != 18) && (ret != 19) && (ret != 20) ) {
        DBG("Invalid certificate");
        shut_down(invalid_certificate, -1);
    }

    DBG("check_cert() passed normally");
} /* End check_cert */

#endif /* for Transcend or internal InterCom */

#if defined(LOCAL_TEST) || defined(LOCAL_TEST_SERVER) || defined(LOCAL_TEST_PORT)
# if !defined(LOCAL_TEST_COUNTER)
#  define  LOCAL_TEST_COUNTER   1000 // give it a high number
#endif
    static int     local_test_counter = LOCAL_TEST_COUNTER;
#endif

int ssl_init(char *buffer )
{  
    int ret;
    extern SSL *ssl;

    /* Build our SSL context*/
    /* this function sets up the library and the ctx structure */
    /* It is found in ssl_common.c */
    ctx = initialize_ctx(NULL, PASSWORD);
    SSL_CTX_set_session_id_context(ctx, (void*)&s_server_session_id_context, sizeof s_server_session_id_context);

    DBG("before checking compiler options for host and port machine=%s Host:Port=%s:%d", server.machine, server.address, ntohs(server.socket));

#if defined(LOCAL_TEST) || defined(LOCAL_TEST_SERVER) || defined(LOCAL_TEST_PORT)
    DBG("local_test_counter=%d", local_test_counter);
    if (local_test_counter-- > 0)
    {
# if defined(LOCAL_TEST_PORT)
        server.socket = htons(LOCAL_TEST_PORT);
# endif

# if defined(LOCAL_TEST) || defined(LOCAL_TEST_SERVER)
#  if defined(LOCAL_TEST_SERVER)
        strcpy(server.machine, LOCAL_TEST_SERVER);
        if (strcmp(server.machine, "127.0.0.1") == 0)
        {
             strcpy(server.address, "127.0.0.1");
        }
#  else
       strcpy(server.address, "127.0.0.1");
       strcpy(server.machine, "127.0.0.1");
#  endif // if defined(LOCAL_TEST_SERVER
# endif // if defined(LOCAL_TEST) || defined(LOCAL_TEST_SERVER
        if (gl_is_web == 0)
        {
             printf("[LOCAL_TEST] %s:%d using local test version server.machine=%s server.address=%s server.socket=%d\n",
                     __FILE__, __LINE__, server.machine,server.address, ntohs(server.socket) );
        }
    } // if (local_test_counter > 0)
# if defined(DEVEL_USING_SINGLE_PROCESS) // give more time to the server, if in a debug session
          sleep(10);
# endif
#endif // defined(LOCAL_TEST) || defined(LOCAL_TEST_SERVER) || defined(LOCAL_TEST_PORT)

    DBG("Connecting to Server %s whose IP:Port is %s:%d", server.machine, server.address, ntohs(server.socket));

    ssl_sock = tcp_connect(server.machine, server.address, server.socket);
    if (ssl_sock <= 0)
    {
        sprintf(rtn.msg, ssl_connect_err, errno, server.machine, server.address, ntohs(server.socket));
        DBG("Socket failed to connect, error=%d", ssl_sock);
        return ssl_sock;  //
    }
    DBG("Socket connected, going to initialize SSL");

    /* Allocate structure needed for the SSL connection */
    /* the structure gets its attributes from the underlying ctx structure */
    ssl = SSL_new(ctx);

    DBG("CTX struct initialized");

    /* Create a new BIO with a new socket */
    sbio = BIO_new_socket(ssl_sock, BIO_NOCLOSE);

    DBG("BIO struct initialized");

    /* Connect the SSL structure with the BIO */
    /* this function cannot fail */
    SSL_set_bio(ssl, sbio, sbio);
    DBG("Connected BIO and SSL struct");

    if ( (ret = SSL_connect(ssl)) < 0)
    {
        sprintf(rtn.msg, ssl_connect_err, errno, server.machine, server.address, ntohs(server.socket));
        DBG("SSL_connect() gives a error, SSL_get_error(SSL, ret='%d')='%d'", ret, SSL_get_error(ssl, ret));
        DBG("SSL_Error is: '%s'", ERR_error_string(SSL_get_error(ssl, ret), NULL));
        DBG("Transcend error will be rtn.msg='%s'", rtn.msg);
        return(CONNECT_ERR);
    }

    DBG("SSL Connection created");

    /* Check the SSL certificate */
    DBG("before call check_cert()");
    check_cert(ssl, server.machine);

    DBG("after call check_cert(), will pass execution to trans_header_send()");
    return( trans_header_send(buffer) );
} /* end ssl_init */

/* Call this when exiting connection */
void ssl_exit(void)
{
    DBG();
    destroy_ctx(ctx);

    exit(0);
  }


/* This is the connect that is similar to that in init_socket */
int tcp_connect(char *host,char *address, int port)
{
    struct sockaddr_in addr;
    extern int sock;

    DBG("host='%s', address='%s', port='%i'", host, address, ntohs(port));

    memset(&addr, 0, sizeof(addr));

    /* Look up the IP node address */
    ip_lookup( (char *)&addr.sin_addr, address, host);

    addr.sin_family = AF_INET;
    addr.sin_port = port;

    if ( (sock = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP )) < 0 )
    {
        DBG("Failed to open socket");
        sprintf(rtn.msg, bad_sock_open, errno);
        strcpy2(rtn.msg+strlen(rtn.msg), DOUBLE_RET, HELP_MSG);

        return(BAD_SOCK_OPEN);
    }

    if ( connect( sock, (struct sockaddr *)&addr, sizeof(addr) ) < 0 )
    {
        DBG("Failed to open socket");
        sprintf(rtn.msg, ssl_connect_err, errno, server.machine, server.address, ntohs(server.socket));

        return(CONNECT_ERR);
    } /* End if */

    DBG("socket connected, will return socket description");
    return sock;
} /* End tcp_connect */

#define MAX_ATTEMPTED_SERVERS  15

short intermediate_ssl_init(char req_type, char *buffer)
{
    extern TRANS_TOOL tool;
    short ret;

    DBG();

    /* First try to connect to server set up in lookup_company */
    ret = ssl_init(buffer);
    // if connection failed try other options if set in the "repository" table
    if ((ret == CONNECT_ERR || ret == WRONG_SERVER) && (req_type != DBPING && req_type != DBSILENT_PING))
    {
        SERVER_PARAMS  attempted_servers[MAX_ATTEMPTED_SERVERS];  // save to compare before connecting
        int cnt = tool.repository_index[0];
        int attempted_counter = 0;
        int already_attempted_counter = 0;
        attempted_servers[attempted_counter++] = server;  // save current server
        int first_connect_error  = ret;
        while ((ret == CONNECT_ERR || ret == WRONG_SERVER) && (cnt <= tool.repository_index[1]))
        {
            if (ret == WRONG_SERVER)
            {
                if (sscanf(rtn.msg, "%s %s %s %d", server.site, server.machine, server.address, &server.socket) == 4)
                {                                      
                    server.socket = htons(server.socket);
                }
                else
                {
                    ret = CONNECT_ERR; // force to try next from repository table
                    cnt++;
                }
            }
            else
            {
                strcpy(server.machine, repository[cnt].machine);
                strcpy(server.address, repository[cnt].address);
                server.socket = htons(repository[cnt].socket);                
                strcpy(server.site, repository[cnt].name);               
                cnt++;               
            }
            for(already_attempted_counter = 0; already_attempted_counter < attempted_counter; ++already_attempted_counter)
            {
                if (strcasecmp(attempted_servers[already_attempted_counter].machine, server.machine) == 0 &&
                               attempted_servers[already_attempted_counter].socket == server.socket)
                {
                    ret = CONNECT_ERR; // force to try next from repository table
                    cnt++;
                    continue; // try next
                }
            }
            if (attempted_counter < MAX_ATTEMPTED_SERVERS)
            {
                attempted_servers[attempted_counter++] = server;
                DBG("[reconnecting] trying to connect into %s:%d", server.machine, ntohs(server.socket));
                rtn.len = 0;
                ret  = ssl_init(buffer);
                if (first_connect_error == WRONG_SERVER && ret == WRONG_SERVER)
                {
                    rtn.len = sprintf(rtn.msg, "Error %d: Keyword not found on %s %s %s %d\n",
                             TRANS_NON_ERR, server.site, server.machine, server.address, ntohs(server.socket));
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    return(ret);
} /* End intermediate_ssl_init */


/* Perform auto registration */
short user_autoreg(void)
{
    DBG();

    extern TRANS_TOOL tool;
    short ret;
    char buffer[64];
     char string1[64];
    char string2[64];

#ifdef garbage
    if (trans[index].display_transfer_status == DBNO)
    {
    strcpy(rtn.msg, trans_reg_err);
     return(TRANS_REG_ERR);
    }
#endif
    PRINTF("        You are not registered to use %s.\n", tool.name);
    PRINTF("              Performing auto registration.\n\n");
#ifdef NXP
    PRINTF("Please enter your NXP User ID (corporate ID).\n> ");
    fflush(stdout);
    scanf("%s", string1);
    PRINTF("Please enter your Active Directory Password (web password).\n> ");
    echo_stars(string2, DBYES);
     if (strlen(string2) == 0) strcpy3(buffer, string1, " ", NONE);
    else strcpy3(buffer, string1, " ", string2);
#else
     PRINTF("Please enter your company email address.\n>  ");
    fflush(stdout);
    scanf("%s", string1);
    PRINTF("Please enter your External LDAP NXP password .\n> ");
    echo_stars(string2, DBYES);
    if (strlen(string2) <= 1) strcpy3(buffer, string1, " ", string1);
    else strcpy3(buffer, string1, " ", string2);
#endif /* Other outside companies */
    put_ssl_string(buffer);
    PRINTF("\n\nYour registration request is being processed. Please wait.\n\n");
    /* Make sure the user could be registered */
    ret = get_ssl_short();
    if (ret >= SUCCESS)
    {
        printf("You have been registered to use %s.\n\n", tool.name);
        /* Carlos Mazieri, Oct 2019
         *   Fix this bug for parallel transfers
         */
        strcpy(user.login, string1);
    }
    /* If authentication done with names, warn the user */
    else if (ret == PWD_AUTHEN_WRN)
    {
        PRINTF(pwd_authen_wrn);
        ret = get_ssl_short();
    } /* End if for authentication warning */
    /* Otherwise get return code */
    else rtn.len += get_ssl_string(rtn.msg+rtn.len);
    return(ret);
} /* End user_autoreg */


/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
