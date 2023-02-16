#include "pch.h"
#include "TypeManager.h"
#include "Tokenizer.h"
#include "Workspace.h"
#include "TypeInfo.h"
#include "Mutex.h"
#include "AstNode.h"

TypeManager* g_TypeMgr = nullptr;

static TypeInfo*                            g_LiteralTypeToType[(int) LiteralType::TT_MAX];
thread_local map<uint32_t, TypeInfoNative*> mapUntypedValuesI;
thread_local map<uint32_t, TypeInfoNative*> mapUntypedValuesB;
thread_local map<uint32_t, TypeInfoNative*> mapUntypedValuesF;

void TypeManager::setup()
{
    typeInfoS8             = new TypeInfoNative(NativeTypeKind::S8, "s8", 1, TYPEINFO_INTEGER | TYPEINFO_CAN_PROMOTE_816);
    typeInfoS16            = new TypeInfoNative(NativeTypeKind::S16, "s16", 2, TYPEINFO_INTEGER | TYPEINFO_CAN_PROMOTE_816);
    typeInfoS32            = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER);
    typeInfoS64            = new TypeInfoNative(NativeTypeKind::S64, "s64", 8, TYPEINFO_INTEGER);
    typeInfoU8             = new TypeInfoNative(NativeTypeKind::U8, "u8", 1, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED | TYPEINFO_CAN_PROMOTE_816);
    typeInfoU16            = new TypeInfoNative(NativeTypeKind::U16, "u16", 2, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED | TYPEINFO_CAN_PROMOTE_816);
    typeInfoU32            = new TypeInfoNative(NativeTypeKind::U32, "u32", 4, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoU64            = new TypeInfoNative(NativeTypeKind::U64, "u64", 8, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoBool           = new TypeInfoNative(NativeTypeKind::Bool, "bool", 1, 0);
    typeInfoF32            = new TypeInfoNative(NativeTypeKind::F32, "f32", 4, TYPEINFO_FLOAT);
    typeInfoF64            = new TypeInfoNative(NativeTypeKind::F64, "f64", 8, TYPEINFO_FLOAT);
    typeInfoRune           = new TypeInfoNative(NativeTypeKind::Rune, "rune", 4, 0);
    typeInfoVoid           = new TypeInfoNative(NativeTypeKind::Void, "void", 0, 0);
    typeInfoString         = new TypeInfoNative(NativeTypeKind::String, "string", 2 * sizeof(Register), 0);
    typeInfoAny            = new TypeInfoNative(NativeTypeKind::Any, "any", 2 * sizeof(Register), 0);
    typeInfoUndefined      = new TypeInfoNative(NativeTypeKind::Undefined, "?", 0, 0);
    typeInfoUntypedInt     = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER | TYPEINFO_UNTYPED_INTEGER);
    typeInfoUntypedBinHexa = new TypeInfoNative(NativeTypeKind::U32, "u32", 4, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED | TYPEINFO_UNTYPED_BINHEXA);
    typeInfoUntypedFloat   = new TypeInfoNative(NativeTypeKind::F32, "f32", 4, TYPEINFO_FLOAT | TYPEINFO_UNTYPED_FLOAT);

    typeInfoVariadic = new TypeInfoVariadic();
    typeInfoVariadic->name.setView("...", 3);
    typeInfoVariadic->sizeOf = 2 * sizeof(Register);

    typeInfoCVariadic       = new TypeInfoVariadic();
    typeInfoCVariadic->kind = TypeInfoKind::CVariadic;
    typeInfoCVariadic->name.setView("cvarargs", 8);
    typeInfoCVariadic->flags |= TYPEINFO_C_VARIADIC;
    typeInfoCVariadic->sizeOf = 0;

    // Some default pointers
    typeInfoNull = new TypeInfoPointer();
    typeInfoNull->name.setView("null", 4);
    typeInfoNull->sizeOf = sizeof(Register);

    typeInfoCString         = new TypeInfoPointer();
    typeInfoCString->sizeOf = sizeof(Register);
    typeInfoCString->setConst();
    typeInfoCString->flags |= TYPEINFO_C_STRING;
    typeInfoCString->pointedType = typeInfoU8;
    typeInfoCString->name.setView("cstring", 7);

    // The rest
    typeInfoCode = new TypeInfoCode();

    typeInfoOpCall             = new TypeInfoFuncAttr();
    typeInfoOpCall->returnType = typeInfoVoid;
    typeInfoOpCall->parameters.push_back(new TypeInfoParam());
    auto typePtr         = new TypeInfoPointer();
    typePtr->pointedType = typeInfoVoid;
    typePtr->sizeOf      = sizeof(void*);
    typePtr->computeName();
    typeInfoOpCall->parameters[0]->typeInfo = typePtr;
    typeInfoOpCall->computeName();

    typeInfoOpCall2             = new TypeInfoFuncAttr();
    typeInfoOpCall2->returnType = typeInfoVoid;
    typeInfoOpCall2->parameters.push_back(new TypeInfoParam());
    typeInfoOpCall2->parameters.push_back(new TypeInfoParam());
    typeInfoOpCall2->parameters[0]->typeInfo = typePtr;
    typeInfoOpCall2->parameters[1]->typeInfo = typePtr;
    typeInfoOpCall2->computeName();

    typeInfoModuleCall             = new TypeInfoFuncAttr();
    typeInfoModuleCall->returnType = typeInfoVoid;
    typeInfoModuleCall->parameters.push_back(new TypeInfoParam());
    typeInfoModuleCall->parameters[0]->typeInfo = typePtr;
    typeInfoModuleCall->computeName();

    typeInfoSliceRunes              = new TypeInfoSlice();
    typeInfoSliceRunes->pointedType = typeInfoRune;
    typeInfoSliceRunes->flags |= TYPEINFO_CONST;
    typeInfoSliceRunes->computeName();

    memset(g_LiteralTypeToType, 0, sizeof(g_LiteralTypeToType));
    g_LiteralTypeToType[(int) LiteralType::TT_U8]              = typeInfoU8;
    g_LiteralTypeToType[(int) LiteralType::TT_U16]             = typeInfoU16;
    g_LiteralTypeToType[(int) LiteralType::TT_U32]             = typeInfoU32;
    g_LiteralTypeToType[(int) LiteralType::TT_U64]             = typeInfoU64;
    g_LiteralTypeToType[(int) LiteralType::TT_S8]              = typeInfoS8;
    g_LiteralTypeToType[(int) LiteralType::TT_S16]             = typeInfoS16;
    g_LiteralTypeToType[(int) LiteralType::TT_S32]             = typeInfoS32;
    g_LiteralTypeToType[(int) LiteralType::TT_S64]             = typeInfoS64;
    g_LiteralTypeToType[(int) LiteralType::TT_F32]             = typeInfoF32;
    g_LiteralTypeToType[(int) LiteralType::TT_F64]             = typeInfoF64;
    g_LiteralTypeToType[(int) LiteralType::TT_BOOL]            = typeInfoBool;
    g_LiteralTypeToType[(int) LiteralType::TT_RUNE]            = typeInfoRune;
    g_LiteralTypeToType[(int) LiteralType::TT_STRING]          = typeInfoString;
    g_LiteralTypeToType[(int) LiteralType::TT_RAW_STRING]      = typeInfoString;
    g_LiteralTypeToType[(int) LiteralType::TT_ESCAPE_STRING]   = typeInfoString;
    g_LiteralTypeToType[(int) LiteralType::TT_VOID]            = typeInfoVoid;
    g_LiteralTypeToType[(int) LiteralType::TT_NULL]            = typeInfoNull;
    g_LiteralTypeToType[(int) LiteralType::TT_ANY]             = typeInfoAny;
    g_LiteralTypeToType[(int) LiteralType::TT_UNTYPED_INT]     = typeInfoUntypedInt;
    g_LiteralTypeToType[(int) LiteralType::TT_UNTYPED_FLOAT]   = typeInfoUntypedFloat;
    g_LiteralTypeToType[(int) LiteralType::TT_UNTYPED_BINHEXA] = typeInfoUntypedBinHexa;
    g_LiteralTypeToType[(int) LiteralType::TT_CSTRING]         = typeInfoCString;
    g_LiteralTypeToType[(int) LiteralType::TT_TYPE]            = nullptr; // will be done with registerTypeType

    // Promotion matrix to force 32 or 64 bits
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::U8]  = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::U16] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::U32] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::S8]  = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::S16] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::S32] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::U8][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::U8]  = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::U16] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::U32] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::S8]  = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::S16] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::S32] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::U16][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::U8]  = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::U16] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::U32] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::S8]  = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::S16] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::S32] = typeInfoU32;
    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::U32][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::U8]  = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::U16] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::U32] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::S8]  = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::S16] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::S32] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::F32] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::U64][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::U8]  = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::U16] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::U32] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::S8]  = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::S16] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::S32] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::S8][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::U8]  = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::U16] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::U32] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::S8]  = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::S16] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::S32] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::S16][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::U8]  = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::U16] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::U32] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::S8]  = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::S16] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::S32] = typeInfoS32;
    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::S32][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::U8]  = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::U16] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::U32] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::S8]  = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::S16] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::S32] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::F32] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::S64][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::U8]  = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::U16] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::U32] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::U64] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::S8]  = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::S16] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::S32] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::S64] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix3264[(int) NativeTypeKind::F32][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::U8]  = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::U16] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::U32] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::U64] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::S8]  = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::S16] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::S32] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::S64] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::F32] = typeInfoF64;
    promoteMatrix3264[(int) NativeTypeKind::F64][(int) NativeTypeKind::F64] = typeInfoF64;

    // Promotion matrix
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::U8]  = typeInfoU8;
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::U16] = typeInfoU16;
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::U32] = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::S8]  = typeInfoU8;
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::S16] = typeInfoU16;
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::S32] = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::U8][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::U8]  = typeInfoU16;
    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::U16] = typeInfoU16;
    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::U32] = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::S8]  = typeInfoU16;
    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::S16] = typeInfoU16;
    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::S32] = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::U16][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::U8]  = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::U16] = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::U32] = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::S8]  = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::S16] = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::S32] = typeInfoU32;
    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::U32][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::U8]  = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::U16] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::U32] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::S8]  = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::S16] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::S32] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::F32] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::U64][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::U8]  = typeInfoS8;
    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::U16] = typeInfoS16;
    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::U32] = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::S8]  = typeInfoS8;
    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::S16] = typeInfoS16;
    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::S32] = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::S8][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::U8]  = typeInfoS16;
    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::U16] = typeInfoS16;
    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::U32] = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::S8]  = typeInfoS16;
    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::S16] = typeInfoS16;
    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::S32] = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::S16][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::U8]  = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::U16] = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::U32] = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::S8]  = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::S16] = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::S32] = typeInfoS32;
    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::S32][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::U8]  = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::U16] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::U32] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::S8]  = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::S16] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::S32] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::F32] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::S64][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::U8]  = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::U16] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::U32] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::U64] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::S8]  = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::S16] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::S32] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::S64] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix816[(int) NativeTypeKind::F32][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::U8]  = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::U16] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::U32] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::U64] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::S8]  = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::S16] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::S32] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::S64] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::F32] = typeInfoF64;
    promoteMatrix816[(int) NativeTypeKind::F64][(int) NativeTypeKind::F64] = typeInfoF64;
}

TypeInfoArray* TypeManager::convertTypeListToArray(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant)
{
    auto      typeArray    = makeType<TypeInfoArray>();
    auto      orgTypeArray = typeArray;
    TypeInfo* finalType    = nullptr;

    while (true)
    {
        typeArray->pointedType = solidifyUntyped(typeList->subTypes.front()->typeInfo);
        finalType              = typeArray->pointedType;
        typeArray->sizeOf      = typeList->sizeOf;
        typeArray->count       = (uint32_t) typeList->subTypes.size();
        typeArray->totalCount  = typeArray->count;
        if (isCompilerConstant)
            typeArray->flags |= TYPEINFO_CONST;
        if (!typeArray->pointedType->isListArray())
            break;
        typeList               = CastTypeInfo<TypeInfoList>(typeArray->pointedType, TypeInfoKind::TypeListArray);
        typeArray->pointedType = makeType<TypeInfoArray>();
        typeArray              = (TypeInfoArray*) typeArray->pointedType;
    }

    // Compute all the type names
    typeArray = orgTypeArray;
    while (typeArray)
    {
        typeArray->finalType = finalType;
        typeArray->computeName();
        if (!typeArray->pointedType->isArray())
            break;
        typeArray = CastTypeInfo<TypeInfoArray>(typeArray->pointedType, TypeInfoKind::Array);
    }

    orgTypeArray->sizeOf = orgTypeArray->finalType->sizeOf * orgTypeArray->totalCount;
    return orgTypeArray;
}

TypeInfoStruct* TypeManager::convertTypeListToStruct(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant)
{
    auto typeStruct  = makeType<TypeInfoStruct>();
    typeStruct->name = typeList->computeTupleName(context);
    typeStruct->flags |= TYPEINFO_STRUCT_IS_TUPLE;

    typeStruct->fields.reserve((int) typeList->subTypes.size());
    for (int idx = 0; idx < typeList->subTypes.size(); idx++)
    {
        auto one = typeList->subTypes[idx];
        if (one->namedParam.empty())
            one->namedParam = Fmt("item%d", idx);
        typeStruct->fields.push_back((TypeInfoParam*) one->clone());
    }

    typeStruct->structName = typeStruct->name;
    return typeStruct;
}

TypeInfo* TypeManager::solidifyUntyped(TypeInfo* typeInfo)
{
    if (typeInfo->isUntypedInteger())
        return g_TypeMgr->typeInfoS32;
    if (typeInfo->isUntypedFloat())
        return g_TypeMgr->typeInfoF32;
    return typeInfo;
}

TypeInfo* TypeManager::resolveUntypedType(TypeInfo* typeInfo, uint32_t value)
{
    if (typeInfo->isUntypedInteger())
    {
        auto it = mapUntypedValuesI.find(value);
        if (it != mapUntypedValuesI.end())
        {
            SWAG_ASSERT(it->second->isUntypedInteger());
            SWAG_ASSERT(it->second->valueInteger == *(int32_t*) &value);
            return it->second;
        }

        TypeInfoNative* newType  = (TypeInfoNative*) typeInfo->clone();
        newType->valueInteger    = *(int32_t*) &value;
        typeInfo                 = newType;
        mapUntypedValuesI[value] = newType;
    }
    else if (typeInfo->isUntypedBinHex())
    {
        auto it = mapUntypedValuesB.find(value);
        if (it != mapUntypedValuesB.end())
        {
            SWAG_ASSERT(it->second->isUntypedBinHex());
            SWAG_ASSERT(it->second->valueInteger == *(int32_t*) &value);
            return it->second;
        }

        TypeInfoNative* newType  = (TypeInfoNative*) typeInfo->clone();
        newType->valueInteger    = *(int32_t*) &value;
        typeInfo                 = newType;
        mapUntypedValuesB[value] = newType;
    }
    else if (typeInfo->isUntypedFloat())
    {
        auto it = mapUntypedValuesF.find(value);
        if (it != mapUntypedValuesF.end())
        {
            SWAG_ASSERT(it->second->isUntypedFloat());
            SWAG_ASSERT(it->second->valueFloat == *(float*) &value);
            return it->second;
        }

        TypeInfoNative* newType  = (TypeInfoNative*) typeInfo->clone();
        newType->valueFloat      = *(float*) &value;
        typeInfo                 = newType;
        mapUntypedValuesF[value] = newType;
    }

    return typeInfo;
}

TypeInfo* TypeManager::literalTypeToType(LiteralType literalType)
{
    SWAG_ASSERT(literalType < LiteralType::TT_MAX);
    auto result = g_LiteralTypeToType[(int) literalType];
    SWAG_ASSERT(result);
    return result;
}

TypeInfo* TypeManager::literalTypeToType(LiteralType literalType, Register literalValue)
{
    auto result = literalTypeToType(literalType);
    SWAG_ASSERT(result);
    result = resolveUntypedType(result, literalValue.u32);
    return result;
}

TypeInfo* TypeManager::makeConst(TypeInfo* typeInfo)
{
    ScopedLock lk(typeInfo->mutex);
    if (typeInfo->isConst())
        return typeInfo;

    TypeInfo* typeConst;
    if (typeInfo->isStruct())
    {
        auto typeAlias = makeType<TypeInfoAlias>();
        typeAlias->copyFrom(typeInfo);
        typeAlias->rawType = typeInfo;
        typeAlias->flags |= TYPEINFO_CONST | TYPEINFO_FAKE_ALIAS;
        typeConst = typeAlias;
    }
    else
    {
        typeConst = typeInfo->clone();
        typeConst->setConst();
    }

    return typeConst;
}

uint64_t TypeManager::align(uint64_t value, uint32_t align)
{
    SWAG_ASSERT(align);
    if (align == 1)
        return value;
    auto alignValue = value & ~((uint64_t) align - 1);
    if (alignValue < value)
        alignValue += align;
    return alignValue;
}

uint32_t TypeManager::alignOf(TypeInfo* typeInfo)
{
    if (typeInfo->isStruct())
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct->sizeOf);
        SWAG_ASSERT(typeStruct->alignOf);
        return typeStruct->alignOf;
    }
    else if (typeInfo->isArray())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        return alignOf(typeArray->finalType);
    }
    else if (typeInfo->isPointer())
    {
        return typeInfo->sizeOf;
    }
    else if (typeInfo->isSlice() ||
             typeInfo->isInterface() ||
             typeInfo->isAny() ||
             typeInfo->isString())
    {
        return sizeof(void*);
    }

    return max(1, typeInfo->sizeOf);
}

void TypeManager::registerTypeType()
{
    typeInfoTypeType                                = makePointerTo(g_Workspace->swagScope.regTypeInfo, TYPEINFO_CONST);
    g_LiteralTypeToType[(int) LiteralType::TT_TYPE] = typeInfoTypeType;
}

TypeInfoPointer* TypeManager::makePointerTo(TypeInfo* toType, uint64_t ptrFlags)
{
    ScopedLock lk(mutex);

    auto ptrType         = makeType<TypeInfoPointer>();
    ptrType->pointedType = toType;
    ptrType->sizeOf      = sizeof(Register);
    ptrType->flags       = ptrFlags;
    ptrType->computeName();
    return ptrType;
}

TypeInfoParam* TypeManager::makeParam()
{
    auto typeParam = g_Allocator.alloc<TypeInfoParam>();
    if (g_CommandLine.stats)
        g_Stats.memParams += Allocator::alignSize(sizeof(TypeInfoParam));
    return typeParam;
}

void TypeManager::convertStructParamToRef(AstNode* node, TypeInfo* typeInfo)
{
    SWAG_ASSERT(node->kind == AstNodeKind::FuncDeclParam);

    // A struct/interface is forced to be a const reference
    if (!node->typeInfo->isGeneric())
    {
        if (typeInfo->isStruct())
        {
            // If this has been transformed to an alias cause of const, take the original
            // type to make the reference
            if (typeInfo->flags & TYPEINFO_FAKE_ALIAS)
                typeInfo = ((TypeInfoAlias*) typeInfo)->rawType;

            auto typeRef   = makeType<TypeInfoPointer>();
            typeRef->flags = typeInfo->flags | TYPEINFO_CONST | TYPEINFO_POINTER_REF | TYPEINFO_POINTER_AUTO_REF;
            typeRef->flags &= ~TYPEINFO_RETURN_BY_COPY;
            typeRef->pointedType = typeInfo;
            typeRef->sizeOf      = sizeof(void*);
            typeRef->computeName();
            node->typeInfo = typeRef;
        }
    }
}
