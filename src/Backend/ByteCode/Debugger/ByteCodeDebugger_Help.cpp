#include "pch.h"
#include "Backend/ByteCode/Debugger/ByteCodeDebugger.h"
#include "Report/Log.h"

void ByteCodeDebugger::printHelp(const BcDbgCommand& cmd, bool commandMode)
{
    g_Log.setColor(LogColor::Gray);

    Utf8 line;
    Utf8 lineRaw;

    line += Log::colorToVTS(LogColor::Name);

    // Short name
    line += cmd.shortname;
    lineRaw += cmd.shortname;
    while (lineRaw.length() < 3)
    {
        line += " ";
        lineRaw += " ";
    }

    // Long name
    line += cmd.name;
    lineRaw += cmd.name;
    line += " ";
    lineRaw += " ";

    // Sub command
    uint32_t colArgAdd = 0;
    if (!commandMode && cmd.subcommand[0])
    {
        line += cmd.subcommand;
        lineRaw += cmd.subcommand;
    }

    if (!commandMode)
        colArgAdd = 10;

    // Arguments
    line += Log::colorToVTS(LogColor::Type);
    while (lineRaw.length() < 14 + colArgAdd)
    {
        line += " ";
        lineRaw += " ";
    }

    if (commandMode && cmd.subcommand[0])
    {
        line += cmd.subcommand;
        lineRaw += cmd.subcommand;
        line += " ";
        lineRaw += " ";
    }
    else
    {
        line += cmd.args;
        lineRaw += cmd.args;
        line += " ";
        lineRaw += " ";
    }

    // Help
    line += Log::colorToVTS(LogColor::White);
    while (lineRaw.length() < 44 + colArgAdd)
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
    Utf8 lastName;
    Utf8 lastCateg;

    for (auto& c : commands)
    {
        if (lastName == c.name)
            continue;
        lastName = c.name;

        if (lastCateg != c.category)
        {
            g_Log.writeEol();
            g_Log.print(c.category, LogColor::Gray);
            g_Log.writeEol();
            g_Log.writeEol();
            lastCateg = c.category;
        }

        printHelp(c, true);
    }
}

BcDbgCommandResult ByteCodeDebugger::cmdHelp(ByteCodeRunContext*, const BcDbgCommandArg& arg)
{
    if (arg.help)
        return BcDbgCommandResult::Continue;

    if (arg.split.size() > 3)
        return BcDbgCommandResult::TooManyArguments;

    // List all commands
    if (arg.split.size() == 1)
    {
        g_ByteCodeDebugger.printHelp();
        return BcDbgCommandResult::Continue;
    }

    // Help on a specific command
    Vector<BcDbgCommand> toPrint;
    for (const auto& c : g_ByteCodeDebugger.commands)
    {
        if (!c.name || !c.cb)
            continue;
        if (arg.split[1] != c.name && (!c.shortname || arg.split[1] != c.shortname))
            continue;
        toPrint.push_back(c);
    }

    if (toPrint.empty())
    {
        printCmdError(form("unknown debugger command [[%s]]", arg.split[1].c_str()));
        return BcDbgCommandResult::Error;
    }

    // Sub command
    if (arg.split.size() == 3)
    {
        const auto toScan = toPrint;
        toPrint.clear();

        for (const auto& cmd : toScan)
        {
            if (!cmd.args[0])
                continue;

            Vector<Utf8> subCmds;
            Vector<Utf8> subCmds1;
            Utf8::tokenizeBlanks(cmd.args, subCmds);
            Utf8::tokenize(subCmds[0], '|', subCmds1);
            for (const auto& c : subCmds1)
            {
                if (arg.split[2] == c)
                {
                    toPrint.push_back(cmd);
                    break;
                }
            }
        }
    }

    // One single command to display
    if (toPrint.size() == 1)
    {
        const auto& c = toPrint[0];

        g_Log.setColor(LogColor::Gray);
        g_Log.print(form("command:     %s%s %s%s\n", Log::colorToVTS(LogColor::Name).c_str(), c.name, Log::colorToVTS(LogColor::Type).c_str(), c.args));
        if (c.shortname[0])
        {
            g_Log.setColor(LogColor::Gray);
            g_Log.print(form("short name:  %s%s\n", Log::colorToVTS(LogColor::Name).c_str(), c.shortname));
        }
        g_Log.setColor(LogColor::Gray);
        g_Log.print(form("description: %s%s\n", Log::colorToVTS(LogColor::White).c_str(), c.help));
        g_Log.setColor(LogColor::Gray);

        if (c.cb)
        {
            BcDbgCommandArg arg1 = arg;
            arg1.split.erase(arg1.split.begin());
            arg1.help = true;
            c.cb(nullptr, arg1);
        }

        return BcDbgCommandResult::Continue;
    }

    // Print all commands
    for (const auto& cmd : toPrint)
        printHelp(cmd, false);

    return BcDbgCommandResult::Continue;
}
