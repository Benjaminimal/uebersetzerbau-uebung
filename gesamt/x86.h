#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#define MAX_ARGS 6
#define MAX_REGS 6
#define QWORD_SIZE 8

void print_regs();

void function_start(char *name, int varcount);
void function_end();

char get_arg_reg(char num);
char next_reg();
char ret_reg();
char *next_label();
void mark_taken(char reg);
void free_reg(char reg);

void push(char reg);

void lbl(char *label);
void jmp(char *label);
void jz(char *label);
void jnz(char *label);

char not_r(char src_dst);

char neg_r(char src_dst);

void ret_r(char src);
void ret_m(int position);
void ret_i(long val);
void ret_e();

void test_r(char reg);
void test_m(int position);
void test_i(long val);

char mov_r_r(char src, char dst);
char mov_i_r(long val, char dst);
char mov_d_r(char src, char dst);
char mov_i_d(long val, char dst);
void mov_r_di(char src, long addr);
void mov_i_di(long val, long addr);
int mov_r_m(char src, int position);
int mov_i_m(long val, int position);
char mov_m_r(int position, char dst);

char and_r_r(char src, char src_dst);
char and_m_r(int position, char src_dst);
char and_i_r(long val, char src_dst);

char mul_r_r(char src, char src_dst);
char mul_m_r(int position, char src_dst);
char mul_i_r(long val, char src_dst);
char mul_i_r_r(long val, char src, char dst);
char mul_i_m_r(long val, int position, char dst);

char add_r_r(char src, char src_dst);
char add_m_r(int position, char src_dst);
char add_i_r(long val, char src_dst);

char drf_r_r(char src, char dst);
char drf_i_r(long val, char dst);

char lea_i_r_r_r(long val, char lsrc, char rsrc, char dst);
char lea_i_r_r(long val, char src, char dst);

char cmp_leq_r_r_r(char lsrc, char rsrc, char dst);

char cmp_leq_m_r_r(int lposition, char src, char dst);
char cmp_geq_m_r_r(int lposition, char src, char dst);

char cmp_leq_i_r_r(long val, char src, char dst);
char cmp_geq_i_r_r(long val, char src, char dst);
char cmp_leq_i_m_r(long val, int position, char dst);
char cmp_geq_i_m_r(long val, int position, char dst);

char cmp_neq_r_r_r(char lsrc, char rsrc, char dst);
char cmp_neq_m_r_r(int position, char src, char dst);
char cmp_neq_i_r_r(long val, char src, char dst);
char cmp_neq_i_m_r(long val, int position, char dst);

#endif /* _TRANSLATOR_H_*/
