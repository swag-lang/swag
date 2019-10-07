#include "pch.h"
#include "ffi.h"
#include "Stats.h"
#include "Log.h"
#include "Workspace.h"
#include "CommandLineParser.h"
#include "Version.h"

void printStats()
{
    if (!g_CommandLine.stats || g_CommandLine.silent)
        return;

    g_Log.setColor(LogColor::White);
    wcout << "swag version ...... " << format("%d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM).c_str();
	wcout << "frontend time ..... " << g_Stats.frontendTime.count() << "s\n";
    if (g_CommandLine.backendOutput)
        wcout << "backend time ...... " << g_Stats.backendTime.count() << "s\n";
    wcout << "total time ........ " << g_Stats.totalTime.count() << "s\n";
    wcout << "workers ........... " << g_Stats.numWorkers << "\n";
    wcout << "modules ........... " << g_Stats.numModules << "\n";
    wcout << "files ............. " << g_Stats.numFiles << "\n";
    wcout << "lines ............. " << g_Stats.numLines << "\n";
    wcout << "lines/s ........... " << (int) (g_Stats.numLines / g_Stats.totalTime.count()) << "\n";
    wcout << "instructions ...... " << g_Stats.numInstructions << "\n";
    if (g_CommandLine.backendOutput)
        wcout << "output modules .... " << g_Stats.numGenModules << "\n";
    if (g_CommandLine.unittest)
        wcout << "test functions .... " << g_Stats.testFunctions << "\n";
    if (g_Workspace.numErrors)
    {
        g_Log.setColor(LogColor::Red);
        wcout << "errors ............ " << g_Workspace.numErrors << "\n";
    }

    g_Log.setDefaultColor();
}

int main(int argc, const char* argv[])
{
    auto timeBefore = chrono::high_resolution_clock::now();

    // We do not want assert, but just reports of the CRT
#ifdef WIN32
    if (!IsDebuggerPresent())
    {
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    }
#endif

    g_Log.setup();

    CommandLineParser cmdParser;
    g_CommandLine.exePath = fs::absolute(argv[0]);
    cmdParser.setup(&g_CommandLine);
    if (!cmdParser.process(argc, argv))
        return -2;

    // Log all arguments
    if (g_CommandLine.help)
    {
        wcout << "swag version " << format("%d.%d.%d\n", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM).c_str();
        cmdParser.logArguments();
        exit(0);
    }

    g_Global.setup();
    g_Workspace.build();

    // Prints stats, then exit
    auto timeAfter    = chrono::high_resolution_clock::now();
    g_Stats.totalTime = timeAfter - timeBefore;
    printStats();

    return g_Workspace.numErrors > 0 ? -1 : 0;
}
