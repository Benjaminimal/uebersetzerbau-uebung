#include <stdlib.h>
#include <string.h>
#include "id_list.h"

int _contains_id(id_list *, char *, id_type);

id_list *empty_id_list() {
    return NULL;
}


id_list *add_id(id_list *list, char *lexeme, id_type type) {
    id_list *head = malloc(sizeof(id_list));
    if (head == NULL) {
        return NULL;
    }

    head->type = type;
    head->lexeme = strdup(lexeme);
    head->next = list;

    return head;
}

char *get_id(id_list *list, char *needle) {
    while (list != NULL) {
        if (strcmp(list->lexeme, needle) == 0)
            return list->lexeme;
    }

    return NULL;
}


int contains_name(id_list *list, char *lexeme) {
    return _contains_id(list, lexeme, NAME);
}

int contains_label(id_list *list, char *lexeme) {
    return _contains_id(list, lexeme, LABEL);
}

int contains_id(id_list *list, char *lexeme) {
    while (list != NULL) {
        if (strcmp(list->lexeme, lexeme) == 0)
            return 1;
        list = list->next;
    }

    return 0;
}

int _contains_id(id_list *list, char *lexeme, id_type type) {
    while (list != NULL) {
        if (list->type == type && strcmp(list->lexeme, lexeme) == 0)
            return 1;
        list = list->next;
    }

    return 0;
}
