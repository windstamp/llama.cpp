#ifndef ZKL_CPU_OPS_H
#define ZKL_CPU_OPS_H

#include <cstdint>

namespace zkl {

/**
 * @brief Element-wise addition of two tensors
 * @param c Output tensor (c = a + b)
 * @param a Input tensor A
 * @param b Input tensor B
 * @param size Number of elements
 */
void add(float* c, const float* a, const float* b, int64_t size);

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

} // namespace zkl

#endif // ZKL_CPU_OPS_H
