#ifndef msserver_errs_h
#define msserver_errs_h
/* Mask shop warning and error messages */
#define MASK_LEN 5
#define no_files_wrn "Warning -510:  No order form or job deck files sent to the mask shop for mask set\n\n\
               '"
#define NO_FILES_WRN -510
#define no_dir_err "Error -511:    No room for additional directories for mask set:"
#define NO_DIR_ERR -511
#define ms_bad_ack "Error -512:    Mask shop server received bad acknowledgement from client."
#define MS_BAD_ACK -512
#define bad_maskprefix      "Error -513:    Invalid mask set file name:"
#define BAD_MASKPREFIX -513
#define wr_open_err "Error -514:    Unable to open for write file:"
#define WR_OPEN_ERR -514
#define ms_write_err "Error -515:    Unable to write to mask set file:"
#define MS_WRITE_ERR -515
#define mkdir_err "Error -516:    Unable to create mask set directory:"
#define MKDIR_ERR -516
#define ms_bad_password "Error -517:    Internal password corrupted during data transmission.  "
#define MS_BAD_PASSWORD -517
#define bad_client_code "Error -518:    return code of '%d' received from client."
#define BAD_CLIENT_CODE -518
#define mail_err "Error -519:    Unable to create mail file to send to\n\
               mask shop administrator."
#define MAIL_ERR -519
#define bad_ms_arg "Error -520:    Invalid option '%s'\n\n\
               Use '-p' for production, '-t' for test,\n\
               and '-d' for development Mask Shop server."
#define BAD_MS_ARG -520
#define put_short_err "Error -521:    Mask shop server unable to send\n\
               length field to the user."
#define PUT_SHORT_ERR -521
#define put_err "Error -522:    Mask shop server unable to\n\
               send data to the user."
#define PUT_ERR -522
#define get_short_err "Error -523:    Mask shop server unable to retrieve\n\
               length field from the user."
#define GET_SHORT_ERR -523
#define get_err "Error -524:    Mask shop server unable to\n\
               retrieve data from the user."
#define GET_ERR -524
#define rename_err "Error -525:    Unable to rename database file:"
#define RENAME_ERR -525
#define bad_len_err "Error -526:  Bad error code of %d received from the client."
#define BAD_LEN_ERR -526
#define rd_open_err "Error -527:    Unable to open for read file:"
#define RD_OPEN_ERR -527
#define lock_err "Error -528:  Unable to lock database file:"
#define LOCK_ERR -528
#define bad_account "Error -529:  Unable to create directory '%s'.\n\
               This may be due to an invalid account name.\n\
               Please re-enter your doc account name and try the request again."
#define BAD_ACCOUNT -529

#define close_down1q(err_msg, file, err_no) {\
    char buffer[1024];\
    strcpy7(buffer, err_msg, RET2_TAB, "'", file, "'",  DOUBLE_RET, HELP_MSG);\
    ms_close_down(buffer, err_no);\
}

#define close_down2q(err_msg, par1, par2, err_no) {\
    char buffer[1024];\
    strcpy7(buffer, err_msg, RET2_TAB, "'", par1, par2, "'",  DOUBLE_RET, HELP_MSG);\
    ms_close_down(buffer, err_no);\
}

#define ms_close_down1d(err_msg, ret, err_no) {\
    sprintf(rtn.msg, err_msg, ret);\
    ms_close_down(buffer, err_no);\
}

#define ms_close_down1p(err_msg, file, err_no) {\
    sprintf(rtn.msg, err_msg, file);\
    ms_close_down(buffer, err_no);\
}

#endif /* ifdef for msserver_errs_h */
