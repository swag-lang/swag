#include "pch.h"
#include "CommandLineParser.h"
#include "CommandLine.h"
#include "ErrorIds.h"
#include "Log.h"
#include "Report.h"

CommandLineArgument::CommandLineArgument(const char* commands, CommandLineType type, void* buffer, const char* param, const char* help) :
    buffer{buffer},
    param{param},
    help{help},
    type{type}
{
    Vector<Utf8> all;
    Utf8::tokenize(commands, ' ', all);

    for (auto& p : all)
    {
        if (p == "all" || p == "bu")
            cmdSet.push_back("build");

        if (p == "all" || p == "bu" || p == "ru")
            cmdSet.push_back("run");

        if (p == "all" || p == "sc")
            cmdSet.push_back("script");

        if (p == "all" || p == "bu" || p == "te")
            cmdSet.push_back("test");

        if (p == "all" || p == "cl")
            cmdSet.push_back("clean");

        if (p == "all" || p == "ne")
            cmdSet.push_back("new");

        if (p == "all" || p == "li")
            cmdSet.push_back("list");

        if (p == "all" || p == "ge")
            cmdSet.push_back("get");

        if (p == "all" || p == "doc")
            cmdSet.push_back("doc");

        if (p == "all" || p == "fmt")
            cmdSet.push_back("format");
    }
}

void CommandLineParser::setup(CommandLine* cmdLine)
{
    addArg("all", "--silent", "-s", CommandLineType::Bool, &cmdLine->silent, nullptr, "do not log messages");
    addArg("all", "--log-colors", nullptr, CommandLineType::Bool, &cmdLine->logColors, nullptr, "output to console can be colored");
    addArg("all", "--log-ascii", nullptr, CommandLineType::Bool, &cmdLine->logAscii, nullptr, "output to console only use ascii characters (no unicode)");
    addArg("all", "--ignore-bad-params", nullptr, CommandLineType::Bool, &cmdLine->ignoreBadParams, nullptr, "ignore unknown command line parameters instead of raising an error");

    addArg("all", "--verbose-cmdline", nullptr, CommandLineType::Bool, &cmdLine->verboseCmdLine, nullptr, "log swag command line");
    addArg("bu sc doc", "--verbose-path", nullptr, CommandLineType::Bool, &cmdLine->verbosePath, nullptr, "log global paths");
    addArg("bu sc", "--verbose-link", nullptr, CommandLineType::Bool, &cmdLine->verboseLink, nullptr, "log linker command line");
    addArg("bu sc", "--verbose-ctypes", nullptr, CommandLineType::Bool, &cmdLine->verboseConcreteTypes, nullptr, "log generated concrete types");
    addArg("bu sc te doc fmt", "--verbose-stages", nullptr, CommandLineType::Bool, &cmdLine->verboseStages, nullptr, "log compiler stages");

    addArg("bu sc doc fmt", "--error-one-line", "-el", CommandLineType::Bool, &cmdLine->errorOneLine, nullptr, "display errors in a single line");
    addArg("bu sc doc fmt", "--error-absolute", "-ea", CommandLineType::Bool, &cmdLine->errorAbsolute, nullptr, "display absolute paths when an error is raised");
    addArg("bu sc doc fmt", "--error-syntax-color", "-es", CommandLineType::Bool, &cmdLine->errorSyntaxColor, nullptr, "syntax color code when an error is raised");
    addArg("bu sc doc fmt", "--error-syntax-color-lum", nullptr, CommandLineType::Bool, &cmdLine->errorSyntaxColorLum, nullptr, "syntax color luminosity factor [0..1]");

    addArg("bu ne cl li ge doc", "--workspace", "-w", CommandLineType::StringPath, &cmdLine->workspacePath, nullptr, "the path to the workspace to work with");
    addArg("bu ne doc", "--module", "-m", CommandLineType::String, &cmdLine->moduleName, nullptr, "module name");
    addArg("ne sc fmt", "--file", "-f", CommandLineType::String, &cmdLine->fileName, nullptr, "the script file name|format:the source file to reformat");
    addArg("ne", "--test", nullptr, CommandLineType::Bool, &cmdLine->test, nullptr, "create a test module");

    addArg("all", "--cache", "-t", CommandLineType::StringPath, &cmdLine->cachePath, nullptr, "specify the cache folder (system specific if empty)");
    addArg("all", "--num-cores", nullptr, CommandLineType::Int, &cmdLine->numCores, nullptr, "maximum number of cpu to use (0 = automatic)");

    addArg("bu fmt", "--output", "-o", CommandLineType::Bool, &cmdLine->output, nullptr, "output backend|format:output the reformatted files");
    addArg("bu", "--output-legit", "-ol", CommandLineType::Bool, &cmdLine->outputLegit, nullptr, "output the legit backend");
    addArg("bu", "--output-test", "-ot", CommandLineType::Bool, &cmdLine->outputTest, nullptr, "output the test backend");
    addArg("doc", "--output-doc", "-od", CommandLineType::Bool, &cmdLine->outputDoc, nullptr, "output the generated documentation");

    addArg("te", "--test-bytecode", "-tb", CommandLineType::Bool, &cmdLine->runByteCodeTests, nullptr, "run #test functions as bytecode");
    addArg("te", "--test-native", "-tn", CommandLineType::Bool, &cmdLine->runBackendTests, nullptr, "run #test functions as native");
    addArg("te", "--test-filter", "-tf", CommandLineType::String, &cmdLine->testFilter, nullptr, "will only compile and test files that match the filter");
    addArg("te", "--test-verbose", "-ve", CommandLineType::Bool, &cmdLine->verboseTestErrors, nullptr, "log errors during test");
    addArg("te", "--test-verbose-filter", "-vef", CommandLineType::String, &cmdLine->verboseErrorFilter, nullptr, "filter log errors during test");

    addArg("bu sc doc", "--rebuild", nullptr, CommandLineType::Bool, &cmdLine->rebuild, nullptr, "full rebuild");
    addArg("bu sc doc", "--rebuild-all", nullptr, CommandLineType::Bool, &cmdLine->rebuildAll, nullptr, "full rebuild (with all dependencies)");
    addArg("ge doc", "--force", nullptr, CommandLineType::Bool, &cmdLine->getDepForce, nullptr, "force to flush dependencies");

    addArg("bu sc", "--dbg-catch", nullptr, CommandLineType::Bool, &cmdLine->dbgCatch, nullptr, "open bytecode debugger (bcdbg) in case of compile time errors");
    addArg("bu sc", "--dbg-main", nullptr, CommandLineType::Bool, &cmdLine->dbgMain, nullptr, "open bytecode debugger (bcdbg) at the start of #main");
    addArg("bu sc", "--dbg-off", nullptr, CommandLineType::Bool, &cmdLine->dbgOff, nullptr, "disable @breakpoint() instruction");
    addArg("bu sc doc", "--callstack", nullptr, CommandLineType::Bool, &cmdLine->dbgCallStack, nullptr, "display callstacks in case of errors");

#ifdef SWAG_DEV_MODE
    addArg("bu sc doc", "--print-bc-ext", nullptr, CommandLineType::Bool, &cmdLine->dbgPrintBcExt, nullptr, "print more bytecode information");
    addArg("bu sc doc", "--randomize", nullptr, CommandLineType::Bool, &cmdLine->randomize, nullptr, "[devmode] randomize behavior");
    addArg("bu sc doc", "--seed", nullptr, CommandLineType::Int, &cmdLine->randSeed, nullptr, "[devmode] set seed for randomize behavior");
#else
    addArg("bu sc doc", "--devmode", nullptr, CommandLineType::Bool, &cmdLine->dbgDevMode, nullptr, "message box in case of exception");
#endif

    addArg("bu cl sc", "--cfg", nullptr, CommandLineType::String, &cmdLine->buildCfg, nullptr, "set the build configuration (debug|fast-debug|fast-compile|release are predefined)");
    addArg("bu cl sc", "--cfg-debug", nullptr, CommandLineType::EnumString, &cmdLine->buildCfgDebug, "true|false|default", "generate debug information");
    addArg("bu cl sc", "--cfg-safety", nullptr, CommandLineType::EnumString, &cmdLine->buildCfgSafety, "true|false|default", "generate safety guards");
    addArg("bu cl sc", "--cfg-inline-bc", nullptr, CommandLineType::EnumString, &cmdLine->buildCfgInlineBC, "true|false|default", "inline marked functions");
    addArg("bu cl sc", "--cfg-optim-bc", nullptr, CommandLineType::EnumString, &cmdLine->buildCfgOptimBC, "0|1|2|default", "bytecode optimization level");
    addArg("bu cl sc", "--cfg-optim-backend", nullptr, CommandLineType::EnumString, &cmdLine->buildCfgOptim, "O1|O2|O3|Os|Oz|default", "optimize output for speed and/or size");
    addArg("bu cl sc", "--cfg-err-stack-trace", nullptr, CommandLineType::EnumString, &cmdLine->buildCfgStackTrace, "true|false|default", "generate call trace for errors");
    addArg("bu cl sc", "--cfg-debug-alloc", nullptr, CommandLineType::EnumString, &cmdLine->buildCfgDebugAlloc, "true|false|default", "use the debug allocator");
    addArg("bu cl sc", "--cfg-llvm-ir", nullptr, CommandLineType::EnumString, &cmdLine->buildCfgLlvmIR, "true|false|default", "generate .ir files (llvm backend)");

    addArg("bu cl sc", "--os", nullptr, CommandLineType::EnumInt, &cmdLine->target.os, "windows", "set the target operating system");
    addArg("bu cl sc", "--arch", nullptr, CommandLineType::EnumInt, &cmdLine->target.arch, "x86_64", "set the target architecture");
    addArg("bu cl sc", "--cpu", nullptr, CommandLineType::String, &cmdLine->target.cpu, nullptr, "set the target micro architecture");

    addArg("bu sc", "--tag", nullptr, CommandLineType::StringSet, &cmdLine->tags, nullptr, "add a build tag, with an optional associated type and value");
    addArg("bu sc", "--args", nullptr, CommandLineType::String, &cmdLine->userArguments, nullptr, "pass some specific arguments to the user code");

    addArg("bu sc", "--limit-recurse-bc", nullptr, CommandLineType::Int, &cmdLine->limitRecurseBC, nullptr, "maximum recursion level in bytecode (0 for no limit)");
    addArg("bu sc", "--limit-recurse-inline", nullptr, CommandLineType::Int, &cmdLine->limitInlineLevel, nullptr, "maximum code inline level (0 for no limit)");
    addArg("bu sc", "--limit-stack-bc", nullptr, CommandLineType::Int, &cmdLine->limitStackBC, nullptr, "set the stack size for bytecode");
    addArg("bu sc", "--limit-stack-rt", nullptr, CommandLineType::Int, &cmdLine->limitStackRT, nullptr, "set the stack size for backend");

    addArg("bu sc", "--backend", nullptr, CommandLineType::EnumInt, &cmdLine->backendGenType, "llvm|scbe", "the type of backend to use");
    addArg("ru", "--bytecode", nullptr, CommandLineType::Bool, &cmdLine->scriptMode, nullptr, "run in bytecode mode");
    addArg("cl", "--script", nullptr, CommandLineType::Bool, &cmdLine->scriptMode, nullptr, "clean also script cache");
    addArg("sc", "--script-run", nullptr, CommandLineType::Bool, &cmdLine->scriptRun, nullptr, "run script, or just compile if false");

#ifdef SWAG_STATS
    addArg("all", "--stats", nullptr, CommandLineType::Bool, &cmdLine->stats, nullptr, "[stats] display statistics at the end");
    addArg("all", "--stats-freq", nullptr, CommandLineType::Bool, &cmdLine->statsFreq, nullptr, "[stats] display statistics about bytecode op usage");
    addArg("all", "--stats-freq-op0", nullptr, CommandLineType::String, &cmdLine->statsFreqOp0, nullptr, "[stats] name of the bytecode op to display statistics about");
    addArg("all", "--stats-freq-op1", nullptr, CommandLineType::String, &cmdLine->statsFreqOp1, nullptr, "[stats] name of the bytecode op to display statistics about");
    addArg("bu sc ru te", "--profile", nullptr, CommandLineType::Bool, &cmdLine->profile, nullptr, "[stats] profile bytecode execution");
    addArg("bu sc ru te", "--profile-filter", nullptr, CommandLineType::String, &cmdLine->profileFilter, nullptr, "[stats] filter profile output names");
    addArg("bu sc ru te", "--profile-children", nullptr, CommandLineType::Int, &cmdLine->profileChildsLevel, nullptr, "[stats] display to the a given amount of sub-functions level");
#endif

    addArg("cl sc", "--clean-dep", nullptr, CommandLineType::Bool, &cmdLine->cleanDep, nullptr, "removes the content of the dependency folder");
    addArg("cl", "--clean-log", nullptr, CommandLineType::Bool, &cmdLine->cleanLog, nullptr, "display what will be removed, without actually cleaning");

    addArg("doc", "--css", nullptr, CommandLineType::String, &cmdLine->docCss, nullptr, "set the css file path to include in documentations");
    addArg("doc", "--ext", nullptr, CommandLineType::String, &cmdLine->docExtension, nullptr, "force the output extension of all documentations");
}

namespace
{
    void getArgValue(const CommandLineArgument* oneArg, Utf8& value, Utf8& defaultValue)
    {
        switch (oneArg->type)
        {
            case CommandLineType::Bool:
                value = "true|false";
                if (*static_cast<bool*>(oneArg->buffer))
                    defaultValue += "true";
                else
                    defaultValue += "false";
                break;
            case CommandLineType::Int:
                value        = "<integer>";
                defaultValue = std::to_string(*static_cast<int*>(oneArg->buffer));
                break;
            case CommandLineType::String:
                value        = "<string>";
                defaultValue = *static_cast<Utf8*>(oneArg->buffer);
                break;
            case CommandLineType::StringPath:
                value        = "<path>";
                defaultValue = *static_cast<Path*>(oneArg->buffer);
                break;
            case CommandLineType::StringSet:
                value = "<string>";
                break;
            case CommandLineType::EnumInt:
            {
                value = oneArg->param;
                Vector<Utf8> tokens;
                Utf8::tokenize(oneArg->param, '|', tokens);
                defaultValue = tokens[*static_cast<int*>(oneArg->buffer)];
                break;
            }
            case CommandLineType::EnumString:
                value        = oneArg->param;
                defaultValue = *static_cast<Utf8*>(oneArg->buffer);
                break;
        }
    }
}

void CommandLineParser::logArguments(const Utf8& cmd) const
{
    Utf8   line0, line1;
    size_t columns[10] = {4};

    static constexpr size_t SPACE = 4;

    columns[0] = SPACE + strlen("Argument");
    columns[1] = SPACE + strlen("Short");
    columns[2] = SPACE + strlen("Value");
    columns[3] = SPACE + strlen("Default");
    columns[4] = SPACE + strlen("Help");

    for (const auto& arg : longNameArgs)
    {
        auto oneArg = arg.second;
        if (!isArgValidFor(cmd, oneArg))
            continue;

        columns[0] = max(columns[0], arg.first.length() + SPACE);
        columns[1] = max(columns[1], oneArg->shortName.length() + SPACE);

        Utf8 value, defaultVal;
        getArgValue(oneArg, value, defaultVal);
        columns[2] = max(columns[2], value.length() + SPACE);
        columns[3] = max(columns[3], defaultVal.length() + SPACE);
    }

    size_t total = 0;

    line0 = "Argument";
    line1 = "--------";
    total += columns[0];
    while (line0.length() < total)
        line0 += " ", line1 += " ";

    line0 += "Short";
    line1 += "-----";
    total += columns[1];
    while (line0.length() < total)
        line0 += " ", line1 += " ";

    line0 += "Value";
    line1 += "-----";
    total += columns[2];
    while (line0.length() < total)
        line0 += " ", line1 += " ";

    line0 += "Default";
    line1 += "-------";
    total += columns[3];
    while (line0.length() < total)
        line0 += " ", line1 += " ";

    line0 += "Help";
    line1 += "----";

    line0 += "\n";
    line1 += "\n";
    g_Log.messageInfo(line0);
    g_Log.messageInfo(line1);

    for (const auto& arg : longNameArgs)
    {
        auto oneArg = arg.second;
        if (!isArgValidFor(cmd, oneArg))
            continue;

        total = 0;
        line0 = arg.first;

        total += columns[0];
        while (line0.length() < total)
            line0 += " ";
        line0 += oneArg->shortName;

        Utf8 value, defaultVal;
        getArgValue(oneArg, value, defaultVal);

        total += columns[1];
        while (line0.length() < total)
            line0 += " ";
        line0 += value;

        total += columns[2];
        while (line0.length() < total)
            line0 += " ";
        line0 += defaultVal;

        if (oneArg->help)
        {
            total += columns[3];
            while (line0.length() < total)
                line0 += " ";

            Vector<Utf8> tokensHelp;
            Utf8::tokenize(oneArg->help, '|', tokensHelp);
            if (tokensHelp.size() == 1)
                line0 += tokensHelp[0];
            else
            {
                bool done = false;
                for (const auto& v : tokensHelp)
                {
                    if (v.find(cmd) == 0 && v[cmd.length()] == ':')
                    {
                        line0 += v.c_str() + cmd.length() + 1;
                        done = true;
                        break;
                    }
                }
                if (!done)
                    line0 += tokensHelp[0];
            }
        }

        g_Log.messageInfo(line0);
    }
}

void CommandLineParser::addArg(const char* commands, const char* longName, const char* shortName, CommandLineType type, void* address, const char* param, const char* help)
{
    const auto arg = new CommandLineArgument{commands, type, address, param, help};

    if (longName)
        arg->longName = longName;
    if (shortName)
        arg->shortName = shortName;

    if (longName)
        longNameArgs[longName] = arg;
    if (shortName)
        shortNameArgs[shortName] = arg;
}

bool CommandLineParser::isArgValidFor(const Utf8& swagCmd, const CommandLineArgument* arg)
{
    return arg->cmdSet.contains(swagCmd);
}

bool CommandLineParser::process(const Utf8& swagCmd, int argc, const char* argv[])
{
    bool result = true;
    for (int i = 0; i < argc; i++)
    {
        // Split in half, with the ':' delimiter
        Utf8        command;
        Utf8        argument;
        const char* pz = argv[i];
        while (*pz && *pz != ':')
            command += *pz++;
        if (*pz)
            pz++;
        while (*pz)
            argument += *pz++;

        // Find command
        auto it = longNameArgs.find(command);
        if (it == longNameArgs.end())
        {
            it = shortNameArgs.find(command);
            if (it == shortNameArgs.end())
            {
                if (!g_CommandLine.ignoreBadParams)
                {
                    Report::error(formErr(Fat0007, command.c_str()));
                    result = false;
                }
                continue;
            }
        }

        const auto arg = it->second;

        // Be sure the argument is valid for the swag command
        if (!isArgValidFor(swagCmd, arg))
        {
            if (!g_CommandLine.ignoreBadParams)
            {
                Report::error(formErr(Fat0007, command.c_str()));
                result = false;
            }

            continue;
        }

        switch (arg->type)
        {
            case CommandLineType::EnumInt:
            {
                Vector<Utf8> tokens;
                Utf8::tokenize(arg->param, '|', tokens);

                int index = 0;
                for (const auto& one : tokens)
                {
                    if (one == argument)
                    {
                        *static_cast<int*>(arg->buffer) = index;
                        break;
                    }

                    index++;
                }

                if (index == static_cast<int>(tokens.size()))
                {
                    Report::error(formErr(Fat0005, it->first.c_str(), arg->param));
                    result = false;
                    continue;
                }
            }
            break;

            case CommandLineType::EnumString:
            {
                Vector<Utf8> tokens;
                Utf8::tokenize(arg->param, '|', tokens);

                int index = 0;
                for (const auto& one : tokens)
                {
                    if (one == argument)
                    {
                        *static_cast<Utf8*>(arg->buffer) = one;
                        break;
                    }

                    index++;
                }

                if (index == static_cast<int>(tokens.size()))
                {
                    Report::error(formErr(Fat0005, it->first.c_str(), arg->param));
                    result = false;
                    continue;
                }
            }
            break;

            case CommandLineType::Bool:
                if (argument == "true" || argument.empty())
                    *static_cast<bool*>(arg->buffer) = true;
                else if (argument == "false")
                    *static_cast<bool*>(arg->buffer) = false;
                else
                {
                    Report::error(formErr(Fat0001, it->first.c_str(), argument.c_str()));
                    result = false;
                    continue;
                }
                break;

            case CommandLineType::String:
            {
                if (argument.empty())
                {
                    Report::error(formErr(Fat0002, it->first.c_str(), argument.c_str()));
                    result = false;
                    continue;
                }

                *static_cast<Utf8*>(arg->buffer) = argument;
                break;
            }

            case CommandLineType::StringPath:
            {
                if (argument.empty())
                {
                    Report::error(formErr(Fat0002, it->first.c_str(), argument.c_str()));
                    result = false;
                    continue;
                }

                *static_cast<Path*>(arg->buffer) = argument;
                break;
            }

            case CommandLineType::StringSet:
            {
                if (argument.empty())
                {
                    Report::error(formErr(Fat0002, it->first.c_str(), argument.c_str()));
                    result = false;
                    continue;
                }

                static_cast<SetUtf8*>(arg->buffer)->insert(argument);
                break;
            }

            case CommandLineType::Int:
            {
                pz               = argument.c_str();
                bool thisIsAnInt = !argument.empty();

                while (*pz)
                {
                    if (!isdigit(*pz))
                    {
                        thisIsAnInt = false;
                        break;
                    }

                    pz++;
                }

                if (!thisIsAnInt)
                {
                    if (argument.empty())
                        Report::error(formErr(Fat0004, it->first.c_str()));
                    else
                        Report::error(formErr(Fat0003, it->first.c_str(), argument.c_str()));
                    result = false;
                    continue;
                }

                *static_cast<int*>(arg->buffer) = argument.toInt();
                break;
            }
        }
    }

    return result;
}

Utf8 CommandLineParser::buildString() const
{
    CommandLine       defaultValues;
    CommandLineParser defaultParser;
    defaultParser.setup(&defaultValues);

    Vector<CommandLineArgument*> list;
    for (const auto& oneArg : longNameArgs | std::views::values)
        list.push_back(oneArg);

    std::ranges::sort(list, [](const CommandLineArgument* a, const CommandLineArgument* b) { return strcmp(a->longName, b->longName) < 0; });

    Utf8 result;
    for (const auto& oneArg : list)
    {
        result += Log::colorToVTS(LogColor::Header);
        switch (oneArg->type)
        {
            case CommandLineType::String:
                result += oneArg->longName + ":";
                result += Log::colorToVTS(LogColor::Value);
                result += *static_cast<Utf8*>(oneArg->buffer);
                break;

            case CommandLineType::StringPath:
                result += oneArg->longName + ":";
                result += Log::colorToVTS(LogColor::Value);
                result += *static_cast<Path*>(oneArg->buffer);
                break;

            case CommandLineType::EnumInt:
            {
                result += oneArg->longName + ":";
                result += Log::colorToVTS(LogColor::Value);

                Vector<Utf8> tokens;
                Utf8::tokenize(oneArg->param, '|', tokens);
                const int idx = *static_cast<int*>(oneArg->buffer);
                result += tokens[idx];
                break;
            }

            case CommandLineType::StringSet:
            {
                result += oneArg->longName + ":";
                result += Log::colorToVTS(LogColor::Value);
                const auto all = static_cast<SetUtf8*>(oneArg->buffer);
                for (auto& one : *all)
                {
                    result += one;
                    result += " ";
                }
                break;
            }

            case CommandLineType::EnumString:
                result += oneArg->longName + ":";
                result += Log::colorToVTS(LogColor::Value);
                result += *static_cast<Utf8*>(oneArg->buffer);
                break;

            case CommandLineType::Int:
                result += oneArg->longName + ":";
                result += Log::colorToVTS(LogColor::Value);
                result += std::to_string(*static_cast<int*>(oneArg->buffer));
                break;

            case CommandLineType::Bool:
            {
                result += oneArg->longName + ":";
                result += Log::colorToVTS(LogColor::Value);
                if (*static_cast<bool*>(oneArg->buffer) == true)
                    result += "true";
                else
                    result += "false";
                break;
            }
        }

        result += "\n";
    }

    return result;
}
