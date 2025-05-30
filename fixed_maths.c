#include "fixed_math.h"

int32_t fixed_mul(int32_t a, int32_t b) {
    int64_t temp = (int64_t)a * b;
    // Rounding: add 0.5 in Q16.16
    temp += 1 << 15;
    return (int32_t)(temp >> 16);
}

static const int32_t exp_table[12] = {
    0, 108853, 84102, 74264, 69783, 67585,
    66573, 66075, 65824, 65700, 65637, 65606
};

#define EXP_E 178145  // exp(1.0) in Q16.16

int32_t fixed_exp(int32_t x) {
    int32_t int_part = x >> 16;
    uint16_t frac_part = x & 0xFFFF;

    int32_t result = Q16_ONE;
    for (int i = 0; i < int_part; i++) {
        result = fixed_mul(result, EXP_E);
    }

    for (int i = 1; i <= 11; i++) {
        uint16_t mask = 1U << (16 - i);
        if (frac_part & mask) {
            result = fixed_mul(result, exp_table[i]);
        }
    }

    return result;
}
