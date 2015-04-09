%{
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "commands.h"
#include "builtins.h"
#include "string_manipulation.h"

extern int yyparse();
extern int yylex();

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
	printf("%s | bs => ", get_cd() );
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

%token PIPE_NEXT NEW_LINE AND
%token BI_CD BI_BYE BI_PRINTENV BI_SETENV BI_UNSETENV BI_ALIAS
%token REDIR_STDIN REDIR_STDOUT_APPEND REDIR_STDOUT_OVERWRITE REDIR_STDERR_STDIN REDIR_STDERR_FILE 

%union
{
	int number;
	char *string;
}

%token <string> WORD 
%token <string> PATH_ABS 
%token <string> PATH_REL 

%type <string> argument

%%

input: /* empty */
	| builtin command_end
	| command arguments pipe redirections command_end
	;

/* Builtin Command Handling */

builtin:
	BI_CD cd_args
	| BI_BYE
	{
		exit(1);
	}
	| BI_PRINTENV
	{
		printenv();
	}
	| BI_SETENV
	{

	}
	| BI_UNSETENV
	{

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
	{
		add_arg( $2 );
	}
	;

argument:
	WORD
	| PATH_ABS 
	| PATH_REL
	;

pipe:
	| pipe PIPE_NEXT command arguments
	;

redirections:
	| redirection_input redirection_output_std redirection_output_err
	;

redirection_input:
	| REDIR_STDIN WORD
	{
		redirect_input_check( $2 );
	}
	| REDIR_STDIN PATH_ABS
	{
		redirect_input_check( $2 );
	}
	| REDIR_STDIN PATH_REL
	{
		redirect_input_check( $2 );
	}
	;

redirection_output_std:
	| REDIR_STDOUT_APPEND WORD
	{

	}
	| REDIR_STDOUT_OVERWRITE WORD
	{

	}
	;

redirection_output_err:
	| REDIR_STDERR_STDIN
	{

	}
	| REDIR_STDERR_FILE WORD
	{

	}
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
