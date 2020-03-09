#include <stdio.h>

extern unsigned char *asma(unsigned char *s);

int main() {
    unsigned char message[] = "0aA(m-`uDc6{|}0247acef!%. BCEF";
    printf("%s\n", message);
    asma(message);
    printf("%s\n", message);
}
