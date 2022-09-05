#include "pch.h"
#include "BackendX64.h"
#include "BackendLinker.h"
#include "Module.h"
#include "File.h"
#include "Os.h"
#include "Workspace.h"
#include "BackendX64SaveObjJob.h"
#include "BackendX64FunctionBodyJob.h"

BackendFunctionBodyJobBase* BackendX64::newFunctionJob()
{
    return g_Allocator.alloc<BackendX64FunctionBodyJob>();
}

bool BackendX64::emitHeader(const BuildParameters& buildParameters)
{
    int       ct              = buildParameters.compileType;
    int       precompileIndex = buildParameters.precompileIndex;
    auto&     pp              = *perThread[ct][precompileIndex];
    auto&     concat          = pp.concat;
    const int NUM_SECTIONS_0  = 12;
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
    // concat.addU32((uint32_t)(now & 0xFFFFFFFF)); // .TimeDateStamp
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

        pp.symBSIndex  = pp.getOrAddSymbol("__bs", CoffSymbolKind::Custom, 0, pp.sectionIndexBS)->index;
        pp.symCSIndex  = pp.getOrAddSymbol("__cs", CoffSymbolKind::Custom, 0, pp.sectionIndexCS)->index;
        pp.symMSIndex  = pp.getOrAddSymbol("__ms", CoffSymbolKind::Custom, 0, pp.sectionIndexMS)->index;
        pp.symTLSIndex = pp.getOrAddSymbol("__tls", CoffSymbolKind::Custom, 0, pp.sectionIndexTLS)->index;
        pp.symCOIndex  = pp.getOrAddSymbol(Fmt("__co%d", precompileIndex), CoffSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = pp.getOrAddSymbol(Fmt("__xd%d", precompileIndex), CoffSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

        // This should match the structure SwagContext declared in Runtime.h
        auto offset                         = pp.globalSegment.reserve(sizeof(SwagContext), nullptr, sizeof(uint64_t));
        pp.symMC_mainContext                = pp.getOrAddSymbol("swag_main_context", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        pp.symMC_mainContext_allocator_addr = pp.getOrAddSymbol("swag_main_context_allocator_addr", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_allocator_itable = pp.getOrAddSymbol("swag_main_context_allocator_itable", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_flags = pp.getOrAddSymbol("swag_main_context_flags", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // defaultAllocTable, an interface itable that contains only one entry
        offset                  = pp.globalSegment.reserve(8, nullptr, 8);
        pp.symDefaultAllocTable = pp.getOrAddSymbol("swag_default_alloc_table", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // tls ID
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symTls_threadLocalId = pp.getOrAddSymbol("swag_tls_threadLocalId", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // This should match the structure SwagProcessInfo declared in Runtime.h
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_processInfos   = pp.getOrAddSymbol("swag_process_infos", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        pp.symPI_modulesAddr    = pp.getOrAddSymbol("swag_process_infos_modulesAddr", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_modulesCount   = pp.getOrAddSymbol("swag_process_infos_modulesCount", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_argsAddr       = pp.getOrAddSymbol("swag_process_infos_argsAddr", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_argsCount      = pp.getOrAddSymbol("swag_process_infos_argsCount", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_contextTlsId   = pp.getOrAddSymbol("swag_process_infos_contextTlsId", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_defaultContext = pp.getOrAddSymbol("swag_process_infos_defaultContext", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_byteCodeRun    = pp.getOrAddSymbol("swag_process_infos_byteCodeRun", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_makeCallback   = pp.getOrAddSymbol("swag_process_infos_makeCallback", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_backendKind    = pp.getOrAddSymbol("swag_process_infos_backendKind", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // Constant stuff needed to convert U64 to F64 (code from clang)
        offset                   = pp.globalSegment.reserve(32, nullptr, 2 * sizeof(uint64_t));
        pp.symCst_U64F64         = pp.getOrAddSymbol("swag_cast_u64f64", CoffSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
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
        pp.symBSIndex  = pp.getOrAddSymbol("__bs", CoffSymbolKind::Extern)->index;
        pp.symCSIndex  = pp.getOrAddSymbol("__cs", CoffSymbolKind::Extern)->index;
        pp.symMSIndex  = pp.getOrAddSymbol("__ms", CoffSymbolKind::Extern)->index;
        pp.symTLSIndex = pp.getOrAddSymbol("__tls", CoffSymbolKind::Extern)->index;
        pp.symCOIndex  = pp.getOrAddSymbol(Fmt("__co%d", precompileIndex), CoffSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = pp.getOrAddSymbol(Fmt("__xd%d", precompileIndex), CoffSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

        pp.symPI_processInfos   = pp.getOrAddSymbol("swag_process_infos", CoffSymbolKind::Extern)->index;
        pp.symPI_modulesAddr    = pp.getOrAddSymbol("swag_process_infos_modulesAddr", CoffSymbolKind::Extern)->index;
        pp.symPI_modulesCount   = pp.getOrAddSymbol("swag_process_infos_modulesCount", CoffSymbolKind::Extern)->index;
        pp.symPI_argsAddr       = pp.getOrAddSymbol("swag_process_infos_argsAddr", CoffSymbolKind::Extern)->index;
        pp.symPI_argsCount      = pp.getOrAddSymbol("swag_process_infos_argsCount", CoffSymbolKind::Extern)->index;
        pp.symPI_contextTlsId   = pp.getOrAddSymbol("swag_process_infos_contextTlsId", CoffSymbolKind::Extern)->index;
        pp.symPI_defaultContext = pp.getOrAddSymbol("swag_process_infos_defaultContext", CoffSymbolKind::Extern)->index;
        pp.symPI_byteCodeRun    = pp.getOrAddSymbol("swag_process_infos_byteCodeRun", CoffSymbolKind::Extern)->index;
        pp.symPI_makeCallback   = pp.getOrAddSymbol("swag_process_infos_makeCallback", CoffSymbolKind::Extern)->index;
        pp.symPI_backendKind    = pp.getOrAddSymbol("swag_process_infos_backendKind", CoffSymbolKind::Extern)->index;
        pp.symTls_threadLocalId = pp.getOrAddSymbol("swag_tls_threadLocalId", CoffSymbolKind::Extern)->index;
        pp.symCst_U64F64        = pp.getOrAddSymbol("swag_cast_u64f64", CoffSymbolKind::Extern)->index;
    }

    return true;
}

JobResult BackendX64::prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    if (!perThread[ct][precompileIndex])
        perThread[ct][precompileIndex] = new X64Gen;

    auto& pp     = *perThread[ct][precompileIndex];
    auto& concat = pp.concat;

    // Message
    if (pp.pass == BackendPreCompilePass::Init && buildParameters.precompileIndex == 0)
        module->printStartBuilding(buildParameters);

    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    if (pp.pass == BackendPreCompilePass::Init)
    {
        pp.pass = BackendPreCompilePass::FunctionBodies;

        pp.filename = Fmt("%s%d", buildParameters.outputFileName.c_str(), precompileIndex);
        pp.filename += Backend::getObjectFileExtension(g_CommandLine->target);

        emitHeader(buildParameters);
        createRuntime(buildParameters);
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        pp.pass = BackendPreCompilePass::End;

        // Align .text section to 16 bytes
        concat.align(16);
        pp.textSectionOffset       = concat.totalCount();
        *pp.patchTextSectionOffset = pp.textSectionOffset;

        emitAllFunctionBody(buildParameters, module, ownerJob);

        return JobResult::KeepJobAlive;
    }

    if (pp.pass == BackendPreCompilePass::End)
    {
        if (g_Workspace->bootstrapModule->numErrors || g_Workspace->runtimeModule->numErrors)
            module->numErrors++;
        if (module->numErrors)
            return JobResult::ReleaseJob;
        if (stage == 1)
            return JobResult::ReleaseJob;

        pp.pass = BackendPreCompilePass::GenerateObj;

        // Specific functions in the main file
        if (precompileIndex == 0)
        {
            buildRelocSegment(buildParameters, &module->constantSegment, pp.relocTableCSSection, SegmentKind::Constant);
            buildRelocSegment(buildParameters, &module->mutableSegment, pp.relocTableMSSection, SegmentKind::Data);
            buildRelocSegment(buildParameters, &module->tlsSegment, pp.relocTableTLSSection, SegmentKind::Tls);
            module->constantSegment.applyPatchPtr();
            emitGetTypeTable(buildParameters);
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
            emitGlobalPreMain(buildParameters);
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
            concat.align(16);
            *pp.patchTextSectionRelocTableOffset = concat.totalCount();
            emitRelocationTable(pp.concat, pp.relocTableTextSection, pp.patchTextSectionFlags, pp.patchTextSectionRelocTableCount);
        }

        if (!pp.relocTablePDSection.table.empty())
        {
            concat.align(16);
            *pp.patchPDSectionRelocTableOffset = concat.totalCount();
            emitRelocationTable(pp.concat, pp.relocTablePDSection, pp.patchPDSectionFlags, pp.patchPDSectionRelocTableCount);
        }

        // Align all segments to 16 bytes
        pp.stringSegment.align(16);
        pp.globalSegment.align(16);
        module->constantSegment.align(16);
        module->mutableSegment.align(16);

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
            uint32_t tlsOffset = msOffset + module->mutableSegment.totalCount;

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
        auto job          = g_Allocator.alloc<BackendX64SaveObjJob>();
        job->module       = module;
        job->dependentJob = ownerJob;
        job->prepJob      = (ModulePrepOutputStage1Job*) ownerJob;
        ownerJob->jobsToAdd.push_back(job);
        return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}

CoffFunction* BackendX64::registerFunction(X64Gen& pp, AstNode* node, uint32_t symbolIndex)
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

bool BackendX64::emitXData(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    concat.align(16);
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

    concat.align(16);
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
            memcpy(ptr, symbol.name.buffer, symbol.name.length());
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
        concat.addString(str->buffer, str->count);
        concat.addU8(0);
        subTotal += str->count + 1;
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

bool BackendX64::saveObjFile(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];

    auto targetPath = Backend::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + pp.filename;
    auto filename   = path;

    Concat      concat;
    OutputFile* destFile = OS::newOutputFile();
    if (!destFile->openWrite(filename))
        return false;

    // Output the full concat buffer
    uint32_t totalCount = 0;
    auto     bucket     = pp.concat.firstBucket;
    while (bucket != pp.concat.lastBucket->nextBucket)
    {
        auto count = pp.concat.bucketCount(bucket);
        destFile->save(bucket->datas, count);
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
        destFile->save(oneB.buffer, oneB.count);
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
            destFile->save(oneB.buffer, oneB.count);
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
            destFile->save(oneB.buffer, oneB.count);
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
            destFile->save(oneB.buffer, oneB.count);
        }
        SWAG_ASSERT(subTotal == *pp.patchMSCount || *pp.patchMSCount == 0);
        SWAG_ASSERT(subTotal == module->mutableSegment.totalCount);

        // The tls segment
        SWAG_ASSERT(totalCount == *pp.patchTLSOffset || *pp.patchTLSOffset == 0);
        subTotal = 0;
        for (auto oneB : module->tlsSegment.buckets)
        {
            totalCount += oneB.count;
            subTotal += oneB.count;
            destFile->save(oneB.buffer, oneB.count);
        }
        SWAG_ASSERT(subTotal == *pp.patchTLSCount || *pp.patchTLSCount == 0);
        SWAG_ASSERT(subTotal == module->tlsSegment.totalCount);

        // The post concat buffer that contains relocation tables for CS and DS
        bucket = pp.postConcat.firstBucket;
        while (bucket != pp.postConcat.lastBucket->nextBucket)
        {
            auto count = pp.postConcat.bucketCount(bucket);
            destFile->save(bucket->datas, count);
            bucket = bucket->nextBucket;
        }
    }

    destFile->close();
    OS::freeOutputFile(destFile);

    pp.concat.release();
    pp.postConcat.release();
    pp.globalSegment.release();
    pp.stringSegment.release();

    pp.relocTableTextSection.table.clear();
    pp.relocTableTextSection.table.shrink_to_fit();
    pp.relocTableCSSection.table.clear();
    pp.relocTableCSSection.table.shrink_to_fit();
    pp.relocTableMSSection.table.clear();
    pp.relocTableMSSection.table.shrink_to_fit();
    pp.relocTableTSSection.table.clear();
    pp.relocTableTSSection.table.shrink_to_fit();
    pp.relocTableTLSSection.table.clear();
    pp.relocTableTLSSection.table.shrink_to_fit();
    pp.relocTablePDSection.table.clear();
    pp.relocTablePDSection.table.shrink_to_fit();
    pp.relocTableDBGSSection.table.clear();
    pp.relocTableDBGSSection.table.shrink_to_fit();
    pp.allSymbols.clear();
    pp.allSymbols.shrink_to_fit();
    pp.functions.clear();
    pp.functions.shrink_to_fit();
    pp.dbgTypeRecords.clear();

    pp.directives.reset();

    pp.mapSymbols.clear();
    pp.globalStrings.clear();
    pp.dbgMapTypes.clear();
    pp.dbgMapPtrTypes.clear();
    pp.dbgMapPtrPtrTypes.clear();

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

    return BackendLinker::link(buildParameters, module, files);
}
