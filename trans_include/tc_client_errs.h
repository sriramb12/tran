#ifndef dfm_client_errs_h
#define dfm_client_errs_h

#define tech_err \
"Error -275:    Technology not defined in control file.  Please\n\
               define the technology before submitting  the job.\n\n"
#define mask_err \
"Error -276:    Mask set not defined in control file.  Please\n\
               define the mask set before submitting  the job.\n\n"
#define gds_err \
"Error -277:    GDS file not defined in control file.  Please\n\
               define the GDS file before submitting  the job.\n\n"
#define script_err \
"Error -278:    Calibre script not defined in control file.  Please\n\
               define the Calibre script before submitting  the job.\n\n"
#define design_err \
"Error -279:    Design not defined in control file.  Please\n\
               define the design before submitting  the job.\n\n"
#define results_err \
"Error -280:    Results file not defined in control file.  Please\n\
               define the results file before submitting  the job.\n\n"
#define summary_err \
"Error -281:    Summary output not defined in control file.  Please\n\
               define the summary output before submitting  the job.\n\n"
#define cookie_err "Error -282:    Specified badge does not match the badge you are registered with.\n\n\
               Specified badge:   %s\n\
               Registered badge:  %s\n"
#define reg_failed "Error -283:  Registration failed.\n\n"
#define required_param_err "Error -284:    The following required command line arguments were not specified:\n\n"
#define required_param_msg "Warning -284:  The following required command line arguments were not specified:\n\n"
#define sql_err "Error -285:    Received error code %d from mySql query:\n\n\
               '%s'\n\n\
               Error string returned is:\n\n\
               '%s'"
#define SQL_ERR -285
#define invalid_request \
"Error -286:    Invalid result found for the following tool and request type:\n\n\
               Tool number:  %d\n\
               Request type: %s"
#define INVALID_REQUEST -286
#define tool_err \
"Error -287:    No tool type specified.  To see supported\n\
               tools, type 'jobsubmit -hp tools'\n\n"
#define invalid_format \
"Error -288:    Invalid command format.   All command options must  be\n\
               followed by one parameter.\n\n\
               Option:  %s\n\
               Arg count:  %d\n\n"
#define technology_err "Error -289:    Invalid technology '%s' entered."
#define TECHNOLOGY_ERR -289
#define invalid_mask "Error -290:    Invalid Mask ID '%s' entered."
#define INVALID_MASK  -290
#define invalid_tools "Error -291:  No tools found in table '%s'\n\n\
Query executed:\n\n\
%s\n\n"
#define invalid_tool "Error -292:  Invalid tool '%s'.\n\n\
Query executed:\n\n\
%s\n\n"
#define invalid_request_type "Error -293:  Invalid request type '%s'.\n\n\
Query executed:\n\n\
%s\n\n"
#define run_script_open_err "Error -294:    Unable to open for write run script:\n\n\
               '%s'\n\n"
#define invalid_request_option \
"Error -295:    Invalid option found for the following tool and request type:\n\n\
               Tool name:  %s\n\
               Tool number:  %d\n\
               Request type: %s\n\n"
#define INVALID_REQUEST_OPTION -295
#define invalid_tool_id "Error -296:  Invalid tool ID '%d'.\n\n\
Query executed:\n\n\
%s\n\n"
#define INVALID_TOOL_ID -296

#endif
