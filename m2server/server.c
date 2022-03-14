/****************************************************************************
 *
 * Name:	main
 *
 * Description:	Main routine to run the Mask Designer database server
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "dir.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "socket_includes.h"
#include "m2defines.h"
#include "strcpy.h"
#include "rtn.h"
#include "global_debug.h"


void wrapper_request(int connected_socket, int argc, char *argv[])
{
    bld_log_name();
    /* Verifying which server being used */
    if (argc == 1)
    {
        sprintf(bf.buffer, "argv is %s", argv[0]);
    }
    else
    {
         sprintf(bf.buffer, "argv is %s %s", argv[0], argv[1]);
    }
    data_log(tr.log_file, bf.buffer);
    DBG("tr.log_file=[%s] bf.buffer[%s]", tr.log_file, bf.buffer);
    process_request(connected_socket);
}


/* Used by DB server and the Mask Shop server */
int main(argc, argv)
    int argc;
    char **argv;
{
    int connected_socket;
    int db_socket;
    int listening_socket;
    struct sockaddr new_client;
    int  new_client_length = sizeof(struct sockaddr);
    /* Indicate no email sent for the current transaction */
    extern int email_cnt ;
    extern int log_cnt ;

/*
    common_maskset_layers_update("rnc5");
    exit(0);
*/

    ldapfsl_setProductionLdapServer();

    email_cnt = 0;
    /* Indicate no messages logged for the current transaction */
    log_cnt = 0;
    /* If running the data base server, bring up one of three servers */
    /* The possible servers are the production, test, & development server */
    /* Define socket number based on command arguments */
    /* If no arguments passed in, run production version of server */
    if (argc == 1)
    {
        db_socket = 6195;
        printf("Welcome to version %s of the production Mask Designer Data Base Server.\n", MASK_VERSION);
    }
    else
{
    switch (argv[1][1])
    {
        case '1':
        {
            printf("Welcome to version %s of the production Mask Designer Data Base Server.\n", MASK_VERSION);
            db_socket = 6191;
        }
            break;
        case 'p':
        {
            printf("Welcome to version %s of the production Mask Designer Data Base Server.\n", MASK_VERSION);
            db_socket = 6195;
        }
            break;

        case 't':
        {
            printf("Welcome to version %s of the test Mask Designer Data Base Server.\n", MASK_VERSION);
            db_socket = 6196;
        }
            break;

        case 'd':
        {
            printf("Welcome to version %s of the development Mask Designer Data Base Server.\n", MASK_VERSION);
            db_socket = 6197;
        }
            break;


        default:
        {
            printf(bad_arg, argv[1]);
            exit(0);
        } /* End default */
    } /* end switch statement */
    } /* End else statement */

    print_build_information(argc, argv);

#if !defined(DEBUG)
   { /* empty block */
       extern void  email_send(char*, char*, char*, char*);
       char msg [256];
       strcpy (so.program, "MDS Server");
       sprintf(msg, "pid %lu using socket %d",getpid(), db_socket);
       email_send("", "b35201",
                   "MDS server started", msg);
    }
#endif
    /* If passed in a directory name, change to that directory */
    if (argc >= 3)
    chdir(argv[2]);
    /* Print out socket running */
    printf("Socket is %d\n", db_socket);
    /* Build the log file name */
    bld_log_name();
    /* Make sure the log directory and other directories are present */
    dir_make("log");
    dir_make("audit_log");
    dir_make("rmt_audit_log");
    dir_make(CONTRACT_DIR);
    dir_make(MASKSET_LOG);
    dir_make(PC_LOG);
    dir_make(H_LOG);
    dir_make("contract_tables/dupont_log");
    dir_make("contract_tables/photronics_log");
    dir_make("contract_tables/rtc_log");
    dir_make("db_admin");
    dir_make("cap_wafer");
    dir_make("fab_data");
    dir_make("fab_dlta");
    dir_make("global_data");
    dir_make("global_dlta");
    dir_make("mask_data");
    dir_make("mask_dlta");
    dir_make("off_fab_data");
    dir_make("off_global_data");
    dir_make("pc_data");
    dir_make("registry");
    dir_make("resend_data");
    dir_make("serial_tables");
    dir_make("serial_tables/mos6log");
    dir_make("serial_tables/mos16log");
    dir_make("serial_tables/mos17log");
    dir_make("serial_tables/mos21log");
    dir_make("tmp_files");
    dir_make("user_data");
    dir_make("user_dlta");
    dir_make("event_log");
    dir_make("fab_tech.files");
    dir_make("fab_tech.files/tmp_files");
    read_global_params();
    /* Get information about remote Mask Designer servers */
    rmt_server_read();

    /* Start up the daemon */
#if !defined(DEBUG)
    startup_daemon();
#endif

    /* Connect to the local socket */
    listening_socket = init_socket(db_socket);
    /* Get the updates from the remote servers */
/*
    get_rmt2updates();
*/
    printf("Just got remote updates\n");
    /* Initialize program name and version */
    strcpy(so.program, "Mask Designer Server");
    /* Accept connections. When we accept one, connected_socket will be
       connected to the client and new_client will contain the address of the
       client. */
    while (1)
    {
       connected_socket = accept(listening_socket, &new_client, &new_client_length);
       /* Ignore the interrupt sent by the dying child */
       if (connected_socket < 0)
       {
           /* This is the interrupt the child sends */
           if (errno == EINTR)	/* interrupted by system, OK to continue */
               continue;
           else
           {
               printf("Unable to accept connections\n");
               exit(0);
           } /* End else */
       } /* End if */

#if DEBUG
        wrapper_request(connected_socket, argc, argv);
        close(connected_socket);
#else
    /* Fork and continue listening; let the child finish the work */
       switch(fork())
       {
          case 0:
          {
            /* This is the  child which will do the work */
            /* Close socket the parent will continue to listen on */
            if (close(listening_socket) != 0)
            {
                data_log(tr.log_file, "error closing listening_socket");
                exit(1);
            } /* end if */

            wrapper_request(connected_socket, argc, argv);

            /* Should never return, but exit just in case */
            exit(2);
          }
          break;
            /* failed fork of first process */
          case -1: break;
          default:
         {
            /* Close socket child is connected to */
            close(connected_socket);
         } /* end default */
       } /*  end switch statement */
#endif
    } /* end while statement */
   return 0;
} /* end main */
