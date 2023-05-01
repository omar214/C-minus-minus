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
%token ENUM

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
        program statement
    | /* Empty Statement */
    ;

statement_list:
        statement                                                                        
    |   statement_list statement                                                              
    ;

data_type:
        INT_TYPE   
    |   FLOAT_TYPE 
    |   CHAR_TYPE  
    |   BOOL_TYPE  
    |   STRING_TYPE
    ;
    

statement:
        SEMICOLON                                                                                 
    |   expr SEMICOLON                                                                            

    /* Declaration & Assignment */
    |   data_type IDENTIFIER SEMICOLON                                                                   
    |   data_type IDENTIFIER ASSIGNMENT expr SEMICOLON                                                   
    |   CONST data_type IDENTIFIER ASSIGNMENT expr SEMICOLON                                             
    |   IDENTIFIER ASSIGNMENT expr SEMICOLON 
    |   enum_statement                                                                                           

    /* Loop statment */
    |   WHILE '(' expr ')' statement                                                             
    |   DO statement WHILE '(' expr ')' SEMICOLON                                                      
    |   FOR '(' IDENTIFIER ASSIGNMENT expr SEMICOLON expr SEMICOLON IDENTIFIER ASSIGNMENT expr ')' statement     
                    
    /*  IF statment*/
    |   IF '(' expr ')' statement %prec IFX                                                      
    |   IF '(' expr ')' statement ELSE statement                                                      

    /* Switch statement */
    |   SWITCH '(' IDENTIFIER ')' '{' case_list case_default '}'                              
    |   BREAK SEMICOLON

    /* Function Statement */
    |   data_type IDENTIFIER function_arguements_list '{' function_statement_list '}'                                      
    |   VOID IDENTIFIER function_arguements_list '{' statement_list '}'                                           
    |   VOID IDENTIFIER function_arguements_list '{' '}'   

    /* Block Statement */
    |   '{' statement_list '}'                                                                   
    |   '{' '}'                                                                             
    |   error SEMICOLON                                                                         
    |   error '}'                                                                         
    ;


enum_arguments:
        enum_arguments COMMA IDENTIFIER
    |   IDENTIFIER
    ;

enum_statement:
        ENUM '{' enum_arguments '}' IDENTIFIER SEMICOLON
    ;

case_list:
        case_list CASE INT COLON statement_list      
    |   case_list CASE CHAR COLON statement_list         
    |   case_list CASE STRING COLON statement_list       
    |   case_list CASE BOOL COLON statement_list
    |  /* Empty statement */                                 
    ;


case_default: 
        DEFAULT COLON statement_list                                                          
    ;

expr:
        INT                                                                                  
    |   FLOAT                                                                                  
    |   CHAR                                                                                   
    |   STRING                                                                                 
    |   BOOL                                                                           
    |   IDENTIFIER                                                                             
    |   SUB expr %prec UMINUS                                                                
    |   NOT expr                                                                               
    /* Mathematical */
    |   expr ADD expr                                                                         
    |   expr SUB expr                                                                        
    |   expr MUL expr                                                                          
    |   expr DIV expr                                                                          
    |   expr MOD expr        

    /* Logical */
    |   expr LT expr                                                                            
    |   expr GT expr                                                                            
    |   expr GTE expr                                                                           
    |   expr LTE expr                                                                           
    |   expr NOT_EQUAL expr                                                                        
    |   expr EQUAL_TO expr                                                                         
    |   expr AND expr                                                                          
    |   expr OR expr
    
    /* function call or grouped */                                                                        /*  */
    |   IDENTIFIER function_call                                                                   
    |   '(' expr ')'                                                                           
    ;


function_statement_list:
        RETURN expr SEMICOLON                                                                  
    |   statement function_statement_list                                                              
    ;

function_arguements:
        data_type IDENTIFIER                                                                  
    |   data_type IDENTIFIER COMMA function_arguements                                                
    ;

function_arguements_list:
        '(' function_arguements ')'                                                            
    |   '(' ')'                                                                        
;

function_arguements_call:
        expr                                                                       
    |   function_arguements_call COMMA expr                                                     
    ;

function_call:
        '(' function_arguements_call ')'                                                            
    |   '(' ')'                                                                        
;


%%

void yyerror(char *s) {
    printf("\n----- Error -----\n");
    fprintf(stderr, "%s Error at line [%d]\n", s , yylineno);
}

int main(void) {
    printf("debug: %d\n", debug);
    printf("----------------- start -----------------\n");
    #if defined(YYDEBUG) && (YYDEBUG==1)
        yydebug = 0;
        debug = 0;
    #endif
    yyparse();
    printf("\n----------------- Parse End -----------------");
    return 0;
}
