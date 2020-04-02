 /**
  * scanner.l
  * Description: TODO
  * Author: Benjamin Zinschitz (00940280)
  * Usage: TODO
  */

 /* C Definitions */
%option yylineno
%option bison-locations

%{
#include "parser.tab.h"

int yycolumn = 1;

#define YY_USER_ACTION \
    yycolumn += yyleng; \
    yylloc->first_line = yylloc->last_line; \
    yylloc->last_line = yylineno; \
    yylloc->first_column = yylloc->last_column; \
    yylloc->last_column = yycolumn;

#define PANIC() \
    fprintf( \
        stderr, \
        "%d:%d: lexical error: unexpected character '%s'\n", \
        yylloc->first_line, \
        yylloc->first_column, \
        yytext \
    ); \
    exit(1);
%}

 /* Name Definitions */
ID              [a-zA-Z_][a-zA-Z0-9_]*
DECNUM          [0-9]+
HEXNUM          0x[0-9a-fA-F]+
SPECIAL         \(|\)|,|;|:|\*|-|\+|#
COMMENT         \/\/.*$?
WHITESPACE      [ \t]+

%%

 /* Rules */
\n+             yycolumn = 1;
{WHITESPACE}    ;
{COMMENT}       ;
"end"           return END;
"return"        return RETURN;
"if"            return IF;
"then"          return THEN;
"else"          return ELSE;
"loop"          return LOOP;
"break"         return BREAK;
"cont"          return CONT;
"var"           return VAR;
"not"           return NOT;
"and"           return AND;
":="            return ASSIGN;
"<="            return LEQ;
{SPECIAL}       return (int) yytext[0];
{ID}            return ID;
{HEXNUM}        return NUM;
{DECNUM}        return NUM;
.               PANIC();
