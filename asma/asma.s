    .file   "asma.c"
    .text
    .globl  asma
    .type   asma, @function
asma:
.LFB0:
    .cfi_startproc
    movq    %rdi, %rax                  # prepare return value

    movdqu  (%rdi), %xmm0               # copy first 16 chars of string to vector register

    movq    $0x9a9a9a9a9a9a9a9a, %rcx   # set up ('z' + 1 + min_t - 'a') vector
    push    %rcx
    push    %rcx
    movdqu  (%rsp), %xmm1

    movq    $0x1f1f1f1f1f1f1f1f, %rcx   # set up (c + min_t - 'a') vector
    push    %rcx
    push    %rcx
    movdqu  (%rsp), %xmm2

    movq    $0xe0e0e0e0e0e0e0e0, %rcx   # set up ('A' - 'a') vector
    push    %rcx
    push    %rcx
    movdqu  (%rsp), %xmm3

    paddb   %xmm0, %xmm2                # compute (c + min_t - 'a')
    pcmpgtb %xmm2, %xmm1                # generate ('z' + 1 + min_t - 'a') > (c + min_t - 'a') bit mask
    pand    %xmm3, %xmm1                # sieve ('A' - 'a') with bit mask
    paddb   %xmm1, %xmm0                # perform conditional computation of (c + ('A' - 'a'))

    movdqu  %xmm0, (%rax)               # write result

    sub     $-48, %rsp                  # restore stack pointer

    rep ret                             # return
    .cfi_endproc
.LFE0:
    .size   asma, .-asma
    .ident  "GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
    .section    .note.GNU-stack,"",@progbits
