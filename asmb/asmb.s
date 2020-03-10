    .file   "asmb.c"
    .text
    .globl  asmb
    .type   asmb, @function
asmb:
.LFB0:
    .cfi_startproc
    movq        %rdi, %rax                  # prepare return value

    movq        $0x9a9a9a9a9a9a9a9a, %rcx   # set up ('z' + 1 + min_t - 'a') vector
    push        %rcx                        # -0x8(%rbp) 
    push        %rcx
    movdqu      (%rsp), %xmm15

    movq        $0x1f1f1f1f1f1f1f1f, %rcx   # set up (c + min_t - 'a') vector
    push        %rcx                        # -0x10(%rbp)
    push        %rcx
    movdqu      (%rsp), %xmm14

    movq        $0xe0e0e0e0e0e0e0e0, %rcx   # set up ('A' - 'a') vector
    push        %rcx                        # -0x18(%rbp)
    push        %rcx
    movdqu      (%rsp), %xmm13

    movq        $0x0000000000000000, %rcx   # set up bounds check vector
    push        %rcx                        # -0x20(%rbp)
    push        %rcx
    movdqu      (%rsp), %xmm12

    movq        %rdi, %rdx                  # initialize loop counter/pointer

.L2:

    movdqu      (%rdx), %xmm0               # copy 16 chars of string to vector register

    paddb       %xmm14, %xmm0               # compute (c + min_t - 'a')
    movdqa      %xmm15, %xmm1               # copy ('z' + 1 + min_t - 'a') to working vector
    pcmpgtb     %xmm0, %xmm1                # generate ('z' + 1 + min_t - 'a') > (c + min_t - 'a') bit mask
    pand        %xmm13, %xmm1               # sieve ('A' - 'a') with bit mask
    movdqu      (%rdx), %xmm3               # copy 16 chars of string to vector register
    paddb       %xmm1, %xmm3                # perform conditional computation of (c + ('A' - 'a'))

    movdqu      %xmm3, (%rdx)               # write result
    addq        $0x10, %rdx                 # increment loop counter/pointer

    pcmpeqb     %xmm12, %xmm3               # look for \0
    pmovmskb    %xmm3, %esi                 # create bit mask of comparison
    cmpw        $0, %si                     # check for \0
    je          .L2                         # keep on going

    sub         $-0x40, %rsp                # restore stack pointer

    rep ret                                 # return
    .cfi_endproc
.LFE0:
    .size       asmb, .-asmb
    .ident      "GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
    .section    .note.GNU-stack,"",@progbits
