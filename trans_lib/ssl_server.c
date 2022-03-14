
#include <openssl/ssl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <mysql.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_errs.h"
#include "trans_defines.h"
#include "trans_lib_prototypes.h"
#include "trans_server_prototypes.h"
#include "global_debug.h"

/* The password is not being used for our certificate */
#define PASSWORD "password"

SSL *ssl;

extern int  gl_socket; // global variable to log in request_log() request.c


void database_initializations()
{
    extern MYSQL dt_sql;
    MYSQL * sql  = & dt_sql;  // later a connection will happen on sql_result_get()
    if (sql)
    {
        mysql_init(sql);
        populate_destinations_on_server();
        mysql_close(sql);
        mysql_init(sql); // clean for other connections
    }
}

#define TIME_TO_RELOAD_DATABASE  900   // 15 minutes


int is_it_time_to_reload_database()
{
    static time_t last_time_checked = 0;
    int ret = 0;
    time_t now = time(NULL);
    if ((now - last_time_checked) >= TIME_TO_RELOAD_DATABASE)
    {
        ret = 1;
        last_time_checked = now;
    }
    return ret;
}


int main(int argc, char **argv)
{
    extern char key_file[];
    int db_socket;
    int sock,s;
    BIO *sbio;
    SSL_CTX *ctx;
    int r;
    pid_t pid;
    char file_name[48];

    DBG("SERVER MAIN FUNCTION");

    /* Initialize parameters common to all of the servers */
    server_options_init(argc, argv, &db_socket, file_name);

    if (db_socket == 443)
        strcpy(key_file, "db_admin/crts/combined.pem");
    else
        strcpy(key_file, "db_admin/crts/trans.ssl.pem");

    DBG("db_socket=%d key_file=%s file_name=%s", db_socket, key_file, file_name);

    gl_socket = db_socket;

    /* Build our SSL context*/
    PRINTF("key_file is %s\n", key_file);
    ctx = initialize_ctx(key_file,PASSWORD);

    print_build_information(argc, argv);
    sock = tcp_listen(db_socket);

    while(1)
    {
        if ((s = accept(sock,0,0)) < 0)
        {
	        close_down1d(accept_err, s, ACCEPT_ERR);
        }
        if (is_it_time_to_reload_database())
        {
            database_initializations();
        }

#if !defined(DEVEL_USING_SINGLE_PROCESS)
       if((pid=fork()))
       {
          DBG("created process pid=%d", pid);
          close(s);
       }
       else // child process
#else
       if (1)
#endif
      {
            sbio = BIO_new_socket(s,BIO_NOCLOSE);
            ssl = SSL_new(ctx);
            SSL_set_bio(ssl,sbio,sbio);
            /* Perform server side of the SSL hand shake */
            if ((r = SSL_accept(ssl)) <= 0)
            {
                DBG("SSL ERROR %d" , SSL_get_error(ssl ,r));
                close_down1d(ssl_accept_err, r, SSL_ACCEPT_ERR);
            }
		    /* Read in the global params again */
		    read_global_params(file_name);
		    process_request(sock);
#if !defined(DEVEL_USING_SINGLE_PROCESS)
            break; // break whiile loop  normal production
#else
            close(s);
            sock = tcp_listen(db_socket);
#endif
	    } /* End else for child that performs the work */
    } /* End while */
    destroy_ctx(ctx);
    exit(0);
} /* End difmain */

