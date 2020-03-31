 /**
  * scanner.l
  * Author: Benjamin Zinschitz (00940280)
  */

 /* C Definitions */
%{
#define YY_USER_ACTION num_column += yyleng;
#define PRINTLN(...) fprintf(yyout, __VA_ARGS__);
#define PANIC() \
    fprintf( \
        stderr, \
        "%d:%d: lexical error: unexpected character '%s'\n", \
        num_lines, \
        num_column, \
        yytext \
    ); \
    exit(1);

int num_chars = 0;
int num_lines = 1;
int num_column = 0;
%}

 /* Name Definitions */
ID              [a-zA-Z_][a-zA-Z0-9_]*
NUM             {DECNUM}|{HEXNUM}
DECNUM          [0-9]+
HEXNUM          0x[0-9a-fA-F]+
KEYWORD         end|return|if|then|else|loop|break|cont|var|not|and
SPECIAL         \(|\)|,|;|:|:=|\*|-|\+|<=|#
COMMENT         \/\/.*$?
WHITESPACE      [ \t]+

%%

 /* Rules */
\n+                     num_lines += yyleng; num_column = 0;
{COMMENT}               ;
{WHITESPACE}            ;
{KEYWORD}|{SPECIAL}     PRINTLN("%s\n", yytext);
{ID}                    PRINTLN("id %s\n", yytext);
{NUM}                   PRINTLN("num %llu\n", strtoull(yytext, NULL, 0));
.                       PANIC();

%%

 /* User Code */
int main(int argc, char *argv[]) {
    (void) yylex();
    exit(0);
}
