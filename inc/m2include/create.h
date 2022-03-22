#ifndef create_h
#define create_h


/* Read data from specified create file */
void create_read(char *create_file);

void mask_create_write(char *create_file);

void template_create_write(char *create_file);

void fab_create_write(char *create_file, char *fab, char *tech);

void create_info_read(void);

#endif
