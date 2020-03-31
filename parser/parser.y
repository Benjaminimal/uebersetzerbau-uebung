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
%token ASSIGN
%token LEQ

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
    | VAR ID ASSIGN expr
    | lexpr ASSIGN expr
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
      term
    | expr_unary
    | expr_binary
    ;

expr_unary:
      NOT expr_unary
    | '-' expr_unary
    | '*' expr_unary
    | NOT term
    | '-' term
    | '*' term
    ;

expr_binary:
      expr_add
    | expr_mul
    | expr_and
    | expr_rel
    ;

expr_add:
      expr_add '+' term
    | term '+' term
    ;

expr_mul:
      expr_mul '*' term
    | term '*' term
    ;

expr_and:
      expr_and AND term
    | term AND term
    ;

expr_rel:
      term LEQ term
    | term '#' term
    ;

expr_list:
      /* empty */
    | expr
    | expr ',' expr_list
    ;

term:
      '(' expr ')'
    | NUM
    | ID
    | ID '(' expr_list ')'
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
