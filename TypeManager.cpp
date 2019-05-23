#include "pch.h"
#include "AstNode.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "Global.h"

string TypeManager::nativeTypeName(TypeInfo* type)
{
    switch (type->nativeType)
    {
    case NativeType::Bool:
        return "bool";
    case NativeType::Char:
        return "char";
    case NativeType::String:
        return "string";
    case NativeType::F32:
        return "f32";
    case NativeType::F64:
	case NativeType::FX:
        return "f64";
    case NativeType::S8:
        return "s8";
    case NativeType::S16:
        return "s16";
    case NativeType::S32:
        return "s32";
    case NativeType::S64:
    case NativeType::SX:
        return "s64";
    case NativeType::U8:
        return "u8";
    case NativeType::U16:
        return "u16";
    case NativeType::U32:
        return "u32";
    case NativeType::U64:
    case NativeType::UX:
        return "u64";
    }

    return "???";
}
