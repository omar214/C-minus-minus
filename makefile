bison:
	@echo -e \n----------- Running Bison on Parser -----------
	bison --yacc parser.y -d --debug

flex: 
	@echo -e \n----------- Running Flex on lexer -----------
	flex lexer.l

compile:
	@echo -e \n----------- Compiling both -----------
	gcc -g -o compiler.exe y.tab.c lex.yy.c


build: bison flex compile
	
run:
	@echo -e \n----------- Running the compiler -----------
	./compiler.exe

clean: 
	@echo -e ----------- Cleaning The Directory -----------
	rm -f y.tab.* lex.yy.c *.o *.out *.exe

all: clean build run