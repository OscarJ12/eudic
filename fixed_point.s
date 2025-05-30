.cpu cortex-m3
	.arch armv7-m
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"binary_exp_benchmark.c"
	.text
	.align	1
	.global	fixed_mul
	.syntax unified
	.thumb
	.thumb_func
	.type	fixed_mul, %function
fixed_mul:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r4, r5, r7, r8, r9, r10, fp}
	sub	sp, sp, #12
	add	r7, sp, #0
	str	r0, [r7, #4]
	str	r1, [r7]
	ldr	r1, [r7, #4]
	asrs	r0, r1, #31
	mov	r10, r1
	mov	fp, r0
	ldr	r1, [r7]
	asrs	r0, r1, #31
	mov	r8, r1
	mov	r9, r0
	mul	r0, r8, fp
	mul	r1, r10, r9
	add	r1, r1, r0
	umull	r2, r3, r10, r8
	add	r1, r1, r3
	mov	r3, r1
	adds	r4, r2, #32768
	adc	r5, r3, #0
	mov	r2, #0
	mov	r3, #0
	lsrs	r2, r4, #16
	orr	r2, r2, r5, lsl #16
	asrs	r3, r5, #16
	mov	r3, r2
	mov	r0, r3
	adds	r7, r7, #12
	mov	sp, r7
	@ sp needed
	pop	{r4, r5, r7, r8, r9, r10, fp}
	bx	lr
	.size	fixed_mul, .-fixed_mul
	.global	exp_table
	.section	.rodata
	.align	2
	.type	exp_table, %object
	.size	exp_table, 48
exp_table:
	.word	0
	.word	108853
	.word	84102
	.word	74264
	.word	69783
	.word	67585
	.word	66573
	.word	66075
	.word	65824
	.word	65700
	.word	65637
	.word	65606
	.text
	.align	1
	.global	fixed_exp
	.syntax unified
	.thumb
	.thumb_func
	.type	fixed_exp, %function
fixed_exp:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #32
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r3, [r7, #4]
	asrs	r3, r3, #16
	str	r3, [r7, #16]
	ldr	r3, [r7, #4]
	uxth	r3, r3
	str	r3, [r7, #12]
	mov	r3, #65536
	str	r3, [r7, #28]
	movs	r3, #0
	str	r3, [r7, #24]
	b	.L4
.L5:
	ldr	r1, .L10
	ldr	r0, [r7, #28]
	bl	fixed_mul
	str	r0, [r7, #28]
	ldr	r3, [r7, #24]
	adds	r3, r3, #1
	str	r3, [r7, #24]
.L4:
	ldr	r2, [r7, #24]
	ldr	r3, [r7, #16]
	cmp	r2, r3
	blt	.L5
	movs	r3, #1
	str	r3, [r7, #20]
	b	.L6
.L8:
	ldr	r3, [r7, #20]
	rsb	r3, r3, #16
	movs	r2, #1
	lsl	r3, r2, r3
	str	r3, [r7, #8]
	ldr	r2, [r7, #12]
	ldr	r3, [r7, #8]
	ands	r3, r3, r2
	cmp	r3, #0
	beq	.L7
	ldr	r2, .L10+4
	ldr	r3, [r7, #20]
	ldr	r3, [r2, r3, lsl #2]
	mov	r1, r3
	ldr	r0, [r7, #28]
	bl	fixed_mul
	str	r0, [r7, #28]
.L7:
	ldr	r3, [r7, #20]
	adds	r3, r3, #1
	str	r3, [r7, #20]
.L6:
	ldr	r3, [r7, #20]
	cmp	r3, #11
	ble	.L8
	ldr	r3, [r7, #28]
	mov	r0, r3
	adds	r7, r7, #32
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L11:
	.align	2
.L10:
	.word	178145
	.word	exp_table
	.size	fixed_exp, .-fixed_exp
	.ident	"GCC: (15:13.2.rel1-2) 13.2.1 20231009"
