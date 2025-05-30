.syntax unified
.cpu cortex-m3
.thumb

.text

.global fixed_mul
.type fixed_mul, %function

/**
 * fixed_mul - Multiply two Q16.16 numbers
 * Input: r0 = a, r1 = b
 * Output: r0 = result
 */
fixed_mul:
    push {r4, r5, lr}
    umull r4, r5, r0, r1      // 64-bit multiply
    adds  r4, r4, #0x8000     // +0.5 rounding
    adc   r5, r5, #0
    lsrs  r0, r4, #16
    orr   r0, r0, r5, lsl #16 // result = high word << 16 | low word >> 16
    pop {r4, r5, pc}

.global fixed_exp
.type fixed_exp, %function

/**
 * fixed_exp - Approximate e^x for x >= 0 (Q16.16)
 * Input: r0 = x (Q16.16)
 * Output: r0 = e^x (Q16.16)
 */
fixed_exp:
    push {r4-r7, lr}
    mov  r4, r0                // x
    asr  r5, r4, #16           // int part
    uxth r6, r4                // frac part
    mov  r7, #0x00010000       // result = 1.0 in Q16.16

    // Multiply result *= e^1 for each integer part
1:
    cmp  r5, #0
    beq  2f
    ldr  r0, =EXP_E
    mov  r1, r7
    bl   fixed_mul
    mov  r7, r0
    subs r5, r5, #1
    b    1b

2:  // Apply fractional bits
    movs r5, #1

3:
    cmp  r5, #11
    bgt  4f

    // Bit mask = 1 << (16 - i)
    rsb  r0, r5, #16
    movs r1, #1
    lsl  r0, r1, r0
    ands r0, r6, r0
    beq  5f

    ldr  r1, =exp_table
    ldr  r0, [r1, r5, lsl #2]  // exp_table[i]
    mov  r1, r7
    bl   fixed_mul
    mov  r7, r0

5:
    adds r5, r5, #1
    b    3b

4:
    mov  r0, r7
    pop {r4-r7, pc}

.global fixed_exp_signed
.type fixed_exp_signed, %function

/**
 * fixed_exp_signed - Approximate e^x for any x
 * Input: r0 = x (Q16.16)
 * Output: r0 = e^x (Q16.16)
 */
fixed_exp_signed:
    push {r1, lr}
    cmp  r0, #0
    bge  fixed_exp           // if x >= 0, just use fixed_exp
    rsb  r0, r0, #0          // negate x
    lsrs r1, r0, #13         // index = x >> 13 (0.125 steps)
    cmp  r1, #80
    bhi  9f
    ldr  r2, =inv_exp_table
    ldr  r0, [r2, r1, lsl #2]
    pop  {r1, pc}
9:
    movs r0, #0              // e^-x ≈ 0 for large x
    pop  {r1, pc}

.data
.align 2

.global exp_table
exp_table:
    .word 0
    .word 108853
    .word 84102
    .word 74264
    .word 69783
    .word 67585
    .word 66573
    .word 66075
    .word 65824
    .word 65700
    .word 65637
    .word 65606

.global inv_exp_table
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

.global EXP_E
EXP_E:
    .word 178145  // e^1 in Q16.16
