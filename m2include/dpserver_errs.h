#ifndef dpserver_errs_h
#define dpserver_errs_h
/* Mask shop warning and error messages */
#define MASK_LEN 4
#define no_files_wrn "Warning -410:  No order form or job deck files sent to the mask shop for mask set\n\n\
               '"
#define NO_FILES_WRN -410
#define no_dir_err "Error -411:    No room for additional directories for mask set:"
#define NO_DIR_ERR -411
#define ms_bad_ack "Error -412:    Mask shop server received bad acknowledgement from client."
#define MS_BAD_ACK -412
#define bad_maskprefix      "Error -413:    Invalid mask set file name:"
#define BAD_MASKPREFIX -413
#define wr_open_err "Error -414:    Unable to open for write file:"
#define WR_OPEN_ERR -414
#define ms_write_err "Error -415:    Unable to write to mask set file:"
#define MS_WRITE_ERR -415
#define mkdir_err "Error -416:    Unable to create mask set directory:"
#define MKDIR_ERR -416
#define ms_bad_password "Error -417:    Internal password corrupted during data transmission.  "
#define MS_BAD_PASSWORD -417
#define bad_client_code "Error -418:    return code of '%d' received from client."
#define BAD_CLIENT_CODE -418
#define mail_err "Error -419:    Unable to create mail file to send to\n\
               mask shop administrator."
#define MAIL_ERR -419
#define bad_ms_arg "Error -420:    Invalid option '%s'\n\n\
               Use '-p' for production, '-t' for test,\n\
               and '-d' for development Mask Shop server."
#define BAD_MS_ARG -420
#define put_short_err "Error -421:    Mask shop server unable to send\n\
               length field to the user."
#define PUT_SHORT_ERR -421
#define put_err "Error -422:    Mask shop server unable to\n\
               send data to the user."
#define PUT_ERR -422
#define get_short_err "Error -423:    Mask shop server unable to retrieve\n\
               length field from the user."
#define GET_SHORT_ERR -423
#define get_err "Error -424:    Mask shop server unable to\n\
               retrieve data from the user."
#define GET_ERR -424

#define close_down2q(err_msg, file, err_no) {\
    char buffer[1024];\
    strcpy7(buffer, err_msg, RET2_TAB, "'", file, "'",  DOUBLE_RET, HELP_MSG);\
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

#endif /* ifdef for dpserver_errs_h */
