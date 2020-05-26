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

treenode *new_if_node(treenode *expression, treenode *statements_true, treenode *statements_false, char *label_false, char *label_end) {

    statements_true->labels[0] = label_false;
    statements_true->labels[1] = label_end;
    treenode *jump_end_node = _new_operator_node(OP_JCC, statements_true, NULL);
    treenode *true_node = _new_operator_node(OP_BRA, jump_end_node, NULL);

    statements_false->labels[0] = label_false;
    statements_false->labels[1] = label_end;
    treenode *jump_not_node = _new_operator_node(OP_JCC, statements_false, NULL);
    treenode *false_node = _new_operator_node(OP_BRA, jump_not_node, NULL);

    treenode *alternative_node = _new_operator_node(OP_ALT, true_node, false_node);

    treenode *test_node = _new_operator_node(OP_TST, expression, NULL);

    treenode *conditional_jump_node = _new_operator_node(OP_JCC, test_node, NULL);
    conditional_jump_node->labels[0] = label_false;
    conditional_jump_node->labels[1] = label_end;

    treenode *if_node = _new_operator_node(OP_IF, conditional_jump_node, alternative_node);

    return if_node;
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

treenode *new_assign_node(treenode *expression, treenode *lexpression) {
    return _new_operator_node(OP_ASN, expression, lexpression);
}

treenode *new_init_node(sym_tab *tab, treenode *expression) {
    treenode *new_variable = new_variable_node(tab);
    return _new_operator_node(OP_INI, expression, new_variable);
}

treenode *new_add_node(treenode *left, treenode *right) {
    return _new_operator_node(OP_ADD, left, right);
}

treenode *new_multiply_node(treenode *left, treenode *right) {
    return _new_operator_node(OP_MUL, left, right);
}

treenode *new_and_node(treenode *left, treenode *right) {
    return _new_operator_node(OP_AND, left, right);
}

treenode *new_compare_less_or_equal_node(treenode *left, treenode *right) {
    return _new_operator_node(OP_LEQ, left, right);
}

treenode *new_compare_not_equal_node(treenode *left, treenode *right) {
    return _new_operator_node(OP_NEQ, left, right);
}

treenode *new_return_node(treenode *expression) {
    return _new_operator_node(OP_RET, expression, NULL);
}

treenode *new_dereference_node(treenode *node) {
    return _new_operator_node(OP_DRF, node, NULL);
}

treenode *new_not_node(treenode *node) {
    return _new_operator_node(OP_NOT, node, NULL);
}

treenode *new_negate_node(treenode *node) {
    return _new_operator_node(OP_NEG, node, NULL);
}

treenode *new_variable_node(sym_tab *tab) {
    treenode *new_node = _new_operator_node(OP_VAR, NULL, NULL);
    new_node->sym = tab->lexeme;
    new_node->pos = tab->pos;
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
        case OP_NEQ:
            return "NEQ";
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
        case OP_JMP:
            return "OP_JMP";
        case OP_IF:
            return "OP_IF";
        case OP_TST:
            return "OP_TST";
        case OP_ALT:
            return "OP_ALT";
        case OP_BRA:
            return "BRA";
        case OP_JCC:
            return "JCC";
        case OP_NOP:
            return "NOP";
    }
}

void _print_tree(treenode *node, int indent) {
    if (node == NULL) return;
    for (int i = 0; i < indent; i++) printf("\t");
    printf("op: %s", op_to_str(node->op));
    if (node->sym != NULL) printf(" id: %s pos: %d", node->sym, node->pos);
    if (node->op == OP_CON) printf(" val: %lld", node->val);
    printf("\n");
    _print_tree(node->kids[0], indent + 1);
    _print_tree(node->kids[1], indent + 1);
}

void print_tree(treenode *node) {
    printf("\n");
    _print_tree(node, 0);
    printf("\n");
}
