all: compiler run
clean: rm
	
parser.tab.c parser.tab.h: code/parser.y
	bison -v -d code/parser.y  ;
	mv parser.output outputs/;
	mv parser.tab.c code/;
	mv parser.tab.h code/;
scanner.c: code/scanner.l headerfiles/tokens.h
	lex --outfile=code/scanner.c code/scanner.l;
symtable.o:	code/symtable.c 
	gcc -c -g	code/symtable.c -o  obj/symtable.o;
functions.o:	code/functions.c
	gcc -c -g	code/functions.c -o  obj/functions.o;
instruction.o:	code/instruction.c
	gcc -c -g	code/instruction.c -o  obj/instruction.o;
prints.o:	code/prints.c
	gcc -c -g	code/prints.c -o  obj/prints.o;
stack.o:	code/stack.c
	gcc -c -g code/stack.c -o  obj/stack.o
create_abc_file.o:	code/create_abc_file.c
	gcc -c -g code/create_abc_file.c -o obj/create_abc_file.o
read_abc.o:	code/read_abc.c
	gcc -c -g code/read_abc.c -o obj/read_abc.o
toString.o: code/toString.c
	gcc -c -g code/toString.c -o obj/toString.o

environment.o: code/environment.c
	gcc -c -g code/environment.c -o obj/environment.o

tables_avm.o: code/tables_avm.c
	gcc -c -g code/tables_avm.c -o obj/tables_avm.o

execution.o: code/execution.c
	gcc -c -g code/execution.c -o obj/execution.o
execution_help.o: code/execution_help.c 
	gcc -c -g code/execution_help.c -o obj/execution_help.o 
libraries.o:	code/libfuncs.c
	gcc -c -g code/libfuncs.c -o obj/libraries.o 
compiler: scanner.c parser.tab.c symtable.o functions.o prints.o stack.o instruction.o create_abc_file.o
	gcc  -g obj/symtable.o obj/functions.o obj/stack.o obj/prints.o obj/instruction.o obj/create_abc_file.o code/parser.tab.c code/scanner.c  -o compiler;
run: read_abc.o toString.o environment.o tables_avm.o execution.o execution_help.o  libraries.o
	gcc -g  obj/read_abc.o obj/toString.o obj/environment.o obj/tables_avm.o obj/execution.o obj/execution_help.o obj/libraries.o code/main_avm.c -o run -lm
rm:	 
	rm -rf obj/*.o code/*.gch outputs/* code/scanner.c code/parser.tab.c code/parser.tab.h binary/* compiler run ;
