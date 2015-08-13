    global escape_test_ps
    global escape_test_pd
    global bulb_test_ps
    global bulb_test_pd

    section .text


    align 16
escape_test_ps:
    ; int escape_test_ps(const float* cr, const float* ci, uint32_t* ic,
    ;     uint64_t size, uint32_t maxiter, float rad)
    ; For each complex value cr[n] + i*ci[n] iterates over the complex quadratic
    ; polynomial
    ; z_n+1 = z_n^2 + c, z_0 = 0
    ; and writes the iteration count where the point escapes the cricle of
    ; radius rad to ic[n]. If the point doesn't escape within maxiter iterations
    ; that value is written instead.
    ; The number of points (size) must be a multiple of 8.
    xor rax, rax

    ; cr is nullptr
    sub rax, 1
    test rdi, rdi
    jz .exit

    ; ci is nullptr
    sub rax, 1
    test rsi, rsi
    jz .exit

    ; ic is nullptr
    sub rax, 1
    test rdx, rdx
    jz .exit

    ; size is not 0 mod 8
    sub rax, 1
    test rcx, 0x07
    jnz .exit


    vbroadcastss ymm0, xmm0
    vmulps  ymm0, ymm0 ; square the radius to later compare with the
                       ; squared norm

    mov r9, r8
    mov r8, rcx
    shl r8, 2
    xor rax,rax

.outer_loop:
    vxorps  ymm1, ymm1      ; z_r
    vxorps  ymm2, ymm2      ; z_i
    vmovups ymm3, [rdi+rax] ; c_r
    vmovups ymm4, [rsi+rax] ; c_i
    vxorps  ymm5, ymm5      ; escape counter
    vxorps  ymm8, ymm8
    vxorps  ymm9, ymm9


    mov rcx, r9

    ; calculates a single iteration of the complex polynomial
    ; z_n+1 = z_n^2 + c = z_r^2 - z_i^2 + c_r + i(2*z_r*z_i + c_i)
    ; and compares with the escape radius.
    ; Note that the squares z_r^2 and z_i^2 are actually calculated _after_ the
    ; main calculation. Since they start out as 0 they are already squared in
    ; the first iteration and subseqent iterations can use the value of the
    ; previous one. This saves repeating two multiplications for the squared norm.
.inner_loop:
    vmulps ymm7, ymm1, ymm2
    vaddps ymm7, ymm7
    vsubps ymm1, ymm8, ymm9
    vaddps ymm1, ymm3
    vaddps ymm2, ymm7, ymm4

    vmulps ymm8, ymm1, ymm1
    vmulps ymm9, ymm2, ymm2
    vaddps ymm7, ymm8, ymm9
    vcmpleps ymm7, ymm0
    vpsubd ymm5, ymm7

    loop .inner_loop

    vmovups [rdx+rax], ymm5

    add rax, 0x20
    cmp rax, r8
    jl .outer_loop

    xor rax, rax
.exit:
    ret





    align 16
escape_test_pd:
    ; int escape_test_pd(const double* cr, const double* ci, uint64_t* ic,
    ;     uint64_t size, uint64_t maxiter, double rad)
    ; For each complex value cr[n] + i*ci[n] iterates over the complex quadratic
    ; polynomial
    ; z_n+1 = z_n^2 + c, z_0 = 0
    ; and writes the iteration count where the point escapes the cricle of
    ; radius rad to ic[n]. If the point doesn't escape within maxiter iterations
    ; that value is written instead.
    ; The number of points (size) must be a multiple of 4.
    xor rax, rax

    ; cr is nullptr
    sub rax, 1
    test rdi, rdi
    jz .exit

    ; ci is nullptr
    sub rax, 1
    test rsi, rsi
    jz .exit

    ; ic is nullptr
    sub rax, 1
    test rdx, rdx
    jz .exit

    ; size is not 0 mod 4
    sub rax, 1
    test rcx, 0x03
    jnz .exit


    vbroadcastsd ymm0, xmm0
    vmulpd  ymm0, ymm0 ; square the radius to later compare with the
                       ; squared norm

    mov r9, r8
    mov r8, rcx
    shl r8, 3
    xor rax,rax

.outer_loop:
    vxorps  ymm1, ymm1      ; z_r
    vxorps  ymm2, ymm2      ; z_i
    vmovups ymm3, [rdi+rax] ; c_r
    vmovups ymm4, [rsi+rax] ; c_i
    vxorps  ymm5, ymm5      ; escape counter
    vxorps  ymm8, ymm8
    vxorps  ymm9, ymm9


    mov rcx, r9

    ; calculates a single iteration of the complex polynomial
    ; z_n+1 = z_n^2 + c = z_r^2 - z_i^2 + c_r + i(2*z_r*z_i + c_i)
    ; and compares with the escape radius.
    ; Note that the squares z_r^2 and z_i^2 are actually calculated _after_ the
    ; main calculation. Since they start out as 0 they are already squared in
    ; the first iteration and subseqent iterations can use the value of the
    ; previous one. This saves repeating two multiplications for the squared norm.
.inner_loop:
    vmulpd ymm7, ymm1, ymm2
    vaddpd ymm7, ymm7
    vsubpd ymm1, ymm8, ymm9
    vaddpd ymm1, ymm3
    vaddpd ymm2, ymm7, ymm4

    vmulpd ymm8, ymm1, ymm1
    vmulpd ymm9, ymm2, ymm2
    vaddpd ymm7, ymm8, ymm9
    vcmplepd ymm7, ymm0
    vpsubd ymm5, ymm7

    loop .inner_loop

    vmovups [rdx+rax], ymm5

    add rax, 0x20
    cmp rax, r8
    jl .outer_loop

    xor rax, rax
.exit:
    ret





    align 16
bulb_test_ps:
    ; int bulb_test_ps(const float* cr, const float* ci,
    ;                  uint32_t* im, uint64_t size)
    ; Tests whether the point defined by cr[n] + i*ci[n] is in either the main
    ; cardoid or the period-2 bulb or not.
    xor rax, rax

    ; cr is nullptr
    sub rax, 1
    test rdi, rdi
    jz .exit

    ; ci is nullptr
    sub rax, 1
    test rsi, rsi
    jz .exit

    ; im is nullptr
    sub rax, 1
    test rdx, rdx
    jz .exit

    ; size is not 0 mod 8
    sub rax, 1
    test rcx, 0x07
    jnz .exit

    vmovaps ymm2, [ovfour_ps]
    vmovaps ymm3, [ovsixteen_ps]
    vmovaps ymm4, [one_ps]

.loop:
    vmovups ymm0, [rdi+4*rcx-4]
    vmovups ymm1, [rsi+4*rcx-4]

    vmulps ymm1, ymm1
    vmulps ymm8, ymm2, ymm1

    vsubps ymm5, ymm0, ymm2
    vmulps ymm6, ymm5, ymm5
    vaddps ymm6, ymm1
    vaddps ymm7, ymm6, ymm5
    vmulps ymm6, ymm7
    vcmpltps ymm6, ymm8

    vaddps ymm7, ymm0, ymm4
    vmulps ymm7, ymm7
    vaddps ymm7, ymm1
    vcmpltps ymm7, ymm3

    vpor ymm7, ymm6
    vmovups [rdx+4*rcx-4], ymm7

    loop .loop

.exit:
    ret





    align 16
bulb_test_pd:
    ; int bulb_test_pd(const double* cr, const double* ci,
    ;                  uint32_t* im, uint64_t size)
    ; Tests whether the point defined by cr[n] + i*ci[n] is in either the main
    ; cardoid or the period-2 bulb or not.
    xor rax, rax

    ; cr is nullptr
    sub rax, 1
    test rdi, rdi
    jz .exit

    ; ci is nullptr
    sub rax, 1
    test rsi, rsi
    jz .exit

    ; im is nullptr
    sub rax, 1
    test rdx, rdx
    jz .exit

    ; size is not 0 mod 8
    sub rax, 1
    test rcx, 0x03
    jnz .exit

    vmovaps ymm2, [ovfour_ps]
    vmovaps ymm3, [ovsixteen_ps]
    vmovaps ymm4, [one_ps]

.loop:
    vmovups ymm0, [rdi+4*rcx-4]
    vmovups ymm1, [rsi+4*rcx-4]

    vmulpd ymm1, ymm1
    vmulpd ymm8, ymm2, ymm1

    vsubpd ymm5, ymm0, ymm2
    vmulpd ymm6, ymm5, ymm5
    vaddpd ymm6, ymm1
    vaddpd ymm7, ymm6, ymm5
    vmulpd ymm6, ymm7
    vcmpltpd ymm6, ymm8

    vaddpd ymm7, ymm0, ymm4
    vmulpd ymm7, ymm7
    vaddpd ymm7, ymm1
    vcmpltpd ymm7, ymm3

    vpor ymm7, ymm6
    vmovups [rdx+4*rcx-4], ymm7

    loop .loop

.exit:
    ret



    section .data
    align 32
ovfour_ps:    times 8 dd 0.25
ovsixteen_ps: times 8 dd 0.0625
one_ps:       times 8 dd 1.0
