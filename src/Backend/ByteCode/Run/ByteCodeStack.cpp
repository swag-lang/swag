#include "pch.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Run/ByteCodeRunContext.h"
#include "Report/Log.h"
#include "Syntax/AstFlags.h"
#include "Syntax/AstNode.h"
#include "Syntax/SyntaxColor.h"

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
    const auto fct = node->hasOwnerInline() && node->ownerInline()->ownerFct == ip->node->ownerFct ? node->ownerInline()->func : node->ownerFct;
    if (fct && fct->hasExtByteCode() && fct->extByteCode()->bc)
        return fct->extByteCode()->bc->getPrintName();
    if (fct)
        return fct->token.text;
    return "";
}

namespace
{
    Utf8 sourceLine(SourceFile* sourceFile, uint32_t line)
    {
        auto code = sourceFile->getLine(line);
        code.trim();
        SyntaxColorContext cxt;
        code = syntaxColor(code, cxt);
        code = "      " + code;
        return code;
    }
}

Utf8 ByteCodeStack::getLogStep(int level, bool current, ByteCodeStackStep& step, bool sourceCode)
{
    auto bc = step.bc;
    auto ip = step.ip;

    Utf8 header;
    header += Log::colorToVTS(LogColor::Index);
    if (current)
        header += form("[%03u] ", level);
    else
        header += form(" %03u  ", level);
    header += Log::colorToVTS(LogColor::Name);

    Utf8 inl;
    inl += Log::colorToVTS(LogColor::Index);
    inl += " ---  ";
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
    auto sourceFile = ip->node->token.sourceFile;
    auto location   = ip->location;
    auto name       = getStepName(ip->node, ip);
    if (name.empty())
        name = bc->getPrintName();

    Utf8 str = ip->node->hasOwnerInline() ? inl : header;
    str += name;
    str += Log::colorToVTS(LogColor::White);
    str += " at ";
    str += Log::colorToVTS(LogColor::Location);
    if (sourceFile)
        str += form("%s:%d:%d", sourceFile->path.c_str(), location->line + 1, location->column + 1);
    if (sourceCode)
    {
        str += "\n";
        str += sourceLine(sourceFile, location->line);
    }

    str += "\n";

    // #mixin
    if (ip->node->hasAstFlag(AST_IN_MIXIN))
    {
        auto owner = ip->node->parent;
        while (owner && owner->isNot(AstNodeKind::CompilerMixin))
            owner = owner->parent;
        if (owner)
        {
            str += owner->hasOwnerInline() ? inl : header;
            str += Log::colorToVTS(LogColor::Name);
            str += getStepName(owner, ip);

            if (owner->token.sourceFile)
            {
                str += Log::colorToVTS(LogColor::White);
                str += " at ";
                str += Log::colorToVTS(LogColor::Location);
                str += form("%s:%d:%d:%d:%d",
                            owner->token.sourceFile->path.c_str(),
                            owner->token.startLocation.line + 1,
                            owner->token.startLocation.column + 1,
                            owner->token.endLocation.line + 1,
                            owner->token.endLocation.column + 1);
                if (sourceCode)
                {
                    str += "\n";
                    str += sourceLine(owner->token.sourceFile, owner->token.startLocation.line);
                }
            }

            str += "\n";
        }
    }

    // Inline chain
    auto parent = ip->node->safeOwnerInline();
    while (parent && parent->ownerFct == ip->node->ownerFct)
    {
        str += parent->hasOwnerInline() ? inl.c_str() : header.c_str();
        str += Log::colorToVTS(LogColor::Name);
        str += getStepName(parent, ip);

        if (parent->token.sourceFile)
        {
            str += Log::colorToVTS(LogColor::White);
            str += " at ";
            str += Log::colorToVTS(LogColor::Location);
            str += form("%s:%d:%d:%d:%d",
                        parent->token.sourceFile->path.c_str(),
                        parent->token.startLocation.line + 1,
                        parent->token.startLocation.column + 1,
                        parent->token.endLocation.line + 1,
                        parent->token.endLocation.column + 1);
            if (sourceCode)
            {
                str += "\n";
                str += sourceLine(parent->token.sourceFile, parent->token.startLocation.line);
            }
        }

        str += "\n";
        parent = parent->safeOwnerInline();
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

Utf8 ByteCodeStack::log(const ByteCodeRunContext* runContext, uint32_t maxSteps, bool sourceCode) const
{
    // Add one step for the current context if necessary
    VectorNative<ByteCodeStackStep> copySteps;
    getSteps(copySteps, runContext);
    if (copySteps.empty())
        return "";

    Utf8           str;
    const uint32_t nb = copySteps.size() - 1;
    for (uint32_t i = nb; i != UINT32_MAX; i--)
    {
        bool current = false;
        if (runContext && runContext->debugOn)
            current = static_cast<size_t>(i) == copySteps.size() - 1 - runContext->debugStackFrameOffset;
        str += getLogStep(i, current, copySteps[static_cast<size_t>(i)], sourceCode);

        if (maxSteps)
        {
            maxSteps--;
            if (maxSteps == 0)
                break;
        }
    }

    return str;
}
