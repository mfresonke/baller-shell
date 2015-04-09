%{
#include <string.h>
#include "shell.tab.h"
extern YYSTYPE yylval;

char* str_filter( char *str );

%}

%option header-file="shell.lex.h"

WORD		[^ ^\n^\t]
FILE_CHARS	[\/]
%%
<<EOF>>								return END_OF_FILE;
&									return AND;
\|									return PIPE_NEXT;
\2\>\&1								return REDIR_STDERR_STDIN;
\2\>								return REDIR_STDERR_FILE;
\<									return REDIR_STDIN;
\>\>								return REDIR_STDOUT_APPEND;
\>									return REDIR_STDOUT_OVERWRITE;
alias								return BI_SETALIAS;
[ \t]+								/* Ignore Whitespace */
"\\\n"								/* Ignore escaped newline */
\n 									return NEW_LINE;
cd									return BI_CD;
bye									return BI_BYE;
printenv							return BI_PRINTENV;
setenv								return BI_SETENV;
unsetenv							return BI_UNSETENV;
\.\/({WORD})+						yylval.string=strdup(yytext); return PATH_REL;
\/({WORD})+							yylval.string=strdup(yytext); return PATH_ABS; 
({WORD})+							yylval.string=strdup(yytext); return WORD;
%%

