#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Format/FormatAst.h"
#include "Format/FormatConcat.h"
#include "Report/Log.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/SyntaxColor.h"
#include "Wmf/Workspace.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
BcDbgCommandResult ByteCodeDebugger::cmdWhere(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 1)
        return BcDbgCommandResult::TooManyArguments;

    const auto ipNode = g_ByteCodeDebugger.cxtIp->node;
    const auto bc     = g_ByteCodeDebugger.cxtBc;

    // Node
    if (ipNode && ipNode->ownerFct)
    {
        auto inlined = ipNode->hasExtOwner() ? ipNode->extOwner()->ownerInline : nullptr;
        while (inlined)
        {
            g_ByteCodeDebugger.printTitleNameType("inlined", inlined->func->getScopedName(), inlined->func->typeInfo->getDisplayNameC());
            inlined = inlined->safeOwnerInline();
        }

        g_ByteCodeDebugger.printTitleNameType("function", ipNode->ownerFct->getScopedName(), ipNode->ownerFct->typeInfo->getDisplayNameC());

        if (ipNode->ownerFct->token.sourceFile)
        {
            const auto loc = form("%s:%u:%u", ipNode->ownerFct->token.sourceFile->path.c_str(), ipNode->ownerFct->token.startLocation.line + 1, ipNode->ownerFct->token.startLocation.column + 1);
            g_ByteCodeDebugger.printTitleNameType("function location", loc, "");
        }
    }

    // ByteCode
    g_ByteCodeDebugger.printTitleNameType("bytecode", bc->getPrintName(), bc->typeInfoFunc->getDisplayNameC());
    if (bc->sourceFile && bc->node)
    {
        const auto loc = form("%s:%u:%u", bc->sourceFile->path.c_str(), bc->node->token.startLocation.line + 1, bc->node->token.startLocation.column + 1);
        g_ByteCodeDebugger.printTitleNameType("bytecode location", loc, "");
    }
    else if (bc->sourceFile)
    {
        g_Log.print("bytecode source file: ", LogColor::Gray);
        g_Log.print(bc->sourceFile->path, LogColor::DarkYellow);
        g_Log.writeEol();
    }

    if (ipNode && ipNode->token.sourceFile)
    {
        const auto loc = form("%s:%u:%u", ipNode->token.sourceFile->path.c_str(), ipNode->token.startLocation.line + 1, ipNode->token.startLocation.column + 1);
        g_ByteCodeDebugger.printTitleNameType("instruction location", loc, "");
    }

    const uint32_t maxLevel = g_ByteCodeStackTrace->maxLevel(context);
    g_ByteCodeDebugger.printTitleNameType("stack frame", form("%u", maxLevel - context->debugStackFrameOffset), "");
    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdMemory(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        g_Log.writeEol();

        g_Log.setColor(LogColor::Gray);
        g_Log.print("--num\n");
        g_Log.setColor(LogColor::White);
        g_Log.print("    The number of values to display\n");

        g_Log.setColor(LogColor::Gray);
        g_Log.print("--format\n");
        g_Log.setColor(LogColor::White);
        g_Log.print("    The display format of each value. Can be one of the following:\n");
        g_Log.setColor(LogColor::Type);
        g_Log.print("    s8 s16 s32 s64 u8 u16 u32 u64 x8 x16 x32 x64 f32 f64\n");

        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    Vector<Utf8> exprCmds;
    Utf8::tokenize(arg.cmdExpr, ' ', exprCmds);

    // Print format
    ValueFormat fmt;
    if (!exprCmds.empty())
    {
        if (getValueFormat(exprCmds.back(), fmt))
            exprCmds.pop_back();
        else if (exprCmds.back().startsWith("--"))
        {
            printCmdError(form("invalid format [[%s]]", exprCmds.back().c_str()));
            return BcDbgCommandResult::Error;
        }
    }

    fmt.print0X = false;
    fmt.align   = true;

    // Count
    int count = 64;
    if (exprCmds.size() != 1 &&
        exprCmds.back().length() > 2 &&
        exprCmds.back()[0] == '-' &&
        exprCmds.back()[1] == '-' &&
        Utf8::isNumber(exprCmds.back() + 2))
    {
        count = exprCmds.back().toInt(2);
        exprCmds.pop_back();
    }

    count = max(count, 1);
    count = min(count, 4096);

    // Expression
    Utf8 expr;
    for (const auto& e : exprCmds)
    {
        expr += e;
        expr += " ";
    }

    expr.trim();
    if (expr.empty())
    {
        printCmdError("invalid [[examine]] address");
        return BcDbgCommandResult::Continue;
    }

    EvaluateResult res;
    if (!g_ByteCodeDebugger.evalExpression(context, expr, res))
        return BcDbgCommandResult::Error;

    void* addr = res.addr;
    if (!addr)
        addr = res.value->reg.pointer;
    else if (res.type->isPointer())
        addr = *static_cast<uint8_t**>(res.addr);

    int perLine = 8;
    switch (fmt.bitCount)
    {
        case 8:
            perLine = 16;
            break;
        case 16:
            perLine = 8;
            break;
        case 32:
            perLine = 8;
            break;
        case 64:
            perLine = 4;
            break;
    }

    auto addrB = static_cast<uint8_t*>(addr);
    while (count > 0)
    {
        const auto addrLine = addrB;

        g_Log.print(form("0x%016llx ", addrLine), LogColor::DarkYellow);
        g_Log.setColor(LogColor::Gray);

        for (int i = 0; i < min(count, perLine); i++)
        {
            Utf8 str;
            appendLiteralValue(context, str, fmt, addrB);
            g_Log.print(str);
            addrB += fmt.bitCount / 8;
        }

        addrB = addrLine;
        if (fmt.bitCount == 8)
        {
            // Align to the right
            for (int i = 0; i < perLine - min(count, perLine); i++)
            {
                if (fmt.isHexa)
                    g_Log.write("   ");
                else if (!fmt.isSigned)
                    g_Log.write("    ");
                else
                    g_Log.write("     ");
            }

            // Print as 'char'
            g_Log.write(" ");
            for (int i = 0; i < min(count, perLine); i++)
            {
                auto c = getAddrValue<uint8_t>(addrB);
                if (c < 32 || c > 127)
                    c = '.';
                g_Log.print(form("%c", c));
                addrB += 1;
            }
        }

        g_Log.writeEol();

        addrB = addrLine;
        addrB += static_cast<int64_t>(min(count, perLine)) * (fmt.bitCount / 8);
        count -= min(count, perLine);
        if (!count)
            break;
    }

    return BcDbgCommandResult::Continue;
}

BcDbgCommandResult ByteCodeDebugger::cmdPrint(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
    {
        g_Log.writeEol();

        g_Log.setColor(LogColor::Gray);
        g_Log.print("--format\n");
        g_Log.setColor(LogColor::White);
        g_Log.print("    The display format of the result, if applicable. Can be one of the following:\n");
        g_Log.setColor(LogColor::Type);
        g_Log.print("    s8 s16 s32 s64 u8 u16 u32 u64 x8 x16 x32 x64 f32 f64\n");

        return BcDbgCommandResult::Continue;
    }

    if (arg.split.size() < 2)
        return BcDbgCommandResult::NotEnoughArguments;

    auto expr = arg.cmdExpr;

    ValueFormat fmt;
    fmt.isHexa     = true;
    fmt.bitCount   = 64;
    bool hasFormat = false;
    if (arg.split.size() > 1)
    {
        if (getValueFormat(arg.split.back(), fmt))
        {
            hasFormat = true;
            expr.remove(expr.length() - arg.split.back().length(), arg.split.back().length());
            expr.trim();
            if (expr.empty())
            {
                printCmdError("expression to evaluate is empty");
                return BcDbgCommandResult::Error;
            }
        }
        else if (arg.split.back().startsWith("--"))
        {
            printCmdError(form("invalid format [[%s]]", arg.split.back().c_str()));
            return BcDbgCommandResult::Error;
        }
    }

    EvaluateResult res;
    if (!g_ByteCodeDebugger.evalExpression(context, expr, res))
        return BcDbgCommandResult::Error;

    if (res.type->isVoid())
        return BcDbgCommandResult::Continue;

    const auto concrete = res.type->getConcreteAlias();
    Utf8       str;
    Utf8       strRes;

    if (hasFormat)
    {
        if (!concrete->isNativeIntegerOrRune() && !concrete->isNativeFloat())
        {
            printCmdError(form("cannot apply print format to type [[%s]]", concrete->getDisplayNameC()));
            return BcDbgCommandResult::Error;
        }

        if (!res.addr && res.value)
            res.addr = &res.value->reg;

        appendLiteralValue(context, strRes, fmt, res.addr);
    }
    else
    {
        str = getPrintValue("", res.type);
        appendTypedValue(context, strRes, res, 0, 0);
    }

    str += strRes;

    // Append to automatic expressions
    if (res.type)
    {
        // Do not add a new expression if this is already an expression number
        auto expr1 = expr;
        expr1.trim();
        if (expr1[0] == '$')
            expr1.remove(0, 1);
        while (!expr1.empty() && SWAG_IS_DIGIT(expr1[0]))
            expr1.remove(0, 1);
        expr1.trim();

        if (expr1.length())
        {
            g_ByteCodeDebugger.evalExpr.push_back(expr);
            if (res.type->isNativeIntegerOrRune() || res.type->isPointer() || res.type->isNativeFloat() || res.type->isBool())
                g_ByteCodeDebugger.evalExprResult.push_back(strRes);
            else
                g_ByteCodeDebugger.evalExprResult.push_back(expr);
            g_Log.setColor(LogColor::Gray);
            g_Log.print(form("$%d = ", g_ByteCodeDebugger.evalExpr.size() - 1));
        }
    }

    g_Log.setColor(LogColor::Name);
    g_Log.print(expr);
    g_Log.setColor(LogColor::Default);
    g_Log.write(" = ");

    printLong(str);
    return BcDbgCommandResult::Continue;
}
