#ifndef B_ERRORS_H
#define B_ERRORS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error_alias_loop_detected();

void error_retrieving_line_of_input_failed();
void error_command_not_found( char *cmd );
void error_directory_invalid();
void error_path_search( char *reason );

void error_set_environment_variable( char *word );
void error_unset_environment_variable( char *word );

void error_redirect_input( char *reason );
void error_redirect_output( char *reason );

void error_alias_not_exist( char *name );
void error_applying_aliases( char *reason );

void error_env_var_not_found( char *env_var );

void error_invalid_wildcard();


#endif