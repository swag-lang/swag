#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64FunctionBodyJob.h"
#include "BackendX64FunctionMacros_.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "Version.h"

const uint32_t SUBSECTION_SYMBOL        = 0xF1;
const uint32_t SUBSECTION_LINES         = 0xF2;
const uint32_t SUBSECTION_STRING_TABLE  = 0xF3;
const uint32_t SUBSECTION_FILE_CHECKSUM = 0xF4;

const uint16_t S_FRAMEPROC                 = 0x1012;
const uint16_t S_COMPILE3                  = 0x113c;
const uint16_t S_LPROC32_ID                = 0x1146;
const uint16_t S_GPROC32_ID                = 0x1147;
const uint16_t S_PROC_ID_END               = 0x114F;
const uint16_t S_LOCAL                     = 0x113E;
const uint16_t S_DEFRANGE_REGISTER         = 0x1141;
const uint16_t S_DEFRANGE_FRAMEPOINTER_REL = 0x1142;
const uint16_t S_DEFRANGE_REGISTER_REL     = 0x1145;

const uint16_t LF_ARGLIST   = 0x1201;
const uint16_t LF_PROCEDURE = 0x1008;
const uint16_t LF_FUNC_ID   = 0x1601;

const uint16_t R_RDX = 331;
const uint16_t R_RDI = 333;
const uint16_t R_RSP = 335;

enum class SimpleTypeKind : DbgTypeIndex
{
    None          = 0x0000, // uncharacterized type (no type)
    Void          = 0x0003, // void
    NotTranslated = 0x0007, // type not translated by cvpack
    HResult       = 0x0008, // OLE/COM HRESULT

    SignedCharacter   = 0x0010, // 8 bit signed
    UnsignedCharacter = 0x0020, // 8 bit unsigned
    NarrowCharacter   = 0x0070, // really a char
    WideCharacter     = 0x0071, // wide char
    Character16       = 0x007a, // char16_t
    Character32       = 0x007b, // char32_t

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

enum class SimpleTypeMode : DbgTypeIndex
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

void BackendX64::dbgSetLocation(CoffFunction* coffFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset)
{
    if (!coffFct->node)
        return;

    if (!ip)
    {
        coffFct->dbgLines.push_back({coffFct->node->token.startLocation.line + 1, byteOffset});
    }
    else if (ip && ip->node && ip->node->ownerScope && ip->node->kind != AstNodeKind::FuncDecl && !(ip->flags & BCI_SAFETY))
    {
        auto location = ip->getLocation(bc);
        if (!location)
            return;
        SWAG_ASSERT(!coffFct->dbgLines.empty());

        if (coffFct->dbgLines.back().line != location->line + 1)
        {
            if (coffFct->dbgLines.back().byteOffset == byteOffset)
                coffFct->dbgLines.back().line = location->line + 1;
            else
                coffFct->dbgLines.push_back({location->line + 1, byteOffset});
        }
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
    Utf8 version = format("swag %d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM);
    concat.addString(version.c_str(), version.length() + 1);
    alignConcat(concat, 4);
    *patchRecordCount = (uint16_t)(concat.totalCount() - patchRecordOffset);

    *patchSCount = concat.totalCount() - patchSOffset;
}

void BackendX64::dbgStartRecord(X64PerThread& pp, Concat& concat, uint16_t what)
{
    pp.dbgStartRecordPtr    = concat.addU16Addr(0);
    pp.dbgStartRecordOffset = concat.totalCount();
    concat.addU16(what);
}

void BackendX64::dbgEndRecord(X64PerThread& pp, Concat& concat)
{
    alignConcat(concat, 4);
    *pp.dbgStartRecordPtr = (uint16_t)(concat.totalCount() - pp.dbgStartRecordOffset);
}

void BackendX64::dbgEmitTruncatedString(Concat& concat, const Utf8& str)
{
    SWAG_ASSERT(str.length() < 0xF00); // Magic number from llvm codeviewdebug (should truncate)
    concat.addString(str.c_str(), str.length() + 1);
}

void BackendX64::dbgEmitSecRel(X64PerThread& pp, Concat& concat, int symbolIndex)
{
    CoffRelocation reloc;

    // Function symbol index relocation
    reloc.type           = IMAGE_REL_AMD64_SECREL;
    reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
    reloc.symbolIndex    = symbolIndex;
    pp.relocTableDBGSSection.table.push_back(reloc);
    concat.addU32(0);

    // .text relocation
    reloc.type           = IMAGE_REL_AMD64_SECTION;
    reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
    reloc.symbolIndex    = pp.symCOIndex;
    pp.relocTableDBGSSection.table.push_back(reloc);
    concat.addU16(0);
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

        case LF_PROCEDURE:
            concat.addU16(f.LF_Procedure.returnType);
            concat.addU16(0);                       // calling convention
            concat.addU16(0);                       // padding
            concat.addU16(f.LF_Procedure.numArgs);  // #params
            concat.addU16(f.LF_Procedure.argsType); // @argstype
            concat.addU16(0);                       // align
            break;

        case LF_FUNC_ID:
            concat.addU32(0);                // ParentScope
            concat.addU16(f.LF_FuncId.type); // @type
            concat.addU16(0);                // padding
            dbgEmitTruncatedString(concat, f.node->token.text);
            break;
        }

        dbgEndRecord(pp, concat);
    }

    return true;
}

void BackendX64::dbgAddTypeRecord(X64PerThread& pp, DbgTypeRecord& tr)
{
    tr.index = (uint16_t) pp.dbgTypeRecords.size() + 0x1000;
    pp.dbgTypeRecords.push_back(tr);
}

DbgTypeIndex BackendX64::dbgGetOrCreateType(X64PerThread& pp, TypeInfo* typeInfo)
{
    // Simple type
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Void:
            return (DbgTypeIndex) SimpleTypeKind::Void;
        case NativeTypeKind::Bool:
            return (DbgTypeIndex) SimpleTypeKind::Boolean8;
        case NativeTypeKind::S8:
            return (DbgTypeIndex) SimpleTypeKind::SByte;
        case NativeTypeKind::S16:
            return (DbgTypeIndex) SimpleTypeKind::Int16;
        case NativeTypeKind::S32:
            return (DbgTypeIndex) SimpleTypeKind::Int32;
        case NativeTypeKind::S64:
            return (DbgTypeIndex) SimpleTypeKind::Int64;
        case NativeTypeKind::U8:
            return (DbgTypeIndex) SimpleTypeKind::Byte;
        case NativeTypeKind::U16:
            return (DbgTypeIndex) SimpleTypeKind::UInt16;
        case NativeTypeKind::U32:
            return (DbgTypeIndex) SimpleTypeKind::UInt32;
        case NativeTypeKind::U64:
            return (DbgTypeIndex) SimpleTypeKind::UInt64;
        case NativeTypeKind::F32:
            return (DbgTypeIndex) SimpleTypeKind::Float32;
        case NativeTypeKind::F64:
            return (DbgTypeIndex) SimpleTypeKind::Float64;
        case NativeTypeKind::Char:
            return (DbgTypeIndex) SimpleTypeKind::Character32;
        }
    }

    // In the cache of pointers
    auto it = pp.dbgMapTypes.find(typeInfo);
    if (it != pp.dbgMapTypes.end())
        return it->second;

    DbgTypeRecord tr;
    switch (typeInfo->kind)
    {
    case TypeInfoKind::FuncAttr:
    {
        TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
        tr.kind                    = LF_PROCEDURE;
        tr.LF_Procedure.returnType = dbgGetOrCreateType(pp, typeFunc->returnType);
        tr.LF_Procedure.numArgs    = (uint16_t) typeFunc->parameters.size();

        // Get the arg list type. We construct a string with all parameters to be able to
        // store something in the cache
        Utf8 args;
        for (auto& p : typeFunc->parameters)
            args += p->typeInfo->name;
        auto itn = pp.dbgMapTypesNames.find(args);
        if (itn == pp.dbgMapTypesNames.end())
        {
            DbgTypeRecord tr1;
            tr1.kind             = LF_ARGLIST;
            tr1.LF_ArgList.count = tr.LF_Procedure.numArgs;
            for (auto& p : typeFunc->parameters)
                tr1.LF_ArgList.args.push_back(dbgGetOrCreateType(pp, p->typeInfo));
            dbgAddTypeRecord(pp, tr1);
            tr.LF_Procedure.argsType  = tr1.index;
            pp.dbgMapTypesNames[args] = tr1.index;
        }
        else
            tr.LF_Procedure.argsType = itn->second;

        dbgAddTypeRecord(pp, tr);
        return tr.index;
    }
    }

    return (DbgTypeIndex) SimpleTypeKind::Void;
}

bool BackendX64::dbgEmitFctDebugS(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    DbgTypeRecord       tr;
    map<Utf8, uint32_t> mapFileNames;
    vector<uint32_t>    arrFileNames;
    Utf8                stringTable;

    for (auto& f : pp.functions)
    {
        if (!f.node)
            continue;

        // Add a func id type record
        /////////////////////////////////
        tr.kind           = LF_FUNC_ID;
        tr.node           = f.node;
        tr.LF_FuncId.type = dbgGetOrCreateType(pp, f.node->typeInfo);
        pp.dbgTypeRecords.push_back(tr);

        // Symbol
        /////////////////////////////////
        {
            concat.addU32(SUBSECTION_SYMBOL);
            auto patchSCount  = concat.addU32Addr(0);
            auto patchSOffset = concat.totalCount();

            // Proc ID
            /////////////////////////////////
            dbgStartRecord(pp, concat, S_GPROC32_ID);
            concat.addU32(0);                             // Parent = 0;
            concat.addU32(0);                             // End = 0;
            concat.addU32(0);                             // Next = 0;
            concat.addU32(f.endAddress - f.startAddress); // CodeSize = 0;
            concat.addU32(0);                             // DbgStart = 0;
            concat.addU32(0);                             // DbgEnd = 0;
            concat.addU32(tr.index);                      // @FunctionType; TODO
            dbgEmitSecRel(pp, concat, f.symbolIndex);
            concat.addU8(0); // ProcSymFlags Flags = ProcSymFlags::None;
            dbgEmitTruncatedString(concat, f.node->token.text);
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

            // Local variables
            /////////////////////////////////
            for (auto localVar : f.node->bc->localVars)
            {
                SymbolOverload* overload = localVar->resolvedSymbolOverload;
                auto            typeInfo = overload->typeInfo;

                //////////
                dbgStartRecord(pp, concat, S_LOCAL);
                concat.addU32(dbgGetOrCreateType(pp, typeInfo)); // Type
                concat.addU16(0);                                // Flags
                dbgEmitTruncatedString(concat, localVar->token.text);
                dbgEndRecord(pp, concat);

                //////////
                dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER_REL);
                concat.addU16(R_RDI); // Register
                concat.addU16(0);     // Flags
                concat.addU32(overload->storageOffset + f.offsetStack);
                dbgEmitSecRel(pp, concat, f.symbolIndex);
                concat.addU16(f.endAddress - f.startAddress); // Range
                dbgEndRecord(pp, concat);

                //////////
                /*dbgStartRecord(pp, concat, S_DEFRANGE_REGISTER);
                concat.addU16(R_RDI); // Register
                concat.addU16(0);     // MayHaveNoName

                // Function symbol index relocation
                reloc.type = IMAGE_REL_AMD64_SECREL;
                reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
                reloc.symbolIndex = f.symbolIndex;
                pp.relocTableDBGSSection.table.push_back(reloc);
                concat.addU32(0);

                // .text relocation
                reloc.type = IMAGE_REL_AMD64_SECTION;
                reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
                reloc.symbolIndex = pp.symCOIndex;
                pp.relocTableDBGSSection.table.push_back(reloc);
                concat.addU16(0);

                concat.addU16(f.endAddress - f.startAddress); // Range
                dbgEndRecord(pp, concat);*/

                //////////
                /*dbgStartRecord(pp, concat, S_DEFRANGE_FRAMEPOINTER_REL);
                concat.addU32(overload->storageOffset);

                // Function symbol index relocation
                reloc.type           = IMAGE_REL_AMD64_SECREL;
                reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
                reloc.symbolIndex    = f.symbolIndex;
                pp.relocTableDBGSSection.table.push_back(reloc);
                concat.addU32(0);

                // .text relocation
                reloc.type           = IMAGE_REL_AMD64_SECTION;
                reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
                reloc.symbolIndex    = pp.symCOIndex;
                pp.relocTableDBGSSection.table.push_back(reloc);
                concat.addU16(0);

                concat.addU16(f.endAddress - f.startAddress); // Range
                dbgEndRecord(pp, concat);*/
            }

            // End
            /////////////////////////////////
            dbgStartRecord(pp, concat, S_PROC_ID_END);
            dbgEndRecord(pp, concat);

            *patchSCount = concat.totalCount() - patchSOffset;
        }

        // Lines table
        /////////////////////////////////
        {
            concat.addU32(SUBSECTION_LINES);
            auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
            auto patchLTOffset = concat.totalCount();

            // Function symbol index relocation
            dbgEmitSecRel(pp, concat, f.symbolIndex);
            concat.addU16(0);                             // Flags
            concat.addU32(f.endAddress - f.startAddress); // Code size

            // Compute file name index in the checksum table
            auto  checkSymIndex = 0;
            auto& name          = f.node->sourceFile->path;
            auto  it            = mapFileNames.find(name);
            if (it == mapFileNames.end())
            {
                checkSymIndex = (uint32_t) arrFileNames.size();
                arrFileNames.push_back((uint32_t) stringTable.length());
                mapFileNames[name] = checkSymIndex;

                // Normalize path name
                auto cpyName = name;
                for (auto& c : cpyName)
                {
                    if (c == '/')
                        c = '\\';
                }

                stringTable += cpyName;
                stringTable.append((char) 0);
            }
            else
            {
                checkSymIndex = it->second;
            }

            auto numDbgLines = (uint32_t) f.dbgLines.size();
            concat.addU32(checkSymIndex * 8);    // File index in checksum buffer (in bytes!)
            concat.addU32(numDbgLines);          // NumLines
            concat.addU32(12 + numDbgLines * 8); // Code size block in bytes (12 + number of lines * 8)
            for (auto& line : f.dbgLines)
            {
                concat.addU32(line.byteOffset); // Offset in bytes
                concat.addU32(line.line);       // Line number
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

bool BackendX64::dbgEmit(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    // .debug$S
    alignConcat(concat, 16);
    *pp.patchDBGSOffset = concat.totalCount();
    concat.addU32(4); // DEBUG_SECTION_MAGIC
    if (buildParameters.buildCfg->backendDebugInformations)
    {
        dbgEmitCompilerFlagsDebugS(concat);
        dbgEmitFctDebugS(buildParameters);
    }
    *pp.patchDBGSCount = concat.totalCount() - *pp.patchDBGSOffset;

    // .debug$T
    alignConcat(concat, 16);
    *pp.patchDBGTOffset = concat.totalCount();
    concat.addU32(4); // DEBUG_SECTION_MAGIC
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

    return true;
}