%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "sym_tab.h"
#include "ast.h"
#include "x86.h"

#define YYERROR_VERBOSE 1

void yyerror(char const *msg);
extern int yylex();
extern int yylineno;

extern void invoke_burm(NODEPTR_TYPE root);
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
%token LPAREN
%token RPAREN
%token COMMA
%token COLON
%token SEMICOLON
%token ASSIGN
%token LEQ
%token POUND
%token AND
%token PLUS
%token ASTERISK
%token HYPHEN
%token NOT

@attributes {
    char *lexeme;
} ID

@attributes {
    long value;
} NUM

@attributes {
    sym_tab *i_symtab, *s_symtab;
    int i_position;
    int s_position;
} pars

@attributes {
    @autoinh
    sym_tab *i_symtab;
    sym_tab *s_symtab;
    astnode *s_node;
    int i_position;
    int s_position;
} stat

@attributes {
    sym_tab *i_symtab;
    int i_position;
    int s_position;
    astnode *s_node;
} stats else_stats

@attributes {
    @autoinh
    sym_tab *i_symtab;
} expr_list

@attributes {
    @autoinh
    sym_tab *i_symtab;
    astnode *s_node;
} lexpr term expr expr_unary expr_binary expr_add expr_mul expr_and expr_rel

@traversal idcheck
@traversal @lefttoright @preorder codegen

%start program

%%

program:
      /* empty */
    | program funcdef SEMICOLON
    ;

funcdef:
      ID LPAREN pars RPAREN stats END
      @{
        @i @pars.i_symtab@ = empty_sym_tab();
        @i @pars.i_position@ = 0;
        @i @stats.i_symtab@ = @pars.s_symtab@;
        @i @stats.i_position@ = @pars.s_position@;
        @codegen function_start(@ID.lexeme@); invoke_burm(@stats.s_node@);
        @codegen @revorder (1) function_end(@ID.lexeme@);
      @}
    ;

pars:
      /* empty */
      @{
        @i @pars.s_symtab@ = @pars.i_symtab@;
        @i @pars.s_position@ = @pars.i_position@;
      @}
    | ID                                /* name neu */  /* sichtbarkeit gesamte funktion */
      @{
        @i @pars.s_symtab@ = add_parameter(@pars.i_symtab@, @ID.lexeme@, @pars.i_position@);
        @i @pars.s_position@ = @pars.i_position@ + 1;
      @}
    | ID COMMA pars                       /* name neu */  /* sichtbarkeit gesamte funktion */
      @{
        @i @pars.0.s_symtab@ = @pars.1.s_symtab@;
        @i @pars.1.i_symtab@ = add_parameter(@pars.0.i_symtab@, @ID.lexeme@, @pars.0.i_position@);
        @i @pars.1.i_position@ = @pars.0.i_position@ + 1;
        @i @pars.0.s_position@ = @pars.1.s_position@;
      @}
    ;

stats:
      /* empty */
      @{
        @i @stats.s_position@ = @stats.i_position@;
        @i @stats.s_node@ = new_nop_node();
      @}
    | stat SEMICOLON stats
      @{
        @i @stats.1.i_symtab@ = @stat.0.s_symtab@;
        @i @stat.i_position@ = @stats.0.i_position@;
        @i @stats.1.i_position@ = @stat.s_position@;
        @i @stats.0.s_position@ = @stats.1.s_position@;
        @i @stats.0.s_node@ = new_sequence_node(@stat.s_node@, @stats.1.s_node@);
      @}
    ;

stat:
      RETURN expr
      @{
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_position@ = @stat.i_position@;
        @i @stat.s_node@ = new_return_node(@expr.s_node@);
      @}
    | IF expr THEN stats else_stats END
      @{
        @i @stats.i_symtab@ = @stat.i_symtab@;
        @i @else_stats.i_symtab@ = @stat.i_symtab@;
        @i @stats.i_position@ = @stat.i_position@;
        @i @else_stats.i_position@ = @stats.s_position@;
        @i @stat.s_position@ = @else_stats.s_position@;
        @i @stat.s_symtab@ = @stat.i_symtab@; // TODO: what if there is a variable definition in else_stats?

        @i @stat.s_node@ = new_if_node(@expr.s_node@, @stats.s_node@, @else_stats.s_node@, next_label(), next_label());
      @}
    | ID COLON LOOP stats END             /* label neu */ /* sichtbarkeit innerhalb der schleife */
      @{
        @i @stats.i_symtab@ = add_label(@stat.i_symtab@, @ID.lexeme@, next_label(), next_label());
        @i @stats.i_position@ = @stat.i_position@;
        @i @stat.s_position@ = @stats.s_position@;
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_node@ = new_loop_node(get_label(@stats.i_symtab@, @ID.lexeme@), @stats.s_node@);
      @}
    | BREAK ID                          /* label bestehend */
      @{
        @idcheck check_label(@stat.i_symtab@, @ID.lexeme@);
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_position@ = @stat.i_position@;
        @i @stat.s_node@ = new_break_node(get_label(@stat.i_symtab@, @ID.lexeme@));
      @}
    | CONT ID                           /* label bestehend */
      @{
        @idcheck check_label(@stat.i_symtab@, @ID.lexeme@);
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_position@ = @stat.i_position@;

        @i @stat.s_node@ = new_cont_node(get_label(@stat.i_symtab@, @ID.lexeme@));
      @}
    | VAR ID ASSIGN expr                /* name neu */  /* sichtbarkeit direkt folgende statements von stat */
      @{
        @i @stat.s_symtab@ = add_name(@stat.i_symtab@, @ID.lexeme@, @stat.i_position@);
        @i @stat.s_position@ = @stat.i_position@ + 1;
        @i @stat.s_node@ = new_init_node(get_name(@stat.s_symtab@, @ID.lexeme@), @expr.s_node@);
      @}
    | lexpr ASSIGN expr
      @{
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_position@ = @stat.i_position@;
        @i @stat.s_node@ = new_assign_node(@expr.s_node@, @lexpr.s_node@);
      @}
    | expr
      @{
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_position@ = @stat.i_position@;
        @i @stat.s_node@ = new_nop_node();
      @}
    ;

else_stats:
      /* empty */
      @{
        @i @else_stats.s_position@ = @else_stats.i_position@;
        @i @else_stats.s_node@ = new_nop_node();
      @}
    | ELSE stats
      @{
        @i @stats.i_symtab@ = @else_stats.i_symtab@;
        @i @stats.i_position@ = @else_stats.i_position@;
        @i @else_stats.s_position@ = @stats.s_position@;
        @i @else_stats.s_node@ = @stats.s_node@;
      @}
    ;

lexpr:
       ID                               /* name bestehend */
      @{
        @i @lexpr.s_node@ = new_variable_node(get_name(@lexpr.i_symtab@, @ID.lexeme@));
        @idcheck check_name(@lexpr.i_symtab@, @ID.lexeme@);
      @}
     | ASTERISK term
      @{
        @i @lexpr.s_node@ = new_dereference_node(@term.s_node@);
      @}
     ;

expr:
      term
      @{
        @i @expr.s_node@ = @term.s_node@;
      @}
    | expr_unary
      @{
        @i @expr.s_node@ = @expr_unary.s_node@;
      @}
    | expr_binary
      @{
        @i @expr.s_node@ = @expr_binary.s_node@;
      @}
    ;

expr_unary:
      NOT expr_unary
      @{
        @i @expr_unary.0.s_node@ = new_not_node(@expr_unary.1.s_node@);
      @}
    | HYPHEN expr_unary
      @{
        @i @expr_unary.0.s_node@ = new_negate_node(@expr_unary.1.s_node@);
      @}
    | ASTERISK expr_unary
      @{
        @i @expr_unary.0.s_node@ = new_dereference_node(@expr_unary.1.s_node@);
      @}
    | NOT term
      @{
        @i @expr_unary.s_node@ = new_not_node(@term.s_node@);
      @}
    | HYPHEN term
      @{
        @i @expr_unary.s_node@ = new_negate_node(@term.s_node@);
      @}
    | ASTERISK term
      @{
        @i @expr_unary.s_node@ = new_dereference_node(@term.s_node@);
      @}
    ;

expr_binary:
      expr_add
      @{
        @i @expr_binary.s_node@ = @expr_add.s_node@;
      @}
    | expr_mul
      @{
        @i @expr_binary.s_node@ = @expr_mul.s_node@;
      @}
    | expr_and
      @{
        @i @expr_binary.s_node@ = @expr_and.s_node@;
      @}
    | expr_rel
      @{
        @i @expr_binary.s_node@ = @expr_rel.s_node@;
      @}
    ;

expr_add:
      expr_add PLUS term
      @{
        @i @expr_add.0.s_node@ = new_add_node(@expr_add.1.s_node@, @term.s_node@);
      @}
    | term PLUS term
      @{
        @i @expr_add.s_node@ = new_add_node(@term.0.s_node@, @term.1.s_node@);
      @}
    ;

expr_mul:
      expr_mul ASTERISK term
      @{
        @i @expr_mul.0.s_node@ = new_multiply_node(@expr_mul.1.s_node@, @term.s_node@);
      @}
    | term ASTERISK term
      @{
        @i @expr_mul.s_node@ = new_multiply_node(@term.0.s_node@, @term.1.s_node@);
      @}
    ;

expr_and:
      expr_and AND term
      @{
        @i @expr_and.0.s_node@ = new_and_node(@expr_and.1.s_node@, @term.s_node@);
      @}
    | term AND term
      @{
        @i @expr_and.s_node@ = new_and_node(@term.0.s_node@, @term.1.s_node@);
      @}
    ;

expr_rel:
      term LEQ term
      @{
        @i @expr_rel.s_node@ = new_compare_less_or_equal_node(@term.0.s_node@, @term.1.s_node@);
      @}
    | term POUND term
      @{
        @i @expr_rel.s_node@ = new_compare_not_equal_node(@term.0.s_node@, @term.1.s_node@);
      @}
    ;

expr_list:
      /* empty */
    | expr
    | expr COMMA expr_list
    ;

term:
      LPAREN expr RPAREN
      @{
        @i @term.s_node@ = @expr.s_node@;
      @}
    | NUM
      @{
        @i @term.s_node@ = new_constant_node(@NUM.value@);
      @}
    | ID                                /* name bestehend */
      @{
        @idcheck check_name(@term.i_symtab@, @ID.lexeme@);
        @i @term.s_node@ = new_variable_node(get_name(@term.i_symtab@, @ID.lexeme@));
      @}
    | ID LPAREN expr_list RPAREN        /* funktion beliebig */
      @{
        @i @term.s_node@ = new_nop_node();
      @}
    ;

%%

sym_tab *check_name(sym_tab *tab, char *lexeme) {
#if DEBUG
    print_sym_tab(tab);
#endif
    if (contains_name(tab, lexeme) == 0) {
        EXIT_ERR_UNDEFINED_ID(lexeme);
    }

    return tab;
}

sym_tab *check_label(sym_tab *tab, char *lexeme) {
#if DEBUG
    print_sym_tab(tab);
#endif
    if (contains_label(tab, lexeme) == 0) {
        EXIT_ERR_UNDEFINED_ID(lexeme);
    }

    return tab;
}

sym_tab *add_parameter(sym_tab *tab, char *lexeme, int position) {
    sym_tab *succ;
    if (contains_id(tab, lexeme) != 0) {
        EXIT_ERR_DUPLICATE_ID(lexeme);
    }

    if ((succ = add_id(tab, lexeme, NAME, position)) == NULL) {
        EXIT_ERR_OOM();
    }
    
    return succ;
}

sym_tab *add_name(sym_tab *tab, char *lexeme, int position) {
    sym_tab *succ;
    if (contains_id(tab, lexeme) != 0) {
        EXIT_ERR_DUPLICATE_ID(lexeme);
    }

    if ((succ = add_id(tab, lexeme, NAME, position)) == NULL) {
        EXIT_ERR_OOM();
    }
    
    return succ;
}

sym_tab *add_label(sym_tab *tab, char *lexeme, char *start, char *end) {
    sym_tab *succ;
    if (contains_id(tab, lexeme) != 0) {
        EXIT_ERR_DUPLICATE_ID(lexeme);
    }

    if ((succ = add_id(tab, lexeme, LABEL, -1)) == NULL) {
        EXIT_ERR_OOM();
    }
    succ->labels[0] = start;
    succ->labels[1] = end;
    
    return succ;
}

void yyerror(char const *msg) {
    EXIT_ERR_PARSE(msg);
}

int main(int argc, char *argv[]) {
    (void) yyparse();
    return 0;
}
