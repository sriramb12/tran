#ifndef global_client_errs_h
#define global_client_errs_h
/* This file holds global errors used by all clients */
#define binary_err "Error -415:  Unable to set FTP transfer type to binary."
#define BINARY_ERR -415
#define NOT_REG -300
#define BAD_PIN -301

#define connect_err "Error -110:    Received error code '%d' while trying\n\
               to connect to database server:\n\n\
               Machine:  %s\n\
               Address:  %s\n\n"
#define ssl_connect_err "Error -110:    Received error code '%d' while trying\n\
               to connect to database server:\n\n\
               Machine:  %s\n\
               Address:  %s\n\
               Port:  %d\n\n"
#define DIAGNOSTICS "Diagnostics:\n\n\
               Verify the correct IP node address for the\n\
               database is in your '.MaskPreprc' file.\n\
               Verify that the database machine is accessible.\n\n"
#define CONNECT_ERR -110
#define bad_sock_open "Error -111:      Received error code '%d' while trying to create \n\
              local socket structure."
#define BAD_SOCK_OPEN -111
#ifdef INTERCOM
#define bad_get \
"Error -112:   Received error code '%d' while retrieving data from the\n\
               server.  This error is usually caused by a temporary network\n\
               problem.  Please try your transaction again.\n\n\
               %s"
#else
#define bad_get "Error -112:  Received error code '%d' while retrieving data \n\
               from the database server.\n\n\
          This error is usually caused by a temporary network\n\
               problem.  Please wait a few minutes and try your \n\
               database request again.\n\n\
               %s"
#endif
#define BAD_GET -112
#ifdef INTERCOM
#define bad_put \
"Error -113:   Received error code '%d' while attempting to send data to the\n\
               server.  This error is usually caused by a temporary network\n\
               problem.  If you are uploading data, try transferring the\n\
               data using the -ps option.  If you are downloading data, try\n\
               retrieving data using the -gs option.  These options perform\n\
               serial transfer so time outs are less likely to occur.\n\n\
               %s"
#else
#define bad_put "Error -113:  Received error code '%d' while attempting to send\n\
               data to the database server.\n\n\
          This error is usually caused by a temporary network\n\
               problem.  Please wait a few minutes and try your \n\
               database request again.\n\n\
               %s"
#endif
#define BAD_PUT -113
#define bad_create "Error -114:    Received error code %d while trying to create file:\n\n\
               '%s'\n\n\
               This error usually occurs when you are running \n\
               %s in a directory where you do not have \n\
               write permission.  It may also occur if your \n\
               disk is full.\n\n%s"
#define BAD_CREATE -114
#define bad_rd_open "Error -115:    Unable to open for read file:"
#define bad_rd_open_msg \
"               This error usually occurs when the file being\n\
               accessed does not exist or when you do not\n\
               have read access on the file.\n\n\
For assistance contact the DMI database administrator:\n\n\
               Name:  Transcend Team\n\
               Email:  transbug@nxp.com"
#define BAD_RD_OPEN -115
#define bad_write "Error -116:    Unable to write to file:"
#define BAD_WRITE -116
#define erase_wrn "\n\nWarning -117:  Unable to erase the following files(s) from your local disk:\n\n\
               '"
#define ERASE_WRN -117
#define overwrite_dsgn "\n\nWarning -118:  The design files in your working directory were over-\n\
               written with the latest version from the database."
#define OVERWRITE_DSGN -118
#define overwrite_tmpl "\n\nWarning -119:  The template files in your working directory were over-\n\
               written with the latest version from the database."
#define OVERWRITE_TMPL -119
#define bad_string_len "Error -120:  Received error code '%d' with retrieved string length of '%d'\n\n%s"
#define BAD_STRING_LEN -120
#define bad_file_get "Error -121:    Unable to retrieve file name from the database server."
#define BAD_FILE_GET -121
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
#define malloc_err "Error -128:  Unable to allocate dynamic memory."
#define MALLOC_ERR -128
#define bad_ip_addr "Error -129:  Invalid IP node address for server machine:"
#define BAD_IP_ADDR -129
#define no_files_err "Error -130:  No files found to upload."
#define NO_FILES_ERR -130
#define nonexist_err "Warning -131:  The following file no longer exists and\n\
               will not be checked into the database.\n\n\
               '%s'\n\n\
               Press 'Continue' to proceed with check-in.\n\
               Press 'Cancel' to abort."
#define NONEXIST_ERR -131
#define nonexist_wrn "Warning -132:  The following file(s) no longer exist and\n\
               were not checked into the database.\n\n\
               '%s'\n\n"
#define NONEXIST_WRN -132
#define bad_prefix "Error -134:  Database Client received file name with invalid prefix:"
#define BAD_PREFIX -134
#define bad_filelist "Overwrite -135:  File list does not contain required file:\n\n\
               '%s%s%s'\n\n\
Press 'Continue' to put blank file in database.\n\n\
Press 'cancel' to abort action."
#define BAD_FILELIST -135
#define bad_suffix "Warning -136:  File with invalid suffix not transferred to the database."
#define BAD_SUFFIX -136
#ifdef INTERCOM
#define server_err \
"Error -112:   Received error code '%d' while retrieving data from the\n\
               server.  This error is usually caused by a temporary network\n\
               problem.  If you are uploading data, try transferring the\n\
               data using the -ps option.  If you are downloading data, try\n\
               retrieving data using the -gs option.  These options force\n\
               serial data transfer.\n\n\
               %s"
#else
#define server_err "Error -137:  Error code '%d' received from the database server."
#endif
#define SERVER_ERR -137
#define network_err "Error -138:  Network to database machine '%s' is unreachable.\n\n%s"
#define NETWORK_ERR -138
#define run_err "Error -139:  Database server on machine '%s' is not running.\n\n%s"
#define RUN_ERR -139
#define big_connect_err "Error -140:  Unable to connect to database servers:\n"
#define BIG_CONNECT_ERR -140
#define sec_server_wrn "\n\nWarning -141:  Data retrieved from secondary database on machine:\n\n\
               '"
#define SEC_SERVER_WRN -141
#define sec_own_wrn "Warning -142:  Data accessed owned by secondary database on machine:\n\n\
               '"
#define FILE_MUST_BE_ENCRYPTED  -777
#define file_must_be_encrypted_err \
"Error -777:    File %s being sent to ITAR must be already encrypted.\n\
Please use 'gpg' or similar command to encrypt the file.\n\
If it is directory, first create a tarball then encrypt it.\n\
Data transfer has been aborted.\n\n"

#define SEC_OWN_WRN -142
#define bad_rename "Error -143:  Unable to rename file:\n\n\
               '%s'\n\n               to \n\n               '%s'\n\n%s"
#define BAD_RENAME -143
#define db_write_err "Error -144:  Database server which owns the data is currently inaccessible.\n\
               Data owned by server on machine '%s'.  Read\n\
               transactions may be performed on alternate database servers."
#define DB_WRITE_ERR -144
#define db_own_err "Error -145:  None of the databases accessed own the requested data."
#define DB_OWN_ERR -145
#define crt_dir_err "Error -146:    Unable to create directory \n\n               '"
#define CRT_DIR_ERR -146
#define bad_wr_open "Error -147:    Unable to open for write file:"
#define BAD_WR_OPEN -147
#define bad_read "Error -148:    Unable to read file:"
#define BAD_READ -148
#define essential_err "Error -149:    Essential file '%s' is not present.\n\
               This file must be present to proceed with check-in."
#define ESSENTIAL_ERR -149
#define ovr_wrn1 "Warning -150:  The following files are already present on your local disk:"
#define ovr_wrn2 "\n               Press 'Continue to overwrite files.\n\
               Press 'Cancel' to abort."
#define OVR_WRN -150
#define no_fab_files "Error -151:  No fab files found to send to database."
#define NO_FAB_FILES -151
#define command_open4write_err "Error -152:    Unable to open for write command:"
#define COMMAND_OPEN4WRITE_ERR -152
#define command_open4read "Error -153:    Unable to open for read command:"
#define COMMAND_OPEN4READ -153
#define command_write_err "Error -154:    Unable to write to command:"
#define COMMAND_WRITE_ERR -154
#define command_read_err "Error -155:    Unable to read from command:"
#define COMMAND_READ_ERR -155
#define bad_files "\n\nWarning -156:  The following objects were not found and were not send:\n\n"
#define BAD_FILES -156
#define format_err "Error -157:  Invalid format entered.\n\n"
#define FORMAT_ERR -157
#define invalid_pin "Error -158:  Two entries enter for new PIN do not match."
#define INVALID_PIN -158
#define no_valid_files "Error -159:  No valid file names specified."
#define NO_VALID_FILES -159
#define dlt_dir_err "Error -160:    Received error code %d while deleting directory:\n\n\
               '%s'\n\n%s"
#define DLT_DIR_ERR -160
#define file_dlt_err "Error -161:    Unable to delete file:"
#define FILE_DLT_ERR -161
#define dir_access_err "Error -162:  Unable to access directory:"
#define DIR_ACCESS_ERR -162
#define access_err "Error -162:  Unable to access:"
#define ACCESS_ERR -162
#define bad_write_open "Error -163:    Unable to open for write remote file:"
#define bad_rmt_write "Error -164:    Unable to write to remote FTP file:"
#define maskshop_err "Error -165:    Unable to find information for Mask Shop:"
#define ftp_connect_err "Error -166:    Unable to FTP to Mask Shop:"
#define bad_stat \
"Error -167:    Received error code %d while performing stat on\n\
               specified file.  This error usually occurs when \n\
               the file is not present.\n\n\
File name:     '%s'"
#define BAD_STAT -167
#define bad_link "Error -168:  Received error code %d while linking %s to %s"
#define BAD_LINK -168
#define bad_tar "Error -169:    Unable to open tar command:"
#define bad_rmt_delete "Error -170:    Unable to delete remote FTP file:"
#define bad_rmt_chdir "Error -171:    Unable to change to remote FTP directory:"

#define array_alloc_err "Error -172:    Array allocation error has occurred.\n\
               Array holds %d elements, but %d entries were added."
#define ARRAY_ALLOC_ERR -172
#define dlt_dir_wrn "\n\nWarning -173:    Received error code %d while deleting directory:\n\n\
               '%s'\n\n%s"
#define DLT_DIR_WRN -173
#define file_dlt_wrn "Warning -174:    Unable to delete file:"
#define FILE_DLT_WRN -174
#define invalid_send2maskshop_input  "Error -175:  Invalid input passed to 'send2maskshop'.\n\
               Parameters passed in:\n\n"

#define path_env_err "Error -175:  No environment variable present for IcomSend"
#define permission_create_err "Error -176:    Received error code %d while trying to create file:\n\n\
               '%s'\n\n\
               This error occurs when you are running \n\
               %s in a directory where you do not have \n\
               write permission.\n\n%s"
#define PERMISSION_CREATE_ERR -176
#define permission_write_err "Error -177:    Received error code %d while trying to write to file:\n\n\
               '%s'\n\n\
               This error occurs when you are running \n\
               %s in a directory where you do not have \n\
               write permission.\n\n%s"
#define PERMISSION_WRITE_ERR -177

#endif /* ifdef for global_client_errs_h */
