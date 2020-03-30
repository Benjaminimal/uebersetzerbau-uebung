 /**
  * scanner.flex
  * Description: TODO
  * Author: Benjamin Zinschitz (00940280)
  * Usage: TODO
  */

 /* C Definitions */
%{
#include "parser.tab.h"

#define YY_USER_ACTION num_chars += yyleng;
#define PRINTLN(...) fprintf(yyout, __VA_ARGS__);
#define PANIC() fprintf(stderr, "Lexical error:%d: Unexpected character '%s'\n", num_lines, yytext); exit(1);

int count_char(char *cursor, char needle);

int num_chars = 0;
int num_lines = 0;
%}

%option noyywrap

 /* Name Definitions */
ID              [a-zA-Z_][a-zA-Z0-9_]*
NUM             {DECNUM}|{HEXNUM}
DECNUM          [0-9]+
HEXNUM          0x[0-9a-fA-F]+
SPECIAL         \(|\)|,|;|:|\*|-|\+|#
COMMENT         \/\/.*$?
WHITESPACE      [ \n\t]+

%%

 /* Rules */
{COMMENT}       ;
{WHITESPACE}    num_lines += count_char(yytext, '\n');
"end"             return END;
"return"          return RETURN;
"if"              return IF;
"then"            return THEN;
"else"            return ELSE;
"loop"            return LOOP;
"break"           return BREAK;
"cont"            return CONT;
"var"             return VAR;
"not"             return NOT;
"and"             return AND;
":="              return ASSIGN;
"<="              return LEQ;
{SPECIAL}       return (int) yytext[0];
{ID}            return ID;
{NUM}           return NUM;
.               PANIC();

%%

 /* User Code */
int count_char(char *cursor, char needle) {
    int cnt = 0;
    while (strchr(cursor, needle) != NULL) {
        cnt++;
        cursor++;
    }
    return cnt;
}
