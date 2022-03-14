#ifndef trans_server_errs_h
#define trans_server_errs_h
#define BAD_GET -112
/* Defines for Transcend and InterCom errors follow */

#define encryption_err "Error -502:  Unable to decrypt keyword and find associated file  keyword.\n\
             Please verify you received the personal secure keyword in the same\n\
             account you are currently using  and that the Email  address\n\
             listed  below is the one the originator used to send you the\n\
             personal secure keyword:\n\n\
             Current Site:   %s\n\
             Email address:  %s"
#define ENCRYPTION_ERR -502
#define bad_disk_path "Error -503:  Invalid disk path '%s' found for disk partition '%d'"
#define BAD_DISK_PATH -503
#define bad_site "Error -504:  Invalid site:  "
#define BAD_SITE -504
#define email_not_fnd "Error -505:  Unable to find Email address for your site:"
#define EMAIL_NOT_FND -505
#define trans_err "Error -506:    Transfer of '%s' is still in progress.\n\
               Please wait a few minutes and try the retrieval again.\n\n"
#define TRANS_ERR -506
#define external_encryption_auth_err "Error -507:    You are not authorized to access\n\
               data with personal secure keyword '%s'.\n\n\
               Only the customer who received the email message with \n\
               the personal secure keyword can use the keyword to\n\
               access the data.\n\n\
The keyword was sent to: %s.\n\n\
Your registered e-mail address at the %s site is:\n\n\
               '%s'"
#define internal_encryption_auth_err "Error -507:    You are not authorized to access\n\
               data with personal secure keyword '%s'.\n\n\
               Only the customer who received the email message with\n\
               the personal secure keyword can use the keyword to\n\
               access the data.\n\n\
The keyword was sent to: %s.\n\n\
Your credentials at the %s site are:\n\n\
User Id:        %s\n\
E-mail address: %s"
#define ENCRYPTION_AUTH_ERR -507
#define no_encryption_key "Error -508:  No entry for personal secure key '%s' present in %s."
#define NO_ENCRYPTION_KEY -508
#define bad_size "Error -509:    File sent to %s repository was not the correcct size.\n\n\
               File name:  %s\n\
               File keyword:  %s\n\
               Expected size:  %llu bytes\n\
               Actual size:    %llu bytes"
#define BAD_SIZE -509
#define bad_checksum "Error -510:    File sent to %s repository does not have the correct checksum.\n\n\
               Expected checksum:  %lld\n\
               Actual checksum:    %lld"
#define BAD_CHECKSUM -510
#define block_err "Error -511:    Data transfer of '%s' has been blocked by:\n\n\
               Name:  %s\n\
               Email:  %s@nxp.com"
#define BLOCK_ERR -511
#define alr_lock_err "Error -512:  Retrieval lock already set on '"
#define ALR_LOCK_ERR -512
#define not_lock_err "Error -513:  Retrieval lock has not been set on '"
#define NOT_LOCK_ERR -513
/* Data locked for zip */
#define zip_lock_err "Error -506:    Compression of '%s' is still in progress.\n\
               Please wait a few minutes and try the retrieval again.\n\n"
#define ZIP_LOCK_ERR -515
#define final_trans_lock_wrn "\n\nError -516:    You have entered an invalid file keyword seven\n\
               or more consecutive times.  To maintain adequate\n\
               Transcend security, you have been locked out from\n\
               using Transcend."
#define final_intercom_lock_wrn "\n\nError -516:    You have entered an invalid file keyword seven\n\
               or more consecutive times.  To maintain adequate\n\
               InterCom security, you have been locked out from\n\
               using InterCom."
#define FINAL_LOCK_WRN -516
#define trans_lock_err "Error -517:    You have been locked out of %s at the %s site.\n\n"
#define intercom_lock_err "Error -517:    You have been locked out of InterCom.\n\n"
#define TRANS_LOCK_ERR -517
#define auto_reg_req "Warning -518:    Requesting new Transcend registration for:\n\n\
    Login:  %s\n\
    IP node address:  %s\n\
    Email Address:  %s\n\
    Badge:  %s\n\
    Name:  %s\n\
    Machine:  %s"
#define AUTO_REG_REQ -518
#define auto_reg_wrn "Warning -519:    Requesting Transcend registration for:\n\n\
               IP node address:  %s\n\
               Login:  %s\n\
               Email Address:  %s\n\
          Name:  %s\n\
	               Machine:  %s"
#define AUTO_REG_WRN -519
#ifdef TRANSCEND
#define reg_err "Error -520:    You are not registered to use Transcend."
#else
#define reg_err "Error -520:    You are not registered to use InterCom."
#endif
#define REG_ERR -520
#define site_entry_err "Error -521:  Entry not found in site table for '"
#define SITE_ENTRY_ERR -521
#define disk_err "Error -523:  There is no more disk space available in the %s repository..%s%s"
#define DISK_ERR -523
/* This value cannot be changed unless the corresponding one in trans_client_errs.h is changed */
#define own_err "Error -424:    You did not create %s '%s'.\n\
               This transaction cannot be performed on data you do not own."
#define OWN_ERR -424
#define extend_err  "Error -525:    You have already extended the expiration date on\n\
               %s '%s' 3 working days.  No more \n\
               expiration date extensions are allowed."
#define EXTEND_ERR -525
#define total1extend_err "Error -526:    You have already extended the expiration date on \n\
               %s '%s' two working days.  You can\n\
               only extend the expiration date one additional day."
#define total2extend_err "Error -526:    You have already extended the expiration date on \n\
               %s '%s' one working day.  You can\n\
               only extend the expiration date two additional days."
#define TOTAL_EXTEND_ERR -526
#define external_err "Error -530:  External server not responding."
#define EXTERNAL_ERR -530
#define external_open_err "Error -531:  Unable to access data returned from external server."
#define EXTERNAL_OPEN_ERR -531
#define external_read_err "Error -532:  Unable to read data received from external server."
#define EXTERNAL_READ_ERR -532
#define no_file_err "Error -534:  No file provided to process."
#define NO_FILE_ERR -534
#define WRONG_SERVER -535
#define company_err "Error -535:    Requested data is owned by %s.  %s is \n\
               not authorized to access data owned by this company."
#define COMPANY_ERR -536
#define intercompany_err "Error -536:    Requested data is owned by %s.  Data \n\
               transfer cannot be done within the same company."
#define INTERCOMPANY_ERR -536
#define external_transfer_err "Error -536:    You are signed in as an external customer, and this data was uploaded by another external customer.\n\
               Data transfer cannot be done between two external customers."
#define EXTERNAL_TRANSFER_ERR -536
#define wrong_destiny "Error -537:    Data with keyword '%s' is being transferred from %s to %s.\n\
               Users at %s are not allowed to retrieve this data."
#define WRONG_DESTINY -537
#define ftp_write_err "Error -538:  Received error while writing to file in remote repository:\n\n\
               File name:  %s\n\
               Unix error no:  %d\n\
               Error message:  %s\n\n"
#define FTP_WRITE_ERR -538
#define reg_problem_err "\nError -539:  Site entries deleted from file '%s'\n\
               Backup copy of file from .delta directory \n\
               merged with original file."
#define REG_PROBLEM_ERR -539
#define no_query "Error -540:  The query command is not implemented on this server."
#define NO_QUERY -540
#define owner_dlt_err "Error -541:  Delete lock already set on %s '%s'."
#define OWNER_DLT_ERR -541
#define user_dlt_err "Warning -542:  No changes were made because the delete lock is not set \n\
               for %s '%s'."
#define USER_DLT_ERR -542
#define dlt_lock_err "Error -543:    You cannot delete %s '%s' because it \n\
               has been locked for deletion by the owner:\n\n\
               Name:   %s\n\
               Email:  %s@nxp.com"
#define DLT_LOCK_ERR -543
#ifdef INTERCOM
#define zero_len_email \
"Error -544:    Zero length email address for user retrieved\n\
               from the InterCom registry."
#endif
#ifdef TRANSCEND
#define zero_len_email \
"Error -544:    Zero length email address for user retrieved\n\
               from the Transcend registry."
#endif
#ifdef TRANSWEB
#define zero_len_email \
"Error -544:    Zero length email address for user retrieved\n\
               from the TransWeb registry."
#endif
#define ZERO_LEN_EMAIL -544

#define no_file_key_wrn "Warning -545:  No file keywords found for '%s' in the %s repository."
#define NO_FILE_KEY_WRN -545
#define no_keyword_wrn "Warning -546:  No personal secure keywords found for '%s' in the %s repository."
#define NO_KEYWORD_WRN -546
#define key_mapping_err "Error -547:  Unable to find associated file keyword for personal secure keyword:"
#define KEY_MAPPING_ERR -547
#define bad_company "Error -548:  Invalid company specified.\n\n"
#define BAD_COMPANY -548
#define no_email "Warning -549:  No email present to send to customers."
#define NO_EMAIL -549
#define transweb_reg_err "Error -550:  Entry for '%s' not present in the TransWeb registry."
#define TRANSWEB_REG_ERR -550
#define user_reg_err "Error -551:    Registration cannot be performed because you are logged in under another user's NXP Semiconductor core ID.\n\n\
               Your core ID:  %s\n\
               Other user's core ID:  %s"
#define USER_REG_ERR -551
#define dir_wrn "\n\nTransWeb does not support directory transfer.  thus, the directory\n\
               was retrieved as a tar file:\n\n\
               '%s.tar'\n\n\
               You can unpack the tar file using tar or Winzip.  To unpack the data using tar, type\n\n\
               'tar xf %s.tar'"
#define files_wrn "\n\nTransWeb does not support multiple file transfer.  thus, the files\n\
               were retrieved as a tar file:\n\n\
               '%s.tar'\n\n\
               You can unpack the tar file using tar or Winzip.  To unpack the data using tar, type\n\n\
               'tar xf %s.tar'"
#define ftp_resend_err "Error -552:  This option is supported only for data being transferred using TransNet.\n\
               No support is provided for data being transferred using InterCom."
#define FTP_RESEND_ERR -552
#define fss_auth_err  \
"Error -553:    You are not authorized to access data using \n\
               the file keyword.  You must use the personal\n\
               secure keyword send via email to access the data."
#define FSS_AUTH_ERR -553 
#define fss_email_err \
"Error -554:    Authorization to upload files denied because an email\n\
               address for a NXP customer was not specified.  You\n\
               must send a personal secure keyword to at least one NXP\n\
               customer.  E-mail addresses for NXP customers \n\
               end with the suffix '@nxp.com'."
#define FSS_EMAIL_ERR -554
#define external_email_err "Error -555:    Data transfer was aborted because you entered an e-mail\n\
               address for a customer that does not work at NXP.\n\
               Data transfer can be done only to NXP customers, and\n\
               all e-mail addresses must end in '@nxp.com'."
#define EXTERNAL_EMAIL_ERR -555
#define external_email_wrn \
"Warning -555:  You did not send a personal secure keyword to the external TransWeb\n\
               customer.   The customer needs both the personal secure keyword\n\
               and the file keyword to access the data.   To send E-mail\n\
               notification to one or more external customers, select 'Send\n\
               E-mail notification' under 'modify A file'.\n\n"
#define EXTERNAL_EMAIL_WRN -554
#define release_upgrade_err \
"Error -556:    The version of InterCom you are using is no longer supported.\n\
               You must install the latest release.\n\n\
               You can retrieve the latest release from our web site:\n\n\
               http://intercom.freescale.net.\n\n\
               You can also retrieve it with Intercom using the '-r' option.\n\n\
               To see commands for retrieving releases, type 'icom -r'"
#define 	RELEASE_UPGRADE_ERR -556
#define transweb_auth_err \
"Error -557:    You did not enter a correct file keyword.  You must\n\
               correctly enter both the file keyword and personal secure to\n\
               access the data.\n\n"
#define TRANSWEB_AUTH_ERR -557
#define secure_suffix_err \
"Error -558:    Invalid personal secure keyword extension of '%c' entered.  The\n\
               ninth character for personal secure keywords at this site is '%c'\n\n"
#define SECURE_SUFFIX_ERR -558
#define file_suffix_err \
"Error -559:    Invalid file keyword extension of '%c' entered.  The\n\
               ninth character for file keywords at this site is '%c'\n\n"
#define FILE_SUFFIX_ERR -559
#define invalid_file_key \
"Error -560:    Incompatible file/personal secure keyword entered.    This  error\n\
               usually occurs when the file and personal secure keywords are not for\n\
               the same file.\n\n%s"
#define INVALID_FILE_KEY -560
#define external_access_err \
"Error -561:    Invalid TransWeb site reached.  this site is for data\n\
               exchange between NXP and external TransWeb customers.\n\n"
#define EXTERNAL_ACCESS_ERR -561
#define key_dlt_err "Error -562:  Data with keyword '%s' has been deleted."
#define KEY_DLT_ERR -562
#define nxp_email_err \
"Error -563:    Your transaction was aborted because an e-mail address for\n\
               an external customer was entered while transferring data to\n\
               the %s repository.   this repository is for  data\n\
               transfer within NXP.  You cannot specify the e-mail\n\
               address of an external customer when transferring data to\n\
               this repository.\n\n\
               to transfer data to the %s repository,  enter a valid\n\
               NXP E-mail address.  To transfer data to an external\n\
               customer,  select 'External TransWeb  Customer (-etc)  as\n\
               the receiving repository or upload the data using the\n\
               'Transfer File to External Customer' link.\n\n"
#define NXP_EMAIL_ERR -563
#define support_access_err \
"Error -564:    You are not authorized to access data for personal secure \n\
               keywords sent to NXP Support."
#define SUPPORT_ACCESS_ERR -564
#define support_auth_err \
"Error -565:    You are not authorized to transfer data as NXP Support."
#define SUPPORT_AUTH_ERR -565
#define mica_access_err \
"Error -566:    You are not authorized to access data for personal secure \n\
               keywords sent to Mica Support."
#define MICA_ACCESS_ERR -566
#define mica_auth_err \
"Error -567:    You are not authorized to transfer data as Mica Support."
#define MICA_AUTH_ERR -567
#define mask_update_err \
"Error -568:    Unable to find mask/keyword entry for:\n\n\
               Mask set:  %s\n\
               File key:  %s"
#define MASK_UPDATE_ERR -568
#define not_itar_data \
"Error -569:    This data is being transferred from 'medusa'.   Only ITAR\n\
               data can be transferred from medusa.  Please transfer the\n\
               data from a different machine."
#define NOT_ITAR_DATA -569
#define encrypt_err \
"Error -570:    Failed to create encrypted file.  This error usually\n\
               occurs when the encryption key cannot be located.\n\
               Data transfer to %s has been aborted."
#define ENCRYPT_ERR -570
#define file_update_err "Error -571:    Transfer of '%s' aborted by the Transcend server\
               because data on the server is no longer being updated."
#define FILE_UPDATE_ERR -571
#define shmem_alloc_err "Error -575:  Unable to allocate shared memory."
#define SHMEM_ALLOC_ERR -575
#define shmem_attach_err "Error -576:  Unable to attach to shared memory."
#define SHMEM_ATTACH_ERR -576
#define shmem_remove_err "Error -577:  Unable to remove shared memory."
#define SHMEM_REMOVE_ERR -577
#define old_release \
"Error -578:    You are using version 7.0 of Transcend.  This software is no\n\
               longer supported.  Please upgrade to the latest version of\n\
               Transcend.  The new release can be retrieved from our web\n\
               site: http://transcend.freescale.net.  It can also be\n\
               retrieved using Transcend with the -r option.  To see a list\n\
               of commands that can be used to retrieve new releases, type\n\
               'trans -r'."
#define OLD_RELEASE -578
#define invalid_encrypted_core_id "Error -579:  Invalid encrypted Core ID entered."
#define invalid_old_encrypted_core_id "Error -579:      This version of Transcend is no longer supported because of security risks.\n\
               Please retrieve the latest version from our web site.\n\
               URL:  http://transcend.freescale.net."
#define INVALID_ENCRYPTED_CORE_ID -579
#define dtms_upgrade_wrn \
"Warning -580:  You must use version 7.2 of Transcend to transfer data to DTMS:\n\
Release web site:  http://transcend.freescale.net\n\
Transcend release command:  trans -r"
#define DTMS_UPGRADE_WRN -580
#define trans_non_err "Error -581:    Keyword '%s' not found.\n\
               Please verify you entered the keyword correctly."
#define TRANS_NON_ERR -581
#define stat_err "Error -582:    Unable to access file '%s'"
#define STAT_ERR -582
#define already_zipped_err \
"Error -882:  %s '%s' is already zipped."
#define ALREADY_ZIPPED_ERR -882
#define registration_url_err \
"Error -883:    You must authenticate with your External LDAP account to use\n\
               InterCom.\n\n\
                            To register for an account go to URL:\n\
               https://www.nxp.com/webapp-signup/register\n\n"
#define REGISTRATION_URL_ERR -883 
#define free_user_err \
"Error -884:    This Intercom software cannot be used by NXP customers.\n\
               Please download the NXP Intercom release from URL:\n\
               http://intercom.freescale.net.\n\n\
               You can also transfer data to external customers from URL:\n\
               https://nww.transweb.nxp.com"
#define FREE_USER_ERR -884
#define expiration_extend_err \
"Error -885:    This data has no expiration date."
#define EXPIRATION_EXTEND_ERR -885
#define subnet_err "Error -886:  Specified subnet not verified when encrypted password created."
#define SUBNET_ERR -886
#define phot_err "Error -888:  Data transfer to Photronics (TransWeb ITAR) was aborted\n\
               because you are attempting to transfer the data using\n\
               version %s of InterCom.  Transfer of ITAR data to Photronics\n\
               is not supported for this version of InterCom.  Please\n\
               upgrade to version 4.5 or later to transfer the data."
#define PHOT_ERR -888
#define extern_autoreg_err \
"Error -889:  Auto registration not supported for external customers."
#define EXTERN_AUTOREG_ERR -889
#define invalid_trans_version \
"Error -890:  This version of Intercom can no longer be used for data transfer to the Maskshop."
#define INVALID_TRANS_VERSION -890

#endif /* ifdef for trans_server_errs_h */
