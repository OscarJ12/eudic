#ifndef FIXED_MATH_H
#define FIXED_MATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t fixed_mul(int32_t a, int32_t b);
int32_t fixed_exp(int32_t x);

#define Q16_ONE (1 << 16)

#ifdef __cplusplus
}
#endif

#endif
