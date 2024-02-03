#include "pch.h"
#include "ByteCodeDebugger.h"

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
    g_Log.eol();

    g_Log.setColor(LogColor::Gray);
}

void ByteCodeDebugger::printHelp()
{
    for (auto& c : commands)
        printHelp(c);
}

BcDbgCommandResult ByteCodeDebugger::cmdHelp(ByteCodeRunContext* context, const BcDbgCommandArg& arg)
{
    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;

    if (arg.split.size() == 2)
    {
        bool ok = false;
        for (const auto& c : g_ByteCodeDebugger.commands)
        {
            if (c.name == arg.split[1] || c.shortname == arg.split[1])
            {
                g_Log.setColor(LogColor::Gray);
                g_Log.print(Fmt("command:     %s%s %s%s\n", COLOR_VTS_NAME.c_str(), c.name, COLOR_VTS_TYPE.c_str(), c.args));
                g_Log.setColor(LogColor::Gray);
                g_Log.print(Fmt("short name:  %s\n", c.shortname));
                g_Log.print(Fmt("description: %s\n", c.help));
                g_Log.eol();
                ok = true;
            }
        }

        if (!ok)
            g_ByteCodeDebugger.printCmdError(Fmt("unknown debugger command [[%s]]", arg.split[1].c_str()));
        return BcDbgCommandResult::Continue;
    }

    g_ByteCodeDebugger.printHelp();
    return BcDbgCommandResult::Continue;
}