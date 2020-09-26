#pragma once

namespace Runtime
{
    int   memcmp(const void* b1, const void* b2, size_t n);
    bool  strcmp(const void* str1, const void* str2, uint32_t num1, uint32_t num2);
    float abs(float value);
    void  print(const void* message, uint32_t len);
    void  print(int64_t value);
    void  print(double value);
} // namespace Runtime