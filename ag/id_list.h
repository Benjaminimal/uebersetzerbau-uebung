#ifndef _ID_LIST_H_
#define _ID_LIST_H_

typedef enum {
    NAME,
    LABEL,
} id_type;

typedef struct id_list {
    id_type type;
    char *lexeme;
    struct id_list *next;
} id_list;

id_list *empty_id_list();

id_list *add_id(id_list *, char *, id_type);

char *get_id(id_list *, char *);

int contains_name(id_list *, char *);
int contains_label(id_list *, char *);
int contains_id(id_list *, char *);

#endif /* _ID_LIST_H_ */
