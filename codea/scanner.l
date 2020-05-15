 /**
  * scanner.l
  * Description: TODO
  * Author: Benjamin Zinschitz (00940280)
  * Usage: TODO
  */

 /* C Definitions */
%option yylineno

%{
#include "error.h"
#include "parser.h"
#include "sym_tab.h"
#include "treenode.h"

int yycolumn = 1;
%}

 /* Name Definitions */
ID              [a-zA-Z_][a-zA-Z0-9_]*
DECNUM          [0-9]+
HEXNUM          0x[0-9a-fA-F]+
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
"("             return LPAREN;
")"             return RPAREN;
","             return COMMA;
";"             return SEMICOLON;
":"             return COLON;
"*"             return ASTERISK;
"-"             return HYPHEN;
"+"             return PLUS;
"#"             return POUND;
{ID}            return ID; @{ @ID.lexeme@ = strdup(yytext); @}
{HEXNUM}        return NUM; @{ @NUM.value@ = strtoull(yytext, NULL, 16); @}
{DECNUM}        return NUM; @{ @NUM.value@ = strtoull(yytext, NULL, 10); @}
{WHITESPACE}    ;
{COMMENT}       ;
.               EXIT_ERR_LEX();
