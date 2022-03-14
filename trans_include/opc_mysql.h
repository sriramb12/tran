#ifndef opc_mysql_h
#define opc_mysql_h

typedef struct
{
    char host[MACHINE_LEN];
    char name[32];
    char user[32];
     char passwd[32];
} MYSQL_T;

/* Process an SQL request */
void opc_mysql(int query_id, char *value_string, char *user_id);

/* Performs the actual query */
int opc_mysql_query(int substitute, int job_id, int query_id, char *value_string, char *user_id, char *results_file, MYSQL_T *db);

/* Add user to MySQL database during user registration */
void opc_mysql_user_add(char *user_id, OPC_USER *user, 
char *dept_name, char *mbg);

/* Log results of completed job to MySQL database */
void opc_mysql_job_log(int job_id);

/* Check for errors with insert/delete queries */
void opc_mysql_check_err(int job_id, int query_id, int ret, char *results_file, char *value_string, MYSQL_T *sql_db);
#endif
