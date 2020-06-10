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

#define _offset(position) -(position + 1) * QWORD_SIZE

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

void _push(char *);
void _test_i_r(long, char *);
void _test_i_m(long, long);
void _test_i_i(long, long);
void _mov_r_r(char *, char *);
void _mov_i_r(long, char *);
void _mov_d_r(char *, char *);
void _mov_i_d(long , char *);
void _mov_r_di(char *, long);
void _mov_i_di(long, long);
void _mov_r_m(char *, long, char *);
void _mov_i_m(long, long, char *);
void _mov_m_r(long, char *, char *);
void _cmp_r_r(char *, char *);
void _cmp_m_r(long, char *, char *);
void _cmp_i_r(long, char *);
void _cmp_i_m(long, long, char *);
void _and_r_r(char *, char *);
void _and_m_r(long, char *, char *);
void _and_i_r(long , char *);
void _mul_r_r(char *, char *);
void _mul_i_r(long, char *);
void _mul_m_r(long, char *, char *);
void _mul_i_r_r(long, char *, char *);
void _mul_i_m_r(long, long, char *, char *);
void _add_r_r(char *, char *);
void _add_m_r(long, char *, char *);
void _add_i_r(long, char *);
void _sub_i(long, char *);
void _lea_i_r_r_r(long, char *, char *, char *);
void _lea_i_r_r(long, char *, char *);
void _not_r(char *);
void _neg_r(char *);
void _drf_r_r(char *, char *);
void _drf_i_r(long, char *);
void _leave();
void _ret();
void _jmp(char *);
void _jcc(char *, char *);
void _lbl(char *);
void _cmp_cc_r_r_r(char *, char *, char *, char *);
void _cmp_cc_m_r_r(char *, long, char *, char *, char *);
void _cmp_cc_i_r_r(char *, long, char *, char *);
void _cmp_cc_i_m_r(char *, long, long, char *, char *);

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

void function_start(char *name, int varcount) {
    if (function_count == 0) {
        printf("\t.text\n");
    }
    printf("\t.globl\t%s\n", name);
    printf("\t.type\t%s, @function\n", name);
    _lbl(name);
    _push(RBP.name);
    _mov_r_r(RSP.name, RBP.name);
    _sub_i(varcount * QWORD_SIZE, RSP.name);
}

void function_end() {
    _leave();
    _ret();
    free_arg_regs();
    function_count++;
}

void push(char reg) {
    _push(reg_to_str(reg));
}

void jz(char *label) {
    _jcc("z", label);
}
void jnz(char *label) {
    _jcc("nz", label);
}

void test_r(char reg) {
    _test_i_r(1, reg_to_str(reg));
}
void test_m(int position) {
    _test_i_m(1, _offset(position));
}
void test_i(long val) {
    _test_i_i(1, val);
}

void lbl(char *label) {
    _lbl(label);
}

void jmp(char *label) {
    _jmp(label);
}

char cmp_leq_r_r_r(char lsrc, char rsrc, char dst) {
    _cmp_cc_r_r_r("le", reg_to_str(rsrc), reg_to_str(lsrc), reg_to_str(dst));
    return dst;
}

char cmp_leq_m_r_r(int lposition, char src, char dst) {
    _cmp_cc_m_r_r("ge", _offset(lposition), RBP.name, reg_to_str(src), reg_to_str(dst));
    return dst;
}

char cmp_geq_m_r_r(int lposition, char src, char dst) {
    _cmp_cc_m_r_r("le", _offset(lposition), RBP.name, reg_to_str(src), reg_to_str(dst));
    return dst;
}

char cmp_leq_i_r_r(long val, char src, char dst) {
    _cmp_cc_i_r_r("ge", val, reg_to_str(src), reg_to_str(dst));
    return dst;
}

char cmp_geq_i_r_r(long val, char src, char dst) {
    _cmp_cc_i_r_r("le", val, reg_to_str(src), reg_to_str(dst));
    return dst;
}

char cmp_leq_i_m_r(long val, int position, char dst) {
    _cmp_cc_i_m_r("ge", val, _offset(position), RBP.name, reg_to_str(dst));
    return dst;
}

char cmp_geq_i_m_r(long val, int position, char dst) {
    _cmp_cc_i_m_r("le", val, _offset(position), RBP.name, reg_to_str(dst));
    return dst;
}

char cmp_neq_r_r_r(char lsrc, char rsrc, char dst) {
    _cmp_cc_r_r_r("ne", reg_to_str(lsrc), reg_to_str(rsrc), reg_to_str(dst));
    return dst;
}
char cmp_neq_m_r_r(int position, char src, char dst) {
    _cmp_cc_m_r_r("ne", _offset(position), RBP.name, reg_to_str(src), reg_to_str(dst));
    return dst;
}
char cmp_neq_i_r_r(long val, char src, char dst) {
    _cmp_cc_i_r_r("ne", val, reg_to_str(src), reg_to_str(dst));
    return dst;
}
char cmp_neq_i_m_r(long val, int position, char dst) {
    _cmp_cc_i_m_r("ne", val, _offset(position), RBP.name, reg_to_str(dst));
    return dst;
}


char mov_r_r(char src, char dst) {
    _mov_r_r(reg_to_str(src), reg_to_str(dst));
    return dst;
}
char mov_i_r(long val, char dst) {
    _mov_i_r(val, reg_to_str(dst));
    return dst;
}
char mov_d_r(char src, char dst) {
    _mov_d_r(reg_to_str(src), reg_to_str(dst));
    return dst;
}
char mov_i_d(long val, char dst) {
    _mov_i_d(val, reg_to_str(dst));
    return dst;
}
void mov_r_di(char src, long addr) {
    _mov_r_di(reg_to_str(src), addr);
}
void mov_i_di(long val, long addr) {
    _mov_i_di(val, addr);
}
int mov_r_m(char src, int position) {
    _mov_r_m(reg_to_str(src), _offset(position), RBP.name);
    return position;
}
int mov_i_m(long val, int position) {
    _mov_i_m(val, _offset(position), RBP.name);
    return position;
}
char mov_m_r(int position, char dst) {
    _mov_m_r(_offset(position), RBP.name, reg_to_str(dst));
    return dst;
}

char and_r_r(char src, char src_dst) {
    _and_r_r(reg_to_str(src), reg_to_str(src_dst));
    return src_dst;
}
char and_m_r(int position, char src_dst) {
    _and_m_r(_offset(position), RBP.name, reg_to_str(src_dst));
    return src_dst;
}
char and_i_r(long val, char src_dst) {
    _and_i_r(val, reg_to_str(src_dst));
    return src_dst;
}

char mul_r_r(char src, char src_dst) {
    _mul_r_r(reg_to_str(src), reg_to_str(src_dst));
    return src_dst;
}
char mul_m_r(int position, char src_dst) {
    _mul_m_r(_offset(position), RBP.name, reg_to_str(src_dst));
    return src_dst;
}
char mul_i_r(long val, char src_dst) {
    _mul_i_r(val, reg_to_str(src_dst));
    return src_dst;
}
char mul_i_r_r(long val, char src, char dst) {
    _mul_i_r_r(val, reg_to_str(src), reg_to_str(dst));
    return dst;
}
char mul_i_m_r(long val, int position, char dst) {
    _mul_i_m_r(val, _offset(position), RBP.name, reg_to_str(dst));
    return dst;
}

char add_r_r(char src, char src_dst) {
    _add_r_r(reg_to_str(src), reg_to_str(src_dst));
    return src_dst;
}
char add_m_r(int position, char src_dst) {
    _add_m_r(_offset(position), RBP.name, reg_to_str(src_dst));
    return src_dst;
}
char add_i_r(long val, char src_dst) {
    _add_i_r(val, reg_to_str(src_dst));
    return src_dst;
}

char lea_i_r_r_r(long val, char lsrc, char rsrc, char dst) {
    _lea_i_r_r_r(val, reg_to_str(lsrc), reg_to_str(rsrc), reg_to_str(dst));
    return dst;
}
char lea_i_r_r(long val, char src, char dst) {
    _lea_i_r_r(val, reg_to_str(src), reg_to_str(dst));
    return dst;
}

char not_r(char src_dst) {
    _not_r(reg_to_str(src_dst));
    return src_dst;
}

char neg_r(char src_dst) {
    _neg_r(reg_to_str(src_dst));
    return src_dst;
}

char drf_r_r(char src, char dst) {
    _drf_r_r(reg_to_str(src), reg_to_str(dst));
    return dst;
}
char drf_i_r(long val, char dst) {
    _drf_i_r(val, reg_to_str(dst));
    return dst;
}

// TODO: this _ret needs to jump to the end
void ret_r(char src) {
    _mov_r_r(reg_to_str(src), RAX.name);
    _leave();
    _ret();
}
void ret_m(int position) {
    _mov_m_r(_offset(position), RBP.name, RAX.name);
    _leave();
    _ret();
}
void ret_i(long val) {
    _mov_i_r(val, RAX.name);
    _leave();
    _ret();
}
void ret_e() {
    _leave();
    _ret();
}

void _push(char *reg) {
    printf("\tpushq\t%%%s\n", reg);
}

void _mov_d_r(char *src, char *dst) {
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
void _mov_r_r(char *src, char *dst) {
    printf("\tmovq\t%%%s, %%%s\n", src, dst);
}
void _mov_i_r(long val, char *dst) {
    printf("\tmovq\t$%ld, %%%s\n", val, dst);
}
void _mov_r_m(char *src, long offset, char *dst) {
    printf("\tmovq\t%%%s, %ld(%%%s)\n", src, offset, dst);
}
void _mov_i_m(long val, long offset, char *dst) {
    printf("\tmovq\t$%ld, %ld(%%%s)\n", val, offset, dst);
}
void _mov_m_r(long offset, char *src, char *dst) {
    printf("\tmovq\t%ld(%%%s), %%%s\n", offset, src, dst);
}

void _drf_r_r(char *src, char *dst) {
    printf("\tmovq\t(%%%s), %%%s\n", src, dst);
}
void _drf_i_r(long val, char *dst) {
    printf("\tmovq\t(%ld), %%%s\n", val, dst);
}

void _cmp_r_r(char *lsrc, char *rsrc) {
    printf("\tcmpq\t%%%s, %%%s\n", lsrc, rsrc);
}
void _cmp_m_r(long offset, char *lsrc, char *rsrc) {
    printf("\tcmpq\t%ld(%%%s), %%%s\n", offset, lsrc, rsrc);
}
void _cmp_i_m(long val, long offset, char *src) {
    printf("\tcmpq\t$%ld, %ld(%%%s)\n", val, offset, src);
}
void _cmp_i_r(long val, char *src) {
    printf("\tcmpq\t$%ld, %%%s\n", val, src);
}

void _and_r_r(char *src, char *src_dst) {
    printf("\tandq\t%%%s, %%%s\n", src, src_dst);
}
void _and_m_r(long offset, char *src, char *src_dst) {
    printf("\tandq\t%ld(%%%s), %%%s\n", offset, src, src_dst);
}
void _and_i_r(long val, char *src_dst) {
    printf("\tandq\t$%ld, %%%s\n", val, src_dst);
}

void _mul_r_r(char *src, char *src_dst) {
    printf("\timulq\t%%%s, %%%s\n", src, src_dst);
}
void _mul_i_r(long val, char *src_dst) {
    printf("\timulq\t$%ld, %%%s\n", val, src_dst);
}
void _mul_m_r(long offset, char *src, char *src_dst) {
    printf("\timulq\t%ld(%%%s), %%%s\n", offset, src, src_dst);
}
void _mul_i_r_r(long val, char *src, char *src_dst) {
    printf("\timulq\t$%ld, %%%s, %%%s\n", val, src, src_dst);
}
void _mul_i_m_r(long val, long offset, char *src, char *src_dst) {
    printf("\timulq\t$%ld, %ld(%%%s), %%%s\n", val, offset, src, src_dst);
}

void _add_r_r(char *src, char *src_dst) {
    printf("\taddq\t%%%s, %%%s\n", src, src_dst);
}
void _add_m_r(long offset, char *src, char *src_dst) {
    printf("\taddq\t%ld(%%%s), %%%s\n", offset, src, src_dst);
}
void _add_i_r(long val, char *src_dst) {
    printf("\taddq\t$%ld, %%%s\n", val, src_dst);
}

void _sub_i(long val, char *src_dst) {
    printf("\tsubq\t$%ld, %%%s\n", val, src_dst);
}

void _lea_i_r_r_r(long val, char *lsrc, char *rsrc, char *dst) {
    printf("\tleaq\t%ld(%%%s, %%%s), %%%s\n", val, lsrc, rsrc, dst);
}
void _lea_i_r_r(long val, char *src, char *dst) {
    printf("\tleaq\t%ld(%%%s), %%%s\n", val, src, dst);
}

void _not_r(char *src_dst) {
    printf("\tnotq\t%%%s\n", src_dst);
}

void _neg_r(char *src_dst) {
    printf("\tnegq\t%%%s\n", src_dst);
}

void _leave() {
    printf("\tleave\n");
}

void _ret() {
    printf("\tret\n");
}

void _jmp(char *loc) {
    printf("\tjmp\t\t%s\n", loc);
}

void _jcc(char *cond, char *loc) {
    printf("\tj%s\t%s\n", cond, loc);
}

void _test_i_r(long val, char *reg) {
    printf("\ttestq\t$%ld, %%%s\n", val, reg);
}
void _test_i_m(long val, long offset) {
    printf("\ttestq\t$%ld, %ld(%%%s)\n", val, offset, RBP.name);
}
void _test_i_i(long left, long right) {
    printf("\ttestq\t$%ld, $%ld\n", left, right);
}

void _lbl(char *label) {
    printf("%s:\n", label);
}

void _cmp_post(char *cond, char *dst) {
    char *label_true = next_label();
    char *label_end = next_label();

    _jcc(cond, label_true);
    _mov_i_r(0, dst);
    _jmp(label_end);
    _lbl(label_true);
    _mov_i_r(-1, dst);
    _lbl(label_end);

    free(label_true);
    free(label_end);
}

void _cmp_cc_r_r_r(char *cond, char *lsrc, char *rsrc, char *dst) {
    _cmp_r_r(lsrc, rsrc);
    _cmp_post(cond, dst);
}

void _cmp_cc_m_r_r(char *cond, long offset, char *lsrc, char *rsrc, char *dst) {
    _cmp_m_r(offset, lsrc, rsrc);
    _cmp_post(cond, dst);
}

void _cmp_cc_i_r_r(char *cond, long val, char *src, char *dst) {
    _cmp_i_r(val, src);
    _cmp_post(cond, dst);
}

void _cmp_cc_i_m_r(char *cond, long val, long offset, char *src, char *dst) {
    _cmp_i_m(val, offset, src);
    _cmp_post(cond, dst);
}
