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

int num_digits(int n) {
    int cnt = 0;
    while (n != 0) {
        n /= 10;
        cnt++;
    }
    return cnt;
}

char *next_label() {
    char *label = malloc(LABEL_PREFIX_LEN + num_digits(label_count + 1));
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
    lbl(name);
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

// TODO: rename to mov and mov_i
void mov(char *src, char *dst) {
    // TODO: check for reduntant move
    printf("\tmovq\t%%%s, %%%s\n", src, dst);
}

void mov_i(long long val, char *dst) {
    printf("\tmovq\t$%lld, %%%s\n", val, dst);
}

void cmp(char *left, char *right) {
    printf("\tcmpq\t%%%s, %%%s\n", left, right);
}
void cmp_i(long long val, char *dst) {
    printf("\tcmpq\t$%lld, %%%s\n", val, dst);
}

void leq(char *dst) {
    printf("\tsetle\t%%%s\n", get_reg(dst)->b_name);
}

void gt(char *dst) {
    printf("\tsetg\t%%%s\n", get_reg(dst)->b_name);
}

void dif(char *dst) {
    printf("\tsetne\t%%%s\n", get_reg(dst)->b_name);
}

void and(char *src, char *dst) {
    printf("\tandq\t%%%s, %%%s\n", src, dst);
}
void and_i(long long val, char *dst) {
    printf("\tandq\t$%lld, %%%s\n", val, dst);
}

void mul(char *src, char *dst) {
    printf("\timulq\t%%%s, %%%s\n", src, dst);
}
void mul_i(long long val, char *dst) {
    printf("\timulq\t$%lld, %%%s\n", val, dst);
}


void add(char *src, char *dst) {
    printf("\taddq\t%%%s, %%%s\n", src, dst);
}
void add_i(long long val, char *dst) {
    printf("\taddq\t$%lld, %%%s\n", val, dst);
}

void not(char *src_dst) {
    printf("\tnotq\t%%%s\n", src_dst);
}

void neg(char *src_dst) {
    printf("\tnegq\t%%%s\n", src_dst);
}

void drf(char *src, char *dst) {
    printf("\tmovq\t(%%%s), %%%s\n", src, dst);
}
void drf_i(long long val, char *dst) {
    printf("\tmovq\t($%lld), %%%s\n", val, dst);
}

void ret() {
    printf("\tret\n");
}

void jmp(char *loc) {
    printf("\tjmp\t%s\n", loc);
}

void jcc(char *cond, char *loc) {
    printf("\tj%s\t%s\n", cond, loc);
}

void lbl(char *label) {
    printf("%s:\n", label);
}

void cmp_cc(char *cond, char *lsrc, char *rsrc, char *dst) {
    char *label_true = next_label();
    char *label_end = next_label();

    cmp(lsrc, rsrc);
    jcc(cond, label_true);
    mov_i(0, dst);
    jmp(label_end);
    lbl(label_true);
    mov_i(-1, dst);
    lbl(label_end);

    free(label_true);
    free(label_end);
}

void cmp_cc_i(char *cond, long long val, char *src, char *dst) {
    char *label_true = next_label();
    char *label_end = next_label();

    cmp_i(val, src);
    jcc(cond, label_true);
    mov_i(0, dst);
    jmp(label_end);
    lbl(label_true);
    mov_i(-1, dst);
    lbl(label_end);

    free(label_true);
    free(label_end);
}

void cmp_leq(char *lsrc, char *rsrc, char *dst) {
    cmp_cc("le", rsrc, lsrc, dst);
}
void cmp_leq_i(long long val, char *src, char *dst) {
    cmp_cc_i("ge", val, src, dst);
}

void cmp_geq_i(long long val, char *src, char *dst) {
    cmp_cc_i("le", val, src, dst);
}

void cmp_dif(char *lsrc, char *rsrc, char *dst) {
    cmp_cc("ne", lsrc, rsrc, dst);
}
void cmp_dif_i(long long val, char *src, char *dst) {
    cmp_cc_i("ne", val, src, dst);
}
