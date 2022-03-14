#ifndef sql_lib_h
#define sql_lib_h

#include <mysql.h>
#include "tcq.h"

/* Get result from an SQL query */
MYSQL_RES * result_get(char *db, char *query1, char *query2);

int insert_data(char *db, char *query);

/* Initialize a connection in preparation for multiple queries */
int sql_connect(MYSQL *sql, char *db);

/* Perform a query to an open connection */
int sql_query(MYSQL *sql, char *query);

/* Log the servertype to the database and return the next job ID */
int job_id_rtv(void);

/* Retrieve the mask, design and technology for a specified UID */
int sql_dessync_info_rtv(TCQ *q);

/* Get the version and directory for a GDB given the UID */
int sql_version_get(int uid, char *directory, char *version);

int sql_tool_rtv(int server_type, char *tool, char *short_name);

/* Scan results from the result file */
int yp_output_scan(char *file_name, int job_id);

/* Add entry to job_q_table */
void sql_q_insert (TCQ *q);

/* Modify deck_version in yp_tech_current_versions */
void sql_deck_version_mod(float deck_version, char *deck_script);

/* Modify extraction_version in yp_tech_current_versions */
void sql_extraction_version_mod(float extraction_version);

/* Modify technology_version in yp_tech_current_versions */
void sql_yp_tech_version_mod(char *technology, float technology_version);

void sql_dfm_tech_version_mod(char *technology, float technology_version);

/* Modify entry in job_q_table */
void sql_q_modify(TCQ *q);

/* Add entry in dfm_jobs table */
void sql_job_report(TCQ *q);

/* Modify entry in dfm_jobs table */
void sql_job_modify(TCQ *q);

/* Modify entry in repository table */
void sql_repository_modify(TCQ *q);

/* Insert entry into repository table */
void sql_repository_insert(TCQ *q);


/* Returns rows with latest values in yp_tech_current_versions */
MYSQL_RES *sql_last_rows_rtv(void);

/* Get the highest version_id from the yp_tech_current_version table */
void yp_tech_current_version_get(TCQ *q);

/* Get the highest version_id from the dfm_tech_current_version table */
void dfm_tech_current_version_get(TCQ *q);

/* Update the chip_area and memory_area fields in the dfm_jobs table */
void sql_chip_area_update(int job_num, char *chip_area, char *memory_area);

/* Update the path status for a Pathcheck job */
void sql_path_status_update(int job_id, char *path_status);

#endif
