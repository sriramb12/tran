/****************************************************************************
 *
 * Function:	mask_entry_add
 *
 * Description:	Functions to access the create file
 *		Deletes entry from mask set table when mask set deleted
 *		Modifies mask set table when mask set transferred, archived, etc.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "close_down.h"
#include "create.h"
#include "global_defines.h"
#include "m2req_types.h"
#include "mask_entry.h"
#include "mask_server_defines.h"
#include "m2server_arrays.h"
#include "m2server_globals.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "m2server_paths.h"
#include "m2state_table.h"
#include "mti.h"
#include "rtn.h"
#include "m2msg.h"
#include "strcpy.h"
#include "user_params.h"

long create_time;

/* Read data from specified create file */
void create_read(char *create_file)
{
    FILE *fp;

    /* Open the create file */
    if ((fp = fopen(create_file, "r")) == NULL)
    close_down1m(rd_open_err, create_file, RD_OPEN_ERR);
    /* Read data from the file */
   if (fscanf(fp, "%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%lu",
mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4, mask_msg.par5, &create_time) != 6)
    close_down1m(read_err, create_file, READ_ERR);
    /* Close the file */
    fclose(fp);
} /* End create_read */

void mask_create_write(char *create_file)
{
    FILE *fp;

    /* Open the create file */
    if ((fp = fopen(create_file, "w")) == NULL)
    close_down1m(wr_open_err, create_file, WR_OPEN_ERR);
    /* Write data to the file */
   fprintf(fp, "%s;%s;%s;%s;%s;%ld\n",
mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4,
    user.email, tr.upd_time);
    /* Close the file */
    fclose(fp);
} /* End mask_create_write */

void template_create_write(char *create_file)
{
    FILE *fp;

    /* Open the create file */
    if ((fp = fopen(create_file, "w")) == NULL)
    close_down1m(wr_open_err, create_file, WR_OPEN_ERR);
    /* Write data to the file */
   fprintf(fp, "%s;%s;%s;%s;%s;%ld\n",
mask_msg.key1, mask_msg.key2, "all", "none",
    user.email, tr.upd_time);
    /* Close the file */
    fclose(fp);
} /* End template_create_write */

void fab_create_write(char *create_file, char *fab, char *tech)
{
    FILE *fp;

    /* Open the create file */
    if ((fp = fopen(create_file, "w")) == NULL)
    close_down1m(wr_open_err, create_file, WR_OPEN_ERR);
    /* Write data to the file */
   fprintf(fp, "%s;%s;%s;%s;%s;%ld\n",
fab, tech, "all", "none",
    user.email, tr.upd_time);
    /* Close the file */
    fclose(fp);
} /* End fab_create_write */

void create_info_read(void)
{
    char string[FILE_SIZE];
    char create_name[24];
    char own_name[24];
    char user_name[24];
    char *rtn_ptr = rtn.msg;
    char create_file[FILE_SIZE];
    int len;

    /* Build name for create file */
    if (mask_msg.data_type == PC)
    strcpy2(create_file, mti.data_dir, "/create");
    else strcpy2(create_file, mti.data_dir, "/.db/create");
    create_read(create_file);
    /* Now retrieve user's name */
    ldapfsl_get_user_full_name(mask_msg.par5,create_name);
    /* Now retrieve owner's name */
    if (strcmp(mask_msg.par5, st.own_id) == 0)
    strcpy(own_name, create_name);
    else ldapfsl_get_user_full_name(st.own_id, own_name);
    /* Get name of last user */
    if (strcmp(mask_msg.par5, st.user_id) == 0)
    strcpy(user_name, create_name);
    else if (strcmp(st.own_id, st.user_id) == 0)
    strcpy(user_name, own_name);
    else ldapfsl_get_user_full_name(st.user_id, user_name);
    /* Now build message to send */
    rtn_ptr += strcpy2(rtn_ptr, "Created:  ", ctime(&create_time));
    rtn_ptr += strcpy5(rtn_ptr, "Creator:  ",  create_name, " (", mask_msg.par5, ")\n");
    rtn_ptr += strcpy5(rtn_ptr, "Owner:    ", own_name, " (", st.own_id, ")\n");
    printf("rtn.msg is %s\n", rtn.msg);
    /* Convert user groups to comma separated list */
    printf("st.usergroup is %s\n", st.usergroup);
    len = strexc2(string, st.usergroup, ' ', ", ") -2;
    /* Chop off trailing comma */
    printf("string is %s\n", string);
    if (string[len] == ',') string[len] = '\0';
    rtn_ptr += strcpy3(rtn_ptr, "User group access:  ", string, "\n");
    /* Convert users to comma separated list */
    printf("st.users is %s\n", st.users);
    len = strexc2(string, st.users, ' ', ", ") -2;
    printf("string is %s\n", string);
    /* Chop off trailing commas */
    if (string[len] == ',') string[len] = '\0';
    printf("now string is %s\n", string);
    rtn_ptr += strcpy3(rtn_ptr, "User access:  ", string, "\n");
    printf("at 1\n");
    rtn_ptr += strcpy3(rtn_ptr, "Owning Site:  ", st.master_site, "\n");
    rtn_ptr += strcpy5(rtn_ptr, "Last update by:  ", user_name, "(", st.user_id, ")\n");
    printf("at 2\n");
    rtn_ptr += strcpy2(rtn_ptr, "Last update on:  ", ctime(&st.upd_time));
    rtn_ptr += strcpy3(rtn_ptr, "Current state:  ", state_array[st.state], "\n");
    printf("at 3\n");
    /* If sending template data, include approved version */
    printf("checking for template\n");
    if (mask_msg.data_type == TEMPLATE)
    {
    if (strcmp(st.version_type, "-") == 0)
    rtn_ptr += strcpy1(rtn_ptr, "Latest approved version:  None\n");
    else rtn_ptr += sprintf(rtn_ptr, "Latest approved version:  %2.3f\n", st.app_version);
    }
    rtn_ptr += sprintf(rtn_ptr, "Latest version %2.3f\n", st.vnum);
    /* do following for PC data */
  if (mask_msg.data_type == PC)
    {
    rtn_ptr += sprintf(rtn_ptr, "Current approved version:  %2.3f\n", st.app_version);
	/* Get approver's name */
	ldapfsl_get_user_full_name(st.approver, user_name);
	strcpy5(rtn_ptr, "Approver:  ", user_name,
	"(", st.approver, ")\n");
    } else {
    /* Print out the archive status */
    if (st.state != ARCHIVED)
    rtn_ptr += strcpy1(rtn_ptr, "Archive status:  Not archived\n\n");
    else
    {
    	rtn_ptr += strcpy1(rtn_ptr, "Archive status:  Archived\n");
	rtn_ptr += strcpy2(rtn_ptr, "Archived on:  ", ctime(&st.upd_time));
    }
    rtn_ptr += strcpy3(rtn_ptr, "Parent Fab:  ", mask_msg.par1, "\n");
    rtn_ptr += strcpy3(rtn_ptr, "Parent technology:  ", mask_msg.par2, "\n");
    rtn_ptr += strcpy3(rtn_ptr, "Parent process flow:  ", mask_msg.par3, "\n");
    rtn_ptr += strcpy3(rtn_ptr, "Parent design group:  ", mask_msg.par4, "\n");
  } /* End else for not PC data */
} /* End create_info_read */

/* Reads version information from create file */
void create_version_read(char *user_id, char *update_time)
{
    FILE *fp;
    char line[FILE_SIZE];
    char create_file[FILE_SIZE];
    char *token_ptr;
    char *current_token_ptr;

    /* Build name for create file */
    strcpy2(create_file, mti.delta_dir, "/.db/create");
    /* Open the file for read */
    if ((fp = fopen(create_file, "r")) == NULL)
    {
	strcpy(user_id, st.user_id);
	strcpy(update_time, "unknown");
	return;
    }
    /* Now retrieve needed data */
    line_get(fp, line);
    fclose(fp);
    /* Now extract the needed info */
    strtok_r(line, " ", &token_ptr);
    strtok_r(token_ptr, " ", &token_ptr);
current_token_ptr =     strtok_r(token_ptr, " ", &token_ptr);
    strcpy(user_id, current_token_ptr);
    strtok_r(token_ptr, " ", &token_ptr);
    strtok_r(token_ptr, " ", &token_ptr);
    strcpy(update_time, token_ptr);
} /* end create_version_read */
