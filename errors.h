#ifndef B_ERRORS_H
#define B_ERRORS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error_command_not_found( char *cmd );
void error_directory_invalid();
void error_path_search( char *reason );

#endif