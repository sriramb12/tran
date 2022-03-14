/****************************************************************************
 *
 * Function:    owner_check
 *
 * Description:    Verifies user authorized to update data
 *
 * Author:    Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "global_defines.h"
#include "list.h"
#include "mask_server_defines.h"
#include "m2files.h"
#include "m2server_globals.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2req_types.h"
#include "m2server_arrays.h"
#include "m2state_file.h"
#include "rmt_server.h"
#include "rtn.h"
#include "m2msg.h"
#include "mti.h"
#include "strcpy.h"
#include "user_params.h"


/* Verify user authorized to access data */
void owner_check(void)
{
    int fnd = DBNO;
    char *usergroup_list[1000];
    char *user_list[1000];
    char file_name[64];
    int cnt;

    /* Allow mdsadmin to do all transactions */
    if (strcmp(user.email, "mdsadmin") == 0) return;
    if (strcmp(st.own_id, user.email) == 0) return;
    /* Otherwise search for user */
    /* first look for user in list of users */
    /* convert this list from string format to list format */
    if (strcmp(st.users, "None") != 0)
    {
	string2list_convert(st.users, user_list);
	/* Look for submitting user on the list */
	if (list_find(user_list, user.email) == SUCCESS) 
	{
	    files_free(user_list);
	    return;
	}
	else files_free(user_list);
    } /* End if for valid user names */
    /* Next search for user in user group list */
    if (strcmp(st.usergroup, "None") != 0)
    {
    /* Convert user group to list format */
	string2list_convert(st.usergroup, usergroup_list);
	for (cnt = 0; usergroup_list[cnt] != NULL; ++cnt)
	{
	/* build name for group */
	strcpy3(file_name, USER_DATA, "/", usergroup_list[cnt]);
	/* If file not present, skip it */
	if (access(file_name, 0) != 0) continue;
	list_read(file_name, user_list);
	if (list_find(user_list, user.email) == SUCCESS) 
	{
		fnd = DBYES;
		break;
	}
	/* Free list no longer needed */
	files_free(user_list);
	/* Free user group name no longer needed */
	} /* End for */
	files_free(usergroup_list);
    } /* End if for valid user groups present */
    if (fnd == DBYES) return;
    else
    {
	strcpy3(rtn.msg, m2own_err, RET2_TAB, mti.prefix);
	shut_down(rtn.msg, M2OWN_ERR);
    }
} /* end owner_check */
