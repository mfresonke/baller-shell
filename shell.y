%{
#include <stdio.h>
#include <string.h>
#include "main.h"

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
	//Set Up Paths!
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
%token <string> ABS_PATH 
%token <string> REL_PATH 

%%

input: /* empty */
	| command arguments meta
	;

command:
	WORD
	{
		printf( "Command is in path.");
	}
	| ABS_PATH 
	{

	}
	| REL_PATH
	{
		printf( "\"%s\" is a relative path.", $1 );
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
