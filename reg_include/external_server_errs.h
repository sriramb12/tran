#ifndef external_server_errs_h
#define external_server_errs_h

#define external_err "Error -900:  External server not responding."
#define EXTERNAL_ERR -900
#define external_open_err "Error -901:  Unable to access data returned from external server."
#define EXTERNAL_OPEN_ERR -901
#define external_read_err "Error -902:  Unable to read data received from external server."
#define EXTERNAL_READ_ERR -902
#define no_file_err "Error -904:  No file provided to process."
#define NO_FILE_ERR -904
#define company_err "Error -905:    Requested data is owned by %s.  You are \n\
               not authorized to access data owned by this company."
#define COMPANY_ERR -905

#endif /* define for external_server_errs_h */
