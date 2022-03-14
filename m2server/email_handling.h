#ifndef EMAIL_HANDLING_H
#define EMAIL_HANDLING_H

const char * email_get_common_From();
const char * email_get_recipients_MaskPrep();
const char * email_get_recipients_Managers();
const char * email_get_recipients_Developers();
const char * email_get_recipients_All();

const char * email_check_recipients(const char * cur_recipients);

/* true when there is @nxp.com */
int email_isThereAnyInternalRecipient(const char *recipients);

#endif // EMAIL_HANDLING_H
