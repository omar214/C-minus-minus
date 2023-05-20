%{
// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // for directory
#include <stdarg.h> // to accept variable number of arguments
#include <stdbool.h> // for boolean

// Header Files
#include "parser.h"
#include "nodes.h"
#include "utils.h"
#include "symbolTable.h"
#include "operations.h"
#include "compile.h"
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
extern FILE* yyin; 
extern FILE* yyout;
FILE* quadrables_file;

// routines
void execute(nodeType *p);  


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

%union {
    int      iValue;        // integer value
    float    fValue;        // float value
    char     cValue;        // char value
    char*    sValue;        // string value
    char*    sIndex;        // sybmol table index
    struct nodeTypeTag * nPtr;         // node pointer 
};

// Data Types
%token INT_TYPE FLOAT_TYPE CHAR_TYPE STRING_TYPE BOOL_TYPE CONST VOID
%token ENUM

// Keywords
%token IF ELSE DO WHILE FOR SWITCH CASE DEFAULT BREAK CONTINUE RETURN

// Values
%token <iValue> INT 
%token <iValue> BOOL
%token <fValue> FLOAT 
%token <cValue> CHAR 
%token <sValue> STRING 

// Identifiers
%token <sIndex> IDENTIFIER

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


%token FUNCTION VOIDFUNCTION FUNCVARLIST CALLVARLIST CALL SYMBOLTABLE
%type <nPtr> statement statement_list expr function_statement_list function_arguements_list function_arguements_call function_call enum_statement case_list case_default enum_arguments data_type program
%type <nPtr> function_arguements 

%%


program:
        program statement   {   
                                execute($2); 
                                // freeNode($2);
                            }    
    | /* Empty Statement */ { /* $$ = NULL; */ }
    ;

statement_list:
        statement                               { $$ = $1; }             
    |   statement_list statement                { $$ = create_oper_node(';', 2, $1, $2); }                  
    ;

data_type: 
        INT_TYPE            { $$ =  create_type_node(typeInt); } 
    |   FLOAT_TYPE          { $$ =  create_type_node(typeFloat); }
    |   CHAR_TYPE           { $$ =  create_type_node(typeChar); }
    |   BOOL_TYPE           { $$ =  create_type_node(typeBool); }
    |   STRING_TYPE         { $$ =  create_type_node(typeString); }
    ;
    

statement:
        SEMICOLON                               { $$ = create_oper_node(';', 2, NULL, NULL); }                                                   
    |   expr SEMICOLON                          { $$ = $1; }                                                                            

    /* Declaration & Assignment */
    |   data_type IDENTIFIER SEMICOLON                                       { $$ = create_oper_node('d', 2, $1, create_identifier_node($2)); }                                 
    |   data_type IDENTIFIER ASSIGNMENT expr SEMICOLON                       { $$ = create_oper_node(ASSIGNMENT, 3, $1, create_identifier_node($2), $4); }                             
    |   CONST data_type IDENTIFIER ASSIGNMENT expr SEMICOLON                 { $$ = create_oper_node(ASSIGNMENT,4, create_type_node(typeConst),$2,create_identifier_node($3),$5); }                             
    |   IDENTIFIER ASSIGNMENT expr SEMICOLON                                 { $$ = create_oper_node(ASSIGNMENT, 2, create_identifier_node($1), $3); } 
    |   enum_statement                                                                                           

    /* Loop statment */
    |   WHILE '(' expr ')' statement                                                                              { $$ = create_oper_node(WHILE, 2, $3, $5); }
    |   DO statement WHILE '(' expr ')' SEMICOLON                                                                 { $$ = create_oper_node(DO, 2, $2, $5); }
    |   FOR '(' IDENTIFIER ASSIGNMENT expr SEMICOLON expr SEMICOLON IDENTIFIER ASSIGNMENT expr ')' statement      { $$ = create_oper_node(FOR,4,create_oper_node(ASSIGNMENT, 2, create_identifier_node($3), $5),$7,create_oper_node(ASSIGNMENT, 2, create_identifier_node($9), $11)    ,$13); }
                    
    /*  IF statment*/
    |   IF '(' expr ')' statement %prec IFX       { $$ = create_oper_node(IF, 2, $3, $5); }                                                
    |   IF '(' expr ')' statement ELSE statement  { $$ = create_oper_node(IF, 3, $3, $5, $7); }                                                      

    /* Switch statement */
    |   SWITCH '(' IDENTIFIER ')' '{' case_list case_default '}'       { $$ = create_oper_node(SWITCH,3,create_identifier_node($3),$6,$7); }                         
    |   BREAK SEMICOLON                                                { $$ = create_oper_node(BREAK,0); }

    /* Function Statement */
    |   data_type IDENTIFIER function_arguements_list '{' function_statement_list '}'            { $$ = create_oper_node(FUNCTION, 4, $1, create_identifier_node($2), $3, $5);}                           
    |   VOID IDENTIFIER function_arguements_list '{' statement_list '}'                          { $$ = create_oper_node(VOIDFUNCTION, 3, create_identifier_node($2), $3, $5);}                  
    |   VOID IDENTIFIER function_arguements_list '{' '}'                                         { $$ = create_oper_node(VOIDFUNCTION, 3, create_identifier_node($2), $3, NULL);}

    /* Block Statement */
    |   '{' statement_list '}'                               { $$ = create_oper_node('s', 1, $2); }                                      
    |   '{' '}'                                              { $$ = NULL; }                                 
    |   error SEMICOLON                                      { $$ = NULL; }                                     
    |   error '}'                                            { $$ = NULL; }                               
    ;


enum_arguments:
        enum_arguments COMMA IDENTIFIER     { $$ = create_oper_node(',', 2, $1, create_identifier_node($3)); } 
    |   IDENTIFIER                          { $$ = create_oper_node(';', 2, NULL, NULL);} // TODO: check this
    ;

enum_statement:
        ENUM '{' enum_arguments '}' IDENTIFIER SEMICOLON    { $$ = create_oper_node(ENUM, 2, $3, create_identifier_node($5)); } // TODO: check this
    ;

case_list:
        case_list CASE INT COLON statement_list      { $$ = create_oper_node(CASE,3,$1,create_int_node($3),$5); } 
    |   case_list CASE CHAR COLON statement_list     { $$ = create_oper_node(CASE,3,$1,create_char_node($3),$5); }     
    |   case_list CASE STRING COLON statement_list   { $$ = create_oper_node(CASE,3,$1,create_string_node($3),$5); }       
    |   case_list CASE BOOL COLON statement_list     { $$ = create_oper_node(CASE,3,$1,create_bool_node($3),$5);  }
    |  /* Empty statement */                         { $$ = NULL;  }
    ;


case_default: 
        DEFAULT COLON statement_list                       { $$ = create_oper_node(DEFAULT, 1, $3); };                                      
    ;

expr:
        INT                     { $$ = create_int_node($1); }                      
    |   FLOAT                   { $$ = create_float_node($1); }                                                               
    |   CHAR                    { $$ = create_char_node($1); }                                                               
    |   STRING                  { $$ = create_string_node($1); }                                                               
    |   BOOL                    { $$ = create_bool_node($1); }                                                       
    |   IDENTIFIER              { $$ = create_identifier_node($1); }                
    |   SUB expr %prec UMINUS   { $$ = create_oper_node(UMINUS, 1, $2); }                               
    |   NOT expr                { $$ = create_oper_node(NOT, 1, $2); }                                                               
    /* Mathematical */
    |   expr ADD expr           { $$ = create_oper_node(ADD, 2, $1, $3); }                                                                         
    |   expr SUB expr           { $$ = create_oper_node(SUB, 2, $1, $3); }                                                                     
    |   expr MUL expr           { $$ = create_oper_node(MUL, 2, $1, $3); }                                                                       
    |   expr DIV expr           { $$ = create_oper_node(DIV, 2, $1, $3); }                                                                       
    |   expr MOD expr           { $$ = create_oper_node(MOD, 2, $1, $3); }     

    /* Logical */
    |   expr LT expr            { $$ = create_oper_node(LT, 2, $1, $3); }                                                                     
    |   expr GT expr            { $$ = create_oper_node(GT, 2, $1, $3); }                                                                     
    |   expr GTE expr           { $$ = create_oper_node(GTE, 2, $1, $3); }                                                                     
    |   expr LTE expr           { $$ = create_oper_node(LTE, 2, $1, $3); }                                                                     
    |   expr NOT_EQUAL expr     { $$ = create_oper_node(NOT_EQUAL, 2, $1, $3); }                                                                        
    |   expr EQUAL_TO expr      { $$ = create_oper_node(EQUAL_TO, 2, $1, $3); }                                                                        
    |   expr AND expr           { $$ = create_oper_node(AND, 2, $1, $3); }                                                                    
    |   expr OR expr            { $$ = create_oper_node(OR, 2, $1, $3); }
    
    /* function call or grouped */
    |   IDENTIFIER function_call        { $$ = create_oper_node('t', 2, create_identifier_node($1), $2);}                                                       
    |   '(' expr ')'                    { $$ = $2; }                                                   
    ;


function_statement_list:
        RETURN expr SEMICOLON               { $$ = create_oper_node(RETURN, 1, $2); }                                                     
    |   statement function_statement_list   { $$ = create_oper_node(';', 2, $1, $2); }                                             
    ;

function_arguements:
        data_type IDENTIFIER                                  { $$ = create_oper_node('r', 2, $1, create_identifier_node($2)); }                                  
    |   data_type IDENTIFIER COMMA function_arguements        { $$ = create_oper_node(';', 3, $1, create_identifier_node($2), $4); }                                          
    ;

function_arguements_list:
        '(' function_arguements ')'    {$$ = $2;}                                                            
    |   '(' ')'                        {$$ = NULL;}                                                    
;

function_arguements_call:
        expr                                    { $$ = create_oper_node('q', 1, $1 ); }                                       
    |   function_arguements_call COMMA expr     { $$ = create_oper_node(':', 2, $1, $3); }                                                     
    ;

function_call:
        '(' function_arguements_call ')'        {$$ = $2;}                                                            
    |   '(' ')'                                 {$$ = NULL;}                                              
;


%%


void yyerror(char *s) {
    fprintf(yyout, "Error at line [%d]: %s\n", yylineno, s);
    fprintf(stdout,"Error at line [%d]: %s\n", yylineno, s);

}

int main(int argc, char* argv[]) {
    if(argc != 3){
        printf("wrong Number of arguments \n");
        exit(0);
    }
    // read the input file
    yyin = read_file(argv[1]);
    printf("File opened successfully\n");

    // check if out directory exists
    char * outDirPath = argv[2];
    DIR * dir = open_dir(outDirPath);

    // create the output files
    int strSize = strlen(argv[2]) * 100;
    char* errorFilePath = malloc(strSize);
    char* quadruplesPath = malloc(strSize);
    char* symbolTablePath = malloc(strSize);

    sprintf(errorFilePath ,  "%s/errors.txt", outDirPath);
    sprintf(quadruplesPath ,  "%s/quadruples.txt", outDirPath);
    sprintf(symbolTablePath ,  "%s/symbol_table.txt", argv[2]);

    yyout = create_file(errorFilePath);
    quadrables_file = create_file(quadruplesPath);
    setTablePath(symbolTablePath);
    setQuadrableFilePath(quadruplesPath, quadrables_file);

    printf("debug: %d\n", debug);
    printf("----------------- start -----------------\n");
    #if defined(YYDEBUG) && (YYDEBUG==1)
        yydebug = 0;
        debug = 0;
    #endif
    yyparse();
    printSymbolTable();
    fclose(yyin);
    fclose(yyout);
    clearTablePath();
    clearQuadrableFilePath();
    fclose(quadrables_file);
    printf("\n----------------- Parse End -----------------\n");
    return 0;
}
