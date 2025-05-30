#include "fixed_math.h"

int32_t fixed_mul(int32_t a, int32_t b) {
    int64_t temp = (int64_t)a * b;
    temp += 1 << 15;  // Rounding: +0.5 in Q16.16
    return (int32_t)(temp >> 16);
}

static const int32_t exp_table[12] = {
    0, 108853, 84102, 74264, 69783, 67585,
    66573, 66075, 65824, 65700, 65637, 65606
};

#define EXP_E 178145  // e^1 ≈ 2.71828 in Q16.16

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

// Precomputed 1/e^x values for x = 0 to 10 in steps of 0.125 (81 entries)
static const int32_t inv_exp_table[81] = {
    65536, 57436, 50399, 44231, 38836, 34061, 29842, 26119, 22837, 19951,
    17416, 15191, 13245, 11547, 10069, 8788, 7676, 6712, 5877, 5158,
    4542, 4016, 3566, 3182, 2853, 2571, 2328, 2117, 1933, 1772,
    1631, 1506, 1395, 1295, 1205, 1123, 1049, 983, 922, 867,
    816, 770, 727, 688, 652, 618, 587, 558, 531, 506,
    483, 461, 441, 422, 404, 387, 372, 357, 343, 330,
    318, 307, 296, 286, 276, 267, 258, 250, 243, 236,
    229, 223, 217, 211, 206, 200, 195, 191, 186, 182,
    178
};

int32_t inv_exp_lookup_from_x(int32_t x_q16) {
    // Clamp input to range [0, 10.0] in Q16.16
    if (x_q16 >= (10 << 16)) return 0;

    uint32_t index = x_q16 >> 13;  // 0.125 step = 8192
    if (index >= 81) index = 80;
    return inv_exp_table[index];
}

int32_t fixed_exp_signed(int32_t x) {
    if (x >= 0) {
        return fixed_exp(x);
    } else {
        return inv_exp_lookup_from_x(-x);  // look up 1 / e^x for positive x
    }
}

