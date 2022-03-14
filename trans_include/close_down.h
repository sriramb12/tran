#ifndef close_down_h
#define close_down_h

/* This file contains different close down routines */
/* These routines are used by DB server and Mask Shop server */

/* Prints error num, error message, bad value, and help message */
void close_down1d(
/* Used with bad_req_type, bad_data_type, bad_version_flag */
    char *err_msg,
    char bad_val,
    register short err_no);

/* Prints error code and one string */
void close_down2d(
    char *err_msg,
    int bad_val,
    char *string,
    register short err_no);

/* Sends formatted message with one parameter and no help message */
void close_down1f(
    char *format,
    char *par1,
    register short err_no);

/* Sends formatted string with two parameters */
void close_down2f(
    char *err_msg,
    char *par1,
    char *par2,
    register short err_no);

/* Sends a formatted message with 3 parameters and no help message */
void close_down3f(
    char *format,
    char *par1,
    char *par2,
    char *par3,
    register short err_no);

/* Sends a formatted message with 4 parameters and no help message */
void close_down4f(
    char *format,
    char *par1,
    char *par2,
    char *par3,
    char *par4,
    register short err_no);

/* Sends a formatted message with 5 parameters and no help message */
void close_down5f(
    char *format,
    char *par1,
    char *par2,
    char *par3,
    char *par4,
    char *par5,
    register short err_no);

/* Sends formatted message with one parameters and a help message */
void close_down1p(
    char *format,
    char *par1,
    register short err_no);

/* Sends formatted message with two parameters and a help message */
void close_down2p(
    char *format,
    char *par1,
    char *par2,
    register short err_no);

/* Sends user error code, error message, quoted string, and help message */
void close_down1q(
    char *err_msg,
    char *file,
    register short err_no);

void close_down1m(
    char *err_msg,
    char *file,
    register short err_no);

/* Prints one error message, 2 quoted strings and help message */
void close_down2q(
    char *err_msg,
    char *file1,
    char *file2,
    register short err_no);

/* Sends one message and help message */
void close_down(
    char *err_msg,
    register short err_no);

/* Sends user error number, one message and help message */
void close_down1s(
    char *err_msg,
    register short err_no);

/* Sends error number and 2 messages on the same line */
void close_down2s(
    char *msg1,
    char *msg2,
    register short err_no);

/* Sends error code, 3 messages, and help message */
void close_down3s(
    char *err_msg,
    char *file,
    char *help,
    register short err_no);


/* Prints one error message, 2 quoted strings and help message */
/* Also sends the database administrator E-mail concerning the problem */
void close_down2m(
    char *err_msg,
    char *file1,
    char *file2,
    register short err_no);

#endif
