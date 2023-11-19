#include "pch.h"
#include "ByteCodeDebugger.h"

void ByteCodeDebugger::printHelp(const BcDbgCommand& cmd)
{
    Utf8 line;
    line.clear();

    line += cmd.name;
    line += " ";

    while (line.length() < 10)
        line += " ";
    line += cmd.shortname;
    line += " ";

    while (line.length() < 14)
        line += " ";
    line += cmd.args;
    line += " ";

    while (line.length() < 44)
        line += " ";
    line += cmd.help;

    g_Log.print(line);
    g_Log.eol();
}

void ByteCodeDebugger::printHelp()
{
    g_Log.setColor(LogColor::Gray);
    for (auto& c : commands)
        printHelp(c);
}

BcDbgCommandResult ByteCodeDebugger::cmdHelp(ByteCodeRunContext* context, const Vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    if (cmds.size() > 1)
        return BcDbgCommandResult::BadArguments;

    g_ByteCodeDebugger.printHelp();
    return BcDbgCommandResult::Continue;
}