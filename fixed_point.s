.syntax unified
.cpu cortex-m3
.thumb

// ----------------------------------------------------------------------------
// Multiply two Q16.16 fixed-point numbers with rounding
// r0 = a, r1 = b => r0 = a*b (Q16.16)
.global fixed_mul
.type fixed_mul, %function
fixed_mul:
    push    {r4, r5, lr}
    umull   r4, r5, r0, r1        // 64-bit multiply: r5:r4 = r0 * r1
    adds    r4, r4, #0x8000       // add 0.5 for rounding
    adc     r5, r5, #0
    lsrs    r0, r4, #16           // low 16 bits to r0
    orr     r0, r0, r5, lsl #16   // combine high bits
    pop     {r4, r5, pc}

// ----------------------------------------------------------------------------
// Compute e^x for x >= 0 in Q16.16 using binary decomposition
.global fixed_exp
.type fixed_exp, %function
fixed_exp:
    push    {r4-r7, lr}
    mov     r4, r0                // x in Q16.16
    asr     r5, r4, #16           // integer part
    uxth    r6, r4                // fractional part
    ldr     r7, =Q16_ONE          // result = 1.0

    // Integer part: multiply by e^1 for each integer
1:  cmp     r5, #0
    beq     2f
    ldr     r0, =EXP_E
    mov     r1, r7
    bl      fixed_mul
    mov     r7, r0
    subs    r5, r5, #1
    b       1b

    // Fractional part: binary decomposition
2:  movs    r5, #1
3:  cmp     r5, #11
    bgt     4f
    // mask = 1 << (16 - r5)
    rsb     r0, r5, #16
    movs    r1, #1
    lsl     r0, r1, r0
    ands    r0, r6, r0
    beq     5f
    ldr     r1, =exp_table
    ldr     r0, [r1, r5, lsl #2]
    mov     r1, r7
    bl      fixed_mul
    mov     r7, r0
5:  adds    r5, r5, #1
    b       3b
4:  mov     r0, r7
    pop     {r4-r7, pc}

// ----------------------------------------------------------------------------
// 1/e^x lookup table + linear interpolation for x >= 0
.global inv_exp_interp
.type inv_exp_interp, %function
inv_exp_interp:
    push    {r4-r7, lr}
    
    // Check bounds
    cmp     r0, #0
    ble     .L_return_one
    ldr     r1, =(10 << 16)
    cmp     r0, r1
    bge     .L_return_zero
    
    // idx = x >> 13 (step = 0.125 = 2^(-3+16) => shift 13)
    lsrs    r4, r0, #13           // r4 = idx
    cmp     r4, #80
    ble     .L_idx_ok
    movs    r4, #80
.L_idx_ok:
    
    // Get y0 and y1 from table
    ldr     r5, =inv_exp_table
    ldr     r6, [r5, r4, lsl #2]  // r6 = y0 = inv_exp_table[idx]
    ldr     r7, [r5, r4, lsl #2]
    ldr     r7, [r5, #4]!         // r7 = y1 = inv_exp_table[idx + 1]
    // Actually do this properly:
    adds    r1, r4, #1
    ldr     r7, [r5, r1, lsl #2]  // r7 = y1 = inv_exp_table[idx + 1]
    
    // frac = (x & 0x1FFF) << 3
    ldr     r1, =0x1FFF
    ands    r1, r0, r1            // r1 = x & 0x1FFF
    lsls    r1, r1, #3            // r1 = frac = (x & 0x1FFF) << 3
    
    // delta = fixed_mul(y1 - y0, frac)
    subs    r0, r7, r6            // r0 = y1 - y0
    bl      fixed_mul             // r0 = delta = fixed_mul(y1 - y0, frac)
    
    // result = y0 + delta
    adds    r0, r6, r0
    pop     {r4-r7, pc}

.L_return_one:
    ldr     r0, =Q16_ONE
    pop     {r4-r7, pc}

.L_return_zero:
    movs    r0, #0
    pop     {r4-r7, pc}

// ----------------------------------------------------------------------------
// Compute e^x for any signed x: positive via fixed_exp, negative via inv_exp_interp
.global fixed_exp_signed
.type fixed_exp_signed, %function
fixed_exp_signed:
    cmp     r0, #0
    bge     fixed_exp
    neg     r0, r0                // x = -x
    bl      inv_exp_interp
    bx      lr

// ----------------------------------------------------------------------------
// Data tables
.data
.align 2

Q16_ONE:
    .word 0x00010000

EXP_E:
    .word 178145

exp_table:
    .word 0, 108853, 84102, 74264, 69783, 67585
    .word 66573, 66075, 65824, 65700, 65637, 65606

inv_exp_table:
    .word 65536, 57436, 50399, 44231, 38836, 34061, 29842, 26119, 22837, 19951
    .word 17416, 15191, 13245, 11547, 10069, 8788, 7676, 6712, 5877, 5158
    .word 4542, 4016, 3566, 3182, 2853, 2571, 2328, 2117, 1933, 1772
    .word 1631, 1506, 1395, 1295, 1205, 1123, 1049, 983, 922, 867
    .word 816, 770, 727, 688, 652, 618, 587, 558, 531, 506
    .word 483, 461, 441, 422, 404, 387, 372, 357, 343, 330
    .word 318, 307, 296, 286, 276, 267, 258, 250, 243, 236
    .word 229, 223, 217, 211, 206, 200, 195, 191, 186, 182
    .word 178
