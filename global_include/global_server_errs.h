#ifndef global_server_errs_h
#define global_server_errs_h
/* The error messages defined in this file are used by all of the DMI databases. */
/* These errors are between -300 and -399 */
#define BAD_GET -112
/* Defines for database errors follow */

#define not_reg "Error -300:    Registration entry not found for '%s'"
#define NOT_REG -300
#define bad_pin "Error -301:    Incorrect PIN entered."
#define BAD_PIN -301
#define bad_arg "Error -302:    Invalid option '%s'\n\n\
               Use '-p' for production, '-t' for test,\n\
               and '-d' for development DMI server.\n\n"
#define BAD_ARG -302
#define bad_rmt_sock_open "Error -304:    Unable to establish communication with remote database server."
#define BAD_RMT_SOCK_OPEN -304
#define bad_rmt_get "Error -305:    Unable to retrieve data from the database server."
#define BAD_RMT_GET -305
#define bad_rmt_put "Error -306:    Unable to send data to the database server."
#define BAD_RMT_PUT -306
#define malloc_err "Error -307:  Unable to allocate dynamic memory."
#define MALLOC_ERR -307
#define bad_rmt_ip_addr "Error -308:  Invalid IP node address for server machine:"
#define BAD_RMT_IP_ADDR -308
#define bad_rmt_connect "Error -309:  Received error code %d while trying to connect to remote server '%s'"
#define BAD_RMT_CONNECT -309
#define rmt_connect_err "Error -310:    Received error code '%d' while trying\n\
               to connect to database server:\n\n\
               Machine:  %s\n\
               Address:  %s\n\n"
#define RMT_CONNECT_ERR -310

/* Database server network errors */
#define put_short_err "Error -311:    Database server received error code '%d'\n\
               while sending length field to the client or remote server."
#define PUT_SHORT_ERR -311
#define put_char_err "Error -312:    Database server received error code '%d'\n\
               while sending character to the client or remote server."
#define PUT_CHAR_ERR -312
#define put_err "Error -313:    Database server received error code '%d'\n\
               while sending data to the client or remote server.\n\n\
               Attempted to send '%d' bytes.\n\
               But sent only '%d' bytes."
#define PUT_ERR -313
#define bad_ack "Error -314:    Database server did not receive final\n\
               acknowledgement from the client."
#define BAD_ACK -314
#define bad_file_ack "Error -315:    Database server did not receive end-of-file\n\
               acknowledgement from the client."
#define BAD_FILE_ACK -315
#define get_short_err "Error -316:    Database server retrieved invalid\n\
               length field of '%d' with error code '%d' from the client."
#define GET_SHORT_ERR -316
#define get_err "Error -317:    Database server received error code '%d'\n\
               while trying to retrieve data from the user."
#define GET_ERR -317
#define bad_rmt_ack "Error -318:  Server unable to retrieve acknowledgement\
               from the remote server."
#define BAD_RMT_ACK -318

#define no_disk_space "Error -319:    There is not enough disk space to perform\
               database transaction."
#define NO_DISK_SPACE -319
#define bad_vnum "Error -320:    Invalid database client version number:\n\n\
               '%s'\n\n\
               Lowest version of client that can run with database server '%s' is '%s'.%s%s"
#define BAD_VNUM -320

/* Invalid data sent by user */
#define bad_req_type  "Error -321:    Invalid request type"
#define BAD_REQ_TYPE -321
#define bad_trans_type  "Error -322:    Invalid transaction type"
#define BAD_TRANS_TYPE -322
#define bad_data_type  "Error -323:    Invalid data type"
#define BAD_DATA_TYPE -323
/* Empty slot -324 */
#define bad_version_flag "Error -325:    Invalid version flag"
#define BAD_VERSION_FLAG -325
#define bad_row_index "Error -326:    Invalid state table row index value"
#define BAD_ROW_INDEX -326
#define bad_col_index "Error -327:    Invalid state table column index value"
#define BAD_COL_INDEX -327
#define bad_state_val "Error -328:    Invalid state table value"
#define BAD_STATE_VAL -328
#define bad_addr "Error -329:    Invalid IP node address:"
#define BAD_ADDR -329
#define bad_password "Error -330:    Internal password corrupted during data transmission.  "
#define bad2password "Error -330:    Internal password corrupted during data transmission.  \n\
               User password:  %s\n\
               Server password:  %s"
#define BAD_PASSWORD -330
/*
#define corrupted_data "Error -331:  Corrupted data found in file:"
#define CORRUPTED_DATA -331
*/
#define checksum_err "Error -332:  Inconsistent check sums found on the %s database for directory:\n\n\
             '%s'"
#define CHECKSUM_ERR -332
#define not_auth_err "Error -333:    You are not authorized to perform database \n\
               administrative functions."
#define NOT_AUTH_ERR -333
#define invalid_password "Error -334:  The password you entered is incorrect."
#define INVALID_PASSWORD -334
#define alr_own_err "Error -334:  %s already owned by:\n\n\
               Site:  '%s'"
#define ALR_OWN_ERR -334
/* Empty slot -335 */

#define origin_blacklist_err "Error -336:    You are not authorized to perform this request."
#define ORIGIN_BLACKLIST_ERR -336

#define pc_flow_err "Error -337:    Process flow %s must be created before %s\n\
               can be created."
#define PC_FLOW_ERR -337
/* Empty slot -338 -339 */

/* Server directory and table of contents  errors */
#define mkdir_err "Error -340:    Received err code %d while trying to create directory:\n\n\
              '%s'"
#define MKDIR_ERR -340
#define dlt_dir_err "Error -341:    Received err code '%d' while trying to delete directory:\n\n\
               '%s'"
#define DLT_DIR_ERR -341
#define open_dir_err "Error -342:    Received err code %d while trying  to open directory:\n\n\
               '%s'"
#define OPEN_DIR_ERR -342
#define bad_dir_name "Error -343:    Invalid directory name of 0 length."
#define BAD_DIR_NAME -343
/* Empty slot 344 */
#define rmt_ovr "\n\nWarning -345:  %s retrieved, but still being updated by the remote\n\
               server.  Thus, you might not have the latest copy of the data.\n\n%s"
#define RMT_OVR -345
#define toc_err "Error -346:    Table of contents entry already present for:"
#define TOC_ERR -346
#define sys_write_err "Error -347:    Unable to execute system command:"
#define SYS_WRITE_ERR -347
#define lock_err "Error -348:    Unable to create lock file:"
#define LOCK_ERR -348
#define unlock_err "Error -349:    Unable to delete lock file:"
#define UNLOCK_ERR -349
#define no_valid_files "Error -349:    No valid files present to send to the database."
#define NO_VALID_FILES -349

/* Server file errors */
#define rd_open_err "Error -350:    Unable to open for read database file:"
#define RD_OPEN_ERR -350
#define wr_open_err "Error -351:    Unable to open for write database file:"
#define WR_OPEN_ERR -351
#define read_err "Error -352:    Unable to read database file:"
/* Currently not being used */
#define READ_ERR -352
#define write_err "Error -353:    Unable to write to database file:"
#define WRITE_ERR -353
#define seek_err "Error -354:  Unable to perform seek on database file:"
#define SEEK_ERR -354
#define rename_err "Error -355:    Unable to rename database file:"
#define RENAME_ERR -355
#define dlt_err "Error -356:    Received error '%d' while deleting file:\n\n\
               '%s'"
#define DLT_ERR -356
/* global_server_errs.h Not currently being used */
#define reg_req "Warning -359:  Requesting registration on %s for:\n\n\
               %IP node address:  %s\n\
               Login:  %s\n\
               Email address:  %s\n\
               User ID:  %s\n\
               Name:  %s\n"
#define REG_REQ -359
#define reg_core_err "Error -358:  Your registration request failed because no entry was  found\n\
             for  you in the NXP email core directory.  For security\n\
             reasons, all users must be  registered  in  the  email  core\n\
             directory before they can register to use this tool.  If you\n\
             are already registered in the email core  directory,  please\n\
             try  registering  again and be sure to type in your official\n\
             Email address at the prompt.\n\n\
             If you are not  registered  in  the  email  core  directory,\n\
please contact the help desk.\n\n\
             After you are registered in the email  core  directory,  you\n\
             can  register  to use this tool by running the tool.  If you\n\
             have any questions or  problems  with  registration,  please\n\
             contact Transcend team - transbug@nxp.com."
#define REG_CORE_ERR -358
#define reg_match_err "Error -360:  Registration failed because your name in the  password  file\n\
             does  not  match  your  name  in  the  NXP  email  core\n\
             directory.   For  security  reasons,  the  DMI  registration\n\
             software  requires  these  names to match.  \n\n\
                 Name in password file:  %s %s\n\
                 Name in core email directory:  %s %s\n\n\
             If your name in the password file is incorrect, contact your\n\
             system  administrator  to correct the problem.  If your name\n\
             in the email core directory is  wrong,  contact  your  local\n\
             email  administrator  to  fix  the  problem.   If  you  have\n\
             questions, contact Transcend team - transbug@nxp.com."
#define REG_MATCH_ERR -360
/* Empty slots    364 365 366 367 368 369 */
#define reg_verify_err "Error -361:  Unable to verify registration for %s."
#define REG_VERIFY_ERR -361
#define auto_reg_err "Error -362:    Unable to perform registration for %s\n\
               on the %s database.\n\n\
               Your registration request has been sent to the\n\
               DMI Database administrator.\n\n\
To insure that your registration request is processed, please\n\
send to Transcend team (transbug@nxp.com) the following information:\n\n\
-   Your full name\n\
-   Your official NXP ID or badge\n\
-   Your telephone number\n\
-   Your department number\n\
-   Your location code\n\
-   Your mail drop"
#define AUTO_REG_ERR -362
#define man_reg_err "Error -363:    Unable to register user '%s %s'\n\
               on the '%s' database."
#define MAN_REG_ERR -363
#define invalid_subdir "Error -365:  Invalid directory '%s' found"
#define INVALID_SUBDIR -365
#define empty_dir "Error -366:  Empty directory '%s' found"
#define EMPTY_DIR -366
#define extra_entry_err "Error -367:  Entry '%s' found and deleted from '%s'"
#define EXTRA_ENTRY -367
#define accept_err "Error -368:  Received error code %d while attempting to accept a connection."
#define ACCEPT_ERR -368
#define ssl_accept_err "Error -369:  Received error code %d while attempting to accept a connection."
#define SSL_ACCEPT_ERR -369

#define crt_err "Error -380:    %s already exists."
#define CRT_ERR -380
#define non_err "Error -381:    %s does not exist."
#define machine_non_err "Error -381:    %s do not exist."
#define NON_ERR -381
/* Empty slots 382 383 384 385 386 387 388 389 390 */
#define no_archive_space "Error -446:  No disk space present for archive."
#define NO_ARCHIVE_SPACE -446
#define usr_not_fnd "Error -447:    Unable to find registration entry \n\
               for '%s'"
#define USR_NOT_FND -702
#define ftp_connect_err "Error -710:    Received error code %d while connecting to \n\n\
               %s"
#define FTP_CONNECT_ERR -710
#define invalid_reg_format "Error -411:    Invalid format found in registration file '%s\n\
               for line %d which reads '%s'"
#define bad_file_sock "Error -412:  Unable to obtain socket for file transfer."
#define BAD_FILE_SOCK -412
#define INVALID_REG_FORMAT -411
#define reg_entry_wrn "Warning -413:  Neither core ID nor Transcend registration entry\n\
               present for requested user.  No email sent to:"
#define REG_ENTRY_WRN -413
#define ascii_err "Error -414:  Unable to set FTP transfer type to ASCII."
#define ASCII_ERR -414
#define binary_err "Error -415:  Unable to set FTP transfer type to binary."
#define BINARY_ERR -415
#define abort_err "Error -416:    Data transfer aborted for file:\n\n\
               '%s'"
#define ABORT_ERR -416
#define df_err "Error -417:  Unable to determin disk usage using 'df' command"
#define DF_ERR -417
#define disk_space_err "Error -418:  There is not enough room in the %s repository for the data.\n\n\
               File Size:  %d bytes\n\
               Available space:  %d bytes\n\
               Percent used:  %d%%"
#define not_fnd_err "Error -418:    Entry for %s not found."
#define NOT_FND_ERR -418

#define dup_entry "Error -420:    Duplicate entry found for:"
#define DUP_ENTRY -420
#define bad_toc "\n\nCaution -422:    Unable to find and delete:\n\n\
               Global entry:  "
#define BAD_TOC -422
#define own_wrn "Warning -424:  Data base request sent to owner: '"
#define OWN_WRN -424
#define ARCHIVE_WRN -624


#define VERSION_NOT_FND -327

#define bad_len_err "Error -445:  Bad error code of '%d' received from the client or remote server."
#define BAD_LEN_ERR -445
#define dlt_dir_cau "\n\nCaution -444:  Unable to delete directory '%s'\n\
               This directory has been renamed '%s'"
#define DLT_DIR_CAU -444
#define not_owner "Error -414:  You cannot modify data you do not own.  Data is owned by:\n\n\
               Login:  '%s'\n\
               Email address:  '%s'\n\
               IP node Address: '%s'"
#define NOT_OWNER -414

#define no_data_err "Error -425:   No data found in database for version '%2.3f' of:\n\n\
               %s"
#define NO_DATA_ERR -425
#define timeout_err "Error -426:  Timed out while trying to connect to remote server."
#define TIMEOUT_ERR -426
#define mask_present \
"Error -480:    %s is already present in the MaskPrep database.\n\
               You must disable the maskset in the MaskPrep database before\n\
               creating it in the Mask Designer database.  Use the 'Disable Mask\n\
               Set' function found under the 'MaskPrep Access' menu!"
#define MASK_PRESENT -480
#define invalid_user_id "Error -485:    Invalid NXP user ID of '%s' entered.  NXP\n\
               user ID's must be 5 or more characters in length."
#define INVALID_USER_ID -485
#define date_err "Error -486:    Invalid date entered.  Dates must\n\
               follow the format mm/dd/yy."
#define DATE_ERR -486
#define month_range_err "Error -487:    Specified month '%0.2d' out of range.\n\
               Valid months are 01-12."
#define MONTH_RANGE_ERR -487
#define day_range_err "Error -488:    Specified day '%0.2d' out of range.\n\
               Valid days are 01-31."
#define DAY_RANGE_ERR -488
#define year_range_err "Error -489:    Specified day '%0.2d' out of range.\n\
               Valid days are 01-31."
#define YEAR_RANGE_ERR -489
#define year_err "Error -490:    The system currently only allows reports \n\
               with the same year on both the start and end date."
#define YEAR_ERR -490
#define bad_dates "Error -491:  NO data for specified dates found."
#define BAD_DATES -841
#define month_err "Error -492:  Ending month must be greater than or equal\n\
               to starting month.\n\n\
               Starting month:  %0.2d\n\
               Ending month:  %0.2d"
#define MONTH_ERR -492
#define YEAR_RANGE_ERr -839
#define no_weekly_data "Error -493:  No data present for specified dates."
#define NO_WEEKLY_DATA -493
#define delete_err "Error -495:    Received error %d while trying to delete:\n\n\
               %s"
#define DELETE_ERR -495
#define fss_err "Error -496:  You are not authorized to transfer, retrieve, or access  Foundry Services Customer data"
#define FSS_ERR -496

#define no_data "Error -725:  No files found for specified version of data."
#define TRANS_NO_DATA -725
#define    authen_err "Error -726:    OneIT authentication failed."
#define AUTHEN_ERR -726
#define convert_ovr "Override -727: Received error code %d while trying to convert \n\
               data to specified version.\n\n\
                Press 'Continue' to retrieve unusable data for evaluation.\n\
                Press 'Cancel' to abort."
#define CONVERT_OVR -727
#define chmod_err "Error -728:  Unable to set execute permission on job script"
#define CHMOD_ERR -728
#define convert_err "Error -729:  %s was not converted correctly \n\
               and this data is unusable.  Conversion\n\
               output is stored in file '.db/convert.output'"
#define CONVERT_ERR -729
#define convert_wrn "Warning -730:  Received return status %d while trying to convert \n\
               data to specified version."
#define CONVERT_WRN -730
#define pclose_err "Error -731:  Received error %d while closing tar command"
#define PCLOSE_ERR -731
#define decrypt_err "Error -732:    Unable to decrypt file:"
#define DECRYPT_ERR -732
#define file_sent_err \
"Error -733:    File '%s' has already been sent to the\n\
               mask shop.  Duplicate file names cannot be sent.  Because of\n\
               this problem, data transfer to the mask shop was aborted."
#define FILE_SENT_ERR -733
#define no_version_history "Error -734:  No version history present for %s"
#define NO_VERSION_HISTORY -734
#define bad_link "Error -735:  Received error code %d while linking %s to %s"
#define BAD_LINK -735
#define tech_err "Error -736:  Technology %s is not defined\n"
#define TECH_ERR -736
#define sql_err "Error -737:    Received error code %d from mySql query:\n\n\
               '%s'\n\n\
               Error string returned is:\n\n\
               '%s'"
#define SQL_ERR -737
#define mask_project_list_err "Error -738:    Unable to find project name for mask set '%s'"
#define MASK_PROJECT_LIST_ERR -837
#define mwr_signoff_err "Warning -739:  Final sign off not completed for MWR number '%s'\n\n"
#define MWR_SIGNOFF_ERR -739
#define no_dtms_signoff_wrn "Warning -740:  No DTMS projects defined for mask set '%s'.\n\n"
#define command_open_err "Error -741:    Received error code %d while executing command:\n\n\
               '%s'"
#define COMMAND_OPEN_ERR -741
#define mwr_signoff_done "Final sign off for MWR number %s was completed on %s.\n\n"
#define dtms_not_needed "DTMS sign off not needed for this mask set."
#define DTMS_NOT_NEEDED -741
#define certificate_err "Error -742:    Unable to  read certificate file:\n\n\
               %s"
#define CERTIFICATE_ERR -742
#define key_file_err "Error -743:   Received error code '%d' while reading certificate file:\n\n\
               '%s'"
#define KEY_FILE_ERR -743
#define ca_list_err "Error -744:  Unable to  read CA list"
#define CA_LIST_ERR -744
#define expired_core_id \
"Error -745:    The authentication token for your e-mail address has expired.\n\
               Please perform authentication again to renew the token."
#define EXPIRED_CORE_ID  -745
#define extract_err \
"Error -980:  Unable to extract data_transfer registry"
#define EXTRACT_ERR -980
#define domain_approve_err \
"Error -981:    Approval request not performed because the domain for\n\
               '%s' is already approved."
#define DOMAIN_APPROVE_ERR -981
#define pending_request_err \
"Error -982:    Pending request not present for:\n\n\
               '%s'"
#define PENDING_REQUEST_ERR -982
#define key_mismatch_err \
"Error -983:    The file keyword does not match the secure keyword:\n\n\
               Secure keyword:  %s\n\
               File keyword:  %s"
#define KEY_MISMATCH_ERR -983
#define unsupported_transaction_err \
"Error -984:    this transaction is not supported for external customers."
#define UNSUPPORTED_TRANSACTION_ERR -984
#define already_approved_err \
"Error -985:  '%s' is already approved."
#define ALREADY_APPROVED_ERR -985
#define bad_encrypt_size \
"Error -986:    Transfer was aborted because the encrypted file \n\
               is 0 bytes in length."
#define BAD_ENCRYPT_SIZE -986
#define approver_mismatch_err \
"Error -987:    The approver Core ID does not match the registered Core ID:\n\n\
               Approver Core ID:  %s\n\
               Registered Core ID:  %s"
#define APPROVER_MISMATCH_ERR -987
#define version_err  \
"Error -988:  This version of Transcend is no longer supported.  Please \n\
               use the latest version present in /usr/fsl/bin."
#define VERSION_ERR -988
#define version_wrn \
"\n\nWarning -989:  This version of Transcend is no longer supported\n\
               and will be retired on %s.  Please upgrade to the\n\
               latest Transcend release present in /usr/fsl/bin."
#define VERSION_WRN -989
#define domain_not_approved_err \
"Error -990:    Approval was not revoked because \n\
               domain '%s' is not approved."
#define DOMAIN_NOT_APPROVED_ERR -990
#define user_not_approved_err \
"Error -991:    Approval was not revoked because \n\
               '%s' is not approved."
#define USER_NOT_APPROVED_ERR -991
#define domain_blacklist_err  \
"Error -992:    This transaction was aborted because \n\
               domain '%s' is black listed."
#define DOMAIN_BLACKLIST_ERR -992
#define user_blacklist_err  \
"Error -993:    This transaction was aborted because \n\
               user '%s' is black listed."
#define USER_BLACKLIST_ERR -993
#define timestamp_err "Error -995:    The approval timestamp is less than the current time.\n\
               Current time:  %ld\n\
               Approval time:  %ld\n"
#define not_reg_err "Error -994:    Your transaction was aborted because you are not registered."
#define NOT_REG_ERR -994
#define TIMESTAMP_ERR -995
#define not_approved_err  \
"Error -996:    Data retrieval was aborted because you are not approved to\n\
               retrieve data from NXP.  You must be registered and\n\
               approved to retrieve data from NXP.  Please contact\n\
               %s (%s) concerning this\n\
               problem.  %s's manager can perform approval.\n\
               For additional details about the approval process, retrieve\n\
               documentation using Intercom by typing 'icom -r approval.txt'."
#define NOT_APPROVED_ERR -996
#define blacklist_err "Error -997:  Your transaction was aborted because you are black listed."
#define BLACKLIST_ERR -997
#define no_customer_approvals "Error -998: No pending customer approval requests present."
#define NO_CUSTOMER_APPROVALS -998
#define no_commandline_approvals "Error -999: No command line approval requests present."
#define NO_COMMANDLINE_APPROVALS -999
#endif /* ifdef for global_server_errs_h */
