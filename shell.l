%{
#include <string.h>
#include "shell.tab.h"
extern YYSTYPE yylval;

char* remove_quotes( char *str );

%}

%option header-file="shell.lex.h"

WORD			[^ ^\n^\t]
WORD_W_SPACES	[^\n^\t]
FILE_CHARS		[\/]
%%
<<EOF>>								return END_OF_FILE;
&									return AND;
\|									return PIPE_NEXT;
2\>[ ]+\&1							return REDIR_STDERR_STDIN;
2\>									return REDIR_STDERR_FILE;
\<									return REDIR_STDIN;
\>\>								return REDIR_STDOUT_APPEND;
\>									return REDIR_STDOUT_OVERWRITE;
alias								return BI_ALIAS;
unalias								return BI_ALIAS_UNSET;
\"(({WORD_W_SPACES})*(\*)+({WORD_W_SPACES}*))+\"	yylval.string=remove_quotes( yytext ); return WILDCARD;
\"(({WORD_W_SPACES})*(\?)+({WORD_W_SPACES}*))+\"	yylval.string=remove_quotes( yytext ); return WILDCARD;
\"(({WORD_W_SPACES})*(~)+({WORD_W_SPACES}*))+\"		yylval.string=remove_quotes( yytext ); return WILDCARD;
\"{WORD_W_SPACES}*\"				yylval.string=remove_quotes( yytext ); return WORD;
[ \t]+								/* Ignore Whitespace */
"\\\n"								/* Ignore escaped newline */
\n 									return NEW_LINE;
cd									return BI_CD;
bye									return BI_BYE;
printenv							return BI_ENV_PRINT;
setenv								return BI_ENV_SET;
unsetenv							return BI_ENV_UNSET;
(({WORD})*(\*)+({WORD}*))+			yylval.string=strdup( yytext ); return WILDCARD;
(({WORD})*(\?)+({WORD}*))+			yylval.string=strdup( yytext ); return WILDCARD;
(({WORD})*(~)+({WORD}*))+			yylval.string=strdup( yytext ); return WILDCARD;
\.\/({WORD})*						yylval.string=strdup( yytext ); return PATH_REL;
\/({WORD})*							yylval.string=strdup( yytext ); return PATH_ABS; 
({WORD})+							yylval.string=strdup( yytext ); return WORD;

%%

char* remove_quotes( char *input )
{
	char *output = input;
	++output;
	output[strlen(output) - 1] = '\0';
	return strdup( output );
}

