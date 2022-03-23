#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	trans_rcread
 *
 * Description:	Routine to read the .transrc or .icomrc file.
 *		        the settings in this file allow the user to configure
 *		        how the software runs.
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef WINDOWS_NT4
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#endif /* for WINDOWS */

#include <string.h>
#include "company.h"
#include "global_defines.h"
#include "server_params.h"
#include "scan_prototypes.h"
#include "strcpy.h"
#include "transrc.h"
#include "transapi_defines.h"
#include "trans_client_errs.h"
#include "trans_defines.h"
#include "trans_prototypes.h"

#include "global_debug.h"


extern char certificate_dir[];
extern char certificate_file[];
extern char global_par6[];
extern char reg_email1[];
extern char reg_email2[];
int rc_wrn;


void trans_rcread(char *rc_file)
{
    DBG("current rc_file is '%s'", rc_file);
    
    extern char icomrc_path[];
    FILE *fp;
    char file_name[FILE_SIZE];
    char line[1024];

    /* Indicate no warnings found */
    rc_wrn = DBNO;

    /* Assign default values */
    /* Values for server machine and socket already assigned */
    /* Print out status every 30 seconds */
    transrc.hashByTime = 30;

    /* Print out amount transcended every 50 mb. */
    transrc.hashBySize = 50*1024*1024;

    /* Log transactions in current directory */
    strcpy(transrc.logDir, user.cwd);

    /* Put history files in current directory */
    strcpy(transrc.historyDir, user.cwd);

#ifdef EXTERNAL
    /* Indicate password and e-mail address not set */
    /* Also indicate certificate location not set */
    global_par6[0] = certificate_file[0] = certificate_dir[0] = 
    reg_email1[0] = reg_email2[0] = '\0';
#endif

    transrc.parallel_zip_size = 1073741824;
    transrc.separate_block_size = 0;

    /* For all tools build name of RC file */
    strcpy3(file_name, icomrc_path, "/", rc_file);

    /* Open the file */
    DBG("PATH for the rc_file is '%s'", file_name);
    if ((fp = fopen(file_name, "r")) == NULL) 
        return;

    /* Read the file a line at a time */
    while (line_get(fp, line) != EOF)
    {
        DBG("will parse line '%s'", line);
        line_parse(line);
    }

    /* Close the file */
    fclose(fp);

    DBG("reg_email1=%s reg_email2=%s", reg_email1, reg_email2);

    /* check to see if warning needs to be printed */
    if (rc_wrn == DBYES)
    	PRINTF(transrc_wrn);
} /* End trans_rcread */


void line_parse(char *line)
{
    DBG();
    int cnt;
    int keyword_cnt;
    int i;
    char keyword1[256];
    char keyword2[256];
    char keyword3[256];
    char keyword4[256];
    char keyword5[256];
    char keyword6[256];
    char num[12];

    /* If this is blank or a comment, return */
    if ((line[0] == '#') || (line[0] == '\0')) return;
    /* Scan in the first six tokens */
    /* If it is a blank line return */

    if ((keyword_cnt = sscanf(line, "%[^:]%*c %s %s%s %s %s", 
    keyword1, keyword2, keyword3, keyword4, keyword5, keyword6)) < 2) return;
    /* Set user e-mail */
    if (strcmp(keyword1, "User e-mail") == 0)
    {
	strcpy(reg_email1, keyword2);
    } /* End if for email */
#ifndef TRANSWEB
    else if (strcmp(keyword1, "User password") == 0)
    {
        /* Indicate that encrypted password from config file being used */
        strcpy(global_par6, "encrypt");
        /* Copy the password into reg_email2 */
        strcpy(reg_email2, keyword2);
        /*
         * March 2018 Carlos Mazieri
         * commented this code it should be wrong
           DBG("reg_email1=%s reg_email2=%s", reg_email1, reg_email2);
           core_id_crypt(reg_email2, 2);
           DBG("reg_email1=%s reg_email2=%s", reg_email1, reg_email2);
         */
    } /* End if for password */
#endif
    /* check for certificate directory */
    else if (strcmp(keyword1, "Certificate dir") == 0)
    strcpy(certificate_dir, keyword2);
    /* Check for certificate file */
    else if (strcmp(keyword1, "Certificate file") == 0)
    strcpy(certificate_file, keyword2);
    /* Set hash by time options */
    else if (strcmp(keyword1, "Block size") == 0)
    {
	transrc.separate_block_size = atoll(keyword2);
    }
    else if (strcmp(keyword1, "PrintByTime") == 0)
    {
	for (cnt = 0; keyword2[cnt] != '\0'; ++cnt)
    {
	if ((keyword2[cnt] >= '0' ) 
	&& (keyword2[cnt] <= '9')) num[cnt] = keyword2[cnt];
	else break;
    } /* End for */
    /* If valid number found, proceed */
    if (cnt > 0)
    {
	    num[cnt] = '\0';
	    i = atoi(num);
	    /* If valid value found, copy it */
	    if (i >= 10) transrc.hashByTime = i;
	    /* If valid but less than 10 set to 10 */
	    /* 10 is the smallest value allowed */
	    else if (i > 0) transrc.hashByTime = 10;
	    else transrc.hashByTime = 0;
	} /* End if for valid value found */
    }
    /* Set hash by date options */
    else if (strcmp(keyword1, "PrintBySize") == 0)
    {
	/* Look for the first nonnumeric character */
	/* in keyword 2 */
	for (cnt = 0; keyword2[cnt] != '\0'; ++cnt)
    {
	if ((keyword2[cnt] >= '0' ) 
	&& (keyword2[cnt] <= '9')) num[cnt] = keyword2[cnt];
	else break;
    } /* End for */
    /* If valid number found, proceed */
    if (cnt > 0)
    {
	    num[cnt] = '\0';
	    i = atoi(num);
	    if (keyword2[cnt] != '\0')
	    keyword3[0] = keyword2[cnt];
	    keyword3[1] = '\0';
	    /* If this is megabytes, make necessary conversion */
	    if ((keyword3[0] == 'm') || (keyword3[0] == 'M')) i *= 1024*1024;
	    /* If this is kilobytes, make necessary conversion */
	    else if ((keyword3[0] == 'k') || (keyword3[0] == 'K')) i *= 1024;
	    if (i > 1024*10) transrc.hashBySize = i;
	    else if (i > 0) transrc.hashBySize = 10*1024;
	    else transrc.hashBySize = 0;
	} /* End if */
    }
    /* Set the log directory name */
    else if (strcmp(keyword1, "LogDirectory") == 0)
    {
	if (strcmp(keyword2, ".") == 0)
    strcpy(transrc.logDir, user.cwd);
	else if (strcmp(keyword2, "~") == 0) 
    strcpy(transrc.logDir, user.home_dir);
	else if (strcmp(keyword2, "None") == 0)
	transrc.logDir[0] = '\0';
    }
    else if (strcmp(keyword1, "HistoryDirectory") == 0)
    {
	if (strcmp(keyword2, ".") == 0)
    strcpy(transrc.historyDir, user.cwd);
	else if (strcmp(keyword2, "~") == 0) 
    strcpy(transrc.historyDir, user.home_dir);
	else if (strcmp(keyword2, "None") == 0)
	transrc.historyDir[0] = '\0';
    }
    else if (strcmp(keyword1, "Parallel zip size") == 0)
    {
	sscanf(keyword2, "%llu", &transrc.parallel_zip_size);
	/* Do not allow parallel transfer for blocks less than 5 Mb */
	if (transrc.parallel_zip_size < 5000000)
	transrc.parallel_zip_size = 5000000;
    }
    /* If 6 keywords found, look for default servers */
    else if (keyword_cnt < 6) return;
} /* End line_parse */

#ifdef TRANSCEND_GUI
void transrc_set(void)
{
    DBG();
    char file_name[FILE_SIZE];
     char line[160];

    /* Assign default values */
    /* Values for server machine and socket already assigned */
    /* Print out status every 30 seconds */
    transrc.hashByTime = 30;
    /* Print out amount transcended every 4 mb. */
    transrc.hashBySize = 4*1024*1024;
    /* Do not log transactions */
    strcpy(transrc.logDir, "None");
    /* Do not save history files */
    strcpy(transrc.historyDir, "None");
} /* End transrc_set */
#endif /* For TRANSCEND_GUI */
#endif /* for TRANSAPI */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
