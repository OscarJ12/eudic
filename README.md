#  Fixed-Point Math Library (Q16.16)

This library provides a portable and efficient implementation of fixed-point multiplication and exponential functions using **Q16.16** fixed-point format. It includes both a **portable C version** and an **optimised ARM Cortex-M3 assembly implementation**.

---

##  Reference

This implementation of **positive-only** `e^x` is based on the method presented in:

**O. W. Jackson**,  
“A Fixed-Point Binary Decomposition Method for Efficient Exponential Approximation in Embedded Systems,”  
*TechRxiv*, May 2025.  
📄 [Read the full paper on TechRxiv](https://www.techrxiv.org/users/921611/articles/1293706-a-fixed-point-binary-decomposition-method-for-efficient-exponential-approximation-in-embedded-systems)

> **Note:**  
> The published paper describes only the positive-`x` algorithm.  
> Support for negative exponents (`fixed_exp_signed`) is a library extension, not (yet) covered in a formal publication.

---

##  Files

| File             | Description                                                                  |
|------------------|------------------------------------------------------------------------------|
| `fixed_math.c`   | Portable C implementation of `fixed_mul()`, `fixed_exp()` and `fixed_exp_signed()` |
| `fixed_math.h`   | Public header declaring the fixed-point API                                  |
| `fixed_point.s`  | Optimised ARMv7-M (Cortex-M3) assembly version                               |

---

## ⚙️ About Q16.16 Format

The Q16.16 format stores fixed-point numbers in a 32-bit integer:  
- **Upper 16 bits**: integer part  
- **Lower 16 bits**: fractional part  

Examples:  
- `1.0` → `0x00010000`  
- `0.5` → `0x00008000`  

Use `Q16_ONE = (1 << 16)` for convenience.

---

## 🧰 Functions

### `int32_t fixed_mul(int32_t a, int32_t b);`  
Multiply two Q16.16 numbers with correct rounding.

### `int32_t fixed_exp(int32_t x);`  
Approximate `e^x` in Q16.16 for **x ≥ 0** using binary decomposition (as in the paper).

### `int32_t fixed_exp_signed(int32_t x);`  
Approximate `e^x` in Q16.16 **for any signed x**.  
Negative inputs are handled via a small lookup table plus one-point linear interpolation—**no divisions**, **integer only**, small code size and RAM footprint.

---

## 🚀 Usage Example

```c
#include <stdio.h>
#include <math.h>
#include "fixed_math.h"

// Convert between Q16.16 and float
static float    to_float(int32_t q) { return q / (float)Q16_ONE; }
static int32_t from_float(float f) { return (int32_t)(f * Q16_ONE); }

int main(void) {
    // Positive exponent
    int32_t q1 = from_float(1.0f);
    int32_t r1 = fixed_exp_signed(q1);
    printf("e^1   ≈ %f  (fixed)\n", to_float(r1));
    printf("expf:  %f\n", expf(1.0f));

    // Negative exponent
    int32_t q2 = from_float(-4.0f);
    int32_t r2 = fixed_exp_signed(q2);
    printf("e^-4  ≈ %f  (fixed)\n", to_float(r2));
    printf("expf:  %f\n", expf(-4.0f));

    return 0;
}

