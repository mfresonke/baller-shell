%{
#define SHELL_EXIT -5
#define MAX_LINE_SIZE 1000

#include "shell.lex.h"
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
	fflush(0);
}

int main()
{
	//setup operations

	//main loop
	for(;;) 
	{
		print_prompt();

		//take in input to string
		char input[MAX_LINE_SIZE];
		if( !fgets( input, MAX_LINE_SIZE, stdin ) )
		{
			//error goes here
		}

		//search for aliases!

		//pass string to lex
		YY_BUFFER_STATE buffer = yy_scan_string ( input );

		switch( yyparse() )
		{
			case 1:
				run_commands();
				clear_commands();
				break;
			case SHELL_EXIT:
				run_commands();
				clear_commands();
				exit(1);
				break;
			default:;
				//check for errors here
		}

		//delete buffer
		yy_delete_buffer( buffer );
	}
}


%}

%token PIPE_NEXT NEW_LINE AND END_OF_FILE
%token BI_CD BI_BYE BI_PRINTENV BI_SETENV BI_UNSETENV BI_SETALIAS
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


/* Common to All Types */

input:
	| builtin command_end
	| command arguments pipe redirections command_end
	| END_OF_FILE
	{
		exit(1);
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
	| END_OF_FILE
	{
		return SHELL_EXIT;
	}
	;


argument:
	WORD
	| PATH_ABS 
	| PATH_REL
	;


/* Builtin Command Handling */

builtin:
	cd
	| bye
	| environment
	| alias
	;

cd:
	BI_CD PATH_ABS
	{
		cd( $2 );
	}
	| BI_CD PATH_REL
	{
		cd( $2 );
	}
	| BI_CD WORD
	{
		cd_word( $2 );
	}
	| BI_CD //no arg
	{
		cd_home();
	}
	;

bye:
	BI_BYE
	{
		exit(1);
	}
	;

environment:
	BI_PRINTENV
	{
		env_print();
	}
	| BI_SETENV WORD argument
	{
		env_set( $2, $3 );
	}
	| BI_UNSETENV WORD
	{
		env_unset( $2 );
	}
	;

alias:
	BI_SETALIAS WORD argument
	{

	}




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

pipe:
	| pipe PIPE_NEXT command arguments
	;

/* Redirection Command Handling */

redirections:
	| redirection_input redirection_output_std redirection_output_err
	;

redirection_input:
	REDIR_STDIN argument
	{
		redirect_input_setup( $2 );
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
