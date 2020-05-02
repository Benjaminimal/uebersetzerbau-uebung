#include "treenode.h"

treenode *_newOperatorNode(int op, treenode *left, treenode *right) {
    treenode *newNode = malloc(sizeof(treenode));

    if (newNode == NULL) { printf("Out of memory.\n"); exit(4);}

    newNode->op = op;
    newNode->kids[0] = left;
    newNode->kids[1] = right;
    newNode->id=0;
    newNode->val=0;

    return newNode;
}

treenode *newBinaryOperatorNode(int op, treenode *left, treenode *right) {
    return _newOperatorNode(op, left, right);
}

treenode *newUnaryOperatorNode(int op, treenode *kid) {
    return _newOperatorNode(op, kid, NULL);
}

treenode *newIdentifierNode(char *identifier) {
    treenode *newNode = _newOperatorNode(OP_ID, NULL, NULL);
    newNode->regname = regname;
    return newNode;
}

treenode *newNumberNode(long long num) {
    treenode *newNode = _newOperatorNode(OP_NUM, NULL, NULL);
    newNode->val = num;
    return newNode;
}

treenode *newNullNode() {
    return NULL;
}
