%{
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "commands.h"
#include "builtins.h"
#include "string_manipulation.h"

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
	printf("%s | bs > ", get_cd() );
}

int main()
{
	//setup operations

	//main loop
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

%token PIPE_NEXT NEW_LINE BI_CD AND BI_BYE

%union
{
	int number;
	char *string;
}

%token <string> WORD 
%token <string> PATH_ABS 
%token <string> PATH_REL 

%%

input: /* empty */
	| builtin command_end
	| command arguments pipe command_end
	;

/* Builtin Command Handling */

builtin:
	BI_CD cd_args
	| BI_BYE
	{
		exit(1);
	}
	;

cd_args:
	  PATH_ABS
	{
		cd( $1 );
	}
	| PATH_REL
	{
		cd( $1 );
	}
	| WORD
	{
		cd_word( $1 );
	}
	| //Blank
	{
		cd_home();
	}
	;

/* Generic Command Handling */

command:
	WORD 	//path command
	{
		new_command_path( $1 );
	}
	| PATH_ABS 
	{
		new_command_abs( $1 );
	}
	| PATH_REL
	{
		new_command_relative( $1 );
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
	| PATH_ABS 
	{
		add_arg( $1 );
	}
	| PATH_REL
	{
		add_arg( $1 );
	}
	;

pipe:
	//empty
	| pipe PIPE_NEXT command arguments
	;

command_end:
	NEW_LINE
	{
		return 1;
	}
	| AND NEW_LINE
	{
		//set the flag to run the command(s) in the background
		cmd_run_in_bkgrnd = true;
		return 1;
	}
	;
