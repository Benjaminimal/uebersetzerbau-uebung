#include <stdio.h>

extern unsigned char *asmb(unsigned char *s);

int main() {
    unsigned char message1[] = "aaaaaaaabbbbbbbbccccccccddddddd";
    printf("%s\n", message1);
    asmb(message1);
    printf("%s\n", message1);

    unsigned char message2[] = "0{47acef!%. BCEF0aA(m-`uDc6{|}a~c";
    printf("%s\n", message2);
    asmb(message2);
    printf("%s\n", message2);
}
