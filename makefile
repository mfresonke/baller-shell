BASENAME=shell
TEMP_FOLDER=output

all:
	mkdir -p output
	flex -v $(BASENAME).l
	bison -d $(BASENAME).y
	clang -g lex.yy.c $(BASENAME).tab.c -o shell commands.c builtins.c string_manipulation.c errors.c -ll
	cp lex.yy.c $(BASENAME).tab.c $(BASENAME).tab.h shell.lex.h ./output
	rm lex.yy.c $(BASENAME).tab.c $(BASENAME).tab.h shell.lex.h
