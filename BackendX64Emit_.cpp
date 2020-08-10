#include "pch.h"
#include "BackendX64.h"
#include "BackendX64Inst.h"
#include "Workspace.h"
#include "OS.h"
#include "Module.h"

void BackendX64::emitCall(X64PerThread& pp, const Utf8& name)
{
    auto& concat = pp.concat;

    auto callSym = getOrAddSymbol(pp, name, CoffSymbolKind::Extern);
    if (callSym->kind == CoffSymbolKind::Function)
    {
        concat.addU8(BackendX64Inst::Call);
        concat.addS32((callSym->value + pp.textSectionOffset) - (concat.totalCount + 4));
    }
    else
    {
        concat.addU8(BackendX64Inst::Call);

        CoffRelocation reloc;
        reloc.virtualAddress = concat.totalCount - pp.textSectionOffset;
        reloc.symbolIndex    = callSym->index;
        reloc.type           = IMAGE_REL_AMD64_REL32;
        pp.relocTableTextSection.table.push_back(reloc);

        concat.addS32(0);
    }
}

void BackendX64::emitSymbolToRAX(X64PerThread& pp, uint32_t symbolIndex)
{
    auto& concat = pp.concat;

    concat.addString("\x48\x8D\x05", 3);

    CoffRelocation reloc;
    reloc.virtualAddress = concat.totalCount - pp.textSectionOffset;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = IMAGE_REL_AMD64_REL32;
    pp.relocTableTextSection.table.push_back(reloc);
    concat.addU32(0);
}
