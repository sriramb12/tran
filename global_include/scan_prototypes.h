#ifndef scan_prototypes_h
#define scan_prototypes_h

/* Get an input line */
void line_scan(char *line);

/* Get an argument from the user */
/* This function allows the user to enter a carriage return */
/* and not specify any argument */
int arg_scan(char *line);

int string_scan(char *line);

#endif
