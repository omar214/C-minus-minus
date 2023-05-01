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
         program stmt
        | /* NULL */
        ;
type:
        INT_TYPE   
    |   FLOAT_TYPE 
    |   CHAR_TYPE  
    |   BOOL_TYPE  
    |   STRING_TYPE
    ;
    
stmt_list:
          stmt                                                                        
        | stmt_list stmt                                                              
        ;

stmt:
          SEMICOLON                                                                                 
        | expr SEMICOLON                                                                            

        | type IDENTIFIER SEMICOLON                                                                   
        | type IDENTIFIER ASSIGNMENT expr SEMICOLON                                                   
        | CONST type IDENTIFIER ASSIGNMENT expr SEMICOLON                                             
        | IDENTIFIER ASSIGNMENT expr SEMICOLON 
        | enum_statement                                                                                           

        | WHILE '(' expr ')' stmt                                                             
        | DO stmt WHILE '(' expr ')' SEMICOLON                                                      
        | FOR '(' IDENTIFIER ASSIGNMENT expr SEMICOLON expr SEMICOLON IDENTIFIER ASSIGNMENT expr ')' stmt     
                                                                                        
        | IF '(' expr ')' stmt %prec IFX                                                      
        | IF '(' expr ')' stmt ELSE stmt                                                      

        | SWITCH '(' IDENTIFIER ')' '{' case_list case_default '}'                              
        | BREAK SEMICOLON                                                                           
        | type IDENTIFIER func_list '{' func_stmt_list '}'                                      
        | VOID IDENTIFIER func_list '{' stmt_list '}'                                           
        | VOID IDENTIFIER func_list '{' '}'                                                     
        | '{' stmt_list '}'                                                                   
        | '{' '}'                                                                             
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
        case_list CASE INT COLON stmt_list      
    |   case_list CASE CHAR COLON stmt_list         
    |   case_list CASE STRING COLON stmt_list       
    |   case_list CASE BOOL COLON stmt_list
    |  /* NULL */                                 
    ;


case_default: DEFAULT COLON stmt_list                                                          

expr:
    INT                                                                                  
    | FLOAT                                                                                  
    | CHAR                                                                                   
    | STRING                                                                                 
    | BOOL                                                                           
    | IDENTIFIER                                                                             
    | SUB expr %prec UMINUS                                                                
    | NOT expr                                                                               
    /* Mathematical */
    | expr ADD expr                                                                         
    | expr SUB expr                                                                        
    | expr MUL expr                                                                          
    | expr DIV expr                                                                          
    | expr MOD expr        

    /* Logical */
    | expr LT expr                                                                            
    | expr GT expr                                                                            
    | expr GTE expr                                                                           
    | expr LTE expr                                                                           
    | expr NOT_EQUAL expr                                                                        
    | expr EQUAL_TO expr                                                                         
    | expr AND expr                                                                          
    | expr OR expr                                                                           
    | IDENTIFIER call_list                                                                   
    | '(' expr ')'                                                                           
    ;


func_stmt_list:
          RETURN expr SEMICOLON                                                                  
        | stmt func_stmt_list                                                              
        ;

func_var_list:
          type IDENTIFIER                                                                  
        | type IDENTIFIER COMMA func_var_list                                                
        ;

func_list:
    '(' func_var_list ')'                                                            
    | '(' ')'                                                                        
;

call_var_list:
          expr                                                                       
        | call_var_list COMMA expr                                                     
        ;

call_list:
    '(' call_var_list ')'                                                            
    | '(' ')'                                                                        
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
        yydebug = 1;
        debug = 0;
    #endif
    yyparse();
    return 0;
}
