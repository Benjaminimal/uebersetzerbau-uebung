    .file   "asma.c"
    .text
    .p2align 4,,15
    .globl  asma
    .type   asma, @function
.LC0:
    .skip 16, 0x9a                      # 'z' + 1 + min_t - 'a'
.LC1:
    .skip 16, 0x1f                      # min_t - 'a'
.LC2:
    .skip 16, 0xe0                      # 'A' - 'a'
asma:
.LFB0:
    .cfi_startproc
    movq    %rdi, %rax                  # prepare return value

    movdqu  (%rdi), %xmm0               # copy first 16 chars of string to vector register

    movdqa  .LC0(%rip), %xmm1           # set up ('z' + 1 + min_t - 'a') vector
    movdqa  .LC1(%rip), %xmm2           # set up (c + min_t - 'a') vector

    paddb   %xmm0, %xmm2                # compute (c + min_t - 'a')
    pcmpgtb %xmm2, %xmm1                # generate ('z' + 1 + min_t - 'a') > (c + min_t - 'a') bit mask
    pand    .LC2(%rip), %xmm1           # sieve ('A' - 'a') with bit mask
    paddb   %xmm1, %xmm0                # perform conditional computation of (c + ('A' - 'a'))

    movdqu  %xmm0, (%rax)               # write result

    rep ret                             # return
    .cfi_endproc
.LFE0:
    .size   asma, .-asma
    .ident  "GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
    .section    .note.GNU-stack,"",@progbits
