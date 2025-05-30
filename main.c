#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "fixed_math.h"

// Helper functions for conversion
float to_float(int32_t q) {
    return q / (float)Q16_ONE;
}

int32_t from_float(float f) {
    return (int32_t)(f * Q16_ONE);
}

int main(void) {
    // Test input: 1.0 in float
    float input_f = 1.0f;
    int32_t input_q = from_float(input_f);

    // Call fixed-point exponential
    int32_t result_q = fixed_exp(input_q);
    float result_f = to_float(result_q);

    // Print results
    printf("Input (float):    %f\n", input_f);
    printf("Input (Q16.16):   0x%08X\n", input_q);
    printf("Result (Q16.16):  0x%08X\n", result_q);
    printf("Result (float):   %f\n", result_f);
    printf("Expected (expf):  %f\n", expf(input_f));

    return 0;
}
