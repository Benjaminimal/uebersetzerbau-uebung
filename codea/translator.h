#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#define MAX_ARGS 6

void function_start(char *name);
void function_end(char *name);

char get_arg_reg(char num);
char next_reg();
void mark_taken(char reg);
void free_reg(char reg);

void not(char src_dst);

void neg(char src_dst);

void ret(char src);
void ret_i(long long val);

void mov(char src, char dst);
void mov_i(long long val, char dst);

void and(char src, char src_dst);
void and_i(long long val, char src_dst);

void mul(char src, char src_dst);
void mul_i(long long val, char src_dst);

void add(char src, char src_dst);
void add_i(long long val, char src_dst);

void drf(char src, char dst);
void drf_i(long long val, char dst);

void lea(long long val, char lsrc, char rsrc, char dst);
void lea_i(long long val, char src, char dst);

void cmp_leq(char lsrc, char rsrc, char dst);
void cmp_leq_i(long long val, char src, char dst);
void cmp_geq_i(long long val, char src, char dst);
void cmp_dif(char lsrc, char rsrc, char dst);
void cmp_dif_i(long long val, char src, char dst);

#endif /* _TRANSLATOR_H_*/
