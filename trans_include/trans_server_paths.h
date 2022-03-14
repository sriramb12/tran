#ifndef trans_server_paths_h
#define trans_server_paths_h

#define DT_TRANSACTIONS                 "dt_transactions"
#define EXT_REGISTRY                    "ext_registry/site_reg.xx"
#define GRP_REGISTRY                    "grp_registry/site_reg.xx"
#define EDC_REGISTRY                    "edc_registry/site_reg.xx"
#define MASK_FILE                       "db_admin/mask_file"
#define SUPPORT_EMAIL_LIST              "db_admin/support_email_list"
#define SUPPORT_EMAIL_ROOT              "support_email_list"
#define SUPPORT_EMAIL_ADMIN             "db_admin/support_email_admin"
#define MICA_EMAIL_LIST                 "db_admin/mica_email_list"
#define MICA_EMAIL_ROOT                 "mica_email_list"
#define ECHUCA_EMAIL_LIST               "db_admin/echuca_email_list"
#define MICA_EMAIL_ADMIN                "db_admin/mica_email_admin"
#define LOG_REQUEST_FILE                "db_admin/log_request_file"
#define MAILLIST_AUTH_FILE              "db_admin/maillist_file"
#define RELEASE_FILE                    "db_admin/trans_release_file"

#define LOG_REPORT_REQUEST_FILE "db_admin/log_report_request_file"




#if defined(DEVEL)
#define TRANSWEB_COMPANY_TABLE      "db_admin/transweb_company.tbl.debug"
#else
#define TRANSWEB_COMPANY_TABLE      "db_admin/transweb_company.tbl"
#endif

#define TRANSWEB_REPOSITORY_TABLE   "db_admin/transweb_repository.tbl"

/* Setup for the Devel server running at port 6928 */
#ifdef SSL_ENCRYPTION

#if defined(DEVEL)
#define GLOBAL_COMPANY_TABLE    "db_admin/ssl.company_table.devel"
#else
#define GLOBAL_COMPANY_TABLE    "db_admin/ssl.company_table"
#endif

#if defined(DEVEL)
#define COMPANY_FILE            "db_admin/ssl.company_file.devel"
#else
#define COMPANY_FILE            "db_admin/ssl.company_file"
#endif

#else
#define GLOBAL_COMPANY_TABLE    "db_admin/company_table"
#define COMPANY_FILE            "db_admin/company_file"
#endif

#define TRANS_DISK_FILE     "db_admin/disk_usage"
#define KEYWORD_FILE        "db_admin/keyword_file"
#define KEYWORD_DIR         "db_admin/keywords/"

#define RELEASE_KEYWORD_FILE    "db_admin/release_keyword_file"
#define COMPANY_EMAIL_FILE      "db_admin/company_email"
#define GLOBAL_EMAIL_FILE       "db_admin/global_email"
#define EMAIL_REG_PATH          "registry/email_reg."
#define SITE_REG_PATH           "edc_registry/site_reg."
#define RESEND_FILE             "db_admin/resend"
#define GLOBAL_PARAM_FILE       "db_admin/global_params"
#define GLOBAL_FILE_PATH        "global_data/"
#define HISTORY_FILE_DIR        "history_data"
#define HISTORY_FILE_PATH       "history_data/"
#define GLOBAL_FILE_DIR         "global_data"

#define GLOBAL_FILE_PATH_LEN 12
#define HISTORY_FILE_PATH_LEN 13
#define MAX_SERVERS 5

#define BIG_FILE_PATH       "big_files/"
#define BIG_FILE_TABLE      "global_data/big_file_table"
#define BIG_FILE_PATH       "big_files/"
#define SITE_REG_REQUEST    "db_admin/reg_request"
#define TMP_SITE_FILE       "registry/tmp_files/tmp_site_file"
#define RMT_X500FILE        "cserv1:emssend.x500/x500.fullz"
#define TMP1X500FILE        "tmp_registry/off.reg.tar.Z"
#define TMP2X500FILE        "tmp_registry/off.reg"
#define X500FILE            "registry/off.reg"

#endif /* ifdef for trans_server_paths_h */
