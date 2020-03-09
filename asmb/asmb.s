    .file   "asmb.c"
    .text
    .globl  asmb
    .type   asmb, @function
asmb:
.LFB0:
    .cfi_startproc
    movq        %rdi, %rax                  # prepare return value

    movq        $0x9a9a9a9a9a9a9a9a, %rcx   # set up ('z' + 1 + min_t - 'a') vector
    push        %rcx                        # padding for 128-bit-alignment
    push        %rcx                        # 0x30(%rsp)
    push        %rcx

    movq        $0x1f1f1f1f1f1f1f1f, %rcx   # set up (c + min_t - 'a') vector
    push        %rcx                        # 0x20(%rsp)
    push        %rcx

    movq        $0xe0e0e0e0e0e0e0e0, %rcx   # set up ('A' - 'a') vector
    push        %rcx                        # 0x10(%rsp)
    push        %rcx

    movq        $0x0000000000000000, %rcx   # set up bounds check vector
    push        %rcx                        # 0x00(%rsp)
    push        %rcx

.L2:

    movdqa      (%rdi), %xmm1               # copy 16 chars of string to vector register

    paddb       0x20(%rsp), %xmm1           # compute (c + min_t - 'a')
    movdqa      0x30(%rsp), %xmm0           # copy ('z' + 1 + min_t - 'a') to working vector
    pcmpgtb     %xmm1, %xmm0                # generate ('z' + 1 + min_t - 'a') > (c + min_t - 'a') bit mask
    pand        0x10(%rsp), %xmm0           # sieve ('A' - 'a') with bit mask
    paddb       (%rdi), %xmm0               # perform conditional computation of (c + ('A' - 'a'))

    movdqa      %xmm0, (%rdi)               # write result

    addq        $0x10, %rdi                 # increment loop counter/pointer
    pcmpeqb     (%rsp), %xmm0               # look for \0
    pmovmskb    %xmm0, %esi                 # create bit mask of comparison
    cmpw        $0, %si                     # check for \0
    je          .L2                         # keep on going

    sub         $-0x48, %rsp                # restore stack pointer

    rep ret                                 # return
    .cfi_endproc
.LFE0:
    .size       asmb, .-asmb
    .ident      "GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
    .section    .note.GNU-stack,"",@progbits
