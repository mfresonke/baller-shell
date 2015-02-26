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
		printf( "Command stands on its own.");
	}
	| REL_PATH
	{
		printf( "Command is relative" );
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
