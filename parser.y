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

// routines
nodeType *opr(int oper, int nops, ...);     // for operations
nodeType *id(char* id);                     // for identifiers
nodeType *typ(conEnum value);               // for types defining
nodeType *con();
nodeType *conInt(int value);                // to add integer value
nodeType *conFloat(float value);            // to add float value
nodeType *conBool(bool value);
nodeType *conChar(char value);
nodeType *conString(char* value);
void freeNode(nodeType *p);

int execute(nodeType *p, FILE * outFile);   // to execute the program code
FILE* assembly;                             // the write the assembly in
extern FILE* yyin;                          // the input file
extern FILE* yyout;                         // output file to save the errors 


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
        program statement
    | /* Empty Statement */ { $$ = NULL; }
    ;

statement_list:
        statement                               { $$ = $1; }             
    |   statement_list statement                { $$ = opr(';', 2, $1, $2); }                  
    ;

data_type: 
        INT_TYPE            { $$ =  typ(typeInt); } 
    |   FLOAT_TYPE          { $$ =  typ(typeFloat); }
    |   CHAR_TYPE           { $$ =  typ(typeChar); }
    |   BOOL_TYPE           { $$ =  typ(typeBool); }
    |   STRING_TYPE         { $$ =  typ(typeString); }
    ;
    

statement:
        SEMICOLON                               { $$ = opr(';', 2, NULL, NULL); }                                                   
    |   expr SEMICOLON                          { $$ = $1; }                                                                            

    /* Declaration & Assignment */
    |   data_type IDENTIFIER SEMICOLON                                       { $$ = opr('d', 2, $1, id($2)); }                                 
    |   data_type IDENTIFIER ASSIGNMENT expr SEMICOLON                       { $$ = opr(ASSIGNMENT, 3, $1, id($2), $4); }                             
    |   CONST data_type IDENTIFIER ASSIGNMENT expr SEMICOLON                 { $$ = opr(ASSIGNMENT,4,typ(typeConst),$2,id($3),$5); }                             
    |   IDENTIFIER ASSIGNMENT expr SEMICOLON                                 { $$ = opr(ASSIGNMENT, 2, id($1), $3); } 
    |   enum_statement                                                                                           

    /* Loop statment */
    |   WHILE '(' expr ')' statement                                                                              { $$ = opr(WHILE, 2, $3, $5); }
    |   DO statement WHILE '(' expr ')' SEMICOLON                                                                 { $$ = opr(DO, 2, $2, $5); }
    |   FOR '(' IDENTIFIER ASSIGNMENT expr SEMICOLON expr SEMICOLON IDENTIFIER ASSIGNMENT expr ')' statement      { $$ = opr(FOR,4,opr(ASSIGNMENT, 2, id($3), $5),$7,opr(ASSIGNMENT, 2, id($9), $11)    ,$13); }
                    
    /*  IF statment*/
    |   IF '(' expr ')' statement %prec IFX       { $$ = opr(IF, 2, $3, $5); }                                                
    |   IF '(' expr ')' statement ELSE statement  { $$ = opr(IF, 3, $3, $5, $7); }                                                      

    /* Switch statement */
    |   SWITCH '(' IDENTIFIER ')' '{' case_list case_default '}'       { $$ = opr(SWITCH,3,id($3),$6,$7); }                         
    |   BREAK SEMICOLON                                                { $$ = opr(BREAK,0); }

    /* Function Statement */
    |   data_type IDENTIFIER function_arguements_list '{' function_statement_list '}'            { $$ = opr(FUNCTION, 4, $1, id($2), $3, $5);}                           
    |   VOID IDENTIFIER function_arguements_list '{' statement_list '}'                          { $$ = opr(VOIDFUNCTION, 3, id($2), $3, $5);}                  
    |   VOID IDENTIFIER function_arguements_list '{' '}'                                         { $$ = opr(VOIDFUNCTION, 3, id($2), $3, NULL);}

    /* Block Statement */
    |   '{' statement_list '}'                               { $$ = opr('s', 1, $2); }                                      
    |   '{' '}'                                              { $$ = NULL; }                                 
    |   error SEMICOLON                                      { $$ = NULL; }                                     
    |   error '}'                                            { $$ = NULL; }                               
    ;


enum_arguments:
        enum_arguments COMMA IDENTIFIER     { $$ = opr(',', 2, $1, id($3)); } 
    |   IDENTIFIER                          { $$ = opr(';', 2, NULL, NULL);} // TODO: check this
    ;

enum_statement:
        ENUM '{' enum_arguments '}' IDENTIFIER SEMICOLON    { $$ = opr(ENUM, 2, $3, id($5)); } // TODO: check this
    ;

case_list:
        case_list CASE INT COLON statement_list      { $$ = opr(CASE,3,$1,conInt($3),$5); } 
    |   case_list CASE CHAR COLON statement_list     { $$ = opr(CASE,3,$1,conChar($3),$5); }     
    |   case_list CASE STRING COLON statement_list   { $$ = opr(CASE,3,$1,conString($3),$5); }       
    |   case_list CASE BOOL COLON statement_list     { $$ = opr(CASE,3,$1,conBool($3),$5);  }
    |  /* Empty statement */                         { $$ = NULL;  }
    ;


case_default: 
        DEFAULT COLON statement_list                       { $$ = opr(DEFAULT, 1, $3); };                                      
    ;

expr:
        INT                     { $$ = conInt($1); }                      
    |   FLOAT                   { $$ = conFloat($1); }                                                               
    |   CHAR                    { $$ = conChar($1); }                                                               
    |   STRING                  { $$ = conString($1); }                                                               
    |   BOOL                    { $$ = conBool($1); }                                                       
    |   IDENTIFIER              { $$ = id($1); }                
    |   SUB expr %prec UMINUS   { $$ = opr(UMINUS, 1, $2); }                               
    |   NOT expr                { $$ = opr(NOT, 1, $2); }                                                               
    /* Mathematical */
    |   expr ADD expr           { $$ = opr(ADD, 2, $1, $3); }                                                                         
    |   expr SUB expr           { $$ = opr(SUB, 2, $1, $3); }                                                                     
    |   expr MUL expr           { $$ = opr(MUL, 2, $1, $3); }                                                                       
    |   expr DIV expr           { $$ = opr(DIV, 2, $1, $3); }                                                                       
    |   expr MOD expr           { $$ = opr(MOD, 2, $1, $3); }     

    /* Logical */
    |   expr LT expr            { $$ = opr(LT, 2, $1, $3); }                                                                     
    |   expr GT expr            { $$ = opr(GT, 2, $1, $3); }                                                                     
    |   expr GTE expr           { $$ = opr(GTE, 2, $1, $3); }                                                                     
    |   expr LTE expr           { $$ = opr(LTE, 2, $1, $3); }                                                                     
    |   expr NOT_EQUAL expr     { $$ = opr(NOT_EQUAL, 2, $1, $3); }                                                                        
    |   expr EQUAL_TO expr      { $$ = opr(EQUAL_TO, 2, $1, $3); }                                                                        
    |   expr AND expr           { $$ = opr(AND, 2, $1, $3); }                                                                    
    |   expr OR expr            { $$ = opr(OR, 2, $1, $3); }
    
    /* function call or grouped */                                                                        /*  */
    |   IDENTIFIER function_call        { $$ = opr('t', 2, id($1), $2);}                                                       
    |   '(' expr ')'                    { $$ = $2; }                                                   
    ;


function_statement_list:
        RETURN expr SEMICOLON               { $$ = opr(RETURN, 1, $2); }                                                     
    |   statement function_statement_list   { $$ = opr(';', 2, $1, $2); }                                             
    ;

function_arguements:
        data_type IDENTIFIER                                  { $$ = opr('r', 2, $1, id($2)); }                                  
    |   data_type IDENTIFIER COMMA function_arguements        { $$ = opr(';', 3, $1, id($2), $4); }                                          
    ;

function_arguements_list:
        '(' function_arguements ')'    {$$ = $2;}                                                            
    |   '(' ')'                        {$$ = NULL;}                                                    
;

function_arguements_call:
        expr                                    { $$ = opr('q', 1, $1 ); }                                       
    |   function_arguements_call COMMA expr     { $$ = opr(':', 2, $1, $3); }                                                     
    ;

function_call:
        '(' function_arguements_call ')'        {$$ = $2;}                                                            
    |   '(' ')'                                 {$$ = NULL;}                                              
;


%%


nodeType *typ(conEnum type){
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeDef;
    p->typ.type = type;

    return p;
}

nodeType *con(){
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    return p;
}

nodeType *conInt(int value) {
    nodeType *p = con();
    p->con.type = typeInt;
    p->con.iValue = value;
    
    return p;
}

nodeType *conFloat(float value) {
    nodeType *p = con();

    p->con.type = typeFloat;
    p->con.fValue = value;

    return p;
}

nodeType *conBool(bool value) {
    nodeType *p = con();

    p->con.type = typeBool;
    p->con.iValue = value;

    return p;
}


nodeType *conChar(char value) {
    nodeType *p = con();

    p->con.type = typeChar;
    p->con.cValue = value;

    return p;
}

nodeType *conString(char* value) {
    nodeType *p = con();

    p->con.type = typeString;
    p->con.sValue = value;

    return p;
}


nodeType *id(char* id) {
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.id = id;

    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    int i;

    /* allocate node, extending op array */
    if ((p = malloc(sizeof(nodeType) + (nops-1) * sizeof(nodeType *))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}


void yyerror(char *s) {
    printf("\n----- Error -----\n");
    fprintf(stderr, "%s Error at line [%d]\n", s , yylineno);
    /* fprintf(yyout, "line [%d]: %s\n", yylineno, s);
    fprintf(stdout, "line [%d]: %s\n", yylineno, s);
     */
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
