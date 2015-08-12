    global xorshift128_next
    global xorshift1024_next
    global fill_canonical128_ps
    global fill_canonical128_pd
    global fill_canonical1024_ps
    global fill_canonical1024_pd

    section .text





    align 16
xorshift128_next:
    vmovupd ymm0, [rdi]
    vmovupd ymm1, [rdi+0x20]
    vmovupd [rdi], ymm1
    vpsllq  ymm2, ymm0, 23
    vxorps  ymm2, ymm0
    vpsrlq  ymm0, ymm2, 17
    vxorps  ymm2, ymm1
    vxorps  ymm2, ymm0
    vpsrlq  ymm0, ymm1, 26
    vxorps  ymm2, ymm0
    vmovupd [rdi+0x20], ymm2
    vpaddq  ymm0, ymm1, ymm2
    ret





    align 16
xorshift1024_next:
    mov      rax, [rdi+0x200]
    vmovdqu  ymm0, [rdi+rax]
    add      rax, 0x20
    and      rax, 0x1e0
    vmovdqu  ymm1, [rdi+rax]
    mov      [rdi+0x200], rax

    vpsllq   ymm2, ymm1, 31
    vpxor    ymm1, ymm2
    vpsrlq   ymm3, ymm0, 30
    vpxor    ymm0, ymm3
    vpsrlq   ymm2, ymm1, 11
    vpxor    ymm1, ymm2
    vpxor    ymm0, ymm1
    vmovdqu  [rdi+rax], ymm0

    vmovdqa  ymm2, [xs1024lo]
    vpsrlq   ymm1, ymm0, 32
    vpmuludq ymm1, ymm2
    vpmuludq ymm3, ymm0, [xs1024hi]
    vpaddq   ymm1, ymm3
    vpmuludq ymm4, ymm0, ymm2
    vpsllq   ymm1, 32
    vpaddq   ymm0, ymm1, ymm4
    ret





    align 16
fill_canonical128_ps:
    ; rdi: pointer to float array to be filled
    ; rsi: size of array to be filled
    ; rdx: xorshift128 state pointer

    ; load rng state
    vmovups ymm0, [rdx]       ; ymm0 <- s1
    vmovups ymm1, [rdx+0x20]  ; ymm1 <- s0
    vmovaps ymm4, [fc128_ps]
    vmovaps ymm8, [fc128_32]
    vmovaps ymm9, [fc128_and_ps]

    ; save old rounding mode and set current one to round to zero
    sub       rsp, 8
    vstmxcsr [rsp]
    vstmxcsr [rsp+4]
    or dword [rsp+4], 3 << 13
    vldmxcsr [rsp+4]


    ; less than 8 remaining?
    cmp rsi, 8
    jl .last

    ; 8 loop
.loop8:
    call fill_canonical128_ps_next

    vmovups [rdi], ymm3
    add rdi, 0x20
    sub rsi, 8

    cmp rsi, 8
    jge .loop8

    ; last <8 elements
.last:
    test rsi, rsi
    jz .end
    call fill_canonical128_ps_next

    vmovd xmm2, esi
    vpbroadcastd ymm2, xmm2
    vpcmpgtd ymm2, ymm2, ymm8
    vpmaskmovd [rdi], ymm2, ymm3

.end:
    ; save rng state
    vmovups [rdx], ymm0
    vmovups [rdx+0x20], ymm1

    ; restore rounding mode
    vldmxcsr [rsp]
    add       rsp, 8

    ret

    align 16
    ; advance rng state and return floats
fill_canonical128_ps_next:
    ; s1 ^= s1 << 23
    vpsllq ymm2, ymm0, 23
    vxorps ymm2, ymm0

    ; s1 ^= s0 ^ (s1 >> 17) ^ (s0 >> 26)
    vpsrlq ymm0, ymm2, 17
    vxorps ymm2, ymm1
    vxorps ymm2, ymm0

    ; s1 ^= s0 >> 26
    vpsrlq ymm0, ymm1, 26
    vxorps ymm2, ymm0

    ; next = s1 + s0
    vpaddq ymm3, ymm2, ymm1

    ; xchg(s1, s0)
    vmovaps ymm0, ymm1
    vmovaps ymm1, ymm2

    ; next = float(next) * 2**-32
    vxorps    ymm2, ymm2
    vcvtdq2ps ymm5, ymm3
    vpcmpgtd  ymm2, ymm3
    vpsrld    ymm6, ymm3, 1
    vandps    ymm3, ymm9
    vorps     ymm3, ymm6
    vcvtdq2ps ymm3, ymm3
    vaddps    ymm3, ymm3
    vblendvps ymm3, ymm5, ymm3, ymm2

    vmulps    ymm3, ymm4
    ret





    align 16
fill_canonical128_pd:
    ; rdi: pointer to double array to be filled
    ; rsi: size of array to be filled
    ; rdx: xorshift128 state pointer

    ; load rng state
    vmovups ymm0, [rdx]       ; ymm0 <- s1
    vmovups ymm1, [rdx+0x20]  ; ymm1 <- s0
    vmovaps ymm4, [fc128_pd]
    vmovaps ymm8, [fc128_64]
    vmovaps ymm9, [fc128_and_pd]

    ; space to spill ymm and mxcsr registers on the stack
    sub rsp, 0x28

    ; save old rounding mode and set current one to round to zero
    vstmxcsr [rsp+0x20]
    vstmxcsr [rsp+0x24]
    or dword [rsp+0x24], 3 << 13
    vldmxcsr [rsp+0x24]

    ; less than 4 remaining?
    cmp rsi, 4
    jl .last

    ; 4 loop
.loop4:
    call fill_canonical128_pd_next

    vmovups [rdi], ymm3
    add rdi, 0x20
    sub rsi, 4

    cmp rsi, 4
    jge .loop4

    ; last <4 elements
.last:
    test rsi, rsi
    jz .end
    call fill_canonical128_pd_next

    vmovq xmm2, rsi
    vpbroadcastq ymm2, xmm2
    vpcmpgtq ymm2, ymm2, ymm8
    vpmaskmovq [rdi], ymm2, ymm3

.end:
    ; save rng state
    vmovups [rdx], ymm0
    vmovups [rdx+0x20], ymm1

    ; restore rounding mode
    vldmxcsr [rsp+0x20]

    ; restore stack pointer
    add rsp, 0x28

    ret

    align 16
    ; advance rng state and return doubles
fill_canonical128_pd_next:
    ; s1 ^= s1 << 23
    vpsllq ymm2, ymm0, 23
    vxorps ymm2, ymm0

    ; s1 ^= s0 ^ (s1 >> 17) ^ (s0 >> 26)
    vpsrlq ymm0, ymm2, 17
    vxorps ymm2, ymm1
    vxorps ymm2, ymm0

    ; s1 ^= s0 >> 26
    vpsrlq ymm0, ymm1, 26
    vxorps ymm2, ymm0

    ; next = s1 + s0
    vpaddq ymm3, ymm2, ymm1

    ; xchg(s1, s0)
    vmovaps ymm0, ymm1
    vmovaps ymm1, ymm2

    ; next = double(next) * 2**-64
    vxorps     ymm2,  ymm2
    vpcmpgtq   ymm2,  ymm3
    vmovups    [rsp+0x08], ymm3
    vpsrlq     ymm6,  ymm3, 1
    vandps     ymm3,  ymm9
    vorps      ymm3,  ymm6
    vpmaskmovq [rsp+0x08], ymm2, ymm3

    vcvtsi2sd xmm3, qword [rsp+0x08]
    vmovq     [rsp+0x08], xmm3
    vcvtsi2sd xmm5, qword [rsp+0x10]
    vmovq     [rsp+0x10], xmm5
    vcvtsi2sd xmm6, qword [rsp+0x18]
    vmovq     [rsp+0x18], xmm6
    vcvtsi2sd xmm7, qword [rsp+0x20]
    vmovq     [rsp+0x20], xmm7

    vmovups ymm5, [rsp+0x08]
    vaddpd  ymm3, ymm5, ymm5
    vblendvpd ymm3, ymm5, ymm3, ymm2

    vmulpd ymm3, ymm4
    ret





    align 16
fill_canonical1024_ps:
    ; load rng state
    mov rax, [rdx+0x200]
    vmovups ymm0, [rdx+rax]
    mov rcx, rax
    add rcx, 0x20
    and rcx, 0x1e0
    vmovups ymm1, [rdx+rcx]
    vmovaps ymm2, [fc128_ps]
    vmovaps ymm3, [fc128_32]
    vmovaps ymm4, [fc128_add_ps]
    vmovaps ymm5, [xs1024lo]
    vmovaps ymm6, [xs1024hi]

    ; save old rounding mode and set current one to round to zero
    sub       rsp, 8
    vstmxcsr [rsp]
    vstmxcsr [rsp+4]
    or dword [rsp+4], 3 << 13
    vldmxcsr [rsp+4]

    ; less than 8 remaining?
    cmp rsi, 8
    jl .last

    ; 8 loop
.loop8:
    mov rax, rcx
    call fill_canonical1024_ps_next

    vmovups [rdi], ymm7
    add rdi, 0x20
    sub rsi, 8

    cmp rsi, 8
    jge .loop8

    ; last <8 elements
.last:
    test rsi, rsi
    jz .end
    mov rax, rcx
    call fill_canonical1024_ps_next

    vmovd xmm8, esi
    vpbroadcastd ymm8, xmm8
    vpcmpgtd ymm8, ymm8, ymm3
    vpmaskmovd [rdi], ymm8, ymm7

.end:
    ; save rng state
    vmovups [rdx+rax], ymm0
    vmovups [rdx+rcx], ymm1
    mov [rdx+0x200], rax

    ; restore rounding mode
    vldmxcsr [rsp]
    add       rsp, 8

    ret

    align 16
fill_canonical1024_ps_next:
    vpsllq  ymm7, ymm1, 31
    vpxor   ymm1, ymm7
    vpsrlq  ymm8, ymm0, 30
    vpxor   ymm0, ymm8
    vpsrlq  ymm7, ymm1, 11
    vpxor   ymm1, ymm7
    vpxor   ymm0, ymm1

    mov rcx, rax
    vmovups [rdx+rax], ymm0
    add rcx, 0x20
    and rcx, 0x1e0
    vmovups ymm1, [rdx+rcx]

    vpsrlq   ymm8, ymm7, 32
    vpmuludq ymm8, ymm5
    vpmuludq ymm9, ymm7, ymm6
    vpaddq   ymm8, ymm9
    vpmuludq ymm10, ymm7, ymm5
    vpsllq   ymm8, 32
    vpaddq   ymm7, ymm8, ymm10

    vxorps    ymm8, ymm8
    vcvtdq2ps ymm9, ymm7
    vpcmpgtd  ymm8, ymm7
    vaddps    ymm7, ymm9, ymm4
    vblendvps ymm7, ymm9, ymm7, ymm8

    vmulps    ymm7, ymm2
    ret





    align 16
fill_canonical1024_pd:
    ; load rng state
    mov rax, [rdx+0x200]
    vmovups ymm0, [rdx+rax]
    mov rcx, rax
    add rcx, 0x20
    and rcx, 0x1e0
    vmovups ymm1, [rdx+rcx]
    vmovaps ymm2, [fc128_pd]
    vmovaps ymm3, [fc128_64]
    vmovaps ymm4, [fc128_add_pd]
    vmovaps ymm5, [xs1024lo]
    vmovaps ymm6, [xs1024hi]

    ; space to spill ymm and mxcsr registers on the stack
    sub rsp, 0x28

    ; save old rounding mode and set current one to round to zero
    vstmxcsr [rsp+0x20]
    vstmxcsr [rsp+0x24]
    or dword [rsp+0x24], 3 << 13
    vldmxcsr [rsp+0x24]

    ; less than 4 remaining?
    cmp rsi, 4
    jl .last

    ; 4 loop
.loop4:
    mov rax, rcx
    call fill_canonical1024_pd_next

    vmovups [rdi], ymm7
    add rdi, 0x20
    sub rsi, 4

    cmp rsi, 4
    jge .loop4

    ; last <4 elements
.last:
    test rsi, rsi
    jz .end
    mov rax, rcx
    call fill_canonical1024_pd_next

    vmovq xmm8, rsi
    vpbroadcastq ymm8, xmm8
    vpcmpgtq ymm8, ymm8, ymm3
    vpmaskmovq [rdi], ymm8, ymm7

.end:
    ; save rng state
    vmovups [rdx+rax], ymm0
    vmovups [rdx+rcx], ymm1
    mov [rdx+0x200], rax

    ; restore rounding mode
    vldmxcsr [rsp+0x20]

    ; restore stack pointer
    add rsp, 0x28

    ret

    align 16
fill_canonical1024_pd_next:
    vpsllq  ymm7, ymm1, 31
    vpxor   ymm1, ymm7
    vpsrlq  ymm8, ymm0, 30
    vpxor   ymm0, ymm8
    vpsrlq  ymm7, ymm1, 11
    vpxor   ymm1, ymm7
    vpxor   ymm0, ymm1

    mov rcx, rax
    vmovups [rdx+rax], ymm0
    add rcx, 0x20
    and rcx, 0x1e0
    vmovups ymm1, [rdx+rcx]

    vpsrlq   ymm8, ymm7, 32
    vpmuludq ymm8, ymm5
    vpmuludq ymm9, ymm7, ymm6
    vpaddq   ymm8, ymm9
    vpmuludq ymm10, ymm7, ymm5
    vpsllq   ymm8, 32
    vpaddq   ymm7, ymm8, ymm10

    vmovups   [rsp+0x08], ymm7
    vxorps    ymm8, ymm8
    vpcmpgtq  ymm8, ymm7

    vcvtsi2sd xmm7, qword [rsp+0x08]
    vmovq     [rsp+0x08], xmm7
    vcvtsi2sd xmm9, qword [rsp+0x10]
    vmovq     [rsp+0x10], xmm9
    vcvtsi2sd xmm10, qword [rsp+0x18]
    vmovq     [rsp+0x18], xmm10
    vcvtsi2sd xmm11, qword [rsp+0x20]
    vmovq     [rsp+0x20], xmm11

    vmovups   ymm7, [rsp+0x08]
    vaddpd    ymm9, ymm7, ymm4
    vblendvpd ymm7, ymm7, ymm9, ymm8

    vmulpd    ymm7, ymm2
    ret



    section .data
    align 32
xs1024hi: times 4 dq 0x00000000106689d4
xs1024lo: times 4 dq 0x000000005497fdb5

fc128_ps: times 8 dd 0x1.0p-32
fc128_32: dd 0, 1, 2, 3, 4, 5, 6, 7
fc128_and_ps:times 8 dd 1
fc128_add_ps:times 8 dd 0x1.0p32

fc128_pd: times 4 dq 0x1.0p-64
fc128_64: dq 0, 1, 2, 3
fc128_and_pd:times 4 dq 1
fc128_add_pd:times 4 dq 0x1.0p64

