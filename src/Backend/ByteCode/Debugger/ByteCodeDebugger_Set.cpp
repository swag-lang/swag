#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"

void ByteCodeDebugger::printSet(ByteCodeRunContext*)
{
    g_Log.messageHeaderDot("stop on @breakpoint()", g_CommandLine.dbgOff ? "off" : "on", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("print struct content", g_ByteCodeDebugger.printStruct ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("print array content", g_ByteCodeDebugger.printArray ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("bytecode source code", g_ByteCodeDebugger.printBcCode ? "on" : "off", LogColor::Name, LogColor::White, " ");
    g_Log.messageHeaderDot("backtrace source code", g_ByteCodeDebugger.printBtCode ? "on" : "off", LogColor::Name, LogColor::White, " ");
}

BcDbgCommandResult ByteCodeDebugger::cmdSet(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() == 1)
    {
        g_ByteCodeDebugger.printSet(context);
        return BcDbgCommandResult::Continue;
    }

    if (arg.split[1] == "bkp")
    {
        if (arg.split.size() == 2)
            g_CommandLine.dbgOff = !g_CommandLine.dbgOff;
        else if (arg.split.size() != 3)
            return BcDbgCommandResult::BadArguments;
        else if (arg.split[2] != "on" && arg.split[2] != "off")
            return BcDbgCommandResult::BadArguments;
        else
            g_CommandLine.dbgOff = arg.split[2] == "off";

        if (g_CommandLine.dbgOff)
            printCmdResult("stop on @breakpoint(): off");
        else
            printCmdResult("stop on @breakpoint(): on");
        return BcDbgCommandResult::Continue;
    }

    if (arg.split[1] == "print")
    {
        if (arg.split[2] == "struct")
        {
            if (arg.split.size() == 3)
                g_ByteCodeDebugger.printStruct = !g_ByteCodeDebugger.printStruct;
            else if (arg.split.size() != 4)
                return BcDbgCommandResult::BadArguments;
            else if (arg.split[3] != "on" && arg.split[3] != "off")
                return BcDbgCommandResult::BadArguments;
            else
                g_ByteCodeDebugger.printStruct = arg.split[3] == "on";

            if (g_ByteCodeDebugger.printStruct)
                printCmdResult("print struct content: on");
            else
                printCmdResult("print struct content: off");
            return BcDbgCommandResult::Continue;
        }

        if (arg.split[2] == "array")
        {
            if (arg.split.size() == 3)
                g_ByteCodeDebugger.printArray = !g_ByteCodeDebugger.printArray;
            else if (arg.split.size() != 4)
                return BcDbgCommandResult::BadArguments;
            else if (arg.split[3] != "on" && arg.split[3] != "off")
                return BcDbgCommandResult::BadArguments;
            else
                g_ByteCodeDebugger.printArray = arg.split[3] == "on";

            if (g_ByteCodeDebugger.printArray)
                printCmdResult("print array content: on");
            else
                printCmdResult("print array content: off");
            return BcDbgCommandResult::Continue;
        }
    }

    if (arg.split[1] == "bytecode" || arg.split[1] == "bc")
    {
        if (arg.split[2] == "code")
        {
            if (arg.split.size() == 3)
                g_ByteCodeDebugger.printBcCode = !g_ByteCodeDebugger.printBcCode;
            else if (arg.split.size() != 4)
                return BcDbgCommandResult::BadArguments;
            else if (arg.split[3] != "on" && arg.split[3] != "off")
                return BcDbgCommandResult::BadArguments;
            else
                g_ByteCodeDebugger.printBcCode = arg.split[3] == "on";

            if (g_ByteCodeDebugger.printBcCode)
                printCmdResult("bytecode source code: on");
            else
                printCmdResult("bytecode source code: off");
            return BcDbgCommandResult::Continue;
        }
    }
    
    if (arg.split[1] == "backtrace" || arg.split[1] == "bt")
    {
        if (arg.split[2] == "code")
        {
            if (arg.split.size() == 3)
                g_ByteCodeDebugger.printBtCode = !g_ByteCodeDebugger.printBtCode;
            else if (arg.split.size() != 4)
                return BcDbgCommandResult::BadArguments;
            else if (arg.split[3] != "on" && arg.split[3] != "off")
                return BcDbgCommandResult::BadArguments;
            else
                g_ByteCodeDebugger.printBtCode = arg.split[3] == "on";

            if (g_ByteCodeDebugger.printBtCode)
                printCmdResult("backtrace source code: on");
            else
                printCmdResult("backtrace source code: off");
            return BcDbgCommandResult::Continue;
        }
    }    

    return BcDbgCommandResult::BadArguments;
}

BcDbgCommandResult ByteCodeDebugger::cmdBc(ByteCodeRunContext* /*context*/, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() != 1)
        return BcDbgCommandResult::BadArguments;

    g_ByteCodeDebugger.bcMode = !g_ByteCodeDebugger.bcMode;
    if (g_ByteCodeDebugger.bcMode)
        printCmdResult("bytecode mode");
    else
        printCmdResult("source code mode");
    return BcDbgCommandResult::Continue;
}
