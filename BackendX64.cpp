#include "pch.h"
#include "BackendX64.h"
#include "Workspace.h"
#include "OS.h"
#include "Module.h"

bool BackendX64::createRuntime(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    getOrAddSymbol(pp, "swag_runtime_tlsAlloc", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_tlsGetValue", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_tlsSetValue", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_convertArgcArgv", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_loadDynamicLibrary", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_interfaceof", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_comparestring", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_comparetype", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_print_n", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_print_i64", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_print_f64", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "swag_runtime_assert", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "malloc", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "free", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "realloc", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "memcmp", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "acosf", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "acos", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "asinf", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "asin", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "tanf", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "tan", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "atanf", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "atan", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "sinhf", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "sinh", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "coshf", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "cosh", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "tanhf", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "tanh", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "powf", CoffSymbolKind::Extern);
    getOrAddSymbol(pp, "pow", CoffSymbolKind::Extern);

    return true;
}

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
        createRuntime(buildParameters);
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
            buildRelocConstantSegment(buildParameters, &module->constantSegment, pp.relocTableCSSection);
            buildRelocDataSegment(buildParameters, &module->mutableSegment, pp.relocTableDSSection);
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
            emitMain(buildParameters);
        }

        // This is it for functions
        *pp.patchTextSectionSize = concat.totalCount - pp.textSectionOffset;

        // Tables
        emitSymbolTable(buildParameters);
        emitStringTable(buildParameters);

        *pp.patchTextSectionRelocTableOffset = concat.totalCount;
        emitRelocationTable(pp.concat, pp.relocTableTextSection, pp.patchTextSectionFlags, pp.patchTextSectionRelocTableCount);

        // Segments
        if (precompileIndex == 0)
        {
            // We do not use concat to avoid dummy copies. We will save the segments as they are
            *pp.patchCSOffset = concat.totalCount;
            *pp.patchDSOffset = concat.totalCount + module->constantSegment.totalCount;

            // And we use another concat buffer for relocation tables of segments, because they must be defined after
            // the content
            *pp.patchCSSectionRelocTableOffset = *pp.patchDSOffset + module->mutableSegment.totalCount;
            emitRelocationTable(pp.postConcat, pp.relocTableCSSection, pp.patchCSSectionFlags, pp.patchCSSectionRelocTableCount);

            *pp.patchDSSectionRelocTableOffset = *pp.patchCSSectionRelocTableOffset + pp.postConcat.totalCount;
            emitRelocationTable(pp.postConcat, pp.relocTableDSSection, pp.patchDSSectionFlags, pp.patchDSSectionRelocTableCount);
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
        if (it->kind == kind)
            return it;
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
    SWAG_ASSERT(pp.allSymbols.size() < UINT32_MAX);
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
    pp.sectionIndexText = 1;
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
        pp.sectionIndexBS = 2;
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
        pp.sectionIndexCS = 3;
        concat.addString(".rdata", 8);                            // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        concat.addU32(module->constantSegment.totalCount);        // .SizeOfRawData
        pp.patchCSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchCSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLinenumbers
        pp.patchCSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLinenumbers
        pp.patchCSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ);

        // mutable section
        /////////////////////////////////////////////
        pp.sectionIndexDS = 4;
        concat.addString(".data", 8);                             // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        concat.addU32(module->mutableSegment.totalCount);         // .SizeOfRawData
        pp.patchDSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchDSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLinenumbers
        pp.patchDSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLinenumbers
        pp.patchDSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
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
            concat.addU16(pp.sectionIndexText);           // .SectionNumber
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
        case CoffSymbolKind::CSReloc:
            concat.addU16(pp.sectionIndexCS);       // .SectionNumber
            concat.addU16(0);                       // .Type
            concat.addU8(IMAGE_SYM_CLASS_EXTERNAL); // .StorageClass
            concat.addU8(0);                        // .NumberOfAuxSymbols
            break;
        case CoffSymbolKind::DSReloc:
            concat.addU16(pp.sectionIndexDS);       // .SectionNumber
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

bool BackendX64::emitRelocationTable(Concat& concat, CoffRelocationTable& cofftable, uint32_t* sectionFlags, uint16_t* count)
{
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

        // Then the post concat buffer that contains relocation tables for CS and DS
        bucket = pp.postConcat.firstBucket;
        while (bucket)
        {
            destFile.write((const char*) bucket->datas, bucket->count);
            bucket = bucket->nextBucket;
        }
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
