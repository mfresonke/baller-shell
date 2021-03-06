%{
#define SHELL_EXIT -5
#define SYNTAX_ERROR -1
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
	//prevent shell from terminating
	signal(SIGINT, sigintHandler);

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
			free( input );
			input = NULL;
			continue;
		}

		char *input_add = input;
		while( ( strlen(input) > 1 ) && ( input_add[ strlen(input_add) - 2 ] == '\\' ) )
		{
			input_add = calloc( sizeof(char), MAX_LINE_SIZE );
			fgets( input_add, MAX_LINE_SIZE, stdin );
			strcat( input, input_add );
			input_add = input;
		}

		//get rid of any forward whitespace
		char *input_no_whitespace = input;
		while( input_no_whitespace[0] == ' ' )
		{
			++input_no_whitespace;
		}
		input_no_whitespace = strdup( input_no_whitespace );
		free( input );
		input = input_no_whitespace;

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
		free( input );
		input = NULL;
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
%token <string> WILDCARD 

%type <string> argument

%%


/* Common to All Types */

input:
	| command_end
	| builtin command_end
	| command command_arguments pipe redirections command_end
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
	BI_CD WILDCARD
	{
		wildcard_cd( $2 );
	}
	| BI_CD PATH_ABS
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
	//error checking
	| BI_CD PATH_ABS error 
	{ 
		printf( "cd accepts only one argument!\n" );
		return(SYNTAX_ERROR); 
	}
	| BI_CD PATH_REL error 
	{ 
		printf( "cd accepts only one argument!\n" ); 
		return(SYNTAX_ERROR);
	}
	| BI_CD WORD error 
	{ 
		printf( "cd accepts only one argument!\n" ); 
		return(SYNTAX_ERROR);
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
	//error checking
	| BI_ENV_SET error 
	{ 
		printf( "setenv requires two arguments!\n" ); 
		return(SYNTAX_ERROR);
	}
	| BI_ENV_UNSET error 
	{ 
		printf( "usetenv requires one argument!\n" ); 
		return(SYNTAX_ERROR);
	}
	| BI_ENV_PRINT error 
	{ 
		printf( "printenv accepts no arguments!\n" ); 
		return(SYNTAX_ERROR);
	}
	;

alias:
	BI_ALIAS WORD argument
	{
		alias_set( $2, $3 );
	}
	BI_ALIAS WORD WILDCARD
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
	//error checking
	| BI_ALIAS error
	{
		printf( "alias requires two arguments!\n" ); 
		return(SYNTAX_ERROR);
	}
	| BI_ALIAS_UNSET error
	{
		printf( "unalias requires one argument!\n" ); 
		return(SYNTAX_ERROR);
	}
	| BI_ALIAS WORD argument error
	{
		printf( "alias requires one argument!\n" ); 
		return(SYNTAX_ERROR);
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

command_arguments:
	| command_arguments argument
	{
		if( command_start )
			add_arg( $2 );
	}
	| command_arguments WILDCARD
	{
		if( command_start )
			wildcard_comand_args( $2 );
	}
	;

pipe:
	| pipe PIPE_NEXT command command_arguments
	//error checking
	| pipe PIPE_NEXT error
	{
		printf( "pipe command requires a valid argument!\n" ); 
		return(SYNTAX_ERROR);
	}
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
	//error checking
	| REDIR_STDIN argument error
	{
		printf( "alias requires one argument!\n" ); 
		return(SYNTAX_ERROR);
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
	//error checking
	| REDIR_STDOUT_APPEND argument error
	{
		printf( "output redirect accepts only one file!\n" ); 
		return(SYNTAX_ERROR);
	}
	| REDIR_STDOUT_OVERWRITE argument error
	{
		printf( "output redirect accepts only one file!\n" ); 
		return(SYNTAX_ERROR);
	}
	;

redirection_output_err:
	| REDIR_STDERR_STDIN
	| REDIR_STDERR_FILE WORD
	{
		redirect_stderr_file_setup( $2 );
	}
	//error checking
	| REDIR_STDERR_STDIN error
	{
		printf( "standard error redirect accepts only one argument!\n" ); 
		return(SYNTAX_ERROR);
	}
	| REDIR_STDERR_FILE WORD error
	{
		printf( "standard error redirect accepts only one file!\n" ); 
		return(SYNTAX_ERROR);
	}
	;
