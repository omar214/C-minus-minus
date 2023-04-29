build: 
	@echo -e \n----------- Running Bison on Parser -----------
	bison --yacc parser.y -d

	@echo -e \n----------- Running Flex on lexer -----------
	flex lexer.l
	
	@echo -e \n----------- Compiling both -----------
	gcc  -o compiler.exe y.tab.c lex.yy.c
	
run:
	@echo -e \n----------- Running the compiler -----------
	@echo -e not implemented yet

clean: 
	@echo -e ----------- Cleaning The Directory -----------
	rm -f y.tab.* lex.yy.c *.o *.out *.exe

all: clean build run