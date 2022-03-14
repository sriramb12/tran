
/* Verify user authorized to build/approve templates */
int entry_find(char *file_name, char *entry);

/* Adds new entry to specified file */
/* If entry already present, replace old entry with new entry */
int entry_add(char *file1, char *entry, char *key);

/* Delete entry from specified file */
int entry_delete(char *file1, char *entry);

/* Returns file descriptor if find is successful */
int entry_find_write(char *file1, char *file2, char *entry, FILE **fi, FILE **fo, char *line);

/* Write out the rest of the file */
void entry_write(int index, char *file1, char *file2, FILE *fi, FILE *fo, char fnd);

void entry_close(int index, char *file1, char *file2, FILE *fi, FILE *fo);
