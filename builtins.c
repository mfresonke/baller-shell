#include "builtins.h"

struct Alias *aliases[MAX_ALIASES];
size_t alias_count = 0;
bool *alias_used = NULL;
char *expansions[MAX_WILDCARD_RESULTS] = { NULL };
size_t expansion_count = 0;

void sigintHandler(int sig_num)
{
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler);

    //fflush(stdout);
}

char* get_cd()
{
	//TODO make this better
	return strdup( getenv( "PWD" ) );
}

void cd( char *dir )
{
	//if dir is NULL (empty) then by default we go back to home.
	if( !dir || dir[0] == '\0' )
	{
		dir = getenv( "HOME" );
	}

	//change working directory
	if( chdir( dir ) < 0 )
	{
		error_directory_invalid();
		return;
	}

	char pwd[MAX_PATH_LENGTH];

	//change enviornment variables
	setenv( "OLDPWD", getenv("PWD"), 1 );
	getcwd( pwd, MAX_PATH_LENGTH );
	setenv( "PWD", pwd, 1 );
	
}

void cd_home()
{
	//Simply Call cd with NULL
	cd( NULL );
}

void cd_word( char *word )
{
	//transform the string into a relative path
	char* path_rel = word_to_rel_path( word );
	//call cd
	cd( path_rel );
}

void env_print()
{
	//duplicate system environ variable
	//for printing
	char **environ_ptr = environ;

	//if an environment variable exists...
	while( *environ_ptr )
	{
			//print out each variable and name
			printf( "%s\n", *environ_ptr );
			++environ_ptr;
	}
}

void env_set( char *vname, char *value )
{

	if( setenv( vname, value, 1 ) < 0 )
	{
		error_set_environment_variable( vname );
		return;
	}
}

void env_unset( char *vname )
{
	if( unsetenv( vname ) < 0 )
	{
		error_unset_environment_variable( vname );
		return;
	}
}

void alias_print()
{
	int a;
	for( a = 0; a != alias_count; ++a )
	{
		printf( "%s=%s\n", aliases[a]->name, aliases[a]->command );
	}
}

void alias_set( char *name, char *command )
{
	//search for if the alias already exists
	int alias_index = alias_search( name );
	if( alias_index != ALIAS_NOT_FOUND )
	{
		//if the alias already exists, just change the command.
		free( aliases[alias_index]->command );
		aliases[alias_index]->command = command;
		return;
	}

	//if the alias doesn't already exist, we need to make one.

	//create a new Alias struct and set its properties.
	struct Alias *alias_new = malloc( sizeof( struct Alias ) );
	alias_new->name = name;
	alias_new->command = command;

	//add it to the array
	aliases[alias_count] = alias_new;
	++alias_count;
}

void alias_unset( char *name )
{
	//search to see if the alias exists
	int alias_index = alias_search( name );

	if( alias_index != ALIAS_NOT_FOUND ) //if the alias exists...
	{
		//free alias properties
		free( aliases[alias_index]->name );
		free( aliases[alias_index]->command );

		//free alias itself
		free( aliases[alias_index] );
		aliases[alias_index] = NULL;

		//decrement alias count
		--alias_count;

		//shift the array
		int a;
		for( a = alias_index; a != alias_count; ++a )
		{
			aliases[a] = aliases[a+1];
		}
		//clean up the last pointer
		aliases[a] = NULL;
	}
	else //alias does not exist...
	{
		error_alias_not_exist( name );
	}
}

int alias_search( char *name )
{
	//check if there are any elements in our list
	if( alias_count == 0 )
	{
		return ALIAS_NOT_FOUND;
	}

	int a;
	//start searching
	for( a = 0; a != alias_count; ++a )
	{
		if( strcmp( aliases[a]->name, name ) == 0 ) //if we found the alias...
		{
			return a;
		}
	}

	//if nothing was found, return ALIAS_NOT_FOUND
	return ALIAS_NOT_FOUND;
}

char* run_preparser( char *input )
{
	if( !input )
		return NULL;
	while( input[0] == ' ' )
		++input;
	input = search_and_apply_aliases( input );
	if( !input )
		return NULL;
	input = search_and_apply_env_vars( input );
	if( !input )
		return NULL;
	return input;
}

char* search_and_apply_aliases( char *input )
{
	int a;
	//create "used" token array.
	bool alias_used[MAX_ALIASES];
	for( a = 0; a != MAX_ALIASES; ++a )
		alias_used[a] = false;

	bool input_changed = true;

	while( input_changed )
	{
		input_changed = false;
		char *token_prev = strdup( input );
		size_t token_index = 0;
		//tokenize input by spaces & quotes 
		char *input_token = strtok( token_prev, " \"\n" );
		if( !input_token )
			return input;
		//we need to add an exception if the first token is the word "alias"
		if( strcmp( input_token, "alias" ) == 0 )
			break;
		if( input_token )
		{
			//search for alias
			int result = alias_search( input_token );
			if( result != ALIAS_NOT_FOUND && !alias_used[result] )
			{
				//apply the alias, and mark it as used. 
				char* input_new = replace_text( input, token_index, ( token_index + strlen( input_token ) - 1 ), aliases[result]->command );
				free( input );
				input = input_new;
				alias_used[result] = true;
				input_changed = true;
				continue;
			}
			else if( alias_used[result] )
			{
				error_alias_loop_detected();
				return NULL;
			}
			token_index = ( strlen( token_prev ) + 1 );
			token_prev = input_token;
			input_token = strtok( NULL, " \"\n" );
		}
	}
	return input;
}

char* search_and_apply_env_vars( char *input )
{
	char *env_var_end = input;
	while( env_var_end )
	{
		char *env_var_token_start = strstr( env_var_end, ENV_TOKEN_START );
		size_t token_start_index = get_char_pointer_index( input, env_var_token_start );
		//clear out env_var_end since it's now not truly the end.
		env_var_end = NULL;
		if( env_var_token_start && strlen( env_var_token_start ) > 2 )
		{
			//account for the TOKEN
			char *env_var_start = ( env_var_token_start + 2 );
			env_var_end = strstr( env_var_start, ENV_TOKEN_END );

			//if the ending token doesn't exist, return.
			if( !env_var_end )
				return input;

			//get the length of the potential env var
			size_t env_var_length = 0;
			char *pos;
			for( pos = env_var_start; pos != env_var_end; ++pos )
				++env_var_length;

			//make env_var its own real string.
			char *env_var = calloc( env_var_length + 1, sizeof(char) );

			strncpy( env_var, env_var_start, env_var_length );

			//get the env var definition!
			char *env_def = getenv( env_var );

			//replace! (if applicable)
			if( env_def )
			{
				char *input_new = NULL;
				input_new = replace_text_pointers( input, env_var_token_start, env_var_end, env_def );
				free( input );
				input = input_new;
				//make env_var_end a pointer to the new string.
				env_var_end = &input[token_start_index+1];
			}
			else
			{
				error_env_var_not_found( env_var );
			}
			//do some cleanup
			free( env_var );
			env_var = NULL;
		}
	}
	return input;
}


void wildcard_comand_args( char *input )
{
	glob_t globs;
	glob( input, GLOB_NOCHECK | GLOB_TILDE, NULL, &globs );
	size_t   glob_count = globs.gl_pathc;
    char   **glob_results = globs.gl_pathv;

    //make a new string
    char line[MAX_LINE_SIZE] = { '\0' };
    size_t a;
    for( a = 0; a != glob_count; ++a )
    {
    	add_arg( glob_results[a] );
    }
}




