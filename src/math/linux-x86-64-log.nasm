; ------------------------------------------------------------------------------
; Functions for calculating the natural logarithm and the logarithm base 2
; of AVX2 ymm registers
; ------------------------------------------------------------------------------

%define _CMP_LT_OQ 17
%define _CMP_EQ_OQ 0


    global mm256_log_pd
    global mm256_log_ps

    section .text

; natural logarithm of 4 packed double precision values
; returns NaN for values < 0
; returns -inf for 0 and denormals
; returns +inf for +inf

; log(x) = log(m*2^e) = log(m) + e*log(2) with m \in [1/sqrt(2), sqrt(2))
; log(m) = 2*artanh((m-1)/(m+1)) = 2r + 2r^3/3 + 2r^5/5 + ... with r := (m-1)/(m+1)

mm256_log_pd:
    vmovapd ymm15, [inf_pd] ; ymm15 := Inf
    vmovapd ymm14, [one_pd] ; ymm14 := 1.0

    ; special return values masks
    vxorpd ymm1, ymm1                        ; ymm1 := 0
    vcmppd ymm13, ymm0, [min_pd], _CMP_LT_OQ ; mask denormals and lower to return -inf
    vcmppd ymm12, ymm0, ymm1,     _CMP_LT_OQ ; mask x<0 to return NaN
    vcmppd ymm11, ymm0, ymm15,    _CMP_EQ_OQ ; mask x=+inf to return +inf

    ; extract mantissa
    vandnpd ymm1,  ymm15, ymm0    ; not(+inf) is just the right mask to extract the (signed) mantissa
    vorpd   ymm2,  ymm1, [hlf_pd] ; mantissa / 2
    vorpd   ymm1,  ymm14          ; normalize mantissa
    vcmppd  ymm10, ymm1, [sqrt_pd], _CMP_LT_OQ ; mask x<sqrt(2)

    ; extract exponent
    vmovapd ymm3, [idx_si]
    vpsrld  ymm0, 20
    vpsubd  ymm0, [bias_pd]
    vpermd  ymm0, ymm3, ymm0
    vcvtdq2pd ymm0, xmm0
    vaddpd  ymm3, ymm0, ymm14

    ; restric mantissa to [1/sqrt(2), sqrt(2)) and adjust exponent
    vblendvpd ymm3, ymm0, ymm10
    vblendvpd ymm2, ymm1, ymm10

    vmulpd ymm3, [log2_pd] ; log(2^e) = e*log(2)

    ; r = (m-1)/(m+1)
    vsubpd ymm1, ymm2, ymm14
    vaddpd ymm2, ymm14
    vdivpd ymm1, ymm2
    vmovapd ymm0, ymm1 ; ymm0 := r
    vmulpd ymm1, ymm1  ; ymm1 := r^2

    ;
    vxorpd ymm4, ymm4
    vmovapd ymm2, [e9_pd]

    vfmadd213pd ymm2, ymm1, [e8_pd]
    vfmadd213pd ymm2, ymm1, [e7_pd]
    vfmadd213pd ymm2, ymm1, [e6_pd]
    vfmadd213pd ymm2, ymm1, [e5_pd]
    vfmadd213pd ymm2, ymm1, [e4_pd]
    vfmadd213pd ymm2, ymm1, [e3_pd]
    vfmadd213pd ymm2, ymm1, [e2_pd]
    vfmadd213pd ymm2, ymm1, [e1_pd]
    vfmadd213pd ymm2, ymm1, [e0_pd]

    vfmadd213pd ymm0, ymm2, ymm3

    ; Blend special return values into result
    vxorpd ymm2, ymm2
    vsubpd ymm2, ymm15
    vblendvpd ymm0, ymm15, ymm11
    vblendvpd ymm0, ymm2, ymm13
    vorpd ymm0, ymm12

    ret



mm256_log_ps:
    vmovaps ymm15, [inf_ps] ; ymm15 := Inf
    vmovaps ymm14, [one_ps] ; ymm14 := 1.0

    ; special return values masks
    vxorps ymm1, ymm1                        ; ymm1 := 0
    vcmpps ymm13, ymm0, [min_ps], _CMP_LT_OQ ; mask denormals and lower to return -inf
    vcmpps ymm12, ymm0, ymm1,     _CMP_LT_OQ ; mask x<0 to return NaN
    vcmpps ymm11, ymm0, ymm15,    _CMP_EQ_OQ ; mask x=+inf to return +inf

    ; extract mantissa
    vandnps ymm1,  ymm15, ymm0    ; not(+inf) is just the right mask to extract the (signed) mantissa
    vorps   ymm2,  ymm1, [hlf_ps] ; mantissa / 2
    vorps   ymm1,  ymm14          ; normalize mantissa
    vcmpps  ymm10, ymm1, [sqrt_ps], _CMP_LT_OQ ; mask x<sqrt(2)

    ; extract exponent
    vpsrld  ymm0, 23
    vpsubd  ymm0, [bias_ps]
    vcvtdq2ps ymm0, ymm0
    vaddps  ymm3, ymm0, ymm14

    ; restric mantissa to [1/sqrt(2), sqrt(2)) and adjust exponent
    vblendvps ymm3, ymm0, ymm10
    vblendvps ymm2, ymm1, ymm10

    vmulps ymm3, [log2_ps] ; log(2^e) = e*log(2)

    ; r = (m-1)/(m+1)
    vsubps ymm1, ymm2, ymm14
    vaddps ymm2, ymm14
    vdivps ymm1, ymm2
    vmovaps ymm0, ymm1 ; ymm0 := r
    vmulps ymm1, ymm1  ; ymm1 := r^2

    ;
    vxorps ymm4, ymm4
    vmovaps ymm2, [e4_ps]

    vfmadd213ps ymm2, ymm1, [e3_ps]
    vfmadd213ps ymm2, ymm1, [e2_ps]
    vfmadd213ps ymm2, ymm1, [e1_ps]
    vfmadd213ps ymm2, ymm1, [e0_ps]

    vfmadd213ps ymm0, ymm2, ymm3

    ; Blend special return values into result
    vxorps ymm2, ymm2
    vsubps ymm2, ymm15
    vblendvps ymm0, ymm15, ymm11
    vblendvps ymm0, ymm2, ymm13
    vorps ymm0, ymm12

    ret




    section .data
    align 32
inf_pd:  times 4 dq __Infinity__
one_pd:  times 4 dq 1.0
hlf_pd:  times 4 dq 0.5
min_pd:  times 4 dq 0x10000000000000    ; smallest positive normalized double
sqrt_pd: times 4 dq 1.4142135623730950  ; sqrt(2)
bias_pd: times 8 dd 1023
idx_si:  dd 1, 3, 5, 7, 0, 2, 4, 6
log2_pd: times 4 dq 0.69314718055994530 ; log(2)

e9_pd:   times 4 dq 0.10526315789473684 ; 2/19
e8_pd:   times 4 dq 0.11764705882352941 ; 2/17
e7_pd:   times 4 dq 0.13333333333333333 ; 2/15
e6_pd:   times 4 dq 0.15384615384615385 ; 2/13
e5_pd:   times 4 dq 0.18181818181818182 ; 2/11
e4_pd:   times 4 dq 0.22222222222222222 ; 2/9
e3_pd:   times 4 dq 0.28571428571428571 ; 2/7
e2_pd:   times 4 dq 0.40000000000000000 ; 2/5
e1_pd:   times 4 dq 0.66666666666666666 ; 2/3
e0_pd:   times 4 dq 2.00000000000000000 ; 2

inf_ps:  times 8 dd __Infinity__
one_ps:  times 8 dd 1.0
hlf_ps:  times 8 dd 0.5
min_ps:  times 8 dd 0x10000000
sqrt_ps: times 8 dd 1.4142135623730950
bias_ps: times 8 dd 127
log2_ps: times 8 dd 0.69314718055994530

e4_ps:   times 8 dd 0.22222222222222222
e3_ps:   times 8 dd 0.28571428571428571
e2_ps:   times 8 dd 0.40000000000000000
e1_ps:   times 8 dd 0.66666666666666666
e0_ps:   times 8 dd 2.00000000000000000
