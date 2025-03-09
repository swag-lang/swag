#include "pch.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Backend/SCBE/Encoder/X64/ScbeX64.h"
#include "Backend/SCBE/Obj/ScbeCoff.h"
#include "Backend/SCBE/Obj/ScbeSaveObjJob.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

Scbe::Scbe(Module* mdl) :
    Backend{mdl}
{
}

Scbe::Scbe() :
    Backend{nullptr}
{
    memset(perThread, 0, sizeof(perThread));
}

void Scbe::createRuntime(ScbeCpu& pp)
{
    const auto module          = pp.module;
    const auto precompileIndex = pp.buildParams.precompileIndex;
    if (precompileIndex == 0)
    {
        pp.globalSegment.setup(SegmentKind::Global, module);
        pp.stringSegment.setup(SegmentKind::String, module);

        pp.symBSIndex  = pp.getOrAddSymbol("__bs", CpuSymbolKind::Custom, 0, pp.sectionIndexBS)->index;
        pp.symCSIndex  = pp.getOrAddSymbol("__cs", CpuSymbolKind::Custom, 0, pp.sectionIndexCS)->index;
        pp.symMSIndex  = pp.getOrAddSymbol("__ms", CpuSymbolKind::Custom, 0, pp.sectionIndexMS)->index;
        pp.symTLSIndex = pp.getOrAddSymbol("__tls", CpuSymbolKind::Custom, 0, pp.sectionIndexTLS)->index;
        pp.symCOIndex  = pp.getOrAddSymbol(form("__co%d", precompileIndex), CpuSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = pp.getOrAddSymbol(form("__xd%d", precompileIndex), CpuSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

        // This should match the structure SwagContext declared in Runtime.h
        auto offset                         = pp.globalSegment.reserve(sizeof(SwagContext), nullptr, sizeof(uint64_t));
        pp.symMC_mainContext                = pp.getOrAddSymbol("swag_main_context", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        pp.symMC_mainContext_allocator_addr = pp.getOrAddSymbol("swag_main_context_allocator_addr", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_allocator_itable = pp.getOrAddSymbol("swag_main_context_allocator_itable", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset += sizeof(void*);
        pp.symMC_mainContext_flags = pp.getOrAddSymbol("swag_main_context_flags", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // defaultAllocTable, an interface ITable that contains only one entry
        offset                  = pp.globalSegment.reserve(8, nullptr, 8);
        pp.symDefaultAllocTable = pp.getOrAddSymbol("swag_default_alloc_table", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // tls ID
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symTls_threadLocalId = pp.getOrAddSymbol("swag_tls_threadLocalId", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // This should match the structure SwagProcessInfo declared in Runtime.h
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_processInfos   = pp.getOrAddSymbol("swag_process_infos", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        pp.symPI_modulesAddr    = pp.getOrAddSymbol("swag_process_infos_modulesAddr", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_modulesCount   = pp.getOrAddSymbol("swag_process_infos_modulesCount", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_argsAddr       = pp.getOrAddSymbol("swag_process_infos_argsAddr", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_argsCount      = pp.getOrAddSymbol("swag_process_infos_argsCount", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_contextTlsId   = pp.getOrAddSymbol("swag_process_infos_contextTlsId", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_defaultContext = pp.getOrAddSymbol("swag_process_infos_defaultContext", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_byteCodeRun    = pp.getOrAddSymbol("swag_process_infos_byteCodeRun", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_makeCallback   = pp.getOrAddSymbol("swag_process_infos_makeCallback", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
        offset                  = pp.globalSegment.reserve(8, nullptr, sizeof(uint64_t));
        pp.symPI_backendKind    = pp.getOrAddSymbol("swag_process_infos_backendKind", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;

        // Constant stuff needed to convert U64 to F64 (code from clang)
        if (g_CommandLine.target.arch == SwagTargetArch::X86_64)
        {
            offset                                  = pp.globalSegment.reserve(32, nullptr, 2 * sizeof(uint64_t));
            pp.symCst_U64F64                        = pp.getOrAddSymbol("swag_cast_u64f64", CpuSymbolKind::Custom, offset, pp.sectionIndexGS)->index;
            const auto addr                         = pp.globalSegment.address(offset);
            *reinterpret_cast<uint32_t*>(addr + 0)  = 0x43300000;
            *reinterpret_cast<uint32_t*>(addr + 4)  = 0x45300000;
            *reinterpret_cast<uint32_t*>(addr + 8)  = 0x00000000;
            *reinterpret_cast<uint32_t*>(addr + 12) = 0x00000000;
            *reinterpret_cast<uint64_t*>(addr + 16) = 0x4330000000000000;
            *reinterpret_cast<uint64_t*>(addr + 24) = 0x4530000000000000;
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }
    else
    {
        pp.symBSIndex  = pp.getOrAddSymbol("__bs", CpuSymbolKind::Extern)->index;
        pp.symCSIndex  = pp.getOrAddSymbol("__cs", CpuSymbolKind::Extern)->index;
        pp.symMSIndex  = pp.getOrAddSymbol("__ms", CpuSymbolKind::Extern)->index;
        pp.symTLSIndex = pp.getOrAddSymbol("__tls", CpuSymbolKind::Extern)->index;
        pp.symCOIndex  = pp.getOrAddSymbol(form("__co%d", precompileIndex), CpuSymbolKind::Custom, 0, pp.sectionIndexText)->index;
        pp.symXDIndex  = pp.getOrAddSymbol(form("__xd%d", precompileIndex), CpuSymbolKind::Custom, 0, pp.sectionIndexXD)->index;

        pp.symPI_processInfos   = pp.getOrAddSymbol("swag_process_infos", CpuSymbolKind::Extern)->index;
        pp.symPI_modulesAddr    = pp.getOrAddSymbol("swag_process_infos_modulesAddr", CpuSymbolKind::Extern)->index;
        pp.symPI_modulesCount   = pp.getOrAddSymbol("swag_process_infos_modulesCount", CpuSymbolKind::Extern)->index;
        pp.symPI_argsAddr       = pp.getOrAddSymbol("swag_process_infos_argsAddr", CpuSymbolKind::Extern)->index;
        pp.symPI_argsCount      = pp.getOrAddSymbol("swag_process_infos_argsCount", CpuSymbolKind::Extern)->index;
        pp.symPI_contextTlsId   = pp.getOrAddSymbol("swag_process_infos_contextTlsId", CpuSymbolKind::Extern)->index;
        pp.symPI_defaultContext = pp.getOrAddSymbol("swag_process_infos_defaultContext", CpuSymbolKind::Extern)->index;
        pp.symPI_byteCodeRun    = pp.getOrAddSymbol("swag_process_infos_byteCodeRun", CpuSymbolKind::Extern)->index;
        pp.symPI_makeCallback   = pp.getOrAddSymbol("swag_process_infos_makeCallback", CpuSymbolKind::Extern)->index;
        pp.symPI_backendKind    = pp.getOrAddSymbol("swag_process_infos_backendKind", CpuSymbolKind::Extern)->index;
        pp.symTls_threadLocalId = pp.getOrAddSymbol("swag_tls_threadLocalId", CpuSymbolKind::Extern)->index;
        pp.symCst_U64F64        = pp.getOrAddSymbol("swag_cast_u64f64", CpuSymbolKind::Extern)->index;
    }
}

JobResult Scbe::prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob)
{
    const auto ct              = buildParameters.compileType;
    const auto precompileIndex = buildParameters.precompileIndex;
    const auto objFileType     = getObjType(g_CommandLine.target);

    SWAG_ASSERT(module == buildParameters.module);

    switch (g_CommandLine.target.arch)
    {
        case SwagTargetArch::X86_64:
            allocatePerObj<ScbeX64>(buildParameters);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    auto& pp = encoder<ScbeCpu>(ct, precompileIndex);
    pp.init(buildParameters);

    auto& concat = pp.concat;

    // Message
    if (pp.pass == BackendPreCompilePass::Init && buildParameters.precompileIndex == 0)
        module->startBuilding();

    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    if (pp.pass == BackendPreCompilePass::Init)
    {
        pp.filename = form("%s%d", buildParameters.module->name.cstr(), precompileIndex);
        pp.filename += getObjectFileExtension(g_CommandLine.target);

        switch (objFileType)
        {
            case BackendObjType::Coff:
                ScbeCoff::emitHeader(buildParameters, pp);
                break;
            default:
                Report::internalError(module, "SCBE::prepareOutput, unsupported output");
                break;
        }

        createRuntime(pp);
        pp.pass = BackendPreCompilePass::FunctionBodies;
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        concat.align(16);
        pp.textSectionOffset       = concat.totalCount();
        *pp.patchTextSectionOffset = pp.textSectionOffset;
        emitAllFunctionBodies(buildParameters, ownerJob);
        pp.pass = BackendPreCompilePass::End;
        return JobResult::KeepJobAlive;
    }

    if (pp.pass == BackendPreCompilePass::End)
    {
        if (g_Workspace->bootstrapModule->numErrors || g_Workspace->runtimeModule->numErrors)
            ++module->numErrors;
        if (module->numErrors)
            return JobResult::ReleaseJob;
        if (stage == 1)
            return JobResult::ReleaseJob;

        // Specific functions in the main file
        if (precompileIndex == 0)
        {
            buildRelocationSegment(pp, &module->constantSegment, pp.relocTableCSSection, SegmentKind::Constant);
            buildRelocationSegment(pp, &module->mutableSegment, pp.relocTableMSSection, SegmentKind::Data);
            buildRelocationSegment(pp, &module->tlsSegment, pp.relocTableTLSSection, SegmentKind::Tls);
            module->constantSegment.applyPatchPtr();
            emitGetTypeTable(pp);
            emitGlobalInit(pp);
            emitGlobalDrop(pp);
            emitGlobalPreMain(pp);
            emitOS(pp);
            emitMain(pp);
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
                ScbeCoff::emitPostFunc(pp.buildParams, pp);
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
        const auto job    = Allocator::alloc<ScbeSaveObjJob>();
        job->module       = module;
        job->dependentJob = ownerJob;
        job->prepJob      = reinterpret_cast<ModulePrepOutputStage1Job*>(ownerJob);
        ownerJob->jobsToAdd.push_back(job);
        return JobResult::KeepJobAlive;
    }

    return JobResult::ReleaseJob;
}

void Scbe::saveObjFile(const BuildParameters& buildParameters) const
{
    const auto ct              = buildParameters.compileType;
    const auto precompileIndex = buildParameters.precompileIndex;
    auto&      pp              = encoder<ScbeCpu>(ct, precompileIndex);

    auto path = getCacheFolder();
    path.append(pp.filename);
    const auto filename = path;

    FILE* f = nullptr;
    if (fopen_s(&f, filename, "wb"))
    {
        Report::errorOS(formErr(Err0732, filename.cstr()));
        return;
    }

    const auto objFileType = getObjType(g_CommandLine.target);
    switch (objFileType)
    {
        case BackendObjType::Coff:
            ScbeCoff::saveFileBuffer(f, pp.buildParams, pp);
            break;
        default:
            Report::internalError(module, "SCBE::saveObjFile, unsupported output");
            break;
    }

    (void) fflush(f);
    (void) fclose(f);
    OS::ensureFileIsWritten(filename);

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
}
