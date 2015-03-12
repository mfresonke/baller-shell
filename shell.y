%{
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "helpers.h"
#include "builtins.h"

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
	print_prompt();
	yyparse();
}


%}

%token PIPE_NEXT NEW_LINE

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
	| command arguments meta command_end input
	;

command:
	WORD	//path command
	{
		set_command( PATH, $1 );
	}
	| ABS_PATH 
	{
		set_command( ABSOLUTE, $1 );
	}
	| REL_PATH
	{
		set_command( RELATIVE, $1 );
	}
	;

arguments:
	| arguments argument
	;

argument:
	WORD
	{
		add_arg( $1 );
	}
	;

command_end:
	NEW_LINE
	{
		run_command();
		print_prompt();
	}
	;

meta:
	| PIPE_NEXT input
	;
