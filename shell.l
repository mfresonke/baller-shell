%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.tab.h"
#include "string_manipulation.h"
extern YYSTYPE yylval;

char* str_filter( char *str );

%}
WORD		[^ ^\n^\t]
FILE_CHARS	[\/]
%%
&									return AND;
\|									return PIPE_NEXT;
[ \t]+								/* Ignore Whitespace */
"\\\n"								/* Ignore escaped newline */
\n 									return NEW_LINE;
cd									return BI_CD;
bye									return BI_BYE;
printenv							return BI_PRINTENV;
\.\/({WORD})+						yylval.string=strdup(yytext); return PATH_REL;
\/({WORD})+							yylval.string=strdup(yytext); return PATH_ABS; 
\"({WORD}*[ \n]*"\\\n"*)*\"			yylval.string=str_filter(strdup(yytext));  return WORD;
({WORD})+							yylval.string=strdup(yytext); return WORD;
%%

