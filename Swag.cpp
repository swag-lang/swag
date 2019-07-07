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
	wcout << "swag version ...... " << format("%d.%d.%d\n", SWAG_VERSION, SWAG_REVISION, SWAG_BUILD).c_str();
    if (g_CommandLine.output)
        wcout << "output time ....... " << g_Stats.outputTime.count() << "s\n";
    wcout << "total time ........ " << g_Stats.totalTime.count() << "s\n";
    wcout << "workers ........... " << g_Stats.numWorkers << "\n";
    wcout << "lines ............. " << g_Stats.numLines << "\n";
	wcout << "lines/s ........... " << (int) (g_Stats.numLines / g_Stats.totalTime.count()) << "\n";
    wcout << "files ............. " << g_Stats.numFiles << "\n";
    wcout << "modules ........... " << g_Stats.numModules << "\n";
    if (g_CommandLine.output)
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

    g_Log.setup();

    CommandLineParser cmdParser;
    g_CommandLine.exePath = argv[0];
    cmdParser.setup(&g_CommandLine);
    if (!cmdParser.process(argc, argv))
        return -2;

    g_Global.setup();
    g_Workspace.build();

    // Prints stats, then exit
    auto timeAfter    = chrono::high_resolution_clock::now();
    g_Stats.totalTime = timeAfter - timeBefore;
    printStats();

    return g_Workspace.numErrors > 0 ? -1 : 0;
}
