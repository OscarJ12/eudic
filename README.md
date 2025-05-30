# Fixed-Point Maths Library (Q16.16)

This library provides efficient implementations of `fixed_mul()` and `fixed_exp()` in Q16.16 format, for use in systems without hardware floating-point.

## Files

- `fixed_math.h`: Public API
- `fixed_math.c`: Portable C implementation
- `fixed_point.s`: Optimized ARM Cortex-M3 assembly version

## Usage

```c
#include "fixed_math.h"

int32_t x = 1 * Q16_ONE; // 1.0 in Q16.16
int32_t y = fixed_exp(x); // e^1 = 2.718...
