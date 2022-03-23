/****************************************************************************
 *
 * Function:	strcpy
 *
 * Description:	Collection of functions used to copy one or more source
 *		strings into a destination string.
 *
 * Author:	Renee Carter
 *
 ***************************************************************************/

#include <alloca.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "global_debug.h" // Mazieri's debug facilities

/*
#include "strcpy.h"
*/
#define DBNO 2
#define DBYES 1

/* Copies source string into destination string returning the number */
/* of characters returned. */
int strcpy1(char *result, char *string)
{
    DBG();
    int length;

    for (length = 0; string[length] != 0; ++length)
    {
        result[length] = string[length];
    }

    result[length] = 0;
    return(length);
} /* End strcpy1 */

/* Copy contents of two strings into destination string */
int strcpy2(char *result, char *str1, char *str2)
{
    DBG();
    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    result[length] = 0;
    return(length);
} /* End strcpy2 */

/* Copy contents of three strings into destination string */
int strcpy3(char *result, char *str1, char *str2, char *str3)
{
    DBG();
    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }
    result[length] = 0;
    return(length);
} /* End strcpy3 */

/* Copy contents of four strings into destination string */
int strcpy4(char *result, char *str1, char *str2, char *str3, char *str4)
{
    DBG();
    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    result[length] = 0;
    return(length);
} /* End strcpy4 */

/* Copy contents of 5 strings into destination string */
int strcpy5(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5)
{
    DBG();
    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    result[length] = 0;
    return(length);
} /* End strcpy5 */

/* Copy contents of six strings into destination string */
int strcpy6(
    char *result,
    char *str1,
    char *str2,
    char *str3,
    char *str4,
    char *str5,
    char *str6)
{
    DBG();
    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    for (length2 = 0; str6[length2] != 0; ++length, ++length2)
    {
        result[length] = str6[length2];
    }
    result[length] = 0;
    return(length);
} /* End strcpy6 */

/* Copy contents of seven strings into destination string */
int strcpy7(char *result, char *str1, char *str2, char *str3, char *str4, char *str5, char *str6, char *str7)
{
    DBG();
    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    for (length2 = 0; str6[length2] != 0; ++length, ++length2)
    {
        result[length] = str6[length2];
    }
    for (length2 = 0; str7[length2] != 0; ++length, ++length2)
    {
        result[length] = str7[length2];
    }

    result[length] = 0;

    return(length);
} /* End strcpy7 */

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
    char *str8)
{
    DBG();
    int length;
    int length2;

    for (length = 0; str1[length] != 0; ++length)
    {
        result[length] = str1[length];
    }

    for (length2 = 0; str2[length2] != 0; ++length, ++length2)
    {
        result[length] = str2[length2];
    }

    for (length2 = 0; str3[length2] != 0; ++length, ++length2)
    {
        result[length] = str3[length2];
    }

    for (length2 = 0; str4[length2] != 0; ++length, ++length2)
    {
        result[length] = str4[length2];
    }
    for (length2 = 0; str5[length2] != 0; ++length, ++length2)
    {
        result[length] = str5[length2];
    }
    for (length2 = 0; str6[length2] != 0; ++length, ++length2)
    {
        result[length] = str6[length2];
    }
    for (length2 = 0; str7[length2] != 0; ++length, ++length2)
    {
        result[length] = str7[length2];
    }
    for (length2 = 0; str8[length2] != 0; ++length, ++length2)
    {
        result[length] = str8[length2];
    }
    result[length] = 0;
    return(length);
} /* End strcpy8 */

/* Copy string exchanging old character for new character */
int strcpy_ex(
    char *new_string,
    char *old_string,
    char new_char,
    char old_char)
{
    DBG();
    int cnt = 0;

    if (old_string != 0 && new_string != 0)
    {
        for (cnt = 0; old_string[cnt] != '\0'; ++cnt)
        {
            if (old_string[cnt] == old_char)
                new_string[cnt] = new_char;
            else new_string[cnt] = old_string[cnt];
        } /* End for */
        /* Put null at end of string */
        new_string[cnt] = '\0';
    }
    return(cnt);
} /* End strcpy_ex */

/* Looks for pattern at end of string */
/* If the pattern is found, it is stripped off */
void str_strip(char *string, char *pattern)
{
    DBG();
    register int len;

    len = strlen(string) -strlen(pattern);
    if (len > 0)
    {
	if (strcmp(pattern, string+len) == 0)
	string[len] = '\0';
    } /* End for */
} /* End str_strip */

/* Convert the specified string to lower case */
void str_lower(char *string)
{
    DBG("string='%s'", string);
    register int cnt;

    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	    if ((string[cnt] >= 65) && (string[cnt] <= 90))
	        string[cnt] += 32;
    } /* end for */
} /* End str_lower */

void str_upper(char *string)
{
    DBG();
    register int cnt;

    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	if ((string[cnt] >= 97) && (string[cnt] <= 122))
	string[cnt] -= 32;
    } /* end for */
} /* End str_upper */

/* Copies subset of source string to destination string */
/* Stops when specified character is found or buffer size for */
/* destination string has been reached */
/* Returns the length of the string if copy is successful */
int strcpy_substr(char *dest_string, char *src_string, char c, int len)
{
    DBG();
    register short cnt = 0;

    while((src_string[cnt] != c) && (src_string[cnt] != '\0') && (cnt < len))
    {
	dest_string[cnt] = src_string[cnt];
	++cnt;
    } /* End while */
    /* Put null at the end of the string */
    dest_string[cnt] = '\0';
    if (src_string[cnt] == c) return(cnt);
    else return(-1);
} /* End strcpy_substr */

/* Finds position of specified character in string */
/* Returns -1 if character not found */
int strfind_char(char *string, char c, int upper_bound)
{
    DBG();
    int cnt = 0;

while ((string[cnt] != '\0') && (cnt < upper_bound))
    {
	if (string[cnt] == c) return(cnt);
	++cnt;
    }
    /* If reached this point, did not find character */
    return(-1);
} /* End strfind_char */

/* Finds specified substring in main string */
/* Returns a pointer to the substring if it is found */
/* Returns NULL if substring is not found in main string */
char *
strfind_substring(
    char *string,
    char *substring)
{
    DBG();
    int cnt;
    int num;
    int sublen;
    char *ptr;

    /* Calculate number of searches to be done */
    /* It it the length of the main string - length of substring + 1 */
    sublen = strlen(substring);
    num = strlen(string) - sublen+1;
    /* Do searches in a for loop */
    for (cnt = 0; cnt <num; ++cnt)
    {
	if (strncmp(string+cnt, substring, sublen) == 0)
	{
	ptr = string+cnt;
	    return(ptr);
	} /* End if */
	} /* End for */
    /* If reached this point, substring was not found */
    return(NULL);
} /* End strfind_substring */

/* Make passed in string specified length */
/* If string not long enough, pad with spaces */
/* If string too long, truncate it */
void str_pad(char *string, int len)
{
    DBG();
    int cnt;

    for (cnt = 0; cnt <len; ++cnt)
    {
	if (string[cnt] == 0) 
	{
	    while (cnt < len) 
	    {
		string[cnt] = ' ';
		++cnt;
	    } /* End while */
	    break;
	} /* End if */
    } /* End for */
    string[len] = 0;
} /* end str_pad */

/* Pad string with specified number of spaces to the left */
void strlpad(char *string, int len)
{
    DBG();
    int pad;
    int old_len;

    /* Get length of string */
    old_len = strlen(string);
    if (old_len >= len) return;
    /* Put null at end of new string */
    string[len] = '\0';
    pad = len-old_len;
    for (len = len-1, old_len = old_len-1; len >= pad; --len, --old_len)
    string[len] = string[old_len];
    /* Now put in the white space */
    for (pad = pad-1; pad >= 0; --pad)
    string[pad] = ' ';
} /* End strlpad */

/* Exchanges all occurrences of old character to new character in string */
int strexc(char *string, char old_char, char new_char)
{
    DBG();
    int cnt;

    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	    if (string[cnt] == old_char)
	    string[cnt] = new_char;
    } /* End for */

    DBG("will return cnt='%d'", cnt);
    return(cnt);
} /* End strexc */

#ifdef SUNOS
char *strerror(int);
char *strerror(
  int errnum
)
{
    DBG();
  extern int sys_nerr;
  extern char *sys_errlist[];
  extern int errno;
 
  if ( errnum >= 0 && errnum < sys_nerr ) {
    return sys_errlist[errnum];
  }
  return "Unknown error code";
}
#endif

/* Copies string1 into string2 */
/* Does not check for the null string */
/* Emulates actions of bcopy */
void str_bcopy(char *string1, char *string2, int len)
{
    DBG();
    int cnt;

    for (cnt =0; cnt <len; ++cnt)
    string2[cnt] = string1[cnt];
}

/* converts string to a long long */
long long str2ll(char *string)
{
    DBG();
    long long total = 0;
    int cnt;
    long long multiple = 1;

    for (cnt = strlen(string)-1;  cnt >= 0; --cnt)
    {
	total += (string[cnt]-48)*multiple;
	multiple *= 10;
    }
    return(total);
} /* end str2ll */

/* Reads in line from buffer and returns number of characters found */
int str_line_get(char *buffer, char *line)
{
    DBG("line='%s'", line);
    int cnt;

    cnt = 0;
    while ((buffer[cnt] != '\n') && (buffer[cnt] != '\0') && (cnt < 260))
    {
	    line[cnt] = buffer[cnt];
	    ++cnt;
    } /* end while */

    /* Put null at end of line */
    line[cnt] = '\0';
    DBG("line='%s'", line);

    /* Return the number of characters found */
    /* Return EOF if end of file reached and no characters found */
    if ((buffer[cnt] == '\0') && (cnt == 0)) return(EOF);
    else return(cnt);
} /* End str_line_get */

/* Verifies string contains only numeric characters including a '.' */
/* Returns DBYES if correct format */
int str_numeric_only(char *string)
{
    DBG();
    int cnt;

    /* Go through string looking for non numeric characters */
    for (cnt = 0; string[cnt] != '\0'; ++cnt)
    {
	/* If non numeric return */
	if (string[cnt] == '.') continue;
	if ((string[cnt] < '0') 
	|| (string[cnt] > '9'))return(DBNO);
    }
    /* If reached this point string is valid */
    return(DBYES);
}

int str_printable_only(char *string1)
{
    DBG();
    int fnd = DBNO;
    char *string2;
    int cnt1;
    int cnt2;

    /* Allocate string2 */
    string2 = malloc(strlen(string1) + 1);
    /* Go through string looking for non numeric characters */
    for (cnt1 = 0, cnt2 = 0; string1[cnt1] != '\0'; ++cnt1)
    {
	/* If nonprintable, skip it */
	if ((string1[cnt1] < 32) 
	|| (string1[cnt1] > 122) )
	{
	    fnd = DBYES;
	    continue;
	}
	else string2[cnt2] = string1[cnt1];
	++cnt2;
    }
    string2[cnt2] = '\0';
    if (fnd == DBYES)
    strcpy(string1, string2);
    free(string2);
    /* Return length of the new string */
    return(cnt2);
} /* end str_printable_only */

/* Strips leading and trailing quotes on string */
void str_strip_quotes(char *new_string, char *old_string)
{
    DBG();
    int len;

    if ((old_string[0] == '"')
  || (old_string[0] == '\''))
    len = strcpy1(new_string, old_string+1);
    else len = strcpy1(new_string,old_string);
    -- len;
    if ((new_string[len] == '"')
    || (new_string[len] == '\''))
    new_string[len] = '\0';
} /* End str_strip_quotes */

/* Strip off all quotes and other characters */
int str_strip_all_quotes(char *string)
{
    DBG();
    char *tmp_string = malloc(strlen(string) + 1);
    int fnd = DBNO;
    int cnt;
    int cnt1;

    for (cnt = 0, cnt1 = 0; string[cnt] != '\0'; ++cnt)
    {
	if ((string[cnt] != '\'')
	&& (string[cnt] != '\\')
	&& (string[cnt] != '"'))
	    tmp_string[cnt1++] = string[cnt];
	else fnd = DBYES;
    } /* end for */
    tmp_string[cnt1] = '\0';
    if (fnd == DBYES) strcpy(string, tmp_string);
    free(tmp_string);
    return(cnt1);
} /* end str_strip_all_quotes */

/* Returns a pointer to right most match for character in a string */
/* The string must be NULL terminated */
char *strchrr(char *string, char c)
{
    DBG();
    int cnt;
    int len = strlen(string);

    for (cnt = len; cnt >= 0; --cnt)
    {
	if (string[cnt] == c)
	{
	    char *ptr = string+cnt;
	    return(ptr);
	} /* End if */
    } /* end for */
    /* If reach this point, character not found */
    return(NULL);
} /* end strchrr */

/* See if file is a zipped file */
int is_zipped_file(char *file_name)
{
    DBG();
    int len = strlen(file_name);
    if (len <3) return(DBNO);
    if ((strcasecmp(file_name+len-3, ".gz") == 0)
    || (strcasecmp(file_name+len-2, ".Z") == 0)
    || (strcasecmp(file_name+len-3, ".rz") == 0)
    || (strcasecmp(file_name+len-3, ".lz") == 0))
    return(DBYES);
    /* Make sure length not less than 4 for next checks */
    else if (len < 4) return(DBNO);
    if ((strcasecmp(file_name+len-4, ".zip") == 0)
    || (strcasecmp(file_name+len-4, ".taz") == 0)
    || (strcasecmp(file_name+len-4, ".tgz") == 0)
    || (strcasecmp(file_name+len-4, ".bz2") == 0)
    || (strcasecmp(file_name+len-4, ".dmp") == 0))
    return(DBYES);
    else return(DBNO);
} /* End is_zipped_file */

int str_whitespace(char *string)
{
    DBG();
    if ((strchr(string, ' ') )
    || (strchr(string, '\n') )
    || (strchr(string, '\t') ))
    return(DBYES);
    else return(DBNO);
} /* end str_whitespace */

/* Move strings */
void str_move(char *string1, char *string2, char *string3)
{
    DBG();
    strcpy(string3, string2);
    strcpy(string2, string1);
}

/* Count number of occurrences of character in string */
int str_numchar(char *string, char c)
{
    DBG();
    int num = 0;
    int cnt;
    for (cnt = 0; string[cnt] != '\0'; ++cnt)
	if (string[cnt] == c) ++num;
    return(num);
} /* end str_numchar */
/*ec.files.fileuse*/
/*ic.files.fileuse*/
/*tc.files.fileuse*/
