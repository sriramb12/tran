#ifndef x500_defines_h
#define x500_defines_h

/* Return values from x500 functions */
#define VALID_LOGIN 200
#define PWD_CHANGED 200
#define X500INVALID_USERID 410
#define X500NONUNIQUE_USERID 415
#define X500PWD_INCORRECT 420
#define X500PWD_EXPIRED 421
#define X500SUSPENDED_ACCOUNT 422
#define X500PWD_STATUS_UNKNOWN 429
#define PWD_SAME 430
#define PWD_ILLEGAL 431
#define PWD_LEN_ERR 432

/* Defines for x500 errors */
#define pwd_incorrect "Error -720:    Incorrect One IT applications directory password entered.  Please  \n\
               try again.\n\n\
               You can set up your One IT applications directory password at web site\n\
               http://password.freescale.net.\n\n\
               If you have questions, contact Transcend team, \n\
               transbug@nxp..com"
#define PWD_INCORRECT -720
#define pwd_authen_wrn "Warning -721:  The One IT applications directory password you entered was incorrect.\n\
               However, authentication was done by verifying\n\
               your name in the email core directory matches \n\
               your name in your local password file.\n\n"
#define PWD_AUTHEN_WRN -721
#define invalid_userid   "Error -722:    Invalid core ID entered.  Please try again.\n\
               If you do not have a core ID, contact\n\
               your local system administrator or email\n\
               administrator.  If you have additional questions, \n\
               contact Transcend team, transbug@nxp.com."
#define INVALID_USERID -722
#define nonunique_userid "Error -723:  Non-unique core ID entered."
#define NONUNIQUE_USERID -723
#define pwd_expired "Error -724:    Your One IT applications directory password has expired and must be changed.\n\
               Please change your One IT applications directory password on the SPS Directory  \n\
               Services Web Page at http://password.freescale.net."
#define PWD_EXPIRED -724
#define suspended_account "Error -725:  Your OneIT user account has been suspended."
#define SUSPENDED_ACCOUNT -725

/* Verify if x500 User ID and password are correct */
char *vfypwd(
    char *server1,
    char *server2,
    int time_out,
    char *user_id,
    char *pwd);

/* Change One IT applications directory password */
char *
/* No longer supported by IS */
chgpwd(
    char *server1,
    char *server2,
    int time_out,
    char *user_id,
    char *old_pwd,
    char *new_pwd);

#endif
