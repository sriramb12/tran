#ifndef sql_lib_h
#define sql_lib_h

#include <mysql.h>


/* Get result from an SQL query */
MYSQL_RES * result_get(char *site, char *db, char *query1, char *query2);

int insert_data(char *site, char *db, char *query, int report_err_flag);

/* Initialize a connection in preparation for multiple queries */
int sql_connect(MYSQL *sql, char *host, char *db_name);

/* Perform a query to an open connection */
int sql_query(MYSQL *sql, char *query);

/* Retrieve name of projects associated with a mask set */
int sql_mask_projects_rtv(char *mask, char *projects[]);

/* Get DTMS status for a project */
int sql_dtms_status_rtv(char *project, char *mwr_num, char *project_date);

#endif
