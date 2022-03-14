/****************************************************************************

 *
 * Function:	process_usergroup
 *
 * Description:	Processes requests to create, add to, 
 *		delete from and list user groups
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <string.h>
#include <sys/stat.h>
     #include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include "close_down.h"
#include "entry.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "global_server_params.h"
#include "global_params.h"
#include "list.h"
#include "m2defines.h"
#include "m2mask_files.h"
#include "m2server_errs.h"
#include "global_server_errs.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "m2req_types.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "pre_suffix.h"
#include "user_params.h"
#include "mti.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "m2msg.h"

/* GenericListFunction Type definition */
int userlist_add(char *list[], char *key, int total_count);

void process_usergroup(void)
{

    /* Set up data dir to be the user group file */
    strcpy3(mti.data_dir, USER_DATA, SLASH, mask_msg.key1);
    strcpy3(mti.delta_dir, USER_DELTA, SLASH, mask_msg.key1);
    /* Initialize partial message sent back to the user */
    strcpy3(mti.prefix, "User group '", mask_msg.key1, "'");
    strcpy3(mti.suffix, "user group '", mask_msg.key1, "'");
    switch (mask_msg.req_type)
    {
	case CREATE:
	{
	    int non_list;
    int present;
	    char users[1024];


    /* See if user group already present and if server owns the data */
	    present = usergroup_check_existence_owner();
    /* Tell client ready to retrieve list */
    put_short(SUCCESS);
	/* Get list of user groups */
	    get_string(users);
	    if (strcmp(users, NONE) == 0) 
	    non_list = DBYES;
	    else non_list = DBNO;
	    /* If user group present, do additional checks */
	    if (present == DBYES)
	    {
		/* If members present, modify existing members */
		if (non_list == DBNO)
		usergroup_mod(users);
		else usergroup_delete();
	    } else {
		/* If group not present and list not empty create */
		if (non_list == DBNO)
		usergroup_create(users);
		else shut_down(no_users, NO_USERS);
	    } /* End else */
	}
	break;
	case DELETE:  usergroup_delete_all(); break;
	/* List users assigned to the user group */
	case LIST_MEMBERS:  usergroup_list_users(); break;
	/* List masks and templates associated with the user group */
	case M2LIST:  usergroup_list_masks(); break;
	/* List all user groups present */
	case LISTALL: usergroup_listall(); break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_usergroup */

/* Create a new usergroup */
void usergroup_create(char *users)
{
    int len;

    /* The requesting user is automaticly added to the list */
    /* Exchange commas for carriage returns */
    strexc(users, ',', '\n') -1;
    /* Make sure owner is on the list */
    usergroup_owner_add(users);
    /* Chop off trailing carriage return */
    /* It is automaticly added when user's name is logged */
    len = strlen(users)-1;
    if ((len >= 0) &&  (users[len] == '\n')) users[len] = '\0';
    data_log(mti.data_dir, users);
    /* Also log master database in delta directory */
    data_log(mti.delta_dir, ip.master_site);
    /* Tell user transaction successful */
    mti.suffix_ptr = CRT;
    put_ret_code(DBNO);
} /* End usergroup_create */

/* Modify an existing user group */
void usergroup_mod(char *users)
{
    int len;

    /* verify that this server owns the data */
    usergroup_check_owner();
    /* Verify this user can access the data */
    if (entry_find(mti.data_dir, user.email) != SUCCESS)
    {
	strcpy3(rtn.msg, add_err, RET2_TAB, mti.prefix);
	return_err(rtn.msg, DBNO);
    }
    /* Exchange commas for carriage returns in list */
    strexc(users, ',', '\n') -1;
    /* Make sure requester is still on the list */
    usergroup_owner_add(users);
    /* chop off trailing carriage return */
    /* It is added when data is logged */
    len = strlen(users)-1;
    if ((len >= 0) &&  (users[len] == '\n')) users[len] = '\0';
    /* Replace existing list with new list */
    unlink(mti.data_dir);
    data_log(mti.data_dir, users);
    strcpy2(rtn.msg, "Requested names added to ", mti.suffix);
    return_success(rtn.msg, DBNO);
} /* End usergroup_mod */

/* Called when user trying to complete user group and all members */
void usergroup_delete_all(void)
{

    /* Verify that this server owns the data */
    usergroup_check_owner();
    /* Verify user authorized to perform this transaction */
    if (entry_find(mti.data_dir, user.email) != SUCCESS)
    {
	strcpy3(rtn.msg, delete_auth_err, RET2_TAB, mti.prefix);
	return_err(rtn.msg, DBNO);
    } /* end if */
    /* If override not set to DBYES, send override message */
    if (mask_msg.override_flag != DBYES)
    usergroup_override_send(dlt4ovr);
    /* Delete the delta file */
    unlink(mti.delta_dir);
    unlink(mti.data_dir);
    /* Delete referenced to associates masks and the mask file */
    usergroup_masks_delete(mask_msg.key1);
    /* Delete associated templates */
    usergroup_templates_delete(mask_msg.key1);
    /* Tell user transaction successful */
    mti.suffix_ptr = DLT;
    put_short(SUCCESS);
    put_ret_code(DBNO);
} /* End usergroup_delete_all */

/* Delete specified users from group */
void usergroup_delete(void)
{

    /* verify that this server owns the data */
    usergroup_check_owner();
    /* Verify user authorized to perform this transaction */
    if (entry_find(mti.data_dir, user.email) != SUCCESS)
    {
	strcpy3(rtn.msg, delete_auth_err, RET2_TAB, mti.prefix);
	return_err(rtn.msg, DBNO);
    } /* end if */
    /* If override not set to DBYES, send override message */
    if (mask_msg.override_flag != DBYES)
    usergroup_override_send(dlt3ovr);
    /* Delete the delta file */
    unlink(mti.delta_dir);
    unlink(mti.data_dir);
    /* Delete referenced to associates masks and the mask file */
    usergroup_masks_delete(mask_msg.key1);
    /* Tell user transaction successful */
    /* Delete associated templates */
    usergroup_templates_delete(mask_msg.key1);
    mti.suffix_ptr = DLT;
    put_ret_code(DBNO);
} /* End usergroup_delete */

/* Send user list of users in user group */
void usergroup_list_users(void)
{
    char *names[1024];
    int cnt;

    /* Verify the user group is present */
    if (access(mti.delta_dir, 0) != 0)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Read list of names from file */
    cnt = users_alpha(names, mti.data_dir);
    /* Tell client ready to send names */
    put_short(cnt);
    list_get(names);
    /* Send return code */
    return_success("names retrieved.", DBNO);
} /* End usergroup_list */

/* Send list of all user groups present */
void usergroup_listall(void)
{
    char *names[200];

    /* Make list of files present in the directory */
int cnt =     mask_files_list(USER_DATA, names);
    /* Tell user ready to send list */
    put_short(cnt);
    /* Send list to user */
    list_get(names);
    /* Tell user list retrieved */
    return_success("List of user groups retrieved.", DBNO);
} /* End usergroup_listall */

/* Verify user entered correct PIN */
/* and that this server owns the data */
void usergroup_check_owner(void)
{
    FILE *fp;
    char master_site[24];

    /* Open file where master server stored */
    if ((fp = fopen(mti.delta_dir, "r")) == NULL)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Read in the master server */
    if (fscanf(fp, "%s", master_site) != 1)
    close_down1m(read_err, mti.delta_dir, READ_ERR);
    if (strcmp(ip.master_site, master_site) != 0)
    m2server_send(master_site);
} /* end usergroup_check_owner */

/* Verify user group is present and that this server owns the user group */
int usergroup_check_existence_owner(void)
{
    FILE *fp;
    char master_site[24];

    /* Open file where master server stored */
    if ((fp = fopen(mti.delta_dir, "r")) == NULL)
    return(DBNO);
    /* Read in the master server */
    if (fscanf(fp, "%s", master_site) != 1)
    close_down1m(read_err, mti.delta_dir, READ_ERR);
    if (strcmp(ip.master_site, master_site) != 0)
    m2server_send(master_site);
    else return (DBYES);
} /* end usergroup_check_existence_owner */

/* Get the new names and add to the list */
int usergroup_list_mod(char *names, int total_cnt, glf_t list_function)
{
    int len;
    int cnt;
    int cnt1 = 0;
    char string[260];
    char name[32];

    /* Tell user ready to get names */
    put_short(SUCCESS);
    get_string(string);
    if (strcmp(string, NONE) != 0)
    {
	for (;; ++cnt1)
	{
	    cnt = cnt1;
	    while ((string[cnt1] != ',') && (string[cnt1] != '\0')) ++cnt1;
	    len = cnt1-cnt;
	    strn_cpy(name, string+cnt, len);
	    total_cnt += (*list_function)(names,  name, total_cnt);
	    /* Fall out of loop if '\0' found */
	    if (string[cnt1] == '\0') break;
	} /* end for */
    } /* end if for valid data passed in */
    return(total_cnt);
} /* End usergroup_list_mod */

/* List masks and templates assigned to the user group */
 void usergroup_list_masks(void)
{
    char *mask_ptr;
    char **template_ptr;
    char *combined_ptr;
    int mask_len;
    int template_len;
    char file_name[FILE_SIZE];

    /* Verify the user group is present */
    if (access(mti.delta_dir, 0) != 0)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* build masks file name */
    strcpy2(file_name, mti.delta_dir, ".masks");
    mask_len = members_list(file_name, &mask_ptr);
    /* build template file name */
    strcpy2(file_name, mti.delta_dir, ".templates");
    template_len = members_list(file_name, &template_ptr);
    combined_ptr = malloc(mask_len + template_len +30);
    strcpy4(combined_ptr, "Masks:", mask_ptr, "\n\nTemplates:", template_ptr);
    /* Indicate ready to send data */
    put_short(SUCCESS);
    put_string(combined_ptr);
    return_success("List of masks and templates retrieved", DBNO);
    free(combined_ptr);
    free(mask_ptr);
    free(template_ptr);
} /* End */

int members_list(char *file_name, char **ptr)
{
    FILE *fp;
    struct stat buf;
    int line_len = 0;
    int len;
    char line[260];
    char *local_ptr;

    /* See if any masks associated with the user group */
    if ((stat(file_name, &buf) != 0) || (buf.st_size == 0))
    {
	*ptr = malloc(10);
	strcpy(*ptr, "  None");
	return(strlen(*ptr));
    }
    /* At this point we are ready to read in data from the file */
    local_ptr = *ptr = malloc((buf.st_size)*2);
    if ((fp = fopen(file_name, "r")) == NULL)
    close_down1m(rd_open_err, file_name, RD_OPEN_ERR);
    while((len = line_get(fp, line)) != EOF)
    {
	/* copy line into buffer */
	local_ptr += strcpy2(local_ptr, line, ", ");
	line_len += len+2;
	if (line_len > 40)
	{
	    local_ptr += strcpy1(local_ptr, "\n");
	line_len = 0;
	} /* End for end of line reached */
    } /* End while */
    /* chop off trailing comma and space */
    if (line_len == 0) local_ptr -= 3;
    else local_ptr -= 2;
    local_ptr[0] = '\0';
    /* Add carriage return if not just put on */
    /* Close file */
    fclose(fp);
    return(strlen(*ptr));
} /* End members_list */

/* Count number of mask sets or templates accessing the user group */
int members_count(char *file_name)
{
    FILE *fp;
    int cnt = 0;
    char line[260];

    if ((fp = fopen(file_name, "r")) == NULL)
    return(0);
    while (line_get(fp, line) != EOF) ++cnt;
    fclose(fp);
    return(cnt);
} /* End members_count */

/* Send usergroup override message to user */
void usergroup_override_send(char *override_msg)
{
    char file_name[FILE_SIZE];
    int mask_num;
    int template_num;

    /* Check on mask and template access */
    strcpy2(file_name, mti.delta_dir, ".masks");
    mask_num = members_count(file_name);
    strcpy2(file_name, mti.delta_dir, ".templates");
    template_num = members_count(file_name);
    sprintf(rtn.msg, override_msg, mti.prefix, mask_num, template_num);
    return_override(rtn.msg, DBNO);
} /* End usergroup_override_send */

/* Delete references to user group from all associated mask sets */
void usergroup_masks_delete(char *usergroup)
{
    char *list[10];
    int total_cnt;
    int index;
    FILE *fp;
    char usergroup_file[FILE_SIZE];
    char mask[8];

    /* Build name for mask file */
    strcpy4(usergroup_file, USER_DELTA, "/", usergroup, ".masks");
    if ((fp = fopen(usergroup_file, "r")) == NULL) return;
    while (fscanf(fp, "%s", mask) != EOF)
    {
	/* Build state file name */
	strcpy4(mti.state_file, MASK_DELTA, "/", mask, "/state_file");
	/* If state file not present, skip it */
	if (access(mti.state_file, 0) != 0) continue;
	/* Open state file for update */
	index = state_file_read4update();
	/* Change user group name */
	total_cnt = string2list_convert(st.usergroup, list);
	if (m2list_delete(list, usergroup, total_cnt) == 0)
	/* If empty list found, set usergroup to None */
	strcpy(st.usergroup, "None");
	/* Convert back to string format */
	else list2string_convert(list, st.usergroup);
	/* Write out the state file */
	state_file_write(index);
    }
    /* Close the masks file and delete it */
    fclose(fp);
    unlink(usergroup_file);
} /* End usergroup_masks_delete */

void usergroup_templates_delete(char *usergroup)
{
    char *list[10];
    int total_cnt;
    int index;
    FILE *fp;
    char usergroup_file[FILE_SIZE];
    char line[260];
    char fab[32];
    char tech[64];

    /* Build name for template file */
    strcpy4(usergroup_file, USER_DELTA, "/", usergroup, ".templates");
    if ((fp = fopen(usergroup_file, "r")) == NULL) return;
    while (line_get(fp, line) != EOF)
    {
	/* Scan in wafer line and flow */
	sscanf(line, "%s %s", fab, tech);
	/* Build state file name */
	strcpy6(mti.state_file, FAB_DELTA, "/", fab, "/", tech, "/state_file");
	/* If state file not present, skip it */
	if (access(mti.state_file, 0) != 0) continue;
	/* Open state file for update */
	index = state_file_read4update();
	/* Change user group name */
	total_cnt = string2list_convert(st.usergroup, list);
	if (m2list_delete(list, usergroup, total_cnt) == 0)
	/* If empty list found, set usergroup to None */
	strcpy(st.usergroup, "None");
	/* Convert back to string format */
	else list2string_convert(list, st.usergroup);
	/* Write out the state file */
	state_file_write(index);
    }
    /* Close the masks file and delete it */
    fclose(fp);
    unlink(usergroup_file);
} /* End usergroup_templates_delete */

/* Determine if user has access to this data */
int user_list_check(char *users)
{
    int cnt;
    int num;
    char tmp_user[20][20];

    num = sscanf(users, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
     tmp_user[0 ], tmp_user[1 ], tmp_user[2 ], tmp_user[3 ], tmp_user[4 ], tmp_user[5 ], tmp_user[6 ], tmp_user[7 ], tmp_user[8 ], tmp_user[9 ], tmp_user[10 ], tmp_user[11 ], tmp_user[12 ], tmp_user[13 ], tmp_user[14 ], tmp_user[15 ], tmp_user[16 ], tmp_user[17 ], tmp_user[18 ], tmp_user[19]);
    for (cnt = 0; cnt < num; ++cnt)
    if (strcmp(tmp_user[cnt], user.email) == 0) return(SUCCESS);
    /* If reached this point, user not found */
    return(FAILURE);
} /* End user_list_check */

/* Add owning user to user list if not already present on the list */
/* At this point members of the list are carriage return separated */
int usergroup_owner_add(char *users)
{
    int offset = 0;
    char tmp_user[16];
    while (sscanf(users+offset, "%s", tmp_user) > 0)
    {
	if (strcmp(tmp_user, user.email) == 0) return(FAILURE);
	else offset += strlen(tmp_user)+1;
    } /* End while */
    /* If reach this point, user not found */
    strcpy(users+offset, user.email);
}

/* Add the user's name before adding to the list */
int userlist_add(char *list[], char *key, int total_cnt)
{
    char entry[80];
    char dpt[8];
    char user_id[10];
    char name[32];

    /* Find out information about the user */
    requester_whobu(key, user_id, dpt, name);
    /* Copy information into entry */
    strcpy4(entry, key, " (", name, ")");
    return(m2list_add(list, entry, key, total_cnt));
} /* End user_list_add */
