bison:
	@echo -e \n----------- Running Bison on Parser -----------
	bison --yacc parser.y -d --debug

bison_debug:
	@echo -e \n----------- Running Bison on Parser -----------
	bison --yacc parser.y -d --debug --verbose

flex: 
	@echo -e \n----------- Running Flex on lexer -----------
	flex lexer.l

compile:
	@echo -e \n----------- Compiling both -----------
	gcc  -o compiler.exe y.tab.c lex.yy.c

compile_debug:
	@echo -e \n----------- Compiling both -----------
	gcc -g -o compiler.exe y.tab.c lex.yy.c 

build: bison flex compile

build_debug: bison_debug flex compile_debug
	
run:
	@echo -e \n----------- Running the compiler -----------
	./compiler.exe

clean: 
	@echo -e ----------- Cleaning The Directory -----------
	rm -f y.tab.* lex.yy.c *.o *.out *.exe *.output

all: clean build run

debug: clean build_debug run