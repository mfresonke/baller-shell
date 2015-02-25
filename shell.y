%{
#include <stdio.h>
#include <string.h>

void yyerror( const char *str )
{
	fprintf( stderr, "error: %s\n",str );
}

int yywrap()
{
	return 1;
}

int main()
{
	yyparse();
}

%}

%token PIPE_IN_FILE PIPE_IN_COMMAND PIPE_OUT_REPLACE PIPE_OUT_APPEND QUOTE BSLASH AND

%union
{
	int number;
	char *string;
}

%token <

%%

commands: /* empty */
	| commands command
	;

command:
	heat_switch
	|
	target_set
	;

heat_switch:
	TOK_HEAT STATE
	{
		if($2)
			printf("\tHeat turned on!\n");
		else
			printf("\tHeat turned off!\n");
	}
	;

target_set:
	TOK_TARGET TOK_TEMPERATURE NUMBER
	{
		printf("\tTemperature set to %d!\n", $3);
	}
	;
