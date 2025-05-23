#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "x86.h"

#define T_VAL -1
#define F_VAL 0

#define RET_REG_IDX -2

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

void _test_i(long, char *);
void _test_ii(long, long);
void _mov(char *, char *);
void _mov_i(long, char *);
void _mov_d(char *, char *);
void _mov_i_d(long , char *);
void _mov_r_di(char *, long);
void _mov_i_di(long, long);
void _cmp(char *, char *);
void _cmp_i(long, char *);
void _and(char *, char *);
void _and_i(long , char *);
void _mul(char *, char *);
void _mul_i(long, char *);
void _mul_i_r(long, char *, char *);
void _add(char *, char *);
void _add_i(long, char *);
void _lea(long, char *, char *, char *);
void _lea_i(long, char *, char *);
void _not(char *);
void _neg(char *);
void _drf(char *, char *);
void _drf_i(long, char *);
void _ret();
void _jmp(char *);
void _jcc(char *, char *);
void _lbl(char *);
void _cmp_cc(char *, char *, char *, char *);
void _cmp_cc_i(char *, long, char *, char *);

void print_regs() {
    for (int i = 0; i < MAX_ARGS; i++) {
        printf("%s: %d\n", reg_args[i]->name, reg_args[i]->taken);
    }
}

int _num_digits(int n) {
    int cnt = 0;
    while (n != 0) {
        n /= 10;
        cnt++;
    }
    return cnt;
}

char *reg_to_str(char reg) {
    if (reg == RET_REG_IDX) {
        return RAX.name;
    }
    if (reg < 0 || reg >= MAX_ARGS) {
        return NULL;
    }
    return reg_args[reg]->name;
}

char *next_label() {
    char *label = malloc(LABEL_PREFIX_LEN + _num_digits(label_count) + 1);
    if (label == NULL) {
        EXIT_ERR_OOM();
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

char ret_reg() {
    return RET_REG_IDX;
}

char next_reg() {
    for (char i = MAX_ARGS - 1; i >= 0; i--) {
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
    // printf("\tpopq\t%%%s\n", RBP); // TODO: might be moved to ret
    // printf(".LFE%d:\n", function_count);
    // printf("\t.size\t%s, .-%s\n", name, name);
}

void jz(char *label) {
    _jcc("z", label);
}
void jnz(char *label) {
    _jcc("nz", label);
}

void test(char reg) {
    _test_i(1, reg_to_str(reg));
}
void test_i(long val) {
    _test_ii(1, val);
}

void lbl(char *label) {
    _lbl(label);
}

void jmp(char *label) {
    _jmp(label);
}

char cmp_leq(char lsrc, char rsrc, char dst) {
    _cmp_cc("le", reg_to_str(rsrc), reg_to_str(lsrc), reg_to_str(dst));
    return dst;
}
char cmp_leq_i(long val, char src, char dst) {
    _cmp_cc_i("ge", val, reg_to_str(src), reg_to_str(dst));
    return dst;
}

char cmp_geq_i(long val, char src, char dst) {
    _cmp_cc_i("le", val, reg_to_str(src), reg_to_str(dst));
    return dst;
}

char cmp_neq(char lsrc, char rsrc, char dst) {
    _cmp_cc("ne", reg_to_str(lsrc), reg_to_str(rsrc), reg_to_str(dst));
    return dst;
}
char cmp_neq_i(long val, char src, char dst) {
    _cmp_cc_i("ne", val, reg_to_str(src), reg_to_str(dst));
    return dst;
}


char mov(char src, char dst) {
    _mov(reg_to_str(src), reg_to_str(dst));
    return dst;
}
char mov_i(long val, char dst) {
    _mov_i(val, reg_to_str(dst));
    return dst;
}
void mov_d(char src, char dst) {
    _mov_d(reg_to_str(src), reg_to_str(dst));
}
void mov_i_d(long val, char dst) {
    _mov_i_d(val, reg_to_str(dst));
}
void mov_r_di(char src, long addr) {
    _mov_r_di(reg_to_str(src), addr);
}
void mov_i_di(long val, long addr) {
    _mov_i_di(val, addr);
}

void and(char src, char src_dst) {
    _and(reg_to_str(src), reg_to_str(src_dst));
}
void and_i(long val, char src_dst) {
    _and_i(val, reg_to_str(src_dst));
}

void mul(char src, char src_dst) {
    _mul(reg_to_str(src), reg_to_str(src_dst));
}
void mul_i(long val, char src_dst) {
    _mul_i(val, reg_to_str(src_dst));
}
char mul_i_r(long val, char src, char dst) {
    _mul_i_r(val, reg_to_str(src), reg_to_str(dst));
    return dst;
}

void add(char src, char src_dst) {
    _add(reg_to_str(src), reg_to_str(src_dst));
}
void add_i(long val, char src_dst) {
    _add_i(val, reg_to_str(src_dst));
}

char lea(long val, char lsrc, char rsrc, char dst) {
    _lea(val, reg_to_str(lsrc), reg_to_str(rsrc), reg_to_str(dst));
    return dst;
}
char lea_i(long val, char src, char dst) {
    _lea_i(val, reg_to_str(src), reg_to_str(dst));
    return dst;
}

void not(char src_dst) {
    _not(reg_to_str(src_dst));
}

void neg(char src_dst) {
    _neg(reg_to_str(src_dst));
}

char drf(char src, char dst) {
    _drf(reg_to_str(src), reg_to_str(dst));
    return dst;
}
char drf_i(long val, char dst) {
    _drf_i(val, reg_to_str(dst));
    return dst;
}

void ret(char src) {
    _mov(reg_to_str(src), RAX.name);
    _ret();
}
void ret_i(long val) {
    _mov_i(val, RAX.name);
    _ret();
}
void ret_e() {
    _ret();
}

void _mov_d(char *src, char *dst) {
    printf("\tmovq\t%%%s, (%%%s)\n", src, dst);
}
void _mov_i_d(long val, char *dst) {
    printf("\tmovq\t$%ld, (%%%s)\n", val, dst);
}
void _mov_r_di(char *src, long addr) {
    printf("\tmovq\t%%%s, (%ld)\n", src, addr);
}
void _mov_i_di(long val, long addr) {
    printf("\tmovq\t$%ld, (%ld)\n", val, addr);
}
void _mov(char *src, char *dst) {
    printf("\tmovq\t%%%s, %%%s\n", src, dst);
}
void _mov_i(long val, char *dst) {
    printf("\tmovq\t$%ld, %%%s\n", val, dst);
}

void _drf(char *src, char *dst) {
    printf("\tmovq\t(%%%s), %%%s\n", src, dst);
}
void _drf_i(long val, char *dst) {
    printf("\tmovq\t(%ld), %%%s\n", val, dst);
}

void _cmp(char *left, char *right) {
    printf("\tcmpq\t%%%s, %%%s\n", left, right);
}
void _cmp_i(long val, char *dst) {
    printf("\tcmpq\t$%ld, %%%s\n", val, dst);
}

void _and(char *src, char *dst) {
    printf("\tandq\t%%%s, %%%s\n", src, dst);
}
void _and_i(long val, char *dst) {
    printf("\tandq\t$%ld, %%%s\n", val, dst);
}

void _mul(char *src, char *dst) {
    printf("\timulq\t%%%s, %%%s\n", src, dst);
}
void _mul_i(long val, char *dst) {
    printf("\timulq\t$%ld, %%%s\n", val, dst);
}
void _mul_i_r(long val, char *src, char *dst) {
    printf("\timulq\t$%ld, %%%s, %%%s\n", val, src, dst);
}

void _add(char *src, char *dst) {
    printf("\taddq\t%%%s, %%%s\n", src, dst);
}
void _add_i(long val, char *dst) {
    printf("\taddq\t$%ld, %%%s\n", val, dst);
}

void _lea(long val, char *lsrc, char *rsrc, char *dst) {
    printf("\tleaq\t%ld(%%%s, %%%s), %%%s\n", val, lsrc, rsrc, dst);
}
void _lea_i(long val, char *src, char *dst) {
    printf("\tleaq\t%ld(%%%s), %%%s\n", val, src, dst);
}

void _not(char *src_dst) {
    printf("\tnotq\t%%%s\n", src_dst);
}

void _neg(char *src_dst) {
    printf("\tnegq\t%%%s\n", src_dst);
}

void _ret() {
    printf("\tret\n");
}

void _jmp(char *loc) {
    printf("\tjmp\t\t%s\n", loc);
}

void _jcc(char *cond, char *loc) {
    printf("\tj%s\t\t%s\n", cond, loc);
}

void _test_i(long val, char *reg) {
    printf("\ttest\t$%ld,%%%s\n", val, reg);
}
void _test_ii(long left, long right) {
    printf("\ttest\t$%ld,$%ld\n", left, right);
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

void _cmp_cc_i(char *cond, long val, char *src, char *dst) {
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
