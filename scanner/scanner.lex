 /**
  * scanner.flex
  * Description: TODO
  * Author: Benjamin Zinschitz (00940280)
  * Usage: TODO
  */

 /* C Definitions */
%{
#define YY_USER_ACTION num_chars += yyleng;
#define PRINTLN(...) fprintf(yyout, __VA_ARGS__);

int count_char(char *cursor, char needle);

int num_chars = 0;
int num_lines = 0;
int num_column = 0;
%}

 /* Name Definitions */
ID              [a-zA-Z_][a-zA-Z0-9_]*
NUM             {DECNUM}|{HEXNUM}
DECNUM          [0-9]+
HEXNUM          0x[0-9a-fA-F]+
KEYWORD         end|return|if|then|else|loop|break
SPECIAL         \(|\)|,|;|:|:=|\*|-|\+|<=|#
COMMENT         \/\/.*$
WHITESPACE      [ \n\t]+

%%

 /* Rules */
{COMMENT}
{WHITESPACE}    num_lines += count_char(yytext, '\n');
{KEYWORD}       PRINTLN("%s\n", yytext);
{SPECIAL}       PRINTLN("%s\n", yytext);
{ID}            PRINTLN("id %s\n", yytext);
{NUM}           PRINTLN("num %ld\n", strtol(yytext, NULL, 0));
.               return -1;

%%

 /* User Code */
int main(int argc, char *argv[]) {
    if (yylex() != 0) {
        (void) fprintf(stderr, "%s:%d: unexpected character '%s'\n", argv[0], num_lines, yytext);
        exit(1);
    }
    exit(0);
}

int count_char(char *cursor, char needle) {
    int cnt = 0;
    while (strchr(cursor, needle) != NULL) {
        cnt++;
        cursor++;
    }
    return cnt;
}
