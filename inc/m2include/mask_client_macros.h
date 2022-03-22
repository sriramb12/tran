#ifndef mask_client_macros_h
#define mask_client_macros_h
/* This file holds macros used by the Mask Prep client and user interface. */

#define CLOSE_DOWN(ret)\
{\
    rtn.flag = DB_ERR;\
    close(sock);\
    return(ret);\
} /* End close_down */

#define CLOSE_DOWN_ERASE(CWD, ret)\
{\
    if (overwrite_file == DBNO) \
    erase_files(CWD, num, files);\
    rtn.flag = DB_ERR;\
    close(sock);\
    return(ret);\
} /* End CLOSE_DOWN_ERASE */

/* Inform calling function that bad data received from database server */
#define RETURN_BAD_PUT(); {\
    sprintf(rtn.msg, bad_put, errno, HELP_MSG);\
    rtn.flag = DB_ERR; \
    close(sock); \
    return(BAD_PUT); \
}
#define RETURN_BAD_GET(); {\
    sprintf(rtn.msg, bad_get, errno, HELP_MSG);\
    rtn.flag = DB_ERR; \
    close(sock); \
    return(BAD_GET); \
}

/* Same as above except for the mask shop server */
#define RETURN_BAD_MS_GET(); {\
    rtn.len = strcpy3(rtn.msg, bad_ms_get, DOUBLE_RET, HELP_MSG); \
    rtn.flag = DB_ERR; \
    close(sock); \
    return(BAD_MS_GET); \
}
/* Mask set macros */

#define dbcanapp_mask(mask, num, files) \
    unget_files(DBCANAPP_DATA, 'm', 'c', 'a', mask, NONE, NONE, num, files)

#define dbcanchkout_mask(mask, num, files) \
    unget_files('H', 'm', 'c', 'a', mask, NONE, NONE, num, files)

#define dbm2canchkout_mask(mask, num, files) \
    unget_files(64, 'm', 'c', 'a', mask, NONE, NONE, num, files)

#define dbm2convert_mask(mask, files) \
    get_files(62, 'm', 'b', 'a', mask, NONE, NONE,\
    NONE, NONE, NONE, NONE,  files, 0);

#define dbcopy_mask(parent_mask, mask, files)\
    get_files(DBCOPY_DATA, 'm', CRT_REQ, 'a', mask, NONE, NONE,\
    parent_mask, NONE, NONE, NONE, files, 0)

#define dbcrt_mask(mask, parent_wl, parent_flow, parent_design, files) \
    get_files(DBCRT_DATA, 'm', CRT_REQ, 'a', mask, NONE, NONE,\
    parent_wl, parent_flow, parent_design, NONE, files, 0);

#define dbdlt_mask(mask) \
rollback_data(mask, NONE, NONE, DBDLT_DATA, 'm', 'a')

/* Delete old versions of mask data */
#define dbdlt_mask_versions(mask, num_versions) \
    dlt_old_versions('a', mask, NONE, NONE, num_versions)
#define dbgrtovrwr_mask(mask, user_id, user_mach)\
    send_data(DBGRANT_OVRWR, 'm',  'a', mask, NONE, NONE, user_id, NONE, user_mach)

#define dbmodown_mask(mask, user_id, user_addr, user_mach)\
    send_data(DBMOD_OWNER, 'm',  'a', mask, NONE, NONE, user_id, user_addr, user_mach)

/* Force shadow of mask set to remote databases */
#define dbshadow_mask(mask) \
    send_data(DBSHADOW_DATA, 'm',  'a', mask, NONE, NONE, \
    NONE, NONE, NONE);

#define dbm2read_mask(mask, files) \
    get_files(61, 'm', 'b', 'a', mask, NONE, NONE,\
    NONE, NONE, NONE, NONE,  files, 0);

#define dbrmv_mask(mask) \
rollback_data(mask, NONE, NONE, DBRMV_DATA, 'm',  'a')

/* Retrieves list of versions for mask */
/* Also retrieves login name, IP node address,  and machine of mask creator */
#define dbrtv_mask_versions(mask, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, 'a', mask, NONE, NONE, files)

#define dbport_mask(mask, parent_wl, parent_flow, parent_design, num, files) \
    put_files(DBPORT_DATA, 'm',  CRT_REQ,  'a', mask, NONE, NONE, \
    parent_wl, parent_flow, parent_design, NONE, num, files)

#define dbtransfer_mask(mask, node_name)\
    transfer_data('a', mask, NONE, NONE, node_name)

#define dbupd_mask(mask,  num, files) \
    put_files(DBUPD_DATA, 'm',  UPD_REQ, 'a', mask, NONE, NONE, \
    NONE, NONE, NONE, NONE, num-FILE_OFFSET, files+FILE_OFFSET)
/* Template macros */

#define dbapprove_tmpl(wl, flow, design, files) \
    get_files(DBAPP_DATA, 'K', 'b','e', wl, flow, design,\
    NONE, NONE, NONE, "e\0", files, 0)

#define dbbackout_tmpl(wl, flow) \
rollback_data(wl, flow, NONE, DBBACKOUT_DATA, 'm', OFF_TMPL)

#define dbchkoff_tmpl(wl, flow, num, files) \
checkoff_data(TMPL, 'm', wl, flow, NONE, num, files)

#define dbcanapp_tmpl(wl, flow, num, files) \
    unget_files(DBCANAPP_DATA, NONE, 'c', TMPL, wl, flow, NONE, num, files)

#define dbcanchkout_tmpl(wl, flow, num, files) \
    unget_files(DB'H'_DATA, NONE, 'c', TMPL, wl, flow, NONE, num, files)


#define dbchkout_tmpl(wl, flow, design, files) \
    get_files('F', 'K', 'b', TMPL_OFF_DSGN, wl, flow, design,\
    NONE, NONE, NONE, "e\0", files, 0)

#define dbcrt_tmpl(parent_wl, parent_flow, parent_design, wl, flow, files) \
    get_files(DBCRT_DATA, 'K', CRT_REQ, TMPL_OFF_DSGN, wl, flow, parent_design,  \
    parent_wl, parent_flow, parent_design, tmpl,  files, 1)

#define dbcopy_mask2tmpl(wl, flow, design, mask, files) \
    get_files(DBCOPY_'a'2TMPL, 'm', CRT_REQ,'e', wl, flow, design,  \
    mask, NONE, NONE, tmpl, files, 1)

#define dbdlt_tmpl(wl, flow) \
rollback_data(wl, flow, NONE, DBDLT_DATA, 'm', TMPL)

#define dbgrtovrwr_tmpl(wl, flow, user_id, user_mach)\
    send_data(DBGRANT_OVRWR, 'm',  TMPL, wl, flow, NONE, user_id, NONE, user_mach)

#define dbmodlnk_tmpl(wl, flow, link) \
    send_data(DBMOD_LINK, 'm',  TMPL, wl, flow, link, NONE, NONE, NONE)

#define dbaddlnk_tmpl(wl, flow, pc_name) \
    send_data(DBADD_LINK, 'm',  TMPL, wl, flow, pc_name, NONE, NONE, NONE)

#define dbdltlnk_tmpl(wl, flow) \
    send_data(DBDLT_LINK, 'm',  TMPL, wl, flow, NONE, NONE, NONE, NONE)

#define dbmodown_tmpl(wl, flow, user_id, user_addr, user_mach)\
    send_data(DBMOD_OWNER, 'm', TMPL, wl, flow, NONE, user_id, user_addr, user_mach)

/* Force shadow of process flow to remote databases */
#define dbshadow_tmpl(wl, flow) \
    send_data(DBSHADOW_DATA, 'm',  TMPL, wl, flow, NONE, \
    NONE, NONE, NONE);

#define dbshadow_off_tmpl(wl, flow) \
    send_data(DBSHADOW_DATA, 'm',  OFF_TMPL, wl, flow, NONE, \
    NONE, NONE, NONE);

#define dbrmv_tmpl(wl, flow) \
rollback_data(wl, flow, NONE, DBRMV_DATA, 'm',  TMPL)

/* Retrieves list of versions for fab template */
/* Also retrieves login name, IP node address,  and machine of template creator */
#define dbrtv_tmpl_versions(wl, flow, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, TMPL, wl, flow, NONE, files)

#define dboff_tmpl_versions(wl, flow, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, OFF_TMPL, wl, flow, NONE, files)

#define dbtransfer_tmpl(wl, flow, node_name)\
    transfer_data(TMPL, wl, flow, NONE, node_name)

#define dbunapp_tmpl(wl, flow) \
rollback_data(wl, flow, NONE, DBUNAPP_DATA, 'm', OFF_TMPL)

/* Design macros */

#define dbapprove_dsgn(wl, flow, design, files) \
    get_files('K', DBAPP_DATA, 'b','e', wl, flow, design,\
    NONE, NONE, NONE, "e\0", files, 0)

#define dbbackout_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBBACKOUT_DATA, 'm', OFF_DSGN)

#define dbcanapp_dsgn(wl, flow, design, num, files) \
    unget_files(DBCANAPP_DATA, NONE, 'c', DSGN, wl, flow, design, \
    num, files)

#define dbcanchkout_dsgn(wl, flow, design, num, files) \
    unget_files(DB'H'_DATA, 'm', 'c', DSGN, wl, flow, design, \
    num, files)

#define dbchkoff_dsgn(wl, flow, design, num, files) \
checkoff_data(DSGN, 'm', wl, flow, design, num, files)

#define dbchkout_dsgn(wl, flow, design, files) \
    get_files('K', 'F', 'b', OFF_TMPL_DSGN, wl, flow, design, \
    NONE, NONE, NONE, "e\0", files, 0)

#define dbcrt_dsgn(parent_wl, parent_flow, parent_design, wl, flow, design, files) \
    get_files('K', DBCRT_DATA, CRT_REQ, OFF_TMPL_DSGN, wl, flow, design, \
    parent_wl, parent_flow, parent_design, dsgn, files, 1)

#define dbdlt_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBDLT_DATA, 'm', DSGN)

#define dbgrtovrwr_dsgn(wl, flow, design, user_id, user_mach)\
    send_data(DBGRANT_OVRWR, 'm',  DSGN, wl, flow, design, user_id, user_mach)

#define dbmodown_dsgn(wl, flow, design, user_id, user_addr, user_mach)\
    send_data(DBMOD_OWNER, 'm', DSGN, wl, flow, design, user_id, user_addr, user_mach)

#define dbrmv_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBRMV_DATA, 'm',  DSGN)

/* Retrieves list of versions for design template */
/* Also retrieves login name, IP node address,  and machine of  template creator */
#define dbrtv_dsgn_versions(wl, flow, design, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, DSGN, wl, flow, design, files)

#define dbshadow_dsgn(wl, flow, dsgn) \
    send_data(DBSHADOW_DATA, 'm',  DSGN, wl, flow, dsgn, \
    NONE, NONE, NONE);

#define dbshadow_off_dsgn(wl, flow, dsgn) \
    send_data(DBSHADOW_DATA, 'm',  OFF_DSGN, wl, flow, dsgn, \
    NONE, NONE, NONE);

#define dboff_dsgn_versions(wl, flow, design, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, OFF_DSGN, wl, flow, design, files)

#define dbtransfer_dsgn(wl, flow, design, node_name)\
    transfer_data(DSGN, wl, flow, design, node_name)

#define dbunapp_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBUNAPP_DATA, 'm', OFF_DSGN)

/* Template design macros */

#define dbapprove_tmpl_dsgn(wl, flow, design, files) \
    get_files(DBAPP_DATA, DBAPP_DATA, 'b','e', wl, flow, design,\
    NONE, NONE, NONE, "e\0", files, 0)

#define dbcanapp_tmpl_dsgn(wl, flow, design, num, files) \
    unget_files(DBCANAPP_DATA, DBCANAPP_DATA, 'c','e', wl, flow, design, \
    num, files)

#define dbcanchkout_tmpl_dsgn(wl, flow, design, num, files) \
    unget_files('H', 'H', 'c','e', wl, flow, design, \
    num, files)

#define dbchkoff_tmpl_dsgn(wl, flow, design, num, files) \
checkoff_data(TMPL_DSGN, DBCHKOFF_DATA, wl, flow, design, num, files)

#define dbchkout_tmpl_dsgn(wl, flow, design, files) \
    get_files('F', 'F',  'b','e', wl, flow, design, \
    NONE, NONE, NONE, "e\0", files, 0)

#define dbcrt_tmpl_dsgn(parent_wl, parent_flow, parent_design, wl, flow, design, files) \
    get_files(DBCRT_DATA, DBCRT_DATA, CRT_REQ,'e', wl, flow, design,  \
    parent_wl, parent_flow, parent_design, "e\0", files, 2)

#define dbgrtovrwr_tmpl_dsgn(wl, flow, design, user_id, user_mach)\
    send_data(DBGRANT_OVRWR, DBGRANT_OVRWR,'e', wl, flow, design, \
    user_id, NONE, user_mach)

#define dbmodown_tmpl_dsgn(wl, flow, design, user_id, user_addr, user_mach)\
    send_data(DBMOD_OWNER, DBMOD_OWNER,'e', wl, flow, design, user_id, user_addr, user_mach)

#define dbread_tmpl_dsgn(wl, flow, design, files) \
    get_files('K', 'K', 'b','e', wl, flow, design,\
    NONE, NONE, NONE, "e\0", files, 0)

/* Read official fab and unofficial design data */
#define dbread_otmpl_dsgn(wl, flow, design, files) \
    get_files('K', 'K', 'b', OFF_TMPL_DSGN, wl, flow, design,\
    NONE, NONE, NONE, "e\0",  files, 0)

/* Read unofficial fab data and official design data */
#define dbread_tmpl_odsgn(wl, flow, design, files) \
    get_files('K', 'K', 'b', TMPL_OFF_DSGN, wl, flow, design,\
    NONE, NONE, NONE, "e\0", files, 0)

#define dbtransfer_tmpl_dsgn(wl, flow, design, node_name)\
    transfer_data(TMPL_DSGN, wl, flow, design, node_name)

#define dbunapp_tmpl_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBUNAPP_DATA, DBUNAPP_DATA, APP_TMPL_DSGN)
#define dbread_dsgn_tmpl(wl, flow, design, files) \
    get_files('K', 'K', 'b', OFF_TMPL_OFF_DSGN, wl, flow, design,\
    NONE, NONE, NONE, dsgn_tmpl, files, 0)


#define dbcrt_pc(pc, control_list, num, files)\
    put_pc_files(DBCRT_DATA, CRT_REQ, PC_LIB, NONE, NONE, pc, control_list, num, files)

#define dbcrt_wlpc(wl, flow, control_list, num, files)\
    put_pc_files(DBCRT_DATA, CRT_REQ, PC_LIB, wl, flow, NONE, control_list, files)

#define dbdlt_pc(pc) \
rollback_data(NONE, NONE, pc, DBDLT_DATA, 'm', PC_LIB)

#define dblist_links(pc, files) \
    rtv_files(DBLIST_LINKS, GLOBAL_REQ, PC_LIB, NONE, NONE, pc, files)

#define dblist_pcs(files) \
    rtv_files(DBLIST_PCS, GLOBAL_REQ, PC_LIB, NONE, NONE, NONE, files)

#define dbread_pc(pc, files)\
    get_files('K', 'K',  'b', PC_LIB, NONE, NONE, pc, \
     NONE, NONE, NONE, NONE, files, 0)

#define dbread_wlpc(wl, flow, files) \
    get_files('K', 'm',  'b', WL_PC, wl, flow, NONE, \
     NONE, NONE, NONE, NONE, files, 0)

#define dbrename_pc(old_pc, new_pc) \
    send_data(DBRENAME_DATA, 'm',  PC_LIB, NONE, NONE, old_pc, new_pc, NONE, NONE)

#define dbrename_wlpc(wl, flow, new_pc) \
    send_data(DBRENAME_DATA, 'm',  WL_PC, wl, flow, NONE, new_pc, NONE, NONE)


#define dbtransfer_pc(pc, node_name)\
    transfer_data(PC_LIB, NONE, NONE, pc, node_name)

#define dbtransfer_wlpc(wl, flow, node_name)\
    transfer_data(PC_LIB, wl, flow, NONE, node_name)

/*
#define dbread_masks(wl, flow, design, user, machine, date1, date2, files) \
    get_files(DBRTV_VERSIONS, 'm', GLOBAL_REQ, 'a', date1, date2, NONE,\
    wl, flow, design, user, machine, files, 0)
*/


#define dbread_beta_rel(release, files) \
    get_files(DBREAD_BETA, 'm', RELEASE_REQ, GLBL, release, NONE, NONE,\
NONE, NONE, NONE, NONE, files, 0)

#define dbread_off_rel(release, version, files) \
    get_files(DBREAD_OFFICIAL, 'm', RELEASE_REQ, GLBL, release, version, NONE,\
NONE, NONE, NONE, NONE, files, 0)

#define dbrtv_maskshops(output_file) dbrtv_global_file("maskshops", output_file)

#define dbread_control_groups(output_file) dbrtv_global_file("control_groups", output_file)


#define dbps_df() \
    dbresend_data(DBPS_DF);
#define dbmake_tmp_dir() \
    send_generic_data(NETWORK_REQ, DBMAKE_TMP_DIR, 'm', GLBL, \
    NONE, NONE, NONE, NONE, NONE, NONE)
#define dbchkout_convert_mask(mask, files) \
    get_files('F', 'F', 'b', 'a', mask, NONE, NONE,\
    NONE, NONE, NONE, NONE,  files, 0);


#define dbread_convert_mask(mask, files) \
    get_files('K', 'F', 'b', 'a', mask, NONE, NONE,\
    NONE, NONE, NONE, NONE,  files, 0);

#endif /* ifdef for mask_client_macros_h */
