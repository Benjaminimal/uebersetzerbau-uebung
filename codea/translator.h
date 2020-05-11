#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#define MAX_ARGS 6

void function_start(char *);
void function_end(char *);

char *get_argument_register(int);
char *next_reg();
void free_reg(char *);

void cmp_leq(char *, char *, char *);
void cmp_leq_i(long long, char *, char *);
void cmp_geq_i(long long, char *, char *);
void cmp_dif(char *, char *, char *);
void cmp_dif_i(long long, char *, char *);



void _mov(char *, char *);
void _mov_i(long long, char *);

void _and(char *, char *);

void _mul(char *, char *);

void _add(char *, char *);

void _not(char *);

void _neg(char *);

void _drf(char *, char *);
void _drf_i(long long, char *);

void _ret();

#endif /* _TRANSLATOR_H_*/
