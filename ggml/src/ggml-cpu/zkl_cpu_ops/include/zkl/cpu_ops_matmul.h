#pragma once

#include <cstdint>

namespace zkl {

/**
 * @brief Matrix multiplication: C = A * B
 * @param c Output matrix (m x n)
 * @param a Input matrix A (m x k)
 * @param b Input matrix B (k x n)
 * @param m Number of rows in A (and C)
 * @param n Number of columns in B (and C)
 * @param k Number of columns in A (and rows in B)
 * 
 * All matrices are stored in row-major order
 */
void matmul(float* c, const float* a, const float* b, 
            int64_t m, int64_t n, int64_t k);

void matmul_cpu_fp32(float* c, const float* a, const float* b, 
            int64_t m, int64_t n, int64_t k);

void matmul_cpu_fp16(float* c, const float* a, const float* b, 
            int64_t m, int64_t n, int64_t k);

void matmul_rpu_fp16(float* c, const float* a, const float* b, 
            int64_t m, int64_t n, int64_t k);

void matmul_rpu_bf16(float* c, const float* a, const float* b, 
            int64_t m, int64_t n, int64_t k);

void matmul_rpu_fp8_e4m3(float* c, const float* a, const float* b, 
            int64_t m, int64_t n, int64_t k);

void matmul_rpu_fp8_e5m2(float* c, const float* a, const float* b, 
            int64_t m, int64_t n, int64_t k);

void matmul_rpu_fp4(float* c, const float* a, const float* b, 
            int64_t m, int64_t n, int64_t k);

void matmul_rpu_int8(float* c, const float* a, const float* b, 
            int64_t m, int64_t n, int64_t k);

void matmul_rpu_int8(int8_t* c, const int8_t* a, const int8_t* b, 
            int64_t m, int64_t n, int64_t k);

} // namespace zkl
