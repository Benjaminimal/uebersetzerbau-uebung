#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#define MAX_ARGS 6

void print_regs();

void function_start(char *name);
void function_end(char *name);

char get_arg_reg(char num);
char next_reg();
char ret_reg();
void mark_taken(char reg);
void free_reg(char reg);

void not(char src_dst);

void neg(char src_dst);

void ret(char src);
void ret_i(long val);

char mov(char src, char dst);
char mov_i(long val, char dst);

void and(char src, char src_dst);
void and_i(long val, char src_dst);

void mul(char src, char src_dst);
void mul_i(long val, char src_dst);

void add(char src, char src_dst);
void add_i(long val, char src_dst);

char drf(char src, char dst);
char drf_i(long val, char dst);

char lea(long val, char lsrc, char rsrc, char dst);
char lea_i(long val, char src, char dst);

char cmp_leq(char lsrc, char rsrc, char dst);
char cmp_leq_i(long val, char src, char dst);
char cmp_geq_i(long val, char src, char dst);
char cmp_dif(char lsrc, char rsrc, char dst);
char cmp_dif_i(long val, char src, char dst);

#endif /* _TRANSLATOR_H_*/
