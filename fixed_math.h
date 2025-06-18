#ifndef FIXED_MATH_H
#define FIXED_MATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Multiply two Q16.16 fixed-point numbers
 * @param a First operand
 * @param b Second operand
 * @return Result (Q16.16)
 */
int32_t fixed_mul(int32_t a, int32_t b);

/**
 * Approximate e^x for x >= 0 in Q16.16 format
 * @param x Input (Q16.16)
 * @return Approximation of e^x (Q16.16)
 */
int32_t fixed_exp(int32_t x);

/**
 * Approximate e^x for any x in Q16.16 format
 * Supports negative x via small LUT + linear interpolation
 * @param x Input (Q16.16), can be negative
 * @return Approximation of e^x (Q16.16)
 */
int32_t fixed_exp_signed(int32_t x);

/**
 * Compute natural logarithm using binary search
 * @param y Input in Q16.16 format (must be positive)
 * @return ln(y) in Q16.16 format
 */
int32_t fixed_log(int32_t y);

/**
 * Compute base-10 logarithm
 * @param y Input in Q16.16 format (must be positive)
 * @return log10(y) in Q16.16 format
 */
int32_t fixed_log10(int32_t y);

/**
 * Compute base-2 logarithm
 * @param y Input in Q16.16 format (must be positive)
 * @return log2(y) in Q16.16 format
 */
int32_t fixed_log2(int32_t y);

/** Q16.16 representation of 1.0 */
#define Q16_ONE (1 << 16)

/** Q16.16 representation of ln(2) */
#define LN2_Q16 45426

/** Q16.16 representation of ln(10) */
#define LN10_Q16 150902

#ifdef __cplusplus
}
#endif

#endif // FIXED_MATH_H
