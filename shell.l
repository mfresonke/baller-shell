%{
#include <stdio.h>
#include "shell.tab.h"
extern YYSTYPE yylval;
%}

%%
[0-9a-zA-Z]+		yylval.string=strdup(yytext); return WORD;
\.[0-9a-zA-Z\/\.]+		yylval.string=strdup(yytext); return REL_PATH;
\/[0-9a-zA-Z\/\.]+		yylval.string=strdup(yytext); return ABS_PATH; 
\|					return PIPE_NEXT;
[ \t]+				/* Ignore Whitespace */
\n
%%