#ifndef opc_req_arrays_h 
#define opc_req_arrays_h

/* List of defined RET request types */

static char opc_req_array[76][20] = 
{
    "RETJOB_SUBMIT",	/* Submit a new OPC job */
    "RETJOB_CANCEL",	/* Cancel an existing OPC job */
    "RETJOB_MOD_PRIORITY",	/* Modify the priority of a queued OPC job */
    "RETJOB_HOLD",	/* Put an OPC job on hold */
    "RETJOB_RELEASE",	/* Take an OPC job off hold */
    "RETJOB_MOD_RUNTIME",	/* Modify the run time for an OPC job */
    "RET_RUN_NEW_JOB",	/* Run new job when one has completed */
    "RET_MKDIR",	/* Send an OPC directory */
    "RET_RMDIR",	/* Delete an existing directory on the OPC machine */
    "RETDIR_ADD_FILES",	/* Add files to an existing directory on the OPC machine */
    "RETDIR_DLT_FILES",	/* Delete files from a directory on the OPC machine */
    "RETDIR_GET_FILE",	/* Get specified file from OPC machine */
    "RETDIR_GET_FILES",	/* Retrieve OPC job output files from OPC machine */
    "RETDIR_LIST",	/* List files in directory on OPC machine */
    "RETDIR_LIST_ROOTS",	/* List OPC roots present on the OPC machine */
    "RET_PS_SHOW",	/* Show work load on OPC machine */
    "RETQ_GET",
    "RETQ_AUDIT",
    "RET_AUTOREG",
    "RET_CHECKREG",
    "RETJOB_FILES2DLT",
    "RET_WORKSPACE_RTV",
    "RETDIR_STAT",
     "RETDIR_CLEANUP",
    "REPORT_JOB_PROFILE",
    "REPORT_MAX",
    "REPORT_PROJECT",
    "REPORT_SUMMARY",
     "REPORT_USER_DISK",
    "REPORT_USER_LIST",
    "REPORT_WEEKLY",
    "REPORT_DISK",
    "PROJECT_TOUCH",
    "PROJECT_TIME_OUT",
    "REPORT1PROJECT",
    "REPORT_ALL_PROJECTS",
  "REPORT_ROOT_PROJECTS",
  "REPORT_DEPT_TECH",
    "REPORT_MBG",
    "REPORT_MBG_TECH",
   "REPORT_TECH",
  "REPORT_USER_TECH",
    "REPORT_USER",
	"EMAIL_LIST",
    "RETQ_RESTORE",
    "RETQ_GET_BACKUP",
    "REPORT_AUDIT",
    "AUDIT_REPAIR",
     "USER_FILE_UPDATE",
    "REG_ADD",
    "SITE_SHOW",
    "SITE_ACTIVATE",
    "SITE_DEACTIVATE",
    "SITE_DELETE",
    "USER_SHOW",
    "USER_ACTIVATE",
    "USER_DEACTIVATE",
    "USER_DELETE",
    "USER_LIST",
    "SHOW_SERVER_TYPE",
    "RETJOB_BLOCK",
    "REPORT_DELETE_TIME",
    "RET_MYSQL",
    "RET_CVS_GET",
    "RET_PING",
    "RET_PAGING_SET",
    "RET_PAGING_GET",
    "RET_AUTH_CHECK",
    "RETDIR64LIST",
    "OPCDIR64STAT",
    "OPCDIR64LIST_ROOTS",
    "gtar_get",
    "RET server rtv",
    "Gtar get",
    "Job num get",
    "Cron check SQL errs",
};

#endif /* Define for opc_req_arrays_h */
