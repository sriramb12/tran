#ifndef dfm_arrays_h 
#define dfm_arrays_h

/* List of defined DFM request types */

static char opc_req_array[32][20] = 
{
    "JOB_SUBMIT",	/* Submit a new DFM job */
    "JOB_CANCEL",	/* Cancel an existing DFM job */
    "DFM_CONTROL_GET",
    "DFM_TECHNOLOGY_GET",
    "DFM file get",
    "Control file put",
    "Technology file put",
    "Control file list",
    "Technology file lst",
    "MKDIR",	/* Send an DFM directory */
    "RMDIR",	/* Delete an existing directory on the DFM machine */
    "DIR_ADD_FILES",	/* Delete files from a directory on the DFM machine */
    "DIR_DLT_FILES",	/* Delete files from a directory on the DFM machine */
    "DIR_GET_FILE",	/* Get specified file from DFM machine */
    "DIR_GET_FILES",	/* Retrieve DFM job output files from OPC machine */
    "DIR_LIST",	/* List files in directory on DFM machine */
    "DIR_LIST_ROOTS",	/* List DFM roots present on the OPC machine */
    "PS_SHOW",	/* Show work load on DFM machine */
    "Q_GET",
    "Cleanup files",
    "AUTOREG",
    "CHECKREG",
    "JOB_FILES2DLT",
    "Audit",
    "Run new job",
    "Andy clean-up",
    "Admin auth check",
    "Ctrl file get prior",
    "Technology get prior",
    "gtar_get",
};

#endif /* Define for dfm_arrays_h */
