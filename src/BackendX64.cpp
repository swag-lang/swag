#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "BackendLinker.h"
#include "Module.h"
#include "BackendX64SaveObjJob.h"

bool BackendX64::emitHeader(const BuildParameters& buildParameters)
{
    int       ct              = buildParameters.compileType;
    int       precompileIndex = buildParameters.precompileIndex;
    auto&     pp              = *perThread[ct][precompileIndex];
    auto&     concat          = pp.concat;
    const int NUM_SECTIONS_0  = 13;
    const int NUM_SECTIONS_X  = 7;

    // Coff header
    /////////////////////////////////////////////
    concat.init();
    concat.addU16(IMAGE_FILE_MACHINE_AMD64); // .Machine
    if (precompileIndex == 0)
        concat.addU16(NUM_SECTIONS_0); // .NumberOfSections
    else
        concat.addU16(NUM_SECTIONS_X); // .NumberOfSections

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
    uint16_t secIndex   = 1;
    pp.sectionIndexText = secIndex++;
    concat.addString(".text\0\0\0", 8);                         // .Name
    concat.addU32(0);                                           // .VirtualSize
    concat.addU32(0);                                           // .VirtualAddress
    pp.patchTextSectionSize             = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchTextSectionOffset           = concat.addU32Addr(0); // .PointerToRawData
    pp.patchTextSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                           // .PointerToLinenumbers
    pp.patchTextSectionRelocTableCount = concat.addU16Addr(0);  // .PointerToRelocations
    concat.addU16(0);                                           // .NumberOfLinenumbers
    pp.patchTextSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_16BYTES);

    // global strings sections
    /////////////////////////////////////////////
    pp.sectionIndexSS = secIndex++;
    concat.addString(".rdata\0\0", 8);       // .Name
    concat.addU32(0);                        // .VirtualSize
    concat.addU32(0);                        // .VirtualAddress
    pp.patchSSCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchSSOffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                        // .PointerToRelocations
    concat.addU32(0);                        // .PointerToLinenumbers
    concat.addU16(0);                        // .NumberOfRelocations
    concat.addU16(0);                        // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_1BYTES);

    // directive section (to register dll exported symbols)
    pp.sectionIndexDR = secIndex++;
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

    // .pdata section (to register functions)
    pp.sectionIndexPD = secIndex++;
    concat.addString(".pdata\0\0", 8);                        // .Name
    concat.addU32(0);                                         // .VirtualSize
    concat.addU32(0);                                         // .VirtualAddress
    pp.patchPDCount                   = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchPDOffset                  = concat.addU32Addr(0); // .PointerToRawData
    pp.patchPDSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                         // .PointerToLinenumbers
    pp.patchPDSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
    concat.addU16(0);                                         // .NumberOfLinenumbers
    pp.patchPDSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    // .xdata section (for unwind infos)
    pp.sectionIndexXD = secIndex++;
    concat.addString(".xdata\0\0", 8);       // .Name
    concat.addU32(0);                        // .VirtualSize
    concat.addU32(0);                        // .VirtualAddress
    pp.patchXDCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchXDOffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                        // .PointerToRelocations
    concat.addU32(0);                        // .PointerToLinenumbers
    concat.addU16(0);                        // .NumberOfRelocations
    concat.addU16(0);                        // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    // .debug$S section
    pp.sectionIndexDBGS = secIndex++;
    concat.addString(".debug$S", 8);                            // .Name
    concat.addU32(0);                                           // .VirtualSize
    concat.addU32(0);                                           // .VirtualAddress
    pp.patchDBGSCount                   = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchDBGSOffset                  = concat.addU32Addr(0); // .PointerToRawData
    pp.patchDBGSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                           // .PointerToLinenumbers
    pp.patchDBGSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
    concat.addU16(0);                                           // .NumberOfLinenumbers
    pp.patchDBGSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_DISCARDABLE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    // .debug$T section
    pp.sectionIndexDBGT = secIndex++;
    concat.addString(".debug$T", 8);           // .Name
    concat.addU32(0);                          // .VirtualSize
    concat.addU32(0);                          // .VirtualAddress
    pp.patchDBGTCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchDBGTOffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                          // .PointerToRelocations
    concat.addU32(0);                          // .PointerToLinenumbers
    concat.addU16(0);                          // .NumberOfRelocations
    concat.addU16(0);                          // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_DISCARDABLE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    if (precompileIndex == 0)
    {
        // constant section
        // This is not readonly because we can patch some stuff during the initialization stage of the module
        /////////////////////////////////////////////
        pp.sectionIndexCS = secIndex++;
        concat.addString(".data\0\0\0", 8);                       // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        pp.patchCSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchCSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchCSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLinenumbers
        pp.patchCSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLinenumbers
        pp.patchCSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // bss section
        /////////////////////////////////////////////
        pp.sectionIndexBS = secIndex++;
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

        // global section
        /////////////////////////////////////////////
        pp.sectionIndexGS = secIndex++;
        concat.addString(".data\0\0\0", 8);      // .Name
        concat.addU32(0);                        // .VirtualSize
        concat.addU32(0);                        // .VirtualAddress
        pp.patchGSCount  = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchGSOffset = concat.addU32Addr(0); // .PointerToRawData
        concat.addU32(0);                        // .PointerToRelocations
        concat.addU32(0);                        // .PointerToLinenumbers
        concat.addU16(0);                        // .NumberOfRelocations
        concat.addU16(0);                        // .NumberOfLinenumbers
        concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // mutable section
        /////////////////////////////////////////////
        pp.sectionIndexMS = secIndex++;
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
        pp.sectionIndexTS = secIndex++;
        concat.addString(".data\0\0\0", 8);                       // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        pp.patchTSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchTSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchTSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLinenumbers
        pp.patchTSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLinenumbers
        pp.patchTSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // tls section
        /////////////////////////////////////////////
        pp.sectionIndexTLS = secIndex++;
        concat.addString(".data\0\0\0", 8);                        // .Name
        concat.addU32(0);                                          // .VirtualSize
        concat.addU32(0);                                          // .VirtualAddress
        pp.patchTLSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchTLSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchTLSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                          // .PointerToLinenumbers
        pp.patchTLSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                          // .NumberOfLinenumbers
        pp.patchTLSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);
    }

    SWAG_ASSERT(precompileIndex != 0 || secIndex == NUM_SECTIONS_0 + 1);
    SWAG_ASSERT(precompileIndex == 0 || secIndex == NUM_SECTIONS_X + 1);

    return true;
}

bool BackendX64::createRuntime(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];

    if (precompileIndex == 0)
    {
        pp.globalSegment.setup(SegmentKind::Global, module);
        pp.stringSegment.setup(SegmentKind::String, module);

        pp.symBSIndex  = getOrAddSymbol(pp, "__bs", CoffSymbolKind::Custom, 0, pp.sectionIndexBS)->index;
        pp.symCSIndex  = getOrAddSymbol(pp, "__cs", CoffSymbolKind::Custom, 0, pp.sectionIndexCS)->index;
        pp.symMSIndex  = getOrAddSymbol(pp, "__ms", CoffSymbolKind::Custom, 0, pp.sectionIndexMS)->index;
        pp.symTSIndex  = getOrAddSymbol(pp, "__ts", CoffSymbolKind::Custom, 0, pp.sectionIndexTS)->index;
        pp.symTLSIndex = getOrAddSymbol(pp, "__tls", CoffSymbolKind::Custom, 0, pp.sectionIndexTLS)->index;
        pp.symCOIndex  = getOrAddSymbol(pp, format("__co%d", precompileIndex), CoffSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = getOrAddSymbol(pp, format("__xd%d", precompileIndex), CoffSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

        // This should match the structure SwagContext declared in Runtime.h
        auto offset                         = pp.globalSegment.reserve(sizeof(SwagContext), sizeof(uint64_t));
        pp.symMC_mainContext                = getOrAddSymbol(pp, "swag_main_context", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        pp.symMC_mainContext_allocator_addr = getOrAddSymbol(pp, "swag_main_context_allocator_addr", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_allocator_itable = getOrAddSymbol(pp, "swag_main_context_allocator_itable", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_flags = getOrAddSymbol(pp, "swag_main_context_flags", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // defaultAllocTable, an interface itable that contains only one entry
        offset                  = pp.globalSegment.reserve(8, 8);
        pp.symDefaultAllocTable = getOrAddSymbol(pp, "swag_default_alloc_table", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // tls ID
        offset                  = pp.globalSegment.reserve(8, sizeof(uint64_t));
        pp.symTls_threadLocalId = getOrAddSymbol(pp, "swag_tls_thread_local_id", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // This should match the structure swag_process_infos_t declared in Runtime.h
        offset                  = pp.globalSegment.reserve(8, sizeof(uint64_t));
        pp.symPI_processInfos   = getOrAddSymbol(pp, "swag_process_infos", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        pp.symPI_args_addr      = getOrAddSymbol(pp, "swag_process_infos_args_addr", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, sizeof(uint64_t));
        pp.symPI_args_count     = getOrAddSymbol(pp, "swag_process_infos_args_count", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, sizeof(uint64_t));
        pp.symPI_contextTlsId   = getOrAddSymbol(pp, "swag_process_infos_contextTlsId", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, sizeof(uint64_t));
        pp.symPI_defaultContext = getOrAddSymbol(pp, "swag_process_infos_defaultContext", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, sizeof(uint64_t));
        pp.symPI_byteCodeRun    = getOrAddSymbol(pp, "swag_process_infos_byteCodeRun", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, sizeof(uint64_t));
        pp.symPI_makeCallback   = getOrAddSymbol(pp, "swag_process_infos_makeCallback", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // Constant stuff needed to convert U64 to F64 (code from clang)
        offset                   = pp.globalSegment.reserve(32, 2 * sizeof(uint64_t));
        pp.symCst_U64F64         = getOrAddSymbol(pp, "swag_cast_u64f64", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        auto addr                = pp.globalSegment.address(offset);
        *(uint32_t*) (addr + 0)  = 0x43300000;
        *(uint32_t*) (addr + 4)  = 0x45300000;
        *(uint32_t*) (addr + 8)  = 0x00000000;
        *(uint32_t*) (addr + 12) = 0x00000000;
        *(uint64_t*) (addr + 16) = 0x4330000000000000;
        *(uint64_t*) (addr + 24) = 0x4530000000000000;
    }
    else
    {
        pp.symBSIndex  = getOrAddSymbol(pp, "__bs", CoffSymbolKind::Extern)->index;
        pp.symCSIndex  = getOrAddSymbol(pp, "__cs", CoffSymbolKind::Extern)->index;
        pp.symMSIndex  = getOrAddSymbol(pp, "__ms", CoffSymbolKind::Extern)->index;
        pp.symTSIndex  = getOrAddSymbol(pp, "__ts", CoffSymbolKind::Extern)->index;
        pp.symTLSIndex = getOrAddSymbol(pp, "__tls", CoffSymbolKind::Extern)->index;
        pp.symCOIndex  = getOrAddSymbol(pp, format("__co%d", precompileIndex), CoffSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = getOrAddSymbol(pp, format("__xd%d", precompileIndex), CoffSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

        pp.symPI_args_addr      = getOrAddSymbol(pp, "swag_process_infos_args_addr", CoffSymbolKind::Extern)->index;
        pp.symPI_args_count     = getOrAddSymbol(pp, "swag_process_infos_args_count", CoffSymbolKind::Extern)->index;
        pp.symPI_contextTlsId   = getOrAddSymbol(pp, "swag_process_infos_contextTlsId", CoffSymbolKind::Extern)->index;
        pp.symPI_defaultContext = getOrAddSymbol(pp, "swag_process_infos_defaultContext", CoffSymbolKind::Extern)->index;
        pp.symPI_byteCodeRun    = getOrAddSymbol(pp, "swag_process_infos_byteCodeRun", CoffSymbolKind::Extern)->index;
        pp.symPI_makeCallback   = getOrAddSymbol(pp, "swag_process_infos_makeCallback", CoffSymbolKind::Extern)->index;
        pp.symTls_threadLocalId = getOrAddSymbol(pp, "swag_tls_thread_local_id", CoffSymbolKind::Extern)->index;
        pp.symCst_U64F64        = getOrAddSymbol(pp, "swag_cast_u64f64", CoffSymbolKind::Extern)->index;
    }

    return true;
}

void BackendX64::alignConcat(Concat& concat, uint32_t align)
{
    while (concat.totalCount() % align)
        concat.addU8(0);
}

JobResult BackendX64::prepareOutput(const BuildParameters& buildParameters, Job* ownerJob)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    if (!perThread[ct][precompileIndex])
        perThread[ct][precompileIndex] = new X64PerThread;

    auto& pp     = *perThread[ct][precompileIndex];
    auto& concat = pp.concat;

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
        pp.filename += Backend::getObjectFileExtension();

        g_Log.verbosePass(LogPassType::Info, "X64 precompile", pp.filename);

        emitHeader(buildParameters);
        createRuntime(buildParameters);
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        pp.pass = BackendPreCompilePass::End;

        // Align .text section to 16 bytes
        alignConcat(concat, 16);
        pp.textSectionOffset       = concat.totalCount();
        *pp.patchTextSectionOffset = pp.textSectionOffset;

        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlive;
    }

    if (pp.pass == BackendPreCompilePass::End)
    {
        pp.pass = BackendPreCompilePass::GenerateObj;

        // Specific functions in the main file
        if (precompileIndex == 0)
        {
            buildRelocSegment(buildParameters, &module->constantSegment, pp.relocTableCSSection, SegmentKind::Constant);
            buildRelocSegment(buildParameters, &module->mutableSegment, pp.relocTableMSSection, SegmentKind::Data);
            buildRelocSegment(buildParameters, &module->typeSegment, pp.relocTableTSSection, SegmentKind::Type);
            buildRelocSegment(buildParameters, &module->tlsSegment, pp.relocTableTLSSection, SegmentKind::Tls);
            module->typeSegment.applyPatchPtr();
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
            emitOS(buildParameters);
            emitMain(buildParameters);
        }

        // This is it for functions
        *pp.patchTextSectionSize = concat.totalCount() - pp.textSectionOffset;

        // Tables
        emitSymbolTable(buildParameters);
        emitStringTable(buildParameters);
        emitDirectives(buildParameters);

        // Unwinding sections
        emitXData(buildParameters);
        emitPData(buildParameters);

        // Debug sections
        emitDebug(buildParameters);

        if (!pp.relocTableTextSection.table.empty())
        {
            alignConcat(concat, 16);
            *pp.patchTextSectionRelocTableOffset = concat.totalCount();
            emitRelocationTable(pp.concat, pp.relocTableTextSection, pp.patchTextSectionFlags, pp.patchTextSectionRelocTableCount);
        }

        if (!pp.relocTablePDSection.table.empty())
        {
            alignConcat(concat, 16);
            *pp.patchPDSectionRelocTableOffset = concat.totalCount();
            emitRelocationTable(pp.concat, pp.relocTablePDSection, pp.patchPDSectionFlags, pp.patchPDSectionRelocTableCount);
        }

        // Align all segments to 16 bytes
        pp.stringSegment.align(16);
        pp.globalSegment.align(16);
        module->constantSegment.align(16);
        module->mutableSegment.align(16);
        module->typeSegment.align(16);

        // Segments
        uint32_t ssOffset = concat.totalCount();
        if (pp.stringSegment.totalCount)
        {
            *pp.patchSSCount  = pp.stringSegment.totalCount;
            *pp.patchSSOffset = ssOffset;
        }

        if (precompileIndex == 0)
        {
            uint32_t gsOffset  = ssOffset + pp.stringSegment.totalCount;
            uint32_t csOffset  = gsOffset + pp.globalSegment.totalCount;
            uint32_t msOffset  = csOffset + module->constantSegment.totalCount;
            uint32_t tsOffset  = msOffset + module->mutableSegment.totalCount;
            uint32_t tlsOffset = tsOffset + module->typeSegment.totalCount;

            // We do not use concat to avoid dummy copies. We will save the segments as they are
            if (pp.globalSegment.totalCount)
            {
                *pp.patchGSCount  = pp.globalSegment.totalCount;
                *pp.patchGSOffset = gsOffset;
            }

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

            if (module->tlsSegment.totalCount)
            {
                *pp.patchTLSCount  = module->tlsSegment.totalCount;
                *pp.patchTLSOffset = tlsOffset;
            }

            // And we use another concat buffer for relocation tables of segments, because they must be defined after
            // the content
            pp.postConcat.init();

            // Warning ! Should be the last segment
            uint32_t csRelocOffset = tlsOffset + module->tlsSegment.totalCount;

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

            uint32_t tlsRelocOffset = csRelocOffset + pp.postConcat.totalCount();
            if (!pp.relocTableTLSSection.table.empty())
            {
                *pp.patchTLSSectionRelocTableOffset = tlsRelocOffset;
                emitRelocationTable(pp.postConcat, pp.relocTableTLSSection, pp.patchTLSSectionFlags, pp.patchTLSSectionRelocTableCount);
            }
        }
    }

    if (pp.pass == BackendPreCompilePass::GenerateObj)
    {
        pp.pass           = BackendPreCompilePass::Release;
        auto job          = g_Pool_backendX64SaveObjJob.alloc();
        job->module       = module;
        job->dependentJob = ownerJob;
        job->prepJob      = (ModulePrepOutputJob*) ownerJob;
        ownerJob->jobsToAdd.push_back(job);
        return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}

CoffFunction* BackendX64::registerFunction(X64PerThread& pp, AstNode* node, uint32_t symbolIndex)
{
    CoffFunction cf;
    cf.node        = node;
    cf.symbolIndex = symbolIndex;
    pp.functions.push_back(cf);
    return &pp.functions.back();
}

void BackendX64::registerFunction(CoffFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind)
{
    fct->startAddress = startAddress;
    fct->endAddress   = endAddress;
    fct->sizeProlog   = sizeProlog;
    fct->unwind       = move(unwind);
}

CoffSymbol* BackendX64::getSymbol(X64PerThread& pp, const Utf8& name)
{
    auto it = pp.mapSymbols.find(name);
    if (it != pp.mapSymbols.end())
        return &pp.allSymbols[it->second];
    return nullptr;
}

CoffSymbol* BackendX64::getOrAddSymbol(X64PerThread& pp, const Utf8& name, CoffSymbolKind kind, uint32_t value, uint16_t sectionIdx)
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

void BackendX64::emitGlobalString(X64PerThread& pp, int precompileIndex, const Utf8& str, uint8_t reg)
{
    BackendX64Inst::emit_Load64_Immediate(pp, 0, reg, true);

    auto&       concat = pp.concat;
    auto        it     = pp.globalStrings.find(str);
    CoffSymbol* sym    = nullptr;
    if (it != pp.globalStrings.end())
        sym = &pp.allSymbols[it->second];
    else
    {
        Utf8 symName          = format("__str%u", (uint32_t) pp.globalStrings.size());
        sym                   = getOrAddSymbol(pp, symName, CoffSymbolKind::GlobalString);
        pp.globalStrings[str] = sym->index;
        sym->value            = pp.stringSegment.addStringNoLock(str);
    }

    CoffRelocation reloc;
    reloc.virtualAddress = (concat.totalCount() - 8) - pp.textSectionOffset;
    reloc.symbolIndex    = sym->index;
    reloc.type           = IMAGE_REL_AMD64_ADDR64;
    pp.relocTableTextSection.table.push_back(reloc);
}

bool BackendX64::emitXData(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    alignConcat(concat, 16);
    *pp.patchXDOffset = concat.totalCount();

    // https://docs.microsoft.com/en-us/cpp/build/exception-handling-x64?view=vs-2019
    uint32_t offset = 0;
    for (auto& f : pp.functions)
    {
        SWAG_ASSERT(f.sizeProlog <= 255);

        f.xdataOffset = offset;
        concat.addU8(1);                         // Version
        concat.addU8((uint8_t) f.sizeProlog);    // Size of prolog
        concat.addU8((uint8_t) f.unwind.size()); // Count of unwind codes
        concat.addU8(0);                         // Frame register | offset
        offset += 4;

        // Unwind array
        for (auto unwind : f.unwind)
        {
            concat.addU16(unwind);
            offset += 2;
        }

        // Align
        if (f.unwind.size() & 1)
        {
            concat.addU16(0);
            offset += 2;
        }
    }

    *pp.patchXDCount = concat.totalCount() - *pp.patchXDOffset;
    return true;
}

bool BackendX64::emitPData(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    alignConcat(concat, 16);
    *pp.patchPDOffset = concat.totalCount();

    uint32_t offset = 0;
    for (auto& f : pp.functions)
    {
        SWAG_ASSERT(f.symbolIndex < pp.allSymbols.size());
        SWAG_ASSERT(f.endAddress > f.startAddress);

        CoffRelocation reloc;
        reloc.type = IMAGE_REL_AMD64_ADDR32NB;

        reloc.virtualAddress = offset;
        reloc.symbolIndex    = f.symbolIndex;
        pp.relocTablePDSection.table.push_back(reloc);
        concat.addU32(0);

        reloc.virtualAddress = offset + 4;
        reloc.symbolIndex    = f.symbolIndex;
        pp.relocTablePDSection.table.push_back(reloc);
        concat.addU32(f.endAddress - f.startAddress);

        reloc.virtualAddress = offset + 8;
        reloc.symbolIndex    = pp.symXDIndex;
        pp.relocTablePDSection.table.push_back(reloc);
        concat.addU32(f.xdataOffset);
        SWAG_ASSERT(f.xdataOffset < *pp.patchXDCount);

        offset += 12;
    }

    *pp.patchPDCount = concat.totalCount() - *pp.patchPDOffset;
    return true;
}

bool BackendX64::emitDirectives(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
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
    auto& pp              = *perThread[ct][precompileIndex];
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
            concat.addU16_safe(pp.sectionIndexSS);     // .SectionNumber
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
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.addU32(pp.stringTableOffset); // .Size of table in bytes + 4
    uint32_t subTotal = 4;
    for (auto str : pp.stringTable)
    {
        concat.addString(str->c_str(), str->length() + 1);

        // Be sure string ends with '0', otherwise this is considered by the linker as a corruption
        // (the last byte of the string table must be 0)
        SWAG_ASSERT(str->buffer[str->count] == 0);

        subTotal += str->length() + 1;
    }

    SWAG_ASSERT(subTotal == pp.stringTableOffset);
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

bool BackendX64::saveObjFile(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];

    auto targetPath = Backend::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + pp.filename;
    auto filename   = path;

    OutputFile    destFile;
    const uint8_t affinity = AFFINITY_ALL ^ AFFINITY_IO;
    destFile.path          = filename;

    // Output the full concat buffer
    uint32_t totalCount = 0;
    auto     bucket     = pp.concat.firstBucket;
    while (bucket != pp.concat.lastBucket->nextBucket)
    {
        auto count = pp.concat.bucketCount(bucket);
        destFile.save(bucket->datas, count, affinity);
        totalCount += count;
        bucket = bucket->nextBucket;
    }

    uint32_t subTotal = 0;
    SWAG_ASSERT(totalCount == pp.concat.totalCount());

    // The global strings segment
    SWAG_ASSERT(totalCount == *pp.patchSSOffset || *pp.patchSSOffset == 0);
    for (auto oneB : pp.stringSegment.buckets)
    {
        totalCount += oneB.count;
        subTotal += oneB.count;
        destFile.save(oneB.buffer, oneB.count, affinity);
    }
    SWAG_ASSERT(subTotal == pp.stringSegment.totalCount);

    if (precompileIndex == 0)
    {
        // The global segment to store context & process infos
        SWAG_ASSERT(totalCount == *pp.patchGSOffset || *pp.patchGSOffset == 0);
        subTotal = 0;
        for (auto oneB : pp.globalSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile.save(oneB.buffer, oneB.count, affinity);
        }
        SWAG_ASSERT(subTotal == *pp.patchGSCount || *pp.patchGSCount == 0);
        SWAG_ASSERT(subTotal == pp.globalSegment.totalCount);

        // The constant segment
        SWAG_ASSERT(totalCount == *pp.patchCSOffset || *pp.patchCSOffset == 0);
        subTotal = 0;
        for (auto oneB : module->constantSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile.save(oneB.buffer, oneB.count, affinity);
        }
        SWAG_ASSERT(subTotal == *pp.patchCSCount || *pp.patchCSCount == 0);
        SWAG_ASSERT(subTotal == module->constantSegment.totalCount);

        // The mutable segment
        SWAG_ASSERT(totalCount == *pp.patchMSOffset || *pp.patchMSOffset == 0);
        subTotal = 0;
        for (auto oneB : module->mutableSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile.save(oneB.buffer, oneB.count, affinity);
        }
        SWAG_ASSERT(subTotal == *pp.patchMSCount || *pp.patchMSCount == 0);
        SWAG_ASSERT(subTotal == module->mutableSegment.totalCount);

        // The type segment
        SWAG_ASSERT(totalCount == *pp.patchTSOffset || *pp.patchTSOffset == 0);
        subTotal = 0;
        for (auto oneB : module->typeSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile.save(oneB.buffer, oneB.count, affinity);
        }
        SWAG_ASSERT(subTotal == *pp.patchTSCount || *pp.patchTSCount == 0);
        SWAG_ASSERT(subTotal == module->typeSegment.totalCount);

        // The tls segment
        SWAG_ASSERT(totalCount == *pp.patchTLSOffset || *pp.patchTLSOffset == 0);
        subTotal = 0;
        for (auto oneB : module->tlsSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile.save(oneB.buffer, oneB.count, affinity);
        }
        SWAG_ASSERT(subTotal == *pp.patchTLSCount || *pp.patchTLSCount == 0);
        SWAG_ASSERT(subTotal == module->tlsSegment.totalCount);

        // The post concat buffer that contains relocation tables for CS and DS
        bucket = pp.postConcat.firstBucket;
        while (bucket != pp.postConcat.lastBucket->nextBucket)
        {
            auto count = pp.postConcat.bucketCount(bucket);
            destFile.save(bucket->datas, count, AFFINITY_ALL ^ AFFINITY_IO);
            bucket = bucket->nextBucket;
        }
    }

    destFile.close();

    pp.concat.release();
    pp.postConcat.release();
    pp.globalSegment.release();
    pp.stringSegment.release();

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
        files.push_back(perThread[buildParameters.compileType][i]->filename);
    auto result = BackendLinker::link(buildParameters, module, files);
    return result;
}
