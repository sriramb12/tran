/****************************************************************************
 *
 * Function:	get_action
 *
 * Description:	Determines if the requested transaction can be performed
 *		based on the request type and state of the data.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include "global_defines.h"
#include <strings.h>
#include <unistd.h>
#include "close_down.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_errs.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "m2server_prototypes.h"
#include "m2req_types.h"
#include "m2server_errs.h"
#include "m2state_table.h"
#include "m2state_file.h"
#include "user_params.h"
#include "mti.h"
#include "rtn.h"
#include "server_lib.h"
#include "m2msg.h"
#include "strcpy.h"
#include "server_params.h"


/* Decide action to perform */
int get_action(void)
{
    short total_state;
    short client_state;
    short row_index;
    char wait_cnt = 0;
     char name[32];
    char phone[20];

    /* calculate the row offset */
    if (strcmp(user.email, "mdsadmin") == 0)
     client_state = ALL;
    else if (strcmp(st.user_id, user.email) == 0)
    {
	if (mask_msg.override_flag == DBYES) client_state = ALL;
	else client_state = USER_ONLY;
	} /* End if */
    else
    {
	if (mask_msg.override_flag == DBYES) client_state = OVERWRITE_ONLY;
	else client_state = NEITHER;
    } /* End else */
    /* Calculate the row index */
row_index = client_state*ROW_OFFSET+mask_msg.req_type;
    /* Make sure the row index is within range */
    if ((row_index < MIN_ROW_INDEX) || (row_index > MAX_ROW_INDEX))
    close_down1d(bad_row_index, row_index, BAD_ROW_INDEX);
    /* Make sure the column index is within range */
    if ((st.state < MIN_COL_INDEX) || (st.state > MAX_COL_INDEX))
    close_down1d(bad_col_index, st.state, BAD_COL_INDEX);
    total_state = state_table[row_index][st.state];
    /* First check for turned off mask set */
    turned_off_mask_checkout(st.state);
    /* If valid state simply return */
    if (total_state == SUCCESS)  return(0);
    /* If deleting data and overwrite occurs, return override message */
    if ((total_state == DLT2OVR) && (mask_msg.req_type == DELETE))
	{
	    sprintf(rtn.msg, dlt2ovr, mti.prefix);
	    return_override(rtn.msg, DLT2OVR);
    }
    /* Otherwise see which warning message being processed */
    switch (total_state)
    {
	/* Errors requiring prefix to be printed */
	/* Overwriting a previous check out */
	case RDO2CHO:
	{
	    sprintf(rtn.msg, rdo2cho, mti.prefix, st.cwd);
	    return_override(rtn.msg, OVR2CHO);
	}
	break;
	case CAN1OVR:
	{
	    sprintf(rtn.msg, can1ovr, mti.prefix);
	    return_override(rtn.msg, CAN1OVR);
	}
	break;
	case DLT1OVR:
	{
	    sprintf(rtn.msg, dlt1ovr, mti.prefix);
	    return_override(rtn.msg, DLT2OVR);
	}
	break;
	/* Errors requiring prefix to be printed */
	case NCI1ERR: close_down1f(nci1err, mti.prefix, NCI1ERR); break;
	case NCB1ERR: close_down1f(ncb1err, mti.prefix, NCB1ERR); break;
	case NCA1ERR: close_down1f(nca1err, mti.prefix, NCA1ERR); break;
	case RMT2UPD: close_down1f(rmt2upd, mti.prefix, RMT2UPD); break;
	case CHO1UPD: close_down1f(cho1upd, mti.prefix, CHO1UPD); break;
	case OVR2UPD: close_down1f(ovr2upd, mti.prefix, OVR2UPD); break;
	case DEL2UPD: close_down1f(del2upd, mti.prefix, DEL2UPD); break;
	case RD2UPD: close_down1f(rd2upd, mti.prefix, RD2UPD); break;
	case CIN2UPD: close_down1f(cin2upd, mti.prefix, CIN2UPD); break;
	case CAN2UPD: close_down1f(can2upd, mti.prefix, CAN2UPD); break;
	case DLT2OVR: close_down1f(dlt2ovr, mti.prefix, DLT2OVR); break;
	case ALR1ARC: close_down1f(alr1arc, mti.prefix, ALR1ARC); break;
	case NOT1ARC: close_down1f(not1arc, mti.prefix, NOT1ARC); break;
	case ARC1ERR: close_down1f(arc1err, mti.prefix, ARC1ERR); break;
	/* End of errors which require prefix to be printed */

	case OUT4ERR:
	{
	    ldapfsl_get_user_name_phone(st.user_id, name, phone);
	    close_down4f(out4err, mti.prefix, name, phone, st.user_id, OUT4ERR);
	}
	break;
	case UPD4ERR:
	{
	    ldapfsl_get_user_name_phone(st.user_id, name, phone);
	    close_down4f(upd4err, mti.prefix, name, phone, st.user_id, UPD4ERR);
	}
	break;
	case DLT4DATA:
	{
	    ldapfsl_get_user_name_phone(st.user_id, name, phone);
	    close_down4f(dlt4data, mti.prefix, name, phone, st.user_id, DLT4DATA);
	}
	break;
	/* Errrors that require suffix to be printed */
	case CHA1CHO:  close_down1f(cha1cho, mti.suffix, CHA1CHO); break;
	case NCO1ERR:  close_down1f(nco1err, mti.suffix, NCO1ERR); break;
	case BAD1UPD:  close_down1f(bad1upd, mti.suffix, BAD1UPD); break;
	/* End of errors which require suffix to be printed */
	/* If data archived, restore it, and then process request */
	    case RESTORE:
	{
	    mask_restore(mask_msg.key1);
	    return(SUCCESS);
	}
	break;
	/* Locked for archive */
	case OVR2CHO:
	{
	    /* If this is Greg Keyser, do not allow him to check out the data */
	    if (strcmp(user.login, "bamsp") == 0)
	    {
		sprintf(rtn.msg, bamsp_err, mti.suffix);
		shut_down(rtn.msg, BAMSP_ERR);
	    }
	    else
	    {
		sprintf(rtn.msg, ovr2cho, mti.prefix, st.cwd);
		return_override(rtn.msg, OVR2CHO);
	    } /* End else */
	}
	break;

	case UP_WAIT:
	{
	    /* Wait for up to 20 seconds for update condition to clear */
	    sleep(1);
	    ++wait_cnt;
	    if (wait_cnt >= 20)
	    {
		/* Inform user that file is in the update state */
		sprintf(rtn.msg+rtn.len,upd4wrn,mti.prefix, st.user_id, st.own_id, st.cwd);
		rtn.len = strlen(rtn.msg);
		return(0);
	    }
	}
	break;
    case RD_WAIT:
	{
	    /* Wait for up to 20 seconds for read condition to clear */
	    sleep(1);
	    ++wait_cnt;
	    if (wait_cnt >= 20)
	    {
		/* Clear read state on file */
		return(0);
	    }
	} /* end case */
	break;
	case NGI_WRN:
	{
	    /* Process action, but return user warning message */
	    strcpy(rtn.msg+rtn.len, ngi_wrn);
	    rtn.len = strlen(rtn.msg);
	    return(0);
	}
	break;
	case OVR_WRN:
	{
	    /* Process action, but return user warning message */
	    strcpy(rtn.msg+rtn.len, ovr_wrn);
	    rtn.len = strlen(rtn.msg);
	    return(0);
	}
	break;
	case CHO4WRN:
	{
	    /* Process action, but return user warning message */
	    ldapfsl_get_user_name_phone(st.user_id, name, phone);
	    sprintf(rtn.msg+rtn.len, cho4wrn, mti.prefix, name, phone, st.user_id);
	    rtn.len = strlen(rtn.msg);
	    return(0);
	}
	break;
	/* Overwriting previous check-out */
	case OVR1OUT:
	{
	    /* If the user is getting ready to overwrite the current check-out */
	    if (strcmp(user.cwd, st.cwd) == 0)
	    {
		/* If this is Greg Keyser, do not allow check-out */
		/* because he constantly overwrites his data */
		if (strcmp(user.login, "bamsp") == 0)
		{
		    sprintf(rtn.msg, bamsp_err, mti.suffix);
		    shut_down(rtn.msg, BAMSP_ERR);
		}
		/* Otherwise tell the user he just overwrote his data */
		else
		{
		    sprintf(rtn.msg+rtn.len, ovr1out, mti.suffix);
		    rtn.len = strlen(rtn.msg);
		    return(0);
		} /* End else */
	    } /* End if */
	    /* If in another directory, just give the user a warning */
	    else
	    {
		sprintf(rtn.msg+rtn.len, rec_out, mti.prefix, st.cwd);
		rtn.len = strlen(rtn.msg);
		return(0);
	    } /* End else */
	}
	break;
	/* Greg Keyser is trying to read data already checked out */
	case BAM1ERR:
	{
	    if (strcmp(user.login, "bamsp") == 0)
	    {
		sprintf(rtn.msg, bam1err, mti.suffix);
		shut_down(rtn.msg, BA_ERR);
	    }
	    else return(SUCCESS);
	}
	break;
	/* Overwrite check-out and delete data */
	case UPD1CIN:
	{
	    sprintf(rtn.msg +rtn.len, upd1cin, mti.suffix);
	    rtn.len = strlen(rtn.msg);
	    return(0);
	}
	break;
	case UPD1OUT:
	{
	    sprintf(rtn.msg +rtn.len, upd1out, mti.suffix);
	    rtn.len = strlen(rtn.msg);
	    return(0);
	}
	break;
	case UPD1UPD:
	{
	    sprintf(rtn.msg +rtn.len, upd1upd, mti.suffix);
	    rtn.len = strlen(rtn.msg);
	    return(0);
	}
	break;
	case APP2WRN:
	{
	    sprintf(rtn.msg +rtn.len, app2wrn, mti.prefix, st.cwd);
	    rtn.len = strlen(rtn.msg);
	    return(0);
	}
	break;
	/* Remote update in progress with override flag on */
	case RMT_OVR: wait_rmt2upd(st.fd); break;
	case ARCHIVE:
	{
	    sprintf(rtn.msg, "%s %s %d %s",
	     server.machine, server.address, server.socket, st.approver);
	    put_short(ARCHIVE_WRN);
	    put_string(rtn.msg);
	    /* Send the status flags */
	    sprintf(bf.tmp_data, "%d %d %d %d %d %d", rtn.flag, fl.info_flag,
	    fl.file_offset, fl.tmpl_file_num, fl.dsgn_file_num, 0);
	    put_string(bf.tmp_data);
	    close(sock);
	    /* Build the message to log */
	    strcpy3(rtn.msg, own_wrn, ip.archive_mach, "'");
	    data_log(tr.log_file, rtn.msg);
    	log3("Closing ", "transaction ", tr.pid_string);
	    exit(1);
	}
	break;
	default: close_down1d(bad_state_val, total_state,  BAD_STATE_VAL);
    } /* end switch */
    return(0);
} /* End get_action */
