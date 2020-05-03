#include <stdio.h>
#include "translator.h"


int function_count = 0;


void tr_function_start(char *name) {
    if (function_count == 0) {
        printf("\t.text\n");
    }
    printf("\t.globl\t%s\n", name);
    printf("\t.type\t%s, @function\n", name);
    printf("%s:\n", name);
    printf(".LFB%d:\n", function_count);
}

void tr_function_end(char *name) {
    printf(".LFE%d:\n", function_count);
    printf("\t.size\t%s, .-%s\n", name, name);
    function_count++;
}
