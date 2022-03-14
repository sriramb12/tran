/************************************************
# $Id: ldapfsl.h.rca 1.2 Fri Jul  4 09:48:37 2014 B35201 Experimental B35201 $
# $Author: B35201 $
# $Date: Fri Jul  4 09:48:37 2014 $
# $Source: /sync/vault/8000/sv/Projects/piams/dev/Ldap/ldapfsl.h.rca $
#
# (C) Copyright 2012, Freescale, all rights reserved.
#
# Procedures list :
#
************************************************
*$Log: ldapfsl.h.rca $
*
* Revision: 1.2 Fri Jul  4 09:48:37 2014 B35201
* converted to use OpenLDAP and compatible with Qt5
*
* Revision: 1.1 Mon Feb 18 20:38:59 2013 b35201
* Adding LDAP stuff
*
************************************************/

#ifndef LDAPFSL_H
#define LDAPFSL_H

#include <stdio.h>

/*
 *  default is to use OpenLDAP
 */
#if !defined(USING_MOZILLA_LDAP) && !defined(USING_OPEN_LDAP)
# define USING_OPEN_LDAP 1
#endif

#if  !defined(LDAP_SUCCESS) && !defined(LDAPFSL_SOURCE)
# define LDAP_SUCCESS 0
#endif

#define GOOD_LDAP_RETURN_DATA(ret)  (ret != 0 && ret->result == LDAP_SUCCESS)
#define BAD_LDAP_RETURN_DATA(ret)   (!GOOD_LDAP_RETURN_DATA(ret))
#define IS_LDAP_CONNECTION_ERROR(ret)  (ret != 0 && ret->connected_code != 0 )

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LDAP_INFO_SIZE  256
#define MAX_LDAP_SITE_SIZE  48

#define LDAP_RESULT_ATTRIBUTE_SIZE  128

enum
{
    LDAP_CONFIG_SYSTEM_INVALID,
    LDAP_CONFIG_SYSTEM_API,
    LDAP_CONFIG_SYSTEM_LDAPSEARCH,
    LDAP_CONFIG_SYSTEM_AD
};


/***************************************************************************************************
 *
 *  IMPORTANT
 *
 *   How to use the field 'alternative_coreId' from  LDAP_CONFIG
 *
 *   When 'alternative_coreId' is not empty in LDAP_CONFIG struct it means that queries by UID will use both fields
 *    'core_id' and 'alternative_coreId' in a OR statement like (|('core_id=value)(alternative_coreId=value))
 *
 *   If the result brings an 'alternative_coreId' value its value can be set in the 'core_id' results value,
 *    that means results will force values 'core_id' = 'alternative_coreId' depending
 *    on the following compiler directives:
 *       LDAP_UID_FORCE_FSL_COREID      - return it for all queries (matches by either 'core_id' and 'alternative_coreId')
 *       LDAP_UID_EQUAL_SEARCH_OR_LOGIN - only the match was made by 'alternative_coreId'
 *                                        other queries: name, email, etc will return 'core_id'
 *   If neither of directives above     - all queries return the real 'core_id' (WBI ID)
 *
 *************************************************************************************************************/


typedef struct
{
    int        system;                                       //FSL=direct=LDAP_CONFIG_SYSTEM_API, NXP=ldapsearch=LDAP_CONFIG_SYSTEM_LDAPSEARCH
    char       serviceDN       [MAX_LDAP_INFO_SIZE];         //FSL="cn=mso_service,ou=application users,ou=applications,ou=intranet,dc=motorola,dc=com"
    char       servicePassword [LDAP_RESULT_ATTRIBUTE_SIZE]; //FSL="My007Service"
    char       searchString    [MAX_LDAP_INFO_SIZE];         //FSL="ou=People,ou=Intranet,dc=Motorola,dc=com"
    char       host            [MAX_LDAP_INFO_SIZE];         //FSL="fsl-ids.freescale.net"
    char       emailSuffix     [LDAP_RESULT_ATTRIBUTE_SIZE];
    int        normal_port;
    int        ssl_port;       //FSL=636 //either normal_port or ssl_port must be not zero

    struct     LDAP_RESULT_STRING   //FSL="motlocationcode mail givenname sn uid"
    {                               //NXP="city mail fullname uid"
        char   firstName       [LDAP_RESULT_ATTRIBUTE_SIZE]; //FSL=givenname NXP=""
        char   lastName        [LDAP_RESULT_ATTRIBUTE_SIZE]; //FSL=sn    NXP=""
        char   fullName        [LDAP_RESULT_ATTRIBUTE_SIZE]; //FSL=""    NXP=fullname
        char   location        [LDAP_RESULT_ATTRIBUTE_SIZE]; //FSL=motlocationcode NXP=city
        char   mail            [LDAP_RESULT_ATTRIBUTE_SIZE]; //FSL=mail NXP=MAIL
        char   core_id         [LDAP_RESULT_ATTRIBUTE_SIZE]; //FSL=uid  NXP=uid
        char   core_id_alternative   [LDAP_RESULT_ATTRIBUTE_SIZE]; //search can be done for this field as OR between this and core_id
        char   extra_info_1  [LDAP_RESULT_ATTRIBUTE_SIZE];
        char   extra_info_2  [LDAP_RESULT_ATTRIBUTE_SIZE];
        char   extra_info_3  [LDAP_RESULT_ATTRIBUTE_SIZE];
        char   extra_info_4  [LDAP_RESULT_ATTRIBUTE_SIZE];
        char   extra_info_5  [LDAP_RESULT_ATTRIBUTE_SIZE];
        char   extra_info_6  [LDAP_RESULT_ATTRIBUTE_SIZE];
        char   extra_info_7  [LDAP_RESULT_ATTRIBUTE_SIZE];
        char   extra_info_8  [LDAP_RESULT_ATTRIBUTE_SIZE];
        char   extra_info_9  [LDAP_RESULT_ATTRIBUTE_SIZE];

    } result_attributes;

} LDAP_CONFIG;


/*!
 *  information got from one person
 */
typedef struct LDAP_USER_INFORMATION
{
    char coreId              [MAX_LDAP_INFO_SIZE];
    char name                [MAX_LDAP_INFO_SIZE];
    char corporate_email     [MAX_LDAP_INFO_SIZE];
    char friendly_email      [MAX_LDAP_INFO_SIZE];
    char site                [MAX_LDAP_SITE_SIZE];
    char alternative_coreId  [MAX_LDAP_SITE_SIZE];
    char extra_info_1        [LDAP_RESULT_ATTRIBUTE_SIZE];
    char extra_info_2        [LDAP_RESULT_ATTRIBUTE_SIZE];
    char extra_info_3        [LDAP_RESULT_ATTRIBUTE_SIZE];
    char extra_info_4        [LDAP_RESULT_ATTRIBUTE_SIZE];
    char extra_info_5        [LDAP_RESULT_ATTRIBUTE_SIZE];
    char extra_info_6        [LDAP_RESULT_ATTRIBUTE_SIZE];
    char extra_info_7        [LDAP_RESULT_ATTRIBUTE_SIZE];
    char extra_info_8        [LDAP_RESULT_ATTRIBUTE_SIZE];
    char extra_info_9        [LDAP_RESULT_ATTRIBUTE_SIZE];

}   LDAP_USER_INFORMATION;

/*!
 * This will be the common return
 *
 * The data MUST be released after using by calling ldapfsl_free_return_data()
 */
typedef struct LDAP_RETURN_DATA
{
    int   result;                  //<! always checked against  LDAP_SUCCESS
    int   entries;                 //<! number of info entries, used as info[0] .. info[entries-1]
    int   connected_code;          // connection and bind return codes, should be 0
    LDAP_USER_INFORMATION **info;
    char  errorMessage[MAX_LDAP_INFO_SIZE];

}   LDAP_RETURN_DATA;


/*!
 *     !!! only for USING_MOZILLA_LDAP
 *
 *     ldapfsl_setCaCertFileName() must be called before any operation, the full path of the cert7.db must be passed
 *
 */
void               ldapfsl_setCaCertFileName(const char *pathname);


/*!
 * \brief ldapfsl_setLdapConfig()   set the global configuration
 * \param config
 */
void  ldapfsl_setLdapConfig(LDAP_CONFIG *config);



/*!
 *   Every return from ldapfsl_authenticateCoreId(), ldapfsl_lookupByCoreId() and ldapfsl_lookupByName()
 *   must be released
 */
void               ldapfsl_free_return_data(LDAP_RETURN_DATA *data);



/*!  \enum Server_Config_ID  defines indexes from available LDAP servers configured
 *
 */
enum Server_Config_ID
{
     SERVER_NXP_Coreid_ID   = 0,
     SERVER_NXP_Wbiid_ID    = 1,
     SERVER_FSL_Intranet_ID = 2,
     SERVER_FSL_Extranet_ID = 3,
     SERVER_NXP_Exranet_ID  = 4,
};

/*! \fn ldapfsl_setServerID()  sets the current server for next searches and authentications
 *
 * If this function is never called the server defaults to SERVER_NXP_Coreid_ID
 */
void ldapfsl_setServerID(enum Server_Config_ID id);




LDAP_CONFIG * ldapfsl_getCurrentConfig();

#if REGRESSION_TEST_LDAP  // testing purposes
void set_regression_test_ldapsearch_file(const char *file);
void ldapfsl_setNXPcoreidLdapConfig();
void ldapfsl_setNXPwbiidLdapConfig();
void ldapfsl_setFSLintranetLdapConfig();
void ldapfsl_setFSLextranetLdapConfig();
LDAP_CONFIG * ldapfsl_getNXPcoreidLdapConfig();
LDAP_CONFIG * ldapfsl_getNXPwbiidLdapConfig();
LDAP_CONFIG * ldapfsl_getFSLextranetLdapConfig();
LDAP_CONFIG * ldapfsl_getFSLintranetLdapConfig();
#endif

//------------------------------------------------------------
/*!
 *  Authenticate the user with his password
 *
 *  Example:
 *  \code
 *
 *#if defined(USING_MOZILLA_LDAP)
 *        ldapfsl_setCaCertFileName("/path/cert7.db");
 *#endif
 *        LDAP_RETURN_DATA * ret = ldapfsl_authenticateCoreId("b35201","mypassword");
 *        if (ret->result == LDAP_SUCCESS) {
 *              printf("OK\n");
 *        }
 *        else {
 *             printf("Failed with code = %d %s\n", ret->result, ret->errorMessage);
 *        }
 *        ldapfsl_free_return_data(ret);
 *  \endcode
 */
LDAP_RETURN_DATA * ldapfsl_authenticateCoreId(const char *coreId, const char *passwd);


//-------------------------------------------------------------------
/*!
 *  Search a user by using by CoreID
 *
 *  Example:
 *  \code
 *#if defined(USING_MOZILLA_LDAP)
 *        ldapfsl_setCaCertFileName("/path/cert7.db");
 *#endif
 *        LDAP_RETURN_DATA * ret = ldapfsl_lookupByCoreId("ra5063");
 *        if (ret->result == LDAP_SUCCESS) {
 *          fprintf(stdout, "ldap name [0] = %s\n", ret->info[0]->name);
            fprintf(stdout, "ldap cmail[0] = %s\n", ret->info[0]->corporate_email);
            fprintf(stdout, "ldap fmail[0] = %s\n", ret->info[0]->friendly_email);
            fprintf(stdout, "ldap sitel[0] = %s\n", ret->info[0]->sitel);
 *        }
 *        else {
 *             printf("Failed with code = %d %s\n", ret->result, ret->errorMessage);
 *        }
 *        ldapfsl_free_return_data(ret);
 *  \endcode
 */
LDAP_RETURN_DATA * ldapfsl_lookupByCoreId(const char *coreId);



//-------------------------------------------------------------------
/*!
 *  Search users using a name
 *
 *  Example:
 *  \code
 *#if defined(USING_MOZILLA_LDAP)
 *        ldapfsl_setCaCertFileName("/path/cert7.db");
 *#endif
 *        LDAP_RETURN_DATA * ret = ldapfsl_lookupByName("Carlos");
 *        if (ret->result == LDAP_SUCCESS) {
 *          for(int counter=0; counter < ret->entries; counter++)
            {
 *             fprintf(stdout, "ldap name [%d] = %s\n", counter, ret->info[counter]->name);
               fprintf(stdout, "ldap cmail[%d] = %s\n", counter, ret->info[counter]->corporate_email);
               fprintf(stdout, "ldap fmail[%d] = %s\n", counter, ret->info[counter]->friendly_email);
               fprintf(stdout, "ldap fmail[%d] = %s\n", counter, ret->info[counter]->site);
            }
 *        }
 *        else {
 *             printf("Failed with code = %d %s\n", ret->result, ret->errorMessage);
 *        }
 *        ldapfsl_free_return_data(ret);
 *  \endcode
 */
LDAP_RETURN_DATA * ldapfsl_lookupByName(const char *name);


/*!  \fn ldapfsl_lookupByMail() searches by email
 */
LDAP_RETURN_DATA * ldapfsl_lookupByMail(const char *mail);


/*! \fn ldapfsl_lookupByFilter() the caller makes its own ldap filter to searchStri ng
 *
 */
LDAP_RETURN_DATA * ldapfsl_lookupByFilter(const char *filter);


/*! \fn ldapfsl_getManagerOf()  searches for the manager of  a user
 *
 * \param user_info is the result from a previuos search perhaps using \sa ldapfsl_lookupByCoreId()
 */
LDAP_RETURN_DATA * ldapfsl_getManagerOf(const LDAP_USER_INFORMATION * user_info);
LDAP_RETURN_DATA * ldapfsl_getManagerOfCoreId(const char *login_id);


/*!
      This function is used just for testing
 */
void printLdapHardResults(LDAP_RETURN_DATA * ret, FILE * output);

#ifdef __cplusplus
}
#endif

#endif // LDAPFSL_H


