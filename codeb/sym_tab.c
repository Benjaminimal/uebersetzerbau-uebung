#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sym_tab.h"


char *type_to_str(sym_type type) {
    switch (type) {
        case NAME:
            return "NAME";
            break;
        case LABEL:
            return "LABEL";
            break;
        default:
            assert(0);
            break;
    }
}

void print_sym_tab(sym_tab *tab) {
    while (tab != NULL) {
        printf("{%s, %s, %d}", type_to_str(tab->type), tab->lexeme, tab->pos);
        printf(" --> ");
        tab = tab->next;
    }
    printf("{NULL}\n");
}

sym_tab *empty_sym_tab() {
    return NULL;
}


sym_tab *add_id(sym_tab *tab, char *lexeme, sym_type type, int position) {
    sym_tab *head = malloc(sizeof(sym_tab));
    if (head == NULL) {
        return NULL;
    }

    head->type = type;
    head->lexeme = strdup(lexeme);
    head->next = tab;
    head->pos = position;

    return head;
}

int contains_name(sym_tab *tab, char *name) {
    sym_tab *element = get_id(tab, name);
    return element != NULL && element->type == NAME;
}

int contains_label(sym_tab *tab, char *label) {
    sym_tab *element = get_id(tab, label);
    return element != NULL && element->type == LABEL;
}

int contains_id(sym_tab *tab, char *lexeme) {
    return get_id(tab, lexeme) != NULL;
}

sym_tab *get_id(sym_tab *tab, char *lexeme) {
    while (tab != NULL) {
        if (strcmp(tab->lexeme, lexeme) == 0)
            return tab;
        tab = tab->next;
    }

    return NULL;
}

sym_tab *get_name(sym_tab *tab, char *name) {
    sym_tab *needle = get_id(tab, name);
    return needle != NULL && needle->type == NAME ? needle : NULL;
}

sym_tab *get_label(sym_tab *tab, char *label) {
    sym_tab *needle = get_id(tab, label);
    return needle != NULL && needle->type == LABEL ? needle : NULL;
}
