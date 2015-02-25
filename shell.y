%{
#include <stdio.h>
#include <string.h>

void yyerror( const char *str )
{
	fprintf( stderr, "error: %s\n",str );
}

int yywrap()
{
	return 1;
}

int main()
{
	yyparse();
}

%}

%token PIPE_NEXT

%union
{
	int number;
	char *string;
}

%token <string> WORD

%%

input: /* empty */
	| command arguments meta
	;

command:
	WORD
	{
		printf( "Command is: %s\n", $1 );
	}
	;

arguments:
	| arguments argument
	;

argument:
	WORD
	{
		printf( "Argument is: %s\n", $1 );
	}
	;

meta:
	| PIPE_NEXT input
	;
