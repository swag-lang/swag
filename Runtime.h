#pragma once

namespace Runtime
{
    int   memcmp(const void* b1, const void* b2, size_t n);
    bool  strcmp(const void* str1, const void* str2, uint32_t num1, uint32_t num2);
    float abs(float value);
} // namespace Runtime