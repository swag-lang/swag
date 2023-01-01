#include "pch.h"
#include "Log.h"
#include "ByteCodeDebugger.h"

void ByteCodeDebugger::printHelp()
{
    g_Log.setColor(LogColor::Gray);

    Utf8 line;
    for (auto& c : commands)
    {
        line.clear();

        line += " ";
        line += c.shortname;
        while (line.length() < 4)
            line += " ";

        line += c.name;
        while (line.length() < 15)
            line += " ";

        line += c.args;
        while (line.length() < 45)
            line += " ";

        line += c.help;
        g_Log.print(line);
        g_Log.eol();
    }
}

BcDbgCommandResult ByteCodeDebugger::cmdHelp(ByteCodeRunContext* context, const vector<Utf8>& cmds, const Utf8& cmdExpr)
{
    printHelp();
    return BcDbgCommandResult::Continue;
}