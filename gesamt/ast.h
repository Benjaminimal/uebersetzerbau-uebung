#ifndef _TREENODE_H_
#define _TREENODE_H_

#include "sym_tab.h"

#define OP_VAR 1
#define OP_CON 2
#define OP_ZRO 3
#define OP_ONE 4
#define OP_LEQ 5
#define OP_NEQ 6
#define OP_AND 7
#define OP_MUL 8
#define OP_ADD 9
#define OP_NOT 10
#define OP_NEG 11
#define OP_DRF 12
#define OP_RET 13
#define OP_INI 14
#define OP_ASN 15
#define OP_SEQ 16
#define OP_LOP 17
#define OP_LBL 18
#define OP_JMP 19
#define OP_IF  20
#define OP_TST 21
#define OP_ALT 22
#define OP_BRA 23
#define OP_JCC 24
#define OP_FUN 25
#define OP_BGN 26
#define OP_ENT 27
#define OP_PRS 29

#define OP_CLL 31
#define OP_ARG 32
#define OP_EXP 33
#define OP_SAV 34
#define OP_NOP 35

#ifdef USE_IBURG
typedef struct burm_state *STATEPTR_TYPE;
#else /* USE_IBURG */
#define STATEPTR_TYPE int
#endif /* USE_IBURG */

typedef struct s_node {
    int             op;
    struct s_node   *kids[2];
    STATEPTR_TYPE   state;
    /* user defined data fields follow here */
    char            reg;
    char            *sym;
    int             pos;
    char            *labels[2];
    long            val;
} astnode;

typedef astnode *astnodep;

#define NODEPTR_TYPE    astnodep
#define OP_LABEL(p)     ((p)->op)
#define LEFT_CHILD(p)   ((p)->kids[0])
#define RIGHT_CHILD(p)  ((p)->kids[1])
#define STATE_LABEL(p)  ((p)->state)
#define PANIC           printf

/* gesamt */
astnode *new_function_node(char *, int, astnode *, astnode *);
astnode *new_parameters_node(int, astnode *);
astnode *new_call_node(char *, astnode *);
astnode *new_arguments_node(astnode *, astnode *);
astnode *new_expression_node(astnode *);

/* codeb */
astnode *new_init_node(sym_tab *, astnode *);
astnode *new_assign_node(astnode *, astnode *);
astnode *new_sequence_node(astnode *, astnode *);
astnode *new_loop_node(sym_tab *, astnode *);
astnode *new_cont_node(sym_tab *);
astnode *new_break_node(sym_tab *);
astnode *new_if_node(astnode *, astnode *, astnode *, char *, char *);

/* codea */
astnode *new_add_node(astnode *, astnode *);
astnode *new_multiply_node(astnode *, astnode *);
astnode *new_and_node(astnode *, astnode *);
astnode *new_compare_less_or_equal_node(astnode *, astnode *);
astnode *new_compare_not_equal_node(astnode *, astnode *);
astnode *new_return_node(astnode *);
astnode *new_dereference_node(astnode *);
astnode *new_not_node(astnode *);
astnode *new_negate_node(astnode *);
astnode *new_variable_node(sym_tab *);
astnode *new_constant_node(long);
astnode *new_nop_node();
void print_tree(astnode *);

#endif /* _TREENODE_H_ */
