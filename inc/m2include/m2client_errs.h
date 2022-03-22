#ifndef mask_client_errs_h
#define xx
/* Database client errors */

#include "global_client_errs.h"

#define reticle_ftp_msg \
"The reticle data has been transferred via FTP.  You will\n\
receive e-mail if problems with the transfer are found."

#define REG_ERR -504
#define AUTO_REG_ERR -505
#define BAD_FILE 0
#define rd_open_msg ""

/* Errors because user did not enter requested data */
#define blank_x_wl "Error -101:    Invalid existing fab name, ''."
#define BLANK_X_WL -101
#define blank_x_flow "Error -102:    Invalid existing process flow name, ''."
#define BLANK_X_FLOW -102
#define blank_x_design "Error -103:    Invalid existing design group name, ''."
#define BLANK_X_DESIGN -103
#define blank_mask "Error -104:    Invalid mask set name, ' '."
#define BLANK_MASK -104
#define blank_x_mask "Error -105:    Invalid existing mask set name, ''."
#define BLANK_X_MASK -105
#define blank_new_design "Error -106:    Invalid new design group name, ''."
#define BLANK_NEW_DESIGN -106
#define blank_wl "Error -107:    Invalid fab name, ''."
#define BLANK_WL -107
#define blank_flow "Error -108:    Invalid process flow name, ''."
#define BLANK_FLOW -108
#define blank_design "Error -109:    Invalid design group name, ''."
#define BLANK_DESIGN -109
/* Errors -110 to -123 are defined in global_client_errs */

/* Client network errors */
#define erase_wrn "\n\nWarning -117:  Unable to erase the following files(s) from your local disk:\n\n\
               '"
#define ERASE_WRN -117
#define bad_string_len "Error -120:  Received error code '%d' with retrieved string length of '%d'\n\n%s"
#define BAD_STRING_LEN -120
#define bad_hostname "Error -122:    Machine name not found in /etc/hosts."
#define BAD_HOSTNAME -122
#define dir_open_err "Error -123:    Unable to open directory:"
#define DIR_OPEN_ERR -123
#define tmpl_wrn "Warning -124:  Template created, but latest copy\n\
               of template list not retrieved."
#define TMPL_WRN -124
#define chkin_wrn "Warning -125:  Unable to find file:\n\n\
               '%s'\n\n\
Data was checked in without this file."
#define CHKIN_WRN -125
#define upd_wrn "Warning -126:  Unable to find file:\n\n\
               '%s'\n\n\
Data was updated without this file."
#define UPD_WRN -126
#define bad_request "Error -127:    Unable to read local file:\n\n\
               '%s'\n\n\
               Bad request type:  '%d'\n\n\
               %s"
#define BAD_REQUEST -127
#define nonexist2err "Override -131:  The following file no longer exists and\n\
                will not be checked into the Mask Designer Database.\n\n\
                '%s'\n\n\
                Press 'Continue' to proceed with check-in.\n\
                Press 'Cancel' to abort."
#define NONEXIST2ERR -131
#define blank_file_wrn "Warning -133:  A blank file was created for required file"
#define BLANK_FILE_WRN -133
#define bad_prefix "Error -134:  Database Client received file name with invalid prefix:"
#define BAD_PREFIX -134
#define server_err "Error -137:  Error code '%d' received from the database server."
#define SERVER_ERR -137
#define network_err "Error -138:  Network to database machine '%s' is unreachable.\n\n%s"
#define NETWORK_ERR -138
#define run_err "Error -139:  Database server on machine '%s' is not running.\n\n%s"
#define RUN_ERR -139
#define big_connect_err "Error -140:  Unable to connect to database servers:\n"
#define BIG_CONNECT_ERR -140
#define SEC_SERVER_WRN -141
#define rename_err "Error -143:  Unable to rename file:\n\n\
               '%s'\n\n               to \n\n               '%s'\n\n%s"
#define RENAME_ERR -143
#define db_write_err "Error -144:  Database server which owns the data is currently inaccessible.\n\
               Data owned by server on machine '%s'.  Read\n\
               transactions may be performed on alternate database servers."
#define DB_WRITE_ERR -144
#define db_dir_err "Error -149:    This transaction cannot be done because the '.db' directory is not present."
#define DB_DIR_ERR -149
#define filelist_err "Error -150:  this transaction cannot be done because the file list file is not present."
#define FILELIST_ERR -150
#define ovr_pc_wrn1 "Warning -150:  The following PC files are already present on your local disk:"
#define ovr_pc_wrn2 "\n                Press 'Continue to overwrite files.\n\
                Press 'Cancel' to abort."
#define OVR_PC_WRN -150

#define ftp_msg "Script created to send files to remote machine via ftp is in file:\n\n\
    '%s'\n\n\
Files sent via FTP are listed below."

#define dd_msg "Script created to transfer files to tape is in file:\n\n\
    '%s'\n\n\
Files found to write to tape are listed below."
#define OWN_WRN -424
#define ARCHIVE_WRN -624
#define ARES_ERR "Error -151:  Invalid input format.\n\n\
To assign a bump mask type '%s -a [mask] [bump mask] [pin]'\n\
To retrieve bump mask info type '%s -r [mask]'\n"

#define dbping_err "Error -151:  Invalid input format.\n\n\
    To access the Tempe database type  '%s tempe'\n\
    To access the Austin database type '%s austin'\n\
    To access the Tokyo database type  '%s tokyo'\n\
    To access the WACC database type   '%s wacc'\n"

#define ARES_MSG \
"      Welcome to ARES (Assign/Retrieve Software)\n\
      Free Scale Semiconductor Confidential Proprietary software\n\
        Not for distribution outside Free Scale\n\n\
To assign a bump mask type '%s -a [mask] [bump mask] '\n\
To retrieve bump mask info type '%s -r [mask]'\n"

#define dbping_msg \
"               Welcome to DBPing\n\
      Free Scale Semiconductor Confidential Proprietary software\n\
        Not for distribution outside Free Scale\n\n\
    To ping the Tempe database type  '%s tempe'\n\
    To ping the Austin database type '%s austin'\n\
    To ping the Tokyo database type  '%s tokyo'\n\
    To ping the WACC database type   '%s wacc'"
#define m2fab_wrn "Warning -161:  Fab '%s' does not exist in Mask Designer.\n\
               Because of this, you will not be able to build\n\
               jobdecks or orderforms for this maskset!"
#define M2FAB_WRN -161
#define m2fab_ovr \
   "\n\nOverride -162: Fab '%s' does not exist in Mask Designer.\n\
                You will not be able to build jobdecks or \n\
                order forms until a fab by this name is created.\n\
                  Are you sure you want to proceed to Move this \n\
                  mask set into the Mask Designer Database?\n\n\
                Press 'Continue' to proceed with convert.\n\
                Press 'Cancel' to abort."

#define M2FAB_OVR -162
#define no_fab_wrn "\n\nWarning -163:  Unable to find parent fab for mask set '%s'"
#define NO_FAB_WRN -163
#define checkin1ovr  \
"Override -164:  Mask set '%s' is not present in the Mask Designer Database, but\n\
                is present in the Mask Prep database, and was previously\n\
                ported to Mask Designer.\n\n\
                Press 'Continue' to proceed with check-in.\n\
                Press 'Cancel' to abort."
#define CHECKIN1OVR -164

#define checkin2ovr \
"Override -165:  Mask set '%s' is not present in the Mask Designer Database, but\n\
                is present in the Mask Prep database, and has been disabled.\n\n\
                Press 'Continue' to proceed with check-in.\n\
                Press 'Cancel' to abort."
#define CHECKIN2OVR -165
#define checkin3ovr \
"Override -166:  Mask set '%s' is not present in the Mask Designer Database.\n\n\
                Press 'Continue' to create the mask set.\n\
                Press 'Cancel' to abort."
#define CHECKIN3OVR -166
#define non_metadata_err "Error -167:    The PC library does not have the associated metadata directory:\n\n\
             %s\n\n\
               Check in cannot be done unless the metadata directory is present."
#define NON_METADATA_ERR -167
#define non2metadata_err "Error -168:    The PC library does not have the associated metadata directory:\n\n\
             %s\n\n\
               Library creation cannot be done unless the meta directory is present."
#define NON2METADATA_ERR -168
#define pc_local_dir_err "Error -169:    PC library '%s' not present in directory:\n\n\
               %s"
#define PC_LOCAL_DIR_ERR -169
               #define can_dlt_wrn "\n\nWarning -170:    Unable to delete PC library '%s'\n\
               from directory:\n\n\
               '%s'"
#define CAN_DLT_WRN -170
               #define can1dlt_wrn "Warning -171:    \n\nUnable to delete PC library metadata in directory\n\n\
               '%s'"
#define CAN1DLT_WRN -171
               #define can2dlt_wrn "\n\nWarning -172:    Unable to delete history file for:\n\n\
               PC library '%s'\n\n\
               from directory:\n\n\
               '%s'"
#define CAN2DLT_WRN -172
#define history_not_present "Error -173:    History file not present for:\n\n\
               PC Library '%s'"
#define HISTORY_NOT_PRESENT -173
#define dirlists_ovr "Override -174:  The following directories are no longer present on your local disk:\n\n\
%s\n\
Press 'Continue' to update the database without these directories.\n\
Press 'Cancel' to abort the transaction."
#define dirlist_ovr "Override -174:  The following directory is no longer present on your local disk:\n\n\
%s\n\
Press 'Continue to udpate the database without this directory.\n\
Press 'Cancel' to abort the transaction."
#define DIRLIST_OVR -174
#define zero_user_err "Error -175:  Invalid user ID with length of 0 passed in."
#define ZERO_USER_ERR -175
#define too_many_files \
"Error -176:    Transaction could not be performed because number of \n\
               files found exceeds number of files supported.\n\n\
	               Number of Files supported:  %d\n\
               Number of files found:      %d"
#define m2array_alloc_err "Error -172:    Array allocation error has occurred.\n\
               Directory list holds %d elements, but %d entries were added."
#define M2ARRAY_ALLOC_ERR -172
#define ccc_err "Error -177:    mds_ccc_update_db terminated because needed information not provided.\n\
               Usage:\n\n\
	               mds_ccc_update_db <UserID> <ccc summary file>\n\n"

#define reticle_ftp_err \
"Warning -173:  The reticle data has been transferred via FTP.  You will\n\
               receive an e-mail message with the output because possible \n\
               problems were found."
#define RETICLE_FTP_ERR -173
#define mask_name_err \
"Error -174:  Mask sets called 'mask' cannot be associated with a cap wafer."
#define MASK_NAME_ERR -174
#define itar_err \
"Error -175:    Data transfer to the mask shop is being done from the\n\
               NXP ITAR  machine,  medusa, but the data is not being\n\
               sent to an ITAR compliant mask shop.  If this is not ITAR\n\
               data,  perform the transfer from a different machine.  If\n\
               this is ITAR data, switch the mask shop to an ITAR mask shop\n\
               and then perform the transfer."
#define reticle_die_update_err "Error -176:    Error updating the reticle_die table.\n\n\
               Mask set %s\n\
               Die per row:  %d\n\
               Die per column:  %d\n\
               Die per shot:  %d\n\
               Program type: %d\n\n\
Error message:\n\n\
%s"

#endif /* ifdef for m2client_errs */
