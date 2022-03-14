#ifndef strcpy_h
#define strcpy_h

/* Copies source string into destination string returning the number */
/* of characters returned. */
int strcpy1(char *result, char *string);

/* Copy contents of two strings into destination string */
int strcpy2(char *result, char *str1, char *str2);

/* Copy contents of three strings into destination string */
int strcpy3(char *result, char *str1, char *str2, char *str3);

/* Copy contents of four strings into destination string */
int strcpy4(char *result, char *str1, char *str2, char *str3, char *str4);

/* Copy contents of 5 strings into destination string */
int strcpy5(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5);

/* Copy contents of six strings into destination string */
int strcpy6(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6);

/* Copy contents of seven strings into destination string */
int strcpy7(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7);

/* Copy contents of eight strings into destination string */
int strcpy8(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7,
    char *str8);

int strcpy9(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7,
    char *str8,
    char *str9);

int strcpy10(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7,
    char *str8,
    char *str9,
    char *str10);

int strcpy12(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6,
    char *str7,
    char *str8,
    char *str9,
    char *str10,
    char *str11,
    char *str12);

/* Copy string exchanging old character for new character */
int strcpy_ex(
    char *new_string,
    char *old_string,
    char new_char,
    char old_char);

/* Looks for pattern at end of string */
/* If the pattern is found, it is stripped off */
void str_strip(char *string, char *pattern);

/* Strip off leading spaces */
void str_strip_leading_spaces(char *string);

/* Strip leading path off of file name */
/* It strips off paths with / or \ in it */
void str_path_strip(char *file_name);

/* Convert the specified string to lower case */
void str_lower(char *string);

/* Converts the specified string to upper case */
void str_upper(char *string);

/* Copies subset of source string to destination string */
/* Stops when specified character is found or buffer size for */
/* destination string has been reached */
/* Returns the length of the string if copy is successful */
int strcpy_substr(char *dest_string, char *src_string, char c, int len);

/* Finds position of specified character in string */
/* Returns -1 if character not found */
int strfind_char(char *string, char c, int upper_bound);

/* Finds specified substring in main string */
/* Returns a pointer to the substring if it is found */
/* Returns NULL if substring is not found in main string */
char *
strfind_substring(
    char *string,
    char *substring);

/* Make passed in string specified length */
/* If string not long enough, pad with spaces */
/* If string too long, truncate it */
void str_pad(char *string, int len);

/* Pad string with specified number of spaces to the left */
void strlpad(char *string, int len);

/* Exchanges all occurrences of old character to new character in string */
/* Returns length of string parsed */
int strexc(char *string, char old_char, char new_char);

/* Copy string changing occurrences of one character to two characters */
/* Returns length of new string */
int strexc2(char *new_string, char *old_string, char old_char, char *new_char);

/* Copies string1 into string2 */
/* Does not check for the null string */
/* Emulates actions of bcopy */
void str_bcopy(char *string1, char *string2, int len);

/* Converts a string to a long long */
long long str2ll(char *string);

/* Reads in line from buffer and returns number of characters found */
int str_line_get(char *buffer, char *line);

/* Version of strncpy which puts NULL at the end of the string */
void strn_cpy(char *result, char *string, int num);

/* Convert long long size to size string */
void str2size(char *size_string, long long size);

/* Verifies string contains onlyt numeric characters */
int str_numeric_only(char *string);

int str_is_alpha(char *string);

/* Strip off quotes, and brackets from beginning and end */
int str_quotes_strip(char *string);

/* Determines if string only contains digits */
/* Returns SUCCESS if all numeric characters */
/* Otherwise returns -1 */

int str_digits(char *string);
void str_move(char *string1, char *string2, char *string3);

/* See if file is a zipped file */
int is_zipped_file(char *file_name);

/* Reverse the characters in a string, must be NULL terminated */
int str_reverse(char *result, char *string);

/* count number of occurrences of character in string */
int str_numchar(char *string, char c);

#endif /* Define for strcpy_h */
