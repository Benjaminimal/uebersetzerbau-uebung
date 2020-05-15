#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "treenode.h"


treenode *_new_operator_node(int op, treenode *left, treenode *right) {
    treenode *new_node = malloc(sizeof(treenode));

    if (new_node == NULL) {
        EXIT_ERR_OOM();
    }

    new_node->op = op;
    new_node->kids[0] = left;
    new_node->kids[1] = right;
    new_node->reg = -1;
    new_node->sym = NULL;
    new_node->pos = -1;
    new_node->val = 0;

    return new_node;
}

treenode *new_binary_operator_node(int op, treenode *left, treenode *right) {
    return _new_operator_node(op, left, right);
}

treenode *new_unary_operator_node(int op, treenode *kid) {
    return _new_operator_node(op, kid, NULL);
}

treenode *new_variable_node(char *sym, char pos) {
    treenode *new_node = _new_operator_node(OP_VAR, NULL, NULL);
    new_node->sym = sym;
    new_node->pos = pos;
    return new_node;
}

treenode *new_constant_node(long long val) {
    treenode *new_node = _new_operator_node(OP_CON, NULL, NULL);
    new_node->val = val;
    return new_node;
}

treenode *new_nop_node() {
    return _new_operator_node(OP_NOP, NULL, NULL);
}

char *op_to_str(int op) {
    switch (op) {
        case OP_VAR:
            return "VAR";
        case OP_CON:
            return "CON";
        case OP_LEQ:
            return "LEQ";
        case OP_DIF:
            return "DIF";
        case OP_AND:
            return "AND";
        case OP_MUL:
            return "MUL";
        case OP_ADD:
            return "ADD";
        case OP_NOT:
            return "NOT";
        case OP_NEG:
            return "NEG";
        case OP_DRF:
            return "DRF";
        case OP_RET:
            return "RET";
        case OP_NOP:
            return "NOP";
    }
}

void _print_tree(treenode *node, int indent) {
    if (node == NULL) return;
    for (int i = 0; i < indent; i++) printf("\t");
    printf("op: %s", op_to_str(node->op));
    if (node->sym != NULL) printf(" id: %s", node->sym);
    printf(" val: %lld", node->val);
    printf("\n");
    _print_tree(node->kids[0], indent + 1);
    _print_tree(node->kids[1], indent + 1);
}

void print_tree(treenode *node) {
    _print_tree(node, 0);
}
