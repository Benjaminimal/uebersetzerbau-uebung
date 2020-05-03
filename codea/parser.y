%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "id_list.h"
#include "treenode.h"
#include "translator.h"

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

id_list *add_parameter(id_list *, char *, int);
id_list *add_name(id_list *, char *);
id_list *add_label(id_list *, char *);

char *get_arg_register_name(id_list *, char *);

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
%token NOT
%token AND
%token ASSIGN
%token LEQ

@attributes { char *lexeme; } ID
@attributes { long long value; } NUM
@attributes { id_list *i_ids, *s_ids; int i_position; } pars
@attributes { id_list *i_ids; } stats
@attributes { id_list *i_ids, *s_ids; treenode *s_n; } stat
@attributes { id_list *i_ids; } else
@attributes { id_list *i_ids, *s_ids; } lexpr
@attributes { id_list *i_ids; treenode *s_n; } expr
@attributes { id_list *i_ids; treenode *s_n; } expr_unary
@attributes { id_list *i_ids; treenode *s_n; } expr_binary
@attributes { id_list *i_ids; treenode *s_n; } expr_add
@attributes { id_list *i_ids; treenode *s_n; } expr_mul
@attributes { id_list *i_ids; treenode *s_n; } expr_and
@attributes { id_list *i_ids; treenode *s_n; } expr_rel
@attributes { id_list *i_ids; } expr_list
@attributes { id_list *i_ids, *s_ids; treenode *s_n; } term

@traversal @postorder codegen

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
        @i @pars.i_position@ = 0;
        @i @stats.i_ids@ = @pars.s_ids@;
        @codegen @revorder (1) function_start(@ID.lexeme@);
        @codegen function_end(@ID.lexeme@);
      @}
    ;

pars:
      /* empty */
      @{
        @i @pars.s_ids@ = @pars.i_ids@;
      @}
    | ID                                /* name neu */  /* sichtbarkeit gesamte funktion */
      @{
        @i @pars.s_ids@ = add_parameter(@pars.i_ids@, @ID.lexeme@, @pars.i_position@);
      @}
    | ID ',' pars                       /* name neu */  /* sichtbarkeit gesamte funktion */
      @{
        @i @pars.0.s_ids@ = @pars.1.s_ids@;
        @i @pars.1.i_ids@ = add_parameter(@pars.0.i_ids@, @ID.lexeme@, @pars.0.i_position@);
        @i @pars.1.i_position@ = @pars.0.i_position@ + 1;
      @}
    ;

stats:
      /* empty */
    | stat ';' stats
      @{
        @i @stat.i_ids@ = @stats.0.i_ids@;
        @i @stats.1.i_ids@ = @stat.0.s_ids@;
        @codegen invoke_burm(@stat.s_n@);
      @}
    ;

stat:
      RETURN expr
      @{
        @i @expr.i_ids@ = @stat.i_ids@;
        @i @stat.s_ids@ = @stat.i_ids@;
        @i @stat.s_n@ = new_unary_operator_node(OP_RET, @expr.s_n@);
      @}
    | IF expr THEN stats else END
      @{
        @i @expr.i_ids@ = @stat.i_ids@;
        @i @stats.i_ids@ = @stat.i_ids@;
        @i @else.i_ids@ = @stat.i_ids@;
        @i @stat.s_ids@ = @stat.i_ids@;
        @i @stat.s_n@ = new_nop_node();
      @}
    | ID ':' LOOP stats END             /* label neu */ /* sichtbarkeit innerhalb der schleife */
      @{
        @i @stats.i_ids@ = add_label(@stat.i_ids@, @ID.lexeme@);
        @i @stat.s_ids@ = @stat.i_ids@;
        @i @stat.s_n@ = new_nop_node();
      @}
    | BREAK ID                          /* label bestehend */
      @{
        @i @stat.s_ids@ = check_label(@stat.i_ids@, @ID.lexeme@);
        @i @stat.s_n@ = new_nop_node();
      @}
    | CONT ID                           /* label bestehend */
      @{
        @i @stat.s_ids@ = check_label(@stat.i_ids@, @ID.lexeme@);
        @i @stat.s_n@ = new_nop_node();
      @}
    | VAR ID ASSIGN expr                /* name neu */  /* sichtbarkeit direkt folgende statements von stat */
      @{
        @i @expr.i_ids@ = @stat.i_ids@;
        @i @stat.s_ids@ = add_name(@stat.i_ids@, @ID.lexeme@);
        @i @stat.s_n@ = new_nop_node();
      @}
    | lexpr ASSIGN expr
      @{
        @i @lexpr.i_ids@ = @stat.i_ids@;
        @i @expr.i_ids@ = @stat.i_ids@;
        @i @stat.s_ids@ = @stat.i_ids@;
        @i @stat.s_n@ = new_nop_node();
      @}
    | expr
      @{
        @i @expr.i_ids@ = @stat.i_ids@;
        @i @stat.s_ids@ = @stat.i_ids@;
        @i @stat.s_n@ = new_nop_node();
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
        @i @term.i_ids@ = @lexpr.i_ids@;
        @i @lexpr.s_ids@ = @lexpr.i_ids@;
      @}
     ;

expr:
      term
      @{
        @i @term.i_ids@ = @expr.i_ids@;
        @i @expr.s_n@ = @term.s_n@;
      @}
    | expr_unary
      @{
        @i @expr_unary.i_ids@ = @expr.i_ids@;
        @i @expr.s_n@ = @expr_unary.s_n@;
      @}
    | expr_binary
      @{
        @i @expr_binary.i_ids@ = @expr.i_ids@;
        @i @expr.s_n@ = @expr_binary.s_n@;
      @}
    ;

expr_unary:
      NOT expr_unary
      @{
        @i @expr_unary.1.i_ids@ = @expr_unary.0.i_ids@;
        @i @expr_unary.0.s_n@ = new_unary_operator_node(OP_NOT, @expr_unary.1.s_n@);
      @}
    | '-' expr_unary
      @{
        @i @expr_unary.1.i_ids@ = @expr_unary.0.i_ids@;
        @i @expr_unary.0.s_n@ = new_unary_operator_node(OP_NEG, @expr_unary.1.s_n@);
      @}
    | '*' expr_unary
      @{
        @i @expr_unary.1.i_ids@ = @expr_unary.0.i_ids@;
        @i @expr_unary.0.s_n@ = new_unary_operator_node(OP_DRF, @expr_unary.1.s_n@);
      @}
    | NOT term
      @{
        @i @term.i_ids@ = @expr_unary.i_ids@;
        @i @expr_unary.s_n@ = new_unary_operator_node(OP_NOT, @term.s_n@);
      @}
    | '-' term
      @{
        @i @term.i_ids@ = @expr_unary.i_ids@;
        @i @expr_unary.s_n@ = new_unary_operator_node(OP_NEG, @term.s_n@);
      @}
    | '*' term
      @{
        @i @term.i_ids@ = @expr_unary.i_ids@;
        @i @expr_unary.s_n@ = new_unary_operator_node(OP_DRF, @term.s_n@);
      @}
    ;

expr_binary:
      expr_add
      @{
        @i @expr_add.i_ids@ = @expr_binary.i_ids@;
        @i @expr_binary.s_n@ = @expr_add.s_n@;
      @}
    | expr_mul
      @{
        @i @expr_mul.i_ids@ = @expr_binary.i_ids@;
        @i @expr_binary.s_n@ = @expr_mul.s_n@;
      @}
    | expr_and
      @{
        @i @expr_and.i_ids@ = @expr_binary.i_ids@;
        @i @expr_binary.s_n@ = @expr_and.s_n@;
      @}
    | expr_rel
      @{
        @i @expr_rel.i_ids@ = @expr_binary.i_ids@;
        @i @expr_binary.s_n@ = @expr_rel.s_n@;
      @}
    ;

expr_add:
      expr_add '+' term
      @{
        @i @expr_add.1.i_ids@ = @expr_add.0.i_ids@;
        @i @term.i_ids@ = @expr_add.0.i_ids@;
        @i @expr_add.0.s_n@ = new_binary_operator_node(OP_ADD, @expr_add.1.s_n@, @term.s_n@);
      @}
    | term '+' term
      @{
        @i @term.0.i_ids@ = @expr_add.i_ids@;
        @i @term.1.i_ids@ = @expr_add.i_ids@;
        @i @expr_add.s_n@ = new_binary_operator_node(OP_ADD, @term.0.s_n@, @term.1.s_n@);
      @}
    ;

expr_mul:
      expr_mul '*' term
      @{
        @i @expr_mul.1.i_ids@ = @expr_mul.0.i_ids@;
        @i @term.i_ids@ = @expr_mul.0.i_ids@;
        @i @expr_mul.0.s_n@ = new_binary_operator_node(OP_MUL, @expr_mul.1.s_n@, @term.s_n@);
      @}
    | term '*' term
      @{
        @i @term.0.i_ids@ = @expr_mul.i_ids@;
        @i @term.1.i_ids@ = @expr_mul.i_ids@;
        @i @expr_mul.s_n@ = new_binary_operator_node(OP_MUL, @term.0.s_n@, @term.1.s_n@);
      @}
    ;

expr_and:
      expr_and AND term
      @{
        @i @expr_and.1.i_ids@ = @expr_and.0.i_ids@;
        @i @term.i_ids@ = @expr_and.0.i_ids@;
        @i @expr_and.0.s_n@ = new_binary_operator_node(OP_AND, @expr_and.1.s_n@, @term.s_n@);
      @}
    | term AND term
      @{
        @i @term.0.i_ids@ = @expr_and.i_ids@;
        @i @term.1.i_ids@ = @expr_and.i_ids@;
        @i @expr_and.s_n@ = new_binary_operator_node(OP_AND, @term.0.s_n@, @term.1.s_n@);
      @}
    ;

expr_rel:
      term LEQ term
      @{
        @i @term.0.i_ids@ = @expr_rel.i_ids@;
        @i @term.1.i_ids@ = @expr_rel.i_ids@;
        @i @expr_rel.s_n@ = new_binary_operator_node(OP_LEQ, @term.0.s_n@, @term.1.s_n@);
      @}
    | term '#' term
      @{
        @i @term.0.i_ids@ = @expr_rel.i_ids@;
        @i @term.1.i_ids@ = @expr_rel.i_ids@;
        @i @expr_rel.s_n@ = new_binary_operator_node(OP_DIF, @term.0.s_n@, @term.1.s_n@);
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
        @i @term.s_ids@ = @term.i_ids@;
        @i @term.s_n@ = @expr.s_n@;
      @}
    | NUM
      @{
        @i @term.s_ids@ = @term.i_ids@;
        @i @term.s_n@ = new_number_node(@NUM.value@);
      @}
    | ID                                /* name bestehend */
      @{
        @i @term.s_ids@ = check_name(@term.i_ids@, @ID.lexeme@);
        @i @term.s_n@ = new_identifier_node(@ID.lexeme@, get_arg_register_name(@term.s_ids@, @ID.lexeme@));
      @}
    | ID '(' expr_list ')'              /* funktion beliebig */
      @{
        @i @expr_list.i_ids@ = @term.i_ids@;
        @i @term.s_ids@ = @term.i_ids@;
        @i @term.s_n@ = new_nop_node();
      @}
    ;

%%

char *get_arg_register_name(id_list *list, char *lexeme) {
    id_list *element = get_id(list, lexeme);
    if (element == NULL) {
        return NULL;
    }

    return get_argument_register(element->parameter_position);
}

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

id_list *add_parameter(id_list *list, char *lexeme, int position) {
    id_list *succ;
    if (contains_id(list, lexeme) != 0) {
        EXIT_DUPLICATE_ID_ERR(lexeme);
    }

    if ((succ = add_id(list, lexeme, NAME, position)) == NULL) {
        EXIT_OOM_ERR();
    }
    
    return succ;
}

id_list *add_name(id_list *list, char *lexeme) {
    id_list *succ;
    if (contains_id(list, lexeme) != 0) {
        EXIT_DUPLICATE_ID_ERR(lexeme);
    }

    if ((succ = add_id(list, lexeme, NAME, -1)) == NULL) {
        EXIT_OOM_ERR();
    }
    
    return succ;
}

id_list *add_label(id_list *list, char *lexeme) {
    id_list *succ;
    if (contains_id(list, lexeme) != 0) {
        EXIT_DUPLICATE_ID_ERR(lexeme);
    }

    if ((succ = add_id(list, lexeme, LABEL, -1)) == NULL) {
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
