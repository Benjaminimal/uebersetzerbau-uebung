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
    int taken;
} reg;

reg RAX = { .name = "rax", .taken = 0 };
reg RBX = { .name = "rbx", .taken = 0 };
reg RCX = { .name = "rcx", .taken = 0 };
reg RDX = { .name = "rdx", .taken = 0 };
reg RSP = { .name = "rsp", .taken = 0 };
reg RBP = { .name = "rbp", .taken = 0 };
reg RSI = { .name = "rsi", .taken = 0 };
reg RDI = { .name = "rdi", .taken = 0 };
reg R8 = { .name = "r8", .taken = 0 };
reg R9 = { .name = "r9", .taken = 0 };
reg R10 = { .name = "r10", .taken = 0 };
reg R11 = { .name = "r11", .taken = 0 };
reg R12 = { .name = "r12", .taken = 0 };
reg R13 = { .name = "r13", .taken = 0 };
reg R14 = { .name = "r14", .taken = 0 };
reg R15 = { .name = "r15", .taken = 0 };

reg *reg_args[MAX_ARGS] = {&RDI, &RSI, &RDX, &RCX, &R8, &R9};
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
void _lea(long long, char *, char *, char *);
void _lea_i(long long, char *, char *);
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

char *reg_to_str(char reg) {
    if (reg < 0 || reg >= MAX_ARGS) {
        return NULL;
    }
    return reg_args[reg]->name;
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

char get_arg_reg(char pos) {
    if (pos < 0 || pos >= MAX_ARGS) {
        return -1;
    }
    return pos;
}

char next_reg() {
    for (char i = 0; i < MAX_ARGS; i++) {
        if (reg_args[i]->taken == 0) {
            reg_args[i]->taken = 1;
            return i;
        }
    }
    return -1;
}

void mark_taken(char reg) {
    if (reg < 0 || reg >= MAX_ARGS) {
        return;
    }
    reg_args[reg]->taken = 1;
}

void free_reg(char reg) {
    if (reg < 0 || reg >= MAX_ARGS) {
        return;
    }
    reg_args[reg]->taken = 0;
}

void free_arg_regs() {
    for (int i = 0; i < MAX_ARGS; i++) {
        reg_args[i]->taken = 0;
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
    free_arg_regs();
    function_count++;
    _ret();
    // printf("\tpopq\t%%%s\n", RBP); // TODO: might be moved to ret
    // printf(".LFE%d:\n", function_count);
    // printf("\t.size\t%s, .-%s\n", name, name);
}

void cmp_leq(char lsrc, char rsrc, char dst) {
    _cmp_cc("le", reg_to_str(rsrc), reg_to_str(lsrc), reg_to_str(dst));
}
void cmp_leq_i(long long val, char src, char dst) {
    _cmp_cc_i("ge", val, reg_to_str(src), reg_to_str(dst));
}

void cmp_geq_i(long long val, char src, char dst) {
    _cmp_cc_i("le", val, reg_to_str(src), reg_to_str(dst));
}

void cmp_dif(char lsrc, char rsrc, char dst) {
    _cmp_cc("ne", reg_to_str(lsrc), reg_to_str(rsrc), reg_to_str(dst));
}
void cmp_dif_i(long long val, char src, char dst) {
    _cmp_cc_i("ne", val, reg_to_str(src), reg_to_str(dst));
}


void mov(char src, char dst) {
    _mov(reg_to_str(src), reg_to_str(dst));
}
void mov_i(long long val, char dst) {
    _mov_i(val, reg_to_str(dst));
}

void and(char src, char src_dst) {
    _and(reg_to_str(src), reg_to_str(src_dst));
}
void and_i(long long val, char src_dst) {
    _and_i(val, reg_to_str(src_dst));
}

void mul(char src, char src_dst) {
    _mul(reg_to_str(src), reg_to_str(src_dst));
}
void mul_i(long long val, char src_dst) {
    _mul_i(val, reg_to_str(src_dst));
}

void add(char src, char src_dst) {
    _add(reg_to_str(src), reg_to_str(src_dst));
}
void add_i(long long val, char src_dst) {
    _add_i(val, reg_to_str(src_dst));
}

void lea(long long val, char lsrc, char rsrc, char dst) {
    _lea(val, reg_to_str(lsrc), reg_to_str(rsrc), reg_to_str(dst));
}
void lea_i(long long val, char src, char dst) {
    _lea_i(val, reg_to_str(src), reg_to_str(dst));
}

void not(char src_dst) {
    _not(reg_to_str(src_dst));
}

void neg(char src_dst) {
    _neg(reg_to_str(src_dst));
}

void drf(char src, char dst) {
    _drf(reg_to_str(src), reg_to_str(dst));
}
void drf_i(long long val, char dst) {
    _drf_i(val, reg_to_str(dst));
}

void ret(char src) {
    _mov(reg_to_str(src), "rax");
}
void ret_i(long long val) {
    _mov_i(val, "rax");
}


void _mov(char *src, char *dst) {
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

void _lea(long long val, char *lsrc, char *rsrc, char *dst) {
    printf("\tleaq\t%lld(%%%s, %%%s), %%%s\n", val, lsrc, rsrc, dst);
}
void _lea_i(long long val, char *src, char *dst) {
    printf("\tleaq\t%lld(%%%s), %%%s\n", val, src, dst);
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
// TODO: this looks very wrong
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
