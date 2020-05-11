#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#define MAX_ARGS 6

void function_start(char *);
void function_end(char *);

void mov(char *, char *);
void mov_i(long long, char *);

void cmp(char *, char *);
void cmp_i(long long, char *);

void cmp_leq(char *, char *, char *);
void cmp_leq_i(long long, char *, char *);
void cmp_geq_i(long long, char *, char *);
void cmp_dif(char *, char *, char *);
void cmp_dif_i(long long, char *, char *);

void leq(char *);

void gt(char *);

void dif(char *);

void and(char *, char *);
void and_i(long long, char *);

void mul(char *, char *);
void mul_i(long long, char *);

void add(char *, char *);
void add_i(long long, char *);

void not(char *);

void neg(char *);

void drf(char *, char *);
void drf_i(long long, char *);

void ret();

void lbl(char *);

char *get_argument_register(int);
char *next_reg();
void free_reg(char *);

#endif /* _TRANSLATOR_H_*/
