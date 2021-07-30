#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64FunctionBodyJob.h"
#include "BackendX64_Macros.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "Version.h"
#include "Workspace.h"
#include "LanguageSpec.h"

// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h

const uint32_t DEBUG_SECTION_MAGIC = 4;

const uint32_t SUBSECTION_SYMBOL        = 0xF1;
const uint32_t SUBSECTION_LINES         = 0xF2;
const uint32_t SUBSECTION_STRING_TABLE  = 0xF3;
const uint32_t SUBSECTION_FILE_CHECKSUM = 0xF4;

const uint16_t S_END                       = 0x0006;
const uint16_t S_FRAMEPROC                 = 0x1012;
const uint16_t S_BLOCK32                   = 0x1103;
const uint16_t S_COMPILE3                  = 0x113c;
const uint16_t S_LPROC32_ID                = 0x1146;
const uint16_t S_GPROC32_ID                = 0x1147;
const uint16_t S_PROC_ID_END               = 0x114F;
const uint16_t S_LOCAL                     = 0x113E;
const uint16_t S_DEFRANGE                  = 0x113F;
const uint16_t S_DEFRANGE_REGISTER         = 0x1141;
const uint16_t S_DEFRANGE_FRAMEPOINTER_REL = 0x1142;
const uint16_t S_DEFRANGE_REGISTER_REL     = 0x1145;
const uint16_t S_CONSTANT                  = 0x1107;
const uint16_t S_LDATA32                   = 0x110C;

const uint16_t LF_POINTER      = 0x1002;
const uint16_t LF_PROCEDURE    = 0x1008;
const uint16_t LF_MFUNCTION    = 0x1009;
const uint16_t LF_ARGLIST      = 0x1201;
const uint16_t LF_FIELDLIST    = 0x1203;
const uint16_t LF_ENUMERATE    = 0x1502;
const uint16_t LF_ARRAY        = 0x1503;
const uint16_t LF_STRUCTURE    = 0x1505;
const uint16_t LF_ENUM         = 0x1507;
const uint16_t LF_MEMBER       = 0x150d;
const uint16_t LF_METHOD       = 0x150f;
const uint16_t LF_ONEMETHOD    = 0x1511;
const uint16_t LF_FUNC_ID      = 0x1601;
const uint16_t LF_MFUNC_ID     = 0x1602;
const uint16_t LF_UDT_SRC_LINE = 0x1606;

const uint16_t LF_NUMERIC   = 0x8000;
const uint16_t LF_CHAR      = 0x8000;
const uint16_t LF_SHORT     = 0x8001;
const uint16_t LF_USHORT    = 0x8002;
const uint16_t LF_LONG      = 0x8003;
const uint16_t LF_ULONG     = 0x8004;
const uint16_t LF_REAL32    = 0x8005;
const uint16_t LF_REAL64    = 0x8006;
const uint16_t LF_REAL80    = 0x8007;
const uint16_t LF_REAL128   = 0x8008;
const uint16_t LF_QUADWORD  = 0x8009;
const uint16_t LF_UQUADWORD = 0x800a;

const uint16_t R_RDX = 331;
const uint16_t R_RDI = 333;
const uint16_t R_RSP = 335;

enum SimpleTypeKind : DbgTypeIndex
{
    None          = 0x0000, // uncharacterized type (no type)
    Void          = 0x0003, // void
    NotTranslated = 0x0007, // type not translated by cvpack
    HResult       = 0x0008, // OLE/COM HRESULT

    SignedCharacter   = 0x0010, // 8 bit signed
    UnsignedCharacter = 0x0020, // 8 bit unsigned
    NarrowCharacter   = 0x0070, // really a char
    WideCharacter     = 0x0071, // wide char
    Character16       = 0x007a, // uint16_t
    Character32       = 0x007b, // uint32_t

    SByte       = 0x0068, // 8 bit signed int
    Byte        = 0x0069, // 8 bit unsigned int
    Int16Short  = 0x0011, // 16 bit signed
    UInt16Short = 0x0021, // 16 bit unsigned
    Int16       = 0x0072, // 16 bit signed int
    UInt16      = 0x0073, // 16 bit unsigned int
    Int32Long   = 0x0012, // 32 bit signed
    UInt32Long  = 0x0022, // 32 bit unsigned
    Int32       = 0x0074, // 32 bit signed int
    UInt32      = 0x0075, // 32 bit unsigned int
    Int64Quad   = 0x0013, // 64 bit signed
    UInt64Quad  = 0x0023, // 64 bit unsigned
    Int64       = 0x0076, // 64 bit signed int
    UInt64      = 0x0077, // 64 bit unsigned int
    Int128Oct   = 0x0014, // 128 bit signed int
    UInt128Oct  = 0x0024, // 128 bit unsigned int
    Int128      = 0x0078, // 128 bit signed int
    UInt128     = 0x0079, // 128 bit unsigned int

    Float16                 = 0x0046, // 16 bit real
    Float32                 = 0x0040, // 32 bit real
    Float32PartialPrecision = 0x0045, // 32 bit PP real
    Float48                 = 0x0044, // 48 bit real
    Float64                 = 0x0041, // 64 bit real
    Float80                 = 0x0042, // 80 bit real
    Float128                = 0x0043, // 128 bit real

    Complex16                 = 0x0056, // 16 bit complex
    Complex32                 = 0x0050, // 32 bit complex
    Complex32PartialPrecision = 0x0055, // 32 bit PP complex
    Complex48                 = 0x0054, // 48 bit complex
    Complex64                 = 0x0051, // 64 bit complex
    Complex80                 = 0x0052, // 80 bit complex
    Complex128                = 0x0053, // 128 bit complex

    Boolean8   = 0x0030, // 8 bit boolean
    Boolean16  = 0x0031, // 16 bit boolean
    Boolean32  = 0x0032, // 32 bit boolean
    Boolean64  = 0x0033, // 64 bit boolean
    Boolean128 = 0x0034, // 128 bit boolean
};

enum SimpleTypeMode : DbgTypeIndex
{
    Direct         = 0, // Not a pointer
    NearPointer    = 1, // Near pointer
    FarPointer     = 2, // Far pointer
    HugePointer    = 3, // Huge pointer
    NearPointer32  = 4, // 32 bit near pointer
    FarPointer32   = 5, // 32 bit far pointer
    NearPointer64  = 6, // 64 bit near pointer
    NearPointer128 = 7  // 128 bit near pointer
};

Utf8 BackendX64::dbgGetScopedName(AstNode* node)
{
    auto nn = node->getScopedName();
    Utf8 result;

    auto pz      = nn.c_str();
    bool lastDot = false;
    for (int i = 0; i < nn.length(); i++, pz++)
    {
        if (*pz == '.')
        {
            if (lastDot)
                continue;
            lastDot = true;
            result += "::";
            continue;
        }

        result += *pz;
        lastDot = false;
    }

    return result;
}

void BackendX64::dbgSetLocation(CoffFunction* coffFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset)
{
    if (!coffFct->node || coffFct->node->isSpecialFunctionGenerated())
        return;

    if (!ip)
    {
        DbgLine dbgLine;
        dbgLine.line       = coffFct->node->token.startLocation.line + 1;
        dbgLine.byteOffset = byteOffset;
        DbgLines dbgLines;
        dbgLines.sourceFile = coffFct->node->sourceFile;
        dbgLines.dbgLines.push_back(dbgLine);
        coffFct->dbgLines.push_back(dbgLines);
        return;
    }

    SourceFile*     sourceFile;
    SourceLocation* location;
    ByteCode::getLocation(bc, ip, &sourceFile, &location);
    if (!location)
        return;

    // Update begin of start scope
    auto scope = ip->node->ownerScope;
    while (true)
    {
        if (scope->backendStart == 0)
            scope->backendStart = byteOffset;
        scope->backendEnd = byteOffset;
        if (scope->kind == ScopeKind::Function)
            break;
        scope = scope->parentScope;
    }

    SWAG_ASSERT(!coffFct->dbgLines.empty());
    if (coffFct->dbgLines.back().sourceFile != sourceFile)
    {
        DbgLines dbgLines;
        dbgLines.sourceFile = sourceFile;
        coffFct->dbgLines.push_back(dbgLines);
    }

    auto& dbgLines = coffFct->dbgLines.back().dbgLines;

    if (dbgLines.empty())
        dbgLines.push_back({location->line + 1, byteOffset});
    else if (dbgLines.back().line != location->line + 1)
    {
        if (dbgLines.back().byteOffset == byteOffset)
            dbgLines.back().line = location->line + 1;
        else
            dbgLines.push_back({location->line + 1, byteOffset});
    }
}

void BackendX64::dbgEmitCompilerFlagsDebugS(Concat& concat)
{
    concat.addU32(SUBSECTION_SYMBOL);
    auto patchSCount  = concat.addU32Addr(0); // Size of sub section
    auto patchSOffset = concat.totalCount();

    auto patchRecordCount  = concat.addU16Addr(0); // Record length, starting from &RecordKind.
    auto patchRecordOffset = concat.totalCount();
    concat.addU16(S_COMPILE3); // Record kind enum (SymRecordKind or TypeRecordKind)

    concat.addU32(0);    // Flags/Language (C)
    concat.addU16(0xD0); // CPU Type (X64)

    // Front end version
    concat.addU16(SWAG_BUILD_VERSION);
    concat.addU16(SWAG_BUILD_REVISION);
    concat.addU16(SWAG_BUILD_NUM);
    concat.addU16(0);

    // Backend end version
    concat.addU16(SWAG_BUILD_VERSION);
    concat.addU16(SWAG_BUILD_REVISION);
    concat.addU16(SWAG_BUILD_NUM);
    concat.addU16(0);

    // Compiler version
    Utf8 version = Utf8::format("swag %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    concat.addString(version.c_str(), version.length() + 1);
    alignConcat(concat, 4);
    *patchRecordCount = (uint16_t)(concat.totalCount() - patchRecordOffset);

    *patchSCount = concat.totalCount() - patchSOffset;
}

void BackendX64::dbgStartRecord(X64PerThread& pp, Concat& concat, uint16_t what)
{
    SWAG_ASSERT(pp.dbgRecordIdx < pp.MAX_RECORD);
    pp.dbgStartRecordPtr[pp.dbgRecordIdx]    = concat.addU16Addr(0);
    pp.dbgStartRecordOffset[pp.dbgRecordIdx] = concat.totalCount();
    concat.addU16(what);
    pp.dbgRecordIdx++;
}

void BackendX64::dbgEndRecord(X64PerThread& pp, Concat& concat, bool align)
{
    if (align)
        alignConcat(concat, 4);
    SWAG_ASSERT(pp.dbgRecordIdx);
    pp.dbgRecordIdx--;
    *pp.dbgStartRecordPtr[pp.dbgRecordIdx] = (uint16_t)(concat.totalCount() - pp.dbgStartRecordOffset[pp.dbgRecordIdx]);
}

void BackendX64::dbgEmitTruncatedString(Concat& concat, const Utf8& str)
{
    SWAG_ASSERT(str.length() < 0xF00); // Magic number from llvm codeviewdebug (should truncate)
    concat.addString(str.c_str(), str.length() + 1);
}

void BackendX64::dbgEmitSecRel(X64PerThread& pp, Concat& concat, uint32_t symbolIndex, uint32_t segIndex, uint32_t offset)
{
    CoffRelocation reloc;

    // Function symbol index relocation
    reloc.type           = IMAGE_REL_AMD64_SECREL;
    reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
    reloc.symbolIndex    = symbolIndex;
    pp.relocTableDBGSSection.table.push_back(reloc);
    concat.addU32(offset);

    // .text relocation
    reloc.type           = IMAGE_REL_AMD64_SECTION;
    reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
    reloc.symbolIndex    = segIndex;
    pp.relocTableDBGSSection.table.push_back(reloc);
    concat.addU16(0);
}

void BackendX64::dbgEmitEmbeddedValue(Concat& concat, TypeInfo* valueType, ComputedValue& val)
{
    SWAG_ASSERT(valueType->kind == TypeInfoKind::Native);
    switch (valueType->nativeType)
    {
    case NativeTypeKind::Bool:
    case NativeTypeKind::U8:
    case NativeTypeKind::S8:
        concat.addU16(LF_CHAR);
        concat.addU8(val.reg.u8);
        break;

    case NativeTypeKind::S16:
        concat.addU16(LF_SHORT);
        concat.addS16(val.reg.s16);
        break;
    case NativeTypeKind::U16:
        concat.addU16(LF_USHORT);
        concat.addU16(val.reg.u16);
        break;

    case NativeTypeKind::S32:
        concat.addU16(LF_LONG);
        concat.addS32(val.reg.s32);
        break;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        concat.addU16(LF_ULONG);
        concat.addU32(val.reg.u32);
        break;

    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        concat.addU16(LF_QUADWORD);
        concat.addS64(val.reg.s64);
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        concat.addU16(LF_UQUADWORD);
        concat.addU64(val.reg.u64);
        break;

    case NativeTypeKind::F32:
        concat.addU16(LF_REAL32);
        concat.addF32(val.reg.f32);
        break;
    case NativeTypeKind::F64:
        concat.addU16(LF_REAL64);
        concat.addF64(val.reg.f64);
        break;

    default:
        // Should never happen, but it's better to not assert
        concat.addU16(LF_UQUADWORD);
        concat.addU64(val.reg.u64);
        break;
    }
}

bool BackendX64::dbgEmitDataDebugT(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    for (auto& f : pp.dbgTypeRecords)
    {
        dbgStartRecord(pp, concat, f.kind);
        switch (f.kind)
        {
        case LF_ARGLIST:
            concat.addU32(f.LF_ArgList.count);
            for (int i = 0; i < f.LF_ArgList.args.size(); i++)
                concat.addU32(f.LF_ArgList.args[i]);
            break;

        // lfProc
        case LF_PROCEDURE:
            concat.addU32(f.LF_Procedure.returnType);
            concat.addU8(0);                        // calling convention
            concat.addU8(0);                        // attributes
            concat.addU16(f.LF_Procedure.numArgs);  // #params
            concat.addU32(f.LF_Procedure.argsType); // @argstype
            break;

        // lfMFunc
        case LF_MFUNCTION:
            concat.addU32(f.LF_MFunction.returnType);
            concat.addU32(f.LF_MFunction.structType);
            concat.addU32(f.LF_MFunction.thisType);
            concat.addU8(0);                        // calling convention
            concat.addU8(0);                        // attributes
            concat.addU16(f.LF_MFunction.numArgs);  // #params
            concat.addU32(f.LF_MFunction.argsType); // @argstype
            concat.addU32(0);                       // thisAdjust
            break;

        // lfFuncId
        case LF_FUNC_ID:
            concat.addU32(0);                // ParentScope
            concat.addU32(f.LF_FuncId.type); // @type
            dbgEmitTruncatedString(concat, f.node->token.text);
            break;

        // lfMFuncId
        case LF_MFUNC_ID:
            concat.addU32(f.LF_MFuncId.parentType);
            concat.addU32(f.LF_MFuncId.type);
            dbgEmitTruncatedString(concat, f.node->token.text);
            break;

        case LF_ARRAY:
            concat.addU32(f.LF_Array.elementType);
            concat.addU32(f.LF_Array.indexType);
            concat.addU16(LF_ULONG);
            concat.addU32(f.LF_Array.sizeOf);
            dbgEmitTruncatedString(concat, "");
            break;

        case LF_FIELDLIST:
            for (auto& p : f.LF_FieldList.fields)
            {
                concat.addU16(p.kind);
                concat.addU16(0x03); // private = 1, protected = 2, public = 3
                switch (p.kind)
                {
                case LF_MEMBER:
                    concat.addU32(p.type);
                    concat.addU16(LF_ULONG);
                    concat.addU32(p.value.reg.u32);
                    break;
                case LF_ONEMETHOD:
                    concat.addU32(p.type);
                    break;
                case LF_ENUMERATE:
                    dbgEmitEmbeddedValue(concat, p.valueType, p.value);
                    break;
                }
                dbgEmitTruncatedString(concat, p.name);
            }
            break;

        // https://llvm.org/docs/PDB/CodeViewTypes.html#lf-pointer-0x1002
        case LF_POINTER:
        {
            concat.addU32(f.LF_Pointer.pointeeType);
            uint32_t kind      = 0x0C; // Near64
            uint32_t mode      = 0;
            uint32_t modifiers = 0;
            uint32_t size      = 8; // 64 bits
            uint32_t flags     = 0;
            uint32_t layout    = (flags << 19) | (size << 13) | (modifiers << 8) | (mode << 5) | kind;
            concat.addU32(layout); // attributes (Near64)
            break;
        }

        case LF_ENUM:
            concat.addU16(f.LF_Enum.count);
            concat.addU16(0); // properties
            concat.addU32(f.LF_Enum.underlyingType);
            concat.addU32(f.LF_Enum.fieldList);
            dbgEmitTruncatedString(concat, f.name);
            break;

        case LF_STRUCTURE:
            concat.addU16(f.LF_Structure.memberCount);
            concat.addU16(f.LF_Structure.forward ? 0x80 : 0); // properties
            concat.addU32(f.LF_Structure.fieldList);
            concat.addU32(0);        // derivedFrom
            concat.addU32(0);        // vTableShape
            concat.addU16(LF_ULONG); // LF_ULONG
            concat.addU32(f.LF_Structure.sizeOf);
            dbgEmitTruncatedString(concat, f.name);
            break;
        }

        dbgEndRecord(pp, concat, false);
    }

    return true;
}

void BackendX64::dbgAddTypeRecord(X64PerThread& pp, DbgTypeRecord& tr)
{
    tr.index = (DbgTypeIndex) pp.dbgTypeRecords.size() + 0x1000;
    pp.dbgTypeRecords.emplace_back(tr);
}

DbgTypeIndex BackendX64::dbgGetSimpleType(TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Void:
            return SimpleTypeKind::Void;
        case NativeTypeKind::Bool:
            return SimpleTypeKind::Boolean8;
        case NativeTypeKind::S8:
            return SimpleTypeKind::SByte;
        case NativeTypeKind::S16:
            return SimpleTypeKind::Int16;
        case NativeTypeKind::S32:
            return SimpleTypeKind::Int32;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            return SimpleTypeKind::Int64;
        case NativeTypeKind::U8:
            return SimpleTypeKind::Byte;
        case NativeTypeKind::U16:
            return SimpleTypeKind::UInt16;
        case NativeTypeKind::U32:
            return SimpleTypeKind::UInt32;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            return SimpleTypeKind::UInt64;
        case NativeTypeKind::F32:
            return SimpleTypeKind::Float32;
        case NativeTypeKind::F64:
            return SimpleTypeKind::Float64;
        case NativeTypeKind::Rune:
            return SimpleTypeKind::Character32;
        }
    }

    return SimpleTypeKind::None;
}

DbgTypeIndex BackendX64::dbgGetOrCreatePointerToType(X64PerThread& pp, TypeInfo* typeInfo)
{
    auto simpleType = dbgGetSimpleType(typeInfo);
    if (simpleType != SimpleTypeKind::None)
        return (DbgTypeIndex)(simpleType | (NearPointer64 << 8));

    // In the cache of pointers
    auto it = pp.dbgMapPtrTypes.find(typeInfo);
    if (it != pp.dbgMapPtrTypes.end())
        return it->second;

    // Pointer to something complex
    DbgTypeRecord tr;
    tr.kind                   = LF_POINTER;
    tr.LF_Pointer.pointeeType = dbgGetOrCreateType(pp, typeInfo);
    dbgAddTypeRecord(pp, tr);
    pp.dbgMapPtrTypes[typeInfo] = tr.index;
    return tr.index;
}

DbgTypeIndex BackendX64::dbgGetOrCreatePointerPointerToType(X64PerThread& pp, TypeInfo* typeInfo)
{
    // In the cache of pointers
    auto it = pp.dbgMapPtrPtrTypes.find(typeInfo);
    if (it != pp.dbgMapPtrPtrTypes.end())
        return it->second;

    auto typeIdx = dbgGetOrCreatePointerToType(pp, typeInfo);

    // Pointer to something complex
    DbgTypeRecord tr;
    tr.kind                   = LF_POINTER;
    tr.LF_Pointer.pointeeType = typeIdx;
    dbgAddTypeRecord(pp, tr);
    pp.dbgMapPtrPtrTypes[typeInfo] = tr.index;
    return tr.index;
}

DbgTypeIndex BackendX64::dbgEmitTypeSlice(X64PerThread& pp, TypeInfo* typeInfo, TypeInfo* pointedType)
{
    DbgTypeRecord tr0;
    DbgTypeField  field;
    tr0.kind            = LF_FIELDLIST;
    field.kind          = LF_MEMBER;
    field.type          = dbgGetOrCreatePointerToType(pp, pointedType);
    field.value.reg.u32 = 0;
    field.name          = "data";
    tr0.LF_FieldList.fields.push_back(field);

    field.kind          = LF_MEMBER;
    field.type          = (DbgTypeIndex)(SimpleTypeKind::UInt64);
    field.value.reg.u32 = sizeof(void*);
    field.name          = "count";
    tr0.LF_FieldList.fields.push_back(field);
    dbgAddTypeRecord(pp, tr0);

    DbgTypeRecord tr1;
    tr1.kind                     = LF_STRUCTURE;
    tr1.LF_Structure.memberCount = 2;
    tr1.LF_Structure.sizeOf      = 2 * sizeof(void*);
    tr1.LF_Structure.fieldList   = tr0.index;
    if (typeInfo->kind == TypeInfoKind::Slice)
        tr1.name = Utf8::format("[..] %s", pointedType->name.c_str()); // debugger dosen't like 'const' before a slice name
    else
        tr1.name = typeInfo->name;

    dbgAddTypeRecord(pp, tr1);
    pp.dbgMapPtrTypes[typeInfo] = tr1.index;
    return tr1.index;
}

DbgTypeIndex BackendX64::dbgGetOrCreateType(X64PerThread& pp, TypeInfo* typeInfo)
{
    typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);

    // Simple type
    auto simpleType = dbgGetSimpleType(typeInfo);
    if (simpleType != SimpleTypeKind::None)
        return simpleType;

    // In the cache
    auto it = pp.dbgMapTypes.find(typeInfo);
    if (it != pp.dbgMapTypes.end())
        return it->second;

    // Pointer
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        return dbgGetOrCreatePointerToType(pp, typePtr->pointedType);
    }

    // Reference
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::Reference)
    {
        auto typePtr = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
        return dbgGetOrCreatePointerToType(pp, typePtr->pointedType);
    }

    // Slice
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        auto typeInfoPtr = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        return dbgEmitTypeSlice(pp, typeInfo, typeInfoPtr->pointedType);
    }

    // TypedVariadic
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::Variadic)
    {
        return dbgEmitTypeSlice(pp, typeInfo, g_TypeMgr.typeInfoAny);
    }

    // TypedVariadic
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        auto typeInfoPtr = CastTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
        return dbgEmitTypeSlice(pp, typeInfo, typeInfoPtr->rawType);
    }

    // Static array
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto          typeArr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        DbgTypeRecord tr;
        tr.kind                 = LF_ARRAY;
        tr.LF_Array.elementType = dbgGetOrCreateType(pp, typeArr->pointedType);
        tr.LF_Array.indexType   = SimpleTypeKind::UInt64;
        tr.LF_Array.sizeOf      = typeArr->sizeOf;
        dbgAddTypeRecord(pp, tr);
        pp.dbgMapTypes[typeInfo] = tr.index;
        return tr.index;
    }

    // Native string
    /////////////////////////////////
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        DbgTypeRecord tr0;
        DbgTypeField  field;
        tr0.kind            = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex)(SimpleTypeKind::UnsignedCharacter | (NearPointer64 << 8));
        field.value.reg.u32 = 0;
        field.name          = "data";
        tr0.LF_FieldList.fields.push_back(field);

        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex)(SimpleTypeKind::UInt64);
        field.value.reg.u32 = sizeof(void*);
        field.name          = "sizeof";
        tr0.LF_FieldList.fields.push_back(field);
        dbgAddTypeRecord(pp, tr0);

        DbgTypeRecord tr1;
        tr1.kind                     = LF_STRUCTURE;
        tr1.LF_Structure.memberCount = 2;
        tr1.LF_Structure.sizeOf      = 2 * sizeof(void*);
        tr1.LF_Structure.fieldList   = tr0.index;
        tr1.name                     = "string";
        dbgAddTypeRecord(pp, tr1);
        pp.dbgMapTypes[typeInfo] = tr1.index;
        return tr1.index;
    }

    // Interface
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::Interface)
    {
        DbgTypeRecord tr0;
        DbgTypeField  field;
        tr0.kind            = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex)(SimpleTypeKind::UnsignedCharacter | (NearPointer64 << 8));
        field.value.reg.u32 = 0;
        field.name          = "data";
        tr0.LF_FieldList.fields.push_back(field);

        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex)(SimpleTypeKind::UnsignedCharacter | (NearPointer64 << 8));
        field.value.reg.u32 = sizeof(void*);
        field.name          = "itable";
        tr0.LF_FieldList.fields.push_back(field);
        dbgAddTypeRecord(pp, tr0);

        DbgTypeRecord tr1;
        tr1.kind                     = LF_STRUCTURE;
        tr1.LF_Structure.memberCount = 2;
        tr1.LF_Structure.sizeOf      = 2 * sizeof(void*);
        tr1.LF_Structure.fieldList   = tr0.index;
        tr1.name                     = "interface";
        dbgAddTypeRecord(pp, tr1);
        pp.dbgMapTypes[typeInfo] = tr1.index;
        return tr1.index;
    }

    // Any
    /////////////////////////////////
    if (typeInfo->isNative(NativeTypeKind::Any))
    {
        DbgTypeRecord tr0;
        DbgTypeField  field;
        tr0.kind            = LF_FIELDLIST;
        field.kind          = LF_MEMBER;
        field.type          = (DbgTypeIndex)(SimpleTypeKind::UnsignedCharacter | (NearPointer64 << 8));
        field.value.reg.u32 = 0;
        field.name          = "ptrvalue";
        tr0.LF_FieldList.fields.push_back(field);

        field.kind          = LF_MEMBER;
        field.type          = dbgGetOrCreatePointerToType(pp, g_Workspace.swagScope.regTypeInfo);
        field.value.reg.u32 = sizeof(void*);
        field.name          = "typeinfo";
        tr0.LF_FieldList.fields.push_back(field);
        dbgAddTypeRecord(pp, tr0);

        DbgTypeRecord tr1;
        tr1.kind                     = LF_STRUCTURE;
        tr1.LF_Structure.memberCount = 2;
        tr1.LF_Structure.sizeOf      = 2 * sizeof(void*);
        tr1.LF_Structure.fieldList   = tr0.index;
        tr1.name                     = "any";
        dbgAddTypeRecord(pp, tr1);
        pp.dbgMapTypes[typeInfo] = tr1.index;
        return tr1.index;
    }

    // Structure
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);

        // Create a forward reference, in case a field points to the struct itself
        DbgTypeRecord tr2;
        tr2.kind                 = LF_STRUCTURE;
        tr2.LF_Structure.forward = true;
        tr2.name                 = typeStruct->name;
        dbgAddTypeRecord(pp, tr2);
        pp.dbgMapTypes[typeInfo] = tr2.index;

        // List of fields, after the forward ref
        DbgTypeRecord tr0;
        tr0.kind = LF_FIELDLIST;
        for (auto& p : typeStruct->fields)
        {
            DbgTypeField field;
            field.kind          = LF_MEMBER;
            field.type          = dbgGetOrCreateType(pp, p->typeInfo);
            field.name          = p->namedParam;
            field.value.reg.u32 = p->offset;
            tr0.LF_FieldList.fields.push_back(field);
        }

        for (auto& p : typeStruct->methods)
        {
            DbgTypeField field;
            field.kind = LF_ONEMETHOD;
            field.type = dbgGetOrCreateType(pp, p->typeInfo);
            auto nn    = dbgGetScopedName(p->typeInfo->declNode);
            field.name = nn;
            tr0.LF_FieldList.fields.push_back(field);
        }

        dbgAddTypeRecord(pp, tr0);

        // Struct itself, pointing to the field list
        DbgTypeRecord tr1;
        tr1.kind                     = LF_STRUCTURE;
        tr1.LF_Structure.memberCount = (uint16_t) typeStruct->fields.size();
        tr1.LF_Structure.sizeOf      = (uint16_t) typeStruct->sizeOf;
        tr1.LF_Structure.fieldList   = tr0.index;
        tr1.name                     = typeStruct->name;
        dbgAddTypeRecord(pp, tr1);

        pp.dbgMapTypes[typeInfo] = tr1.index;
        return tr1.index;
    }

    // Enum
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        TypeInfoEnum* typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);

        // List of values
        if (typeEnum->rawType->flags & TYPEINFO_INTEGER)
        {
            DbgTypeRecord tr0;
            tr0.kind = LF_FIELDLIST;
            for (auto& p : typeEnum->values)
            {
                DbgTypeField field;
                field.kind      = LF_ENUMERATE;
                field.type      = dbgGetOrCreateType(pp, p->typeInfo);
                field.name      = p->namedParam;
                field.valueType = typeEnum->rawType;
                field.value     = p->value;
                tr0.LF_FieldList.fields.push_back(field);
            }
            dbgAddTypeRecord(pp, tr0);

            // Enum itself, pointing to the field list
            DbgTypeRecord tr1;
            tr1.kind                   = LF_ENUM;
            tr1.LF_Enum.count          = (uint16_t) typeEnum->values.size();
            tr1.LF_Enum.fieldList      = tr0.index;
            tr1.LF_Enum.underlyingType = dbgGetOrCreateType(pp, typeEnum->rawType);
            tr1.name                   = typeEnum->name;
            dbgAddTypeRecord(pp, tr1);
            pp.dbgMapTypes[typeInfo] = tr1.index;
            return tr1.index;
        }

        else
        {
            return dbgGetOrCreateType(pp, typeEnum->rawType);
        }
    }

    // Function
    /////////////////////////////////
    if (typeInfo->kind == TypeInfoKind::FuncAttr)
    {
        TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
        DbgTypeRecord     tr0;

        // Get the arg list type. We construct a string with all parameters to be able to
        // store something in the cache
        Utf8 args;
        for (auto& p : typeFunc->parameters)
            args += p->typeInfo->name;

        bool isMethod = typeFunc->isMethod();
        auto numArgs  = (uint16_t) typeFunc->parameters.size();
        if (isMethod) // Remove 'using self' first parameter
            numArgs--;

        DbgTypeIndex argsTypeIndex;
        auto         itn = pp.dbgMapTypesNames.find(args);
        if (itn == pp.dbgMapTypesNames.end())
        {
            DbgTypeRecord tr1;
            tr1.kind             = LF_ARGLIST;
            tr1.LF_ArgList.count = numArgs;
            for (int i = 0; i < typeFunc->parameters.size(); i++)
            {
                if (isMethod && i == 0) // Remove 'using self' first parameter
                    continue;
                auto p = typeFunc->parameters[i];
                tr1.LF_ArgList.args.push_back(dbgGetOrCreateType(pp, p->typeInfo));
            }

            dbgAddTypeRecord(pp, tr1);
            pp.dbgMapTypesNames[args] = tr1.index;
            argsTypeIndex             = tr1.index;
        }
        else
            argsTypeIndex = itn->second;

        if (isMethod)
        {
            tr0.kind                    = LF_MFUNCTION;
            tr0.LF_MFunction.returnType = dbgGetOrCreateType(pp, typeFunc->returnType);
            auto typeThis               = CastTypeInfo<TypeInfoPointer>(typeFunc->parameters[0]->typeInfo, TypeInfoKind::Pointer);
            tr0.LF_MFunction.structType = dbgGetOrCreateType(pp, typeThis->pointedType);
            tr0.LF_MFunction.thisType   = dbgGetOrCreateType(pp, typeThis);
            tr0.LF_MFunction.numArgs    = numArgs;
            tr0.LF_MFunction.argsType   = argsTypeIndex;
        }
        else
        {
            tr0.kind                    = LF_PROCEDURE;
            tr0.LF_Procedure.returnType = dbgGetOrCreateType(pp, typeFunc->returnType);
            tr0.LF_Procedure.numArgs    = numArgs;
            tr0.LF_Procedure.argsType   = argsTypeIndex;
        }

        dbgAddTypeRecord(pp, tr0);
        return tr0.index;
    }

    return (DbgTypeIndex) SimpleTypeKind::UInt64;
}

void BackendX64::dbgEmitConstant(X64PerThread& pp, Concat& concat, AstNode* node)
{
    if (node->typeInfo->kind == TypeInfoKind::Native && node->typeInfo->sizeOf <= 8)
    {
        dbgStartRecord(pp, concat, S_CONSTANT);
        concat.addU32(dbgGetOrCreateType(pp, node->typeInfo));
        switch (node->typeInfo->sizeOf)
        {
        case 1:
            concat.addU16(LF_CHAR);
            concat.addU8(node->computedValue->reg.u8);
            break;
        case 2:
            concat.addU16(LF_USHORT);
            concat.addU16(node->computedValue->reg.u16);
            break;
        case 4:
            concat.addU16(LF_ULONG);
            concat.addU32(node->computedValue->reg.u32);
            break;
        case 8:
            concat.addU16(LF_QUADWORD);
            concat.addU64(node->computedValue->reg.u64);
            break;
        }

        auto nn = dbgGetScopedName(node);
        dbgEmitTruncatedString(concat, nn);
        dbgEndRecord(pp, concat);
    }
}

void BackendX64::dbgEmitGlobalDebugS(X64PerThread& pp, Concat& concat, VectorNative<AstNode*>& gVars, uint32_t segSymIndex)
{
    concat.addU32(SUBSECTION_SYMBOL);
    auto patchSCount  = concat.addU32Addr(0);
    auto patchSOffset = concat.totalCount();

    for (auto& p : gVars)
    {
        // Compile time constant
        if (p->flags & AST_VALUE_COMPUTED)
        {
            dbgEmitConstant(pp, concat, p);
            continue;
        }

        dbgStartRecord(pp, concat, S_LDATA32);
        concat.addU32(dbgGetOrCreateType(pp, p->typeInfo));

        CoffRelocation reloc;

        // symbol index relocation inside segment
        reloc.type           = IMAGE_REL_AMD64_SECREL;
        reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
        reloc.symbolIndex    = segSymIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU32(p->resolvedSymbolOverload->computedValue.storageOffset);

        // segment relocation
        reloc.type           = IMAGE_REL_AMD64_SECTION;
        reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
        reloc.symbolIndex    = segSymIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU16(0);

        auto nn = dbgGetScopedName(p);
        dbgEmitTruncatedString(concat, nn);
        dbgEndRecord(pp, concat);
    }

    // Fake symbol, because lld linker (since v12) generates a warning if this subsection is empty (wtf)
    if (patchSOffset == concat.totalCount())
    {
        dbgStartRecord(pp, concat, S_LDATA32);
        concat.addU32(dbgGetOrCreateType(pp, g_TypeMgr.typeInfoBool));
        concat.addU32(0);
        dbgEmitTruncatedString(concat, "__fake__");
        concat.addU16(0);
        dbgEndRecord(pp, concat);
    }

    *patchSCount = concat.totalCount() - patchSOffset;
}

bool BackendX64::dbgEmitFctDebugS(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    map<Utf8, uint32_t> mapFileNames;
    vector<uint32_t>    arrFileNames;
    Utf8                stringTable;

    for (auto& f : pp.functions)
    {
        if (!f.node || f.node->isSpecialFunctionGenerated())
            continue;

        auto decl     = CastAst<AstFuncDecl>(f.node, AstNodeKind::FuncDecl);
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(decl->typeInfo, TypeInfoKind::FuncAttr);

        // Add a func id type record
        /////////////////////////////////
        DbgTypeRecord tr;
        tr.node = f.node;
        if (typeFunc->isMethod())
        {
            tr.kind                  = LF_MFUNC_ID;
            auto typeThis            = CastTypeInfo<TypeInfoPointer>(typeFunc->parameters[0]->typeInfo, TypeInfoKind::Pointer);
            tr.LF_MFuncId.parentType = dbgGetOrCreateType(pp, typeThis->pointedType);
            tr.LF_MFuncId.type       = dbgGetOrCreateType(pp, typeFunc);
        }
        else
        {
            tr.kind           = LF_FUNC_ID;
            tr.LF_FuncId.type = dbgGetOrCreateType(pp, typeFunc);
        }

        dbgAddTypeRecord(pp, tr);

        // Symbol
        /////////////////////////////////
        {
            concat.addU32(SUBSECTION_SYMBOL);
            auto patchSCount  = concat.addU32Addr(0);
            auto patchSOffset = concat.totalCount();

            // Proc ID
            // PROCSYM32
            /////////////////////////////////
            dbgStartRecord(pp, concat, f.wrapper ? S_GPROC32_ID : S_LPROC32_ID);
            concat.addU32(0);                             // Parent = 0
            concat.addU32(0);                             // End = 0
            concat.addU32(0);                             // Next = 0
            concat.addU32(f.endAddress - f.startAddress); // CodeSize = 0
            concat.addU32(0);                             // DbgStart = 0
            concat.addU32(0);                             // DbgEnd = 0
            concat.addU32(tr.index);                      // FuncID type index
            dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex);
            concat.addU8(0); // ProcSymFlags Flags = ProcSymFlags::None
            auto nn = dbgGetScopedName(f.node);
            dbgEmitTruncatedString(concat, nn);
            dbgEndRecord(pp, concat);

            // Frame Proc
            /////////////////////////////////
            dbgStartRecord(pp, concat, S_FRAMEPROC);
            concat.addU32(f.frameSize); // FrameSize
            concat.addU32(0);           // Padding
            concat.addU32(0);           // Offset of padding
            concat.addU32(0);           // Bytes of callee saved registers
            concat.addU32(0);           // Exception handler offset
            concat.addU32(0);           // Exception handler section
            concat.addU32(0);           // Flags (defines frame register)
            dbgEndRecord(pp, concat);

            // Parameters
            /////////////////////////////////
            if (decl->parameters && !(decl->attributeFlags & ATTRIBUTE_COMPILER_FUNC) && !f.wrapper)
            {
                auto idxParam    = typeFunc->numReturnRegisters();
                auto countParams = decl->parameters->childs.size();
                for (int i = 0; i < countParams; i++)
                {
                    auto child     = decl->parameters->childs[i];
                    auto typeParam = typeFunc->parameters[i]->typeInfo;
                    auto overload  = child->resolvedSymbolOverload;

                    DbgTypeIndex typeIdx;
                    switch (typeParam->kind)
                    {
                    case TypeInfoKind::Array:
                        typeIdx = dbgGetOrCreatePointerToType(pp, typeParam);
                        break;
                    default:
                        typeIdx = dbgGetOrCreateType(pp, typeParam);
                        break;
                    }

                    //////////
                    dbgStartRecord(pp, concat, S_LOCAL);
                    concat.addU32(typeIdx); // Type
                    concat.addU16(0);       // CV_LVARFLAGS (do not set IsParameter, because we do not want a dereference, don't know what's going on here)
                    dbgEmitTruncatedString(concat, child->token.text);
                    dbgEndRecord(pp, concat);

                    //////////
                    dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER_REL);
                    concat.addU16(R_RDI); // Register
                    concat.addU16(0);     // Flags
                    concat.addU32(overload->storageIndex * sizeof(Register) + f.offsetParam);
                    dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex);
                    concat.addU16((uint16_t)(f.endAddress - f.startAddress)); // Range
                    dbgEndRecord(pp, concat);

                    // Codeview seems to need this pointer to be named "this"...
                    // So add it
                    if (typeFunc->isMethod() && child->token.text == g_LangSpec.name_self)
                    {
                        //////////
                        dbgStartRecord(pp, concat, S_LOCAL);
                        concat.addU32(typeIdx); // Type
                        concat.addU16(0);       // CV_LVARFLAGS (do not set IsParameter, because we do not want a dereference, don't know what's going on here)
                        dbgEmitTruncatedString(concat, "this");
                        dbgEndRecord(pp, concat);

                        //////////
                        dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER_REL);
                        concat.addU16(R_RDI); // Register
                        concat.addU16(0);     // Flags
                        concat.addU32(overload->storageIndex * sizeof(Register) + f.offsetParam);
                        dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex);
                        concat.addU16((uint16_t)(f.endAddress - f.startAddress)); // Range
                        dbgEndRecord(pp, concat);
                    }

                    idxParam += typeParam->numRegisters();
                }
            }

            // Lexical blocks
            /////////////////////////////////
            auto funcDecl = CastAst<AstFuncDecl>(f.node, AstNodeKind::FuncDecl);
            dbgEmitScope(pp, concat, f, funcDecl->scope);

            // End
            /////////////////////////////////
            dbgStartRecord(pp, concat, S_PROC_ID_END);
            dbgEndRecord(pp, concat);

            *patchSCount = concat.totalCount() - patchSOffset;
        }

        // Lines table
        /////////////////////////////////
        for (int idxDbgFile = 0; idxDbgFile < f.dbgLines.size(); idxDbgFile++)
        {
            auto& itFile     = f.dbgLines[idxDbgFile];
            auto  sourceFile = itFile.sourceFile;
            auto& dbgLines   = itFile.dbgLines;
            concat.addU32(SUBSECTION_LINES);
            auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
            auto patchLTOffset = concat.totalCount();

            // Function symbol index relocation
            // Relocate to the first (relative) byte offset of the first line
            // Size is the address of the next subsection start, or the end of the function for the last one
            auto startByteIndex = dbgLines[0].byteOffset;
            dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex, dbgLines[0].byteOffset);
            concat.addU16(0); // Flags
            uint32_t endAddress;
            if (idxDbgFile != f.dbgLines.size() - 1)
                endAddress = f.dbgLines[idxDbgFile + 1].dbgLines[0].byteOffset;
            else
                endAddress = f.endAddress - f.startAddress;
            concat.addU32(endAddress - dbgLines[0].byteOffset); // Code size

            // Compute file name index in the checksum table
            auto   checkSymIndex = 0;
            string name;
            if (f.wrapper)
                name = exportFilePath;
            else
                name = sourceFile->path;
            auto it = mapFileNames.find(name);
            if (it == mapFileNames.end())
            {
                checkSymIndex = (uint32_t) arrFileNames.size();
                arrFileNames.push_back((uint32_t) stringTable.length());
                mapFileNames[name] = checkSymIndex;

                // Normalize path name
                for (auto& c : name)
                {
                    if (c == '/')
                        c = '\\';
                }

                stringTable += name;
                stringTable.append((char) 0);
            }
            else
            {
                checkSymIndex = it->second;
            }

            auto numDbgLines = (uint32_t) dbgLines.size();
            concat.addU32(checkSymIndex * 8);    // File index in checksum buffer (in bytes!)
            concat.addU32(numDbgLines);          // NumLines
            concat.addU32(12 + numDbgLines * 8); // Code size block in bytes (12 + number of lines * 8)
            for (auto& line : dbgLines)
            {
                concat.addU32(line.byteOffset - startByteIndex); // Offset in bytes from the start of the section
                concat.addU32(line.line);                        // Line number
            }

            *patchLTCount = concat.totalCount() - patchLTOffset;
        }
    }

    // File checksum table
    /////////////////////////////////
    concat.addU32(SUBSECTION_FILE_CHECKSUM);
    concat.addU32((int) arrFileNames.size() * 8); // Size of sub section
    for (auto& p : arrFileNames)
    {
        concat.addU32(p); // Offset of file name in string table
        concat.addU32(0);
    }

    // String table
    /////////////////////////////////
    concat.addU32(SUBSECTION_STRING_TABLE);
    while (stringTable.length() & 3) // Align to 4 bytes
        stringTable.append((char) 0);
    concat.addU32(stringTable.length());
    concat.addString(stringTable);

    return true;
}

bool BackendX64::dbgEmitScope(X64PerThread& pp, Concat& concat, CoffFunction& f, Scope* scope)
{
    // Empty scope
    if (!scope->backendEnd)
        return true;

    // Header
    /////////////////////////////////
    dbgStartRecord(pp, concat, S_BLOCK32);
    concat.addU32(0);                                       // Parent = 0;
    concat.addU32(0);                                       // End = 0;
    concat.addU32(scope->backendEnd - scope->backendStart); // CodeSize;
    dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex, scope->backendStart);
    dbgEmitTruncatedString(concat, "");
    dbgEndRecord(pp, concat);

    // Local variables marked as global
    /////////////////////////////////
    auto funcDecl = (AstFuncDecl*) f.node;
    for (int i = 0; i < (int) funcDecl->localGlobalVars.size(); i++)
    {
        auto localVar = funcDecl->localGlobalVars[i];
        if (localVar->ownerScope != scope)
            continue;

        SymbolOverload* overload = localVar->resolvedSymbolOverload;
        auto            typeInfo = overload->typeInfo;

        SWAG_ASSERT(localVar->attributeFlags & ATTRIBUTE_GLOBAL);

        dbgStartRecord(pp, concat, S_LDATA32);
        concat.addU32(dbgGetOrCreateType(pp, typeInfo));

        CoffRelocation reloc;
        auto           segSymIndex = overload->flags & OVERLOAD_VAR_BSS ? pp.symBSIndex : pp.symMSIndex;

        // symbol index relocation inside segment
        reloc.type           = IMAGE_REL_AMD64_SECREL;
        reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
        reloc.symbolIndex    = segSymIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU32(overload->computedValue.storageOffset);

        // segment relocation
        reloc.type           = IMAGE_REL_AMD64_SECTION;
        reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
        reloc.symbolIndex    = segSymIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU16(0);

        dbgEmitTruncatedString(concat, localVar->token.text);
        dbgEndRecord(pp, concat);
    }

    // Local variables
    /////////////////////////////////
    for (int i = 0; i < (int) f.node->extension->bc->localVars.size(); i++)
    {
        auto localVar = f.node->extension->bc->localVars[i];
        if (localVar->ownerScope != scope)
            continue;

        SymbolOverload* overload = localVar->resolvedSymbolOverload;
        auto            typeInfo = overload->typeInfo;

        //////////
        dbgStartRecord(pp, concat, S_LOCAL);
        if (overload->flags & OVERLOAD_RETVAL)
            concat.addU32(dbgGetOrCreatePointerPointerToType(pp, typeInfo)); // Type
        else
            concat.addU32(dbgGetOrCreateType(pp, typeInfo)); // Type
        concat.addU16(0);                                    // CV_LVARFLAGS
        dbgEmitTruncatedString(concat, localVar->token.text);
        dbgEndRecord(pp, concat);

        //////////
        dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER_REL);
        concat.addU16(R_RDI);                  // Register
        concat.addU16(0);                      // Flags
        if (overload->flags & OVERLOAD_RETVAL) // Offset to register
            concat.addU32(f.offsetRetVal);
        else
            concat.addU32(overload->computedValue.storageOffset + f.offsetStack);

        dbgEmitSecRel(pp, concat, f.symbolIndex, pp.symCOIndex, localVar->ownerScope->backendStart);
        auto endOffsetVar = localVar->ownerScope->backendEnd == 0 ? f.endAddress : localVar->ownerScope->backendEnd;
        concat.addU16((uint16_t)(endOffsetVar - localVar->ownerScope->backendStart)); // Range
        dbgEndRecord(pp, concat);
    }

    // Sub scopes
    // Must be sorted, from first to last. We use the byte index of the first instruction in the block
    /////////////////////////////////
    if (scope->childScopes.size() > 1)
    {
        sort(scope->childScopes.begin(), scope->childScopes.end(), [](Scope* n1, Scope* n2) {
            return n1->backendStart < n2->backendStart;
        });
    }

    for (auto c : scope->childScopes)
        dbgEmitScope(pp, concat, f, c);

    // End
    /////////////////////////////////
    dbgStartRecord(pp, concat, S_END);
    dbgEndRecord(pp, concat);
    return true;
}

bool BackendX64::emitDebug(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;
    auto  beforeCount     = concat.totalCount();

    // .debug$S
    alignConcat(concat, 16);
    *pp.patchDBGSOffset = concat.totalCount();
    concat.addU32(DEBUG_SECTION_MAGIC);
    if (buildParameters.buildCfg->backendDebugInformations)
    {
        dbgEmitCompilerFlagsDebugS(concat);
        dbgEmitGlobalDebugS(pp, concat, module->globalVarsMutable, pp.symMSIndex);
        dbgEmitGlobalDebugS(pp, concat, module->globalVarsBss, pp.symBSIndex);
        dbgEmitGlobalDebugS(pp, concat, module->globalVarsConstant, pp.symCSIndex);
        dbgEmitFctDebugS(buildParameters);
    }
    *pp.patchDBGSCount = concat.totalCount() - *pp.patchDBGSOffset;

    // .debug$T
    alignConcat(concat, 16);
    *pp.patchDBGTOffset = concat.totalCount();
    concat.addU32(DEBUG_SECTION_MAGIC);
    if (buildParameters.buildCfg->backendDebugInformations)
        dbgEmitDataDebugT(buildParameters);
    *pp.patchDBGTCount = concat.totalCount() - *pp.patchDBGTOffset;

    // Reloc table .debug$S
    if (!pp.relocTableDBGSSection.table.empty())
    {
        alignConcat(concat, 16);
        *pp.patchDBGSSectionRelocTableOffset = concat.totalCount();
        emitRelocationTable(pp.concat, pp.relocTableDBGSSection, pp.patchDBGSSectionFlags, pp.patchDBGSSectionRelocTableCount);
    }

    g_Stats.sizeBackendDbg += concat.totalCount() - beforeCount;
    return true;
}
