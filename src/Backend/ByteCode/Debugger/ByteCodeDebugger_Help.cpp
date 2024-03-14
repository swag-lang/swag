#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"

void ByteCodeDebugger::printHelp(const BcDbgCommand& cmd)
{
    g_Log.setColor(LogColor::Gray);

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
    g_Log.writeEol();

    g_Log.setColor(LogColor::Gray);
}

void ByteCodeDebugger::printHelp() const
{
    for (auto& c : commands)
        printHelp(c);
}

BcDbgCommandResult ByteCodeDebugger::cmdHelp(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;

    if (arg.split.size() == 2)
    {
        bool ok = false;
        for (const auto& c : g_ByteCodeDebugger.commands)
        {
            if (arg.split[1] == c.name || arg.split[1] == c.shortname)
            {
                g_Log.setColor(LogColor::Gray);
                g_Log.print(form("command:     %s%s %s%s\n", Log::colorToVTS(LogColor::Name).c_str(), c.name, Log::colorToVTS(LogColor::Type).c_str(), c.args));
                g_Log.setColor(LogColor::Gray);
                g_Log.print(form("short name:  %s\n", c.shortname));
                g_Log.print(form("description: %s\n", c.help));
                g_Log.writeEol();
                ok = true;
            }
        }

        if (!ok)
            printCmdError(form("unknown debugger command [[%s]]", arg.split[1].c_str()));
        return BcDbgCommandResult::Continue;
    }

    g_ByteCodeDebugger.printHelp();
    return BcDbgCommandResult::Continue;
}
