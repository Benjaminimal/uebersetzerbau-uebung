 /**
  * scanner.l
  * Description: TODO
  * Author: Benjamin Zinschitz (00940280)
  * Usage: TODO
  */

 /* C Definitions */
%option yylineno

%{
#include "parser.h"

int yycolumn = 1;

#define EXIT_LEX_ERR() \
    fprintf( \
        stderr, \
        "%d: lexical error: unexpected character '%s'\n", \
        yylineno, \
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
WHITESPACE      [ \n\t]+

%%

 /* Rules */
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
{WHITESPACE}    ;
{COMMENT}       ;
.               EXIT_LEX_ERR();
