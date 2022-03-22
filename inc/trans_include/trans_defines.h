#ifndef trans_defines_h
#define trans_defines_h

#include <time.h>
#include "global_defines.h"

#define ONE_MONTH 2592000
#define ONE_DAY 86400
#define SERIAL 0
#define PARALLEL 1
#define SEPARATE_BLOCKS 2
#define DTMS "DTMS"
#define TRANS_DIRLIST "trans_dirlist.txt"
#define FSS_EMAIL_DIR "fss_email_files"
#define FSS_TAR_DIR "fss_email_files.tar"
#define FSS_COMPANY "Foundry Services Customer"
#define TRANSNET_COMPANY "TransNet Customer"
#define TRANSWEB_COMPANY "External TransWeb Customer"
#define EXTERNAL_INTERCOM_COMPANY "External Intercom Customer"
#define EXTERNAL_TRANSWEB_COMPANY "External TransWeb Customer"
#define FREE_COMPANY "NXP Semiconductor"
#define SUPPORT_EMAIL "support@nxp.com"
#define FROM_SUPPORT_EMAIL "'NXP Support <support@nxp.com>'"
#define MICA_EMAIL "mica@nxp.com"
#define ECHUCA_EMAIL "echuca@nxp.com"
#define TRANSWEB_EMAIL "transbug@nxp.com"

#ifdef TRANS_FTP
#define TRANS_VERSION CURRENT_ICOM_VERSION
#endif

#define REPORT_VERSION "1.0.1"
 #define TRANS_KEY "big_file"
/* Info options allow user to specify the site where the command is executed */
#define INFO_OPTIONS "csrvqQ"
#define RETRIEVE_OPTIONS "bBcdDeEgGhilLmsU"
#define PUT_OPTIONS "CfFIJkKMnNpPOrRStTVXYzZ-"
#define PUT_SET_OPTIONS "CfFIJkKMnNopPOrRStTuzZcs1-"
#define PUT_GET_OPTIONS "fFgGJkKnNpPOtTzZ-"
#define NON_COMPANY_OPTIONS "aAIHMxX"
#define THREEPARAM_OPTIONS "kKzZ"

#ifdef TRANSCEND
#define TRANS_VERSION CURRENT_TRANS_VERSION
#define CUSTOMER_NAME "Transcend Customer"
#define TWO_PARAM_OPTIONS "aAIMoqQrSvVwWXxY"
#define LOCAL_SITE_OPTIONS "IMqQrRu1vVwW"
#define TRANS_LOG "/trans.log"
#endif

#ifdef OPCTRANS
#define TRANS_VERSION "2.1"
#endif

#ifdef INTERCOM
#define TRANS_LOG "/icom.log"
#define TRANS_PROGRAM "InterCom Server"
#define TRANS_VERSION CURRENT_ICOM_VERSION
#define CUSTOMER_NAME "Intercom Customer"
#define TWO_PARAM_OPTIONS "aAIMqQorRSvVwWxX"
#define LOCAL_SITE_OPTIONS "IMqQorRSu1vVwW"
#endif

#ifdef TRANSWEB
#define TRANS_VERSION "3.5.3"
#define TRANS_LOG "/trans.log"
#define CUSTOMER_NAME "TransWeb Customer"
#endif

#ifdef M2
#define TRANS_LOG "/icom.log"
#define TRANS_PROGRAM "InterCom Server"
#define TRANS_VERSION  CURRENT_TRANS_VERSION
#define CUSTOMER_NAME "Intercom Customer"
#define TWO_PARAM_OPTIONS "aAIMqQorRvVwWxX"
#define LOCAL_SITE_OPTIONS "MqQorRu1vVwW"
#endif

#define TRANS_EMAIL "transbug@nxp.com"
#define HASHTIME 15
#define REG_DELETE "delete"
#define REG_RESET "reset"
#define TRANS_ARRAY_OFFSET 48
#define EMAIL_LEN 64
#define NONE_STRING "0", "0", "0", "0", "0", "0"
#define NONE5STRING "0", "0", "0", "0", "0"
#define NONE4STRING "0", "0", "0", "0"
#define DBRMT_MAN_REG 64

/* Defines used by the emailer program written by Robert */
#define EMAIL_ADDRESS "maskprep_log@ssdt-tempe.sps.mot.com"

/* Enum for request types */
typedef enum
{
 /*000*/   DBGET_DATA ,                              // Get request               Retrieve data from Transcend/Intercom repository
 /*001*/   DBPUT_DATA,                               // Put request               Upload data to Transcend/InterCom repository
 /*002*/   DBDLT_DATA,                               // Keyword request           Delete data from Transcend/InterCom repository
 /*003*/   DBCRON_DLT,                               // Admin request             Delete expired files once an hour
 /*004*/   DBINFO_DATA,                              // Keyword request           Retrieve information about data in repository
 /*005*/   DBHIST_DATA,                              // Custom request            Retrieve history of transactions perform on stored data
 /*006*/   DBPUT_TAR_FILE,                           // Put request               Upload data
 /*007*/   DBEXTEND_DATA,                            // Keyword request           Extend file expiration 1-3 working days
 /*008*/   DBSET_USER_EMAIL,                         // Simple request            Set e-mail notification on for all data retrieved
 /*009*/   DBSET_FILE_EMAIL,                         // Keyword request           Set e-mail notification on for specified keyword
 /*010*/   DBCAN_USER_EMAIL,                         // Simple request            Cancel e-mail notification for all files
 /*011*/   DBCAN_FILE_EMAIL,                         // Keyword request           Cancel e-mail notification for specified keyword
 /*012*/   DBGETDLT_DATA,                            // Get request               Retrieve data and delete from repository
 /*013*/   DBPUTDLT_DATA,                            // Put request               Upload data and delete after first retrieval
 /*014*/   DBPUTDLT_TAR_FILE,                        // Put request               Upload data and delete after first retrieval
 /*015*/   DBREG_QUERY,                              // Undocumented request      Undocumented
 /*016*/   DBAUTO_REG,                               // Simple request            Perform auto registration with Core ID and password PAR1 holds Core ID, PAR2 holds password
 /*017*/   DBMAN_REG,                                // Unused request            Manual registration not currently used
 /*018*/   DBFORMAL_REG,                             // Unused request            Different type of manual registration not currently used
 /*019*/   DBRESEND_REG,                             // Not currently  used       Resend registration entries
 /*020*/   DBSEND_KEYWORD,                           // Custom request            Send secure keyword to specified users
 /*021*/   DBQUERY_USER,                             // Simple request            EMAIL_LIST holds array of e-mail addresses to receive secure keyword
 /*022*/   DBFORMAT_EMAIL,                           // Simple request            Unknown
 /*023*/   DBGET_RELEASE,                            // Simple request            Retrieve new Transcend/InterrCom release
 /*024*/   DBMOD_ADMIN,                              // Not currently used        Modify administrator's Email and login
 /*025*/   DBPUT_FILES,                              // Put request               Upload group of files
 /*026*/   DBSIZE_CHECK,                             // Keyword request           Display uncompressed, compressed, and stored file size
 /*027*/   DBPUTDLT_FILES,                           // Put request               Put group of files and delete after first retrieval
 /*028*/   DBGET_RELEASE_INFO,                       // Simple request            Display list of Transcend/InterCom releases
 /*029*/   DBDLT_OLD_SITES,                          // Unsupported  request      Internal option to delete registration entries no longer being used
 /*030*/   DBDLT_X500,                               // Unsupported request       Internal option
 /*031*/   DBLOCK_DATA,                              // Keyword request           Lock data for retrieval
 /*032*/   DBUNLOCK_DATA,                            // Keyword request           Unlock data for retrieval
 /*033*/   DBPUT_ACK,                                // Internal request          Internal option for put ack
 /*034*/   DBGET_ACK,                                // Internal request          Internal option for get ack
 /*035*/   DBGETDLT_ACK,                             // Internal request          Internal option
 /*036*/   DBPUTDLT_ACK,                             // Internal request          Internal option
 /*037*/   DBX500AUTHEN,                             // Simple request            Perform user authentication
 /*038*/   DBSET_ASCII,                              // No longer used            No longer used
 /*039*/   DBSET_BINARY,                             // Unsupported request       No longer used
 /*040*/   DBRMT_REG,                                // Internal request          Internal option for remote registration
 /*041*/   DBGET_ICOM_RELEASE,                       // Unsupported request       Retrieve Intercom release,  no longer used
 /*042*/   DBGET_ICOM_RELEASE_INFO,                  // Unsupported request       List Intercom releases,  no longer used
 /*043*/   DBSITE_CHECK,                             // Simple request            Verify user registered at specified site
 /*044*/   DBREAD_KEYWORD,                           // Simple request            Display list of file keywords
 /*045*/   DBOWNER_DLT,                              // Keyword request           Allow only owner to delete data with specified keyword
 /*046*/   DBUSER_DLT,                               // Keyword request           Allow anyone with keyword to delete the associated data
 /*047*/   DBADMIN_AUTH,                             // Admin request             Verify user is authorized to perform admin functions
 /*048*/   DBPING,                                   // Simple request            Ping specified Transcend/InterCom server
 /*049*/   DBSILENT_PING,                            // Simple request            Silent ping to specified Transcend/InterCom server
 /*050*/   DBPUT_LIST_ALL,                           // Simple request            List all puts
 /*051*/   DBREPORT_USAGE,                           // Unsupported request       Report Transcend usage,  no longer used
 /*052*/   DBFTP_RESEND,                             // Keyword request           Resend encrypted data to foundry
 /*053*/   DBFAST_QUERY_USER,                        // Simple request            Retrieve LDAP registration information for user
 /*054*/   DBUSR_TABLE_BUILD,                        // Internal request          Undocumented
 /*055*/   DBCOMPANY_TABLE_RTV,                      // Simple request            Retrieve list of defined companies
 /*056*/   DBQUERY_KEYWORD,                          // Simple request            View list of secure keywords sent
 /*057*/   DBMETTRACKS,                              // Unsupported request       No longer used
 /*058*/   DBSTATUS_DATA,                            // Simple request            Undocumented
 /*059*/   DBUSR_KEYS,                               // Simple request            Display list of file keywords for user
 /*060*/   DBTRANSWEB_COMPANY_RTV,                   // Simple request            Retrieve list of TransWeb companies
 /*061*/   TRANSWEB_HISTORY,                         // Simple request            Retrieve TransWeb history
 /*062*/   DBSECURE_KEYS,                            // Simple request            Retrieve list of secure keywords user sent
 /*063*/   DBTRANSWEB_REPOSITORY_RTV,                // Simple request            Retrieve list of Transcend repositories
 /*064*/   TRANSWEB_EMAIL_SEND,                      // Simple request            Undocumented
 /*065*/   USER_KEYS_DELETE,                         // Simple request            Delete data for all user keywords
 /*066*/   SECURE_KEYS_DELETE,                       // Simple request            Delete data for all secure keywords user sent
 /*067*/   DBRMT_TRANSWEB_REG,                       // Unsupported request       Perform remote TransWeb registration,  no longer used
 /*068*/   DBREG_AUDIT,                              // Admin request             Audit Transcend/Intercom registry
 /*069*/   DBREPORT_SUMMARY,                         // Unsupported request       Report summary,  no longer used
 /*070*/   DBCOMPANY_ADD,                            // Simple request            Undocumented
 /*071*/   DBSERVER_SET,                             // Simple request            find closest server to user
 /*072*/   FSS_RTV_LOGS,                             // Simple request            No longer used
 /*073*/   FSS_EMAIL_SEND,                           // Simple request            No longer used
 /*074*/   DBFTP_GET,                                // Unsupported request       No longer used
 /*075*/   DBPUTDLT_VERIFY_FILES,                    // Put request               Send to maskshop,  verify not previously sent, & delete after retrieval
 /*076*/   DBSUPPORT_ADD,                            // Simple request            Add new user for support user group
 /*077*/   DBSUPPORT_DELETE,                         // Simple request            Delete user from Support user group
 /*078*/   DBSUPPORT_VIEW,                           // Simple request            View authorized users in Support group
 /*079*/   DBPUT_VERIFY_FILES,                       // Put request               Upload data and verify not previously sent to the maskshop
 /*080*/   DBMASK_FILELIST_SEND,                     // Simple request            Undocumented
 /*081*/   DBMASK_HISTORY,                           // Not currently used        Retrieve history for mask data sent to Maskshop
 /*082*/   DBPUT_DUPLICATE_FILES,                    // Put request               Force resend of data to the Maskshop
 /*083*/   DBSERVER_REG,                             // Simple request            Undocumented
 /*084*/   DBRMT_SERVER_REG,                         // Internal request          Internal option
 /*085*/   DBGET_ZIPPED,                             // Get request               Retrieve data as a zipped file
 /*086*/   DBMASK_PROJECTS_RTV,                      // Unused request            No longer used
 /*087*/   DTMS_PROJECT_STATUS_CHECK,                // Unused request            No longer used
 /*088*/   DBGETDLT_ZIPPED,                          // Get request               Retrieve data as a zipped file and delete from repository
 /*089*/   DBBLOCK_PUT,                              // Internal request          Internal option used during parallel transfer
 /*090*/   DBBLOCK_GET,                              // Internal request          Internal option used during parallel transfer
 /*091*/   DBPUT_PARALLEL,                           // Put request               Upload data using parallel transfer
 /*092*/   DBRANDOM_BLOCK_PUT,                       // Internal request          Internal option used during parallel upload
 /*093*/   DBPUT_RANDOM_PARALLEL,                    // Put request               Internal option used during parallel upload
 /*094*/   DBX500LOOKUP,                             // Simple request            Display name and e-mail address for specified user
 /*095*/   DBMODE_DATA,                              // Simple request            Undocumented
 /*096*/   TRANSDIRLIST_RTV,                         // Custom request            Retrieve list of files for specified keyword
 /*097*/   TRANSDIRLIST_PUT,                         // Custom request
 /*098*/   DBGET_PARALLEL,                           // Get request               Retrieve data using parallel transfer
 /*099*/   SHARED_MEMORY_CLEANUP,                    // Custom request            Clean up shared memory not properly deallocated during parallel transfer
 /*100*/   DBRANDOM_BLOCK_GET,                       // Internal request
 /*101*/   DBGET_RANDOM_PARALLEL,                    // Internal request          Internal option used during parallel download
 /*102*/   DBLIST_EXTENSIONS_RTV,                    // Internal request          Internal option used to retrieve file type ratios
 /*103*/   DBGET_SERIAL,                             // Get request               Retrieve data using serial transfer
 /*104*/   GUI_COMPANY_TABLE_RTV,                    // GUI request               Retrieve list of companies
 /*105*/   DBGET_SEPARATE,                           // Get request               Retrieve data using separate block transfer
 /*106*/   DBPUT_SEPARATE,                           // Put request               Upload data using separate block transfer
 /*107*/   DBREG_URL,                                // Simple request            Retrieve URL for External LDAP registration
 /*108*/   DBLIST_DTMS_TEST_FILES,                   // Internal request          No longer used
 /*109*/   DBTRANSWEB_LOG_RTV,                       // Simple request            Undocumented
 /*110*/   DBSEPARATE_BLOCK_PUT,                     // Internal request          Internal option used during separate block upload
 /*111*/   DBSEPARATE_BLOCK_GET,                     // Internal request          Internal option used during separate block retrieval
 /*112*/   DBENCRYPT_PASSWORD_LOG,                   // Simple request            Log e-mail and subnet for user storing encrypted password
 /*113*/   ENCRYPT_ITAR_DATA,                        // Internal request          Encrypt ITAR data before transfer to the Maskshop
 /*114*/   DBGETDLT_PARALLEL,                        // Get request               Retrieve data with parallel transfer and delete from repository
 /*115*/   DBPUTDLT_PARALLEL,                        // Put request               Upload using parallel transfer and delete after first retrieval
 /*116*/   DBAUTH_USER,                              // EDC request               Authorized external customer to retrieve data from NXP
 /*117*/   DBAUTH_RTV,                               // EDC request               Undocumented
 /*118*/   EDC_COMMANDLINE_APPROVE,                  // EDC request               Undocumented
 /*119*/   EDC_COMMANDLINE_RTV,                      // EDC request               Undocumented
 /*120*/   DBREQUEST_APPROVAL,                       // EDC request               Undocumented
 /*121*/   DBREQUEST_STATUS,                         // EDC request               Undocumented
 /*122*/   DBEXTRACT_REGISTRY,                       // Internal request          Extractcopy of Transcend/Intercom registry
 /*123*/   DBBUILD_REGISTRY,                         // Internal request          Rebuild Transcend/Intercom registry
 /*124*/   DBPUT_LARGE_PARALLEL,                     // Put request               Upload with parallel transfer with ack to prevent time outs
 /*125*/   SEND4FOUNDRY,                             // Unused request            No longer used
 /*126*/   DBTRUST_USER,                             // EDC request               EDC request
 /*127*/   DBGET_LARGE_PARALLEL,                     // Get request               Send separate ack during large parallel download
 /*128*/   DBPUTDLT_LARGE_PARALLEL,                  // Internal request          Send separate ack during parallel upload & delete after retrieval
 /*129*/   DBGETDLT_LARGE_PARALLEL,                  // Internal request          Undocumented
 /*130*/   DBFILENAME_RTV,                           // Keyword request           Retrieve file name for specified keyword
 /*131*/   TRANSDIRLIST_FILE_RTV,                    // Keyword request           Retrieve list of files for keyword and store in local file
 /*132*/   DBSITE_RTV,                               // Retrieve list of sites
 /*133*/   DBREPOSITORY_RTV,                         // Retrieve list of repositories
 /*134*/   DBICAP_MSG,                               // Display iCAP classifications
 /*135*/   DBLONG_SECURE_KEYS_RTV,                   // Retrieve long list of secure keywords
 /*136*/   DBLONG_FILE_KEYS_RTV,                     // Retrieve long list of file keywords
 /*137*/   DBRTV_NAME_PHONE,                         // Retrieve user's name and phone number
 /*138*/   DB1STEP_UPLOAD,                           // Custome request
 /*139*/   DB1STEP_DOWNLOAD,                         // Keyword request
 /*140*/   DB1STEP_DOWNLOAD_COMPLETE,                // Keyword request
 /*141*/   DB1STEP_ZIP,
 /*142*/   DBGETDLT_SERIAL,                          // Get request               Retrieve data and delete keyword using serial transfer
 /*143*/   DBGET_SERIAL_ZIPPED,
 /*144*/   DBGETDLT_SERIAL_ZIPPED,
 /*145*/   DBGETDLT_SEPARATE,
} REQ_TYPE;

// this will be set only then the user forces the transfer to be serial
#define IS_REQUEST_GET_SERIAL(req_type)     \
    (req_type == DBGET_SERIAL        \
     || req_type == DBGET_SERIAL_ZIPPED \
     || req_type == DBGETDLT_SERIAL_ZIPPED \
     || req_type == DBGETDLT_SERIAL)

#define IS_REQUEST_GET_DELETE(req_type)     \
    (req_type    == DBGETDLT_DATA      \
     || req_type == DBGETDLT_ZIPPED \
     || req_type == DBGETDLT_PARALLEL \
     || req_type == DBGETDLT_SERIAL \
     || req_type == DBGETDLT_SERIAL_ZIPPED \
     || req_type == DBGETDLT_SEPARATE \
     || req_type == DBGETDLT_LARGE_PARALLEL)

#define IS_REQUEST_GET_PARALLEL(req_type)    \
    (req_type   == DBGET_PARALLEL     \
    || req_type == DBGETDLT_PARALLEL)

#define IS_REQUEST_GET_LARGE_PARALLEL(req_type)    \
     (req_type ==  DBGET_LARGE_PARALLEL    \
      || req_type == DBGETDLT_LARGE_PARALLEL)


#define IS_REQUEST_GET_SEPARATED(req_type) \
    (req_type     == DBGET_SEPARATE        \
      || req_type == DBGETDLT_SEPARATE)


#define IS_REQUEST_GET_ZIPPED(req_type) \
    (req_type    ==   DBGET_ZIPPED \
     || req_type ==  DBGETDLT_SERIAL_ZIPPED \
     || req_type ==  DBGET_SERIAL_ZIPPED \
     || req_type ==  DBGETDLT_ZIPPED)





#define IS_REQUEST_PUT_DELETE(req_type)  (  req_type == DBPUTDLT_DATA \
                                          || req_type == DBPUTDLT_TAR_FILE \
                                          || req_type == DBPUTDLT_FILES \
                                          || req_type == DBPUTDLT_VERIFY_FILES \
                                          || req_type == DBPUTDLT_PARALLEL \
                                          || req_type == DBPUTDLT_LARGE_PARALLEL)

#define IS_REQUEST_PUT_PARALLEL(req_type)        (req_type == DBPUT_PARALLEL || req_type == DBPUTDLT_PARALLEL)

#define IS_REQUEST_PUT_LARGET_PARALLEL(req_type) (req_type == DBPUT_LARGE_PARALLEL || req_type == DBPUTDLT_LARGE_PARALLEL)

#define IS_KEYWORD_REQUEST(req_type) (  req_type ==  DBDLT_DATA        \
                                     || req_type == DBINFO_DATA        \
                                     || req_type == DBEXTEND_DATA      \
                                     || req_type == DBSET_FILE_EMAIL   \
                                     || req_type == DBCAN_FILE_EMAIL   \
                                     || req_type == DBSIZE_CHECK       \
                                     || req_type == DBLOCK_DATA        \
                                     || req_type == DBUNLOCK_DATA      \
                                     || req_type == DBOWNER_DLT        \
                                     || req_type == DBUSER_DLT         \
                                     || req_type == DBFTP_RESEND       \
                                     || req_type == DBFILENAME_RTV     \
                                     || req_type == DB1STEP_DOWNLOAD   \
                                     || req_type == DB1STEP_DOWNLOAD_COMPLETE)



#define IS_SINGLE_BLOCK_FROM_PARALLEL_TRANSFER(req_type)  (  req_type == DBBLOCK_PUT \
                                                          || req_type == DBBLOCK_GET \
                                                          || req_type == DBRANDOM_BLOCK_PUT \
                                                          || req_type == DBRANDOM_BLOCK_GET \
                                                          || req_type == DBSEPARATE_BLOCK_PUT \
                                                          || req_type == DBSEPARATE_BLOCK_GET)



/* Defines for Transcend file types */
typedef enum
{
    ASCII = 'a',
    BINARY = 'b',
    MAC_BINARY = 'c', /* Currently not being used */
    DIRECTORY= 'd',
    UNSPECIFIED = 'e',
    FILES = 'f',
    UNKNOWN = 'g',
    HUGE_DIR = 'h',
    HUGE_FILE = 'i',
    HUGE_FILES = 'j',
    UNCOMPRESSED = 'k'
} TRANS_FILE_TYPE;

#define SUPPORT_EMAIL_LIST "db_admin/support_email_list"
#define SUPPORT_EMAIL_ROOT "support_email_list"
#define SUPPORT_EMAIL_ADMIN "db_admin/support_email_admin"
#define MICA_EMAIL_LIST "db_admin/mica_email_list"
#define MICA_EMAIL_ROOT "mica_email_list"
#define MICA_EMAIL_ADMIN "db_admin/mica_email_admin"
#define NUM_PARALLEL_PROCESSES 10
#define FTP_PARALLEL_SIZE 20000000
#define FTP_SERIAL_SIZE 16000000
#define DTMS_WRN \
"This transcend repository is for DTMS GDB/DRC uploads only.\n\
Please do not use it for other purposes.\n\n"
#ifdef NXP
#define ICAP_MSG \
"Defined iCAP classifications include: \n\n\
    -  FCP - NXP Confidential Proprietary.  \n\
    -  FIUO:  NXP Internal Use Only.  \n\
    -  PUBI:  Public Information.  \n\
    -  GBI:  General Business Information.  \n\n\
To assign an iCAP classification, include one of the above \n\
options as a command line parameter during data upload.  \n\
The following command assigns the uploaded data iCAP \n\
classification FIUO: \n\n\
%s -p -fiuo <file> \n\n\
If not specified, the iCAP classification defaults to FCP.  \n\n"
#else
#define ICAP_MSG \
"Defined iCAP classifications include: \n\n\
    -  CP - Confidential Proprietary.  \n\
    -  PUBI:  Public Information.  \n\
    -  GBI:  General Business Information.  \n\n\
To assign an iCAP classification, include one of the above \n\
options as a command line parameter during data upload.  \n\
The following command assigns the uploaded data iCAP \n\
classification PUBI: \n\n\
%s -p -pubi <file> \n\n\
If not specified, the iCAP classification defaults to CP.  \n\n"
#endif

#endif /* ifdef for trans_defines_h */
