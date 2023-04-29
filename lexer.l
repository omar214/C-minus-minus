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
    int debug = 1;
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
"int"           {if(debug) printf("data_type %s\n", yytext); }
"float"         {if(debug) printf("data_type %s\n", yytext); }
"string"        {if(debug) printf("data_type %s\n", yytext); }
"char"          {if(debug) printf("data_type %s\n", yytext); }
"bool"          {if(debug) printf("data_type %s\n", yytext); }
"const"         {if(debug) printf("data_type %s\n", yytext); }
"void"          {if(debug) printf("data_type %s\n", yytext); }

    /* Reserved Keywords */
"if"            {printf("if\n"); }
"else"          {printf("else\n"); }

"switch"        {printf("switch\n"); }
"case"          {printf("case\n"); }
"default"       {printf("default\n"); }

"while"         {printf("while\n"); }
"break"         {printf("break\n"); }

"for"           {printf("for\n"); }
"do"            {printf("do\n"); }
"return"        {printf("return\n"); }

    /* Mathematical Operators */
"="             {printf("assignment\n"); }
"+"             {printf("addition\n"); }
"-"             {printf("subtraction\n"); }
"*"             {printf("multiplication\n"); }
"/"             {printf("division\n"); }
"%"             {printf("modulus\n"); }

    /* Logical Operators */
"&&"            {printf("logical_and\n"); }
"||"            {printf("logical_or\n"); }
"!"             {printf("logical_not\n"); }

    /* Relational Operators */
"=="            {printf("equal_to\n"); }
"!="            {printf("not_equal_to\n"); }
"<"             {printf("less_than\n"); }
">"             {printf("greater_than\n"); }
"<="            {printf("less_than_equal_to\n"); }
">="            {printf("greater_than_equal_to\n"); }

    /* boolean values */
"true"          {printf("true\n"); }
"false"         {printf("false\n"); }

    /* String Values */
{STRING_VALUE}    {printf("string %s\n", yytext); }

    /* Character Values */
{CHAR_VALUE}    {printf("character %c\n", yytext[1]); }

    /* Integer Values */
{INT_VALUE}   {printf("integer %d\n", atoi(yytext)); }

    /* Floating Values */
{FLOAT_VALUE}  {printf("float %f\n", atof(yytext)); }

    /* Identifiers */
{IDENTIFIER}  {printf("identifier %s\n", yytext); }

    /* Punctuators */
";"             {printf("semicolon\n"); }
","             {printf("comma\n"); }
":"             {printf("colon\n"); }

    /* special characters */
{SPECIAL_CHAR}         {printf("special_character %s\n", yytext); }

    /* whitespace */
{WHITESPACE}          {; }

    /* new line */
{NEWLINE}              { yy_lineno++; }

    /* comments */
{SINGLE_COMMENT}                  {if(debug) printf("comment %s\n", yytext); }
{MULTI_COMMENT}            {if(debug) printf("comment %s\n", yytext); }


    /* others */
{OTHERS}           yyerror("Unknown character");

%%

int yywrap(void) {
    return 1;
}
