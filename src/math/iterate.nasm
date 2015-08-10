    global test_in_M_ps

    section .text

    align 16
test_in_M_ps:
    ; int test_in_M_ps(const float* points, uint32_t* in, uint64_t size,
    ;    uint32_t maxiter, float rad)
    ; Takes an array of complex values with single precision, _points_, and
    ; iterates them through the complex quadratic polynomial
    ; z_n+1 = z_n^2 + c, z_0 = 0
    ; to a maximum of _maxiter_ iterations.
    ; It saves the number of the iteration the points escape the circle of
    ; radius _rad_ in the array _in_. If a point doesn't escape in _maxiter_
    ; iterations that value will be saved instead.
    ; The complex values are layed out in memory such that 8 imagenary values
    ; follow their respective 8 real values:
    ; (rrrrrrrriiiiiiii)(rrrrrrrriiiiiiii)(rrrrrrrriiiiiiii)
    ; _size_ is the length of the _in_ array (thus the _points_ array
    ; contains 2*_size_ single precision values). It must be a multiple of 8.

    xor rax, rax

    ; _points_ is nullptr
    sub rax, 1
    test rdi, rdi
    jz .exit

    ; _in_ is nullptr
    sub rax, 1
    test rsi, rsi
    jz .exit

    ; _size_ is not 0 mod 8
    sub rax, 1
    test rdx, 0x07
    jnz .exit


    vbroadcastss ymm0, xmm0
    vmulps  ymm0, ymm0 ; square the radius to later compare with the
                       ; squared norm


    shl rdx, 2
    xor rax,rax

.outer_loop:
    vxorps  ymm1, ymm1             ; z_r
    vxorps  ymm2, ymm2             ; z_i
    vmovups ymm3, [rdi+2*rax+0x00] ; c_r
    vmovups ymm4, [rdi+2*rax+0x20] ; c_i
    vxorps  ymm5, ymm5             ; escape counter
    vxorps  ymm8, ymm8
    vxorps  ymm9, ymm9


    xor r8, r8

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

    add r8, 1
    cmp r8, rcx
    jl .inner_loop

    vmovups [rsi+rax], ymm5

    add rax, 0x20
    cmp rax, rdx
    jl .outer_loop

    xor rax, rax
.exit:
    ret
