BASENAME=shell

all:
	flex -v $(BASENAME).l
	bison -d $(BASENAME).y
	cc lex.yy.c $(BASENAME).tab.c -o $(BASENAME).bin -ll
