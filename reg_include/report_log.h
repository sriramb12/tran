#ifndef report_log_h
#define report_log_h


void report_log(char *keyword, char *file_name, char *mask);

int maskshop_input_scan(char *line, char rep[17][256]);

void report_fields_pad(char rep[17][256]);

void report_header_build(char *header, int fields[]);


void report_fields_scan(char *scan_fields, int fields[]);

/* Design for report_join */
void report_join(char *field1, char *field2); /* Hold value for new fields to join */

void field_convert(char *field, int cnt);

void elapsed_time_convert(char *string);
void size_convert(char *size_string);
void num_gmb_calc(char *size_string, unsigned long long size);

void num_bytes_calc(char *byte_string, long long bytes);

void time_convert(char *time_string);
void mask_format(char *mask);

/* Determine if first time mask set sent to the mask shop */
void mask_status_find(char *mask, char *status);

void report_put_list(char *date1, char *date2);

/* General function for building report lists */
void report_list(
    char *date1, 
    char *date2, 
    char *keep_filter1, 	/* Holds keep_filter index and keep_filter value */
    char *keep_filter2,	/* Holds keep_filter index and keep_filter value */
    char *discard_filter1, 	/* Holds discard_filter index and discard_filter value */
    char *discard_filter2,	/* Holds discard_filter index and discard_filter value */
 char *scan_fields);

void report_header_print(FILE *fo, char *report_title, char *date1, char *date2);

/* Report specified fields */
int report_fields_print(char *buffer, char rep[17][256], int fields[]);

#endif
