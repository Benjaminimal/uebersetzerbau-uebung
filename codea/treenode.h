#ifndef _TREENODE_H_
#define _TREENODE_H_

#define OP_ID 1
#define OP_NUM 2
#define OP_LEQ 3
#define OP_DIF 4
#define OP_AND 5
#define OP_MUL 6
#define OP_ADD 7
#define OP_NOT 8
#define OP_NEG 9
#define OP_DRF 10
#define OP_RET 11

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
    char            *regname;
    char            *identifier;
    long long       value;
} treenode;

typedef treenode *treenodep;

#define NODEPTR_TYPE    treenodep
#define OP_LABEL(p)     ((p)->op)
#define LEFT_CHILD(p)   ((p)->kids[0])
#define RIGHT_CHILD(p)  ((p)->kids[1])
#define STATE_LABEL(p)  ((p)->state)
#define PANIC           printf

treenode *newBinaryOperatorNode(int, treenode*, treenode*);
treenode *newUnaryOperatorNode(int, treenode*);
treenode *newIdentifierNode(char*, char*);
treenode *newNumberNode(long long);
treenode *newNullNode();

#endif /* _TREENODE_H_ */
