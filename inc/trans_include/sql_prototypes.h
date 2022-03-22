#ifndef sql_prototypes_h
#define sql_prototypes_h

/* Configurable edc database name */
extern char edc_database[];
extern char dt_database[];

/* See if SQL error occurred */
int sql_err_check(char *query, MYSQL *sql, char *err_msg);

/* Map status from the database to defined values */
int sql_status_map(char *status);

/* Look up user's site in the subnet table */
/* This is used to tell what site user is coming from */
int site_subnet_lookup(char *site);

int sql_insert_data(char *query, char *err_msg);

MYSQL_RES * sql_result_get(char *query, int *errno);
/* Retrieve a column from an SQL table */
int sql_column_rtv(char *query, char *column);

/* Add entry to dt_transactions table */
void sql_dt_transaction_insert ( int rtn_status, char *rtn_msg);

/* Update timestamp in registry */
void sql_reg_timestamp_update(void);

#endif
