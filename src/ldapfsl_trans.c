#include <string.h>
#include "ldapfsl.h"
#include "user_params.h"
#include "trans_server_defines.h"
//#include "trans_server_prototypes.h"
#include "trans_server_globals.h"
#include "global_debug.h"



/* Some high level functions used in Trancend tools */

void ldapfsl_setProductionLdapServer()
{
#if defined (DMZ) || defined(INTERCOM) || OS == Linux
     ldapfsl_setServerID(SERVER_NXP_Exranet_ID);
#else
    ldapfsl_setServerID(SERVER_NXP_Coreid_ID);
#endif
}


void ldapfsl_setDevelopmentLdapServer()
{
     ldapfsl_setServerID(SERVER_NXP_Coreid_ID);
}


int ldapfsl_exists_account_by_coreid(char *user_id)
{
     int ret = DBNO;
     LDAP_RETURN_DATA* user_info    = ldapfsl_lookupByCoreId(user_id);
     if (user_info != 0 && user_info->result == LDAP_SUCCESS)
     {
          ldapfsl_free_return_data(user_info);
          ret = DBYES;
     }
     DBG("ret=%d, user_id=%s", ret, user_id);
     return ret;
}


int ldapfsl_exists_account_by_email(const char *email_addr /*in*/)
{
     int ret = DBNO;
     LDAP_RETURN_DATA* user_info    = ldapfsl_lookupByMail(email_addr);
     if (user_info != 0 && user_info->result == LDAP_SUCCESS)
     {
          ldapfsl_free_return_data(user_info);
          ret = DBYES;
     }
     DBG("ret=%d, email_addr=%s", ret, email_addr);
     return ret;
}


LDAP_RETURN_DATA * ldapfsl_authenticate_user_id(char *user_id, const char *passwd)
{
    /* this is an oportunity to ignore authentication */
    int let_user_go = DBNO;   // special variable to bypass authentication
    LDAP_RETURN_DATA * ldap_ret = ldapfsl_authenticateCoreId(user_id, passwd);

    DBG("user_id=%s passwd=%s", user_id, passwd);

    if (BAD_LDAP_RETURN_DATA(ldap_ret))
    {
        LDAP_CONFIG * config = ldapfsl_getCurrentConfig();
        DBG("LDAP config=%p", config);
        if (config != 0)
        {
            DBG("connected=%d host=%s, port=%d", IS_LDAP_CONNECTION_ERROR(ldap_ret),
                config->host, config->ssl_port);
        }
#if defined(LDAP_BYPASS_AUTHENTICATION)
        let_user_go = DBYES;
#else
      {
            FILE * super_user_file = fopen("db_admin/users_bypass_authentication.txt", "r");
            if (super_user_file != NULL)
            {
                char super_user[256] = {0};
                while (let_user_go == DBNO && fscanf(super_user_file, "%s", super_user) == 1)
                {
                   if (strcasecmp(super_user, user_id) == 0)
                   {
                       let_user_go = DBYES;
                   }
                }
                fclose(super_user_file);
            }
      }
#endif
    }

    if (let_user_go == DBYES)
    {
        ldapfsl_free_return_data(ldap_ret);
        ldap_ret = ldapfsl_lookupByCoreId(user_id);
    }
    return ldap_ret;
}



int ldapfsl_user_authenticate_and_copy_global_structures(char *user_id, const char *passwd)
{
    int ret = DBNO;
    LDAP_RETURN_DATA * ldap_ret = ldapfsl_authenticate_user_id(user_id, passwd);
    if (GOOD_LDAP_RETURN_DATA(ldap_ret))
    {
        ret = DBYES;
        ldapfsl_copy_info_to_global_trans_structure(ldap_ret);
        ldapfsl_copy_info_to_global_user_structure(ldap_ret);
    }
    ldapfsl_free_return_data(ldap_ret);
    DBG("ret=%d", ret);
    return ret;
}


int ldapfsl_user_authenticate(char *user_id, const char *passwd)
{
    int ret = DBNO;
    LDAP_RETURN_DATA * ldap_ret = ldapfsl_authenticate_user_id(user_id, passwd);
    if (GOOD_LDAP_RETURN_DATA(ldap_ret))
    {
        ret = DBYES;
    }
    ldapfsl_free_return_data(ldap_ret);
    DBG("ret=%d", ret);
    return ret;
}

int ldapfsl_user_lookup_and_copy_global_structures(char *user_id)
{
     int ret = DBNO;
     LDAP_RETURN_DATA * ldap_ret = ldapfsl_lookupByCoreId(user_id);
     if (GOOD_LDAP_RETURN_DATA(ldap_ret))
     {
          ldapfsl_copy_info_to_global_trans_structure(ldap_ret);
          ldapfsl_copy_info_to_global_user_structure(ldap_ret);
          ret = DBYES;
     }
     ldapfsl_free_return_data(ldap_ret);
     DBG("ret=%d, user_id=%s", ret, user_id);
     return ret;
}

void  ldapfsl_copy_info_to_global_trans_structure(LDAP_RETURN_DATA * ldap_ret)
{
     extern TRANS_PARAMS gu;
     if (GOOD_LDAP_RETURN_DATA(ldap_ret))
     {
          strcpy(gu.email1, ldap_ret->info[0]->friendly_email);
          strcpy(gu.email2, ldap_ret->info[0]->friendly_email);
     }
}


int ldapfsl_get_user_full_name(char *user_id, char *name)
{
    extern USER_PARAMS user;
    int ret = DBNO;
    name[0] = 0;
    if (ldapfsl_user_lookup_and_copy_global_structures(user_id) == DBYES)
    {
        strcpy(name, user.name);
        ret = DBYES;
    }
    DBG("ret=%d, user_id=%s", ret, user_id);
    return ret;
}


int ldapfsl_get_user_name_phone(char *user_id, char *name, char *phone)
{
    extern USER_PARAMS user;
    int ret = ldapfsl_get_user_full_name(user_id, name);
    phone[0] = 0;
    if (ret == DBYES)
    {
        strcpy(phone, user.phone);
    }
    return ret;
}



int ldapfsl_get_user_name_dept(char *user_id, char *name, char *dept)
{
    extern USER_PARAMS user;
    int ret = ldapfsl_get_user_full_name(user_id, name);
    dept[0] = 0;
    if (ret == DBYES)
    {
        strcpy(dept, user.dept);
    }
    return ret;
}


int ldapfsl_get_all_fields(char *user_id, char fields[USER_ROWS][USER_COLS])
{
    int ret = -1;
    int counter = 0;
    for (counter = 0; counter < USER_ROWS; ++counter)
    {
        fields[counter][0] = 0;
    }
    if (ldapfsl_user_lookup_and_copy_global_structures(user_id) == DBYES)
    {
        ret = 0;
        char name[256];
        char *pt = 0;
        strcpy(fields[CN],                user.login);
        strcpy (fields[USER_ID],          user.login);
        strcpy(fields[FRIENDLY_EMAIL],    user.email);
        strcpy(fields[MAIL_DROP],         user.email);
        strcpy(fields[NAME],              user.name);
        strcpy(fields[PHONE],             user.phone);
        strcpy(fields[DEPT],              user.dept);
        strcpy(fields[LOC],               user.site );
        strcpy(name, user.name);
        if ((pt = strchr(name, ' ')))
        {
            *pt = 0;
            strncpy(fields[FIRST_NAME], name, sizeof(fields[LAST_NAME]) -1);
            *pt = ' ';
            while (*pt == ' ' && *pt != 0)
            {
                pt++;
            }
            if (*pt != 0)
            {
                strncpy(fields[LAST_NAME], pt, sizeof(fields[LAST_NAME]) -1);
            }
        }
        else
        {
            strncpy(fields[FIRST_NAME], name, sizeof(fields[FIRST_NAME]) -1);
        }
    }
    return ret;
}

void ldapfsl_copy_info_to_global_user_structure(LDAP_RETURN_DATA * ldap_ret)
{
     extern USER_PARAMS user;        // global_include/user_params.h
     if (GOOD_LDAP_RETURN_DATA(ldap_ret))
     {
          strcpy(user.email,  ldap_ret->info[0]->friendly_email);
          strcpy(user.name,   ldap_ret->info[0]->name);
          strcpy(user.site,   ldap_ret->info[0]->site);
          strcpy(user.login,  ldap_ret->info[0]->coreId);
          strcpy(user.phone,  ldap_ret->info[0]->extra_info_4);
          strcpy(user.dept,   ldap_ret->info[0]->extra_info_5);
          user.ldap_ok = DBYES;
     }
}


/*!
   convert_emailString_2_list()
           \brief receives a string of emails or coreid/wbi-id and expand it to emails in a list
   In the string Items are separated by spaces

   The email is checked against LDAP
*/
int convert_emailString_2_list(char *string, char *list[])
{
    int cnt;
    int len;
    char list_item[1024];
    char *ptr = string;

    DBG();

    /* Add a space at the end of the e-mail address */
    strcat(string, " ");

    for (cnt = 0; sscanf(ptr, "%s", list_item) == 1; ++cnt)
    {
        LDAP_RETURN_DATA * ldap_ret =  ldapfsl_lookupByCoreId(list_item);
        if (BAD_LDAP_RETURN_DATA(ldap_ret) && strchr(list_item,  '@') != NULL)
        {
            ldapfsl_free_return_data(ldap_ret);
            // handle a email already set, try to find it by email
            ldap_ret = ldapfsl_lookupByMail(list_item);
            if (BAD_LDAP_RETURN_DATA(ldap_ret)) // if email not found try to cut it (perhaps wrong email)
            {
                char email_tmp[256];
                char *at = NULL;
                ldapfsl_free_return_data(ldap_ret);
                strcpy(email_tmp, list_item);
                if ((at = strchr(email_tmp, '@')))
                {
                    *at = '\0';
                    ldap_ret =  ldapfsl_lookupByCoreId(email_tmp);
                }
            }
        }
        // len is used to walk in the list
        len = strlen(list_item)+1;
        if (GOOD_LDAP_RETURN_DATA(ldap_ret)) // get right email
        {
            if (ldap_ret->info[0]->friendly_email[0] != 0)  // external users may have their email empty
            {
                strcpy(list_item, ldap_ret->info[0]->friendly_email);
            }
            else if (strchr(ldap_ret->info[0]->coreId, '@'))
            {
                strcpy(list_item, ldap_ret->info[0]->coreId);
            }
            list[cnt] = malloc(strlen(list_item)+1);
        }
        else
        {
             list[cnt] = malloc(len);
        }
        ldapfsl_free_return_data(ldap_ret);
        strcpy(list[cnt], list_item);
        ptr += len;

        if (ptr == NULL) break;
    }

    /* Put null at end of string */
    list[cnt] = NULL;

    DBG("cnt=%d", cnt);
    return(cnt);
}

