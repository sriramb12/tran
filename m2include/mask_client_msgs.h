#ifndef mask_client_msgs_h
#define mask_client_msgs_h
/* Mask Prep Client messages */
#define rd_open_msg ""

#define ftp_msg "Script created to send files to remote machine via ftp is in file:\n\n\
    '%s'\n\n\
Files sent via FTP are listed below."

#define dd_msg "Script created to transfer files to tape is in file:\n\n\
    '%s'\n\n\
Files found to write to tape are listed below."
#define ARES_ERR "Error -151:  Invalid input format.\n\n\
To assign a bump mask type '%s -a [mask] [bump mask] [pin]'\n\
To retrieve bump mask info type '%s -r [mask]'\n"

#define dbping_err "Error -151:  Invalid input format.\n\n\
    To access the Tempe database type  '%s tempe'\n\
    To access the Austin database type '%s austin'\n\
    To access the Tokyo database type  '%s tokyo'\n\
    To access the WACC database type   '%s wacc'\n"

#define ARES_MSG \
"      Welcome to ARES (Assign/Retrieve Software)\n\
      Free Scale Semiconductor Confidential Proprietary software\n\
        Not for distribution outside Free Scale\n\n\
To assign a bump mask type '%s -a [mask] [bump mask] '\n\
To retrieve bump mask info type '%s -r [mask]'\n"

#define dbping_msg \
"               Welcome to DBPing\n\
      Free Scale Semiconductor Confidential Proprietary software\n\
        Not for distribution outside Free Scale\n\n\
    To ping the Tempe database type  '%s tempe'\n\
    To ping the Austin database type '%s austin'\n\
    To ping the Tokyo database type  '%s tokyo'\n\
    To ping the WACC database type   '%s wacc'"
#endif /* ifdef for mask_client_msgs_h */
