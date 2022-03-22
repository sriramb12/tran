#ifndef opc_transactions_h
#define opc_transactions_h

#include <sys/types.h>
/*
#ifndef LINUX
#include "opcDbClient.h"
#endif
*/
#include "opc_req_types.h"

/* The following functions are called by the RETsetup client */

/* Retrieve group of files from CVS */
/* List of files retrieved returned in files array */
/* Files is a dynamicly allocated array terminated with a NULL pointer */
/* This array must be freed by the software calling this function */
/* returns SUCCESS (0) if transaction successful */
/* returns negative failure code if transaction fails */
/* If transaction fails, reason for failure held in rtn.msg */
short opc_cvs_get(char *repository, char *module, char *tag);

/* Print out the current RETtrans version number */
char *opcversion_get(void);

/* Return all RETtrans version information */
char **opcversions_get(void);

/* Verifies user is registered to use RETtrans */
/* Returns 0 if user registered */
/* Returns RET_REG_ERR (-818) if user not registered. */
short opc_checkreg(void);

/* Performs registration for user */
/* Returns SUCCESS (0) if user registered */
/* Otherwise returns negative return code */
short opc_autoreg(char *user_id, char *password, char *dept_name, char *mbg);

/* Retrieve list of available work space */
short opc_workspace_rtv(char *workspace[]);

short opc_socket_init(RET_REQ_TYPE req_type, int job_id, char *dir,
 char *script,  long increment, int memory, int diskspace, long long sendsize);

/* Send request to RET server */
short opc_request_send(RET_REQ_TYPE req_type, int job_id, char *project, char *script, long increment);

/* Send MySQL query to RET server */
/* Returns SUCCESS (0) if output retrieved */
/* Returns negative number if no output retrieved */
/* If successful,  tbl_handle can be used to read records from the SQL table */
/* If not successful, tbl_handle points to NULL */
/* and rtn_msg_read can be used to retrieve a pointer to the error message */
/* The fourth argument, mysql_server, gives the name of the */
/* MySQL server to connect to */
/* To use the default server pass in NULL */
/*
#ifndef LINUX
short opc_mysql_query(int query_id, char *string, dbTbl_t **tbl_handle);
#endif
*/

/* Returns pointer to return message string */
/* A message is returned with all database transactions */
/* Storage for the message is handled by the database client */
/* Memory pointed to is global and does not need to be freed */
void opc_rtn(char *msg);

/* Creates a random keyword */
/* Key and index used to make the keyword random */
/* Keyword created returned in new_keyword */
void keyword_create(char *new_keyword, char *key, int index);
#endif /* define for opc_transactions_h */
