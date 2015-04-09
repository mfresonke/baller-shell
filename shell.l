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
\2\>\&1								return REDIR_STDERR_STDIN;
\2\>								return REDIR_STDERR_FILE;
\<									return REDIR_STDIN;
\>\>								return REDIR_STDOUT_APPEND;
\>									return REDIR_STDOUT_OVERWRITE;
alias								return BI_ALIAS;
[ \t]+								/* Ignore Whitespace */
"\\\n"								/* Ignore escaped newline */
\n 									return NEW_LINE;
cd									return BI_CD;
bye									return BI_BYE;
\.\/({WORD})+						yylval.string=strdup(yytext); return PATH_REL;
\/({WORD})+							yylval.string=strdup(yytext); return PATH_ABS; 
\"({WORD}*[ \n]*"\\\n"*)*\"			yylval.string=str_filter(strdup(yytext));  return WORD;
({WORD})+							yylval.string=strdup(yytext); return WORD;
%%

