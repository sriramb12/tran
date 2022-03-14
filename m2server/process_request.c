void XUngrabServer() { }
void XOpenDisplay() { }
void XChangeProperty() { }
void XGetWindowProperty() { }
void XFlush() { }
void XGrabServer() { }
void XDeleteProperty() { }
void XFree() { }
void XSendEvent() { }
void XInternAtom() { }
/****************************************************************************
 *
 * Function:	process_request
 *
 * Description:	Processes requests from Mask Designer client
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "close_down.h"
#include "global_defines.h"
#include "global_server_prototypes.h"
#include "mask_server_defines.h"
#include "m2server_globals.h"
#include "mti.h"
#include "global_server_params.h"
#include "global_params.h"
#include "global_server_errs.h"
#include "m2msg.h"
#include "m2server_errs.h"
#include "m2req_types.h"
#include "m2server_arrays.h"
#include "m2state_file.h"
#include "m2server_paths.h"
/*
#include "m2server_prototypes.h"
*/
#include "rmt_server.h"
#include "rtn.h"
#include "server_lib.h"
#include "strcpy.h"
#include "user_params.h"
#include "global_debug.h"

void process_request(int socket)
{
    char new_password[16];
    register int cnt;
    int pid;
    int len;
    /* Indicate no email sent for the current transaction */
    extern int email_cnt ;
    extern int log_cnt ;
    extern char log_date[];
    char *log_ptr;
    char log_buffer[30];

    email_cnt = 0;
    /* Indicate no messages logged for the current transaction */
    log_cnt = 0;
    /* Initialize pre-state used when action fails */
    st.pre_state = NO_STATE;
    /* Copy the socket value into the global variable */
    sock = socket;
    /* Read the socket for the string */
    get_string(bf.buffer);
    sscanf(bf.buffer+12, "%d %d %d %d %[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%d %d %[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]%*c%[^;]",
    /* Specific request type, general request type, type of data, etc */
    &mask_msg.req_type, &mask_msg.data_type, &mask_msg.override_flag, &mask_msg.program,
    user.login, user.email, user.mach, user.addr, user.cwd, user.machine_type,
    mask_msg.key1, mask_msg.key2, mask_msg.key3, user.pin, &mask_msg.num,  &mask_msg.transaction_time,
    mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4, mask_msg.par5, user.name);
    data_log("buffer", mask_msg.key1);
    data_log("buffer", bf.buffer+12);
    /* copy the Core ID from the login name */
/*
    strcpy(user.email, user.login);
*/
    /* convert email to lower case */
    str_lower(user.email);
    /* Pull off the password  */
    /* Must get it by hand in order to copy null characters */
     for (cnt = 0; cnt <12; ++cnt)
    mask_msg.password[cnt] = bf.buffer[cnt];
    mask_msg.password[12] = '\0';
    /* Indicate no new directories have been created */
    fl.crt_dir1 = fl.crt_dir2 = fl.crt_dir3 = fl.crt_dir4 = NULL;
    fl.sec_state_flag = fl.cleanup_flag = fl.source_flag = DBNO;
    sys.vnum_flag = DBNO;
    pid = getpid();
    sprintf(tr.pid_string, "%d", pid);
    /* Get the current time and Build the log file name */
    bld_log_name();
    /* build audit log name */
    strcpy2(mti.audit_summary, AUDIT_PATH, tr.log_file+4);
    strcpy2(mti.rmt_audit_summary, RMT_AUDIT_PATH, tr.log_file+4);
    /* Save date for later use */
    /* do not need the buffer length for Linux */
    log_ptr = ctime_r(&tr.upd_time, log_buffer, 30);
    if (log_ptr != NULL)
    {
    len = strcpy1(log_date, log_ptr);
    log_date[len-1] = '\0';
    } /* End if */
    /* Reformat the message before it is put in the log file */
    /* do no log event_log transactions */
    if (mask_msg.req_type != EVENT_LOG)
    {
        /* Override flag holds user type */
        /* key3 holds time type */
        /* num holds time flag */
        sprintf(bf.buffer, "Processing %s %s, from %s\n\
                overwrite flag %d, PID %d  server time %s\
                Site %s%s, email %s machine %s\n\
                cwd: %s\n\
                Key %s, %s, %s, Pin %s,  program %d num %d\n\
                par1-5: %s, %s, %s, %s, %s\n",
                req_array[mask_msg.req_type], data_array[mask_msg.data_type],
                user.name,
                mask_msg.override_flag, pid, "ctime(&tr.upd_time)",
                user.login, user.addr, user.email, user.mach, user.cwd,
                mask_msg.key1, mask_msg.key2, mask_msg.key3, user.pin, mask_msg.program, mask_msg.num,
                mask_msg.par1, mask_msg.par2, mask_msg.par3, mask_msg.par4, mask_msg.par5);
        data_log(tr.log_file, bf.buffer);
        DBG("mask_msg.req_type=%d mask_msg.data_type=%d", mask_msg.req_type, mask_msg.data_type);
        DBG("tr.log_file=[%s] bf.buffer[%s]", tr.log_file, bf.buffer);
#if 0
#if defined(DEBUG) || defined(ENABLE_LOGS)
        if (mask_msg.req_type == 28 && mask_msg.data_type == 4)
        {
            DBG_WAIT_LONGER();
        }
#endif
#endif
    }
    if ((mask_msg.data_type == MASK_SET) && (mask_msg.req_type == M2LIST))
    {
    sprintf(bf.buffer, "User type %s time type %s time flag %s\n",
    user_type_array[mask_msg.override_flag], user_type_array[atoi(mask_msg.key3)], time_type_array[mask_msg.num]);
    data_log(tr.log_file, bf.buffer);
    } /* end for mask list function */
    /* Decrypt the password */
    /* Same function used to encrypt and decrypt the password */
    password_encrypt(new_password, mask_msg.password, PWD_LEN);
    /* If invalid password found, inform user */
    strcpy2(bf.tmp_data, user.login, MASK_KEY);
    bf.tmp_data[PWD_LEN] = 0;
    if (strcmp(new_password, bf.tmp_data) != 0)
    shut_down(bad_password, BAD_PASSWORD);
    /* Check to see if user is a registered user */
    /* do not make check if user is registering, */
    /* or refresh is being done */
    /* or user is retrieving a mask set number */
    if ((mask_msg.data_type != REGISTER) && (mask_msg.req_type != CHECK_PIN)
    && (mask_msg.req_type != REFRESH) && (mask_msg.req_type != MARK)
    && (mask_msg.req_type != SHADOW) && (mask_msg.req_type != RMT_DELETE)
    && (mask_msg.data_type != METRICS)
    && (mask_msg.data_type != SERIAL))
     reg_check();
    /* Indicate no files found to send */
    fl.file_offset = fl.dsgn_file_num = fl.tmpl_file_num = 0;
    /* Indicate the PC link has not changed */
    fl.sec_app_flag = fl.app_flag = fl.pc_flag = NO_MOD;
    /* Indicate no entries to add or delete */
    dlt_entry_cnt = add_entry_cnt = 0;
    /* Indicate no warnings or errors or global files found */
    fl.info_flag = rtn.len = 0;
     /* Indicate no lock file present */
    lock_files_init();
    switch (mask_msg.data_type)
    {
    case MASK_SET: process_mask(); break;
    case TEMPLATE: process_template(); break;
    case MACHINE:  process_machine(); break;
    case PC: process_pc(); break;
    case PC_HISTORY: process_pc_history(); break;
    case PARTITION: process_partition(); break;
    case GLOBAL:
    {
        process_global();
    }
    break;
    case FAB:
    {
        process_fab();
    }
    break;
    case CAP_WAFER: process_cap_wafer(); break;
    case REGISTER: process_register(); break;
    case USERGROUP: process_usergroup(); break;
    /* Process cron requests */
    case CRON: process_cron(); break;
    /* Process requests for contract numbers and serial numbers */
    case SERIAL: process_serial(); break;
    /* Inform user that invalid transaction type received */
    case METRICS:  process_metrics(); break;
    case OFF_TEMPLATE: process_off_template(); break;
    case OFF_GLOBAL: process_off_global(); break;
    case SQL_QUERY: process_sql(); break;
    default:
    {
        /* Return failure code to client */
        close_down1d(bad_data_type, mask_msg.data_type, BAD_DATA_TYPE);
    } /* end default */
    } /* End switch */
    log3("Closing ", "transaction ", tr.pid_string);
    exit(0);
} /* End process_request */
void process_db_admin() { }
void state_file_check() { }
void mask_restore(char *value) { }
void wait_rmt2upd() { }
void version_file_update() { }
void add_resend_file_entry() { }
void send_rmt_files() { printf("in send remote files\n");  }

/*#ifdef LOCAL_SERVER*/
flow_find_flow() { }
flow_free_process_step_list() { }
flow_get_process_step_list() { }
get_maskset() { }
maskset_free_flow_list() { }
maskset_get_created_from_fab() { }
maskset_get_created_from_technology() { }
maskset_get_flow_list() { }
maskset_restore() { }
option_get_first_process_step() { }
option_get_name() { }
option_get_next_process_step() { }
option_get_num_process_steps() { }
option_group_get_first_option() { }
option_group_get_name() { }
option_group_get_next_option() { }
option_group_get_num_options() { }
option_group_get_ret_group_of_interest() { }
option_group_init_search() { }
option_group_next_object() { }
process_step_get_name() { }
new_reg() { }
int common_template_layers_update(char *approve_status)
{ }
int common_layers_update(void)
{ }
/*#endif*/
mysql_library_end() { }
/* The following functions are not used by MDS */
size_convert() { }
long_time_convert() { }
elapsed_time_convert() { }
void report_usage(int req_type, char *time1, char *time2, char *fields)
{}
