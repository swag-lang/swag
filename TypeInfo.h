#pragma once
enum class NativeType
{
    IntX,
    UIntX,
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Float32,
    Float64,
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