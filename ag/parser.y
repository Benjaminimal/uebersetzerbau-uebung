%{
#include <stdlib.h>
#include <stdio.h>

#define YYERROR_VERBOSE 1

void yyerror(char const *msg);
extern int yylex();
extern int yylineno;
%}

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
      ID '(' pars ')' stats END         /* funktion neu */ /* kollision mit funktion oder name oder label */
    ;

pars:
      /* empty */
    | ID                                /* name neu */  /* sichtbarkeit gesamte funktion */
    | ID ',' pars                       /* name neu */  /* sichtbarkeit gesamte funktion */
    ;

stats:
      /* empty */
    | stat ';' stats
    ;

stat:
      RETURN expr
    | IF expr THEN stats else END
    | ID ':' LOOP stats END             /* label neu */ /* sichtbarkeit innerhalb der schleife */
    | BREAK ID                          /* label bestehend */
    | CONT ID                           /* label bestehend */
    | VAR ID ASSIGN expr                /* name neu */  /* sichtbarkeit direkt folgende statements von stats */
    | lexpr ASSIGN expr
    | expr
    ;

else:
      /* empty */
    | ELSE stats
    ;

lexpr:
       ID                               /* name bestehend */
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
    | ID                                /* name bestehend */
    | ID '(' expr_list ')'              /* funktion beliebig */
    ;

%%

void yyerror(char const *msg) {
    (void) fprintf(
        stderr,
        "%d: %s\n",
        yylineno,
        msg
    );
    exit(2);
}

int main(int argc, char *argv[]) {
    (void) yyparse();
    return 0;
}
