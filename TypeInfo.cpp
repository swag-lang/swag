#include "pch.h"
#include "TypeInfo.h"

TypeInfoNative g_TypeInfoSX(NativeType::SX, TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoS8(NativeType::S8, TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoS16(NativeType::S16, TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoS32(NativeType::S32, TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoS64(NativeType::S64, TYPEINFO_INT_SIGNED);
TypeInfoNative g_TypeInfoUX(NativeType::UX, TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoU8(NativeType::U8, TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoU16(NativeType::U16, TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoU32(NativeType::U32, TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoU64(NativeType::U64, TYPEINFO_INT_UNSIGNED);
TypeInfoNative g_TypeInfoBool(NativeType::Bool, 0);
TypeInfoNative g_TypeInfoF32(NativeType::F32, TYPEINFO_FLOAT);
TypeInfoNative g_TypeInfoF64(NativeType::F64, TYPEINFO_FLOAT);
TypeInfoNative g_TypeInfoFX(NativeType::FX, TYPEINFO_FLOAT);
TypeInfoNative g_TypeInfoChar(NativeType::Char, 0);
TypeInfoNative g_TypeInfoString(NativeType::String, 0);