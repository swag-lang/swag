#include "pch.h"
#include "TypeInfo.h"

NativeTypeInfo g_TypeInfoSX(NativeType::SX, TYPEINFO_INT_SIGNED);
NativeTypeInfo g_TypeInfoS8(NativeType::S8, TYPEINFO_INT_SIGNED);
NativeTypeInfo g_TypeInfoS16(NativeType::S16, TYPEINFO_INT_SIGNED);
NativeTypeInfo g_TypeInfoS32(NativeType::S32, TYPEINFO_INT_SIGNED);
NativeTypeInfo g_TypeInfoS64(NativeType::S64, TYPEINFO_INT_SIGNED);
NativeTypeInfo g_TypeInfoUX(NativeType::UX, TYPEINFO_INT_UNSIGNED);
NativeTypeInfo g_TypeInfoU8(NativeType::U8, TYPEINFO_INT_UNSIGNED);
NativeTypeInfo g_TypeInfoU16(NativeType::U16, TYPEINFO_INT_UNSIGNED);
NativeTypeInfo g_TypeInfoU32(NativeType::U32, TYPEINFO_INT_UNSIGNED);
NativeTypeInfo g_TypeInfoU64(NativeType::U64, TYPEINFO_INT_UNSIGNED);
NativeTypeInfo g_TypeInfoBool(NativeType::Bool, 0);
NativeTypeInfo g_TypeInfoF32(NativeType::F32, TYPEINFO_FLOAT);
NativeTypeInfo g_TypeInfoF64(NativeType::F64, TYPEINFO_FLOAT);
NativeTypeInfo g_TypeInfoFX(NativeType::FX, TYPEINFO_FLOAT);
NativeTypeInfo g_TypeInfoChar(NativeType::Char, 0);
NativeTypeInfo g_TypeInfoString(NativeType::String, 0);