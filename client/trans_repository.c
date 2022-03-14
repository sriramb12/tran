
/****************************************************************************
 *
 * Function:	trans_repository_find
 *
 * Description:	functions to set, find, and print Transcend sites and repositories
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#ifdef WINDOWS_NT4
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#endif

#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "global_params.h"
#include "server_params.h"
#include "global_defines.h"
#include "rtn.h"
#include "scan_prototypes.h"
#include "strcpy.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transapi_defines.h"
#include "transapi_prototypes.h"
#include "transapi_hidden_prototypes.h"

#include "global_debug.h" // Mazieri's debug facilities


extern TRANS_TOOL tool;
extern TRANS_REPOSITORY repository[];
extern TRANS_COMPANY site[];
char *trans_extension_map(char extension);
int icom_server_status_rtv(int index);
void trans_company_table_rtv(void);
int icom_site_print(int index);

/* Map the one character extension to the three character extension */
char *trans_extension_map(char extension)
{
    char *ret = "phx";
    switch (extension)
    {
        case 'p': ret = "phx"; break;
        case 'n': ret = "idc"; break;
        case 'd': ret = "dtms"; break;
        case 'e': ret = "etc"; break;
        case 's': ret = "tsm"; break;
        case 'i': ret = "adam"; break;
        case 'g': ret = "shg" ; break;
        default:  break;
    } /* end switch */

    DBG("extension='%c' ret=%s", extension, ret);
    return ret;

} /* end trans_extension_map */

/* Obtain status for InterCom servers */
int icom_server_status_rtv(int index)
{
    DBG();

    char status[12];

    strcpy(server.address, repository[tool.repository_index[0]].address);
    strcpy(server.machine, repository[tool.repository_index[0]].machine);
    server.socket = htons(repository[tool.repository_index[0]].socket);
    if (trans_request_send(DBPING, index) == SUCCESS)
    {
        strcpy(status, "Online");
    }
    else
    {
        strcpy(status, "Offline");
    }
    /* Now print the server and abbreviation */
    sprintf(rtn.msg, "%s;%s;%s\n", repository[tool.repository_index[0]].name,
            trans_extension_map(repository[tool.repository_index[0]].extension), status);
    return(SUCCESS);
} /* end icom_server_status_rtv */

/* Get the latest company table from the data repository */
void trans_company_table_rtv(void)
{
    DBG();

    int ret;
    char line[260];
    int len;
    int cnt;
    int index;

    /** Carlos Mazieri, Oct 2019
     *
     *  check if already done
     */
    if (trans[0] != 0 && trans[0]->company_list_rtvd == DBYES)
    {
        return;
    }

    /* Allocate structure for this transaction */
    index = trans_init(DBNO, DBNO,  DBNO, DBNO);

    trans_default_repository_set();

    /* If unable to do the connect, simply return */
    if ((ret = trans_request_send(DBCOMPANY_TABLE_RTV, index)) != SUCCESS)
    {
	    trans_cleanup(index);
	    return;
    } /* end if */

    /* Scan data from return message into global table */
    for (cnt = tool.site_index[0], rtn.len = 0; (len = str_line_get(rtn.msg+rtn.len, line)) > 0; ++cnt)
    {
	    sscanf(line, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c",
            site[cnt].name, site[cnt].abbreviation, site[cnt].machine, site[cnt].address,
            site[cnt].socket, site[cnt].server_type, site[cnt].email, site[cnt].is_foundry, site[cnt].is_maskshop, site[cnt].compression);
	    rtn.len += len+1;
    }

    /* Set index to new location */
    tool.site_index[1] = cnt-1;

    DBG("tool.site_index[1]=%d", tool.site_index[1]);

    /* Free structure no longer needed */
    trans_cleanup(index);
    if (trans[0] != 0)
    {
        trans[0]->company_list_rtvd = DBYES;
    }
    return;
} /* End trans_company_table_rtv */


/* Print a list of supported companies */
int trans_site_print(void)
{
    DBG();

    int cnt;
    char string[80];

    /* Print abbreviation */
    rtn.len += strcpy1(rtn.msg+rtn.len, "Company                                      Abbreviation\n");
    rtn.len += strcpy1(rtn.msg+rtn.len, "------------------------                     ------------\n");

    /* Print companies in a for loop */
    for (cnt = tool.site_index[0]; cnt <= tool.site_index[1]; ++ cnt)
    {
        strcpy(string, site[cnt].name);
        str_pad(string, 45);
        strcpy2(string+45, site[cnt].abbreviation, "\n");
        rtn.len += sprintf(rtn.msg + rtn.len, string);
    } /* end for */

    /* Put out extra carriage return */
    sprintf(rtn.msg+rtn.len, "\n");

    return(SUCCESS);
} /* End trans_site_print */


/* Print list of repositories for this tool */
int trans_repository_print(void)
{
    DBG();

    int cnt;
    char string[80];

    /* Print extension */
    rtn.len += sprintf(rtn.msg+rtn.len, "Data Repository               Abbreviation   Keyword Extension   Example\n");
    rtn.len += sprintf(rtn.msg+rtn.len, "-----------------             ------------   -----------------   ---------\n\n");
    /* Print companies in a for loop */
    for (cnt = tool.repository_index[0]; cnt <= tool.repository_index[1]; ++cnt)
    {
        strcpy(string, repository[cnt].name);
        str_pad(string, 31);
        sprintf(string+30, "%-14s %c                   abcd1234%c\n",
                           repository[cnt].abbreviation, repository[cnt].extension, repository[cnt].extension);
        rtn.len += sprintf(rtn.msg+rtn.len, string);
    } /* end for */
    /* Put out extra carriage return */
    sprintf(rtn.msg+rtn.len, "\n");
    return(0);
} /* End trans_repository_print */


/* Find the repository for the specified keyword extension */
void trans_repository_find(char extension)
{
    DBG();

    int cnt;

    /* Look for specified site */
    /* Read through the table looking for specified site */
    for (cnt = tool.repository_index[0]; cnt <= tool.repository_index[1]; ++cnt)
    {
        if ((extension == repository[cnt].extension)
                || (extension == repository[cnt].secure_extension))
        {
            strcpy(server.address, repository[cnt].address);
            strcpy(server.machine, repository[cnt].machine);
            server.socket = htons(repository[cnt].socket);
            strcpy(server.site, repository[cnt].name);
            return;
        } /* End if for site found */
    } /* End for */
     /* If reach this point, site not found */
    /* Use default repository */
    trans_default_repository_set();
} /* end trans_repository_find */

void trans_repository_scan(char *line)
{
    DBG();

    /* Scan line into parameters */
    sscanf(line, "%[^;]%*c%*[^;]%*c%*c%*c %*c%*c %[^;]%*c %[^;]%*c %d",
    server.site, server.machine, server.address, &server.socket);
    server.socket = htons(server.socket);
} /* End repository_scan */

int trans_site_find(int index, char *user_abbreviation)
{
    int len;
    int cnt;
    char abbreviation[32];

    DBG( "index=%d, user_abbreviation=%c", index, user_abbreviation );

    /* Get a copy of the company table */
    if (trans[index]->company_list_rtvd == DBNO)
    {
        trans_company_table_rtv();
        trans[index]->company_list_rtvd = DBYES;
        rtn.len = 0;
    } /* end if */
    /* Read through the table looking for specified company */
    len = strlen(user_abbreviation);
    if (len >= 3)
    {
        /* do not change the original string */
        strcpy(abbreviation, user_abbreviation);
        str_lower(abbreviation);
        for (cnt = tool.site_index[0]; cnt <= tool.site_index[1]; ++ cnt)
        {
            if ((strncmp(abbreviation, site[cnt].abbreviation, len) == 0)
                    || (strcasecmp(abbreviation, site[cnt].name) == 0))
            {
                strcpy(server.address, site[cnt].address);
                strcpy(server.machine, site[cnt].machine);
                server.socket = htons(atoi(site[cnt].socket));
                strcpy(server.site, site[cnt].name);
                /* Check to see if this is a foundry */
                if (strcmp(site[cnt].is_foundry, "yes") == 0)
                {
                    PRINTF(FTP_TRANSFER_MSG, site[cnt].email, site[cnt].email);
                    /* Remote directory temporarily stored in the secondary e-mail address */
                    arg_scan(trans[index]->email2);
                } /* end if for is foundry */
                /* check to see if this is a maskshop */
                else if (strcmp(site[cnt].is_maskshop, "yes") == 0)
                {
                    /* Set the type to maskshop */
                    trans[index]->is_maskshop = DBYES;
                    if (strcmp(site[cnt].email, NONE) != 0)
                    {
                        /* Allocate spacefor the e-mail for the maskshop */
                        trans[index]->email_list = malloc(strlen(site[cnt].email) +10);
                        strcpy(trans[index]->email_list, site[cnt].email);
                        strcpy(trans[index]->par1, site[cnt].email);
                    }
                    /* If this is ITAR data, indicate it is */
                    if ((strcasecmp(abbreviation, "itr") == 0)
                            || (strcasecmp(abbreviation, "pti") == 0))
                        trans[index]->is_itar = DBYES;
                } /* end if for maskshop */
                /* If e-mail specified for any company, set it */
                else if (strcmp(site[cnt].email, NONE) != 0)
                {
                    trans[index]->email_list = malloc(strlen(site[cnt].email) +10);
                    strcpy(trans[index]->email_list, site[cnt].email);
                    strcpy(trans[index]->par1, site[cnt].email);
                } /* end else */
                return(SUCCESS);
            } /* End if for company found */
        } /* End for */
    } /* End if */
     /* If reach this point, company not found */
    return(-1);
} /* end trans_site_find */


/* Sets default site for transaction */
int trans_default_repository_set(void)
{
    DBG();

    /* Use the first repository specified for this tool */
    int index = tool.repository_index[0];
    DBG("index = '%d'", index);

    strcpy(server.address, repository[index].address);
    strcpy(server.machine, repository[index].machine);

    server.socket = htons(repository[index].socket);
    strcpy(server.site, repository[index].name);

    DBG("Default repository server\nserver.address=%s\nserver.machine=%s\nserver.site=%s\nserver.socket=%d", server.address, server.machine, server.site, repository[index].socket);

    return(SUCCESS);
} /* End trans_default_repository_set */

int trans_default_site_set(void)
{
    DBG();
    /* Use the first site specified for this tool */
    int index = tool.site_index[0];

    strcpy(server.address, site[index].address);
    strcpy(server.machine, site[index].machine);
    server.socket = htons(atoi(site[index].socket));
    strcpy(server.site, site[index].name);
    return(SUCCESS);
} /* End trans_default_site_set */

void trans_dtms_server_set(void)
{
    DBG();
    int cnt;

    for (cnt = tool.site_index[0]; cnt <= tool.site_index[1]; ++ cnt)
    {
        if (strcasecmp(site[cnt].name, "DTMS") == 0) break;
    }
    strcpy(server.address, site[cnt].address);
    strcpy(server.machine, site[cnt].machine);
    server.socket = htons(atoi(site[cnt].socket));
    strcpy(server.site, site[cnt].name);
} /* End trans_dtms_server_set */


/* Find the server closest to the customer */
int trans_closest_server_set(void)
{
    DBG();

    int index = trans_init(DBNO, DBNO, DBNO, DBNO);

    /* Set default repository so have somewhere to make request */
    trans_default_repository_set();

    /* Send request to find nearest server */
    if (trans_request_send(DBSERVER_SET, index) == SUCCESS)
    {
        sscanf(rtn.msg, "%s %s %s %d", server.site, server.machine, server.address, &server.socket);
	    server.socket = htons(server.socket);
    }

    /* Free structure no longer needed */
    trans_cleanup(index);

    return(SUCCESS);
} /* End trans_closest_server_set */


/* Retrieve status of Transcend servers */
/* This function is used by the GUI to report server status */
int transapi_server_status_rtv(int index)
{
    DBG();

    if ((tool.type == INTERNAL_TRANSCEND)
    || (tool.type == TRANSCEND_INTERCOM))
    return(trans_server_status_rtv(index));
    else return(icom_server_status_rtv(index));
} /* end transapi_server_status_rtv */

/* Retrieve status for servers */
int trans_server_status_rtv(int index)
{
    DBG();

    char *server_status[15];
    char line[64];
    char status[15];
    int online_status[15];
    int len;
    int server_cnt;
    int cnt;

    for (server_cnt = tool.repository_index[0]; server_cnt <= tool.repository_index[1]; ++server_cnt)
    {
        strcpy(server.address, repository[server_cnt].address);
        strcpy(server.machine, repository[server_cnt].machine);
        server.socket = htons(repository[server_cnt].socket);
        if (trans_request_send(DBPING, index) == SUCCESS)
        {
            strcpy(status, "Online");
            online_status[server_cnt] = DBYES;
        } else {
            strcpy(status, "Offline");
            online_status[server_cnt] = DBNO;
        } /* end else */
        /* Now print the server and abbreviation */
        len = sprintf(line, "%s;%s;%s\n", repository[server_cnt].name, trans_extension_map(repository[server_cnt].extension), status);
        /* Allocate space for the status */
        server_status[server_cnt] = malloc(len+1);
        strcpy(server_status[server_cnt], line);
    } /* End for */
    /* Now get the default server */
    if ((tool.type == INTERNAL_TRANSCEND)
            || (tool.type == TRANSCEND_INTERCOM))
        trans_closest_server_set();
    /* This function sets server.site, server.address, etc. */
    /* Find the server in the list and print it */
    for (cnt = 0; cnt < server_cnt; ++cnt)
    {
        if (strcmp(server.site, site[cnt].name) == 0)
        {
            rtn.len = sprintf(rtn.msg, "%s", server_status[cnt]);
            online_status[cnt] = SUCCESS;
            break;
        } /* end if */
    } /* end for */
    /* Print all online servers first */
    for (cnt = 0;  cnt < server_cnt; ++cnt)
    {
        if (online_status[cnt] == DBYES)
            rtn.len += sprintf(rtn.msg+rtn.len, "%s", server_status[cnt]);
    }
    /* Now go through the servers and pick up any that are off line */
    for (cnt = 0; cnt < server_cnt; ++cnt)
    {
        if (online_status[cnt] == DBNO)
            rtn.len += sprintf(rtn.msg+rtn.len, "%s", server_status[cnt]);
    } /* End for */
    /* Indicate transaction successful */
    return(SUCCESS);
} /* End trans_server_status_rtv */

/* Set repository */
int  trans_repository_set(int index)
{
    DBG();

    extern int repository_index[7][2];
    /* Verify the index is within the range for this tool */
    if ((index < repository_index[tool.type][0])
    || ( index > repository_index[tool.type][1]))
    {
	strcpy(rtn.msg, tool_index_err);
	return(TOOL_INDEX_ERR);
    }
    	strcpy(server.address, repository[index].address);
    strcpy(server.machine, repository[index].machine);
    server.socket = htons(repository[index].socket);;
    	strcpy(server.site, repository[index].name);
    sprintf(rtn.msg, "Repository set to site %s, machine %s, address %s, port %d\n",
    server.site, server.machine, server.address, htons(server.socket));
    return(SUCCESS);
} /* end trans_repository_set */


/* Print list of defined companies */
int icom_site_print(int index)
{
    DBG();

    char email[6][64];
    int param_num;
    int cnt;
    int cnt1;
    char string[260];

    /* Print abbreviation */
    rtn.len = strcpy1(rtn.msg, "COMPANY                            ABR  TOOL TYPE E-MAIL NOTIFICATION\n");
        rtn.len += strcpy1(rtn.msg+rtn.len, "------------------------           ---  --------- -------------------\n");
    /* Print companies in a for loop */
    for (cnt = tool.site_index[0]; cnt <= tool.site_index[1]; ++ cnt)
    {
        strcpy(string, site[cnt].name);
        str_pad(string, 35);
        strcpy(string+35, site[cnt].abbreviation);
    str_pad(string, 40);
	/* this tells the transfer type */
     strcpy(string+40, site[cnt].server_type);
    str_pad(string, 50);
	/* Parse out the email addresses */
	email[0][0] = '\0';
	param_num = sscanf(site[cnt].email, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]",
	email[0], email[1], email[2], email[3], email[4], email[5]);
	if (strcmp(email[0], NONE) == 0)
	strcpy(string+50, "\n");
     else strcpy2(string+50, email[0], "\n");
	rtn.len += strcpy1(rtn.msg+rtn.len, string);
	for (cnt1 = 1; cnt1 <param_num; ++cnt1)
	rtn.len += sprintf(rtn.msg+rtn.len, "                                               %s  \n", email[cnt1]);
    } /* end for */
    /* Put out extra carriage return */
    strcpy(rtn.msg+rtn.len, "\n");
    return(SUCCESS);
} /* End icom_site_print */


/* Print defined sites for Transcend and InterCom */
int transapi_site_print(int index)
{
    DBG( "index=%d", index );   
    switch(tool.type)
    {
       case INTERNAL_TRANSCEND: return( trans_site_print()); break;
       case TRANSCEND_INTERCOM: return( trans_site_print()); break;
       case INTERNAL_INTERCOM:  return(icom_site_print(index)); break;
       case EXTERNAL_INTERCOM:  strcpy(rtn.msg, "All transfers are performed to/from NXP Semiconductor."); break;
       default:  strcpy(rtn.msg, "No sites defined."); return(SUCCESS);
    } /* end switch */
    return(SUCCESS);
} /* end transapi_site_print */

