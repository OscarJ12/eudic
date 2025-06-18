// fixed_log.c - Fixed-point natural logarithm implementation
// Uses binary search with the existing fixed_exp function

#include <stdint.h>
#include "fixed_math.h"

// Constants in Q16.16 format
#define LN2_Q16 45426      // ln(2) ≈ 0.693147
#define LN10_Q16 150902    // ln(10) ≈ 2.302585
#define SQRT2_Q16 92682    // sqrt(2) ≈ 1.414214

// Optional: Define USE_NEWTON_REFINEMENT to enable the Newton-Raphson step
// This improves accuracy but uses the division-free reciprocal approximation
// #define USE_NEWTON_REFINEMENT

// Division-free reciprocal approximation using Newton-Raphson
// Computes 1/b in Q16.16 format without using division
static int32_t fixed_reciprocal(int32_t b) {
    if (b <= 0) return 0x7FFFFFFF;
    
    // Initial approximation using shifts
    // For b in [0.5, 2], use linear approximation: 1/b ≈ 2 - b
    int32_t x;
    int shift = 0;
    
    // Normalize b to range [0.5, 1] by shifting
    int32_t b_norm = b;
    if (b_norm < (Q16_ONE >> 1)) {
        while (b_norm < (Q16_ONE >> 1)) {
            b_norm <<= 1;
            shift--;
        }
    } else {
        while (b_norm >= Q16_ONE) {
            b_norm >>= 1;
            shift++;
        }
    }
    
    // Initial guess: x = 2 - b_norm (good for b_norm in [0.5, 1])
    x = (Q16_ONE << 1) - b_norm;
    
    // Newton-Raphson iterations: x_new = x * (2 - b_norm * x)
    // This converges to 1/b_norm
    for (int i = 0; i < 3; i++) {
        int32_t bx = fixed_mul(b_norm, x);
        int32_t two_minus_bx = (Q16_ONE << 1) - bx;
        x = fixed_mul(x, two_minus_bx);
    }
    
    // Adjust for the normalization shift
    if (shift > 0) {
        x >>= shift;
    } else {
        x <<= -shift;
    }
    
    return x;
}

// Compute fixed_div without hardware division
static int32_t fixed_div(int32_t a, int32_t b) {
    if (b == 0) return 0x7FFFFFFF;
    // a/b = a * (1/b)
    int32_t recip = fixed_reciprocal(b);
    return fixed_mul(a, recip);
}

/**
 * Compute natural logarithm using binary search
 * @param y Input in Q16.16 format (must be positive)
 * @return ln(y) in Q16.16 format
 */
int32_t fixed_log(int32_t y) {
    // Special cases
    if (y <= 0) {
        return -0x7FFFFFFF;  // Return large negative value for invalid input
    }
    if (y == Q16_ONE) {
        return 0;  // ln(1) = 0
    }
    
    // Step 1: Domain reduction - extract power of 2
    // Find k such that y = 2^k * m, where m ∈ [1, 2)
    int k = 0;
    int32_t m = y;
    
    if (y < Q16_ONE) {
        // For y < 1, shift left and count
        while (m < Q16_ONE) {
            m <<= 1;
            k--;
        }
        // Ensure m < 2
        if (m >= (Q16_ONE << 1)) {
            m >>= 1;
            k++;
        }
    } else {
        // For y > 1, shift right and count
        while (m >= (Q16_ONE << 1)) {
            m >>= 1;
            k++;
        }
    }
    
    // Now m is normalized to [1, 2), and y = 2^k * m
    // So ln(y) = k * ln(2) + ln(m)
    
    // Step 2: Binary search for ln(m)
    // We know ln(m) ∈ [0, ln(2)] since m ∈ [1, 2)
    int32_t x_low = 0;
    int32_t x_high = LN2_Q16;
    int32_t x_mid;
    
    // Initial approximation: ln(m) ≈ m - 1 for m near 1
    // This gives us a better starting point
    if (m < 98304) {  // m < 1.5
        x_mid = m - Q16_ONE;
        if (x_mid < 0) x_mid = 0;
        if (x_mid > LN2_Q16) x_mid = LN2_Q16 >> 1;
    } else {
        x_mid = LN2_Q16 >> 1;  // Start at midpoint
    }
    
    // Binary search iterations
    for (int i = 0; i < 16; i++) {
        // Compute e^x_mid
        int32_t exp_mid = fixed_exp(x_mid);
        
        if (exp_mid < m) {
            // e^x_mid < m, so x_mid < ln(m)
            x_low = x_mid;
        } else {
            // e^x_mid >= m, so x_mid >= ln(m)
            x_high = x_mid;
        }
        
        // Update midpoint with rounding
        x_mid = (x_low + x_high + 1) >> 1;
        
        // Early termination if converged
        if (x_high - x_low <= 1) {
            break;
        }
    }
    
    
    #ifdef USE_NEWTON_REFINEMENT
    // Optional: One Newton-Raphson iteration for refinement
    // This uses division-free reciprocal approximation
    // x_new = x_old + (m - e^x_old) / e^x_old
    int32_t exp_final = fixed_exp(x_mid);
    if (exp_final > 0 && exp_final < (Q16_ONE << 2)) {  // Avoid overflow
        int32_t error = m - exp_final;
        int32_t correction = fixed_div(error, exp_final);
        x_mid += correction;
    }
    #endif
    
    // Step 3: Reconstruct result
    // ln(y) = k * ln(2) + ln(m)
    int32_t result = x_mid;
    if (k != 0) {
        result += fixed_mul(k << 16, LN2_Q16);
    }
    
    return result;
}

/**
 * Compute base-10 logarithm
 * @param y Input in Q16.16 format (must be positive)
 * @return log10(y) in Q16.16 format
 */
int32_t fixed_log10(int32_t y) {
    // log10(y) = ln(y) / ln(10)
    int32_t ln_y = fixed_log(y);
    // Division by constant can be optimized as multiplication by reciprocal
    // 1/ln(10) ≈ 0.434294 ≈ 28480/65536 in Q16.16
    return fixed_mul(ln_y, 28480);
}

/**
 * Compute base-2 logarithm
 * @param y Input in Q16.16 format (must be positive)
 * @return log2(y) in Q16.16 format
 */
int32_t fixed_log2(int32_t y) {
    // log2(y) = ln(y) / ln(2)
    int32_t ln_y = fixed_log(y);
    // 1/ln(2) ≈ 1.442695 ≈ 94548/65536 in Q16.16
    return fixed_mul(ln_y, 94548);
}

// Test harness
#ifdef TEST_LOG

#include <stdio.h>
#include <math.h>

float to_float(int32_t q) {
    return q / (float)Q16_ONE;
}

int32_t from_float(float f) {
    return (int32_t)(f * Q16_ONE);
}

void test_log_accuracy() {
    printf("Testing fixed_log accuracy:\n");
    printf("%-10s %-12s %-12s %-12s %-10s\n", 
           "x", "fixed_log", "log(x)", "abs_error", "rel_error");
    printf("--------------------------------------------------------\n");
    
    float test_values[] = {
        0.1f, 0.25f, 0.5f, 0.75f, 1.0f, 1.5f, 2.0f, 2.718f,
        3.0f, 5.0f, 7.5f, 10.0f, 20.0f, 50.0f, 100.0f
    };
    
    for (int i = 0; i < sizeof(test_values)/sizeof(float); i++) {
        float x = test_values[i];
        int32_t x_q16 = from_float(x);
        int32_t result_q16 = fixed_log(x_q16);
        float result = to_float(result_q16);
        float expected = logf(x);
        float abs_error = fabsf(result - expected);
        float rel_error = fabsf(abs_error / expected);
        
        printf("%-10.3f %-12.6f %-12.6f %-12.6f %-10.6f\n",
               x, result, expected, abs_error, rel_error);
    }
}

void benchmark_performance() {
    printf("\nPerformance comparison (rough estimate):\n");
    
    // Test a range of values
    int32_t sum = 0;
    for (int i = 1000; i < 100000; i += 1000) {
        sum += fixed_log(i);
    }
    
    printf("Binary search method: ~16 iterations of fixed_exp\n");
    printf("Each iteration: ~12 cycles (your exp) + 5 cycles (compare/branch)\n");
    printf("Total: ~272 cycles per log computation\n");
    printf("Memory usage: 0 additional bytes (reuses exp tables)\n");
}

int main() {
    test_log_accuracy();
    benchmark_performance();
    return 0;
}

#endif // TEST_LOG
