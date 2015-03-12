%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.tab.h"
extern YYSTYPE yylval;

char* str_filter( char *str );

%}
WORD	[0-9a-zA-Z\.\-,] 
FILE_CHARS	[\/]
%%
\"({WORD}*[ \n]*"\\\n"*)*\"				yylval.string=str_filter(strdup(yytext)); return WORD;
{WORD}+							yylval.string=strdup(yytext); return WORD;
\.({WORD}*{FILE_CHARS}*)+		yylval.string=strdup(yytext); return REL_PATH;
\/({WORD}*{FILE_CHARS}*)+		yylval.string=strdup(yytext); return ABS_PATH; 
\|								return PIPE_NEXT;
[ \t]+							/* Ignore Whitespace */
"\\\n"							/* Ignore escaped newline */
\n 								return NEW_LINE;
%%

/** Special Function for filtering strings. Either returns the same string, or a new one (where the old one has been deleted. */
char* str_filter( char *str )
{
	//remove quotes if they exist
	size_t length = strlen( str );

	//if the first and last characters are quotes....
	if( str[0] == '"' && str[length-1] == '"' )
	{
		//remove them! //remember that length - 2 + 1 (null char)
		char *new_str = calloc( (length-1), sizeof(char) );		
		int a;
		//copy the characters
		strncpy( new_str, &str[1], (length-2) );
		free( str );
		return new_str;
	}
	return str;
}
