%{
    // Libraies  
    #include <stdlib.h>
    #include<stdio.h>
    #include <stdarg.h> 
    #include <string.h>
    #include <stdbool.h> // for boolean

    // Tokens include
    #include "y.tab.h"
    #include"parser.h"

    // Function prototype
    void yyerror(char *);
    // #define debug_token(str) if(debug) printf("%s  %s\n", str, yytext); 

    // Global variables
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
"int"               {return INT_TYPE; }
"float"             {return FLOAT_TYPE; }
"string"            {return STRING_TYPE; }
"char"              {return CHAR_TYPE; }
"bool"              {return BOOL_TYPE;  }
"const"             {return CONST; }
"void"              {return VOID; }
"enum"              {return ENUM; }

    /* Reserved Keywords */
"if"                { return IF;}
"else"              { return ELSE;}

"switch"            { return SWITCH;}
"case"              { return CASE;}
"default"           { return DEFAULT;}

"while"             { return WHILE;}
"break"             { return BREAK;}

"for"               { return FOR;}
"do"                { return DO;}
"return"            { return RETURN;}
"print"             { return PRINT;}

    /* Mathematical Operators */
"="                 { return ASSIGNMENT;}
"+"                 { return ADD;}
"-"                 { return SUB;}
"*"                 { return MUL;}
"/"                 { return DIV;}
"%"                 { return MOD;}

    /* Logical Operators */
"&&"                { return AND;}
"||"                { return OR;}
"!"                 { return NOT;}

    /* Relational Operators */
"=="                { return EQUAL_TO;}
"!="                { return NOT_EQUAL; }
"<"                 { return LT;}
">"                 { return GT;}
"<="                { return LTE; }
">="                { return GTE;}


"true"              {  yylval.iValue = 1;  return BOOL;}
"false"             {  yylval.iValue = 0; return BOOL;}

    /* String Values */
{STRING_VALUE}      {
                        // copy the string value to yylval.sValue
                        yylval.sValue = strdup(yytext+1); // +1 to remove the first " || double quote

                        // remove the last double quote
                        int lastIdx = strlen(yylval.sValue) - 1;
                        yylval.sValue[lastIdx] = '\0';
                        
                        // return the token
                        return STRING;
                    }   

    /* Character Values */
{CHAR_VALUE}        {
                        // 1 to neglect single quotes
                        yylval.cValue = yytext[1];
                        return CHAR;
                    }

    /* Integer Values */
{INT_VALUE}         { 
                        yylval.iValue = atoi(yytext);
                        return INT;
                    }

    /* Floating Values */
{FLOAT_VALUE}       {
                        yylval.fValue = atof(yytext);
                        return FLOAT;
                    }

    /* Identifiers */
{IDENTIFIER}        {
                        yylval.sIndex = malloc(strlen(yytext));
                        strcpy(yylval.sIndex, yytext); 
                        return IDENTIFIER;
                    }

    /* Punctuators */
";"                 { return SEMICOLON;}
","                 { return COMMA; }
":"                 { return COLON; }

    /* special characters => mainly braces */
{SPECIAL_CHAR}      {return *yytext;}

    /* whitespace */
{WHITESPACE}          ;

    /* new line */
{NEWLINE}           { yylineno++; }

    /* comments */
{SINGLE_COMMENT}        ;
{MULTI_COMMENT}         ;


    /* others */
{OTHERS}            { 
                        // printf("Error: Unknown token %s\n", yytext);
                        // yyerror("Unknown token found");
                        // exit(1);
                    }

%%

/* return 1 if end of file is reached, else 0: int */
int yywrap(void) {
    return 1;
}
