%{
#include <stdlib.h>
#include <stdio.h>
#include "id_list.h"

#define YYERROR_VERBOSE 1

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
@attributes { id_list *i_ids; } stats
@attributes { id_list *s_ids, *i_ids; } stat
@attributes { id_list *i_ids; } else
@attributes { id_list *i_ids; } lexpr
@attributes { id_list *i_ids; } expr
@attributes { id_list *i_ids; } term
@attributes { id_list *i_ids; } expr_unary
@attributes { id_list *i_ids; } expr_binary
@attributes { id_list *i_ids; } expr_add
@attributes { id_list *i_ids; } expr_mul
@attributes { id_list *i_ids; } expr_and
@attributes { id_list *i_ids; } expr_rel
@attributes { id_list *i_ids; } expr_list

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
        @i @stats.i_ids@ = @pars.s_ids@;
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
        @i @pars.0.s_ids@ = @pars.1.s_ids@;
        @i @pars.1.i_ids@ = add_name(@pars.0.i_ids@, @ID.lexeme@);
      @}
    ;

stats:
      /* empty */
    | stat ';' stats
      @{
        @i @stat.i_ids@ = @stats.0.i_ids@;
        @i @stats.1.i_ids@ = @stat.0.s_ids@;
      @}
    ;

stat:
      RETURN expr
      @{
        @i @expr.i_ids@ = @stat.i_ids@;
      @}
    | IF expr THEN stats else END
      @{
        @i @expr.i_ids@ = @stat.i_ids@;
        @i @stats.i_ids@ = @stat.i_ids@;
        @i @else.i_ids@ = @stat.i_ids@;
      @}
    | ID ':' LOOP stats END             /* label neu */ /* sichtbarkeit innerhalb der schleife */
      @{
        @i @stats.i_ids@ = add_label(@stat.i_ids@, @ID.lexeme@);
      @}
    | BREAK ID                          /* label bestehend */
      @{
        @i (contains_label(@stat.i_ids@, @ID.lexeme@) != 0) || EXIT_SCOPE_ERR(@ID.lexeme@);
      @}
    | CONT ID                           /* label bestehend */
      @{
        @i (contains_label(@stat.i_ids@, @ID.lexeme@) != 0) || EXIT_SCOPE_ERR(@ID.lexeme@);
      @}
    | VAR ID ASSIGN expr                /* name neu */  /* sichtbarkeit direkt folgende statements von stats */
      @{
        @i @expr.i_ids@ = @stat.i_ids@;
        @i @stat.s_ids@ = add_name(@stat.i_ids@, @ID.lexeme@);
      @}
    | lexpr ASSIGN expr
      @{
        @i @lexpr.i_ids@ = @stat.i_ids@;
        @i @expr.i_ids@ = @stat.i_ids@;
      @}
    | expr
      @{
        @i @expr.i_ids@ = @stat.i_ids@;
      @}
    ;

else:
      /* empty */
    | ELSE stats
      @{
        @i @stats.i_ids@ = @else.i_ids@;
      @}
    ;

lexpr:
       ID                               /* name bestehend */
      @{
        @i (contains_name(@lexpr.i_ids@, @ID.lexeme@) != 0) || EXIT_SCOPE_ERR(@ID.lexeme@);
      @}
     | '*' term
      @{
        @i @term.i_ids@ = @lexpr.i_ids@;
      @}
     ;

expr:
      term
      @{
        @i @term.i_ids@ = @expr.i_ids@;
      @}
    | expr_unary
      @{
        @i @expr_unary.i_ids@ = @expr.i_ids@;
      @}
    | expr_binary
      @{
        @i @expr_binary.i_ids@ = @expr.i_ids@;
      @}
    ;

expr_unary:
      NOT expr_unary
      @{
        @i @expr_unary.1.i_ids@ = @expr_unary.0.i_ids@;
      @}
    | '-' expr_unary
      @{
        @i @expr_unary.1.i_ids@ = @expr_unary.0.i_ids@;
      @}
    | '*' expr_unary
      @{
        @i @expr_unary.1.i_ids@ = @expr_unary.0.i_ids@;
      @}
    | NOT term
      @{
        @i @term.i_ids@ = @expr_unary.i_ids@;
      @}
    | '-' term
      @{
        @i @term.i_ids@ = @expr_unary.i_ids@;
      @}
    | '*' term
      @{
        @i @term.i_ids@ = @expr_unary.i_ids@;
      @}
    ;

expr_binary:
      expr_add
      @{
        @i @expr_add.i_ids@ = @expr_binary.i_ids@;
      @}
    | expr_mul
      @{
        @i @expr_mul.i_ids@ = @expr_binary.i_ids@;
      @}
    | expr_and
      @{
        @i @expr_and.i_ids@ = @expr_binary.i_ids@;
      @}
    | expr_rel
      @{
        @i @expr_rel.i_ids@ = @expr_binary.i_ids@;
      @}
    ;

expr_add:
      expr_add '+' term
      @{
        @i @expr_add.1.i_ids@ = @expr_add.0.i_ids@;
        @i @term.i_ids@ = @expr_add.0.i_ids@;
      @}
    | term '+' term
      @{
        @i @term.0.i_ids@ = @expr_add.i_ids@;
        @i @term.1.i_ids@ = @expr_add.i_ids@;
      @}
    ;

expr_mul:
      expr_mul '*' term
      @{
        @i @expr_mul.1.i_ids@ = @expr_mul.0.i_ids@;
        @i @term.i_ids@ = @expr_mul.0.i_ids@;
      @}
    | term '*' term
      @{
        @i @term.0.i_ids@ = @expr_mul.i_ids@;
        @i @term.1.i_ids@ = @expr_mul.i_ids@;
      @}
    ;

expr_and:
      expr_and AND term
      @{
        @i @expr_and.1.i_ids@ = @expr_and.0.i_ids@;
        @i @term.i_ids@ = @expr_and.0.i_ids@;
      @}
    | term AND term
      @{
        @i @term.0.i_ids@ = @expr_and.i_ids@;
        @i @term.1.i_ids@ = @expr_and.i_ids@;
      @}
    ;

expr_rel:
      term LEQ term
      @{
        @i @term.0.i_ids@ = @expr_rel.i_ids@;
        @i @term.1.i_ids@ = @expr_rel.i_ids@;
      @}
    | term '#' term
      @{
        @i @term.0.i_ids@ = @expr_rel.i_ids@;
        @i @term.1.i_ids@ = @expr_rel.i_ids@;
      @}
    ;

expr_list:
      /* empty */
    | expr
      @{
        @i @expr.i_ids@ = @expr_list.i_ids@;
      @}
    | expr ',' expr_list
      @{
        @i @expr.i_ids@ = @expr_list.0.i_ids@;
        @i @expr_list.1.i_ids@ = @expr_list.0.i_ids@;
      @}
    ;

term:
      '(' expr ')'
      @{
        @i @expr.i_ids@ = @term.i_ids@;
      @}
    | NUM
    | ID                                /* name bestehend */
      @{
        @i (contains_name(@term.i_ids@, @ID.lexeme@) != 0) || EXIT_SCOPE_ERR(@ID.lexeme@);
      @}
    | ID '(' expr_list ')'              /* funktion beliebig */
      @{
        @expr_list.i_ids@ = @term.i_ids@;
      @}
    ;

%%

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
