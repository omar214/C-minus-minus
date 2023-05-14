%{
    // Libraies  
    #include <stdlib.h>
    #include<stdio.h>
    #include <stdarg.h> 
    #include <string.h>

    // Tokens include
    #include "y.tab.h"

    // Function prototype
    void yyerror(char *);
    // #define debug_token(str) if(debug) printf("%s  %s\n", str, yytext); 

    // Global variables
    int yy_lineno = 1;
    int debug = 0;
%}

%option yylineno

STRING_VALUE    \"[^\n\"]*\"
CHAR_VALUE      \'[^\n\']\'
INT_VALUE       0|[-]?[1-9][0-9]*
FLOAT_VALUE     [-]?[0-9]*\.[0-9]+
IDENTIFIER      [a-zA-Z_][a-zA-Z0-9_]*
SPECIAL_CHAR    [(){}]
PUNCTUATORS     [;,:]
WHITESPACE      [ \t]+
NEWLINE         \n
SINGLE_COMMENT  \/\/[^\n]*
MULTI_COMMENT   \/\*([^\*]|\*[^\/])*\*\/
OTHERS          .


%%
    /* Reserved Data Types */
"int"           {return INT_TYPE; }
"float"         {return FLOAT_TYPE; }
"string"        {return STRING_TYPE; }
"char"          {return CHAR_TYPE; }
"bool"          {return BOOL_TYPE;  }
"const"         {return CONST; }
"void"          {return VOID; }
"enum"          {return ENUM; }

    /* Reserved Keywords */
"if"            { return IF;}
"else"          { return ELSE;}

"switch"        { return SWITCH;}
"case"          { return CASE;}
"default"       { return DEFAULT;}

"while"         { return WHILE;}
"break"         { return BREAK;}

"for"           { return FOR;}
"do"            { return DO;}
"return"        { return RETURN;}

    /* Mathematical Operators */
"="             { return ASSIGNMENT;}
"+"             { return ADD;}
"-"             { return SUB;}
"*"             { return MUL;}
"/"             { return DIV;}
"%"             { return MOD;}

    /* Logical Operators */
"&&"            { return AND;}
"||"            { return OR;}
"!"             { return NOT;}

    /* Relational Operators */
"=="            { return EQUAL_TO;}
"!="            { return NOT_EQUAL; }
"<"             { return LT;}
">"             { return GT;}
"<="            { return LTE; }
">="            { return GTE;}


"true"          {  yylval= 1;  return BOOL;}
"false"         { yylval= 0; return BOOL;}

    /* String Values */
{STRING_VALUE}    {return STRING;}

    /* Character Values */
{CHAR_VALUE}    {return CHAR;}

    /* Integer Values */
{INT_VALUE}   { return INT;}

    /* Floating Values */
{FLOAT_VALUE}  {return FLOAT;}

    /* Identifiers */
{IDENTIFIER}  {return IDENTIFIER;}

    /* Punctuators */
";"             { return SEMICOLON;}
","             { return COMMA; }
":"             { return COLON; }

    /* special characters */
{SPECIAL_CHAR}         {return *yytext;}

    /* whitespace */
{WHITESPACE}          ;

    /* new line */
{NEWLINE}              { yy_lineno++; }

    /* comments */
{SINGLE_COMMENT}        ;
{MULTI_COMMENT}         ;


    /* others */
{OTHERS}            { 
                        printf("Error: Unknown token %s\n", yytext);
                        yyerror("Unknown token found");
                        // exit(1);
                    }

%%

/* return 1 if end of file is reached, else 0: int */
int yywrap(void) {
    return 1;
}
