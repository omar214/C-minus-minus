%{
// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // for directory
#include <stdarg.h> // to accept variable number of arguments

// Header Files
#include "parser.h"

// User-defined Prototype


// Lexical Prototype
void yyerror(char *);
int yylex(void);
int yyparse(void);

// Options
#define YYERROR_VERBOSE // to print error messages in detail


// Global Variables
extern int debug ;
extern int yylineno;


// Global declared variables
/*
    int yylineno: line number 
    char * yytext: pointer to the matched string 
    int yyleng: length of the matched string 
    FILE * yyin: input stream 
    FILE * yyout: output stream 
    YYSTYPE yylval: value of the token
    yydebug: debug mode
    yywrap: return 1 if end of file is reached, else 0
*/

%}

// Data Types
%token INT_TYPE FLOAT_TYPE CHAR_TYPE STRING_TYPE BOOL_TYPE CONST VOID


// Values
// TODO: specify the types & based on representation
%token INT FLOAT CHAR STRING BOOL

// Keywords
%token IF ELSE DO WHILE FOR SWITCH CASE DEFAULT BREAK  RETURN

// Identifiers
%token IDENTIFIER

// punctuators
%token SEMICOLON COMMA COLON

%nonassoc IFX
%nonassoc ELSE

// Operators
%right ASSIGNMENT
%left OR
%left AND
%left EQUAL_TO NOT_EQUAL
%left GT LT GTE LTE
%left ADD SUB
%left MUL DIV MOD
%right NOT
%nonassoc UMINUS


%%

program: 
    statements;

statements: 
        statement
    |   statements statement
    ;

statement:
        declaration SEMICOLON   { printf("declaration: %d\n", $1);}
    |   assigment   SEMICOLON   { printf("assigment: %d\n", $1); }
    |   expr        SEMICOLON   { printf("expr: %d\n", $1);}
    |   ;            { printf("empty statement\n"); }
    |   keywords    SEMICOLON   { printf("keyword: %d\n", $1);}
	/* |   if_statement 
	|   do_while_statement SEMICOLON 
	|   while_statement 
	|   for_statement 
	|   switch_statement    
	|   function       */
	;  

data_type:
        INT_TYPE        { $$ = 1; if(debug) printf("data_type: %d\n", 1); }
    |   FLOAT_TYPE      { $$ = 2; if(debug) printf("data_type: %d\n", 2); }
    |   CHAR_TYPE       { $$ = 3; if(debug) printf("data_type: %d\n", 3); }
    |   STRING_TYPE     { $$ = 4; if(debug) printf("data_type: %d\n", 4); }
    |   BOOL_TYPE       { $$ = 5; if(debug) printf("data_type: %d\n", 5); }
    ;

data_value:
        INT
    |   FLOAT
    |   CHAR
    |   STRING
    |   BOOL
    ;


declaration:
        data_type IDENTIFIER 
    |   data_type IDENTIFIER ASSIGNMENT expr
    |   CONST data_type IDENTIFIER ASSIGNMENT expr
    ;

assigment: 
        IDENTIFIER ASSIGNMENT expr
    ;

expr:   
        data_value              { $$ = $1;       if(debug) printf("expr 1: %d\n", $1);}
    |   IDENTIFIER              { $$ = $1;       if(debug) printf("expr 2: %d\n", $1); }    
    |   SUB expr %prec UMINUS   { $$ = -$2;      if(debug) printf("expr 3: %d\n", -$2); }
    |   NOT expr                { $$ = !$2;      if(debug) printf("expr 4: %d\n", !$2); }
    |   expr ADD expr           { $$ = $1 + $3;  if(debug) printf("expr 5: %d\n", $1 + $3);}
    |   expr SUB expr           { $$ = $1 - $3;  if(debug) printf("expr 6: %d\n", $1 - $3);}
    |   expr MUL expr           { $$ = $1 * $3;  if(debug) printf("expr 7: %d\n", $1 * $3);}
    |   expr DIV expr           { $$ = $1 / $3;  if(debug) printf("expr 8: %d\n", $1 / $3);}
    |   expr MOD expr           { $$ = $1 % $3;  if(debug) printf("expr 9: %d\n", $1 % $3);}
    |   expr LT expr            { $$ = $1 < $3;  if(debug) printf("expr10: %d\n", $1 < $3);}
    |   expr GT expr            { $$ = $1 > $3;  if(debug) printf("expr11: %d\n", $1 > $3);}
    |   expr GTE expr           { $$ = $1 >= $3; if(debug) printf("expr12: %d\n", $1 >= $3);}
    |   expr LTE expr           { $$ = $1 <= $3; if(debug) printf("expr13: %d\n", $1 <= $3);}
    |   expr NOT_EQUAL expr     { $$ = $1 != $3; if(debug) printf("expr14: %d\n", $1 != $3);}
    |   expr EQUAL_TO expr      { $$ = $1 == $3; if(debug) printf("expr15: %d\n", $1 == $3);}
    |   expr AND expr           { $$ = $1 && $3; if(debug) printf("expr16: %d\n", $1 && $3);}
    |   expr OR expr            { $$ = $1 || $3; if(debug) printf("expr17: %d\n", $1 || $3);}
    |   '(' expr ')'            { $$ = $2;       if(debug) printf("expr: %d\n", $2); }
    /* TODO: function call */
    ;

keywords:
        IF     
    |   ELSE   
    |   DO     
    |   WHILE  
    |   FOR    
    |   SWITCH 
    |   CASE   
    |   DEFAULT
    |   BREAK  
    |   RETURN 
    |   SEMICOLON
    |   COMMA
    |   COLON
    |   CONST
    |   data_type
    ;


%%

void yyerror(char *s) {
    printf("\n----- Error -----\n");
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    printf("debug: %d\n", debug);
    printf("----------------- start -----------------\n");
    yyparse();
    return 0;
}
