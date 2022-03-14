/****************************************************************************
 *
 * Function:	main
 *
 * Description:	Main routine used to start the non SSL server.
 *
 ***************************************************************************/

#include "errno.h"
#include <signal.h>
#include <sys/shm.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include "company.h"
#include "dir.h"
#include "global_defines.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "rtn.h"
#include "trans_defines.h"
#include "trans_lib_prototypes.h"
#include "global_server_errs.h"
#include "server_lib.h"
#include "trans_server_errs.h"
#include "trans_server_paths.h"
#include "trans_server_prototypes.h"
#include "user_params.h"
#include "global_debug.h"


/* Prototypes for initializing socket */
void db_socket_init(char option, int *db_socket);

extern char program_name[];


void server_options_init(int argc, char **argv, int *db_socket, char *file_name)
{
    int option;

    DBG();

    server_params_init();

    if (argc == 1)
        option = '3';
    else 
        option = argv[1][1];

    /* Bring up the SSL, test, or production server */
    /* Bring up server based on option passed in */
    db_socket_init(option, db_socket);

    strcpy(file_name, "db_admin/global_params");

    read_global_params(file_name);
    
    /* If passed in a directory name, change to that directory */
    if (argc >= 3)
        chdir(argv[2]);

    /* Indicate no lock files present */
    lock_files_init();
} /* End server_options_init */

/* Make directories needed for the server */
void server_dir_make(void)
{
    char dir_name[32];
    int cnt;

    DBG();

    dir_make("db_admin");
    dir_make("db_admin/tmp_files");
    system("touch db_admin/ssl.rmt_servers");
     dir_make("email_log");
    dir_make("fss_log");
    dir_make("fss_errs");
    dir_make("global_data");
    dir_make("history_data");
    /* Make sure the log directory is present */
     dir_make("log");
    dir_make("registry");
    dir_make("edc_registry");
    /* Check for and make report_log directory */
    dir_make("report_log");
    /* Check for and make transfer_log directory */
    dir_make("transfer_log");
    dir_make("tmp_files");
    dir_make("transweb_registry");
    dir_make("transweb_registry/tmp_files");
    dir_make("user_keys");
#ifndef TRANSCEND
    dir_make("user_approvals");
    dir_make("key_approvals");
#endif
    for (cnt = 0; cnt < 10; ++cnt)
    {
	sprintf(dir_name, "data_files%d", cnt);
    dir_make(dir_name);
    }
} /* End server_dir_make */


/* Initialize the socket value */
void db_socket_init(char option, int *db_socket)
{

    DBG();

#ifdef INTERCOM
    strcpy(so.program, "InterCom server");
    strcpy(program_name, "InterCom");
#endif
#ifdef TRANSWEB
#ifdef NXP
    strcpy(so.program, "Internal-TransWeb server");
#endif
#ifdef EXTERNAL
    strcpy(so.program, "TransWeb server");
#endif
    strcpy(program_name, "TransWeb");
#endif
#ifdef TRANSCEND
    strcpy(so.program, "Transcend server");
    strcpy(program_name, "Transcend");
#endif
#ifdef TRANS_FTP
    strcpy(so.program, "TransFTP server");
    strcpy(program_name, "TransFTP");
#endif
    switch(option)
	{
	    case 's':
	    {
		*db_socket = 443;
	    }
	    break;
	default:
	    {
		*db_socket = 6920 + option-48;
	    } /* End default */
	} /* end switch statement */
    PRINTF("Welcome to version %s of the %s.\n", TRANS_VERSION, so.program);
    PRINTF("Socket is %d\n", *db_socket);
} /* End db_socket_init */

#ifdef TRANSCEND
/* this code releases shared memory */
void handle_signal( int  signum )
     {
    DBG();
    extern int shm_id;
    if (shm_id > 0)
     shmctl(shm_id, IPC_RMID, 0);
     exit (1);
     }
#endif

/* Initialize params common to the servers */
void server_params_init(void)
{
    DBG();

#ifdef TRANSCEND
    /* Exit gracefully if signals received */
   signal( SIGHUP, handle_signal);
   signal( SIGQUIT, handle_signal);
#endif
    /* Indicate no email sent for the current transaction */
    email_cnt = 0;
    /* Indicate no messages logged for the current transaction */
    log_cnt = 0;
    /* Initialize the command option */

    //in devel test mode work with single process
#if !defined(DEVEL_USING_SINGLE_PROCESS)
    /* Start up the daemon */
    startup_daemon();
#endif

    /* Build the log name */
    bld_log_name();
    /* Make needed server directories */
    server_dir_make();
    /* Get the remote server address */
#ifndef TRANS_FTP
    /* Read in the remote servers */
    rmt_server_read();
#endif
    /* Initialize the version number */
    strcpy(so.version, TRANS_VERSION);
} /* end server_params_init */

#ifndef SSL_ENCRYPTION
int main(int argc, char **argv)
{
    char file_name[48];
    int db_socket;
    int connected_socket;
    int listening_socket;
    struct sockaddr new_client;
    int  new_client_length = sizeof(struct sockaddr);
    extern int email_cnt;
    extern int log_cnt;

    DBG("MAIN FUNCTION");

    /* Initialize params common to the servers */
    server_options_init(argc, argv, &db_socket, file_name);
    /* Connect to the local socket */
    listening_socket = init_socket(db_socket);
    PRINTF("Finished bind ready to listen\n");
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
	    else data_log(tr.log_file, "Server error");
	}
	/* Read global params */
	read_global_params(file_name);
	    rmt_server_read();
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
		process_request(connected_socket);
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
	} /* end switch statement */
    } /* end while statement */
} /* end main */
#endif


void replace_blank_character(char *site)
{
    char *pt_blank = site;
    // site needs to be just one word
    while ((pt_blank = strchr(pt_blank, ' ')) !=  NULL)
    {
        *pt_blank =  '_';
    }
}

void server_set(void)
{
    extern char global_default_site;
    extern USER_PARAMS user;
    char *pt_domain;
    TRANS_COMPANY  site = {0};

    DBG();
    // verify is user.mach contains a valid hostname with dmain part
    if (user.mach[0] && inet_network(user.mach) == -1 && (pt_domain = strchr(user.mach, '.'))!= NULL)
    {
        // look on the table dt_server_by_domain and get default abbreviation
       char *abbreviation = sql_search_domain(pt_domain+1);
       if (abbreviation != 0 && abbreviation[0] != 0)
       {
           // search be extension
           TRANS_COMPANY * company = get_company_info(abbreviation);
           if (company!= NULL)
           {
               site = *company;
           }
       }
    }
    if (site.name[0] == 0)
    {
        int counter = 0;
        for(;counter < gl_site_destinations_counter; ++counter)
        {
            if (gl_site_destinations[counter].extension[0] == global_default_site)
            {
                site = gl_site_destinations[counter];
                break;
            }
        }
    }

    /* Send return code */
    put_short(SUCCESS);

    replace_blank_character(site.name);
    rtn.len = sprintf(rtn.msg, "%s %s %s %s", site.name, site.machine, site.address, site.socket);

    put_success_msg(NONE);

    DBG("rtn.msg='%s'", rtn.msg);
} /* End server_set */
