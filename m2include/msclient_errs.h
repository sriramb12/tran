#ifndef msclient_errs_h
#define msclient_errs_h
/* Defines for Mask Shop client errors follow */

#define ms_connect_err "Error -210:  Received error code '%d' while trying to connect to mask shop server:\n\
               Requested connection to:\n\n\
               Machine: %s\n\
               Socket number:  %d\n\
               IP node address: %s\n\n%s"
#define MS_CONNECT_ERR -210
#define bad_ms_sock_open "Error -211:    Unable to establish communication with Mask Shop server."
#define BAD_MS_SOCK_OPEN -211
#define bad_ms_get "Error -212:    Unable to retrieve data from the Mask Shop server."
#define BAD_MS_GET -212
#define bad_ms_put "Error -213:    Unable to send data to the Mask Shop server."
#define BAD_MS_PUT -213
#define rexec_err "Error -214:    Unable to connect to remote host:\n\n\
               Machine Name:  %s\n\
               Login name:  %s\n\n\
For assistance, please contact your system administrator."
#define REXEC_ERR -214
#define rexec_wr_err "Error -215:    Unable to write to file on remote host:\n\n\
               Machine Name:  %s\n\
               Login name:  %s\n\
               File name:  %s\n\n\
For assistance, please contact your system administrator."
#define REXEC_WR_ERR -215
#define bad_ms_rd_open "Error -220:    Unable to open for read file:"
#define BAD_MS_RD_OPEN -220
#define bad_ms_wr_open "Error -221:    Unable to open for write file:"
#define BAD_MS_WR_OPEN -221
#define bad_ms_write "Error -222:    Unable to write to file:"
#define BAD_MS_WRITE -222
#define bad_ms_string_len "Error -223:    Bad string length of"
#define BAD_MS_STRING_LEN -223
#define bad_ms_hostname "Error -224:    Unable to find Mask Shop server machine name:"
#define BAD_MS_HOSTNAME -224
#define data100_wrn "\n\nWarning -225:  No data100 files available to send to the Mask Shop."
#define DATA100_WRN -225
#define ibm_wrn "\n\nWarning -226:  No indication that file(s) were successfullly\n\
               sent to the Mesa Mask Shop has been received.\n\
               Client will check for additional reqults every\n\
               30 seconds for the next half hour."
#define IBM_WRN -226
#define no_dd_files "Error -228:    No mask prep  files present to write to tape."
#define NO_DD_FILES -228
#define no_cntrl_files "\nError -229:    No mask set control files present for mask set '"
#define NO_CNTRL_FILES -229
#define no_order_files "\nWarning -230:    No order form files present for mask set '"
#define NO_ORDER_FILES -230
#define no_etec_files "\nWarning -231:    No etec files present for mask set '"
#define NO_ETEC_FILES -231
#define no_contract_files "\nWarning -232:    No contract release files present for mask set '"
#define NO_CONTRACT_FILES -232
#define no_deck_files "\nWarning -233:  No job deck files present for mask set '"
#define NO_DECK_FILES -233
#define files_not_fnd "Warning -233:  One or more files specified in the control files \n\
               were not present in the current working directory."
#define FILES_NOT_FND -233
#define send_wrn "\nWarning -234:  Unable to read file(s):"
#define SEND_WRN -234
#define ms_network_err "Error -235:  Network to Mask Shop machine is unreachable.\n\
               Connection requested to:\n\n\
               Machine: %s\n\
               Socket number:  %d\n\
               IP node address: %s\n\n%s"
#define MS_NETWORK_ERR -235
#define ms_run_err "Error -236:    Connection to Mask Shop Server refused.\n\
               Connection requested to:\n\n\
               Machine: %s\n\
               Socket number:  %d\n\
               IP node address: %s\n\n%s"
#define MS_RUN_ERR -236
#define draw_wrn "\nWarning -237:  Unexpected code in start drawing field - etec file generation aborted.\n"
#define DRAW_WRN -237
#define trapezoid_wrn "Warning -238:  Unexpected trapezoid type %d %x\n\
               '%d' records read from input file\n\
               Word number '%d'\n"
#define TRAPEZOID_WRN -238
#define eof_wrn "\nWarning -239:  Unexpected enf of file found - etec file generation aborted.\n"
#define EOF_WRN -239
#define format_wrn "\nWarning -240:  Bad etec file name format found - etec file generation aborted.\n"
#define FORMAT_WRN -240
#define deck_ovr "Warning -241:  No job deck files present for mask set '"
#define order_ovr "Warning -242:  No order form files present for mask set '"
#define contract_ovr "Warning -243:  No contract release forms present for mask set '"
#define etec_ovr "Warning -244:  No etec files present for mask set '"
#define all_ovr "Files to send are listed below:\n\n\
               Press 'Send Data' to proceed.\n\
               Press 'Cancel' to abort."

#define JOBDECK_MSG "    (Job decks have not been generated in this directory.)\n\
    (Therefore, no job decks from this directory will be sent.)\n\n"
#define ORDER_MSG "    (Order forms have not been generated in this directory.)\n\
    (Therefore, no order forms will be sent.)\n\n"
#define CONTRACT_MSG "    (Contract release forms have not been generated in this directory.)\n\
    (Therefore, no contract release forms will be sent.)\n\n"
#define ETEC_MSG "    (etec files have not been generated in this directory.)\n\
    (Therefore, no etec files from this directory will be sent.)\n\n"
#define PATTERN_MSG "    (Etec pattern files have not been generated in this directory.)\n\
    (Therefore, no etec files from this directory will be sent.)\n\n"
#define no_dupont_files "Error -250:  No files present to send to Dupont."
#define NO_DUPONT_FILES -250
#define no_mesa_files "Error -251:  No files present to send to the Mesa Mask Shop."
#define NO_MESA_FILES -251
#define no_ftp_files "Error -252:  No files present to send via FTP."
#define NO_FTP_FILES -252
#define no_tape_files "Error -253:  No files present to write to tape."
#define NO_TAPE_FILES -253
#define access_err "Error -280:    Unable to access file:\n\n\
               '%s'"
#define ACCESS_ERR -280
#endif /* ifdef for msclient_errs_h */
