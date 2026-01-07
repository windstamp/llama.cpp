#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// External matmul function: c = a * b
// a: m x k matrix (row-major)
// b: k x n matrix (row-major)
// c: m x n matrix (row-major)
void external_matmul(float *c, const float *a, const float *b, int64_t m, int64_t n, int64_t k);

void external_matmul_fp32(float *c, const float *a, const float *b, int64_t m, int64_t n, int64_t k);
void external_matmul_fp16(float *c, const float *a, const float *b, int64_t m, int64_t n, int64_t k);
void external_matmul_bf16(float *c, const float *a, const float *b, int64_t m, int64_t n, int64_t k);

#ifdef __cplusplus
}
#endif
