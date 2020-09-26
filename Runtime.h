#pragma once

namespace Runtime
{
    int   memcmp(const void* b1, const void* b2, size_t n);
    bool  strcmp(const void* str1, const void* str2, uint32_t num1, uint32_t num2);
    float abs(float value);
    void  print(const void* message, uint32_t len);
    void  print(int64_t value);
    void  print(double value);

    static const uint32_t COMPARE_STRICT   = 0x00000000;
    static const uint32_t COMPARE_CAST_ANY = 0x00000001;
    bool                  compareType(const void* type1, const void* type2, uint32_t flags);
    void*                 interfaceOf(const void* structType, const void* itfType);

} // namespace Runtime