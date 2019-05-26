#include "pch.h"
#include "TypeInfo.h"

TypeInfoNative g_TypeInfoSX(NativeType::SX, "s64", TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoS8(NativeType::S8, "s8", TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoS16(NativeType::S16, "s16", TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoS32(NativeType::S32, "s32", TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoS64(NativeType::S64, "s64", TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoUX(NativeType::UX, "u64", TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoU8(NativeType::U8, "u8", TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoU16(NativeType::U16, "u16", TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoU32(NativeType::U32, "u32", TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoU64(NativeType::U64, "u64", TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoBool(NativeType::Bool, "bool", 0);
TypeInfoNative g_TypeInfoF32(NativeType::F32, "f32", TYPEINFO_FLOAT);
TypeInfoNative g_TypeInfoF64(NativeType::F64, "f64", TYPEINFO_FLOAT);
TypeInfoNative g_TypeInfoFX(NativeType::FX, "f64", TYPEINFO_FLOAT);
TypeInfoNative g_TypeInfoChar(NativeType::Char, "char", 0);
TypeInfoNative g_TypeInfoString(NativeType::String, "string", 0);