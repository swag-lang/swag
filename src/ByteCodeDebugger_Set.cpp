#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeDebugger.h"
#include "Module.h"

BcDbgCommandResult ByteCodeDebugger::cmdSet(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split[1] == "inline")
    {
        if (arg.split.size() != 3)
            return BcDbgCommandResult::BadArguments;
        if (arg.split[2] != "on" && arg.split[2] != "off")
            return BcDbgCommandResult::BadArguments;

        context->bc->sourceFile->module->buildCfg.byteCodeDebugInline = arg.split[2] == "on" ? true : false;

        if (context->bc->sourceFile->module->buildCfg.byteCodeDebugInline)
            g_ByteCodeDebugger.printCmdResult("inline mode: on");
        else
            g_ByteCodeDebugger.printCmdResult("inline mode: off");
        g_ByteCodeDebugger.printDebugContext(context, true);
        return BcDbgCommandResult::Continue;
    }

    if (arg.split[1] == "bkp")
    {
        if (arg.split.size() != 3)
            return BcDbgCommandResult::BadArguments;
        if (arg.split[2] != "on" && arg.split[2] != "off")
            return BcDbgCommandResult::BadArguments;

        g_CommandLine.dbgOff = arg.split[2] == "on" ? true : false;

        if (g_CommandLine.dbgOff)
            g_ByteCodeDebugger.printCmdResult("@breakpoint(): on");
        else
            g_ByteCodeDebugger.printCmdResult("@breakpoint(): off");
        return BcDbgCommandResult::Continue;
    }

    return BcDbgCommandResult::BadArguments;
}
