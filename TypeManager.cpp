#include "pch.h"
#include "TypeManager.h"
#include "TypeInfo.h"

void TypeManager::setup()
{
    typeInfoSX     = new TypeInfoNative(NativeType::SX, "s64", TYPEINFO_INT_SIGNED);
    typeInfoS8     = new TypeInfoNative(NativeType::S8, "s8", TYPEINFO_INT_SIGNED);
    typeInfoS16    = new TypeInfoNative(NativeType::S16, "s16", TYPEINFO_INT_SIGNED);
    typeInfoS32    = new TypeInfoNative(NativeType::S32, "s32", TYPEINFO_INT_SIGNED);
    typeInfoS64    = new TypeInfoNative(NativeType::S64, "s64", TYPEINFO_INT_SIGNED);
    typeInfoUX     = new TypeInfoNative(NativeType::UX, "u64", TYPEINFO_INT_UNSIGNED);
    typeInfoU8     = new TypeInfoNative(NativeType::U8, "u8", TYPEINFO_INT_UNSIGNED);
    typeInfoU16    = new TypeInfoNative(NativeType::U16, "u16", TYPEINFO_INT_UNSIGNED);
    typeInfoU32    = new TypeInfoNative(NativeType::U32, "u32", TYPEINFO_INT_UNSIGNED);
    typeInfoU64    = new TypeInfoNative(NativeType::U64, "u64", TYPEINFO_INT_UNSIGNED);
    typeInfoBool   = new TypeInfoNative(NativeType::Bool, "bool", 0);
    typeInfoF32    = new TypeInfoNative(NativeType::F32, "f32", TYPEINFO_FLOAT);
    typeInfoF64    = new TypeInfoNative(NativeType::F64, "f64", TYPEINFO_FLOAT);
    typeInfoFX     = new TypeInfoNative(NativeType::FX, "f64", TYPEINFO_FLOAT);
    typeInfoChar   = new TypeInfoNative(NativeType::Char, "char", 0);
    typeInfoString = new TypeInfoNative(NativeType::String, "string", 0);
    typeInfoVoid   = new TypeInfoNative(NativeType::Void, "void", 0);
}

TypeInfo* TypeManager::flattenType(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Enum)
        return typeInfo;
    return static_cast<TypeInfoEnum*>(typeInfo)->rawType;
}
