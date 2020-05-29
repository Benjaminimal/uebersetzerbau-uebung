#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "sym_tab.h"
#include "ast.h"

// TODO: rename this to ast or something similar

astnode *_new_operator_node(int op, astnode *left, astnode *right) {
    astnode *new_node = malloc(sizeof(astnode));

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

astnode *new_if_node(astnode *expression, astnode *statements_true, astnode *statements_false, char *label_false, char *label_end) {

    astnode *jump_cc_true_node = _new_operator_node(OP_JCC, statements_true, NULL);
    jump_cc_true_node->labels[0] = label_false;
    jump_cc_true_node->labels[1] = label_end;

    astnode *true_node = _new_operator_node(OP_BRA, jump_cc_true_node, NULL);

    astnode *jump_cc_false_node = _new_operator_node(OP_JCC, statements_false, NULL);
    jump_cc_false_node->labels[0] = label_false;
    jump_cc_false_node->labels[1] = label_end;

    astnode *false_node = _new_operator_node(OP_BRA, jump_cc_false_node, NULL);

    astnode *alternative_node = _new_operator_node(OP_ALT, true_node, false_node);

    astnode *test_node = _new_operator_node(OP_TST, expression, NULL);

    astnode *jump_cc_node = _new_operator_node(OP_JCC, test_node, NULL);
    jump_cc_node->labels[0] = label_false;
    jump_cc_node->labels[1] = label_end;

    astnode *if_node = _new_operator_node(OP_IF, jump_cc_node, alternative_node);

    return if_node;
}

astnode *new_cont_node(sym_tab *tab) {
    astnode *cont_node = _new_operator_node(OP_JMP, NULL, NULL);
    if (tab != NULL) {
        cont_node->labels[0] = tab->labels[0];
    }
    return cont_node;
}

astnode *new_break_node(sym_tab *tab) {
    astnode *break_node = _new_operator_node(OP_JMP, NULL, NULL);
    if (tab != NULL) {
        break_node->labels[0] = tab->labels[1];
    }
    return break_node;
}

astnode *new_label_node(char *label) {
    astnode *label_node = _new_operator_node(OP_LBL, NULL, NULL);
    label_node->labels[0] = label;
    return label_node;
}

astnode *new_loop_node(sym_tab *tab, astnode *sequence) {
    astnode *label_node = new_label_node(tab->labels[0]);
    astnode *loop_node = _new_operator_node(OP_LOP, label_node, sequence);
    if (tab != NULL) {
        loop_node->labels[0] = tab->labels[0];
        loop_node->labels[1] = tab->labels[1];
    }
    return loop_node;
}

astnode *new_sequence_node(astnode *left, astnode *right) {
    return _new_operator_node(OP_SEQ, left, right);
}

astnode *new_assign_node(astnode *expression, astnode *lexpression) {
    return _new_operator_node(OP_ASN, expression, lexpression);
}

astnode *new_init_node(sym_tab *tab, astnode *expression) {
    astnode *new_variable = new_variable_node(tab);
    return _new_operator_node(OP_INI, expression, new_variable);
}

astnode *new_add_node(astnode *left, astnode *right) {
    return _new_operator_node(OP_ADD, left, right);
}

astnode *new_multiply_node(astnode *left, astnode *right) {
    return _new_operator_node(OP_MUL, left, right);
}

astnode *new_and_node(astnode *left, astnode *right) {
    return _new_operator_node(OP_AND, left, right);
}

astnode *new_compare_less_or_equal_node(astnode *left, astnode *right) {
    return _new_operator_node(OP_LEQ, left, right);
}

astnode *new_compare_not_equal_node(astnode *left, astnode *right) {
    return _new_operator_node(OP_NEQ, left, right);
}

astnode *new_return_node(astnode *expression) {
    return _new_operator_node(OP_RET, expression, NULL);
}

astnode *new_dereference_node(astnode *node) {
    return _new_operator_node(OP_DRF, node, NULL);
}

astnode *new_not_node(astnode *node) {
    return _new_operator_node(OP_NOT, node, NULL);
}

astnode *new_negate_node(astnode *node) {
    return _new_operator_node(OP_NEG, node, NULL);
}

astnode *new_variable_node(sym_tab *tab) {
    astnode *new_node = _new_operator_node(OP_VAR, NULL, NULL);
    if (tab != NULL) {
        new_node->sym = tab->lexeme;
        new_node->pos = tab->pos;
    }
    return new_node;
}

astnode *new_constant_node(long val) {
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
    astnode *new_node = _new_operator_node(op, NULL, NULL);
    new_node->val = val;
    return new_node;
}

astnode *new_nop_node() {
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
            return "JMP";
        case OP_IF:
            return "IF";
        case OP_TST:
            return "TST";
        case OP_ALT:
            return "ALT";
        case OP_BRA:
            return "BRA";
        case OP_JCC:
            return "JCC";
        case OP_NOP:
            return "NOP";
    }
}

void _print_tree(astnode *node, int indent) {
    if (node == NULL) return;
    for (int i = 0; i < indent; i++) printf("\t");
    printf("op: %s", op_to_str(node->op));
    if (node->sym != NULL) printf(" id: %s pos: %d", node->sym, node->pos);
    for (int i = 0; i < 2; i++) {
        if (node->labels[i] != NULL) {
            printf(" labels[%d]: %s", i, node->labels[i]);
        }
    }
    if (node->op == OP_CON) printf(" val: %ld", node->val);
    printf("\n");
    _print_tree(node->kids[0], indent + 1);
    _print_tree(node->kids[1], indent + 1);
}

void print_tree(astnode *node) {
    printf("\n");
    _print_tree(node, 0);
    printf("\n");
}
