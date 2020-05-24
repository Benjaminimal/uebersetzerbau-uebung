#ifndef _TREENODE_H_
#define _TREENODE_H_

#include "sym_tab.h"

#define OP_VAR 1
#define OP_CON 2
#define OP_ZRO 3
#define OP_ONE 4
#define OP_LEQ 5
#define OP_DIF 6
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
#define OP_TRU 23
#define OP_FAL 24
#define OP_NOP 25

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
    char            pos;
    char            *labels[2];
    long            val;
} treenode;

typedef treenode *treenodep;

#define NODEPTR_TYPE    treenodep
#define OP_LABEL(p)     ((p)->op)
#define LEFT_CHILD(p)   ((p)->kids[0])
#define RIGHT_CHILD(p)  ((p)->kids[1])
#define STATE_LABEL(p)  ((p)->state)
#define PANIC           printf

/* codeb */
treenode *new_init_node(treenode *, treenode *);
treenode *new_assign_node(treenode *, treenode *);
treenode *new_sequence_node(treenode *, treenode *);
treenode *new_loop_node(sym_tab *, treenode *);
treenode *new_cont_node(sym_tab *);
treenode *new_break_node(sym_tab *);
treenode *new_if_node(treenode *, treenode *, treenode *, char *, char *);

/* codea */
treenode *new_binary_operator_node(int, treenode *, treenode *);
treenode *new_unary_operator_node(int, treenode *);
treenode *new_variable_node(char *, char);
treenode *new_constant_node(long);
treenode *new_nop_node();
void print_tree(treenode *);

#endif /* _TREENODE_H_ */
