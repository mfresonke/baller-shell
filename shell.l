%{
#include <stdio.h>
#include "shell.tab.h"
extern YYSTYPE yylval;
%}

%%
[0-9a-zA-Z]+		yylval.string=strdup(yytext); return WORD;
\|					return PIPE_NEXT;
[\n \t]				/* Ignore Whitespace */
%%