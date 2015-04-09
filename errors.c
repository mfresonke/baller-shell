#include "errors.h"

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
	printf( "Error! Failed to set up input redirection: %s", reason );
}

