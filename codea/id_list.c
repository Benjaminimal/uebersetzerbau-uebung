#include <stdlib.h>
#include <string.h>
#include "id_list.h"

id_list *empty_id_list() {
    return NULL;
}


id_list *add_id(id_list *list, char *lexeme, id_type type, int position) {
    id_list *head = malloc(sizeof(id_list));
    if (head == NULL) {
        return NULL;
    }

    head->type = type;
    head->lexeme = strdup(lexeme);
    head->next = list;
    head->parameter_position = position;

    return head;
}

int contains_name(id_list *list, char *lexeme) {
    id_list *element = get_id(list, lexeme);
    return element != NULL && element->type == NAME;
}

int contains_label(id_list *list, char *lexeme) {
    id_list *element = get_id(list, lexeme);
    return element != NULL && element->type == LABEL;
}

int contains_id(id_list *list, char *lexeme) {
    return get_id(list, lexeme) != NULL;
}

id_list *get_id(id_list *list, char *lexeme) {
    while (list != NULL) {
        if (strcmp(list->lexeme, lexeme) == 0)
            return list;
        list = list->next;
    }

    return NULL;
}
