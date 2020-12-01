#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendX64FunctionBodyJob.h"
#include "BackendX64FunctionMacros_.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"

bool BackendX64::emitDBGSData(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    alignConcat(concat, 16);
    *pp.patchDBGSOffset = concat.totalCount();

    concat.addU32(4); // DEBUG_SECTION_MAGIC
    uint32_t offset = 4;

    const uint32_t SUBSECTION_LINES         = 0xF2;
    const uint32_t SUBSECTION_STRING_TABLE  = 0xF3;
    const uint32_t SUBSECTION_FILE_CHECKSUM = 0xF4;

    for (auto& f : pp.functions)
    {
        concat.addU32(SUBSECTION_LINES);
        concat.addU32(12 + 12); // Size of sub section
        offset += 8;

        // Function symbol index
        CoffRelocation reloc;
        reloc.type           = IMAGE_REL_AMD64_ADDR32NB;
        reloc.virtualAddress = offset;
        reloc.symbolIndex    = f.symbolIndex;
        pp.relocTableDBGSSection.table.push_back(reloc);
        concat.addU32(0);

        concat.addU16(0);                             // RelocSegment
        concat.addU16(0);                             // Flags
        concat.addU32(f.endAddress - f.startAddress); // Code size
        offset += 12;

        // Line
        concat.addU32(0);  // File NameIndex in checksum buffer
        concat.addU32(0);  // NumLines
        concat.addU32(12); // Code size block in bytes
        offset += 12;

        //break;
    }

    // File checksum table
    concat.addU32(SUBSECTION_FILE_CHECKSUM);
    concat.addU32(8 + 8); // Size of sub section
    offset += 2 * 4;

    concat.addU32(0); // Offset of file name in string table
    concat.addU32(0);
    offset += 2 * 4;

    concat.addU32(1); // Offset of file name in string table
    concat.addU32(0);
    offset += 2 * 4;

    // String table
    concat.addU32(SUBSECTION_STRING_TABLE);
    concat.addU32(4); // Size of sub section
    concat.addString("ABC", 4);
    offset += 3 * 4;

    *pp.patchDBGSCount = concat.totalCount() - *pp.patchDBGSOffset;
    return true;
}

bool BackendX64::emitDebugData(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    emitDBGSData(buildParameters);

    if (!pp.relocTableDBGSSection.table.empty())
    {
        alignConcat(concat, 16);
        *pp.patchDBGSSectionRelocTableOffset = concat.totalCount();
        emitRelocationTable(pp.concat, pp.relocTableDBGSSection, pp.patchDBGSSectionFlags, pp.patchDBGSSectionRelocTableCount);
    }

    return true;
}