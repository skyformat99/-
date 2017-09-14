/*
#include<string>
#ifndef myStrtok_H
#define myStrtok_H

char *strtok_r(char *str, const char *delim, char **saveptr);
#endif
*/
/*
* strtok_r.c:
* Implementation of strtok_r for systems which don't have it.
*
* This is taken from the GNU C library and is distributed under the terms of
* the LGPL. See copyright notice below.
*
*/

#ifdef HAVE_CONFIG_H      
#include "configuration.h"      
#endif /* HAVE_CONFIG_H */      

#ifdef HAVE_STRTOK_R      

static const char rcsid[] = "$Id: strtok_r.c,v 1.1 2001/04/24 14:25:34 chris Exp $";
#endif
#include <string.h>      

#undef strtok_r      

/* Parse S into tokens separated by characters in DELIM.
If S is NULL, the saved pointer in SAVE_PTR is used as
the next starting point.  For example:
char s[] = "-abc-=-def";
char *sp;
x = strtok_r(s, "-", &sp);      // x = "abc", sp = "=-def"
x = strtok_r(NULL, "-=", &sp);  // x = "def", sp = NULL
x = strtok_r(NULL, "=", &sp);   // x = NULL
// s = "abc/0-def/0"
*/

char *strtok_r(char *s, const char *delim, char **save_ptr) {
	char *token;

	if (s == NULL) s = *save_ptr;

	/* Scan leading delimiters.  */
	s += strspn(s, delim);   // 返回字符串中第一个不在指定字符串中出现的字符下标，去掉了以空字符开头的
							 // 字串的情况
	if (*s == '/0')
		return NULL;

	/* Find the end of the token.  */
	token = s;
	s = strpbrk(token, delim);  //   返回s1中第一个满足条件的字符的指针 
	if (s == NULL)
		/* This token finishes the string.  */
		*save_ptr = strchr(token, '/0');
	else {
		/* Terminate the token and make *SAVE_PTR point past it.  */
		*s = '/0';
		*save_ptr = s + 1;
	}

	return token;
}
