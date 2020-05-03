#include <stdlib.h>
#include <stdio.h>
#include "treenode.h"


treenode *_new_operator_node(int op, treenode *left, treenode *right) {
    treenode *new_node = malloc(sizeof(treenode));

    if (new_node == NULL) {
        // TODO: rephrase
        fprintf(stderr, "Out of memory.\n");
        // TODO: not sure if this should be 3
        exit(4);
    }

    new_node->op = op;
    new_node->kids[0] = left;
    new_node->kids[1] = right;
    new_node->reg = 0;
    new_node->identifier = 0;
    new_node->val = 0;

    return new_node;
}

treenode *new_binary_operator_node(int op, treenode *left, treenode *right) {
    return _new_operator_node(op, left, right);
}

treenode *new_unary_operator_node(int op, treenode *kid) {
    return _new_operator_node(op, kid, NULL);
}

treenode *new_identifier_node(char *identifier, char *reg) {
    treenode *new_node = _new_operator_node(OP_ID, NULL, NULL);
    new_node->identifier = identifier;
    new_node->reg = reg;
    return new_node;
}

treenode *new_number_node(long long num) {
    treenode *new_node = _new_operator_node(OP_NUM, NULL, NULL);
    new_node->val = num;
    return new_node;
}

treenode *new_nop_node() {
    return _new_operator_node(OP_NOP, NULL, NULL);
}
