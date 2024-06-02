#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"

void ByteCodeDebugger::printHelp(const BcDbgCommand& cmd)
{
    g_Log.setColor(LogColor::Gray);

    Utf8 line;
    Utf8 lineRaw;

    line += Log::colorToVTS(LogColor::Name);
    line += cmd.name;
    lineRaw += cmd.name;
    line += " ";
    lineRaw += " ";

    while (lineRaw.length() < 10)
    {
        line += " ";
        lineRaw += " ";
    }

    line += cmd.shortname;
    lineRaw += cmd.shortname;
    line += " ";
    lineRaw += " ";

    line += Log::colorToVTS(LogColor::Type);
    while (lineRaw.length() < 14)
    {
        line += " ";
        lineRaw += " ";
    }

    line += cmd.args;
    lineRaw += cmd.args;
    line += " ";
    lineRaw += " ";

    line += Log::colorToVTS(LogColor::White);
    while (lineRaw.length() < 44)
    {
        line += " ";
        lineRaw += " ";
    }

    line += cmd.help;
    lineRaw += cmd.help;

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
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 2)
        return BcDbgCommandResult::BadArguments;

    // List all commands
    if (arg.split.size() == 1)
    {
        g_ByteCodeDebugger.printHelp();
        return BcDbgCommandResult::Continue;
    }

    // Help on a specific command
    bool ok = false;
    for (const auto& c : g_ByteCodeDebugger.commands)
    {
        if (!c.name)
            continue;
        if (arg.split[1] != c.name && (!c.shortname || arg.split[1] != c.shortname))
            continue;

        g_Log.setColor(LogColor::Gray);
        g_Log.print(form("command:     %s%s %s%s\n", Log::colorToVTS(LogColor::Name).c_str(), c.name, Log::colorToVTS(LogColor::Type).c_str(), c.args));

        if (c.shortname)
        {
            g_Log.setColor(LogColor::Gray);
            g_Log.print(form("short name:  %s%s\n", Log::colorToVTS(LogColor::Name).c_str(), c.shortname));
        }

        g_Log.setColor(LogColor::Gray);
        g_Log.print(form("description: %s%s\n", Log::colorToVTS(LogColor::White).c_str(), c.help));
        g_Log.setColor(LogColor::Gray);
        g_Log.writeEol();

        if(c.cb)
        {
            BcDbgCommandArg arg1 = arg;
            arg1.help = true;
            c.cb(nullptr, arg1);
        }

        ok = true;
    }

    if (!ok)
        printCmdError(form("unknown debugger command [[%s]]", arg.split[1].c_str()));

    return BcDbgCommandResult::Continue;
}
