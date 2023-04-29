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
"int"           {if(debug) printf("data_type %s\n", yytext); return INT_TYPE; }
"float"         {if(debug) printf("data_type %s\n", yytext); return FLOAT_TYPE; }
"string"        {if(debug) printf("data_type %s\n", yytext); return STRING_TYPE; }
"char"          {if(debug) printf("data_type %s\n", yytext); return CHAR_TYPE; }
"bool"          {if(debug) printf("data_type %s\n", yytext); return BOOL_TYPE;  }
"const"         {if(debug) printf("data_type %s\n", yytext); return CONST; }
"void"          {if(debug) printf("data_type %s\n", yytext); return VOID; }

    /* Reserved Keywords */
"if"            {if(debug)  printf("if\n"); return IF;}
"else"          {if(debug)  printf("else\n"); return ELSE;}

"switch"        {if(debug)  printf("switch\n"); return SWITCH;}
"case"          {if(debug)  printf("case\n"); return CASE;}
"default"       {if(debug)  printf("default\n"); return DEFAULT;}

"while"         {if(debug)  printf("while\n"); return WHILE;}
"break"         {if(debug)  printf("break\n"); return BREAK;}

"for"           {if(debug)  printf("for\n"); return FOR;}
"do"            {if(debug)  printf("do\n"); return DO;}
"return"        {if(debug)  printf("return\n"); return RETURN;}

    /* Mathematical Operators */
"="             {if(debug)  printf("assignment\n"); return ASSIGNMENT;}
"+"             {if(debug)  printf("addition\n"); return ADD;}
"-"             {if(debug)  printf("subtraction\n"); return SUB;}
"*"             {if(debug)  printf("multiplication\n"); return MUL;}
"/"             {if(debug)  printf("division\n"); return DIV;}
"%"             {if(debug)  printf("modulus\n"); return MOD;}

    /* Logical Operators */
"&&"            {if(debug)  printf("logical_and\n"); return AND;}
"||"            {if(debug)  printf("logical_or\n"); return OR;}
"!"             {if(debug)  printf("logical_not\n"); return NOT;}

    /* Relational Operators */
"=="            {if(debug)  printf("equal_to\n"); return EQUAL_TO;}
"!="            {if(debug)  printf("not_equal_to\n"); return NOT_EQUAL; }
"<"             {if(debug)  printf("less_than\n"); return LT;}
">"             {if(debug)  printf("greater_than\n"); return GT;}
"<="            {if(debug)  printf("less_than_equal_to\n"); return LTE; }
">="            {if(debug)  printf("greater_than_equal_to\n"); return GTE;}

    /* boolean values */
    /* "true"|"false" yylval=!strcmp(yytext,"true"); */
"true"          {if(debug)  printf("true\n");  yylval= 1;  return BOOL;}
"false"         {if(debug)  printf("false\n"); yylval= 0; return BOOL;}

    /* String Values */
{STRING_VALUE}    {if(debug)    printf("string %s\n", yytext); return STRING;}

    /* Character Values */
{CHAR_VALUE}    {if(debug)  printf("character %c\n", yytext[1]); return CHAR;}

    /* Integer Values */
{INT_VALUE}   {if(debug)    printf("integer %d\n", atoi(yytext)); return INT;}

    /* Floating Values */
{FLOAT_VALUE}  {if(debug)   printf("float %f\n", atof(yytext)); return FLOAT;}

    /* Identifiers */
{IDENTIFIER}  {if(debug)    printf("identifier %s\n", yytext); return IDENTIFIER;}

    /* Punctuators */
";"             {if(debug)  printf("semicolon\n"); return SEMICOLON;}
","             {if(debug)  printf("comma\n"); return COMMA; }
":"             {if(debug)  printf("colon\n"); return COLON; }

    /* special characters */
{SPECIAL_CHAR}         {if(debug)   printf("special_character %s\n", yytext);  return *yytext;}

    /* whitespace */
{WHITESPACE}          ;

    /* new line */
{NEWLINE}              { yy_lineno++; }

    /* comments */
{SINGLE_COMMENT}                  {if(debug) printf("comment %s\n", yytext); }
{MULTI_COMMENT}            {if(debug) printf("comment %s\n", yytext); }


    /* others */
{OTHERS}           yyerror("Unknown character");

%%

/* return 1 if end of file is reached, else 0: int */
int yywrap(void) {
    return 1;
}
