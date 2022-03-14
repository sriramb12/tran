#ifdef M2
/****************************************************************************
 *
 * Function:	rmt_man_reg
 *
 * Description: Sends registration request to remote servers 
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include "reg_globals.h"
#include "global_defines.h"
#include "global_params.h"
#include "global_reg_prototypes.h"
#include "global_server_params.h"
#include "global_server_prototypes.h"
#include "global_server_errs.h"
#include "user_params.h"
#include "rmt_server.h"
#include "rtn.h"
#include "strcpy.h"
#include "registry_prototypes.h"

void rmt_man_reg(char req_type, char *site, char *email1, char *email2)
{
    char tmp_password[24];
    register int cnt;
    short ret;

    /* Build the password and encrypt it */
    /* Mask Prep uses a different password than Transcend and Intercom */
    strcpy2(tmp_password, user.login, RMT_PWD_KEY);
    tmp_password[PWD_LEN] = '\0';
    /* Encrypt password and put into first 12 characters of buffer */
    password_encrypt(bf.buffer, tmp_password, PWD_LEN);
    /* Build header to send to remote server */
    sprintf(bf.buffer+12, " %c %c %c %c %c \
 l 1.000 l 1.000\
 %s %s %s 0\
 %s %s %s %s %s\
 %s %s %s %s %s %d %s",
    /* Specific request type, general request type, type of data, etc */
    req_type, REGISTER_REQ, GLBL, DBNO,  DBNO_REQ,
    /* No versioning done, but parameters still needed */
    user.login, user.mach, user.addr, 
    /* Site, Email address, badge, place holds for PIN */
    site, email1, email2, NONE, NONE,
    /* first name, last name, phone, location code, mail drop */
    /* These are just place holders not needed in manual registration */
    NONE, NONE, NONE, NONE, NONE, 0, user.name);
    /* Send registration request to remote servers */
    for (cnt = 0; cnt < rmt_mpd.server_num; ++cnt)
    {
	/* Open the socket for the server */
	ret = init_rmt_socket(bf.buffer, &rmt_mpd, cnt);
    if (ret == 0) ret = get_rmt_short();
	get_rmt_string(rtn.msg);
	close(sock);
	data_log(tr.log_file,  rtn.msg);
	/* If the transaction was not successful, save to resend later */
	if (ret != SUCCESS)
	{
	    /* Build string to save */
	    strcpy5(rtn.msg, site, " ", email1, " ", email2);
	    /* Build name of file where data stored */
	    strcpy2(bf.file_name, rmt_mpd.site[cnt], ".man_reg");
	    /* Add the entry */
	    add_file_entry("resend_data", bf.file_name, rtn.msg);
	} /* End if for unsuccessful transaction */
    } /* End for statement */
} /* End rmt_man_reg */
#endif
