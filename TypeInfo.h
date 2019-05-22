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

static const uint64_t TYPEINFO_NATIVE = 0x00000000'00000001;

struct TypeInfo
{
    uint32_t   flags;
    NativeType nativeType;
};

struct NativeTypeInfo : public TypeInfo
{
    NativeTypeInfo(NativeType type)
    {
		nativeType = type;
        flags = TYPEINFO_NATIVE;
    }
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