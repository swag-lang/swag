#include "pch.h"
#include "SCBE.h"
#include "SCBE_Coff.h"
#include "BackendLinker.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Os.h"
#include "Report.h"
#include "SCBE_SaveObjJob.h"
#include "Workspace.h"

SCBE::SCBE(Module* mdl)
    : Backend{mdl}
{
    memset(perThread, 0, sizeof(perThread));
}

SCBE::SCBE()
    : Backend{nullptr}
{
    memset(perThread, 0, sizeof(perThread));
}

bool SCBE::createRuntime(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];

    if (precompileIndex == 0)
    {
        pp.globalSegment.setup(SegmentKind::Global, module);
        pp.stringSegment.setup(SegmentKind::String, module);

        pp.symBSIndex  = pp.getOrAddSymbol("__bs", CPUSymbolKind::Custom, 0, pp.sectionIndexBS)->index;
        pp.symCSIndex  = pp.getOrAddSymbol("__cs", CPUSymbolKind::Custom, 0, pp.sectionIndexCS)->index;
        pp.symMSIndex  = pp.getOrAddSymbol("__ms", CPUSymbolKind::Custom, 0, pp.sectionIndexMS)->index;
        pp.symTLSIndex = pp.getOrAddSymbol("__tls", CPUSymbolKind::Custom, 0, pp.sectionIndexTLS)->index;
        pp.symCOIndex  = pp.getOrAddSymbol(Fmt("__co%d", precompileIndex), CPUSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = pp.getOrAddSymbol(Fmt("__xd%d", precompileIndex), CPUSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

        // This should match the structure SwagContext declared in Runtime.h
        auto offset                         = pp.globalSegment.reserve(sizeof(SwagContext), nullptr, sizeof(uint64_t));
        pp.symMC_mainContext                = pp.getOrAddSymbol("swag_main_context", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        pp.symMC_mainContext_allocator_addr = pp.getOrAddSymbol("swag_main_context_allocator_addr", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_allocator_itable = pp.getOrAddSymbol("swag_main_context_allocator_itable", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_flags = pp.getOrAddSymbol("swag_main_context_flags", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // defaultAllocTable, an interface itable that contains only one entry
        offset                  = pp.globalSegment.reserve(8, nullptr, 8);
        pp.symDefaultAllocTable = pp.getOrAddSymbol("swag_default_alloc_table", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // tls ID
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symTls_threadLocalId = pp.getOrAddSymbol("swag_tls_threadLocalId", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // This should match the structure SwagProcessInfo declared in Runtime.h
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_processInfos   = pp.getOrAddSymbol("swag_process_infos", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        pp.symPI_modulesAddr    = pp.getOrAddSymbol("swag_process_infos_modulesAddr", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_modulesCount   = pp.getOrAddSymbol("swag_process_infos_modulesCount", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_argsAddr       = pp.getOrAddSymbol("swag_process_infos_argsAddr", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_argsCount      = pp.getOrAddSymbol("swag_process_infos_argsCount", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_contextTlsId   = pp.getOrAddSymbol("swag_process_infos_contextTlsId", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_defaultContext = pp.getOrAddSymbol("swag_process_infos_defaultContext", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_byteCodeRun    = pp.getOrAddSymbol("swag_process_infos_byteCodeRun", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_makeCallback   = pp.getOrAddSymbol("swag_process_infos_makeCallback", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_backendKind    = pp.getOrAddSymbol("swag_process_infos_backendKind", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // Constant stuff needed to convert U64 to F64 (code from clang)
        {
            offset                   = pp.globalSegment.reserve(32, nullptr, 2 * sizeof(uint64_t));
            pp.symCst_U64F64         = pp.getOrAddSymbol("swag_cast_u64f64", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
            auto addr                = pp.globalSegment.address(offset);
            *(uint32_t*) (addr + 0)  = 0x43300000;
            *(uint32_t*) (addr + 4)  = 0x45300000;
            *(uint32_t*) (addr + 8)  = 0x00000000;
            *(uint32_t*) (addr + 12) = 0x00000000;
            *(uint64_t*) (addr + 16) = 0x4330000000000000;
            *(uint64_t*) (addr + 24) = 0x4530000000000000;
        }
    }
    else
    {
        pp.symBSIndex  = pp.getOrAddSymbol("__bs", CPUSymbolKind::Extern)->index;
        pp.symCSIndex  = pp.getOrAddSymbol("__cs", CPUSymbolKind::Extern)->index;
        pp.symMSIndex  = pp.getOrAddSymbol("__ms", CPUSymbolKind::Extern)->index;
        pp.symTLSIndex = pp.getOrAddSymbol("__tls", CPUSymbolKind::Extern)->index;
        pp.symCOIndex  = pp.getOrAddSymbol(Fmt("__co%d", precompileIndex), CPUSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = pp.getOrAddSymbol(Fmt("__xd%d", precompileIndex), CPUSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

        pp.symPI_processInfos   = pp.getOrAddSymbol("swag_process_infos", CPUSymbolKind::Extern)->index;
        pp.symPI_modulesAddr    = pp.getOrAddSymbol("swag_process_infos_modulesAddr", CPUSymbolKind::Extern)->index;
        pp.symPI_modulesCount   = pp.getOrAddSymbol("swag_process_infos_modulesCount", CPUSymbolKind::Extern)->index;
        pp.symPI_argsAddr       = pp.getOrAddSymbol("swag_process_infos_argsAddr", CPUSymbolKind::Extern)->index;
        pp.symPI_argsCount      = pp.getOrAddSymbol("swag_process_infos_argsCount", CPUSymbolKind::Extern)->index;
        pp.symPI_contextTlsId   = pp.getOrAddSymbol("swag_process_infos_contextTlsId", CPUSymbolKind::Extern)->index;
        pp.symPI_defaultContext = pp.getOrAddSymbol("swag_process_infos_defaultContext", CPUSymbolKind::Extern)->index;
        pp.symPI_byteCodeRun    = pp.getOrAddSymbol("swag_process_infos_byteCodeRun", CPUSymbolKind::Extern)->index;
        pp.symPI_makeCallback   = pp.getOrAddSymbol("swag_process_infos_makeCallback", CPUSymbolKind::Extern)->index;
        pp.symPI_backendKind    = pp.getOrAddSymbol("swag_process_infos_backendKind", CPUSymbolKind::Extern)->index;
        pp.symTls_threadLocalId = pp.getOrAddSymbol("swag_tls_threadLocalId", CPUSymbolKind::Extern)->index;
        pp.symCst_U64F64        = pp.getOrAddSymbol("swag_cast_u64f64", CPUSymbolKind::Extern)->index;
    }

    return true;
}

JobResult SCBE::prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    SWAG_ASSERT(module == buildParameters.module);

    if (!perThread[ct][precompileIndex])
        perThread[ct][precompileIndex] = new SCBE_X64;

    auto& pp     = *perThread[ct][precompileIndex];
    auto& concat = pp.concat;

    // Message
    if (pp.pass == BackendPreCompilePass::Init && buildParameters.precompileIndex == 0)
        module->startBuilding(buildParameters);

    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    if (pp.pass == BackendPreCompilePass::Init)
    {
        pp.pass = BackendPreCompilePass::FunctionBodies;

        pp.filename = Fmt("%s%d", buildParameters.outputFileName.c_str(), precompileIndex);
        pp.filename += Backend::getObjectFileExtension(g_CommandLine.target);

        SCBE_Coff::emitHeader(buildParameters, pp);
        createRuntime(buildParameters);
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        pp.pass = BackendPreCompilePass::End;

        // Align .text section to 16 bytes
        concat.align(16);
        pp.textSectionOffset       = concat.totalCount();
        *pp.patchTextSectionOffset = pp.textSectionOffset;

        emitAllFunctionBodies(buildParameters, module, ownerJob);
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
        SCBE_Coff::emitPostFunc(buildParameters, pp);

        // Debug sections
        emitDebug(buildParameters);

        if (!pp.relocTableTextSection.table.empty())
        {
            concat.align(16);
            *pp.patchTextSectionRelocTableOffset = concat.totalCount();
            SCBE_Coff::emitRelocationTable(pp.concat, pp.relocTableTextSection, pp.patchTextSectionFlags, pp.patchTextSectionRelocTableCount);
        }

        if (!pp.relocTablePDSection.table.empty())
        {
            concat.align(16);
            *pp.patchPDSectionRelocTableOffset = concat.totalCount();
            SCBE_Coff::emitRelocationTable(pp.concat, pp.relocTablePDSection, pp.patchPDSectionFlags, pp.patchPDSectionRelocTableCount);
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
                SCBE_Coff::emitRelocationTable(pp.postConcat, pp.relocTableCSSection, pp.patchCSSectionFlags, pp.patchCSSectionRelocTableCount);
            }

            uint32_t msRelocOffset = csRelocOffset + pp.postConcat.totalCount();
            if (!pp.relocTableMSSection.table.empty())
            {
                *pp.patchMSSectionRelocTableOffset = msRelocOffset;
                SCBE_Coff::emitRelocationTable(pp.postConcat, pp.relocTableMSSection, pp.patchMSSectionFlags, pp.patchMSSectionRelocTableCount);
            }

            uint32_t tlsRelocOffset = csRelocOffset + pp.postConcat.totalCount();
            if (!pp.relocTableTLSSection.table.empty())
            {
                *pp.patchTLSSectionRelocTableOffset = tlsRelocOffset;
                SCBE_Coff::emitRelocationTable(pp.postConcat, pp.relocTableTLSSection, pp.patchTLSSectionFlags, pp.patchTLSSectionRelocTableCount);
            }
        }
    }

    if (pp.pass == BackendPreCompilePass::GenerateObj)
    {
        pp.pass           = BackendPreCompilePass::Release;
        auto job          = Allocator::alloc<SCBE_SaveObjJob>();
        job->module       = module;
        job->dependentJob = ownerJob;
        job->prepJob      = (ModulePrepOutputStage1Job*) ownerJob;
        ownerJob->jobsToAdd.push_back(job);
        return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}

CPUFunction* SCBE::registerFunction(SCBE_X64& pp, AstNode* node, uint32_t symbolIndex)
{
    CPUFunction cf;
    cf.node        = node;
    cf.symbolIndex = symbolIndex;
    pp.functions.push_back(cf);
    return &pp.functions.back();
}

void SCBE::registerFunction(CPUFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind)
{
    fct->startAddress = startAddress;
    fct->endAddress   = endAddress;
    fct->sizeProlog   = sizeProlog;
    fct->unwind       = std::move(unwind);
}

bool SCBE::saveObjFile(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];

    auto path = Backend::getCacheFolder(buildParameters);
    path.append(pp.filename.c_str());
    auto filename = path;

    FILE* f = nullptr;
    if (fopen_s(&f, filename.string().c_str(), "wb"))
    {
        Report::errorOS(Fmt(Err(Err0524), filename.string().c_str()));
        return false;
    }

    auto objFileType = Backend::getObjType(g_CommandLine.target);
    switch (objFileType)
    {
    case BackendObjType::Coff:
        SCBE_Coff::emitBuffer(f, buildParameters, pp);
        break;
    default:
        Report::internalError(module, "SCBE::saveObjFile, unsupported output");
        break;
    }

    fflush(f);
    fclose(f);
    OS::ensureFileIsWritten(filename.string().c_str());

    pp.concat.release();
    pp.postConcat.release();
    pp.globalSegment.release();
    pp.stringSegment.release();
    pp.relocTableTextSection.table.release();
    pp.relocTableCSSection.table.release();
    pp.relocTableMSSection.table.release();
    pp.relocTableTSSection.table.release();
    pp.relocTableTLSSection.table.release();
    pp.relocTablePDSection.table.release();
    pp.relocTableDBGSSection.table.release();
    pp.allSymbols.release();
    pp.functions.release();
    pp.stringTable.release();
    pp.directives.release();
    pp.mapSymbols.release();
    pp.globalStrings.release();
    pp.dbgTypeRecords.release();
    pp.dbgMapTypes.release();
    pp.dbgMapTypesNames.release();

    return true;
}

bool SCBE::generateOutput(const BuildParameters& buildParameters)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    Vector<Path> files;
    files.reserve(numPreCompileBuffers);
    for (auto i = 0; i < numPreCompileBuffers; i++)
        files.push_back(perThread[buildParameters.compileType][i]->filename);

    return BackendLinker::link(buildParameters, module, files);
}

bool SCBE::emitDebug(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

#ifdef SWAG_STATS
    auto beforeCount = concat.totalCount();
#endif

    SCBE_Debug::emit(buildParameters, this, pp);

#ifdef SWAG_STATS
    g_Stats.sizeBackendDbg += concat.totalCount() - beforeCount;
#endif
    return true;
}
