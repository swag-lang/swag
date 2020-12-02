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
const uint16_t S_FRAMEPROC              = 0x1012;
const uint16_t S_COMPILE3               = 0x113c;
const uint16_t S_LPROC32_ID             = 0x1146;
const uint16_t S_GPROC32_ID             = 0x1147;
const uint16_t S_PROC_ID_END            = 0x114F;

void BackendX64::setDebugLocation(CoffFunction* coffFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset)
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

void BackendX64::emitDBGSCompilerFlags(Concat& concat)
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

void BackendX64::startTypeRecord(Concat& concat, uint16_t what)
{
    startTypeRecordPtr    = concat.addU16Addr(0);
    startTypeRecordOffset = concat.totalCount();
    concat.addU16(what);
}

void BackendX64::endTypeRecord(Concat& concat)
{
    *startTypeRecordPtr = (uint16_t)(concat.totalCount() - startTypeRecordOffset);
}

void BackendX64::emitTruncatedString(Concat& concat, const Utf8& str)
{
    SWAG_ASSERT(str.length() < 0xF00); // Magic number from llvm codeviewdebug (should truncate)
    concat.addString(str.c_str(), str.length() + 1);
}

bool BackendX64::emitDBGTData(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    uint16_t typeId = 0x1000;
    for (auto& f : pp.functions)
    {
        if (!f.node)
            continue;

        const uint16_t LF_ARGLIST = 0x1201;
        startTypeRecord(concat, LF_ARGLIST);
        concat.addU32(2);     // #params
        concat.addU32(0x613); // param 1
        concat.addU32(0x613); // param 2
        endTypeRecord(concat);
        typeId++;

        const uint16_t LF_PROCEDURE = 0x1008;
        startTypeRecord(concat, LF_PROCEDURE);
        concat.addU16(0x613);      // @returntype
        concat.addU16(0);          // calling convention
        concat.addU16(0);          // padding
        concat.addU16(2);          // #params
        concat.addU16(typeId - 1); // @argstype
        concat.addU16(0);          // align
        endTypeRecord(concat);
        typeId++;

        const uint16_t LF_FUNC_ID = 0x1601;
        startTypeRecord(concat, LF_FUNC_ID);
        concat.addU32(0);          // ParentScope
        concat.addU16(typeId - 1); // @type
        concat.addU16(0);          // padding
        emitTruncatedString(concat, f.node->token.text);
        endTypeRecord(concat);
        f.dbgTypeId = typeId;
        typeId++;
    }

    return true;
}

bool BackendX64::emitDBGSData(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    emitDBGSCompilerFlags(concat);

    map<Utf8, uint32_t> mapFileNames;
    vector<uint32_t>    arrFileNames;
    Utf8                stringTable;
    CoffRelocation      reloc;

    for (auto& f : pp.functions)
    {
        if (!f.node)
            continue;

        // Symbol
        /////////////////////////////////
        {
            concat.addU32(SUBSECTION_SYMBOL);
            auto patchSCount  = concat.addU32Addr(0);
            auto patchSOffset = concat.totalCount();

            // Proc ID
            auto patchRecordCount  = concat.addU16Addr(0);
            auto patchRecordOffset = concat.totalCount();
            concat.addU16(S_GPROC32_ID);

            concat.addU32(0);                             // Parent = 0;
            concat.addU32(0);                             // End = 0;
            concat.addU32(0);                             // Next = 0;
            concat.addU32(f.endAddress - f.startAddress); // CodeSize = 0;
            concat.addU32(0);                             // DbgStart = 0;
            concat.addU32(0);                             // DbgEnd = 0;
            concat.addU32(f.dbgTypeId);                   // @FunctionType; TODO

            reloc.type           = IMAGE_REL_AMD64_SECREL;
            reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
            reloc.symbolIndex    = f.symbolIndex;
            pp.relocTableDBGSSection.table.push_back(reloc);
            concat.addU32(0); // uint32_t CodeOffset = 0;

            reloc.type           = IMAGE_REL_AMD64_SECTION;
            reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
            reloc.symbolIndex    = pp.symCOIndex;
            pp.relocTableDBGSSection.table.push_back(reloc);
            concat.addU16(0); // Segment

            concat.addU8(0); // ProcSymFlags Flags = ProcSymFlags::None;
            emitTruncatedString(concat, f.node->token.text);

            alignConcat(concat, 4);
            *patchRecordCount = (uint16_t)(concat.totalCount() - patchRecordOffset);

            // Frame Proc
            /////////////////////////////////
            patchRecordCount  = concat.addU16Addr(0);
            patchRecordOffset = concat.totalCount();
            concat.addU16(S_FRAMEPROC);

            concat.addU32(0); // FrameSize
            concat.addU32(0); // Padding
            concat.addU32(0); // Offset of padding
            concat.addU32(0); // Bytes of callee saved registers
            concat.addU32(0); // Exception handler offset
            concat.addU32(0); // Exception handler section
            concat.addU32(0); // Flags (defines frame register)

            alignConcat(concat, 4);
            *patchRecordCount = (uint16_t)(concat.totalCount() - patchRecordOffset);

            // End
            /////////////////////////////////
            concat.addU16(2);
            concat.addU16(S_PROC_ID_END);
            *patchSCount = concat.totalCount() - patchSOffset;
        }

        // Lines table
        /////////////////////////////////
        {
            concat.addU32(SUBSECTION_LINES);
            auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
            auto patchLTOffset = concat.totalCount();

            // Function symbol index
            reloc.type           = IMAGE_REL_AMD64_SECREL;
            reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
            reloc.symbolIndex    = f.symbolIndex;
            pp.relocTableDBGSSection.table.push_back(reloc);
            concat.addU32(0);

            reloc.type           = IMAGE_REL_AMD64_SECTION;
            reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
            reloc.symbolIndex    = pp.symCOIndex;
            pp.relocTableDBGSSection.table.push_back(reloc);
            concat.addU16(0); // Segment

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

            concat.addU32(checkSymIndex * 8); // File index in checksum buffer (in bytes!)
            auto numDbgLines = (uint32_t) f.dbgLines.size();
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

bool BackendX64::emitDebugData(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    // .debug$T
    alignConcat(concat, 16);
    *pp.patchDBGTOffset = concat.totalCount();
    concat.addU32(4); // DEBUG_SECTION_MAGIC
    if (buildParameters.buildCfg->backendDebugInformations)
        emitDBGTData(buildParameters);
    *pp.patchDBGTCount = concat.totalCount() - *pp.patchDBGTOffset;

    // .debug$S
    alignConcat(concat, 16);
    *pp.patchDBGSOffset = concat.totalCount();
    concat.addU32(4); // DEBUG_SECTION_MAGIC
    if (buildParameters.buildCfg->backendDebugInformations)
        emitDBGSData(buildParameters);
    *pp.patchDBGSCount = concat.totalCount() - *pp.patchDBGSOffset;

    // Reloc table
    if (!pp.relocTableDBGSSection.table.empty())
    {
        alignConcat(concat, 16);
        *pp.patchDBGSSectionRelocTableOffset = concat.totalCount();
        emitRelocationTable(pp.concat, pp.relocTableDBGSSection, pp.patchDBGSSectionFlags, pp.patchDBGSSectionRelocTableCount);
    }

    return true;
}