#include "string_manipulation.h"

/** Special Function for filtering strings. Either returns the same string, or a new one (where the old one has been deleted. */
char* str_filter( char *str )
{
	//remove quotes if they exist
	size_t length = strlen( str );

	//if the first and last characters are quotes....
	if( str[0] == '"' && str[length-1] == '"' )
	{
		//remove them! //remember that length - 2 + 1 (null char)
		char *new_str = calloc( (length-1), sizeof(char) );		
		int a;
		//copy the characters
		strncpy( new_str, &str[1], (length-2) );
		free( str );
		return new_str;
	}
	return str;
}

char* word_to_rel_path( char *word )
{
	size_t length_word = strlen( word );
	//make a new string of length 3, ( 2 for ./ + 1 for null char )
	char *path_rel = malloc( ( length_word + 3 ) * sizeof( char ) );
	strcpy( path_rel, "./" );
	strcat( path_rel, word );

	return path_rel;
}

char* path_rel_to_abs( char *path_rel )
{
	char *pwd = get_cd();
	size_t pwd_size = strlen( pwd );
	size_t path_rel_size = strlen( path_rel );

	char *path_abs = malloc( ( pwd_size + path_rel_size ) * sizeof( char ) );

	//copy the current directory into newly allocated string.
	strcpy( path_abs, pwd );
	strcat( path_abs, &path_rel[1] );

	return path_abs;
}

