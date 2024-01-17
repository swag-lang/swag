#include "pch.h"
#include "BackendSCBEFunctionBodyJob.h"
#include "BackendSCBE.h"
#include "ByteCode.h"
#include "Module.h"
#include "Timer.h"

JobResult BackendSCBEFunctionBodyJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_GenFunc};
#endif

    BackendSCBE* bachendSCBE = (BackendSCBE*) backend;

    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *bachendSCBE->perThread[ct][precompileIndex];
    auto& concat          = pp.concat;

    // :ChkStk
    // Stack probing
    // See SWAG_LIMIT_PAGE_STACK
    if (precompileIndex == 0)
    {
        concat.align(16);
        auto symbolFuncIndex  = pp.getOrAddSymbol("__chkstk", CoffSymbolKind::Function, concat.totalCount() - pp.textSectionOffset)->index;
        auto coffFct          = bachendSCBE->registerFunction(pp, nullptr, symbolFuncIndex);
        coffFct->startAddress = concat.totalCount();

        concat.addString1("\x51");                           // push rcx
        concat.addString1("\x50");                           // push rax
        concat.addString("\x48\x3d\x00\x10\x00\x00", 6);     // cmp rax, 1000h
        concat.addString5("\x48\x8d\x4c\x24\x18");           // lea rcx[rsp+18h]
        concat.addString2("\x72\x18");                       // jb @1
        concat.addString("\x48\x81\xe9\x00\x10\x00\x00", 7); // @2 sub rcx, 1000h
        concat.addString3("\x48\x85\x09");                   // test qword ptr [rcx], rax
        concat.addString("\x48\x2d\x00\x10\x00\x00", 6);     // sub rax, 1000h
        concat.addString("\x48\x3d\x00\x10\x00\x00", 6);     // cmp rax, 1000h
        concat.addString2("\x77\xe8");                       // ja @2
        concat.addString3("\x48\x29\xc1");                   // @1 sub rcx, rax
        concat.addString3("\x48\x85\x09");                   // test qword ptr [rcx], rcx
        concat.addString1("\x58");                           // pop rax
        concat.addString1("\x59");                           // pop rcx
        concat.addString1("\xc3");                           // ret

        coffFct->endAddress = concat.totalCount();
    }

    for (auto one : byteCodeFunc)
    {
        if (one->node && one->node->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO | ATTRIBUTE_COMPILER))
            continue;

        // Emit the internal function
        if (!bachendSCBE->emitFunctionBody(buildParameters, module, one))
            return JobResult::ReleaseJob;
    }

    return JobResult::ReleaseJob;
}
