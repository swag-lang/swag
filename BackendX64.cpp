#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "Workspace.h"
#include "OS.h"
#include "Module.h"

bool BackendX64::createRuntime(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];

    if (precompileIndex == 0)
    {
        pp.symBSIndex = getOrAddSymbol(pp, "__bs", CoffSymbolKind::Custom, 0, pp.sectionIndexBS)->index;
        pp.symCSIndex = getOrAddSymbol(pp, "__cs", CoffSymbolKind::Custom, 0, pp.sectionIndexCS)->index;
        pp.symMSIndex = getOrAddSymbol(pp, "__ms", CoffSymbolKind::Custom, 0, pp.sectionIndexMS)->index;
        pp.symTSIndex = getOrAddSymbol(pp, "__ts", CoffSymbolKind::Custom, 0, pp.sectionIndexTS)->index;

        // This should match the structure swag_context_t  declared in SwagRuntime.h
        auto offset                           = module->mutableSegment.reserve(8, true);
        pp.symMC_mainContext                  = getOrAddSymbol(pp, "swag_main_context", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;
        pp.symMC_mainContext_allocator_addr   = getOrAddSymbol(pp, "swag_main_context_allocator_addr", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;
        offset                                = module->mutableSegment.reserve(8, true);
        pp.symMC_mainContext_allocator_itable = getOrAddSymbol(pp, "swag_main_context_allocator_itable", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;

        // defaultAllocTable, an interface itable that contains only one entry
        offset                  = module->mutableSegment.reserve(8, true);
        pp.symDefaultAllocTable = getOrAddSymbol(pp, "swag_default_alloc_table", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;

        // This should match the structure swag_process_infos_t declared in SwagRuntime.h
        offset                  = module->mutableSegment.reserve(8, true);
        pp.symPI_processInfos   = getOrAddSymbol(pp, "swag_process_infos", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;
        pp.symPI_args_addr      = getOrAddSymbol(pp, "swag_process_infos_args_addr", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;
        offset                  = module->mutableSegment.reserve(8, true);
        pp.symPI_args_count     = getOrAddSymbol(pp, "swag_process_infos_args_count", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;
        offset                  = module->mutableSegment.reserve(8, true);
        pp.symPI_contextTlsId   = getOrAddSymbol(pp, "swag_process_infos_contextTlsId", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;
        offset                  = module->mutableSegment.reserve(8, true);
        pp.symPI_defaultContext = getOrAddSymbol(pp, "swag_process_infos_defaultContext", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;
        offset                  = module->mutableSegment.reserve(8, true);
        pp.symPI_byteCodeRun    = getOrAddSymbol(pp, "swag_process_infos_byteCodeRun", CoffSymbolKind::Custom, offset, pp.sectionIndexMS)->index;
    }
    else
    {
        pp.symBSIndex = getOrAddSymbol(pp, "__bs", CoffSymbolKind::Extern)->index;
        pp.symCSIndex = getOrAddSymbol(pp, "__cs", CoffSymbolKind::Extern)->index;
        pp.symMSIndex = getOrAddSymbol(pp, "__ms", CoffSymbolKind::Extern)->index;
        pp.symTSIndex = getOrAddSymbol(pp, "__ts", CoffSymbolKind::Extern)->index;

        pp.symPI_args_addr      = getOrAddSymbol(pp, "swag_process_infos_args_addr", CoffSymbolKind::Extern)->index;
        pp.symPI_args_count     = getOrAddSymbol(pp, "swag_process_infos_args_count", CoffSymbolKind::Extern)->index;
        pp.symPI_contextTlsId   = getOrAddSymbol(pp, "swag_process_infos_contextTlsId", CoffSymbolKind::Extern)->index;
        pp.symPI_defaultContext = getOrAddSymbol(pp, "swag_process_infos_defaultContext", CoffSymbolKind::Extern)->index;
        pp.symPI_byteCodeRun    = getOrAddSymbol(pp, "swag_process_infos_byteCodeRun", CoffSymbolKind::Extern)->index;
    }

    return true;
}

JobResult BackendX64::prepareOutput(const BuildParameters& buildParameters, Job* ownerJob)
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
        while (concat.totalCount() % 16)
            concat.addU8(0);
        pp.textSectionOffset       = concat.totalCount();
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
            buildRelocMutableSegment(buildParameters, &module->mutableSegment, pp.relocTableMSSection);
            buildRelocTypeSegment(buildParameters, &module->typeSegment, pp.relocTableTSSection);
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
            emitMain(buildParameters);
        }

        // This is it for functions
        *pp.patchTextSectionSize = concat.totalCount() - pp.textSectionOffset;

        // Tables
        emitSymbolTable(buildParameters);
        emitStringTable(buildParameters);
        emitDirectives(buildParameters);

        if (!pp.relocTableTextSection.table.empty())
        {
            *pp.patchTextSectionRelocTableOffset = concat.totalCount();
            emitRelocationTable(pp.concat, pp.relocTableTextSection, pp.patchTextSectionFlags, pp.patchTextSectionRelocTableCount);
        }

        // Segments
        if (precompileIndex == 0)
        {
            uint32_t csOffset = concat.totalCount();
            uint32_t msOffset = csOffset + module->constantSegment.totalCount;
            uint32_t tsOffset = msOffset + module->mutableSegment.totalCount;

            // We do not use concat to avoid dummy copies. We will save the segments as they are
            if (module->constantSegment.totalCount)
            {
                *pp.patchCSCount  = module->constantSegment.totalCount;
                *pp.patchCSOffset = csOffset;
            }

            if (module->mutableSegment.totalCount)
            {
                *pp.patchMSCount  = module->mutableSegment.totalCount;
                *pp.patchMSOffset = msOffset;
            }

            if (module->typeSegment.totalCount)
            {
                *pp.patchTSCount  = module->typeSegment.totalCount;
                *pp.patchTSOffset = tsOffset;
            }

            // And we use another concat buffer for relocation tables of segments, because they must be defined after
            // the content
            pp.postConcat.init();
            uint32_t csRelocOffset = tsOffset + module->typeSegment.totalCount;
            if (!pp.relocTableCSSection.table.empty())
            {
                *pp.patchCSSectionRelocTableOffset = csRelocOffset;
                emitRelocationTable(pp.postConcat, pp.relocTableCSSection, pp.patchCSSectionFlags, pp.patchCSSectionRelocTableCount);
            }

            uint32_t msRelocOffset = csRelocOffset + pp.postConcat.totalCount();
            if (!pp.relocTableMSSection.table.empty())
            {
                *pp.patchMSSectionRelocTableOffset = msRelocOffset;
                emitRelocationTable(pp.postConcat, pp.relocTableMSSection, pp.patchMSSectionFlags, pp.patchMSSectionRelocTableCount);
            }

            uint32_t tsRelocOffset = csRelocOffset + pp.postConcat.totalCount();
            if (!pp.relocTableTSSection.table.empty())
            {
                *pp.patchTSSectionRelocTableOffset = tsRelocOffset;
                emitRelocationTable(pp.postConcat, pp.relocTableTSSection, pp.patchTSSectionFlags, pp.patchTSSectionRelocTableCount);
            }
        }
        else
        {
            uint32_t csOffset = concat.totalCount();
            if (pp.stringSegment.totalCount)
            {
                *pp.patchCSOffset = csOffset;
                *pp.patchCSCount  = pp.stringSegment.totalCount;
            }
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

CoffSymbol* BackendX64::getOrAddSymbol(X64PerThread& pp, const Utf8Crc& name, CoffSymbolKind kind, uint32_t value, uint16_t sectionIdx)
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
    sym.name       = name;
    sym.kind       = kind;
    sym.value      = value;
    sym.sectionIdx = sectionIdx;
    SWAG_ASSERT(pp.allSymbols.size() < UINT32_MAX);
    sym.index = (uint32_t) pp.allSymbols.size();
    pp.allSymbols.emplace_back(sym);
    pp.mapSymbols[name] = (uint32_t) pp.allSymbols.size() - 1;
    return &pp.allSymbols.back();
}

void BackendX64::emitGlobalString(X64PerThread& pp, int precompileIndex, const Utf8Crc& str, uint8_t reg)
{
    BackendX64Inst::emit_Load64_Immediate(pp, 0, reg, true);

    auto&       concat = pp.concat;
    auto        it     = pp.globalStrings.find(str);
    CoffSymbol* sym    = nullptr;
    if (it != pp.globalStrings.end())
        sym = &pp.allSymbols[it->second];
    else
    {
        Utf8 symName          = format("__str_%u_%s", (uint32_t) pp.globalStrings.size(), pp.filename.c_str());
        sym                   = getOrAddSymbol(pp, symName, CoffSymbolKind::GlobalString);
        pp.globalStrings[str] = sym->index;

        // Use the constant segment for the main thread (obj 0), and use the stringSegment for others
        if (precompileIndex == 0)
            sym->value = module->constantSegment.addStringNoLock(str);
        else
            sym->value = pp.stringSegment.addStringNoLock(str);
    }

    CoffRelocation reloc;
    reloc.virtualAddress = (concat.totalCount() - 8) - pp.textSectionOffset;
    reloc.symbolIndex    = sym->index;
    reloc.type           = IMAGE_REL_AMD64_ADDR64;
    pp.relocTableTextSection.table.push_back(reloc);
}

bool BackendX64::emitHeader(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    // Coff header
    /////////////////////////////////////////////
    concat.init();
    concat.addU16(IMAGE_FILE_MACHINE_AMD64); // .Machine
    if (precompileIndex == 0)
        concat.addU16(6); // .NumberOfSections
    else
        concat.addU16(3); // .NumberOfSections

    time_t now;
    time(&now);
    //concat.addU32((uint32_t)(now & 0xFFFFFFFF)); // .TimeDateStamp
    concat.addU32(0);

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

    // constant section
    // as an additional thread can create strings, we have a constant segment too
    /////////////////////////////////////////////
    pp.sectionIndexCS = 2;
    concat.addString(".rdata\0\0", 8);                        // .Name
    concat.addU32(0);                                         // .VirtualSize
    concat.addU32(0);                                         // .VirtualAddress
    pp.patchCSCount                   = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchCSOffset                  = concat.addU32Addr(0); // .PointerToRawData
    pp.patchCSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                         // .PointerToLinenumbers
    pp.patchCSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
    concat.addU16(0);                                         // .NumberOfLinenumbers
    pp.patchCSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_1BYTES);

    // directive section (to register dll exported symbols)
    pp.sectionIndexDR = 3;
    concat.addString(".drectve", 8);         // .Name
    concat.addU32(0);                        // .VirtualSize
    concat.addU32(0);                        // .VirtualAddress
    pp.patchDRCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchDROffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                        // .PointerToRelocations
    concat.addU32(0);                        // .PointerToLinenumbers
    concat.addU16(0);                        // .NumberOfRelocations
    concat.addU16(0);                        // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_1BYTES | IMAGE_SCN_LNK_INFO);

    if (precompileIndex == 0)
    {
        // bss section
        /////////////////////////////////////////////
        pp.sectionIndexBS = 4;
        concat.addString(".bss\0\0\0\0", 8);          // .Name
        concat.addU32(0);                             // .VirtualSize
        concat.addU32(0);                             // .VirtualAddress
        concat.addU32(module->bssSegment.totalCount); // .SizeOfRawData
        concat.addU32(0);                             // .PointerToRawData
        concat.addU32(0);                             // .PointerToRelocations
        concat.addU32(0);                             // .PointerToLinenumbers
        concat.addU16(0);                             // .NumberOfRelocations
        concat.addU16(0);                             // .NumberOfLinenumbers
        concat.addU32(IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // mutable section
        /////////////////////////////////////////////
        pp.sectionIndexMS = 5;
        concat.addString(".data\0\0\0", 8);                       // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        pp.patchMSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchMSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchMSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLinenumbers
        pp.patchMSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLinenumbers
        pp.patchMSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // type section
        /////////////////////////////////////////////
        pp.sectionIndexTS = 6;
        concat.addString(".rdata\0\0\0", 8);                      // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        pp.patchTSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchTSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchTSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLinenumbers
        pp.patchTSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLinenumbers
        pp.patchTSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_1BYTES);
    }

    return true;
}

bool BackendX64::emitDirectives(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    if (pp.directives.empty())
        return true;
    *pp.patchDROffset = concat.totalCount();
    *pp.patchDRCount  = pp.directives.length();
    concat.addString(pp.directives);
    return true;
}

bool BackendX64::emitSymbolTable(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    *pp.patchSymbolTableOffset = concat.totalCount();
    SWAG_ASSERT(pp.allSymbols.size() <= UINT32_MAX);
    *pp.patchSymbolTableCount = (uint32_t) pp.allSymbols.size();

    pp.stringTableOffset = 4;
    for (auto& symbol : pp.allSymbols)
    {
        concat.ensureSpace(18);

        // .Name
        if (symbol.name.length() <= 8)
        {
            // Be sure it's stuffed with 0 after the name, or we can have weird things
            // in the compiler
            concat.addU64_safe(0);
            auto ptr = concat.getSeekPtr() - 8;
            memcpy(ptr, symbol.name.c_str(), symbol.name.length());
        }
        else
        {
            concat.addU32_safe(0);
            concat.addU32_safe(pp.stringTableOffset);
            pp.stringTable.push_back(&symbol.name);
            pp.stringTableOffset += symbol.name.length() + 1;
        }

        concat.addU32(symbol.value); // .Value
        switch (symbol.kind)
        {
        case CoffSymbolKind::Function:
            concat.addU16_safe(pp.sectionIndexText);           // .SectionNumber
            concat.addU16_safe(IMAGE_SYM_DTYPE_FUNCTION << 8); // .Type
            concat.addU8_safe(IMAGE_SYM_CLASS_EXTERNAL);       // .StorageClass
            concat.addU8_safe(0);                              // .NumberOfAuxSymbols
            break;
        case CoffSymbolKind::Extern:
            concat.addU16_safe(0);                       // .SectionNumber
            concat.addU16_safe(0);                       // .Type
            concat.addU8_safe(IMAGE_SYM_CLASS_EXTERNAL); // .StorageClass
            concat.addU8_safe(0);                        // .NumberOfAuxSymbols
            break;
        case CoffSymbolKind::Custom:
            concat.addU16_safe(symbol.sectionIdx);       // .SectionNumber
            concat.addU16_safe(0);                       // .Type
            concat.addU8_safe(IMAGE_SYM_CLASS_EXTERNAL); // .StorageClass
            concat.addU8_safe(0);                        // .NumberOfAuxSymbols
            break;
        case CoffSymbolKind::GlobalString:
            concat.addU16_safe(pp.sectionIndexCS);     // .SectionNumber
            concat.addU16_safe(0);                     // .Type
            concat.addU8_safe(IMAGE_SYM_CLASS_STATIC); // .StorageClass
            concat.addU8_safe(0);                      // .NumberOfAuxSymbols
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
        concat.ensureSpace(4 + 4 + 2);
        concat.addU32_safe(tableSize + 1);
        concat.addU32_safe(0);
        concat.addU16_safe(0);
    }
    else
    {
        *count = (uint16_t) tableSize;
    }

    for (auto& reloc : cofftable.table)
    {
        concat.ensureSpace(4 + 4 + 2);
        concat.addU32_safe(reloc.virtualAddress);
        concat.addU32_safe(reloc.symbolIndex);
        concat.addU16_safe(reloc.type);
    }

    return true;
}

void BackendX64::emitSymbolRelocation(X64PerThread& pp, const Utf8& name)
{
    auto& concat  = pp.concat;
    auto  callSym = getOrAddSymbol(pp, name, CoffSymbolKind::Extern);
    if (callSym->kind == CoffSymbolKind::Function)
    {
        concat.addS32((callSym->value + pp.textSectionOffset) - (concat.totalCount() + 4));
    }
    else
    {
        CoffRelocation reloc;
        reloc.virtualAddress = concat.totalCount() - pp.textSectionOffset;
        reloc.symbolIndex    = callSym->index;
        reloc.type           = IMAGE_REL_AMD64_REL32;
        pp.relocTableTextSection.table.push_back(reloc);

        concat.addU32(0);
    }
}

void BackendX64::emitCall(X64PerThread& pp, const Utf8& name)
{
    auto& concat = pp.concat;
    concat.addU8(0xE8); // call
    emitSymbolRelocation(pp, name);
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
    if (!destFile.is_open())
    {
        module->error(format("unable to write output file '%s'", filename.c_str()));
        return false;
    }

    // Output the full concat buffer
    uint32_t totalCount = 0;
    auto     bucket     = pp.concat.firstBucket;
    while (bucket != pp.concat.lastBucket->nextBucket)
    {
        auto count = pp.concat.bucketCount(bucket);
        destFile.write((const char*) bucket->datas, count);
        totalCount += count;
        bucket = bucket->nextBucket;
    }

    uint32_t subTotal = 0;
    SWAG_ASSERT(totalCount == pp.concat.totalCount());
    if (precompileIndex)
    {
        // Then the constant segment
        SWAG_ASSERT(totalCount == *pp.patchCSOffset || *pp.patchCSOffset == 0);
        for (auto oneB : pp.stringSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile.write((const char*) oneB.buffer, oneB.count);
        }
        SWAG_ASSERT(subTotal == pp.stringSegment.totalCount);
    }
    else
    {
        // Then the constant segment
        SWAG_ASSERT(totalCount == *pp.patchCSOffset || *pp.patchCSOffset == 0);
        for (auto oneB : module->constantSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile.write((const char*) oneB.buffer, oneB.count);
        }
        SWAG_ASSERT(subTotal == module->constantSegment.totalCount);
        subTotal = 0;

        // Then the mutable segment
        SWAG_ASSERT(totalCount == *pp.patchMSOffset || *pp.patchMSOffset == 0);
        subTotal = 0;
        for (auto oneB : module->mutableSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile.write((const char*) oneB.buffer, oneB.count);
        }
        SWAG_ASSERT(subTotal == module->mutableSegment.totalCount);

        // Then the type segment
        SWAG_ASSERT(totalCount == *pp.patchTSOffset || *pp.patchTSOffset == 0);
        subTotal = 0;
        for (auto oneB : module->typeSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile.write((const char*) oneB.buffer, oneB.count);
        }
        SWAG_ASSERT(subTotal == module->typeSegment.totalCount);

        // Then the post concat buffer that contains relocation tables for CS and DS
        bucket = pp.postConcat.firstBucket;
        while (bucket != pp.postConcat.lastBucket->nextBucket)
        {
            auto count = pp.postConcat.bucketCount(bucket);
            destFile.write((const char*) bucket->datas, count);
            bucket = bucket->nextBucket;
        }
    }

    destFile.flush();
    destFile.close();

    return true;
}

bool BackendX64::generateOutput(const BuildParameters& buildParameters)
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
