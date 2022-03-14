#ifndef m2_server_arrays_h
#define m2_server_arrays_h

/* program names understood by the database */
static char m2programs [10][10] = 
{
    "opcsetup",
    "fabint",
    "mask2",
    "bumpmask",
    "hercules",
    "padlock",
    "shotmap", 
};

/* Data types understood by the DMI database */
static char data_array[24][16] =
{
/* Data types */
    "Mask",
    "Fab",
    "Off_template",
    "Template",
    "PC",
    "Partition",
    "PC history",
    "Metrics",
    "MACHINE",
    "GLOBAL",
    "Off_global",
    "REGISTER",
    "USERGROUP",
    "NETWORK",
    "FILES",
    "CRON",
    "VERSION",
    "GLOBAL_TABLES",
    "OFF_TMPL",
    "DB_ADMIN",
    "SERIAL",
    "SQL_QUERY",
    "CAP_WAFER",
};

/* Request types */
static char req_array[209][20] =
{
    /* Update requests */
    "Overwrite",
    "Checkin",
    "Update",
    "Save",
    "Delete",
    "Checkout",
    "Approve",
    "Cancheckout",
    "Transfer",
    "Read",
    "Rtv_versions",
    "Unapprove",
    "archive",
    "Unarchive",
    "Retrieve",
    "Read_any",
    "Retrieve1file",
    "Restore1mask",
    "C4set",
    "transaction log rtv",
    "Mod_owner ",
    "grant overwrite",
    "Create",
    "Copy",
    "Rename",
    "Shadow",
    "Versions",
    "History",
    "List",
    "Listall",
    "Delete_all",
    "Mod usergroup",
    "Status",
    /* retrieve data for mars */
    "Retrieve_mask_name",
    "Retrieve_h_name",
    "Retrieve_pc_name",
    /* retrieve serial numbers */
    "Retrieve_mos6serial ",
    "Rtvmos16serial  ",
    "Rtv_mos17serial ",
    "Rtv_mos21serial ",
/* retrieve annual contract number */
    "Retrieve_annual ",
    "Rtv_contract_num ",
    "Event log metrics",
    "MARS metrics",

    /* registration requests */
    "Check_email",
    "Check_pin",
    "Check_x500",
    "Reg_perform",
    "Manual_reg",
    "Remote_reg",
    "Resend_reg",
    "Pin_set",
    "Pin_get",
    "X500query",
     /* Remote request types */
    "Audit",
    "Off audit",
     "Refresh",
    "Mark",
    "Rmt create",
    "RmtDelete",
    "RmtRename",
    "Rmt reg",
     "Rmt audit",
    "Rmt audit state",
     "Rmt lock4archive",
    "Rmt lock4archive",
    "Rmt shadow check",


    /* retrieve one or more files */
    /* have transaction type rtv_files */
    "List members",
    "Retrieve_files ",
    "Rtv_name_phone",
    "Rtv mask info",
    "Rtv PC info",
    "Rtv RET password",

    /* MDSadmin requests */
    "Add usergroup",
    "Add user",
    "Unregister",
    "Add approver",
    "Add builder",
    "Add PC approver",
    "Add PC builder",
    "view CWD",
    "View users",
    "View usergroups",
    "admin check",
    "event log",

    /* network requests */
    "Audit_data ",
    "Resend data ",
    "Send weekly",
    "Send group",
    "Send admin",

    "Report list",
    "Report join",
    "append",
    "rtv RET & MSO pwd",

/* perform df and ps on database */
    "Dbps_df",
/* resend global files to remote servers */
    "Dbresend_files",
/* copy directory to remote servers */
    /* database administrative functions */
/* add template builder and template approver */
    "Mask projects rtv",
    "DTMS status rtv",
    "Add",
    "update reticle cnt",
    "Rmt_approver",
    "Put_global_file ",
    "Audit file",
    "Audit dir",
    "rmt audit file",
    "rmt audit dir",
    "checkout status rtv",
    "Common_layers update",
};

static char state_array[5][12] =
{
    "No action",
    "Read",
    "Checked out",
     "Update",
    "No action",
};

/* User types */
static char time_type_array[5][6] =
{
    "None",
    "On",
    "Since",
    "Range",
    "Prior"
};

static char user_type_array[4][8] =
{
    "None",
    "Creator",
    "Modify",
    "Owner"
};
#endif /* ifdef for m2server_arrays_h */
