#include "pch.h"
#include "BackendSCBEDbg.h"
#include "BackendSCBE.h"
#include "ByteCode.h"
#include "BackendSCBEDbg_CodeView.h"
#include "EncoderCPU.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "TypeManager.h"
#include "Version.h"
#include "Workspace.h"

Utf8 BackendSCBEDbg::dbgGetScopedName(AstNode* node)
{
    auto nn = node->getScopedName();
    Utf8 result;
    result.reserve(nn.allocated);

    auto pz      = nn.buffer;
    bool lastDot = false;
    for (uint32_t i = 0; i < nn.length(); i++, pz++)
    {
        if (*pz == '.')
        {
            if (lastDot)
                continue;
            lastDot = true;
            result += "::";
            continue;
        }

        result += *pz;
        lastDot = false;
    }

    return result;
}

void BackendSCBEDbg::dbgSetLocation(CoffFunction* coffFct, ByteCode* bc, ByteCodeInstruction* ip, uint32_t byteOffset)
{
    if (!coffFct->node || coffFct->node->isSpecialFunctionGenerated())
        return;

    if (!ip)
    {
        DbgLine dbgLine;
        dbgLine.line       = coffFct->node->token.startLocation.line + 1;
        dbgLine.byteOffset = byteOffset;
        DbgLines dbgLines;
        dbgLines.sourceFile = coffFct->node->sourceFile;
        dbgLines.lines.push_back(dbgLine);
        coffFct->dbgLines.push_back(dbgLines);
        return;
    }

    if (ip->node->kind == AstNodeKind::FuncDecl)
    {
        return;
    }

    auto loc = ByteCode::getLocation(bc, ip, bc->sourceFile && bc->sourceFile->module->buildCfg.backendDebugInline);
    if (!loc.location)
        return;

    // Update begin of start scope
    auto scope = ip->node->ownerScope;
    while (true)
    {
        if (scope->backendStart == 0)
            scope->backendStart = byteOffset;
        scope->backendEnd = byteOffset;
        if (scope->kind == ScopeKind::Function)
            break;
        scope = scope->parentScope;
    }

    SWAG_ASSERT(!coffFct->dbgLines.empty());

    AstFuncDecl* inlined = nullptr;
    if (ip->node && ip->node->ownerInline)
        inlined = ip->node->ownerInline->func;

    if (coffFct->dbgLines.back().sourceFile != loc.file || inlined != coffFct->dbgLines.back().inlined)
    {
        DbgLines dbgLines;
        dbgLines.sourceFile = loc.file;
        dbgLines.inlined    = inlined;
        coffFct->dbgLines.push_back(dbgLines);
    }

    // Add a new line if it is different from the previous one
    auto& lines = coffFct->dbgLines.back().lines;
    if (lines.empty())
    {
        lines.push_back({loc.location->line + 1, byteOffset});
    }
    else if (lines.back().line != loc.location->line + 1)
    {
        if (lines.back().byteOffset == byteOffset)
            lines.back().line = loc.location->line + 1;
        else
            lines.push_back({loc.location->line + 1, byteOffset});
    }
}

bool BackendSCBEDbg::dbgEmit(const BuildParameters& buildParameters, BackendSCBE* scbe, EncoderCPU& pp)
{
    BackendSCBEDbg_CodeView dbg;
    dbg.scbe = scbe;
    return dbg.dbgEmit(buildParameters, pp);
}
