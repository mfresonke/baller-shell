#ifndef B_SHELL_STRING_MANIPULATION_H
#define B_SHELL_STRING_MANIPULATION_H

#include <stdlib.h>
#include <string.h>
#include "builtins.h"

/* Current Convention will be to never delete strings after manipulating.
	In other words, always return a newly allocated string. */

/** Current Main Filter for Strings with Quotes */
char* str_filter( char *str );

void print_string_elements( char *str );

/** Combines a directory (e.g. /bin) with a filename (e.g. echo) to create a full absolute path. (e.g /bin/echo) */
char* combine_dir_and_file( char *dir, char *file );

/** Takes a word, and prepends "./" in order to turn it into a relative path. */
char* word_to_rel_path( char *word );

/** Takes a relative path, and turns it into a (crappy) absolute path that exec will understand. */
char* path_rel_to_abs( char *path_rel );

/** replaces the text from "index_start" to "index_end" with the text specified in "replacement". */
char* replace_text( char *original, size_t replace_start, size_t replace_end, char *replacement );

/** Does the same thing as above, except it takes in pointers. replace_start & replace_end MUST BE PART OF str_orig! */
char* replace_text_pointers( char *str_orig, char *replace_start, char *replace_end, char *str_rep );

int get_char_pointer_index( char *str, char *search );

/** Creates a new string from start to end, inclusive. */
char* new_string_from_pointers( char *start, char *end );



#endif