#include "errors.h"

void error_directory_invalid()
{
	printf("Invalid Directory.\n");
}

void error_path_search( char *reason )
{
	printf("Error while searching the PATH: %s\n", reason );
}

void error_command_not_found( char *cmd )
{
	printf( "Error! Command %s not found!\n", cmd );
}
