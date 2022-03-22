#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	client
 *
 * Description:	Main routine for Transcend and InterCom
 *
 ***************************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifndef SOLARIS10
     #include <signal.h>
#endif

#include <sys/shm.h>
#include "company.h"
#include "global_client_errs.h"
#include "global_defines.h"
#include "global_params.h"
#include "rtn.h"
#include "trans_client_errs.h"
#include "transapi_msg.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_hidden_prototypes.h"
#include "transapi_prototypes.h"
#include "transrc.h"
#include "server_params.h"
#include "strcpy.h"

#include "global_debug.h" // Mazieri's debug facilities

int gl_is_web;  // special flag for transweb

void handle_signal(int signum)
{
    DBG("signum='%d'", signum);

    extern int shm_id;

    if (shm_id > 0)
        shmctl(shm_id, IPC_RMID, 0);
 
    exit (1);
}


/* Set e-mail addresses for secure keywords being sent */
int trans_email_scan(int index, int arg_cnt, int argc, char **argv)
{
    DBG("index=%d, arg_cnt=%d, argc=%d, argv=%s", index, arg_cnt, argc, argv);

	if (argc > arg_cnt)
    {
	/* If this is the GUI, copy in the description */
#ifdef TRANSCEND_GUI
	    if ((trans[index]->description = malloc(strlen(argv[arg_cnt])+1)) == NULL)
	    {
	        sprintf(rtn.msg, malloc_err);
	        return(MALLOC_ERR);
	    } /* end if for cannot allocate */

	    strcpy(trans[index]->description, argv[arg_cnt]);
	    ++arg_cnt;
#endif
	
	    commandline_email_set(index, arg_cnt, argc, argv);

#ifndef TRANSCEND_GUI
		/* Set description for secure keywords being sent */
		if (trans[index]->email_list != NULL)
		    trans_description_scan(index);
#endif
    } /* end if */

    return(SUCCESS);
} /* End trans_email_scan */


/* If this is internal Intercom, prompt user for company abbreviation */
int internal_icom_company_set(int index)
{
    DBG("index='%d'", index);

    char extension[8] = "";
    int offset;

	PRINTF("Please enter abbreviation for destination company.\n\nDefined companies are listed below:\n\n");
	transapi_site_print(index);
    PRINTF("%s", rtn.msg);
	PRINTF("> ");
	arg_scan(extension);

    if (extension[0] == '-')
    {
        offset = 1;
    } else {
        offset = 0;
    }
	PRINTF("\n");
	/* If extension not found, give error */
	if (trans_site_find(index, extension+offset) != SUCCESS)
	{
	    rtn.len = strcpy1(rtn.msg, trans_company_err);
	    return(TRANS_SITE_ERR);
    } /* end if */
    else return(SUCCESS);
} /* end internal_icom_company_set */


/* Set alternate repository and icap classification */
int repository_icap_class_set(int index, int argc, char **argv, int min_cnt)
{
    extern TRANS_TOOL tool;
    char extension[8];
    int increment = 0;
    int offset;
    int ret;

    DBG( "index=%d, argc=%d, min_cnt=%d", index, argc, min_cnt );

    /* check for minimum number of arguments */
    if (argc == min_cnt)
    {
        DBG( "minimum number of arguments specified, simply return" );

	    /* If using Transcend, set to closest server */
	    if ((tool.type == TRANSCEND_INTERCOM) || (tool.type == INTERNAL_TRANSCEND))
	    {
	        DBG("Using Transcend, setting to closest server");
	        trans_closest_server_set();
	        return(0);
	    } 
	    else if ((tool.type == INTERNAL_INTERCOM) && (strchr(TRANSAPI_PUT_OPTIONS, argv[1][1]) != NULL)) 
	    {
	        /* If using internal Intercom and company abbreviation required */
	        if ((ret = internal_icom_company_set(index)) < 0)
	            return(ret);
	        else 
	            return(0);
        } /* end if */

	    else return(0);
    } /* end if */

    /* If this is External Intercom, only set the ICAP class */
    if (tool.type == EXTERNAL_INTERCOM)
    {
        DBG( "this is External Intercom, only set the ICAP class"  );

	    if (argc == 3) {
	        return(0);
	    } else {
	        return(icap_class_set(index, argc, argv, 0));
        }
    }

    /* If argument too long to be extension, return */
    else if (strlen(argv[2]) > 5) {
        return(0);
    }
    
    if (argv[2][0] == '-') {
        offset = 1;
    } else {
        offset = 0;
    }

	/* See if customer or repository specified */
	/* If found, point past the repository */
	if (trans_site_find(index, argv[2]+offset) == SUCCESS)
    {
        increment = 1;
	    /* Save extension for later use */
	    strcpy(extension, argv[2] + offset);

    } else if ((increment == 0) && ((tool.type == INTERNAL_TRANSCEND) || (tool.type == TRANSCEND_INTERCOM))) {
        /* Set closest server for Transcend */
        trans_closest_server_set();

    } else if ((increment == 0) && (tool.type == INTERNAL_INTERCOM)) {
        if ((ret = internal_icom_company_set(index)) < 0)
            return(ret);
    } /* end if for internal intercom */


    return(icap_class_set(index, argc, argv, increment));
} /* end repository_icap_class_set */


int icap_class_set(int index, int argc, char **argv, int increment)
{
    DBG("index='%d', argc='%d', argv='%s', increment='%d'", index, argc, argv, increment);

	/* Check for icap class specification */
    /* If no additional arguments, return */
    if (argv[2+increment] == NULL) {
        return(increment);
    }

	if (argv[2+increment][0] == '-')
    {
	    strcpy(trans[index]->icap_class, argv[2+increment]+1);
	    ++increment;
    }

    DBG( "Will return the following increment=%d", increment  );

    return(increment);
} /* end icap_class_set */

/* authenticate external customer */
int customer_authenticate(int index, int *password_scanned)
{
    extern char reg_email1[];
    extern char reg_email2[];
    extern TRANS_TOOL tool;
    int scanned = DBNO;
    DBG("index='%d' and password_scanned='%d'", index, password_scanned);
    DBG("reg_email1=%s reg_email2=%s", reg_email1, reg_email2);
    if (tool.type != EXTERNAL_INTERCOM && reg_email1[0] == '\0')
    {
        strcpy (reg_email1, user.login);
    }

    if (reg_email1[0] == '\0')
    {
	    PRINTF("Performing authentication with your e-mail address and External LDAP\n");
	    PRINTF("NXP password. (This is not the destination e-mail address)\n\n");
	    PRINTF("Please enter your e-mail address.\n");

	    fflush(stdout);

	    scanf("%s", reg_email1);
	    scanned = DBYES;
    } /* end if for email not set */


    PRINTF("Using the NXP account '%s'\n", reg_email1);

    if (reg_email2[0] == '\0')
    {
	    PRINTF("Please enter your External LDAP NXP password.\n");
	    fflush(stdout);
	
	    echo_stars(reg_email2, scanned);
	    *password_scanned = DBYES;
	    scanned = DBYES;
    } /* End if for password not initialized */

    /* Make needed changes to the password */
    if (strlen(reg_email2) == 0) strcpy(reg_email2, NONE);

    /* Copy reg_email1 into login and e-mail */
    strcpy(user.login, reg_email1);
    strcpy(trans[index]->email1, reg_email1);
    
    if (scanned == DBYES) PRINTF("\n\n");

    trans_simple_basic_request_set(index, DBX500AUTHEN, reg_email2);
    
    return( trans_perform(index));
} /* end customer_authenticate */


/* Encrypt password for storage in the .transrc file */
int trans_password_encrypt(int index)
{
    DBG( "index='%d'", index );

    /* Following variables hold the user e-mail address and password */
    extern char reg_email1[];
    extern char reg_email2[];
    int ret;
    extern char reg_email2[];
    char int_pwd[128];

    /* Perform ASCII encryption of password */
    encrypt_ascii_pwd(int_pwd, reg_email2);

    /* Set request type */
    trans_simple_basic_request_set(index, DBENCRYPT_PASSWORD_LOG, int_pwd); 

    /* Copy reg_email1 into login and e-mail */
    strcpy(user.login, reg_email1);
    strcpy(trans[index]->email1, reg_email1);

    /* Send request to server */
    /* If successful, copy encrypted password */
    if ((ret = trans_perform(index)) == SUCCESS)
    {
	    if (core_id_crypt(reg_email2, 1)  != SUCCESS)
	    {
	        ret = ENCRYPT_PWD_ERR;
	        strcpy(rtn.msg, encrypt_pwd_err);
	    } else strcpy(rtn.msg, reg_email2);

	} /* end if */

    return(ret);
} /* end trans_password_encrypt */


int main(int argc, char *argv[])
{
    extern char icomrc_path[];
    extern TRANS_TOOL tool;
    int index;
    int password_scanned = DBNO;
    extern int compression;
    extern int shm_id;
    extern char tar_option;
    int silent = DBNO;
    int ret;
    char req_type;
    char *root_ptr = argv[0];
    int cnt = 0;
    int len;
    char *ptr;
    extern char reg_email1[];
    char **list;

    DBG("MAIN FUNCTION, argc='%d'", argc);

#ifdef DEBUG
    int i;
    for (i = 1; i < argc; i++) {
        DBG("MAIN FUNCTION, parameter argv[%d] = %s", i, argv[i]);
    }
#endif

#ifdef WINDOWS_NT4
	/* Disable warning about dos path */
	system("set CYGWIN=nodosfilewarning");
#endif

    compression = rtn.len = shm_id = 0;
    strcpy(remote_company_email, NONE);

#ifndef SOLARIS10
    /* Exit gracefully if signals received */
    /* catch Signals */
   signal( SIGHUP, handle_signal);
   signal( SIGQUIT, handle_signal);
#endif

    /* Initialize user options */
    /* this includes login name, machine, IP node address, etc. */
    /* It also includes pointers to the global trans structure */
    trans_user_params_init(NULL);

    /* Get an index to a transaction object */
    /* Parameters passed in include */
    /* Overwrite existing files */
    /* Perform background transfer */
    /* Suppress prompting and displaying output */
    /* suppress transfer status */
    index = trans_init(DBNO, DBNO,  DBNO, DBNO);

    /* The following code is used when Transcend is called as an executable, it is not used by the API */
    /* If this is for the Transcend GUI, set the login name using argv[1], this overwrites the value set in trans_user_params_init */
#ifdef TRANSCEND_GUI
    if (argc > 1)
    {
        /* Initialize the encrypted user name */
        strcpy(user.login, argv[1]);
        argv += 1;
        argc -= 1;
    } /* End if for more than 1 argument */
    silent = DBYES;
#endif /* for TRANSCEND_GUI */

    strcpy(icomrc_path, user.home_dir);
    DBG("user.home_dir=%s icomrc_path=%s", user.home_dir, icomrc_path);

    /* check for some special options:
     *          -icomrc <path>
     *          -web [user]
     */
    if (argc > 1)
    {
        int is_icomrc = 0;
        int is_web    = 0;
        while ( argc > 1 &&  ((is_icomrc = (strcasecmp(argv[1], "-icomrc") == 0))
                               || (is_web = (strcasecmp(argv[1], "-web") == 0))))
        {
            if (is_icomrc)
            {
                strcpy(icomrc_path, argv[2]);
                DBG("icomrc_path=%s argv[2]=%s", icomrc_path, argv[2]);
                argc -= 2;
                argv += 2;
            }
            else // -web for transweb
            {
                gl_is_web = 1;
                silent = DBYES;
                get_web_client_information();
                argc--;
                argv++;
                if (argv[1][0] != '-')  // if web and not an option it must be username
                {
                    strcpy(user.login, argv[1]);
                    argc--;
                    argv++;
                }
            }
        }
        // special handling for Amsterdam repository which was copied from Eindhoven
        if (argc > 2 &&  strcmp(argv[2], "-eind") == 0 )
        {
            argv[2] = "-adam";
        }
    }

    /* Set tool type, default repository and read configuration file
     *
     * trans_tool_set() will read .transcrc/.icomrc
     */
#ifdef NXP 
#ifdef TRANSCEND
    trans_tool_set(index, TRANSCEND_INTERCOM);
    DBG("trans_tool_set( index, TRANSCEND_INTERCOM=%d )", TRANSCEND_INTERCOM);
#endif
#ifdef INTERCOM
    trans_tool_set(index, INTERNAL_INTERCOM);
    DBG("trans_tool_set( index, INTERNAL_INTERCOM=%d )", INTERNAL_INTERCOM);
#endif
#ifdef TRANSWEB
    trans_tool_set(index, INTERNAL_TRANSWEB);
    DBG("trans_tool_set( index, INTERNAL_TRANSWEB=%d )", INTERNAL_TRANSWEB);
#endif
#endif /* for NXP */

#ifdef EXTERNAL
#ifdef INTERCOM
    trans_tool_set(index, EXTERNAL_INTERCOM);
    DBG("trans_tool_set( index, EXTERNAL_INTERCOM=%d )", EXTERNAL_INTERCOM);
#endif
#ifdef TRANSWEB
    trans_tool_set(index, EX_TRANSWEB);
    DBG("trans_tool_set( index, EX_TRANSWEB=%d )", EX_TRANSWEB);
#endif
#endif /* for EXTERNAL */



#ifdef SEND2FOUNDRY
    silent = DBYES;
    PRINTF("Welcome to version %s of Foundry Send\n", TRANSAPI_VERSION);
#endif




    DBG( "argc=%d, tool.name=%s, TRANSAPI_VERSION=%s", argc, tool.name, TRANSAPI_VERSION );

    /* Check for silent ping requests */
    if (argc > 1 && argv[1][1] == 'W')
    {
        silent = DBYES;
    }

    /* Check for the -A option */
    if ((argc > 2) && ((argv[1][1] == 'A') || (argv[1][1] == '7')))
        silent = DBYES;

    /* Print the program banner */
    if (silent == DBNO)
    {
	    PRINTF(TRANSAPI_BANNER, TRANSAPI_VERSION, tool.name, tool.name);
	    fflush(stdout);
    } else if (gl_is_web == 0)
    {
        PRINTF("\n");
    }
    
    /* If no arguments given, print intro screen */
    /* Print help for specified commands */
    /* Help is printed for all commands that require one */
    /* or more arguments */
    if ((argc == 1) || ((argc == 2)  && (strchr(TWO_PARAM_OPTIONS, argv[1][1]) == NULL)))
    {
	    /* Look for the leaf of the executable file name */
	    ptr = root_find(root_ptr, '/');
	    /* Print help on specified command */
	    if (argc > 1) 
	    {
	        trans_help_print(ptr, argv[1][1], DBYES);
	        goto label;
	    } /* end inner if */
	
        print_build_information(argc, argv);
        printf ("\n");

	    /* If option not found or no command given */
	    /* show user intro screen */
	    PRINTF(TRANS_FORMAT_MSG, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr);
	    exit(0);
    }

    /* If there is not a '-' on option, give error */
    /* Also if not enough arguments for function being performed, give error */
    if ((argv[1][0] != '-') || ((argc <= 3) && (strchr(THREEPARAM_OPTIONS, argv[1][1]) != NULL)))
    {
	    /* Look for the leaf of the executable file name */
	    ptr = root_find(root_ptr, '/');
	    PRINTF(TRANS_FORMAT_ERR);
	    PRINTF(TRANS_FORMAT_MSG, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr);
	    exit(101);
    }

    /* Initialize remote directory used to send data to TSMC */
    strcpy(trans[index]->remote_dir, NONE);
    strcpy(trans[index]->mask, NONE);
    req_type = argv[1][1];

    if (strlen(argv[1]) > 1)
        tar_option = argv[1][2];
    else 
        tar_option = '\0';

    /*  May 2018, Carlos Mazieri,  .transrc did not work */
    if (icomrc_path[0] != '\0' && reg_email1[0] != 0)
    {
        strcpy(user.login, reg_email1);
    }

    /*
     * Oct 2019, Carlos Mazieri,
     *
     * Added -rc option to create '.icomrc' (or '.transrc' for transcend)
     *
     *      Only 'icom -a'  for external icom shows that option,
     *         but that option is available for all clients
     */
    if (argc > 1 && strcmp(argv[1], "-rc") == 0)
    {
        extern char reg_email1[];
        extern char reg_email2[];       
        reg_email2[0] = 0;
        if (argc > 2)
        {
            strcpy (reg_email1, argv[2]);
        }
        DBG("reg_email1=%s reg_email2=%s", reg_email1, reg_email2);
        if ((ret = customer_authenticate(index, &password_scanned)) < SUCCESS)
        {
            goto label;
        } /* end if */
        char pathname[PATH_SIZE];
        sprintf(pathname, "%s/%s", user.home_dir,
                tool.type == TRANSCEND_INTERCOM ? ".transrc" : ".icomrc");
        FILE *icomrc = NULL;
        if ((icomrc = fopen(pathname, "w")) != NULL)
        {
             if (    fprintf(icomrc, "User e-mail: %s\n", user.login) > 0
                  && fprintf(icomrc, "User password: %s\n", trans[index]->keyword) > 0)
             {
                  ret = 1;
             }
             fclose(icomrc);
        }
        printf ("generated %s\n", pathname);
        return 0;
    }


    /* If this is SSL Intercom, require authentication */
    if ((trans[index]->authen == DBYES) && (strchr("aAHIMoRvxX", req_type) == NULL) && (strcmp(user.machine_type+strlen(user.machine_type)-3, "GUI") != 0))
    {
        if ((ret = customer_authenticate(index, &password_scanned)) < SUCCESS)
        {
            goto label;
        } /* end if */
    } /* End if for not doing -v -a -x or -H option */


    /* Determine action to perform */
    switch (req_type)
    {
	/* Put a big file */
	case 'p':
	{
        DBG( "req_type=%c, will Put 'big' file=%s, argv[1][2]=%d", req_type, argv[2], argv[1][2] );

	    /* Set the request type */
	    trans_put_request_set(index, DBPUT_DATA, argv[1][2]);

	    /* See if iCAP class  or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;

	    /* Set the file name */
	    if ((ret = trans_file_name_set(index, argv[2])) < 0) break;

	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 3, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Put performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Put performed, the return value is %d, and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Perform one step upload */
	/* This can only be done if the client and server have access to the same machine */
	case '@':
	{
        DBG( "req_type=%c, will perform one step upload of the file=%s", req_type, argv[2] );

	    /* Set the request type */
	    trans_custom_request_set(index, DB1STEP_UPLOAD, NONE);

	    /* Set the file name */
	    strcpy(trans[index]->keyword, argv[2]);

	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 3, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "One step upload performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "One step upload performed, the return value is %d, and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Retrieve file name needed for 1step download */
	case '#':
	{
        DBG( "req_type=%c, will retrieve file name needed for 1step download of the file=%s", req_type, argv[2] );

	    if ((ret = trans_keyword_request_set(index, DB1STEP_DOWNLOAD, argv[2])) == SUCCESS)
	        ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "retrieved file name needed for 1step download with *error*, the return value is %d and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "retrieved file name needed for 1step download, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Send completion for download of data */
	case '$':
	{
        DBG( "req_type=%c, will Send completion for download of data of the file=%s", req_type, argv[2] );

	    if ((ret = trans_keyword_request_set(index, DB1STEP_DOWNLOAD_COMPLETE, argv[2])) == SUCCESS)
	        ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "After send completion for download of data, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "retrieved file name needed for 1step download, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
    /* Zip the file after 1step upload */
	case '0':
	{
	    if ((ret = trans_keyword_request_set(index, DB1STEP_ZIP, argv[2])) == SUCCESS)
	    ret = trans_perform(index);
	}
	break;
	/* Put a big file using afile descriptor */
	case '%':
	{
        DBG( "req_type=%c, Put a big file using afile descriptor of the file=%s", req_type, argv[2] );

	    FILE *fp;
	    int fd;
	    /* Set the request type */
	    trans_put_request_set(index, DBPUT_DATA, argv[1][2]);

	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }

	    /* Set the file name */
	    if ((ret = trans_file_name_set(index, argv[2])) < 0) break;

	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 4, argc, argv)) != SUCCESS) break;
	        fp = fopen(trans[index]->file_name, "r");

	    fd = open(trans[index]->file_name, O_RDONLY);
	    trans_file_descriptor_set(index, fd);
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "After Put a big file using afile descriptor, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "retrieved Put a big file using afile descriptor, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Put file and delete after first successful retrieval */
	case 'P':
	{
        DBG( "req_type=%c, Put file and delete after first successful retrieval for the file=%s", req_type, argv[2] );

	    /* Set the request type */
        trans_put_request_set(index, DBPUTDLT_DATA, argv[1][2]);

	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;

	    if ((ret = trans_file_name_set(index, argv[2])) < 0) break;

	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 3, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "After Put file and delete after first successful retrieval, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "After Put file and delete after first successful retrieval, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	case 't':
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    /* Set the request type */
	    trans_put_request_set(index, DBPUT_FILES, argv[1][2]);

	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;

	    /* Set to follow links for tar */
	    trans[index]->follow_links = DBYES;

	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_file_name_set(index, argv[2])) < 0) break;
	    if ((ret = trans_email_scan(index, 3, argc, argv)) != SUCCESS) break;

	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Transfer following link and delete after first successful retrieval */
	case 'T':
	{
        DBG( "req_type=%c is Transfer following link and delete after first successful retrieval, file=%s", req_type, argv[2] );

	    /* Set the request type */
	    trans_put_request_set(index, DBPUTDLT_FILES, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    /* Set to follow links for tar */
	    trans_option_set(index, FOLLOW_LINKS, DBYES);

	    if ((ret = trans_file_name_set(index, argv[2])) < 0) break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 3, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer following link and delete after first successful retrieval performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer following link and delete after first successful retrieval performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Put duplicate files for Mask Designer */
	case 'O': 
	{
        DBG( "req_type=%c is Put duplicate files for Mask Designer, file=%s", req_type, argv[2] );

	    /* Set the request type */
	    trans_put_request_set(index, DBPUT_DUPLICATE_FILES, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    if ((ret = trans_file_name_set(index, argv[2])) < 0) break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 3, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Put duplicate files for Mask Designer performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Put duplicate files for Mask Designer performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
    }
	break;
	/* Transfer group of files */
	case 'f': 
	{
        DBG( "req_type=%c is Transfer group of files, file=%s", req_type, argv[2] );

	    trans_put_request_set(index, DBPUT_FILES, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    ret = trans_files_set(index, argv+2);
	    if (ret < 0) break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 1000, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer group of files performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer group of files performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Transfer group of files and delete after first retrieval */
	case 'F': 
	{
        DBG( "req_type=%c is Transfer group of files and delete after first retrieval, file=%s", req_type, argv[2] );

	    trans_put_request_set(index, DBPUTDLT_FILES, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    if ((ret = trans_files_set(index, argv+2)) < 0)
	    break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 1000, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer group of files and delete after first retrieval performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer group of files and delete after first retrieval performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Sends group of files stored in specified file */
	case 'n': 
	{
        DBG( "req_type=%c is Sends group of files stored in specified file, file=%s", req_type, argv[2] );

	    trans_put_request_set(index, DBPUT_FILES, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    if ((ret = trans_named_files_set(index, argv[2])) < 0) break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 3, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Sends group of files stored in specified file performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Sends group of files stored in specified file performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Transfer group of files and delete after first retrieval */
	case 'N': 
	{
        DBG( "req_type=%c is Transfer group of files and delete after first retrieval, file=%s", req_type, argv[2] );

	    trans_put_request_set(index, DBPUTDLT_FILES, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    if ((ret = trans_named_files_set(index, argv[2])) < 0) break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 4, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer group of files and delete after first retrieval performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer group of files and delete after first retrieval performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Transfer group of files in specified directory */
	case 'k': 
	{
        DBG( "req_type=%c is Transfer group of files in specified directory, file=%s", req_type, argv[2] );

	    trans_put_request_set(index, DBPUT_DATA, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 4)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    /* Set the symbolic directory */
	    if ((ret = trans_symbolic_dir_set(index, argv[2])) < 0) break;
	    /* Initialize names of files in the directory */
	    if ((ret = trans_dir_files_link(index, argv+3)) < 0) break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 1000, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer group of files in specified directory performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer group of files in specified directory performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Transfer group of files and delete after first retrieval */
	    /* Files are put in specified directory */
	case 'K': 
	{
        DBG( "req_type=%c is Transfer group of files and delete after first retrieval Files are put in specified directory, file=%s", req_type, argv[2] );

	    trans_put_request_set(index, DBPUTDLT_FILES, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 4)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    /* Set the symbolic directory */
	    if ((ret = trans_symbolic_dir_set(index, argv[2])) < 0) break;
	    /* Initialize names of files in the directory */
	    if ((ret = trans_dir_files_link(index, argv+3)) < 0) break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 1000, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Sends group of files stored in specified file */
	    /* Files are bundled in specified directory */
	case 'z': 
	{
        DBG( "req_type=%c is Sends group of files stored in specified file and files are bundled in specified directory, file=%s", req_type, argv[2] );

	    trans_put_request_set(index, DBPUT_FILES, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 4)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    /* Set the symbolic directory */
	    if ((ret = trans_symbolic_dir_set(index, argv[2])) < 0) break;
	    /* Initialize names of files in the directory */
	    if ((ret = trans_dir_files_read(index, argv[3])) < 0) break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 4, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Transfer group of files and delete after first retrieval */
	    /* They are placed in specified directory */
	case 'Z': 
	{
        DBG( "req_type=%c is Transfer group of files and delete after first retrieval, They are placed in specified directory, file=%s", req_type, argv[2] );

	    trans_put_request_set(index, DBPUTDLT_FILES, argv[1][2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 4)) > 0)
	    { argc -= ret; argv += ret; }
	    /* If company required and not found, exit */
	    else if (ret < 0) break;
	    /* Set the symbolic directory */
	    if ((ret = trans_symbolic_dir_set(index, argv[2])) < 0) break;
	    /* Initialize names of files in the directory */
	    if ((ret = trans_dir_files_read(index, argv[3])) < 0) break;
	    /* Set e-mail addresses for secure keywords being sent */
	    if ((ret = trans_email_scan(index, 4, argc, argv)) != SUCCESS) break;
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Display original and stored size of data */
	case 'l': 
	{
        DBG( "req_type=%c is Display original and stored size of data, file=%s", req_type, argv[2] );

	    if ((ret = trans_keyword_request_set(index, DBSIZE_CHECK, argv[2])) == SUCCESS) 
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Lock data for retrieval */
	case 'L': 
	{
        DBG( "req_type=%c is Lock data for retrieval, file=%s", req_type, argv[2] );

	    if ((ret = trans_keyword_request_set(index, DBLOCK_DATA, argv[2])) == SUCCESS) 
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Unlock data for retrieval */
	case 'U': 
	{
        DBG( "req_type=%c is unlock data for retrieval, file=%s", req_type, argv[2] );

	    if ((ret = trans_keyword_request_set(index, DBUNLOCK_DATA, argv[2])) == SUCCESS) 
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Transfer performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Transfer performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	case 'h': 
	{
#ifdef NXP
#ifdef INTERCOM
	    if (strlen(argv[2]) == 4) ret = maskshop_history(argv[2]);
	    else 
#endif /* For INTERCOM */
#endif /* for NXP */
	    if ((ret = trans_custom_keyword_request_set(index, DBHIST_DATA, argv[2])) == SUCCESS)
	    ret = trans_perform(index);
	}
	break;
	/* Extend expiration date up to 3 working days */
	case 'e': 
    {
        DBG("req_type=%c is Extend expiration, file=%s", req_type, argv[2]);

	    /* Set number of days to extend expiration */
	    int days = trans_days_set(argc, argv);
	    DBG("days='%d'", days);
	    if ((days > 0) && (days < 4))
	    {
		    /* Set request type and keyword */
		    if ((ret = trans_keyword_request_set(index, DBEXTEND_DATA, argv[2])) < 0) break;

		    /* Set the number of days to extend file */
		    DBG("index='%d', num_days='%d', days='%d'", index, NUM_DAYS, days);
		    trans_option_set(index, NUM_DAYS, days);
		    ret = trans_perform(index);
	    } /* end if */

	    if (ret != SUCCESS) {
            DBG( "Extend expiration date up to 3 working days performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Extend expiration date up to 3 working days performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Send sender e-mail when his data is retrieved */
	case 's':
	{
        DBG( "req_type=%c is Send sender e-mail when his data is retrieved, file=%s", req_type, argv[2] );

	    /* See if iCAP class  or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    /* Determine if setting e-mail notification for one file or all files */
	    if ((strcmp(argv[2], "all") == 0) 
	    || (strcmp(argv[2], "ALL") == 0))
	    ret = trans_simple_basic_request_set(index, DBSET_USER_EMAIL, argv[2]);
		else ret = trans_keyword_request_set(index, DBSET_FILE_EMAIL, argv[2]);
	    if (ret == SUCCESS)
	ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Operation performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Operation performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Cancel reception of Email */
	case 'c':
	{
        DBG( "req_type=%c is to Cancel reception of Email, file=%s", req_type, argv[2] );

	    /* See if iCAP class  or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 3)) > 0)
	    { argc -= ret; argv += ret; }
	    if ((strcmp(argv[2], "all") == 0) 
	    || (strcmp(argv[2], "ALL") == 0))
	    ret = trans_simple_basic_request_set(index, DBCAN_USER_EMAIL, argv[2]);
		else ret = trans_keyword_request_set(index, DBCAN_FILE_EMAIL, argv[2]);
	    if (ret == SUCCESS)
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Operation performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Operation performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Delete file with specified keyword */
	case 'd': 
	{
        DBG( "req_type=%c is Delete file with specified keyword, file=%s", req_type, argv[2] );

	    if (strcasecmp(argv[2], "all") != 0)
	    {
		 if ((ret = trans_keyword_request_set(index, DBDLT_DATA, argv[2])) == SUCCESS) 
		ret = trans_perform(index);
	    } /* end if */
	    else trans_keywords_delete(index, USER_KEYS_DELETE, "All");

	    if (ret != SUCCESS) {
            DBG( "Operation performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Operation performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Allow only owner to delete data with specified keyword */
	case 'D':
	{
        DBG( "req_type=%c is Allow only owner to delete data with specified keyword, file=%s", req_type, argv[2] );

	    if ((ret = trans_keyword_request_set(index, DBOWNER_DLT, argv[2])) == SUCCESS) {
	        ret = trans_perform(index);
	    }

	    if (ret != SUCCESS) {
            DBG( "Operation performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Operation performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Allow everyone to delete data with specified keyword */
	/* this is called to reverse action set with the -D option */
	/* For internal InterCom, this is used to resend data to the foundry */
	/* Transfer data to foundry already in intermediate repository */
	case 'E':
	{
        DBG( "req_type=%c is Allow everyone to delete data with specified keyword, file=%s", req_type, argv[2] );

	    if ((ret = trans_keyword_request_set(index, DBUSER_DLT, argv[2])) == SUCCESS)
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Operation performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Operation performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Also resends encrypted data */
	case '6':  
	{
        DBG( "req_type=%c is Also resends encrypted data, file=%s", req_type, argv[2] );

		ret = trans_keyword_request_set(index, DBFTP_RESEND, argv[2]);
		foundry_info_get(index);
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Operation performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Operation performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	case 'i':
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );
#ifdef TRANSCEND
	    /* Check for retrieval of DTMS keys */
        if ((strcasecmp(argv[2], "dtms") == 0)
                || (strcasecmp(argv[2], "-dtms") == 0))
        {
            trans_dtms_server_set();
            if (dblist_request(DBLIST_DTMS_TEST_FILES, "abcd1234d", NONE, &list) > 0)
            {
                PRINTF("List of keywords with no expiration date include:\n\n");
                for (cnt = 0;list[cnt] != '\0'; ++cnt)
                    PRINTF("    %s\n", list[cnt]);
                PRINTF("\n");
            } /* End if for one or more keywords found */
        } /* End if */
	else
#endif
	    /* next check for keyword request */
            if (strcasecmp(argv[2], "all") != 0)
            {
                if ((ret = trans_keyword_request_set(index, DBINFO_DATA, argv[2])) == SUCCESS)
                    ret = trans_perform(index);
            } /* end if */
        /* Otherwise retrieve list of user keys */
            else
            {
                trans_custom_basic_request_set(index, DBUSR_KEYS, NONE);
                ret = trans_multi_keys_rtv(index);
            } /* end else */

        if (ret != SUCCESS) {
            DBG( "Operation performed, received *error* and the return value is %d with the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Operation performed, the return value is %d and the return message=\n%s", ret, rtn.msg );
        }
	}
	break;
	/* Execute ping command */
	case 'w':
	{
        DBG( "req_type='%c' is Execute ping command", req_type );

	    /* See if repository specified */
	    repository_icap_class_set(index, argc, argv, 2);
	    trans_simple_basic_request_set(index, DBPING, NONE);
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Simple ping performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Simple ping performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Execute silent ping, nothing displayed if successful */
	case 'W':
	{
        DBG( "req_type=%c is Execute silent ping, file=%s", req_type, argv[2] );

	    /* See if repository specified */
	    repository_icap_class_set(index, argc, argv, 2);
	    trans_simple_basic_request_set(index, DBPING, NONE);
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "Silent ping performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "Silent ping performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }

	    /* Exit without displaying results */
	    exit(0);
	}
	break;
	/* Retrieve file name for specified keyword */
	case 'J':
	{
        DBG( "req_type=%c is Retrieve file name for specified keyword, file=%s", req_type, argv[2] );

		if ((ret = trans_keyword_request_set(index, DBFILENAME_RTV, argv[2])) == SUCCESS) {
	        ret = trans_perform(index);
        }

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
    /* perform Cron delete or NXP lookup */
	case 'C': 
	{
        DBG( "req_type=%c is perform Cron delete or NXP lookup, file=%s", req_type, argv[2] );

	  /* External not allowed to perform cron delete */
	    if (tool.type == EXTERNAL_INTERCOM)
        {
	  sprintf(rtn.msg, "Administrative command for deletion of files .\n\n");
        }
	  else 
	  {
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 2)) > 0)
            {
                argc -= ret; argv += ret;
            }
	    if (strcasecmp(argv[2], "files") == 0)
            {
	    trans_simple_basic_request_set(index, DBCRON_DLT, NONE); 
            }
            else
            {
                trans_simple_basic_request_set(index, DBX500LOOKUP, argv[2]);
            }
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	  } /* end else */
	}
	break;
	/* Authenticate or print advanced msg */
	case 'a':
	{
        DBG( "req_type=%c is Authenticate or print advanced msg, file=%s", req_type, argv[2] );

	    /* If no arguments, print help screen */
	    if (argc == 2)
	    {
	ptr = root_find(root_ptr, '/');
            PRINTF(ADV_MSG, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr);
		exit(0); 
	    } /* end if */
	    /* Otherwise perform authentication */
	    /* If password not entered, prompt for it */
	    if (argc == 3)
	    {
		char pwd[48];
		PRINTF("Please enter your Active Directory Password\n");
		echo_stars(pwd, DBYES);
	    /* Put in separation enter */
	    PRINTF("\n\n");
		trans_simple_basic_request_set(index, DBX500AUTHEN, pwd);
	    } /* end if */
	    else trans_simple_basic_request_set(index, DBX500AUTHEN, argv[3]);
	    /* Set the e-mail to be the core ID */
	    trans_option_set(index, EMAIL, argv[2]);
	    strcpy(user.name, argv[2]);
	    strcpy(user.login, argv[2]);
	    /* Perform authentication */
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Encrypt user's password for .icomrc file */
	/* this is used only by external customers */
	case ';': 
	{
        DBG( "req_type=%c is Encrypt user's password for .icomrc file, file=%s", req_type, argv[2] );

	  if (password_scanned == DBNO)
	  {
	    PRINTF("Please enter your External LDAP NXP password. < ");
		echo_stars(reg_email1, DBNO);
	    PRINTF("\n\n");
	password_scanned = DBYES;
	  } /* End if for password not already scanned */
	    /* Send request to server for loggin purposes */
trans_simple_basic_request_set(index, DBENCRYPT_PASSWORD_LOG, reg_email1);
	    ret = trans_perform(index);
	    if (ret == SUCCESS)
	    {
		if (core_id_crypt(reg_email1, 1)  != SUCCESS)
		strcpy(rtn.msg, "Unable to encrypt password.");
		else strcpy(rtn.msg, reg_email1);
	    } /* End if for send to server successful */

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
break;
	/* View list of secure keywords */
	case 'Q':
	case 'V':
	{
        DBG( "req_type=%c is View list of secure keywords, file=%s", req_type, argv[2] );

	    /* If retrieving for all sites, call function to perform the transactions */
	    if ((argc > 2)
	    && (strcasecmp(argv[2], "all") == 0))
	    {
	    trans_custom_basic_request_set(index, DBSECURE_KEYS, NONE);
	    ret = trans_multi_keys_rtv(index);
		break;
	    }
	    /* See if repository specified */
	    repository_icap_class_set(index, argc, argv, 2);
	    if (argv[1][2] == 'l')
	    ret = trans_simple_file_get(index, DBLONG_SECURE_KEYS_RTV, NONE);
	    else
	    {
	    /* Get short list of secure keywords */
		trans_custom_basic_request_set(index, DBSECURE_KEYS, NONE);
		trans_user_keys_rtv(index);
	    } /* end else */

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* View list of file keywords */
	case 'q': 
	{
        DBG( "req_type=%c is View list of file keywords, file=%s", req_type, argv[2] );

	    /* If retrieving for all sites, call function to perform the transactions */
	    if ((argc > 2)
	    && (strcasecmp(argv[2], "all") == 0))
	    {
	    trans_custom_basic_request_set(index, DBUSR_KEYS, NONE);
	    ret = trans_multi_keys_rtv(index);
		break;
	    }
	    /* See if repository specified */
	    repository_icap_class_set(index, argc, argv, 2);
	    if (argv[1][2] == 'l')
	    ret = trans_simple_file_get(index, DBLONG_FILE_KEYS_RTV, NONE);
	    else
	    {
		int status_printed = DBNO;
	    /* Get short list of file keywords */
		trans_custom_basic_request_set(index, DBUSR_KEYS, NONE);
		trans_commandline_user_keys_rtv(index, &status_printed);
	    } /* end else */

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	case '"':
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    trans_custom_basic_request_set(index, DBSECURE_KEYS, NONE);
	    trans_user_keys_rtv(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Retrieve release info or release */
	case 'r':
	{
        DBG( "req_type=%c is Retrieve release info or release, file=%s", req_type, argv[2] );

	    if (argc == 2)
	    trans_simple_basic_request_set(index, DBGET_RELEASE_INFO, "releases");
	    else trans_custom_basic_request_set(index, DBGET_RELEASE, argv[2]);
	    /* See if customer or repository specified */
	    if ((ret = repository_icap_class_set(index, argc, argv, 2)) > 0)
	    { argc -= ret; argv += ret; }
	    ret = trans_perform(index);
	    if (ret < SUCCESS)
		ret = trans_release_print(argv);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Print help on specified command */
	case 'H':
	{
        DBG( "req_type=%c is Print help on specified command, file=%s", req_type, argv[2] );

	    ptr = root_find(root_ptr, '/');
	    if (argc > 2)
	    ret = trans_help_print(ptr, argv[2][1], DBYES);
	    else ret = trans_help_print(ptr, 'H', DBYES);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Authenticate user with his user ID and password */
	/* Transfer directory and include symbolic links */
	/* Overwrite checking for duplicate files */
	case 'm':
	{
        DBG( "req_type=%c is Authenticate user with his user ID and password, file=%s", req_type, argv[2] );

	    /* Set custom transaction */
	    if ((ret = trans_custom_keyword_request_set(index, DBSEND_KEYWORD, argv[2])) == SUCCESS)
	    {
		/* Verify that at least one user to receive keyword */
		/* has been identified */
		if (argc < 4)
		{
		    char *email_ptr[2];
		    PRINTF("Please enter the receiver's e-mail address.\n> ");
		    arg_scan(reg_email1);
		    email_ptr[0] = reg_email1;
		    email_ptr[1] = NULL;
		    trans_option_set(index, EMAIL_LIST, email_ptr);
		}
		else trans_option_set(index, EMAIL_LIST, argv+3);
		/* Get the description */
		/* GUI sets description using trans_option_set */
		trans_description_scan(index);
		ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }

	    } /* end if for keyword successfully set */
	}
	break;
	/* Retrieve list of files transferred and output to a file */
	case 'b': 
	{
        DBG( "req_type=%c is Retrieve list of files transferred and output to a file, file=%s", req_type, argv[2] );

	    if ((ret = trans_custom_keyword_request_set(index, TRANSDIRLIST_FILE_RTV, argv[2])) == SUCCESS)
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Retrieve list of files and output to standard out */
	case 'B': 
	{
        DBG( "req_type=%c is Retrieve list of files and output to standard out, file=%s", req_type, argv[2] );

	    if ((ret = trans_custom_keyword_request_set(index, TRANSDIRLIST_RTV, argv[2])) == SUCCESS)
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
#ifndef EXTERNAL
	/* Obtain registration status of specified account */
	case 'o': 
	{
        DBG( "req_type=%c is Obtain registration status of specified account, file=%s", req_type, argv[2] );

	    /* See if customer or repository specified */
	    /* If found server site is set */
	    if ((ret = repository_icap_class_set(index, argc, argv, 2)) > 0)
	    { argc -= ret; argv += ret; }
	    trans_simple_basic_request_set(index, DBSITE_CHECK, NONE);
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
#endif
	/* Query user, return X500 and site info */
	case 'u':
	{
        DBG( "req_type=%c is Query user, return X500 and site info, file=%s", req_type, argv[2] );

	    trans_custom_basic_request_set(index, DBQUERY_USER, argv[2]);
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
break;
#ifndef EXTERNAL
	/* Request approval for external customer */
	case 'j': 
    {
        DBG( "req_type=%c, argv[1]=%s is Request approval for external customer, file=%s", req_type, argv[1], argv[2] );

        if (argv[1][2] == 'i') 
	{
	    trans_commandline_approve(index, DBREQUEST_STATUS, argc, argv);
	}
	else {
	    trans_commandline_approve(index, DBREQUEST_APPROVAL, argc, argv);
	}

	if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
    }
	break;
	/* Perform command line approval for existing approval request */
	case 'y':
{
    DBG( "req_type=%c is Perform command line approval for existing approval request, file=%s", req_type, argv[2] );

    trans_commandline_approve(index, EDC_COMMANDLINE_APPROVE, argc, argv);

	if (ret != SUCCESS) {
        DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
    } else {
        DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
    }
}
break;
#endif
	/* View iCAP info */
	case 'I':   
	{
        DBG( "req_type=%c is View iCAP info, file=%s", req_type, argv[2] );

	    ptr = root_find(argv[0], '/');
	    PRINTF(ICAP_MSG, ptr); exit(0);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Get data  */
	case 'g':
	{
        DBG( "req_type=%c is Get data, file=%s", req_type, argv[2] );

	    if ((ret = trans_get_request_set(index, DBGET_DATA, argv[1][2], argv[2])) < 0) break;
	if (argv[1][2] == '1') PRINTF(g1flag_wrn);
	    tar_option = argv[1][2];
	    /* If tar option specifies a transfer type, set it */
	    trans_get_transfer_type_set(index, tar_option);
	    /* Check to see if directory specified */
	    if (argc > 3) 
	    {
		trans_field_allocate(&trans[index]->alternate_dir, argv[3]);
		/* Check for alternate file name */
	    if (argc > 4) 
		trans_field_allocate(&trans[index]->alternate_file, argv[4]);
	    } /* end if */
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	case 'G':
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    trans_get_request_set(index, DBGETDLT_DATA, argv[1][2], argv[2]);
	if (argv[1][2] == '1') PRINTF(g1flag_wrn);
	    tar_option = argv[1][2];
	    /* If tar option specifies a transfer type, set it */
	    trans_get_transfer_type_set(index, tar_option);
	    /* Check to see if directory specified */
	    if (argc > 3) 
	    {
		trans_field_allocate(trans[index]->alternate_dir, argv[3]);
		/* Check for alternate file name */
	    if (argc > 4) 
		trans_field_allocate(trans[index]->alternate_file, argv[4]);
	    } /* end if */
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Display version information */
	case 'v': ret = version_display(); exit(0);
	case 'Y':
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    if ((ret = repository_icap_class_set(index, argc, argv, 2)) > 0)
	    { argc -= ret; argv += ret; }
	    trans_simple_basic_request_set(index, DBEXTRACT_REGISTRY, NONE);
	    trans_perform(index);
	}
	break;
	/* Display advanced put/get options */
	case 'A':
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    ptr = root_find(root_ptr, '/');
	    PRINTF(PUT_GET_MSG, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr);
	    exit(0);
	}
	break;
	/* Set trusted user */
	/* Print DB administrator help screen */
	case '*':
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    ptr = root_find(root_ptr, '/');
	    PRINTF(ADMIN_MSG, 
	    ptr, ptr, ptr, ptr, ptr);
	    exit(0);
	}
	break;
#ifdef NXP
	case ':':
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    ptr = root_find(root_ptr, '/');
	    PRINTF(CUST_MSG, ptr, ptr, ptr);
	    exit(0);
	}
	break;
#endif
	/* Assign data type to be ASCII */
	/* Release shared memory */
#ifndef WINDOWS_NT4
	case 'M': 
	{
	    ret = shared_memory_cleanup(user.login); 
	}
	break;
#endif
	/* cron delete */
	/* displays user's name, e-mail telephone, department, etc */
	/* does not list the Transcend sites */
	case '1': 
	{
        DBG( "req_type=%c is displays user's name, e-mail telephone, department, etc, file=%s", req_type, argv[2] );

	    /* Set request type to fast user query */
	    trans_custom_basic_request_set(index, DBFAST_QUERY_USER, argv[2]);
	    /* Perform the transaction */
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Rebuild the user table */
	case '2':  ret = trans_request_send(DBUSR_TABLE_BUILD, index); break;
	case '3':  ret = trans_support_request(index, argc, argv, SUPPORT_EMAIL_LIST, SUPPORT_EMAIL_ADMIN); break;
	case '4':  ret = trans_support_request(index, argc, argv, MICA_EMAIL_LIST, SUPPORT_EMAIL_ADMIN); break;
	/* Retrieve list of GUI companies */
	case '5':
	{
        DBG( "req_type=%c is Retrieve list of GUI companies, file=%s", req_type, argv[2] );

	    trans_custom_basic_request_set(index, GUI_COMPANY_TABLE_RTV, NONE);
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	case '7':
	{
	if (argc > 4)
	ret = trans_edc_request(index, DBTRUST_USER, argv[3], argv[4], argv[2]);
	else if (argc > 3)
	ret = trans_edc_request(index, DBTRUST_USER, argv[3], NONE, argv[2]);
	}
	break;
	/* Get file status */
	/* Returns global e-mail flag, deletion flag, extension days, etc */
    case '8': 
	{
	    if ((ret = trans_keyword_request_set(index, DBSTATUS_DATA, argv[2])) < 0) break; 
	    ret = trans_list_rtv(index, &list);
	    for (cnt = 0; list[cnt] != NULL; ++cnt)
	    PRINTF("%s\n", list[cnt]);
	}
	break;
	case '9':  
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    trans_simple_basic_request_set(index, DBREG_AUDIT, NONE);
	    trans_simple_basic_request_set(index, DBRTV_NAME_PHONE, argv[2]);
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
/*
	case 'D': 
	{
    if ((strlen(argv[2]) != 2)
	    || (!isdigit(argv[2][0])) || (!isdigit(argv[2][1])))
	    {
		ptr = root_find(root_ptr, '/');
		PRINTF(TRANS_FORMAT_ERR);
	PRINTF(TRANS_FORMAT_MSG, ptr, ptr, ptr, ptr, ptr, ptr, ptr,  ptr);
		exit(101);
	    }
	    trans_simple_basic_request_set(index, DBDLT_OLD_SITES, argv[2]); 
	    ret = trans_perform(index);
	}
	break;
*/
	/* Allow user to perform auto registration */
	/* If external customer, server will return the URL where he can register */
	/* The user's e-mail address is passed in par1 */
	/* The user's active directory password is passed in par2 */
	case 'R':  ret = trans_reg_perform(index, argc, argv); break;
	case ',': 
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    trans_simple_basic_request_set(index, DBREG_QUERY, argv[2]); 
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
    case 'x':
        /**
         *   Carlos Mazieri, Oct 2019
         *   changed to get the list from the server,he hard coded list still exist in
         *         client/transapi_client_globals.c
         */
        trans_company_table_rtv();
        rtn.len = 0;
        transapi_site_print(index);
        break;
	case 'X': 
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    /* If this is an external customer, encrypt his password */
	    if (tool.type == EXTERNAL_INTERCOM) ret = trans_password_encrypt(index);
	    else ret = trans_repository_print();

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* View encrypted password */
	/* Retrieve list of servers with their status */
	case 'S':
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    ret = transapi_server_status_rtv(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }
	}
	break;
	/* Do Ping and send email if Ping not successful */
	case '.':  
	{
        DBG( "req_type=%c, file=%s", req_type, argv[2] );

	    trans_simple_basic_request_set(index, DBPING, NONE); 
	    ret = trans_perform(index);

	    if (ret != SUCCESS) {
            DBG( "performed with *error*, the return value is %d, and the return message=%s", ret, rtn.msg );
        } else {
            DBG( "performed, the return value is %d, and the return message=%s", ret, rtn.msg );
        }

	    if (ret != SUCCESS)
	        email_send("", "transbug@nxp.com", "Unable to Ping Transcend server", rtn.msg);
	}
	exit(0);
	/* Retrieve status for data present in repository */
	default:
	{
        DBG( "(default switch-case) Retrieve status for data present in repository, file=%s", "", argv[2] );

	    /* Look for the leaf of the executable file name */
	    ptr = root_find(root_ptr, '/');
	    PRINTF(TRANS_FORMAT_ERR);
	    PRINTF(TRANS_FORMAT_MSG, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr);
	    exit(101);
	}
    } /* end switch */
    /* Make sure there are two carriage returns on the string */
    /* Get length of the string */
    label: len = strlen(rtn.msg)-1;
    /* Find first character before carriage return */
    for (cnt = len; cnt > 0 && rtn.msg[cnt] == '\n'; --cnt)
    /* Lop off all of the carriage returns */
     /* We want to put them in ourselves to keep them consistent */
    rtn.msg[cnt] = '\0';
#ifdef SEND2FOUNDRY
    status_log("transfer_msg", rtn.msg);
#else
	printf("%s\n\n", rtn.msg);
    fflush(stdout);
#endif
    /* Free the trans structure no longer needed */
    trans_cleanup(index);
     /* convert return value to a positive integer */
    ret = -ret;
     /* If return value is greater than 300, subtract 300 */
    if (ret > 300) ret -= 300;
#ifdef TRANSCEND_GUI
    /* Print status message for the GUI */
    if (ret == 0)
    PRINTF("Operation completed successfully.\n");
    else PRINTF("Operation completed with errors.\n");
#endif /* for TRANSCEND_GUI */
	exit(ret);
} /* end main */

char z_errmsg[80];
#endif  // TRANSAPI
