BASENAME=shell
TEMP_FOLDER=output

all:
	mkdir -p output
	flex -v $(BASENAME).l
	bison -d $(BASENAME).y
	cc lex.yy.c $(BASENAME).tab.c -o $(BASENAME).bin commands.c builtins.c string_manipulation.c errors.c -ll
	cp lex.yy.c $(BASENAME).tab.c $(BASENAME).tab.h $(BASENAME).bin shell.lex.h ./output
	rm lex.yy.c $(BASENAME).tab.c $(BASENAME).tab.h $(BASENAME).bin shell.lex.h

clean:
	cd ${TEMP_FOLDER}
	rm $(BASENAME).tab.c $(BASENAME).tab.h lex.yy.c
	cd ..
	rm $(BASENAME).bin
