%{
#include <stdlib.h>
#include <stdio.h>
#include "id_list.h"

#define YYERROR_VERBOSE 1

void yyerror(char const *msg);
extern int yylex();
extern int yylineno;

id_list *add_name(id_list *, char *);
id_list *add_label(id_list *, char *);
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

@attributes { char *lexeme; } ID
@attributes { id_list *i_ids, *s_ids; } pars

%start program

%%

program:
      /* empty */
    | program funcdef ';'
    ;

funcdef:
      ID '(' pars ')' stats END         /* funktion neu */ /* kollision mit funktion oder name oder label */
      @{
        @i @pars.i_ids@ = empty_id_list();
      @}
    ;

pars:
      /* empty */
      @{
        @i @pars.s_ids@ = @pars.i_ids@;
      @}
    | ID                                /* name neu */  /* sichtbarkeit gesamte funktion */
      @{
        @i @pars.s_ids@ = add_name(@pars.i_ids@, @ID.lexeme@);
      @}
    | ID ',' pars                       /* name neu */  /* sichtbarkeit gesamte funktion */
      @{
        @i @pars.s_ids@ = @pars.1.s_ids@;
        @i @pars.1.i_ids@ = add_name(@pars.i_ids@, @ID.lexeme@);
      @}
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

#define EXIT_SCOPE_ERR(lexeme) \
    fprintf( \
        stderr, \
        "%d: scope error, %s already defined\n", \
        yylineno, \
        lexeme \
    ); \
    exit(3);

#define EXIT_OOM_ERR() \
    fprintf( \
        stderr, \
        "out of memory\n" \
    ); \
    exit(4);

id_list *add_name(id_list *list, char *lexeme) {
    id_list *succ;
    if (contains_id(list, lexeme) != 0)
        EXIT_SCOPE_ERR(lexeme);

    if ((succ = add_id(list, lexeme, NAME)) == NULL)
        EXIT_OOM_ERR();
    
    return succ;
}

id_list *add_label(id_list *list, char *lexeme) {
    id_list *succ;
    if (contains_id(list, lexeme) != 0)
        EXIT_SCOPE_ERR(lexeme);

    if ((succ = add_id(list, lexeme, LABEL)) == NULL)
        EXIT_OOM_ERR();
    
    return succ;
}

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
