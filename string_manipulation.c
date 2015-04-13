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

void print_string_elements( char *str )
{
	printf("Printing string: %s\n", str);
	while( *str  )
	{
		printf("%c ", *str);
		++str;
	}
	printf("\n");
}

char* combine_dir_and_file( char *dir, char *file )
{
	size_t length_dir = strlen( dir );
	size_t length_file = strlen( file );
	// +2 due to 1 char for the '/', and another for the null char (\0)
	size_t length_total = length_dir + length_file + 2;
	char *new_str = calloc( length_total, sizeof(char) );
	strncpy( new_str, dir, length_dir );
	strcat( new_str, "/" );
	strcat( new_str, file );

	return new_str;
}

char* word_to_rel_path( char *word )
{
	size_t length_word = strlen( word );
	//make a new string of length 3, ( 2 for ./ + 1 for null char )
	char *path_rel = calloc( ( length_word + 3 ), sizeof( char ) );
	strcpy( path_rel, "./" );
	strcat( path_rel, word );

	return path_rel;
}

char* path_rel_to_abs( char *path_rel )
{
	char *pwd = get_cd();
	size_t pwd_size = strlen( pwd );
	size_t path_rel_size = strlen( path_rel );

	char *path_abs = calloc( ( pwd_size + path_rel_size ), sizeof( char ) );

	//copy the current directory into newly allocated string.
	strcpy( path_abs, pwd );
	strcat( path_abs, &path_rel[1] );

	return path_abs;
}

char* replace_text( char *str_orig, size_t replace_start, size_t replace_end, char *str_rep )
{
	//size of original, to help with calculation
	size_t length_orig = strlen( str_orig );

	//get the sizes of the individual pieces we are putting together
	size_t length_orig_beg = replace_start;
	size_t length_rep = strlen( str_rep );
	size_t length_orig_end = ( length_orig - replace_end + 1 );

	//add them all up, accounting for the ending char.
	size_t length_new = ( length_orig_beg + length_rep + length_orig_end + 1 );

	char *new_str = calloc( length_new, sizeof(char) );

	//copy the pieces into place!
	strncpy( new_str, str_orig, length_orig_beg );
	strncat( new_str, str_rep, length_rep );
	if( replace_end < ( length_orig - 1 ) )
		strncat( new_str, &str_orig[ replace_end+1 ], length_orig_end );

	return new_str;
}

char* replace_text_pointers( char *str_orig, char *replace_start, char *replace_end, char *str_rep )
{
	//just find the indicies so you can pass the values to the real replace_text function.
	size_t replace_start_index = 0;
	size_t replace_end_index = 0;
	char *curr;

	//compute the index of replace_start. NOTICE THERE IS NO DEREFERENCING HERE! We are comparing memory addresses!
	for( curr = str_orig; curr != replace_start; ++curr )
		++replace_start_index;
	for( curr = str_orig; curr != replace_end; ++curr )
		++replace_end_index;

	return replace_text( str_orig, replace_start_index, replace_end_index, str_rep );
}

int get_char_pointer_index( char *str, char *search )
{
	int a;
	for( a = 0; str[a]; ++a )
	{
		if( search == &str[a] )
			return a;
	}
	return -1;
}

char* new_string_from_pointers( char *start, char *end )
{
	char *curr = start;
	size_t count = 0;

	while( curr < end )
	{
		++count;
		++curr;
	}

	//allocate new string
	char *str = calloc( count + 1, sizeof( char ) );
	strncpy( str, start, count );

	return str;
}


