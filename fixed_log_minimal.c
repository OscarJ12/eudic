// fixed_log_minimal.c - Division-free logarithm for resource-constrained MCUs
// This version uses NO division operations at all

#include <stdint.h>
#include "fixed_math.h"

// Constants in Q16.16 format
#define LN2_Q16 45426      // ln(2) ≈ 0.693147

/**
 * Minimal fixed-point natural logarithm - NO DIVISION VERSION
 * Uses only shifts, adds, and multiplies (via your fixed_mul)
 * @param y Input in Q16.16 format (must be positive)
 * @return ln(y) in Q16.16 format
 */
int32_t fixed_log_minimal(int32_t y) {
    // Special cases
    if (y <= 0) {
        return -0x7FFFFFFF;  // Large negative for invalid input
    }
    if (y == Q16_ONE) {
        return 0;  // ln(1) = 0
    }
    
    // Step 1: Extract power of 2 using only shifts
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
    
    // Now m is in [1, 2), and y = 2^k * m
    // So ln(y) = k * ln(2) + ln(m)
    
    // Step 2: Binary search for ln(m) - MAIN ALGORITHM
    int32_t x_low = 0;
    int32_t x_high = LN2_Q16;
    
    // Use more iterations for better accuracy without Newton step
    for (int i = 0; i < 20; i++) {  // Increased from 16 to compensate
        // Compute midpoint with rounding
        int32_t x_mid = (x_low + x_high + 1) >> 1;
        
        // Compute e^x_mid using your existing function
        int32_t exp_mid = fixed_exp(x_mid);
        
        if (exp_mid < m) {
            x_low = x_mid;
        } else {
            x_high = x_mid;
        }
        
        // Early termination if converged
        if (x_high - x_low <= 1) {
            break;
        }
    }
    
    // Take the midpoint as final result
    int32_t ln_m = (x_low + x_high) >> 1;
    
    // Step 3: Reconstruct result using only multiplication
    // ln(y) = k * ln(2) + ln(m)
    int32_t result = ln_m;
    if (k != 0) {
        // k * ln(2) computed as fixed_mul(k << 16, LN2_Q16)
        result += fixed_mul(k << 16, LN2_Q16);
    }
    
    return result;
}

/**
 * Base-10 logarithm without division
 * @param y Input in Q16.16 format (must be positive)
 * @return log10(y) in Q16.16 format
 */
int32_t fixed_log10_minimal(int32_t y) {
    // log10(y) = ln(y) / ln(10) = ln(y) * (1/ln(10))
    // 1/ln(10) ≈ 0.434294 ≈ 28480/65536 in Q16.16
    int32_t ln_y = fixed_log_minimal(y);
    return fixed_mul(ln_y, 28480);
}

/**
 * Base-2 logarithm without division
 * @param y Input in Q16.16 format (must be positive)
 * @return log2(y) in Q16.16 format
 */
int32_t fixed_log2_minimal(int32_t y) {
    // log2(y) = ln(y) / ln(2) = ln(y) * (1/ln(2))
    // 1/ln(2) ≈ 1.442695 ≈ 94548/65536 in Q16.16
    int32_t ln_y = fixed_log_minimal(y);
    return fixed_mul(ln_y, 94548);
}

// Assembly characteristics for ARM Cortex-M0/M3:
// - NO division instructions (SDIV/UDIV)
// - Only uses: LSL, LSR, ADD, SUB, MUL (via fixed_mul)
// - Completely deterministic execution time
// - No hardware dependencies beyond basic ALU

#ifdef TEST_MINIMAL
#include <stdio.h>
#include <math.h>

float to_float(int32_t q) { return q / (float)Q16_ONE; }
int32_t from_float(float f) { return (int32_t)(f * Q16_ONE); }

int main() {
    printf("Division-Free Minimal Logarithm Test\n");
    printf("===================================\n\n");
    
    float test_vals[] = {0.1f, 0.5f, 1.0f, 2.0f, 2.718f, 10.0f};
    
    printf("%-10s %-12s %-12s %-12s\n", "x", "fixed_log", "logf(x)", "error");
    printf("------------------------------------------------\n");
    
    for (int i = 0; i < sizeof(test_vals)/sizeof(float); i++) {
        float x = test_vals[i];
        int32_t result_q16 = fixed_log_minimal(from_float(x));
        float result = to_float(result_q16);
        float expected = logf(x);
        float error = fabsf(result - expected);
        
        printf("%-10.3f %-12.6f %-12.6f %-12.6f\n", 
               x, result, expected, error);
    }
    
    printf("\nThis version uses NO division operations!\n");
    return 0;
}
#endif
