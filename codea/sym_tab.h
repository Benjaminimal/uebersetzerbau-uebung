#ifndef _ID_LIST_H_
#define _ID_LIST_H_

typedef enum {
    NAME,
    LABEL,
} sym_type;

typedef struct sym_tab {
    sym_type type;
    char *lexeme;
    char pos;
    struct sym_tab *next;
} sym_tab;

sym_tab *empty_sym_tab();

sym_tab *add_id(sym_tab *, char *, sym_type, int);

int contains_name(sym_tab *, char *);
int contains_label(sym_tab *, char *);
int contains_id(sym_tab *, char *);

sym_tab *get_id(sym_tab *, char *);
sym_tab *get_name(sym_tab *, char *);
sym_tab *get_label(sym_tab *, char *);

#endif /* _ID_LIST_H_ */
