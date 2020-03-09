#include <stdio.h>

extern unsigned char *asmb(unsigned char *s);

int main() {
    unsigned char message[] = "0{47acef!%. BCEF0aA(m-`uDc6{|}a~c";
    printf("%s\n", message);
    asmb(message);
    printf("%s\n", message);
}
