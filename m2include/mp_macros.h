/* This file holds macros used by the client and user interface. */

#define CLOSE_DOWN(ret)\
{\
    WRN_FLAG = DB_ERR;\
    close(sock);\
    return(ret);\
} /* End close_down */

#define CLOSE_DOWN_ERASE(CWD, ret)\
{\
    if (overwrite_file == DBNO) \
    erase_files(CWD, num, files);\
    WRN_FLAG = DB_ERR;\
    close(sock);\
    return(ret);\
} /* End CLOSE_DOWN_ERASE */

/* Inform calling function that bad data received from database server */
#define RETURN_BAD_PUT(); {\
    sprintf(rtn.msg, bad_put, errno, HELP_MSG);\
    WRN_FLAG = DB_ERR; \
    close(sock); \
    return(BAD_PUT); \
}
#define RETURN_BAD_GET(); {\
    sprintf(rtn.msg, bad_get, errno, HELP_MSG);\
    WRN_FLAG = DB_ERR; \
    close(sock); \
    return(BAD_GET); \
}

/* Same as above except for the mask shop server */
#define RETURN_BAD_MS_GET(); {\
    rtn_msg_len = strcpy3(rtn.msg, bad_ms_get, DOUBLE_RET, HELP_MSG); \
    WRN_FLAG = DB_ERR; \
    close(sock); \
    return(BAD_MS_GET); \
}
/* Mask set macros */

#define dbcanapp_mask(mask, num, files) \
    unget_files(DBCANAPP_DATA, DBNO_REQ, MOD_REQ, MASK, mask, NONE, NONE, num, files)

#define dbcanchkout_mask(mask, num, files) \
    unget_files(DBCANCHKOUT_DATA, DBNO_REQ, MOD_REQ, MASK, mask, NONE, NONE, num, files)

#define dbchkout_mask(mask, files) \
    get_files(DBCHKOUT_DATA, DBNO_REQ, READ_REQ, MASK, mask, NONE, NONE,\
    NONE, NONE, NONE, NONE, NONE, files, 0);

#define dbcopy_mask(parent_mask, mask, files)\
    get_files(DBCOPY_DATA, DBNO_REQ, CRT_REQ, MASK, mask, NONE, NONE,\
    parent_mask, NONE, NONE, NONE, NONE, files, 0)

#define dbcrt_mask(mask, parent_wl, parent_flow, parent_design, files) \
    get_files(DBCRT_DATA, DBNO_REQ, CRT_REQ, MASK, mask, NONE, NONE,\
    parent_wl, parent_flow, parent_design, NONE, NONE, files, 0);

#define dbdlt_mask(mask) \
rollback_data(mask, NONE, NONE, DBDLT_DATA, DBNO_REQ, MASK)

/* Delete old versions of mask data */
#define dbdlt_mask_versions(mask, num_versions) \
    dlt_old_versions(MASK, mask, NONE, NONE, num_versions)
#define dbgrtovrwr_mask(mask, user_id, user_mach)\
    send_data(DBGRANT_OVRWR, DBNO_REQ,  MASK, mask, NONE, NONE, user_id, NONE, user_mach)

#define dbmodown_mask(mask, user_id, user_addr, user_mach)\
    send_data(DBMOD_OWNER, DBNO_REQ,  MASK, mask, NONE, NONE, user_id, user_addr, user_mach)

/* Force shadow of mask set to remote databases */
#define dbshadow_mask(mask) \
    send_data(DBSHADOW_DATA, DBNO_REQ,  MASK, mask, NONE, NONE, \
    NONE, NONE, NONE);

#define dbread_mask(mask, files) \
    get_files(DBREAD_DATA, DBNO_REQ, READ_REQ, MASK, mask, NONE, NONE,\
    NONE, NONE, NONE, NONE, NONE,  files, 0);

#define dbrmv_mask(mask) \
rollback_data(mask, NONE, NONE, DBRMV_DATA, DBNO_REQ,  MASK)

#define dbrtv_mask_versions(mask, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, MASK, mask, NONE, NONE, files)

#define dbport_mask(mask, parent_wl, parent_flow, parent_design, num, files) \
    put_files(DBPORT_DATA, DBNO_REQ,  CRT_REQ,  MASK, mask, NONE, NONE, \
    parent_wl, parent_flow, parent_design, NONE, NONE, num, files)

#define dbtransfer_mask(mask, node_name)\
    transfer_data(MASK, mask, NONE, NONE, node_name)

#define dbupd_mask(mask,  num, files) \
    put_files(DBUPD_DATA, DBNO_REQ,  UPD_REQ, MASK, mask, NONE, NONE, \
    NONE, NONE, NONE, NONE, NONE, num-FILE_OFFSET, files+FILE_OFFSET)
/* Template macros */

#define dbapprove_tmpl(wl, flow, design, files) \
    get_files(DBAPP_DATA, DBREAD_DATA, READ_REQ, TMPL_DSGN, wl, flow, design,\
    NONE, NONE, NONE, tmpl_dsgn, NONE, files, 0)

#define dbbackout_tmpl(wl, flow) \
rollback_data(wl, flow, NONE, DBBACKOUT_DATA, DBNO_REQ, OFF_TMPL)

#define dbchkoff_tmpl(wl, flow, num, files) \
checkoff_data(TMPL, DBNO_REQ, wl, flow, NONE, num, files)

#define dbcanapp_tmpl(wl, flow, num, files) \
    unget_files(DBCANAPP_DATA, NONE, MOD_REQ, TMPL, wl, flow, NONE, num, files)

#define dbcanchkout_tmpl(wl, flow, num, files) \
    unget_files(DBCANCHKOUT_DATA, NONE, MOD_REQ, TMPL, wl, flow, NONE, num, files)


#define dbchkout_tmpl(wl, flow, design, files) \
    get_files(DBCHKOUT_DATA, DBREAD_DATA, READ_REQ, TMPL_OFF_DSGN, wl, flow, design,\
    NONE, NONE, NONE, tmpl_dsgn, NONE, files, 0)

#define dbcrt_tmpl(parent_wl, parent_flow, parent_design, wl, flow, files) \
    get_files(DBCRT_DATA, DBREAD_DATA, CRT_REQ, TMPL_OFF_DSGN, wl, flow, parent_design,  \
    parent_wl, parent_flow, parent_design, tmpl, NONE, files, 1)

#define dbcopy_mask2tmpl(wl, flow, design, mask, files) \
    get_files(DBCOPY_MASK2TMPL, DBNO_REQ, CRT_REQ, TMPL_DSGN, wl, flow, design,  \
    mask, NONE, NONE, tmpl, NONE, files, 1)

#define dbdlt_tmpl(wl, flow) \
rollback_data(wl, flow, NONE, DBDLT_DATA, DBNO_REQ, TMPL)

#define dbgrtovrwr_tmpl(wl, flow, user_id, user_mach)\
    send_data(DBGRANT_OVRWR, DBNO_REQ,  TMPL, wl, flow, NONE, user_id, NONE, user_mach)

#define dbmodlnk_tmpl(wl, flow, link) \
    send_data(DBMOD_LINK, DBNO_REQ,  TMPL, wl, flow, link, NONE, NONE, NONE)

#define dbaddlnk_tmpl(wl, flow, pc_name) \
    send_data(DBADD_LINK, DBNO_REQ,  TMPL, wl, flow, pc_name, NONE, NONE, NONE)

#define dbdltlnk_tmpl(wl, flow) \
    send_data(DBDLT_LINK, DBNO_REQ,  TMPL, wl, flow, NONE, NONE, NONE, NONE)

#define dbmodown_tmpl(wl, flow, user_id, user_addr, user_mach)\
    send_data(DBMOD_OWNER, DBNO_REQ, TMPL, wl, flow, NONE, user_id, user_addr, user_mach)

/* Force shadow of process flow to remote databases */
#define dbshadow_tmpl(wl, flow) \
    send_data(DBSHADOW_DATA, DBNO_REQ,  TMPL, wl, flow, NONE, \
    NONE, NONE, NONE);

#define dbrmv_tmpl(wl, flow) \
rollback_data(wl, flow, NONE, DBRMV_DATA, DBNO_REQ,  TMPL)

#define dbrtv_tmpl_versions(wl, flow, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, TMPL, wl, flow, NONE, files)

#define dboff_tmpl_versions(wl, flow, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, OFF_TMPL, wl, flow, NONE, files)

#define dbtransfer_tmpl(wl, flow, node_name)\
    transfer_data(TMPL, wl, flow, NONE, node_name)

#define dbunapp_tmpl(wl, flow) \
rollback_data(wl, flow, NONE, DBUNAPP_DATA, DBNO_REQ, OFF_TMPL)

/* Design macros */

#define dbapprove_dsgn(wl, flow, design, files) \
    get_files(DBREAD_DATA, DBAPP_DATA, READ_REQ, TMPL_DSGN, wl, flow, design,\
    NONE, NONE, NONE, tmpl_dsgn, NONE, files, 0)

#define dbbackout_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBBACKOUT_DATA, DBNO_REQ, OFF_DSGN)

#define dbcanapp_dsgn(wl, flow, design, num, files) \
    unget_files(DBCANAPP_DATA, NONE, MOD_REQ, DSGN, wl, flow, design, \
    num, files)

#define dbcanchkout_dsgn(wl, flow, design, num, files) \
    unget_files(DBCANCHKOUT_DATA, DBNO_REQ, MOD_REQ, DSGN, wl, flow, design, \
    num, files)

#define dbchkoff_dsgn(wl, flow, design, num, files) \
checkoff_data(DSGN, DBNO_REQ, wl, flow, design, num, files)

#define dbchkout_dsgn(wl, flow, design, files) \
    get_files(DBREAD_DATA, DBCHKOUT_DATA, READ_REQ, OFF_TMPL_DSGN, wl, flow, design, \
    NONE, NONE, NONE, tmpl_dsgn, NONE, files, 0)

#define dbcrt_dsgn(parent_wl, parent_flow, parent_design, wl, flow, design, files) \
    get_files(DBREAD_DATA, DBCRT_DATA, CRT_REQ, OFF_TMPL_DSGN, wl, flow, design, \
    parent_wl, parent_flow, parent_design, dsgn, NONE, files, 1)

#define dbdlt_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBDLT_DATA, DBNO_REQ, DSGN)

#define dbgrtovrwr_dsgn(wl, flow, design, user_id, user_mach)\
    send_data(DBGRANT_OVRWR, DBNO_REQ,  DSGN, wl, flow, design, user_id, user_mach)

#define dbmodown_dsgn(wl, flow, design, user_id, user_addr, user_mach)\
    send_data(DBMOD_OWNER, DBNO_REQ, DSGN, wl, flow, design, user_id, user_addr, user_mach)

#define dbrmv_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBRMV_DATA, DBNO_REQ,  DSGN)

#define dbrtv_dsgn_versions(wl, flow, design, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, DSGN, wl, flow, design, files)

#define dboff_dsgn_versions(wl, flow, design, files) \
    rtv_files(DBRTV_VERSIONS, VERSION_REQ, OFF_DSGN, wl, flow, design, files)

#define dbtransfer_dsgn(wl, flow, design, node_name)\
    transfer_data(DSGN, wl, flow, design, node_name)

#define dbunapp_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBUNAPP_DATA, DBNO_REQ, OFF_DSGN)

/* Template design macros */

#define dbapprove_tmpl_dsgn(wl, flow, design, files) \
    get_files(DBAPP_DATA, DBAPP_DATA, READ_REQ, TMPL_DSGN, wl, flow, design,\
    NONE, NONE, NONE, tmpl_dsgn, NONE, files, 0)

#define dbcanapp_tmpl_dsgn(wl, flow, design, num, files) \
    unget_files(DBCANAPP_DATA, DBCANAPP_DATA, MOD_REQ, TMPL_DSGN, wl, flow, design, \
    num, files)

#define dbcanchkout_tmpl_dsgn(wl, flow, design, num, files) \
    unget_files(DBCANCHKOUT_DATA, DBCANCHKOUT_DATA, MOD_REQ, TMPL_DSGN, wl, flow, design, \
    num, files)

#define dbchkoff_tmpl_dsgn(wl, flow, design, num, files) \
checkoff_data(TMPL_DSGN, DBCHKOFF_DATA, wl, flow, design, num, files)

#define dbchkout_tmpl_dsgn(wl, flow, design, files) \
    get_files(DBCHKOUT_DATA, DBCHKOUT_DATA,  READ_REQ, TMPL_DSGN, wl, flow, design, \
    NONE, NONE, NONE, tmpl_dsgn, NONE, files, 0)

#define dbcrt_tmpl_dsgn(parent_wl, parent_flow, parent_design, wl, flow, design, files) \
    get_files(DBCRT_DATA, DBCRT_DATA, CRT_REQ, TMPL_DSGN, wl, flow, design,  \
    parent_wl, parent_flow, parent_design, tmpl_dsgn, SEC_PIN, files, 2)

#define dbgrtovrwr_tmpl_dsgn(wl, flow, design, user_id, user_mach)\
    send_data(DBGRANT_OVRWR, DBGRANT_OVRWR, TMPL_DSGN, wl, flow, design, \
    user_id, NONE, user_mach)

#define dbmodown_tmpl_dsgn(wl, flow, design, user_id, user_addr, user_mach)\
    send_data(DBMOD_OWNER, DBMOD_OWNER, TMPL_DSGN, wl, flow, design, user_id, user_addr, user_mach)

#define dbread_tmpl_dsgn(wl, flow, design, files) \
    get_files(DBREAD_DATA, DBREAD_DATA, READ_REQ, TMPL_DSGN, wl, flow, design,\
    NONE, NONE, NONE, tmpl_dsgn, NONE, files, 0)

#define dbread_otmpl_dsgn(wl, flow, design, files) \
    get_files(DBREAD_DATA, DBREAD_DATA, READ_REQ, OFF_TMPL_DSGN, wl, flow, design,\
    NONE, NONE, NONE, tmpl_dsgn, NONE, files, 0)

#define dbread_tmpl_odsgn(wl, flow, design, files) \
    get_files(DBREAD_DATA, DBREAD_DATA, READ_REQ, TMPL_OFF_DSGN, wl, flow, design,\
    NONE, NONE, NONE, tmpl_dsgn, NONE, files, 0)

#define dbtransfer_tmpl_dsgn(wl, flow, design, node_name)\
    transfer_data(TMPL_DSGN, wl, flow, design, node_name)

#define dbunapp_tmpl_dsgn(wl, flow, design) \
rollback_data(wl, flow, design, DBUNAPP_DATA, DBUNAPP_DATA, APP_TMPL_DSGN)
#define dbread_dsgn_tmpl(wl, flow, design, files) \
    get_files(DBREAD_DATA, DBREAD_DATA, READ_REQ, OFF_TMPL_OFF_DSGN, wl, flow, design,\
    NONE, NONE, NONE, dsgn_tmpl, NONE, files, 0)


#define dbcrt_pc(pc, control_list, num, files)\
    put_pc_files(DBCRT_DATA, CRT_REQ, PC, NONE, NONE, pc, control_list, num, files)

#define dbcrt_wlpc(wl, flow, control_list, num, files)\
    put_pc_files(DBCRT_DATA, CRT_REQ, PC, wl, flow, NONE, control_list, files)

#define dbdlt_pc(pc) \
rollback_data(NONE, NONE, pc, DBDLT_DATA, DBNO_REQ, PC)

#define dblist_links(pc, files) \
    rtv_files(DBLIST_LINKS, GLOBAL_REQ, PC, NONE, NONE, pc, files)

#define dblist_pcs(files) \
    rtv_files(DBLIST_PCS, GLOBAL_REQ, PC, NONE, NONE, NONE, files)

#define dbread_pc(pc, files)\
    get_files(DBREAD_DATA, DBREAD_DATA,  READ_REQ, PC, NONE, NONE, pc, \
     NONE, NONE, NONE, NONE, NONE, files, 0)

#define dbread_wlpc(wl, flow, files) \
    get_files(DBREAD_DATA, DBNO_REQ,  READ_REQ, WL_PC, wl, flow, NONE, \
     NONE, NONE, NONE, NONE, NONE, files, 0)

#define dbrename_pc(old_pc, new_pc) \
    send_data(DBRENAME_DATA, DBNO_REQ,  PC, NONE, NONE, old_pc, new_pc, NONE, NONE)

#define dbrename_wlpc(wl, flow, new_pc) \
    send_data(DBRENAME_DATA, DBNO_REQ,  WL_PC, wl, flow, NONE, new_pc, NONE, NONE)


#define dbtransfer_pc(pc, node_name)\
    transfer_data(PC, NONE, NONE, pc, node_name)

#define dbtransfer_wlpc(wl, flow, node_name)\
    transfer_data(PC, wl, flow, NONE, node_name)

#define dbread_masks(wl, flow, design, user, machine, date1, date2, files) \
    get_files(DBRTV_VERSIONS, DBNO_REQ, GLOBAL_REQ, MASK, date1, date2, NONE,\
    wl, flow, design, user, machine, files, 0)


#define dbread_beta_rel(release, files) \
    get_files(DBREAD_BETA, DBNO_REQ, RELEASE_REQ, GLBL, release, NONE, NONE,\
NONE, NONE, NONE, NONE, NONE, files, 0)

#define dbread_off_rel(release, version, files) \
    get_files(DBREAD_OFFICIAL, DBNO_REQ, RELEASE_REQ, GLBL, release, version, NONE,\
NONE, NONE, NONE, NONE, NONE, files, 0)

#define dbrtv_maskshops(output_file) dbrtv_global_file("maskshops", output_file)

#define dbread_control_groups(output_file) dbrtv_global_file("control_groups", output_file)


#define dbps_df() \
    dbresend_data(DBPS_DF);
