#include "pch.h"
#include "ByteCodeStack.h"
#include "AstFlags.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "ByteCodeRunContext.h"
#include "Log.h"
#include "SyntaxColor.h"

thread_local ByteCodeStack  g_ByteCodeStackTraceVal;
thread_local ByteCodeStack* g_ByteCodeStackTrace = &g_ByteCodeStackTraceVal;

uint32_t ByteCodeStack::maxLevel(const ByteCodeRunContext* runContext)
{
    if (!runContext)
        return steps.size() - 1;

    // The last stack step can be the same as the current context. If it's not the case,
    // then it's like we have one more step.
    const auto& back = steps.back();
    if (back.bc == runContext->bc && back.ip == runContext->ip)
        return steps.size() - 1;

    // Not the case. One more step.
    return steps.size();
}

Utf8 ByteCodeStack::getStepName(const AstNode* node, const ByteCodeInstruction* ip)
{
    const auto fct = node->ownerInline && node->ownerInline->ownerFct == ip->node->ownerFct ? node->ownerInline->func : node->ownerFct;
    if (fct && fct->hasExtByteCode() && fct->extByteCode()->bc)
        return fct->extByteCode()->bc->getPrintName();
    if (fct)
        return fct->token.text;
    return "";
}

namespace
{
    Utf8 sourceLine(SourceFile* sourceFile, int line)
    {
        auto code = sourceFile->getLine(line);
        code.trim();
        SyntaxColorContext cxt;
        code = syntaxColor(code, cxt);
        code = "      " + code;
        return code;
    }
}

Utf8 ByteCodeStack::getLogStep(int level, bool current, ByteCodeStackStep& step)
{
    auto bc = step.bc;
    auto ip = step.ip;

    Utf8 header;
    header += Log::colorToVTS(LogColor::Index);
    if (current)
        header += FMT("[%03u] ", level);
    else
        header += FMT("-%03u- ", level);
    header += Log::colorToVTS(LogColor::Name);

    Utf8 inl;
    inl += Log::colorToVTS(LogColor::Index);
    inl += "----- ";
    inl += Log::colorToVTS(LogColor::Name);

    if (!ip)
    {
        auto str = header;
        str += Log::colorToVTS(LogColor::Location);
        str += "<foreign code>";
        str += "\n";
        return str;
    }

    if (!ip->node)
        return "";

    // Current ip
    auto sourceFile = ip->node->sourceFile;
    auto location   = ip->location;
    auto name       = getStepName(ip->node, ip);
    if (name.empty())
        name = bc->getPrintName();

    Utf8 str = ip->node->ownerInline ? inl.c_str() : header.c_str();
    str += name;
    str += "\n";
    str += "      ";
    str += Log::colorToVTS(LogColor::Location);
    if (sourceFile)
        str += FMT(" %s:%d:%d", sourceFile->path.string().c_str(), location->line + 1, location->column + 1);
    str += "\n";
    str += sourceLine(sourceFile, location->line);
    str += "\n";

    // #mixin
    if (ip->node->hasAstFlag(AST_IN_MIXIN))
    {
        auto owner = ip->node->parent;
        while (owner && owner->kind != AstNodeKind::CompilerMixin)
            owner = owner->parent;
        if (owner)
        {
            str += owner->ownerInline ? inl.c_str() : header.c_str();
            str += Log::colorToVTS(LogColor::Name);
            str += getStepName(owner, ip);

            if (owner->sourceFile)
            {
                str += "\n";
                str += "      ";
                str += Log::colorToVTS(LogColor::Location);
                str += FMT(" %s:%d:%d:%d:%d",
                           owner->sourceFile->path.string().c_str(),
                           owner->token.startLocation.line + 1,
                           owner->token.startLocation.column + 1,
                           owner->token.endLocation.line + 1,
                           owner->token.endLocation.column + 1);
                str += "\n";
                str += sourceLine(owner->sourceFile, owner->token.startLocation.line);
            }

            str += "\n";
        }
    }

    // Inline chain
    auto parent = ip->node->ownerInline;
    while (parent && parent->ownerFct == ip->node->ownerFct)
    {
        str += parent->ownerInline ? inl.c_str() : header.c_str();
        str += Log::colorToVTS(LogColor::Name);
        str += getStepName(parent, ip);

        if (parent->sourceFile)
        {
            str += "\n";
            str += "      ";
            str += Log::colorToVTS(LogColor::Location);
            str += FMT(" %s:%d:%d:%d:%d",
                       parent->sourceFile->path.string().c_str(),
                       parent->token.startLocation.line + 1,
                       parent->token.startLocation.column + 1,
                       parent->token.endLocation.line + 1,
                       parent->token.endLocation.column + 1);
            str += "\n";
            str += sourceLine(parent->sourceFile, parent->token.startLocation.line);
        }

        str += "\n";
        parent = parent->ownerInline;
    }

    return str;
}

void ByteCodeStack::getSteps(VectorNative<ByteCodeStackStep>& copySteps, const ByteCodeRunContext* runContext) const
{
    // Add one step for the current context if necessary
    copySteps = steps;
    if (runContext)
    {
        if (copySteps.empty() || copySteps.back().bc != runContext->bc || copySteps.back().ip != runContext->ip)
        {
            ByteCodeStackStep step;
            step.bc    = runContext->bc;
            step.ip    = runContext->ip;
            step.bp    = runContext->bp;
            step.sp    = runContext->sp;
            step.spAlt = runContext->spAlt;
            step.stack = runContext->stack;
            copySteps.push_back(step);
        }
    }
}

Utf8 ByteCodeStack::log(const ByteCodeRunContext* runContext) const
{
    // Add one step for the current context if necessary
    VectorNative<ByteCodeStackStep> copySteps;
    getSteps(copySteps, runContext);
    if (copySteps.empty())
        return "";

    Utf8      str;
    int       maxSteps = 20;
    const int nb       = static_cast<int>(copySteps.size()) - 1;
    for (int i = nb; i >= 0; i--)
    {
        bool current = false;
        if (runContext && runContext->debugOn)
            current = static_cast<size_t>(i) == copySteps.size() - 1 - runContext->debugStackFrameOffset;
        str += getLogStep(i, current, copySteps[static_cast<size_t>(i)]);

        maxSteps--;
        if (maxSteps == 0)
            break;
    }

    return str;
}
