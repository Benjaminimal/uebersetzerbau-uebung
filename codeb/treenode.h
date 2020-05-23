#ifndef _TREENODE_H_
#define _TREENODE_H_

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
#define OP_NOP 14

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
    long       val;
} treenode;

typedef treenode *treenodep;

#define NODEPTR_TYPE    treenodep
#define OP_LABEL(p)     ((p)->op)
#define LEFT_CHILD(p)   ((p)->kids[0])
#define RIGHT_CHILD(p)  ((p)->kids[1])
#define STATE_LABEL(p)  ((p)->state)
#define PANIC           printf

treenode *new_binary_operator_node(int, treenode*, treenode*);
treenode *new_unary_operator_node(int, treenode*);
treenode *new_variable_node(char*, char);
treenode *new_constant_node(long);
treenode *new_nop_node();
void print_tree(treenode *);

#endif /* _TREENODE_H_ */
