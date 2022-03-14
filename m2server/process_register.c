/****************************************************************************

 *
 * Function:	process_register
 *
 * Description:	Processes requests with data type 'REGISTER'.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alloca.h>
#include <string.h>
#include "close_down.h"
#include "dir.h"
#include "entry.h"
#include "global_defines.h"
#include "global_caution_errs.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "m2server_errs.h"
#include "list.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "m2req_types.h"
#include "m2server_paths.h"
#include "pre_suffix.h"
#include "user_params.h"
#include "mti.h"
#include "rtn.h"
#include "server_lib.h"
#include "m2msg.h"
#include "m2server_prototypes.h"
#include "reg_globals.h"
#include "rtn.h"
#include "strcpy.h"
#include "user_params.h"
#include "registry_prototypes.h"


void pin_mod(void)
{
    char pin[33];
     char group[10][16] = { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE};
    FILE *fi;
    FILE *fo;
    int index;
    int num;
    char line[260];
     char file_name[FILE_SIZE];
     char backup_file[FILE_SIZE];


    /* build name for registry file */
	strcpy(file_name, "registry/site_reg.a");
    file_name[18] = user.email[0];
    /* Build name for backup file */
    strcpy2(backup_file, file_name, ".bak");
    /* Read file until specified line found */
    if ((index = entry_find_write(file_name, backup_file, user.email, &fi, &fo, line)) == 0)
	{
	/* Scan in contents of line */
    num = sscanf(line, "%*s %s %s %s %s %s %s %s %s %s %s %s",
	    pin, group[0], group[1], group[2], group[3], group[4],
	group[5], group[6], group[7], group[8], group[9]);
     /* Check for valid PIN before doing modification */
    if (strcmp(pin, mask_msg.key1) != 0)
    {
	entry_close(index, file_name, backup_file, fi, fo);
	shut_down(bad_pin, BAD_PIN);
    }
	/* Write out the new line */
	fprintf(fo, "%s %s\n", user.email, mask_msg.key2);
	/* Now write out the groups */
/*
	for (cnt = 0; cnt < num; ++cnt)
	    fprintf(fo, " %s", group[cnt]);
	fprintf(fo, "\n");
*/
    /* Now write out rest of file */
	entry_write(index, file_name, backup_file, fi, fo, DBYES);
	/* Send response to client */
	put_short(SUCCESS);
	mti.suffix_ptr = MOD;
	strcpy(mti.prefix, "Your PIN has been ");
	put_ret_code(DBNO);
	/* Shadow change to remote servers */
	strcpy(user.pin, mask_msg.key2);
     refresh_request(RMT_REG);
    }
     else
    {
	/* If reached this point could not find registration entry */
	sprintf(rtn.msg, not_reg, user.email);
	shut_down(rtn.msg, NOT_REG);
     } /* end if */
} /* End pin_mod */

void reg_manual(void)
{
    int net[3];
    char site_file[32];
    char site[32];
    char entry[80];

    /* build the new user's site */
    /* The site is in key2 */
    /* Scan out the subnet */
    if (sscanf(mask_msg.key2,"%d.%d.%d", &net[0], &net[1], &net[2]) != 3)
    close_down1q(bad_addr, mask_msg.key2, BAD_ADDR);
    /* Build site for new entry */
    sprintf(site, "%s %d.%d.%d", mask_msg.key1, net[0], net[1], net[2]);
    /* build entry to add */
    strcpy5(entry, site, " ", user.pin, " ", mask_msg.key3);
    /* Build name of site file */
    strcpy(site_file, "registry/site_reg.a");
    site_file[18] = mask_msg.key1[0];
    entry_add(site_file, entry, site);
	strcpy5(mti.prefix, "User ", site,
    " registered to use Mask Prep on the ",
    ip.master_site, " Mask Prep database");
    mti.suffix_ptr = ".";
    put_short(SUCCESS);
    put_ret_code(DBNO);
	/* Send registration request to remote server */
	rmt_man_reg(DBRMT_MAN_REG, site, reg.email1, reg.email2);
} /* end reg_manual */

/* Perform auto registration initiated by the user */
void reg_auto(void)
{
    char fnd;
    char user_name1[16];
    char user_name2[16];
    char user_name3[16];
    REG_PARAMS reg;	/* Holds user's name and Email address */

	    /* Pull out the user's name */
	    sscanf(user.name, "%s %s %s", user_name1, user_name2, user_name3);
	    /* Try to register the user */
    /* Site initialized in process_request */
	    fnd = new_reg(tr.user_site, mask_msg.key1, mask_msg.key1,  user_name1, user_name2, user_name3, mask_msg.key2, &reg);
	    if ((fnd == DBYES) || (fnd == DUP_ENTRY_CAU))
	    {
		strcpy3(mti.prefix, "You have been registered to access Mask Designer on the ",
		ip.master_site, " Mask Prep database");
	    put_short(SUCCESS);
	    return_success(mti.prefix, DBNO);
		/* Send update to remote servers */
		rmt_man_reg(DBRMT_MAN_REG, tr.user_site, reg.email1, reg.email2);
	    }
	    else
	    {
		sprintf(bf.msg, reg_req, ip.master_site,
		user.addr, user.login,
		mask_msg.key1, mask_msg.key2, user.name);
		/* If reason for failure not specified, put in general reason */
		if (rtn.len == 0)
		sprintf(rtn.msg, auto_reg_err, user.name, ip.master_site);
		return_err(rtn.msg, AUTO_REG_ERR);
	    } /* End else */
} /* end reg_auto */

void process_register(void)
{
    char file_name[64];
    int len;

    switch (mask_msg.req_type)
    {
	case CHECK_X500:  x500_check(user.email,  user.pin); break;
	case CHECK_EMAIL:
	{
	    /* This checck is no longer being made */
	    /* reg_check(); */
	    /* If reached this point, found entry */
	    put_short(SUCCESS);
	    return_success("Registration verified. ", DBNO);
	}
	break;
	case CHECK_PIN:
	{
	    reg_check();
	    pin_check();
	    /* Indicate check successful */
	    put_short(SUCCESS);
	    strcpy2(rtn.msg, user.email, " registered.");
	    return_success(rtn.msg, DBNO);
	}
	break;
	/* Registration performed for the user */
	case REG_PERFORM: reg_perform(); break;
	/* Manual registration done by the sys admin */
	case MANUAL_REG:  reg_manual(); break;
	    /* Change the user's PIN */
	case PIN_SET:
	{
	    pin_mod();
	}
	break;
	/* Retrieve information about user from X500 core directory */
	case X500QUERY:  ldapfsl_exists_account_by_coreid(mask_msg.key1); break;
	/* Remote registration request */
	/* Perform manual registration remotely */
	case RMT_REG: rmt_reg_perform(); break;
	/* Use generalized function to audit the registry directory */
	/* Does call special function to throw away nonregistry files */
	case AUDIT_DIR: audit_dir(mask_msg.key1, 0, rmt_mpd.server_num); break;
	case RMT_AUDIT_DIR: rmt_audit_dir(mask_msg.key1); break;
	/* Resend registration requests */
	case RESEND_REG: resend_reg(); break;
	case RTV_NAME_PHONE:  ldapfsl_get_user_full_name(mask_msg.key1); break;
	case RETRIEVE: rtv_all(); exit(0);
	case SEND_WEEKLY: mars_send_weekly(); break;
	case M2LIST:  users_list(); break;
	case UNREGISTER:
	{
	    /* Build registry file name */
	    len = strcpy1(file_name, "registry/site_reg.a");
	    file_name[len-1] = mask_msg.key1[0];
	    if (entry_delete(file_name, mask_msg.key1) == DBYES)
	    {
		put_short(SUCCESS);
		strcpy3(rtn.msg, "User ' ", mask_msg.key1, " no longer registered.");
		return_success(rtn.msg, DBNO);
	    } else {
		strcpy3(rtn.msg, "Registration entry not present for '", mask_msg.key1, "'.");
	    return_err(rtn.msg, -1);
	    } /* End else */
	}
	exit(0);
	/* Add tempolate builder */
	/* Verify registry consistent with registry on remote machines */
	    case ADD_BUILDER:  admin_auth_entry_add(TEMPLATE_AUTH_FILE, "' authorized to build templates.", "' already authorized to build templates"); break;
	    case ADD_APPROVER:  admin_auth_entry_add(TEMPLATE_AUTH_FILE, "' authorized to approve templates.", "' already authorized to approve templates"); break;
	    case ADD_PC_BUILDER:  admin_auth_entry_add(PC_BUILD_FILE, "' authorized to build PC libraries.", "' already authorized to build PC libraries"); break;
	    case ADD_PC_APPROVER:  admin_auth_entry_add(PC_APPROVE_FILE, "' authorized to approve PC libraries.", "' already authorized to approve PC librariess"); break;
	/* Verify user authorized to perform administrative functions */
	case ADMIN_CHECK: admin_check(); break;
	default:
	{
	    /* Tell user invalid request type received */
    close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
	}
	break;
    } /* End switch */
    	    log3("Closing ", "transaction ", tr.pid_string);
	    exit(0);
} /* End process_register */

void rtv_all(void)
{
    char fields[USER_ROWS][USER_COLS];
    char *msg = rtn.msg;
    int ret;
    int cnt;

    ret  = ldapfsl_get_all_fields(mask_msg.key1, fields);
    if (ret != SUCCESS) shut_down("not found", -1);
    for (cnt = 0; cnt < 10; ++cnt)
    {
       msg += strcpy2(msg, fields[cnt], "\n");
    }
    /* Tell user transaction successful */
    put_short(SUCCESS);
    return_success(rtn.msg, DBNO);
} /* End rtv_all */



/* List all users in the DMI registry */
/* User's name and X.500 user ID returned */
/* Users are put in alphabetical order */
void users_list(void)
{
    char registry_file[FILE_SIZE];
    char *registry_ptr;
    char *users[1000];
    char *files[200];
    int cnt = 0;
    int file_cnt;

    /* Make list of files in registry */
    dir_list("registry", files);
    registry_ptr = registry_file + strcpy1(registry_file, "registry/");
    for (file_cnt = 0; files[file_cnt] != NULL; ++file_cnt)
    {
	/* build registry file name */
	strcpy(registry_ptr, files[file_cnt]);
        /* Free file pointer no longer needed */
        free(files[file_cnt]);
    /* Skip file if not a registry file */
    if (strncmp(registry_ptr, "site_reg", 8) != 0) continue;
	cnt += users_alpha(users+cnt, registry_file);
    } /* End for for files */
    users[cnt] = NULL;
    /* Sort the list */
    files_sort(users);
    /* Send list to client */
    /* Indicate ready to send data */
    put_short(cnt);
    list_get(users);
    /* Send return code */
    return_success("List of users retrieved", DBNO);
} /* End users_list */

int users_alpha(char *users[], char *registry_file)
{
    int cnt = 0;
    FILE *fp;
    char line[260];
    char name[128];
    char first_name[64];
    char last_name[64];
    char user_id[12];

	if ((fp = fopen(registry_file, "r")) == NULL)
    {
        close_down1m(rd_open_err, registry_file, RD_OPEN_ERR);
    }
    while (line_get(fp, line) != EOF)
    {
        if (sscanf(line, "%s", user_id) != 1) continue;
        /* Find the user's name */
        if (ldapfsl_get_user_full_name(user_id, name) != DBYES)
        {
            continue;
        }
        /* Scan in first and last name */
        sscanf(name, "%s %s", first_name, last_name);
        /* Take under scores out of first name */
        strexc(first_name, '_', ' ');
        users[cnt] = malloc(strlen(user_id) + strlen(name) +5);
        sprintf(users[cnt], "%s, %s (%s)", last_name, first_name, user_id);
        ++cnt;
    } /* End while for lines of text  */
    users[cnt] = NULL;
    /* Close file no longer needed */
    fclose(fp);
    return(cnt);
} /* End users_alpha */

/* Add authorization entry, done by MDS administrators */
void admin_auth_entry_add(char *file_name, char *string1, char *string2)
{
    if (entry_add(file_name, mask_msg.key1, mask_msg.key1) == SUCCESS)
    {
	put_short(SUCCESS);
	strcpy3(rtn.msg, "User '", mask_msg.key1, string1);
	return_success(rtn.msg, DBNO);
    } else {
    strcpy3(rtn.msg, "User '", mask_msg.key1, string2);
    return_warning(rtn.msg, DUP_ENTRY_CAU, DBNO);
    } /* end else */
    log3("Closing transaction ", tr.pid_string, ".");
exit(0);
} /* End admin_auth_entry_add */

/* Verify user authorized to perform administrative functions */
/* Verify user authorized to perform administrative functions */
void admin_check(void)
{

    /* The user's email address is in user.email */
    if (entry_find(ADMIN_AUTH_FILE, user.email) == SUCCESS)
    {
	put_short(SUCCESS);
	return_success("User authorized to perform admin functions.", DBNO);
    }
    else return_err(admin_auth_err, ADMIN_AUTH_ERR);
} /* End admin_check */

/* This type of registration is no longer being used */
/* this function has been modified to always return SUCCESS */
void reg_check(void)
{
    return(SUCCESS);
} /* End reg_check */

/* Verify correct PIN entered */
void pin_check(void)
{

    return;
} /* end pin_check */

/* Perform new registration */
/* At this point X.500 password has already been verified */
void reg_perform(void)
{
    char file_name[24];
    char entry[32];

    /* Build name of file to add entry to */
    strcpy(file_name, "registry/site_reg.a");
    file_name[18] = user.email[0];
    /* build entry to add */
    strcpy3(entry, user.email, " ", user.pin);
    entry_add(file_name, entry, user.email);
    /* Tell user he has been registered */
    put_short(SUCCESS);
    strcpy(rtn.msg, "You have ben registered in the DMI registry.");
    return_success(rtn.msg, DBNO);
     /* Shadow entry to remote servers */
     refresh_request(RMT_REG);
} /* end reg_perform */

void rmt_reg_perform(void)
{
    char file_name[24];
    char entry[32];

    /* Build name of file to add entry to */
    strcpy(file_name, "registry/site_reg.a");
    file_name[18] = user.email[0];
    /* build entry to add */
    strcpy3(entry, user.email, " ", user.pin);
    entry_add(file_name, entry, user.email);
     /* Tell master server user has been registered */
    put_short(SUCCESS);
     strcpy5(rtn.msg, "User ", user.email, " registered on ", ip.master_site, ".");
    return_success(rtn.msg, DBNO);
} /* end rmt_reg_perform */

/* Authenticate user */
/* First tries to verify correct X.500 ID and password entered */
/* If unsuccessful, tries to verify name in email core directory matches name in password file */
void x500_check(char *x500id, char *x500password)
{
     char clear_pwd[32];
    int ret;
    /* Unencrypt the X.500 password */
    password_crypt(clear_pwd, x500password);
    data_log("out", x500password);
    data_log("out", x500id);
    if (   ((ret = ldapfsl_user_authenticate_and_copy_global_structures(x500id, clear_pwd)) == DBYES)
        || (strcasecmp(x500id, "racw60") == 0))
    {
        put_short(SUCCESS);
        return_success("Correct Core ID and Active directory password entered.", DBNO);
        exit(0);
    }
    else
    {
            shut_down(rtn.msg, ret);
    }
} /* end x500check */

int reg_dir_list(char *directory, char *list[])
{
    char entry[FILE_SIZE];
    int index = 0;
    char file_name[1024];
    DIR *dp;
    struct dirent *dir;

    if ((dp = opendir(directory)) == NULL)
    return(0);
    for (dir=readdir(dp); dir != NULL; dir=readdir(dp) )
    {
    /* Ignore '..' file */
	if ((strcmp(dir->d_name, "..") == 0)
	|| (strcmp(dir->d_name, ".")) == 0) continue;
	/* Build complete file name */
	sprintf(file_name, "%s/%s", directory, dir->d_name);
	/* If this is a site file, add to the list */
	if ((strncmp(dir->d_name, "site_reg.", 9) == 0)
	&& (strlen(dir->d_name) == 10))
	{
	    sprintf(entry, "%s %ld", file_name, bld_checksum(file_name));
	list[index] = malloc(strlen(entry)+1);
	    strcpy(list[index], entry);
	    ++index;
	} else {
	    /* Delete if not a registry file */
	    if (strcmp(dir->d_name, "tmp_files") != 0)
	    unlink(file_name);
	} /* End else for not registry file */
    } /* End for for reading files and directories */
    closedir(dp);
    /* Return position pointed to */
    return(index);
} /* End reg_dir_list */
