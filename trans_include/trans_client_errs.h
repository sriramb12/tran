#ifndef trans_client_errs_h
#define trans_client_errs_h
/* Database client errors */

#include "global_client_errs.h"


#define REG_REQ -359
#define SUBNET_ERR -886
#define archive_rd_header_err "Error -51:  Unable to read archive header.  download aborted."
#define ARCHIVE_RD_HEADER_ERR -51
#define archive_wr_header_err "Error -52:  Unable to write archive header.  download aborted."
#define ARCHIVE_WR_HEADER_ERR -52
#define archive_wr_data_err "Error -53:  Unable to write to archive.  download aborted."
#define ARCHIVE_WR_DATA_ERR -53
#define archive_finish_err "Error -54:  Unable to finish write of archive.  Data download aborted."
#define ARCHIVE_FINISH_ERR -54
#define domain_err "Error -55:     Invalid domain/e-mail address specified.\n\n\
               '%s'"
#define DOMAIN_ERR -55
#define client_encrypt_err \
"Error -57:    Failed to create encrypted file.  This error usually\n\
               occurs when the encryption key cannot be located.\n\
               or the source file is not present.\n\
               Data transfer has been aborted."
#define CLIENT_ENCRYPT_ERR -57
#define req_type_not_set \
"Error -201:    This option cannot be initialized until a \n\
               valid request type is set."
#define REQ_TYPE_NOT_SET -201
#define invalid_option_type  \
"Error -202:    Specified option not supported."
#define INVALID_OPTION_TYPE -202
#define extend_req_err \
"Error -203:    The extend request type must be set \n\
               before this option can be set."
#define EXTEND_REQ_ERR -203
#define put_req_err \
"Error -205:    The put request type must be set \n\
               before this option can be set."
#define PUT_REQ_ERR -205
#define get_req_err \
"Error -207:    The get request type must be set \n\
               before this option can be set."
#define GET_REQ_ERR -207
#define trans_reg_err "Error -220:    You are not registered to use Transcend."
#define TRANS_REG_ERR -220
#define invalid_icap_class \
"Error -221:    Invalid iCAP class specified."
#define INVALID_ICAP_CLASS -221
#define no_pending_request \
"Error -222:    Approval  for  '%s' denied because a pending\n\
               approval request is not present.   Please  request  approval\n\
               with   the   -j   option  or  by  going  to  the  EDC  site:\n\n\
               http://nww.edc.nxp.com"
#define NO_PENDING_REQUEST -222
#define tool_index_err "Error -223:     Server index out of range for this tool."
#define TOOL_INDEX_ERR -223
#define invalid_transfer_type \
"Error -224:    Invalid transfer type specified."
#define INVALID_TRANSFER_TYPE -224
#define invalid_request_type \
"Error -225:    Invalid request type specified."
#define INVALID_REQUEST_TYPE -225
#define invalid_stream_ptr \
"Error -226:    Invalid stream pointer  passed in."
#define INVALID_STREAM_PTR -226
#define invalid_file_descriptor \
"Error -227:    Invalid file descriptor  passed in."
#define INVALID_FILE_DESCRIPTOR -227
#define invalid_num_days \
"Error -228:    Invalid number of days of '%d' specified.\n\
               Valid number of days is 1, 2, or 3."
#define INVALID_NUM_DAYS -228
#define invalid_tool \
"Error -229:    Invalid tool specified."
#define INVALID_TOOL -229
#define invalid_mask_name \
"Error -230:  Invalid mask set name '%s' entered."
#define INVALID_MASK_NAME -230
#define ll_platform_wrn \
"Warning -238:  Parallel transfer is not supported on Windows. \n\n"
#define aix_ll_platform_wrn \
"Warning -238:  Parallel transfer is not supported on AIX. \n\n"
#define LL_PLATFORM_WRN -238
#define ll_wrn \
"Warning -239:  Parallel transfer is not supported for this type of data.\n\n"
#define ll_dir_wrn \
"Warning -240:  Parallel transfer not supported for directories. \n\n"
#define ll_files_wrn \
"Warning -241:  Parallel transfer not supported for groups of files. \n\n"
#define ll_5mb_wrn  \
"Warning -242:  Parallel transfer is not supported for data \n\
               less than 5 Mb in size.\n\n"
#define ll_zip_wrn \
"Warning -243:  Parallel transfer supported only for zipped data.\n\n"
#define files_retry_wrn \
"	Warning -244:  The retry option is not supported for files \n\
               less then 5 Mb in size.\n\n"
#define ll5mb_wrn  \
"Warning -245:  Parallel transfer is not supported for data \n\
               less than 5 Mb in size.\n\n"
#define retry_dir_wrn \
"	Warning -246:  The retry option is not supported for directories.\n\n"
#define retry5mb_wrn \
"Warning -247:  The retry option is not supported for data \n\
               less than 5 Mb in size.\n\n"
#define g1flag_wrn "Warning -248:  The -g1 option is not supported.  Try using -gl instead.\n\n"
#define G1FLAG_WRN -248
#define g2flag_wrn "Warning -249:  The -G1 option is not supported.  Try using -Gl instead.\n\n"
#define G2FLAG_WRN -249
#define tar_err "Error -250:  Unable to create directory '%s'.  You\n\
             may create the directory yourself by typing:\n\n\
             'tar -xf %s'\n"
#define TAR_ERR -250
#define release_err "Error -251:    Specified Transcend documentation/release not present.\n\n\
To obtain Transcend documentation:  Type '%s -r trans.doc'\n\
To obtain Transcend release notes:  Type '%s -r trans.rel'\n\
To retrieve an ASCII man page:      Type '%s -r trans.a'\n\
To retrieve a troff man page:       Type '%s -r trans.1'\n\
To obtain user survey results:      Type '%s -r trans.survey'\n\n\
To obtain HP version of Transcend:  Type '%s -r hpux_trans'\n\
To retrieve the SunOS version:      Type '%s -r sunos_trans'\n\
To retrieve the Solaris version:    Type '%s -r solaris_trans'\n\
To retrieve the AIX version:        Type '%s -r aix_trans'\n\
To retrieve the IRIX version:       Type '%s -r irix_trans'\n\
After retrieving a new release:     rename the release 'trans\n\n"
#define RELEASE_ERR -251
#define zero_len_file_err "Error -252:     Unable to retrieve file ''."
#define ZERO_LEN_FILE_ERR -252
#define not_medusa_err \
"Error -253:  You are attempting to upload ITAR data to the maskshop.\n\
               ITAR transfers must be done from medusa.  Please log onto\n\
               medusa to perform this transfer.\n\n"
#define NOT_MEDUSA_ERR -253
#define help_err "Error -254:  Help not present for specified command.\n\n"
#define HELP_ERR -254
#define invalid_trans_keyword "Error -255:    Invalid file keyword '%s'.  Valid keywords\n\
               contain 4 alphabetic characters (a-z) followed\n\
               by 4 digits (0-9).  They end with an alphabetic\n\
               character which specifies the site where the data\n\
               is stored.  File keywords end with a lower case\n\
               alphabetic character, and personal secure keywords \n\
               end with an upper case alphabetic character.\n\n"
#define INVALID_TRANS_KEYWORD -255
#ifdef TRANSCEND
#define gz_sockopen_err "Error -256:  Unable to write compressed data to Transcend socket."
#else
#define gz_sockopen_err "Error -256:  Unable to write compressed data to InterCom socket."
#endif
#define GZ_SOCKOPEN_ERR -256
#define trans_company_err "Error -257:    Specified company not present.\n\n"
#define TRANS_COMPANY_ERR -257
#define trans_site_err "Error -258:    Specified site not present.\n\
               Defined sites are listed below.\n\n"
#define TRANS_SITE_ERR -258
#define site_err "Error -259:  Specified site not found."
#define SITE_ERR -259
#define company_route_err "Error -260:  Unable to find routing data for '%s'%s%s%s"
#define COMPANY_ROUTE_ERR -260
/* Used only for InterCom */
#define no_company_err "A destination company must be specified.\n\
               Defined companies are listed below.\n\n"
#ifdef TRANSCEND
#define nt_put_wrn "Warning -261:  Directory transfer on Windows machines must\n\
               be done as a two step process.  To transfer the\n\
               directory, execute the following commands:\n\n\
               tar cf %s.tar %s\n\
               trans -p %s.tar\n\n"
#else
#define nt_put_wrn "Warning -261:  Directory transfer on Windows machines must\n\
               be done as a two step process.  To transfer the\n\
               directory, execute the following commands:\n\n\
               tar cf %s.tar %s\n\
               icom -p -<site> %s.tar\n\n"
#endif
#define NT_PUT_WRN -261
#define nt_get_wrn "\n\nWarning -262:  Directory transfer on Windows machines must\n\
               be done as a two step process.  Therefore, the data\n\
               was retrieved as a tar file.  Please unbundle \n\
               the directory with the Winzip or tar utility.\n\n\
To use the tar command:  type 'tar zxf %s'\n\n"
#define files_get_wrn "\n\nWarning -262:  Transfer of multiple files on Windows machines must\n\
               be done as a two step process.  Therefore, the data\n\
               was retrieved as a tar file.  Please unbundle \n\
               the files with the Winzip or tar utility.\n\n\
To use the tar command:  type 'tar xf %s'\n\n"
#define NT_GET_WRN -262
#define no_valid_keyword "Error -263:  Valid InterCom keyword not found in email from NXP Semiconductor"
#define NO_VALID_KEYWORD -263
#define nt_put_files_wrn \
"Warning -264:  Transfer of multiple files on Windows machines must\n\
               be done as a two step process.  To transfer the\n\
               files, execute the following commands:\n\n\
               tar cf files.tar %s\n\
               trans -p files.tar\n\n"
#define NT_PUT_FILES_WRN -264
#define root_dir_err "Error -265:    Unable to create file '%s' in directory '%s'.\n\
               You must perform the retrieval in a subdirectory i.e. '%s//transcend'."
#define fss_send_type_err "Error -266:    Invalid send type used.  Valid send types include:\n\
               - SEND2MOT\n\
               - SEND2FREE\n\
               - SEND2BOTH."
#define FSS_SEND_TYPE_ERR -266
#define external_auth_err \
"Error -267:    Authentication failed because an incorrect E-mail address\n\
               was not entered.   E-mail addresses are of the form: 'user\n\
               name@company.domain' i.e. 'transbug@nxp.com'.\n\n\
Note:          this site is for use by external customers for transferring\n\
               data to/from NXP Semiconductor customers.  NXP\n\
               customers can transfer data to/from external customers at\n\
               URL  https://nww.transweb.nxp.com.   To transfer data to an\n\
               external customer,  go to 'Put a new file'  and select\n\
               'External TransWeb Customer'  from the list of receiving\n\
               repositories.\n\n"
#define EXTERNAL_AUTH_ERR -267
#define not_enough_params \
"Error -268:    Not enough arguments specified.  You must enter both the \n\
               personal secure and file keyword to perform this transaction."
#define NOT_ENOUGH_PARAMS -268

#define no_filename_err \
"Error -269:    A file or directory name was not specified.  You \n\
               must enter the name of the file to be transferred."
#define NO_FILENAME_ERR -269
#define not_enough_arguments \
"Error -270:    Not enough arguments specified.  Check the syntax \n\
               for this command to see what arguments are required."
#define NOT_ENOUGH_ARGUMENTS -270
#define gtar_wrn \
"\n\nWarning -271:  the data was bundled using 'gtar' before it was transferred\n\
               to the repository.   However,  since 'gtar'  could not be\n\
               located, the data was retrieved and unbundled using 'tar'.\n\
               On rare occasions, problems occur when the data is bundled\n\
               with 'gtar' and unbundled with 'tar'.  If you experience any\n\
               problems,  they can be resolved by retrieving the data on a\n\
               machine where 'gtar' or 'gnutar'  can be found using the\n\
               'whereis' command."
#define tmp_wrn \
"\n\nWarning -271:  When the data was transferred to the repository it was\n\
               bundled with gtar.   However,  since 'gtar'  could not be\n\
               located, the data was retrieved and unbundled using 'tar'.\n\
               On rare occasions, problems occur when the data is bundled\n\
               with 'gtar' and unbundled with 'tar'.  If you experience any\n\
               problems,  they can be resolved by retrieving the data on a\n\
               machine where 'gtar' or 'gnutar'  can be found using the\n\
               'whereis' command."
#define GTAR_WRN -271
#define no_mask_projects "Warning -273:  No valid active projects associated with mask set '%s'.\n\n"
#define NO_MASK_PROJECTS -273
#define parallel_write_err  \
"Error -274:    Transfer aborted because parallel processes\n\
               no longer writing data."
#define PARALLEL_WRITE_ERR -274
#define shmem_alloc_err "Error -275:  Unable to allocate shared memory."
#define SHMEM_ALLOC_ERR -275
#define shmem_attach_err "Error -276:  Unable to attach to shared memory."
#define SHMEM_ATTACH_ERR -276
#define shmem_remove_err "Error -277:  Unable to remove shared memory.\n\n"
#define SHMEM_REMOVE_ERR -277
#define host_mismatch_err "Error -578:    Host name does not match common name:\n\n\
               Common name:  %s\n\
               Host name:  %s\n\n"
#define HOST_MISMATCH_ERR  -278
#define invalid_certificate "Error -279:  Invalid SSL certificate."
#define INVALID_CERTIFICATE -279
#define bad_trans_write "Error -280:    Unable to write to file:\n\n\
               %s\n\n\
               this error usually occurs when your disk is\n\
               full or you have reached your disk quota."
#define BAD_TRANS_WRITE -280
#define archive_write_header_err "Error -281:    Received error %d while writing archive header for file %s.\n\
%s\n\n\
%s"
#define ARCHIVE_WRITE_HEADER_ERR -281
#define archive_write_data_err "Error -282:    Received error %d while writing archive header for file %s.\n\
%s\n\n\
%s"
#define ARCHIVE_WRITE_DATA_ERR -282
#define encrypt_pwd_err \
"Error -283:  Unable to encrypt user password."
#define ENCRYPT_PWD_ERR -283
#define no_db_records "Error -291:  No records found in the database."
#define inconsistent_err "Error -292:  Inconsistent  records found for '%s'.\n\
               Registratry status:  %s\n\
               Database status:  %s"
#define db_record_err "Error -293:  Record for '%s' not present in the database."
#define reg_record_err "Error -294:  Record for '%s' not present in the registry."
#define cert_err "Error -295:    Unable to  read certificate file:\n\n\
               '%s'"
#define CERT_ERR -295
#define key_err "Error -296:  Unable to  read key file."
#define KEY_ERR -296
#define ca_err "Error -297:  Unable to  read CA list."
#define CA_ERR -297
#define host_err "Error -298:    Unable to resolve host:\n\n\
               '%s'"
#define HOST_ERR -298
#define transrc_wrn \
"Warning -299:    The .transrc file is not up to date.  You can correct this\n\
               problem by installing a new .transrc file in your home\n\
               directory.\n\n\
               To retrieve the latest .transrc file, type:\n\n\
                trans -r .transrc\n\n"
#define already_present_err "Error -568:  Data transfer was aborted because '%s' is already present on your disk."
#define ALREADY_PRESENT_ERR -568

#endif /* ifdef for trans_client_errs_h */
