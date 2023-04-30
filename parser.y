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
%token IF ELSE DO WHILE FOR SWITCH CASE DEFAULT BREAK CONTINUE RETURN

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
    statements;     // TODO: execute() , freeNode()

statements: 
        statement
    |   statements statement
    ;

statement:  {printf("statement: \n\t");}
        declaration SEMICOLON   { printf("declaration: \n");}
    |   assigment   SEMICOLON   { printf("assigment: \n"); }
    |   expr SEMICOLON      {printf("expr: \n\t");}    
    |   if_statement        { printf("if_statement: \n\t");}
    |   switch_statement    { printf("switch_statement: \n");}
    |   for_statement { printf("for_statement: \n");} 
    |   while_statement     { printf("while_statement: \n");}
    |   do_while_statement { printf("do_while_statement: \n");}  
    /* |   function_statement  { printf("function_statement: \n");} */

	|   RETURN expr SEMICOLON { printf("return_statement: \n");}
    |   '{' statements '}'
    |   '{' '}'
    |   SEMICOLON
    /* |   error SEMICOLON
    |   error '}' */
	;

data_type:
        INT_TYPE        { $$ = 1; if(debug) printf("data_type: int \n"); }
    |   FLOAT_TYPE      { $$ = 2; if(debug) printf("data_type: float\n"); }
    |   CHAR_TYPE       { $$ = 3; if(debug) printf("data_type: char\n"); }
    |   STRING_TYPE     { $$ = 4; if(debug) printf("data_type: string\n"); }
    |   BOOL_TYPE       { $$ = 5; if(debug) printf("data_type: bool\n"); }
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
        data_value              { if(debug) printf("expr : \n");}
    |   IDENTIFIER              { if(debug) printf("expr : \n"); }    
    |   SUB expr %prec UMINUS   { if(debug) printf("UMINUS expr :\n"); }
    |   NOT expr                { if(debug) printf("not expr : \n"); }
    |   expr ADD expr           { if(debug) printf("add expr : \n");}
    |   expr SUB expr           { if(debug) printf("sub expr : \n");}
    |   expr MUL expr           { if(debug) printf("mul expr : \n");}
    |   expr DIV expr           { if(debug) printf("div expr : \n");}
    |   expr MOD expr           { if(debug) printf("mod expr : ");}
    |   expr LT expr            { if(debug) printf("LT expr: \n");}
    |   expr GT expr            { if(debug) printf("GT expr: \n");}
    |   expr GTE expr           { if(debug) printf("GTe expr: \n");}
    |   expr LTE expr           { if(debug) printf("LTE expr: \n");}
    |   expr NOT_EQUAL expr     { if(debug) printf("!= expr: \n");}
    |   expr EQUAL_TO expr      { if(debug) printf("== expr: \n");}
    |   expr AND expr           { if(debug) printf("&& expr: \n");}
    |   expr OR expr            { if(debug) printf("|| expr: \n");}
    |   '(' expr ')'            { $$ = $2;       if(debug) printf("parenthes expr: %d\n", $2); }
    /* TODO: function call */
    ;

if_statement: 
        IF '(' expr ')' statement %prec IFX         {printf("single if \n");}
    |   IF '(' expr ')' statement ELSE statement    {printf("if_else \n");}
    ;

case_statement:
        case_statement CASE data_value COLON statements BREAK SEMICOLON
    |   case_statement CASE data_value COLON  BREAK SEMICOLON
    |  /* empty */
    ;

case_default_statement:
        DEFAULT COLON statements
    |   DEFAULT COLON statements BREAK SEMICOLON
    |   DEFAULT COLON BREAK SEMICOLON
    ;

switch_statement:
        SWITCH '(' IDENTIFIER ')' '{' case_statement case_default_statement '}'

loop_statement:
        statement
    |   BREAK SEMICOLON      { printf("break: %d\n", $1);}
    |   CONTINUE SEMICOLON   { printf("continue: %d\n", $1);}

for_init:
        data_type IDENTIFIER ASSIGNMENT expr
    |   assigment 
    ;  

for_statement:
        FOR '(' for_init SEMICOLON expr SEMICOLON  assigment')' loop_statement
    ;

while_statement:
        WHILE '(' expr ')' loop_statement
    ;

do_while_statement:
        DO statement WHILE '(' expr ')' SEMICOLON   //TODO: check if loop_statement is 
    ;



function_arguments:
        data_type IDENTIFIER COMMA function_arguments
    |   data_type IDENTIFIER
    |   /* empty */
    ;

function_statement_list:
        statement function_statement   
    /* |   RETURN expr SEMICOLON */
    |   statement
    ;

void_function_statement_list:
        void_function_statement_list statement 
    |   RETURN SEMICOLON
    |   statement 
    |   
    ;

function_statement:
        data_type IDENTIFIER '(' function_arguments ')' '{' function_statement_list '}'
    |   VOID IDENTIFIER '(' function_arguments ')' '{' void_function_statement_list '}'  // has no return value 
    ;


%%

void yyerror(char *s) {
    printf("\n----- Error -----\n");
    fprintf(stderr, "%s Error at line [%d]\n", s , yylineno);
}

int main(void) {
    printf("debug: %d\n", debug);
    printf("----------------- start -----------------\n");
    yyparse();
    return 0;
}
