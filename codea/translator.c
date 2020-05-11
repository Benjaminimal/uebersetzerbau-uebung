#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "translator.h"

#define T_VAL -1
#define F_VAL 0

#define LABEL_PREFIX ".L"
#define LABEL_PREFIX_LEN 2

int function_count = 0;
int label_count = 0;

typedef struct {
    char *name;
    char *b_name;
    int taken;
} reg;

reg RAX = { .name = "rax", .b_name = "al", .taken = 0 };
reg RBX = { .name = "rbx", .b_name = "bl", .taken = 0 };
reg RCX = { .name = "rcx", .b_name = "cl", .taken = 0 };
reg RDX = { .name = "rdx", .b_name = "dl", .taken = 0 };
reg RSP = { .name = "rsp", .b_name = "sil", .taken = 0 };
reg RBP = { .name = "rbp", .b_name = "dil", .taken = 0 };
reg RSI = { .name = "rsi", .b_name = "bpl", .taken = 0 };
reg RDI = { .name = "rdi", .b_name = "spl", .taken = 0 };
reg R8 = { .name = "r8", .b_name = "r8b", .taken = 0 };
reg R9 = { .name = "r9", .b_name = "r9b", .taken = 0 };
reg R10 = { .name = "r10", .b_name = "r10b", .taken = 0 };
reg R11 = { .name = "r11", .b_name = "r11b", .taken = 0 };
reg R12 = { .name = "r12", .b_name = "r12b", .taken = 0 };
reg R13 = { .name = "r13", .b_name = "r13b", .taken = 0 };
reg R14 = { .name = "r14", .b_name = "r14b", .taken = 0 };
reg R15 = { .name = "r15", .b_name = "r15b", .taken = 0 };

reg *reg_args[] = {&RDI, &RSI, &RDX, &RCX, &R8, &R9};
/*
const reg *reg_caller[] = {&RAX, &RCX, &RDX, &RSI, &RDI, &R8, &R9, &R10, &R11}; // TODO: not sure if r10 can be used
const reg *reg_callee[] = {&RBX, &R12, &R13, &R14, &R15}; // TODO: rbp (and rsp) is missing
*/

void _mov(char *, char *);
void _mov_i(long long, char *);
void _cmp(char *, char *);
void _cmp_i(long long, char *);
void _and(char *, char *);
void _and_i(long long , char *);
void _mul(char *, char *);
void _mul_i(long long, char *);
void _add(char *, char *);
void _add_i(long long, char *);
void _not(char *);
void _neg(char *);
void _drf(char *, char *);
void _drf_i(long long, char *);
void _ret();
void _jmp(char *);
void _jcc(char *, char *);
void _lbl(char *);
void _cmp_cc(char *, char *, char *, char *);
void _cmp_cc_i(char *, long long, char *, char *);


int _num_digits(int n) {
    int cnt = 0;
    while (n != 0) {
        n /= 10;
        cnt++;
    }
    return cnt;
}

char *next_label() {
    char *label = malloc(LABEL_PREFIX_LEN + _num_digits(label_count + 1));
    if (label == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(4);
    }
    sprintf(label, "%s%d", LABEL_PREFIX, label_count);
    label_count++;
    return label;
}

char *get_argument_register(int position) {
    if (position < 0 || position >= MAX_ARGS) {
        return NULL;
    }
    reg_args[position]->taken = 1;
    return reg_args[position]->name;
}

char *next_reg() {
    for (int i = 0; i < MAX_ARGS; i++) {
        reg *current = reg_args[i];
        if (current->taken == 0) {
            current->taken = 1;
            return current->name;
        }
    }
    return NULL;
}

reg *get_reg(char *name) {
    for (int i = 0; i < MAX_ARGS; i++) {
        reg *current = reg_args[i];
        if (strcmp(current->name, name) == 0) {
            return current;
        }
    }
    return NULL;
}

void free_reg(char *name) {
    reg *found = get_reg(name);
    if (found != NULL) {
        found->taken = 0;
    }
}

void function_start(char *name) {
    if (function_count == 0) {
        printf("\t.text\n");
    }
    printf("\t.globl\t%s\n", name);
    printf("\t.type\t%s, @function\n", name);
    _lbl(name);
    // printf(".LFB%d:\n", function_count);
    // printf("\tpushq\t%%%s\n", RBP.name);
    // printf("\tmovq\t%%%s, %%%s\n", RSP.name, RBP.name);
}

void function_end(char *name) {
    // printf("\tpopq\t%%%s\n", RBP); // TODO: might be moved to ret
    // printf(".LFE%d:\n", function_count);
    // printf("\t.size\t%s, .-%s\n", name, name);
    function_count++;
}

void cmp_leq(char *lsrc, char *rsrc, char *dst) {
    _cmp_cc("le", rsrc, lsrc, dst);
}
void cmp_leq_i(long long val, char *src, char *dst) {
    _cmp_cc_i("ge", val, src, dst);
}

void cmp_geq_i(long long val, char *src, char *dst) {
    _cmp_cc_i("le", val, src, dst);
}

void cmp_dif(char *lsrc, char *rsrc, char *dst) {
    _cmp_cc("ne", lsrc, rsrc, dst);
}
void cmp_dif_i(long long val, char *src, char *dst) {
    _cmp_cc_i("ne", val, src, dst);
}

void and(char *lsrc, char *rsrc, char *dst) {
    _mov(lsrc, dst);
    _and(rsrc, dst);
}
void and_i(long long val, char *src, char *dst) {
    _mov_i(val, dst);
    _and(src, dst);
}

void mul(char *lsrc, char *rsrc, char *dst) {
    _mov(lsrc, dst);
    _mul(rsrc, dst);
}
void mul_i(long long val, char *src, char *dst) {
    _mov_i(val, dst);
    _mul(src, dst);
}

void add(char *lsrc, char *rsrc, char *dst) {
    _mov(lsrc, dst);
    _add(rsrc, dst); // TODO: replace by leaq (l,r), d
}
void add_i(long long val, char *src, char *dst) {
    _mov_i(val, dst);
    _add(src, dst); // TODO: replace by leaq (l,r), d
}

void not(char *src, char *dst) {
    _mov(src, dst);
    _not(dst);
}

void neg(char *src, char *dst) {
    _mov(src, dst);
    _neg(dst);
}

void drf(char *src, char *dst) {
    _drf(src, dst);
}
void drf_i(long long val, char *dst) {
    _drf_i(val, dst);
}

void ret(char *src) {
    _mov(src, "rax");
    _ret();
}
void ret_i(long long val) {
    _mov_i(val, "rax");
    _ret();
}


void _mov(char *src, char *dst) {
    // TODO: check for reduntant move
    printf("\tmovq\t%%%s, %%%s\n", src, dst);
}

void _mov_i(long long val, char *dst) {
    printf("\tmovq\t$%lld, %%%s\n", val, dst);
}

void _cmp(char *left, char *right) {
    printf("\tcmpq\t%%%s, %%%s\n", left, right);
}
void _cmp_i(long long val, char *dst) {
    printf("\tcmpq\t$%lld, %%%s\n", val, dst);
}

void _and(char *src, char *dst) {
    printf("\tandq\t%%%s, %%%s\n", src, dst);
}
void _and_i(long long val, char *dst) {
    printf("\tandq\t$%lld, %%%s\n", val, dst);
}

void _mul(char *src, char *dst) {
    printf("\timulq\t%%%s, %%%s\n", src, dst);
}
void _mul_i(long long val, char *dst) {
    printf("\timulq\t$%lld, %%%s\n", val, dst);
}


void _add(char *src, char *dst) {
    printf("\taddq\t%%%s, %%%s\n", src, dst);
}
void _add_i(long long val, char *dst) {
    printf("\taddq\t$%lld, %%%s\n", val, dst);
}

void _not(char *src_dst) {
    printf("\tnotq\t%%%s\n", src_dst);
}

void _neg(char *src_dst) {
    printf("\tnegq\t%%%s\n", src_dst);
}

void _drf(char *src, char *dst) {
    printf("\tmovq\t(%%%s), %%%s\n", src, dst);
}
void _drf_i(long long val, char *dst) {
    printf("\tmovq\t($%lld), %%%s\n", val, dst);
}

void _ret() {
    printf("\tret\n");
}

void _jmp(char *loc) {
    printf("\tjmp\t%s\n", loc);
}

void _jcc(char *cond, char *loc) {
    printf("\tj%s\t%s\n", cond, loc);
}

void _lbl(char *label) {
    printf("%s:\n", label);
}

void _cmp_cc(char *cond, char *lsrc, char *rsrc, char *dst) {
    char *label_true = next_label();
    char *label_end = next_label();

    _cmp(lsrc, rsrc);
    _jcc(cond, label_true);
    _mov_i(0, dst);
    _jmp(label_end);
    _lbl(label_true);
    _mov_i(-1, dst);
    _lbl(label_end);

    free(label_true);
    free(label_end);
}

void _cmp_cc_i(char *cond, long long val, char *src, char *dst) {
    char *label_true = next_label();
    char *label_end = next_label();

    _cmp_i(val, src);
    _jcc(cond, label_true);
    _mov_i(0, dst);
    _jmp(label_end);
    _lbl(label_true);
    _mov_i(-1, dst);
    _lbl(label_end);

    free(label_true);
    free(label_end);
}
