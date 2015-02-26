BASENAME=shell

all:
	flex -v $(BASENAME).l
	bison -d $(BASENAME).y
	cc lex.yy.c $(BASENAME).tab.c -o $(BASENAME).bin -ll

clean:
	rm $(BASENAME).bin $(BASENAME).tab.c $(BASENAME).tab.h lex.yy.c
