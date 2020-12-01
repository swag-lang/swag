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

bool BackendX64::emitDBGSData(const BuildParameters& buildParameters)
{
    int      ct              = buildParameters.compileType;
    int      precompileIndex = buildParameters.precompileIndex;
    auto&    pp              = *perThread[ct][precompileIndex];
    auto&    concat          = pp.concat;
    uint32_t offset          = 4;

    const uint32_t SUBSECTION_SYMBOL        = 0xF1;
    const uint32_t SUBSECTION_LINES         = 0xF2;
    const uint32_t SUBSECTION_STRING_TABLE  = 0xF3;
    const uint32_t SUBSECTION_FILE_CHECKSUM = 0xF4;
    const uint16_t S_COMPILE3               = 0x113c;
    const uint16_t S_GPROC32_ID             = 0x1147;

    map<Utf8, uint32_t> mapFileNames;
    vector<uint32_t>    arrFileNames;
    Utf8                stringTable;

    // Compiler flags
    {
        auto patchSectionOffset = concat.totalCount();
        concat.addU32(SUBSECTION_SYMBOL);
        auto patchSCount  = concat.addU32Addr(0); // Size of sub section
        auto patchSOffset = concat.totalCount();

        auto patchRecordCount  = concat.addU16Addr(0); //ulittle16_t RecordLen;  // Record length, starting from &RecordKind.
        auto patchRecordOffset = concat.totalCount();
        concat.addU16(S_COMPILE3); //ulittle16_t RecordKind; // Record kind enum (SymRecordKind or TypeRecordKind)

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

        while (concat.totalCount() & 3)
            concat.addChar(0);
        *patchRecordCount = (uint16_t)(concat.totalCount() - patchRecordOffset);

        *patchSCount = concat.totalCount() - patchSOffset;
        offset += concat.totalCount() - patchSectionOffset;
    }

    for (auto& f : pp.functions)
    {
        if (!f.node)
            continue;

        // Symbols table
        /////////////////////////////////
        auto patchSectionOffset = concat.totalCount();
        concat.addU32(SUBSECTION_SYMBOL);
        auto patchSCount  = concat.addU32Addr(0); // Size of sub section
        auto patchSOffset = concat.totalCount();

        auto patchRecordCount  = concat.addU16Addr(0); //ulittle16_t RecordLen;  // Record length, starting from &RecordKind.
        auto patchRecordOffset = concat.totalCount();
        concat.addU16(S_GPROC32_ID); //ulittle16_t RecordKind; // Record kind enum (SymRecordKind or TypeRecordKind)

        concat.addU32(0);                             // uint32_t Parent = 0;
        concat.addU32(0);                             // uint32_t End = 0;
        concat.addU32(0);                             // uint32_t Next = 0;
        concat.addU32(f.endAddress - f.startAddress); // uint32_t CodeSize = 0;
        concat.addU32(f.sizeProlog);                  // uint32_t DbgStart = 0;
        concat.addU32(0);                             // uint32_t DbgEnd = 0;
        concat.addU32(0);                             // TypeIndex FunctionType;

        // Function symbol index
        CoffRelocation reloc;
        reloc.type           = IMAGE_REL_AMD64_SECREL;
        reloc.virtualAddress = concat.totalCount() - *pp.patchDBGSOffset;
        reloc.symbolIndex    = f.symbolIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU32(0); // uint32_t CodeOffset = 0;

        concat.addU16(0); // uint16_t Segment = 0;
        concat.addU8(0);  // ProcSymFlags Flags = ProcSymFlags::None;
        concat.addString(f.node->token.text.c_str(), f.node->token.text.length() + 1);
        while (concat.totalCount() & 3)
            concat.addChar(0);
        *patchRecordCount = (uint16_t)(concat.totalCount() - patchRecordOffset);

        *patchSCount = concat.totalCount() - patchSOffset;
        offset += concat.totalCount() - patchSectionOffset;

        // Lines table
        /////////////////////////////////
        concat.addU32(SUBSECTION_LINES);
        auto patchLTCount  = concat.addU32Addr(0); // Size of sub section
        auto patchLTOffset = concat.totalCount();
        offset += 8;

        // Function symbol index
        reloc.type           = IMAGE_REL_AMD64_ADDR32NB;
        reloc.virtualAddress = offset;
        reloc.symbolIndex    = f.symbolIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU32(0);

        concat.addU16(0);                             // RelocSegment
        concat.addU16(0);                             // Flags
        concat.addU32(f.endAddress - f.startAddress); // Code size
        offset += 12;

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
        offset += 12;

        for (auto& line : f.dbgLines)
        {
            concat.addU32(line.byteOffset); // Offset in bytes
            concat.addU32(line.line);       // Line number
        }

        offset += numDbgLines * 8;
        *patchLTCount = concat.totalCount() - patchLTOffset;
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

    // .debug$S
    alignConcat(concat, 16);
    *pp.patchDBGSOffset = concat.totalCount();
    concat.addU32(4); // DEBUG_SECTION_MAGIC
    if (buildParameters.buildCfg->backendDebugInformations)
        emitDBGSData(buildParameters);
    *pp.patchDBGSCount = concat.totalCount() - *pp.patchDBGSOffset;

    // .debug$T
    alignConcat(concat, 16);
    *pp.patchDBGTOffset = concat.totalCount();
    concat.addU32(4); // DEBUG_SECTION_MAGIC
    *pp.patchDBGTCount = concat.totalCount() - *pp.patchDBGTOffset;

    // Reloc table
    if (!pp.relocTableDBGSSection.table.empty())
    {
        alignConcat(concat, 16);
        *pp.patchDBGSSectionRelocTableOffset = concat.totalCount();
        emitRelocationTable(pp.concat, pp.relocTableDBGSSection, pp.patchDBGSSectionFlags, pp.patchDBGSSectionRelocTableCount);
    }

    return true;
}