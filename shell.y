%{
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "commands.h"
#include "builtins.h"

extern int yyparse();

void yyerror( const char *str )
{
	fprintf( stderr, "error: %s\n",str );
}

int yywrap()
{
	return 1;
}

void print_prompt()
{
	printf("bs > ");
}

int main()
{
	for(;;) 
	{
		print_prompt();
		switch( yyparse() )
		{
			case 1:
				run_commands();
				clear_commands();
				break;
			default:;
				//check for errors here
		}
	}
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
	| command arguments pipe command_end
	;

command:
	WORD	//path command
	{
		new_command( PATH, $1 );
	}
	| ABS_PATH 
	{
		new_command( ABSOLUTE, $1 );
	}
	| REL_PATH
	{
		new_command( RELATIVE, $1 );
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

pipe:
	| PIPE_NEXT command arguments pipe
	;

command_end:
	NEW_LINE
	{
		return 1;
	}
	;
