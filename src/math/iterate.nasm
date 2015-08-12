    global escape_test_ps

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

    ; _size_ is not 0 mod 8
    sub rax, 1
    test rcx, 0x07
    jnz .exit


    vbroadcastss ymm0, xmm0
    vmulps  ymm0, ymm0 ; square the radius to later compare with the
                       ; squared norm


    shl rcx, 2
    xor rax,rax

.outer_loop:
    vxorps  ymm1, ymm1      ; z_r
    vxorps  ymm2, ymm2      ; z_i
    vmovups ymm3, [rdi+rax] ; c_r
    vmovups ymm4, [rsi+rax] ; c_i
    vxorps  ymm5, ymm5      ; escape counter
    vxorps  ymm8, ymm8
    vxorps  ymm9, ymm9


    xor r9, r9

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
    vcmpps ymm7, ymm0, 18 ; _CMP_LE_OQ
    vpsubd ymm5, ymm7

    add r9, 1
    cmp r9, r8
    jl .inner_loop

    vmovups [rdx+rax], ymm5

    add rax, 0x20
    cmp rax, rcx
    jl .outer_loop

    xor rax, rax
.exit:
    ret
