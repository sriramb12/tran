/****************************************************************************
 *
 * Function:	process_cap_wafer
 *
 * Description:	Processes requests with data type of MASK *
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include <wait.h>
#include "create.h"
#include "dir.h"
#include "close_down.h"
#include "dir.h"
#include "entry.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "list.h"
#include "mask_server_defines.h"
#include "m2audit_prototypes.h"
#include "m2files.h"
#include "m2globals.h"
#include "m2server_arrays.h"
#include "m2server_globals.h"
#include "m2server_paths.h"
#include "m2server_prototypes.h"
#include "m2req_types.h"
#include "server_lib.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2server_errs.h"
#include "m2state_table.h"
#include "mti.h"
#include "pre_suffix.h"
#include "user_params.h"
#include "rmt_server.h"
#include "rtn.h"
#include "strcpy.h"
#include "m2msg.h"


/* Read a cap wafer file already present with the primary mask set */
void cap_wafer_file_read(void)
{
    char primary_mask[8];
    char file_name[FILE_SIZE];

    /* First map the cap wafer to the original mask set */
    cap_wafer_map(primary_mask, mask_msg.key1);
    /* Build the prefix for later use */
    strcpy3(mti.prefix, "Cap wafer file for '", mask_msg.key1, "'");
    /* Make sure the cap wafer file is present */
    strcpy4(file_name, MASK_DATA, "/", primary_mask, "/.db/rb_cap_wafer");
    if (access(file_name, 0) != 0)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Tell client ready to transfer data */
    put_short(SUCCESS);
    /* Transfer the file */
    file_get_alternate(".", file_name, "rb_cap_wafer");
    mti.suffix_ptr = RTV;
    /* Indicate transaction was successful */
    put_ret_code(DBNO);
} /* End cap_wafer_file_read */


/* Map the cap wafer to the primary mask set name */
void cap_wafer_map(char *primary_mask, char *cap_wafer)
{
    int ret;
    FILE *fp;
    char file_name[32];

    /* First make sure the cap wafer file is present */
    strcpy2(file_name, "cap_wafer/", cap_wafer);
    if ((fp = fopen(file_name, "r")) == NULL)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Read in the primary mask */
    ret = fscanf(fp, "%s", primary_mask);
    fclose(fp);
    if (ret < 1)
    close_down1f(non_err, mti.suffix, NON_ERR);
} /* end cap_wafer_map */

void cap_wafer_mask_name_add(void)
{
    char file_name[32];

    /* Make sure the associated mask set does not exist */
    strcpy3(file_name, "mask_dlta/", mask_msg.key1, "/state_file");
    if (access(file_name, 0) == 0)
    close_down1f(cap_wafer_create_err, mask_msg.key1, CAP_WAFER_CREATE_ERR);
    /* Verify that the mask being added exists */
    strcpy3(file_name, "mask_dlta/", mask_msg.key2, "/state_file");
    if (access(file_name, 0) != 0)
    {
	strcpy3(mti.prefix, "Mask set '", mask_msg.key2, "'");
    close_down1f(non_err, mti.prefix, NON_ERR);
    }
    /* Build the name for the cap wafer file */
    strcpy2(file_name, "cap_wafer/", mask_msg.key1);
    /* Verify that it is present */
    if (access(file_name, 0) != 0)
    data_log(file_name, mask_msg.key2);
    /* Otherwise see if entry already present */
    else if (entry_find( file_name, mask_msg.key2) != SUCCESS)
    /* Add the new name */
    data_log(file_name, mask_msg.key2);
    sprintf(rtn.msg, "Mask set '%s' added to cap wafer '%s'", mask_msg.key2, mask_msg.key1);
    /* Let user know name was added */
    put_short(SUCCESS);
    /* Make the mask set the primary key for logging purposes */
    strcpy(mask_msg.key1, mask_msg.key2);
    return_success(rtn.msg, DBYES);
} /* End cap_wafer_mask_name_add */

/* Read list of mask sets associated with the cap wafer */
void cap_wafer_masks_list(void)
{
    char *list[100];
    int cnt;

    char file_name[32];

    strcpy2(file_name, "cap_wafer/", mask_msg.key1);
    if (access(file_name, 0) != 0)
    close_down1f(non_err, mti.prefix, NON_ERR);
    /* Read in the list of masks */
    cnt = list_read(file_name, list);
    /* Tell client list will be sent */
    put_short(cnt);
    list_get(list);
    /* Tell client transaction was successful */
    strcpy3(rtn.msg, "List of masks for ", mti.suffix, RTV);
    return_success(rtn.msg, DBNO);
} /* end cap_wafer_mask_list_rtv */

void which_cap_wafer(void)
{
    FILE *po;
    char *ptr;
    char command[80];
    char cap_wafer[32];

    /* first make sure the mask set exists */
    strcpy3(cap_wafer, "mask_dlta/", mask_msg.key1, "/state_file");
    if (access(cap_wafer, 0) != 0)
    {
	strcpy3(mti.prefix, "Mask set '", mask_msg.key1, "'");
	close_down1f(non_err, mti.prefix, NON_ERR);
    }
    /* Now make sure it is not a cap wafer */
    strcpy2(cap_wafer, "cap_wafer/", mask_msg.key1);
    if (access(cap_wafer, 0) == 0)
    close_down1f(cap_wafer_err, mask_msg.key1, CAP_WAFER_ERR);
    /* build command to find the cap wafer */
    strcpy3(command, "grep -l ", mask_msg.key1, " cap_wafer/*");
    if ((po = popen(command, "r")) == NULL)
    {
	sprintf(rtn.msg, cap_wafer_not_fnd, mask_msg.key1);
	shut_down(rtn.msg, CAP_WAFER_NOT_FND);
    } /* end if */
    if (fscanf(po, "%s", cap_wafer) == 1)
    {
	put_short(SUCCESS);
	if ((ptr = strchr(cap_wafer, '/')) != NULL)
	++ptr;
	else ptr = cap_wafer;
	return_success(ptr, DBNO);
    } else {
	sprintf(rtn.msg, cap_wafer_not_fnd, mask_msg.key1);
	shut_down(rtn.msg, CAP_WAFER_NOT_FND);
    } /* end else */
} /* End which_cap_wafer */

/* Send list of all defined cap wafers */
void all_cap_wafers_list(void)
{
    int cnt;
    char *cap_wafers[100];

    /* Make a list of the files in the cap wafer directory */
    /* These are the defined cap wafers */
    cnt = dir_list("cap_wafer", cap_wafers);
    /* Tell client list will be sent */
    put_short(cnt);
    list_get(cap_wafers);
    /* Tell client transaction was successful */
    strcpy(rtn.msg, "List of cap wafers retrieved.");
    return_success(rtn.msg, DBNO);
} /* all_cap_wafers_list */

/* Process cap wafer requests */
void process_cap_wafer(void)
{
    /* Make sure user is retrieving data from WROC site */
    if (strcmp(ip.master_site, "wacc") != 0)
    m2server_send("wacc");
	    strcpy(key, mask_msg.key1);
    /* Initialize paths needed for following transactions */
    strcpy3(mti.prefix, "Cap wafer '", mask_msg.key1, "'");
    strcpy3(mti.suffix, "Data for cap wafer '", mask_msg.key1, "'");
    switch(mask_msg.req_type)
{
	case READ: cap_wafer_file_read(); break;
	case READ_ANY: cap_wafer_file_read(); break;
	case ADD: cap_wafer_mask_name_add(); break;
	case M2LIST: cap_wafer_masks_list(); break;
	case RETRIEVE: which_cap_wafer(); break;
	case LISTALL: all_cap_wafers_list(); break;
	default:  close_down1d(bad_req_type, mask_msg.req_type, BAD_REQ_TYPE);
    } /* End switch */
} /* End process_cap_wafer */
