# 🔢 Fixed-Point Math Library (Q16.16)

This library provides a portable and efficient implementation of fixed-point multiplication and exponential functions using **Q16.16** fixed-point format. It includes both a **portable C version** and an **optimized ARM Cortex-M3 assembly implementation**.

---

## 📁 Files

| File            | Description                                      |
|-----------------|--------------------------------------------------|
| `fixed_math.c`  | Portable C implementation of `fixed_mul()` and `fixed_exp()` |
| `fixed_math.h`  | Public header declaring the fixed-point API      |
| `fixed_point.s` | Optimized ARMv7-M (Cortex-M3) assembly version   |

---

## 🧠 About Q16.16 Format

The Q16.16 format stores fixed-point numbers in a 32-bit integer:
- The **upper 16 bits** represent the integer part
- The **lower 16 bits** represent the fractional part

For example:
- `1.0` = `0x00010000`
- `0.5` = `0x00008000`
- Use `Q16_ONE = (1 << 16)` for convenience

---

## 🚀 Functions

### `int32_t fixed_mul(int32_t a, int32_t b);`

Multiplies two Q16.16 numbers with rounding.

### `int32_t fixed_exp(int32_t x);`

Approximates the exponential function `exp(x)` using integer math.

---

## 📦 Usage Example

```c
#include <stdio.h>
#include "fixed_math.h"

float to_float(int32_t q) { return q / (float)Q16_ONE; }
int32_t from_float(float f) { return (int32_t)(f * Q16_ONE); }

int main() {
    int32_t x = from_float(1.0f);        // 1.0 in Q16.16
    int32_t result = fixed_exp(x);       // Approximate exp(1.0)
    printf("e^1 ≈ %f\n", to_float(result)); // Convert back to float
    return 0;
}
