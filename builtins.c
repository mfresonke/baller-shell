#include "builtins.h"

void cd( char *dir )
{
	//change enviornment variables
	setenv( "OLDPWD", getenv("PWD"), 1 );
	setenv( "PWD", dir, 1 );
	//change working directory
	chdir( dir );
}