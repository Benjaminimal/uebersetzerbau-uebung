#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "sym_tab.h"
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
    new_node->labels[0] = NULL;
    new_node->labels[1] = NULL;
    new_node->val = 0;

    return new_node;
}

treenode *new_cont_node(sym_tab *tab) {
    treenode *new_node = _new_operator_node(OP_JMP, NULL, NULL);
    new_node->labels[0] = tab->labels[0];
}

treenode *new_break_node(sym_tab *tab) {
    treenode *new_node = _new_operator_node(OP_JMP, NULL, NULL);
    new_node->labels[0] = tab->labels[1];
}

treenode *new_label_node(char *label) {
    treenode *new_node = _new_operator_node(OP_LBL, NULL, NULL);
    new_node->labels[0] = label;
}

treenode *new_loop_node(sym_tab *tab, treenode *sequence) {
    treenode *label_node = new_label_node(tab->labels[0]);
    treenode *loop_node = _new_operator_node(OP_LOP, label_node, sequence);
    loop_node->labels[0] = tab->labels[0];
    loop_node->labels[1] = tab->labels[1];
    return loop_node;
}

treenode *new_sequence_node(treenode *left, treenode *right) {
    return _new_operator_node(OP_SEQ, left, right);
}

treenode *new_init_node(treenode *left, treenode *right) {
    return _new_operator_node(OP_INI, left, right);
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

treenode *new_constant_node(long val) {
    int op;
    switch (val) {
        case 0:
            op = OP_ZRO;
            break;
        case 1:
            op = OP_ONE;
            break;
        default:
            op = OP_CON;
            break;
    }
    treenode *new_node = _new_operator_node(op, NULL, NULL);
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
        case OP_ZRO:
            return "ZRO";
        case OP_ONE:
            return "ONE";
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
        case OP_INI:
            return "INI";
        case OP_ASN:
            return "ASN";
        case OP_SEQ:
            return "SEQ";
        case OP_LOP:
            return "LOP";
        case OP_LBL:
            return "LBL";
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
    printf("\n");
    _print_tree(node, 0);
    printf("\n");
}
