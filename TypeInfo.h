#pragma once
enum class NativeType
{
    SX,
    UX,
    S8,
    S16,
    S32,
    S64,
    U8,
    U16,
    U32,
    U64,
    F32,
    F64,
    Bool,
    Char,
    String,
};

struct TypeInfo
{
};

struct NativeTypeInfo
{
    NativeTypeInfo(NativeType type)
        : type{type} {};

    NativeType type;
};

extern NativeTypeInfo g_TypeInfoSX;
extern NativeTypeInfo g_TypeInfoS8;
extern NativeTypeInfo g_TypeInfoS16;
extern NativeTypeInfo g_TypeInfoS32;
extern NativeTypeInfo g_TypeInfoS64;
extern NativeTypeInfo g_TypeInfoUX;
extern NativeTypeInfo g_TypeInfoU8;
extern NativeTypeInfo g_TypeInfoU16;
extern NativeTypeInfo g_TypeInfoU32;
extern NativeTypeInfo g_TypeInfoU64;
extern NativeTypeInfo g_TypeInfoBool;
extern NativeTypeInfo g_TypeInfoF32;
extern NativeTypeInfo g_TypeInfoF64;
extern NativeTypeInfo g_TypeInfoChar;
extern NativeTypeInfo g_TypeInfoString;