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
 * @return Result in Q16.16
 */
int32_t fixed_mul(int32_t a, int32_t b);

/**
 * Approximate e^x for x >= 0 in Q16.16 format
 * @param x Input in Q16.16
 * @return Approximation of e^x in Q16.16
 */
int32_t fixed_exp(int32_t x);

/**
 * Approximate e^x for any x in Q16.16 format
 * @param x Input in Q16.16 (can be negative)
 * @return Approximation of e^x in Q16.16
 */
int32_t fixed_exp_signed(int32_t x);

/** Q16.16 representation of 1.0 */
#define Q16_ONE (1 << 16)

#ifdef __cplusplus
}
#endif

#endif // FIXED_MATH_H
