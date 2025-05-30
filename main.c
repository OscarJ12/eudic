#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "fixed_math.h"

// Convert Q16.16 to float
float to_float(int32_t q) {
    return q / (float)Q16_ONE;
}

// Convert float to Q16.16
int32_t from_float(float f) {
    return (int32_t)(f * Q16_ONE);
}

int main(void) {
    float input_f;
    printf("e^x\n");
    printf("Input x: ");
    if (scanf("%f", &input_f) != 1) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }

    int32_t input_q = from_float(input_f);

    // **Use the signed version** so negatives go through the lookup + interp path
    int32_t result_q = fixed_exp_signed(input_q);
    float result_f = to_float(result_q);

    printf("Input (float):    %f\n", input_f);
    printf("Input (Q16.16):   0x%08X\n", input_q);
    printf("Result (Q16.16):  0x%08X\n", result_q);
    printf("Result (float):   %f\n", result_f);
    printf("Expected (expf):  %f\n", expf(input_f));

    return 0;
}

