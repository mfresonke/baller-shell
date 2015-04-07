#ifndef B_SHELL_STRING_MANIPULATION_H
#define B_SHELL_STRING_MANIPULATION_H

#include <stdlib.h>
#include <string.h>
#include "builtins.h"

/* Current Convention will be to never delete strings after manipulating.
	In other words, always return a newly allocated string. */

/** Current Main Filter for Strings with Quotes */
char* str_filter( char *str );

/** Takes a word, and appends "./" in order to turn it into a relative path. */
char* word_to_rel_path( char *word );

//Currently Unnecessary.
char* path_rel_to_abs( char *path_rel );

#endif