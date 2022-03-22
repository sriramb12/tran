#ifndef tc_arrays_h 
#define tc_arrays_h

/* List of defined TC request types */
static char tc_request_array[25][32] = 
{
    "REGISTER",
    "JOB_SUBMIT",	/* Submit a new TC job */
    "JOB_CANCEL",	/* Cancel an existing TC job */
    "Q_GET",
    "Q_AUDIT",
    "RUN_NEW_JOB",
    "AUTOREG",
    "CHECKREG",
    "CLEANUP",
    "ANDY_CLEANUP",
    "AUTH_CHECK",
    "CONTROL_FILE_GET",
    "CONTROL_FILE_LIST",
    "CONTROL_FILE_PUT",
    "CONTROL_FILE_GET_PRIOR",
    "TECHNOLOGY_GET",
    "TECHNOLOGY_FILE_PUT",
    "TECHNOLOGY_FILE_GET",
    "TECHNOLOGY_FILE_LIST",
    "TECHNOLOGY_GET_PRIOR",
    "FILE_GET",
    "MKDIR",	/* Send an TC directory */
    "RMDIR",	/* Delete an existing directory on the TC machine */
    "AUDIT",
    "GTAR_GET",
};

static char server_type_array[60][32] = 
{
    "dtms_dessync",
    "dtms_drc_create_waiver",
    "dtms_mvrepository",
    "viaCnt",
    "pathcheck",
    "yieldprediction",
    "dfm_scoring",
    "devstats",
    "updateRepository",
    "dlc",
    "loadup",
    "lonely_via",
    "uploadRepository",
    "scoring",
    "tile_search",
    "extract_text",
    "dtms_drc_create_waivers",
    "jdp",
    "hotSpot",
    "cellCount",
    "EZpathcheck",
    "gds2oasis",
    "xor",
    "both",
    "cellTree",
    "evil_twins",
    "artiscan",
    "drc_gdb_upload",
    "extents_by_layer",
    "hotspot_check",
    "dpram_check",
    "drc_results_link",
    "hotSpot_metal2via",
    "extents_by_cell",
    "gridchk",
    "custom_mask_text_check",
    "gdbHeader",
    "pkg_gdb",
    "viperESD",
    "dfm_lhs_c90",
    "transDessync",
    "dfm_lhs_lpa",
    "dfm_gates",
    "doLSF",
    "viaCnt2",
    "dieAreaUtilization",
    "doLSF32",
    "wssd",
    "metadata",
    "bundle",
    "tecarr",
};


#endif /* Define for tc_arrays_h */
