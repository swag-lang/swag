#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"

void ByteCodeDebugger::printSet(ByteCodeRunContext*)
{
    g_Log.messageHeaderDot("stop on @breakpoint()", !g_CommandLine.dbgOff ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("print struct content", g_ByteCodeDebugger.printStruct ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("print array content", g_ByteCodeDebugger.printArray ? "on" : "off", LogColor::Name, LogColor::White, " ");
}

BcDbgCommandResult ByteCodeDebugger::cmdSet(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() == 1)
    {
        g_ByteCodeDebugger.printSet(context);
        return BcDbgCommandResult::Continue;
    }

    if (arg.split[1] == "bkp")
    {
        if (arg.split.size() != 3)
            return BcDbgCommandResult::BadArguments;
        if (arg.split[2] != "on" && arg.split[2] != "off")
            return BcDbgCommandResult::BadArguments;

        g_CommandLine.dbgOff = arg.split[2] == "off";

        if (g_CommandLine.dbgOff)
            printCmdResult("stop on @breakpoint(): on");
        else
            printCmdResult("stop on @breakpoint(): off");
        return BcDbgCommandResult::Continue;
    }

    if (arg.split[1] == "print")
    {
        if (arg.split.size() != 4)
            return BcDbgCommandResult::BadArguments;

        if (arg.split[2] == "struct")
        {
            if (arg.split[3] != "on" && arg.split[3] != "off")
                return BcDbgCommandResult::BadArguments;

            g_ByteCodeDebugger.printStruct = arg.split[2] == "on";

            if (g_CommandLine.dbgOff)
                printCmdResult("print struct content: on");
            else
                printCmdResult("print struct content: off");
            return BcDbgCommandResult::Continue;
        }

        if (arg.split[2] == "array")
        {
            if (arg.split[3] != "on" && arg.split[3] != "off")
                return BcDbgCommandResult::BadArguments;

            g_ByteCodeDebugger.printArray = arg.split[2] == "on";

            if (g_CommandLine.dbgOff)
                printCmdResult("print array content: on");
            else
                printCmdResult("print array content: off");
            return BcDbgCommandResult::Continue;
        }
    }

    return BcDbgCommandResult::BadArguments;
}
