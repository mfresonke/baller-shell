#include "errors.h"

void error_retrieving_line_of_input_failed()
{
	printf("Error! Could not retrieve line of input. Please try again.\n");
}

void error_directory_invalid()
{
	printf("Error! Invalid Directory.\n");
}

void error_path_search( char *reason )
{
	printf("Error! Searching the PATH failed: %s\n", reason );
}

void error_command_not_found( char *cmd )
{
	printf( "Error! Command %s not found!\n", cmd );
}


void error_set_environment_variable( char *vname )
{
	printf( "Error! Could not set environment variable %s!\n", vname );
}

void error_unset_environment_variable( char *vname )
{
	printf( "Error! Could not unset environment variable %s!\n", vname );
}

void error_redirect_input( char *reason )
{
	printf( "Error! Failed to set up input redirection: %s\n", reason );
}

void error_alias_not_exist( char *name )
{
	printf( "Error! Alias %s does not exist!\n", name );
}

void error_applying_aliases( char *reason )
{
	printf( "Error! Falied to apply Alias(es): %s\n", reason );
}
