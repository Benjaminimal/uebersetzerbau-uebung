    .file   "asmb.c"
    .text
    .p2align 4,,15
    .globl  asmb
    .type   asmb, @function
.LC0:
    .skip 16, 0x9a                      # 'z' + 1 + min_t - 'a'
.LC1:
    .skip 16, 0x1f                      # min_t - 'a'
.LC2:
    .skip 16, 0xe0                      # 'A' - 'a'
.LC3:
    .skip 16, 0x00                      # bounds check
asmb:
.LFB0:
    .cfi_startproc
    movq        %rdi, %rax          # prepare return value

    movdqa      .LC1(%rip), %xmm15  # set up (c + min_t - 'a') vector

.L2:

    movdqu      (%rdi), %xmm0       # copy 16 chars of string to vector register

    paddb       %xmm15, %xmm0       # compute (c + min_t - 'a')
    movdqa      .LC0(%rip), %xmm1   # copy ('z' + 1 + min_t - 'a') to working vector
    pcmpgtb     %xmm0, %xmm1        # generate ('z' + 1 + min_t - 'a') > (c + min_t - 'a') bit mask
    pand        .LC2(%rip), %xmm1   # sieve ('A' - 'a') with bit mask
    movdqu      (%rdi), %xmm0       # copy 16 chars of string to vector register
    paddb       %xmm1, %xmm0        # perform conditional computation of (c + ('A' - 'a'))

    movdqu      %xmm0, (%rdi)       # write result
    addq        $0x10, %rdi         # increment loop counter/pointer

    pcmpeqb     .LC3(%rip), %xmm0   # look for \0
    pmovmskb    %xmm0, %esi         # create bit mask of comparison
    cmpw        $0, %si             # check for \0
    je          .L2                 # keep on going

    rep ret                         # return
    .cfi_endproc
.LFE0:
    .size       asmb, .-asmb
    .ident      "GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
    .section    .note.GNU-stack,"",@progbits
