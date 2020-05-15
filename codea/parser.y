%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "sym_tab.h"
#include "treenode.h"
#include "translator.h"

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
    long long value;
} NUM

@attributes {
    sym_tab *i_symtab, *s_symtab;
    int i_position;
} pars

@attributes {
    @autoinh
    sym_tab *i_symtab;
    sym_tab *s_symtab;
    treenode *s_n;
} stat

@attributes {
    sym_tab *i_symtab;
} stats

@attributes {
    @autoinh
    sym_tab *i_symtab;
} lexpr expr_list else_stats

@attributes {
    @autoinh
    sym_tab *i_symtab;
    treenode *s_n;
} term expr expr_unary expr_binary expr_add expr_mul expr_and expr_rel

@traversal idcheck
@traversal @lefttoright @postorder codegen

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
        @codegen @revorder (1) function_start(@ID.lexeme@);
        @codegen function_end(@ID.lexeme@);
      @}
    ;

pars:
      /* empty */
      @{
        @i @pars.s_symtab@ = @pars.i_symtab@;
      @}
    | ID                                /* name neu */  /* sichtbarkeit gesamte funktion */
      @{
        @i @pars.s_symtab@ = add_parameter(@pars.i_symtab@, @ID.lexeme@, @pars.i_position@);
      @}
    | ID COMMA pars                       /* name neu */  /* sichtbarkeit gesamte funktion */
      @{
        @i @pars.0.s_symtab@ = @pars.1.s_symtab@;
        @i @pars.1.i_symtab@ = add_parameter(@pars.0.i_symtab@, @ID.lexeme@, @pars.0.i_position@);
        @i @pars.1.i_position@ = @pars.0.i_position@ + 1;
      @}
    ;

stats:
      /* empty */
    | stat SEMICOLON stats
      @{
        @i @stats.1.i_symtab@ = @stat.0.s_symtab@;
        @codegen invoke_burm(@stat.s_n@);
      @}
    ;

stat:
      RETURN expr
      @{
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_n@ = new_unary_operator_node(OP_RET, @expr.s_n@);
      @}
    | IF expr THEN stats else_stats END
      @{
        @i @stats.i_symtab@ = @stat.i_symtab@;
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_n@ = new_nop_node();
      @}
    | ID COLON LOOP stats END             /* label neu */ /* sichtbarkeit innerhalb der schleife */
      @{
        @i @stats.i_symtab@ = add_label(@stat.i_symtab@, @ID.lexeme@);
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_n@ = new_nop_node();
      @}
    | BREAK ID                          /* label bestehend */
      @{
        @idcheck check_label(@stat.i_symtab@, @ID.lexeme@);
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_n@ = new_nop_node();
      @}
    | CONT ID                           /* label bestehend */
      @{
        @idcheck check_label(@stat.i_symtab@, @ID.lexeme@);
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_n@ = new_nop_node();
      @}
    | VAR ID ASSIGN expr                /* name neu */  /* sichtbarkeit direkt folgende statements von stat */
      @{
        @i @stat.s_symtab@ = add_name(@stat.i_symtab@, @ID.lexeme@);
        @i @stat.s_n@ = new_nop_node();
      @}
    | lexpr ASSIGN expr
      @{
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_n@ = new_nop_node();
      @}
    | expr
      @{
        @i @stat.s_symtab@ = @stat.i_symtab@;
        @i @stat.s_n@ = new_nop_node();
      @}
    ;

else_stats:
      /* empty */
    | ELSE stats
      @{
        @i @stats.i_symtab@ = @else_stats.i_symtab@;
      @}
    ;

lexpr:
       ID                               /* name bestehend */
      @{
        @idcheck check_name(@lexpr.i_symtab@, @ID.lexeme@);
      @}
     | ASTERISK term
     ;

expr:
      term
      @{
        @i @expr.s_n@ = @term.s_n@;
      @}
    | expr_unary
      @{
        @i @expr.s_n@ = @expr_unary.s_n@;
      @}
    | expr_binary
      @{
        @i @expr.s_n@ = @expr_binary.s_n@;
      @}
    ;

expr_unary:
      NOT expr_unary
      @{
        @i @expr_unary.0.s_n@ = new_unary_operator_node(OP_NOT, @expr_unary.1.s_n@);
      @}
    | HYPHEN expr_unary
      @{
        @i @expr_unary.0.s_n@ = new_unary_operator_node(OP_NEG, @expr_unary.1.s_n@);
      @}
    | ASTERISK expr_unary
      @{
        @i @expr_unary.0.s_n@ = new_unary_operator_node(OP_DRF, @expr_unary.1.s_n@);
      @}
    | NOT term
      @{
        @i @expr_unary.s_n@ = new_unary_operator_node(OP_NOT, @term.s_n@);
      @}
    | HYPHEN term
      @{
        @i @expr_unary.s_n@ = new_unary_operator_node(OP_NEG, @term.s_n@);
      @}
    | ASTERISK term
      @{
        @i @expr_unary.s_n@ = new_unary_operator_node(OP_DRF, @term.s_n@);
      @}
    ;

expr_binary:
      expr_add
      @{
        @i @expr_binary.s_n@ = @expr_add.s_n@;
      @}
    | expr_mul
      @{
        @i @expr_binary.s_n@ = @expr_mul.s_n@;
      @}
    | expr_and
      @{
        @i @expr_binary.s_n@ = @expr_and.s_n@;
      @}
    | expr_rel
      @{
        @i @expr_binary.s_n@ = @expr_rel.s_n@;
      @}
    ;

expr_add:
      expr_add PLUS term
      @{
        @i @expr_add.0.s_n@ = new_binary_operator_node(OP_ADD, @expr_add.1.s_n@, @term.s_n@);
      @}
    | term PLUS term
      @{
        @i @expr_add.s_n@ = new_binary_operator_node(OP_ADD, @term.0.s_n@, @term.1.s_n@);
      @}
    ;

expr_mul:
      expr_mul ASTERISK term
      @{
        @i @expr_mul.0.s_n@ = new_binary_operator_node(OP_MUL, @expr_mul.1.s_n@, @term.s_n@);
      @}
    | term ASTERISK term
      @{
        @i @expr_mul.s_n@ = new_binary_operator_node(OP_MUL, @term.0.s_n@, @term.1.s_n@);
      @}
    ;

expr_and:
      expr_and AND term
      @{
        @i @expr_and.0.s_n@ = new_binary_operator_node(OP_AND, @expr_and.1.s_n@, @term.s_n@);
      @}
    | term AND term
      @{
        @i @expr_and.s_n@ = new_binary_operator_node(OP_AND, @term.0.s_n@, @term.1.s_n@);
      @}
    ;

expr_rel:
      term LEQ term
      @{
        @i @expr_rel.s_n@ = new_binary_operator_node(OP_LEQ, @term.0.s_n@, @term.1.s_n@);
      @}
    | term POUND term
      @{
        @i @expr_rel.s_n@ = new_binary_operator_node(OP_DIF, @term.0.s_n@, @term.1.s_n@);
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
        @i @term.s_n@ = @expr.s_n@;
      @}
    | NUM
      @{
        @i @term.s_n@ = new_constant_node(@NUM.value@);
      @}
    | ID                                /* name bestehend */
      @{
        @idcheck check_name(@term.i_symtab@, @ID.lexeme@);
        @i @term.s_n@ = new_variable_node(@ID.lexeme@, get_var_pos(@term.i_symtab@, @ID.lexeme@));
      @}
    | ID LPAREN expr_list RPAREN        /* funktion beliebig */
      @{
        @i @term.s_n@ = new_nop_node();
      @}
    ;

%%

char get_var_pos(sym_tab *tab, char *name) {
    sym_tab *needle = get_name(tab, name);
    return needle != NULL ? needle->pos : -1;
}

sym_tab *check_name(sym_tab *tab, char *lexeme) {
    if (contains_name(tab, lexeme) == 0) {
        EXIT_ERR_UNDEFINED_ID(lexeme);
    }

    return tab;
}

sym_tab *check_label(sym_tab *tab, char *lexeme) {
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

sym_tab *add_name(sym_tab *tab, char *lexeme) {
    sym_tab *succ;
    if (contains_id(tab, lexeme) != 0) {
        EXIT_ERR_DUPLICATE_ID(lexeme);
    }

    if ((succ = add_id(tab, lexeme, NAME, -1)) == NULL) {
        EXIT_ERR_OOM();
    }
    
    return succ;
}

sym_tab *add_label(sym_tab *tab, char *lexeme) {
    sym_tab *succ;
    if (contains_id(tab, lexeme) != 0) {
        EXIT_ERR_DUPLICATE_ID(lexeme);
    }

    if ((succ = add_id(tab, lexeme, LABEL, -1)) == NULL) {
        EXIT_ERR_OOM();
    }
    
    return succ;
}

void yyerror(char const *msg) {
    EXIT_ERR_PARSE(msg);
}

int main(int argc, char *argv[]) {
    (void) yyparse();
    return 0;
}
