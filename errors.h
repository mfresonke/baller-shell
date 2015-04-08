#ifndef B_ERRORS_H
#define B_ERRORS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error_command_not_found( char *cmd );
void error_directory_invalid();
void error_path_search( char *reason );
void error_set_environment_variable( char *word );
void error_unset_environment_variable( char *word );

#endif