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
		char *input = calloc( sizeof(char), MAX_LINE_SIZE );
		if( !fgets( input, MAX_LINE_SIZE, stdin ) )
		{
			error_retrieving_line_of_input_failed();
			//skip to the next loop iteration
			continue;
		}

		input = run_preparser( input );

		if( !input )
			continue;

		//pass string to lex
		YY_BUFFER_STATE buffer = yy_scan_string ( input );

		switch( yyparse() )
		{
			case 1:
				run_commands();
				clear_commands();
				redirect_clear();
				break;
			case SHELL_EXIT:
				run_commands();
				clear_commands();
				redirect_clear();
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
%token BI_CD BI_BYE BI_ENV_PRINT BI_ENV_SET BI_ENV_UNSET BI_ALIAS BI_ALIAS_UNSET
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
	| command command_arguments pipe redirections command_end
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
	BI_ENV_PRINT
	{
		env_print();
	}
	| BI_ENV_SET WORD argument
	{
		env_set( $2, $3 );
	}
	| BI_ENV_UNSET WORD
	{
		env_unset( $2 );
	}
	;

alias:
	BI_ALIAS WORD argument
	{
		alias_set( $2, $3 );
	}
	| BI_ALIAS //prints all aliases
	{
		alias_print();
	}
	| BI_ALIAS_UNSET WORD
	{
		alias_unset( $2 );
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

command_arguments:
	| command_arguments argument
	{
		add_arg( $2 );
	}
	;

pipe:
	| pipe PIPE_NEXT command command_arguments
	;

/* Redirection Command Handling */

redirections:
	redirection_input redirection_output_std redirection_output_err
	;

redirection_input:
	| REDIR_STDIN argument
	{
		redirect_input_setup( $2 );
	}
	;

redirection_output_std:
	| REDIR_STDOUT_APPEND argument
	{
		redirect_output_append_setup( $2 );
	}
	| REDIR_STDOUT_OVERWRITE argument
	{
		redirect_output_overwrite_setup( $2 );
	}
	;

redirection_output_err:
	| REDIR_STDERR_STDIN
	| REDIR_STDERR_FILE WORD
	{
		redirect_stderr_file_setup( $2 );
	}
	;
