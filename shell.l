%{
#include <stdio.h>
#include "shell.tab.h"
extern YYSTYPE yylval;
%}

%%
[0-9a-zA-Z]+		yylval=atoi(yytext); return NUMBER;
heat		return TOK_HEAT;
on|off		yylval=strcmp( "off", yytext ); return STATE;
target		return TOK_TARGET;
temperature	return TOK_TEMPERATURE;
[\n \t]+	/* Ignore */
%%