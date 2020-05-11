#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#define MAX_ARGS 6

void function_start(char *);
void function_end(char *);

char *get_argument_register(int);
char *next_reg();
void free_reg(char *);

void not(char *, char *);

void neg(char *, char *);

void ret(char *);
void ret_i(long long);

void and(char *, char*, char*);
void and_i(long long, char*, char*);

void mul(char *, char *, char *);
void mul_i(long long, char *, char *);

void add(char *, char *, char *);
void add_i(long long, char *, char *);

void drf(char *, char *);
void drf_i(long long, char *);

void cmp_leq(char *, char *, char *);
void cmp_leq_i(long long, char *, char *);
void cmp_geq_i(long long, char *, char *);
void cmp_dif(char *, char *, char *);
void cmp_dif_i(long long, char *, char *);

#endif /* _TRANSLATOR_H_*/
