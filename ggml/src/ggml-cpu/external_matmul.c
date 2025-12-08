#include "external_matmul.h"

#include <stdio.h>
#include <stdbool.h>

// External matmul function: c = a * b
// a: m x k matrix (row-major)
// b: k x n matrix (row-major)
// c: m x n matrix (row-major)
void external_matmul(float *c, const float *a, const float *b, int64_t m, int64_t n, int64_t k) {
    // printf("%s:%d | %s\n", __FILE__, __LINE__, __func__);
    
    // if(true) { return; }

    for (int64_t i = 0; i < m; i++) {
        for (int64_t j = 0; j < n; j++) {
            float sum = 0.0f;
            for (int64_t p = 0; p < k; p++) {
                sum += a[i * k + p] * b[p * n + j];
            }
            c[i * n + j] = sum;
        }
    }
}
