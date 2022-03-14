#ifndef maskshop_h
#define maskshop_h

#define MASKSHOP_LOG_MSG \
"Mask set '%s' has been successfully transferred to the\n\
Intercom repository.  The log generated by Intercom is shown\n\
below.  The receiving mask shop can retrieve the data with\n\
the file or secure keyword generated by InterCom.\n\n\
InterCom Log:\n\n\
%s\n\
A history of the transfer has been saved on your local disk\n\
in file 'send2maskshop_log'.  The log has also been saved in\n\
the Mask Designer database and can be accessed by reading or\n\
checking out mask set '%s'.  In addition, a copy of the\n\
log is provided below.\n\n"

#define TRANS_FTP_LOG_MSG \
"Mask set '%s' has been successfully transferred to the\n\
intermediate TransFTP data repository.  The log generated by\n\
the TransFTP server is shown below.  You will receive additional \n\
e-mail when transfer to the Mask shop via FTP completes.\n\n\
InterCom Log:\n\n\
%s\n\
A history of the transfer has been saved on your local disk\n\
in file 'send2maskshop_log'.  The log has also been saved in\n\
the Mask Designer database and can be accessed by reading or\n\
checking out mask set '%s'.  In addition, a copy of the\n\
log is provided below.\n\n"

#define INTERCOM_LOG_mSG \

/* Holds defined file types */
#define M2BINARY 'B'
#define M2ASCII 'T'


void tmp_log(char *log_file, char *msg);

void intercom_exit(char *err, TICKLER *tk, MASKSHOP *ms, char *tmp_dir);
short intercom_send(MASKSHOP *ms, TICKLER *tk, char *files[], char *contract_num);
short intercom_file_send(char *mask, char *file_keyword, char *file_name);
short intercom_file_put(char *file_name);
int intercom_defaults_set(void);

/* Get user's name and phone number from database  using whobu command */
short name_user_phone_rtv(TICKLER *tk);



/* Determine if specified file is binary or ASCII */
char get_file_type(char *file_name, char *sec_file_type);

/* Retrieve information about specified mask shop from mask shop table */
short maskshop_lookup(char *ms_tla, MASKSHOP *ms, int local_retrieve);


/* Determine if file name is an etec file name */
/* If it return DBYES */
/* Also convert to etec format */
short is_etec(char *etec_name, char *file_name);


/* Sends specified files to mask shop for specified mask */
/* Looks up information about user using X.500 user ID passed in */
/* Data transfer only done using InterCom */
/* Sets up data connection to specified mask shop and then spawns */
/* child process to perform data transfer, parent returns to calling program */
/* User is sent email when data transfer completes. */
short sendfiles2maskshop(char *maskshop, char *mask, char *files[], char *x500, char *project);

void close_down1f(char *err_msg, char *file);

void m2contract_file_send(char *mask, char *user_email, char *ms_name, char *ms_tla, char *contract_num);


/* Build list of contract files present */
short m2contract_names_find(char *mask, char *tla, char *files[]);


#endif /* define for maskshop_h */
