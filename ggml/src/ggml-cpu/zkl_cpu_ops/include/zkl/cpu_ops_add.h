#pragma once

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

} // namespace zkl
