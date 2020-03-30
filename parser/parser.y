//
%locations
%define api.pure full

%code top{
#include <stdlib.h>
#include <stdio.h>

#define YYERROR_VERBOSE 1
}

%code {
void yyerror(YYLTYPE *locp, char const *msg);
extern int yylex(YYSTYPE *lvalp, YYLTYPE *llocp);
}

%token ID
%token NUM
%token END
%token RETURN
%token IF
%token THEN
%token ELSE
%token LOOP
%token BREAK
%token CONT
%token VAR
%token NOT
%token AND
%token ASSIGN ":="
%token LEQ "<="

%start program

%%

program:
      /* empty */
    | program funcdef ';'
    ;

funcdef:
      ID '(' pars ')' stats END
    ;

pars:
      /* empty */
    | ID
    | ID ',' pars
    ;

stats:
      /* empty */
    | stat ';' stats
    ;

stat:
      RETURN expr
    | IF expr THEN stats else END
    | ID ':' LOOP stats END
    | BREAK ID
    | CONT ID
    | VAR ID ":=" expr
    | lexpr ":=" expr
    | expr
    ;

else:
      /* empty */
    | ELSE stats
    ;

lexpr:
       ID
     | '*' term
     ;

expr:
    | unary term
    | term expr_add
    | term expr_mul
    | term expr_and
    | term "<=" term
    | term '#' term
    ;
expr_add:
      /* empty */
    | '+' term expr_add
    ;
expr_mul:
      /* empty */
    | '*' term expr_mul
    ;
expr_and:
      /* empty */
    | AND term expr_and
    ;

unary:
      /* empty */
    | NOT unary
    | '-' unary
    | '*' unary
    ;

term:
      '(' expr ')'
    | NUM
    | ID
    | ID '(' expr_list ')'
    ;

expr_list:
      /* empty */
    | expr
    | expr ',' expr_list
    ;

%%

void yyerror(YYLTYPE *locp, char const *msg) {
    (void) fprintf(
        stderr,
        "%d:%d: %s\n",
        locp->first_line,
        locp->first_column,
        msg
    );
    exit(2);
}

int main(int argc, char *argv[]) {
    (void) yyparse();
    return 0;
}
