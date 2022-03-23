#ifdef TRANSAPI
/****************************************************************************
 *
 * Function:	version_display
 *
 * Description:	Displays version information for Transcend/InterCom.
 *
 ***************************************************************************/

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#ifndef WINDOWS_NT4
#include <netinet/in.h>
#endif
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include "global_defines.h"
#include "rtn.h"
#include "server_params.h"
#include "trans_defines.h"
#include "trans_prototypes.h"
#include "transrc.h"
#include "transapi_defines.h"

#include "global_debug.h"


int version_display()
{
    DBG();

    char string[16];

    PRINTF("Software Version:   %s\n", TRANSAPI_VERSION);
    PRINTF("Company:            %s\n", user.from_company);
    PRINTF("Platform:           %s\n", MACHINE_TYPE);
    PRINTF("Local machine:      %s\n", user.mach);
    PRINTF("Local address:      %s\n", user.addr);
#ifdef PHOTRONICS
    PRINTF("Server machine:     %s\n", server.machine);
    PRINTF("Server address:     %s\n", server.address);
#else
    PRINTF("Server machine:     %s\n", server.machine);
    PRINTF("Server Address:     %s\n", server.address);
#endif

    PRINTF("Transport socket:   %d\n", ntohs(server.socket));
    if (transrc.logDir[0] == '\0')
        PRINTF("Log directory:      None\n");
    else {
	    printf("Log directory:      ");

	    if (strlen(transrc.logDir) < 60)
	        PRINTF("%s\n", transrc.logDir);
	    else
	        dir_print(transrc.logDir);
    } /* End else */

    if (transrc.historyDir[0] == '\0')
        PRINTF("History directory:  None\n");
    else {
	    printf("History directory:  ");

	    if (strlen(transrc.historyDir) < 60)
	        PRINTF("%s\n", transrc.historyDir);
	    else
	        dir_print(transrc.historyDir);
    } /* End else */

    PRINTF("Current directory:  ");
    dir_print(user.cwd);

    PRINTF("User login:         %s\n", user.login);
    PRINTF("User name:          %s\n", user.name);

    if (transrc.hashByTime > 0)
        PRINTF("Time display:       %d sec.\n", transrc.hashByTime);
    else
        PRINTF("Time display:       None\n");

    if (transrc.hashBySize > 0) {
	    if (transrc.hashBySize < 1024*1024)
	        sprintf(string, "%d bytes", transrc.hashBySize);
	    else
	        sprintf(string, "%d mb.", transrc.hashBySize /1024/1024);

	    printf("Size display:       %s\n\n", string);
    } /* End if */
    else
        PRINTF("Size Display:       None\n\n");

    /* Clear the return message so not printed */
    rtn.msg[0] = '\0';

    return(0);
} /* End version_display */


void dir_print(char *dir)
{
    DBG("dir='%s'", dir);

    int line_len = 20;
    int total_len = 0;
    int len;
    char *ptr = dir;

    while (ptr[0] == '/') {
	    printf("/");

	    ++ptr;
	    ++line_len;
	    ++total_len;
    }

    ptr = strtok(dir, "/");

    if (ptr != NULL){
	    printf(ptr);
	    len = strlen(ptr)+1;
	    line_len += len;
	    total_len += len;
    } /* End if for NULL pointer not found */

    while ((ptr = strtok(dir+total_len, "/")) != NULL) {
	    printf("/");
	    len = strlen(ptr)+1;

	    if (line_len + len < 78) {
	        line_len += len;
	        PRINTF(ptr);
	    } else{
	        PRINTF("\n                    %s", ptr);
	        line_len = 20+len;
	    } /* end else */

	    total_len += len;
    } /* END WHILE */

    PRINTF("\n");
} /* End dir_print */
#endif
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
