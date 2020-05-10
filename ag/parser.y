%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "id_list.h"

#define YYERROR_VERBOSE 1

#define EXIT_DUPLICATE_ID_ERR(lexeme) \
    fprintf( \
        stderr, \
        "%d: scope error, %s already defined\n", \
        yylineno, \
        lexeme \
    ); \
    exit(3);

#define EXIT_UNDEFINED_ID_ERR(lexeme) \
    fprintf( \
        stderr, \
        "%d: scope error, %s undefined\n", \
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

id_list *err_list = NULL;

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

@attributes {
    char *lexeme;
} ID

@attributes {
    id_list *i_ids;
} stats

@attributes {
    id_list *i_ids, *s_ids;
} pars

@attributes {
    @autoinh
    id_list *i_ids;
    id_list *s_ids;
} stat lexpr term

@attributes {
    @autoinh
    id_list *i_ids;
} expr expr_unary expr_binary expr_add expr_mul expr_and expr_rel expr_list else

%start program

%%

program:
      /* empty */
    | program funcdef ';'
    ;

funcdef:
      ID '(' pars ')' stats END
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
        @i @stats.1.i_ids@ = @stat.0.s_ids@;
      @}
    ;

stat:
      RETURN expr
      @{
        @i @stat.s_ids@ = @stat.i_ids@;
      @}
    | IF expr THEN stats else END
      @{
        @i @stats.i_ids@ = @stat.i_ids@;
        @i @stat.s_ids@ = @stat.i_ids@;
      @}
    | ID ':' LOOP stats END             /* label neu */ /* sichtbarkeit innerhalb der schleife */
      @{
        @i @stats.i_ids@ = add_label(@stat.i_ids@, @ID.lexeme@);
        @i @stat.s_ids@ = @stat.i_ids@;
      @}
    | BREAK ID                          /* label bestehend */
      @{
        @i @stat.s_ids@ = check_label(@stat.i_ids@, @ID.lexeme@);
      @}
    | CONT ID                           /* label bestehend */
      @{
        @i @stat.s_ids@ = check_label(@stat.i_ids@, @ID.lexeme@);
      @}
    | VAR ID ASSIGN expr                /* name neu */  /* sichtbarkeit direkt folgende statements von stat */
      @{
        @i @stat.s_ids@ = add_name(@stat.i_ids@, @ID.lexeme@);
      @}
    | lexpr ASSIGN expr
      @{
        @i @stat.s_ids@ = @stat.i_ids@;
      @}
    | expr
      @{
        @i @stat.s_ids@ = @stat.i_ids@;
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
        @i @lexpr.s_ids@ = check_name(@lexpr.i_ids@, @ID.lexeme@);
      @}
     | '*' term
      @{
        @i @lexpr.s_ids@ = @lexpr.i_ids@;
      @}
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
      @{
        @i @term.s_ids@ = @term.i_ids@;
      @}
    | NUM
      @{
        @i @term.s_ids@ = @term.i_ids@;
      @}
    | ID                                /* name bestehend */
      @{
        @i @term.s_ids@ = check_name(@term.i_ids@, @ID.lexeme@);
      @}
    | ID '(' expr_list ')'              /* funktion beliebig */
      @{
        @i @term.s_ids@ = @term.i_ids@;
      @}
    ;

%%

id_list *check_name(id_list *list, char *lexeme) {
    if (contains_name(list, lexeme) == 0) {
        EXIT_UNDEFINED_ID_ERR(lexeme);
    }

    return list;
}

id_list *check_label(id_list *list, char *lexeme) {
    if (contains_label(list, lexeme) == 0) {
        EXIT_UNDEFINED_ID_ERR(lexeme);
    }

    return list;
}

id_list *add_name(id_list *list, char *lexeme) {
    id_list *succ;
    if (contains_id(list, lexeme) != 0) {
        EXIT_DUPLICATE_ID_ERR(lexeme);
    }

    if ((succ = add_id(list, lexeme, NAME)) == NULL) {
        EXIT_OOM_ERR();
    }
    
    return succ;
}

id_list *add_label(id_list *list, char *lexeme) {
    id_list *succ;
    if (contains_id(list, lexeme) != 0) {
        EXIT_DUPLICATE_ID_ERR(lexeme);
    }

    if ((succ = add_id(list, lexeme, LABEL)) == NULL) {
        EXIT_OOM_ERR();
    }
    
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
