#include "pch.h"
#include "BackendX64.h"
#include "Workspace.h"
#include "OS.h"
#include "Module.h"

JobResult BackendX64::preCompile(const BuildParameters& buildParameters, Job* ownerJob)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    // Message
    if (pp.pass == BackendPreCompilePass::Init && buildParameters.precompileIndex == 0)
        module->printUserMessage(buildParameters);

    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    if (pp.pass == BackendPreCompilePass::Init)
    {
        pp.pass = BackendPreCompilePass::FunctionBodies;

        pp.filename = format("%s%d", buildParameters.outputFileName.c_str(), precompileIndex);
        pp.filename += buildParameters.postFix;
        pp.filename += ".obj";

        if (g_CommandLine.verbose)
            g_Log.verbose(format("   module %s, x64 backend, precompile", perThread[ct][precompileIndex].filename.c_str(), module->byteCodeTestFunc.size()));

        emitHeader(buildParameters);
        emitDataSegment(buildParameters, &module->bssSegment);
        emitDataSegment(buildParameters, &module->mutableSegment);
        emitDataSegment(buildParameters, &module->constantSegment);
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        pp.pass = BackendPreCompilePass::End;

        // Align .text section to 16 bytes
        while (concat.totalCount % 16)
            concat.addU8(0);
        pp.textSectionOffset       = concat.totalCount;
        *pp.patchTextSectionOffset = pp.textSectionOffset;

        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlive;
    }

    if (pp.pass == BackendPreCompilePass::End)
    {
        // Specific functions in the main file
        if (precompileIndex == 0)
        {
            emitInitDataSeg(buildParameters);
            emitInitConstantSeg(buildParameters);
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
            emitMain(buildParameters);
        }

        // This is it for functions
        *pp.patchTextSectionSize = concat.totalCount - pp.textSectionOffset;

        // Tables
        emitSymbolTable(buildParameters);
        emitStringTable(buildParameters);
        emitRelocationTables(buildParameters);

        // Segments
        if (precompileIndex == 0)
        {
            *pp.patchCSOffset = concat.totalCount;
            *pp.patchDSOffset = concat.totalCount + module->constantSegment.totalCount;
        }

        // Output file
        generateObjFile(buildParameters);
    }

    return JobResult::ReleaseJob;
}

CoffSymbol* BackendX64::getSymbol(X64PerThread& pp, const Utf8Crc& name)
{
    auto it = pp.mapSymbols.find(name);
    if (it != pp.mapSymbols.end())
        return &pp.allSymbols[it->second];
    return nullptr;
}

CoffSymbol* BackendX64::getOrAddSymbol(X64PerThread& pp, const Utf8Crc& name, CoffSymbolKind kind, uint32_t value)
{
    auto it = getSymbol(pp, name);
    if (it)
    {
        if (kind == CoffSymbolKind::Extern)
            return it;
        if (kind == CoffSymbolKind::Function && it->kind == CoffSymbolKind::Extern)
        {
            it->kind  = kind;
            it->value = value;
            return it;
        }

        SWAG_ASSERT(false);
    }

    CoffSymbol sym;
    sym.name  = name;
    sym.kind  = kind;
    sym.value = value;
    sym.index = (uint32_t) pp.allSymbols.size();
    pp.allSymbols.emplace_back(sym);
    pp.mapSymbols[name] = (uint32_t) pp.allSymbols.size() - 1;
    return &pp.allSymbols.back();
}

bool BackendX64::emitHeader(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    // Coff header
    /////////////////////////////////////////////
    concat.addU16(IMAGE_FILE_MACHINE_AMD64); // .Machine
    if (precompileIndex == 0)
        concat.addU16(4); // .NumberOfSections
    else
        concat.addU16(1); // .NumberOfSections

    time_t now;
    time(&now);
    concat.addU32((uint32_t)(now & 0xFFFFFFFF)); // .TimeDateStamp

    pp.patchSymbolTableOffset = concat.addU32Addr(0); // .PointerToSymbolTable
    pp.patchSymbolTableCount  = concat.addU32Addr(0); // .NumberOfSymbols

    concat.addU16(0); // .SizeOfOptionalHeader

    concat.addU16(IMAGE_FILE_LARGE_ADDRESS_AWARE | IMAGE_FILE_DEBUG_STRIPPED); // .Characteristics

    // Code section
    /////////////////////////////////////////////
    concat.addString(".text", 8); // .Name
    concat.addU32(0);             // .VirtualSize
    concat.addU32(0);             // .VirtualAddress

    pp.patchTextSectionSize             = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchTextSectionOffset           = concat.addU32Addr(0); // .PointerToRawData
    pp.patchTextSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                           // .PointerToLinenumbers
    pp.patchTextSectionRelocTableCount = concat.addU16Addr(0);  // .PointerToRelocations
    concat.addU16(0);                                           // .NumberOfLinenumbers
    pp.patchTextSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_16BYTES);

    if (precompileIndex == 0)
    {
        // bss section
        /////////////////////////////////////////////
        concat.addString(".bss", 8);                  // .Name
        concat.addU32(0);                             // .VirtualSize
        concat.addU32(0);                             // .VirtualAddress
        concat.addU32(module->bssSegment.totalCount); // .SizeOfRawData
        concat.addU32(0);                             // .PointerToRawData
        concat.addU32(0);                             // .PointerToRelocations
        concat.addU32(0);                             // .PointerToLinenumbers
        concat.addU16(0);                             // .NumberOfRelocations
        concat.addU16(0);                             // .NumberOfLinenumbers
        concat.addU32(IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);

        // constant section
        /////////////////////////////////////////////
        concat.addString(".rdata", 8);                     // .Name
        concat.addU32(0);                                  // .VirtualSize
        concat.addU32(0);                                  // .VirtualAddress
        concat.addU32(module->constantSegment.totalCount); // .SizeOfRawData
        pp.patchCSOffset = concat.addU32Addr(0);           // .PointerToRawData
        concat.addU32(0);                                  // .PointerToRelocations
        concat.addU32(0);                                  // .PointerToLinenumbers
        concat.addU16(0);                                  // .NumberOfRelocations
        concat.addU16(0);                                  // .NumberOfLinenumbers
        concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ);

        // mutable section
        /////////////////////////////////////////////
        concat.addString(".data", 8);                                            // .Name
        concat.addU32(0);                                                        // .VirtualSize
        concat.addU32(0);                                                        // .VirtualAddress
        pp.patchDSOffset = concat.addU32Addr(module->mutableSegment.totalCount); // .SizeOfRawData
        concat.addU32(0);                                                        // .PointerToRawData
        concat.addU32(0);                                                        // .PointerToRelocations
        concat.addU32(0);                                                        // .PointerToLinenumbers
        concat.addU16(0);                                                        // .NumberOfRelocations
        concat.addU16(0);                                                        // .NumberOfLinenumbers
        concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
    }

    return true;
}

bool BackendX64::emitSymbolTable(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    *pp.patchSymbolTableOffset = concat.totalCount;
    SWAG_ASSERT(pp.allSymbols.size() <= UINT32_MAX);
    *pp.patchSymbolTableCount = (uint32_t) pp.allSymbols.size();

    pp.stringTableOffset = 4;
    for (auto& symbol : pp.allSymbols)
    {
        // .Name
        if (symbol.name.length() <= 8)
            concat.addString(symbol.name.c_str(), 8);
        else
        {
            concat.addU32(0);
            concat.addU32(pp.stringTableOffset);
            pp.stringTable.push_back(&symbol.name);
            pp.stringTableOffset += symbol.name.length() + 1;
        }

        concat.addU32(symbol.value); // .Value
        switch (symbol.kind)
        {
        case CoffSymbolKind::Function:
            concat.addU16(1);                             // .SectionNumber
            concat.addU16(IMAGE_SYM_DTYPE_FUNCTION << 8); // .Type
            concat.addU8(IMAGE_SYM_CLASS_EXTERNAL);       // .StorageClass
            concat.addU8(0);                              // .NumberOfAuxSymbols
            break;
        case CoffSymbolKind::Extern:
            concat.addU16(0);                       // .SectionNumber
            concat.addU16(0);                       // .Type
            concat.addU8(IMAGE_SYM_CLASS_EXTERNAL); // .StorageClass
            concat.addU8(0);                        // .NumberOfAuxSymbols
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    return true;
}

bool BackendX64::emitStringTable(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.addU32(pp.stringTableOffset); // .Size of table in bytes + 4
    for (auto str : pp.stringTable)
        concat.addString(str->c_str(), str->length() + 1);

    return true;
}

bool BackendX64::emitRelocationTable(X64PerThread& pp, CoffRelocationTable& cofftable, uint32_t* sectionFlags, uint32_t* offset, uint16_t* count)
{
    auto& concat = pp.concat;
    *offset      = concat.totalCount;

    SWAG_ASSERT(cofftable.table.size() < UINT32_MAX);
    auto tableSize = (uint32_t) cofftable.table.size();
    if (tableSize > 0xFFFF)
    {
        *count = 0xFFFF;
        *sectionFlags |= IMAGE_SCN_LNK_NRELOC_OVFL;
        concat.addU32(tableSize + 1);
        concat.addU32(0);
        concat.addU16(0);
    }
    else
    {
        *count = (uint16_t) tableSize;
    }

    for (auto& reloc : cofftable.table)
    {
        concat.addU32(reloc.virtualAddress);
        concat.addU32(reloc.symbolIndex);
        concat.addU16(reloc.type);
    }

    return true;
}

bool BackendX64::emitRelocationTables(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];

    // .text
    emitRelocationTable(pp, pp.relocTableTextSection, pp.patchTextSectionFlags, pp.patchTextSectionRelocTableOffset, pp.patchTextSectionRelocTableCount);

    return true;
}

bool BackendX64::generateObjFile(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];

    auto targetPath = Backend::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + pp.filename;
    auto filename   = path;

    ofstream destFile(filename, ios::binary);

    // Output the full concat buffer
    auto bucket = pp.concat.firstBucket;
    while (bucket)
    {
        destFile.write((const char*) bucket->datas, bucket->count);
        bucket = bucket->nextBucket;
    }

    if (precompileIndex == 0)
    {
        // Then the constant segment
        for (auto oneB : module->constantSegment.buckets)
            destFile.write((const char*) oneB.buffer, oneB.count);

        // Then the mutable segment
        for (auto oneB : module->mutableSegment.buckets)
            destFile.write((const char*) oneB.buffer, oneB.count);
    }

    destFile.flush();
    destFile.close();

    return true;
}

bool BackendX64::compile(const BuildParameters& buildParameters)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    vector<string> files;
    files.reserve(numPreCompileBuffers);
    for (auto i = 0; i < numPreCompileBuffers; i++)
        files.push_back(perThread[buildParameters.compileType][i].filename);
    return OS::link(buildParameters, module, files);
}
