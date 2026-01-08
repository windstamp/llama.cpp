#include "external_matmul.h"
#include "zkl/cpu_ops.h"

#include <cstdio>

// External matmul function: c = a * b
// a: m x k matrix (row-major)
// b: k x n matrix (row-major)
// c: m x n matrix (row-major)
// 
// This implementation calls zkl::matmul from the ZKL CPU ops library
void external_matmul(float *c, const float *a, const float *b, int64_t m, int64_t n, int64_t k) {
    // printf("%s:%d | %s\n", __FILE__, __LINE__, __func__);
    
    // if(true) { return; }

#if 0
    for (int64_t i = 0; i < m; i++) {
        for (int64_t j = 0; j < n; j++) {
            float sum = 0.0f;
            for (int64_t p = 0; p < k; p++) {
                sum += a[i * k + p] * b[p * n + j];
            }
            c[i * n + j] = sum;
        }
    }
#else
    zkl::matmul(c, a, b, m, n, k);
#endif
}

void external_matmul_fp32(float *c, const float *a, const float *b, int64_t m, int64_t n, int64_t k) {
    return external_matmul(c, a, b, m, n, k);
}

void external_matmul_fp16(float *c, const float *a, const float *b, int64_t m, int64_t n, int64_t k) {
    return zkl::matmul_rpu_fp16(c, a, b, m, n, k);
}

void external_matmul_bf16(float *c, const float *a, const float *b, int64_t m, int64_t n, int64_t k) {
    return zkl::matmul_rpu_bf16(c, a, b, m, n, k);
}
