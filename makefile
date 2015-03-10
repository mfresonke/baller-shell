BASENAME=shell

all:
	flex -v $(BASENAME).l
	bison -d $(BASENAME).y
	cc lex.yy.c $(BASENAME).tab.c -o $(BASENAME).bin helpers.c builtins.c -ll

clean:
	rm $(BASENAME).bin $(BASENAME).tab.c $(BASENAME).tab.h lex.yy.c
