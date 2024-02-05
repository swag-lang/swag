#include "pch.h"
#include "SCBE.h"
#include "BackendLinker.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Os.h"
#include "Report.h"
#include "SCBE_Coff.h"
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

bool SCBE::createRuntime(const BuildParameters& buildParameters) const
{
    const int ct              = buildParameters.compileType;
    const int precompileIndex = buildParameters.precompileIndex;
    auto&     pp              = *perThread[ct][precompileIndex];

    if (precompileIndex == 0)
    {
        pp.globalSegment.setup(SegmentKind::Global, module);
        pp.stringSegment.setup(SegmentKind::String, module);

        pp.symBSIndex  = pp.getOrAddSymbol("__bs", CPUSymbolKind::Custom, 0, pp.sectionIndexBS)->index;
        pp.symCSIndex  = pp.getOrAddSymbol("__cs", CPUSymbolKind::Custom, 0, pp.sectionIndexCS)->index;
        pp.symMSIndex  = pp.getOrAddSymbol("__ms", CPUSymbolKind::Custom, 0, pp.sectionIndexMS)->index;
        pp.symTLSIndex = pp.getOrAddSymbol("__tls", CPUSymbolKind::Custom, 0, pp.sectionIndexTLS)->index;
        pp.symCOIndex  = pp.getOrAddSymbol(FMT("__co%d", precompileIndex), CPUSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = pp.getOrAddSymbol(FMT("__xd%d", precompileIndex), CPUSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

        // This should match the structure SwagContext declared in Runtime.h
        auto offset                         = pp.globalSegment.reserve(sizeof(SwagContext), nullptr, sizeof(uint64_t));
        pp.symMC_mainContext                = pp.getOrAddSymbol("swag_main_context", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        pp.symMC_mainContext_allocator_addr = pp.getOrAddSymbol("swag_main_context_allocator_addr", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_allocator_itable = pp.getOrAddSymbol("swag_main_context_allocator_itable", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_flags = pp.getOrAddSymbol("swag_main_context_flags", CPUSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // defaultAllocTable, an interface ITable that contains only one entry
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
            const auto addr          = pp.globalSegment.address(offset);
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
        pp.symCOIndex  = pp.getOrAddSymbol(FMT("__co%d", precompileIndex), CPUSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = pp.getOrAddSymbol(FMT("__xd%d", precompileIndex), CPUSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

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

JobResult SCBE::prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob)
{
    const int  ct              = buildParameters.compileType;
    const int  precompileIndex = buildParameters.precompileIndex;
    const auto objFileType     = Backend::getObjType(g_CommandLine.target);

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
        pp.filename = FMT("%s%d", buildParameters.outputFileName.c_str(), precompileIndex);
        pp.filename += Backend::getObjectFileExtension(g_CommandLine.target);

        switch (objFileType)
        {
        case BackendObjType::Coff:
            SCBE_Coff::emitHeader(buildParameters, pp);
            break;
        default:
            Report::internalError(module, "SCBE::prepareOutput, unsupported output");
            break;
        }

        createRuntime(buildParameters);
        pp.pass = BackendPreCompilePass::FunctionBodies;
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        concat.align(16);
        pp.textSectionOffset       = concat.totalCount();
        *pp.patchTextSectionOffset = pp.textSectionOffset;
        emitAllFunctionBodies(buildParameters, module, ownerJob);
        pp.pass = BackendPreCompilePass::End;
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

        // Specific functions in the main file
        if (precompileIndex == 0)
        {
            buildRelocationSegment(buildParameters, &module->constantSegment, pp.relocTableCSSection, SegmentKind::Constant);
            buildRelocationSegment(buildParameters, &module->mutableSegment, pp.relocTableMSSection, SegmentKind::Data);
            buildRelocationSegment(buildParameters, &module->tlsSegment, pp.relocTableTLSSection, SegmentKind::Tls);
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

        // Align all segments to 16 bytes
        pp.stringSegment.align(16);
        pp.globalSegment.align(16);
        module->constantSegment.align(16);
        module->mutableSegment.align(16);

        // Emit
        switch (objFileType)
        {
        case BackendObjType::Coff:
            SCBE_Coff::emitPostFunc(buildParameters, pp);
            break;
        default:
            Report::internalError(module, "SCBE::prepareOutput, unsupported output");
            break;
        }

        pp.pass = BackendPreCompilePass::GenerateObj;
    }

    if (pp.pass == BackendPreCompilePass::GenerateObj)
    {
        pp.pass           = BackendPreCompilePass::Release;
        const auto job    = Allocator::alloc<SCBE_SaveObjJob>();
        job->module       = module;
        job->dependentJob = ownerJob;
        job->prepJob      = (ModulePrepOutputStage1Job*) ownerJob;
        ownerJob->jobsToAdd.push_back(job);
        return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}

void SCBE::initFunction(CPUFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind)
{
    fct->startAddress = startAddress;
    fct->endAddress   = endAddress;
    fct->sizeProlog   = sizeProlog;
    fct->unwind       = std::move(unwind);
}

bool SCBE::saveObjFile(const BuildParameters& buildParameters) const
{
    const int ct              = buildParameters.compileType;
    const int precompileIndex = buildParameters.precompileIndex;
    auto&     pp              = *perThread[ct][precompileIndex];

    auto path = Backend::getCacheFolder(buildParameters);
    path.append(pp.filename.c_str());
    const auto filename = path;

    FILE* f = nullptr;
    if (fopen_s(&f, filename.string().c_str(), "wb"))
    {
        Report::errorOS(FMT(Err(Err0096), filename.string().c_str()));
        return false;
    }

    const auto objFileType = Backend::getObjType(g_CommandLine.target);
    switch (objFileType)
    {
    case BackendObjType::Coff:
        SCBE_Coff::saveFileBuffer(f, buildParameters, pp);
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
