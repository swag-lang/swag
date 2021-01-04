#include "pch.h"
#include "Stats.h"
#include "Workspace.h"
#include "Version.h"
#include "Os.h"

Stats g_Stats;

void Stats::print()
{
    if (!g_CommandLine.stats || g_CommandLine.silent)
        return;

    g_Log.setColor(LogColor::DarkCyan);
    g_Log.print("\n");

    g_Log.messageHeaderDot("swag version", format("%d.%d.%d", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    g_Log.messageHeaderDot("workers", format("%d", numWorkers));
    g_Log.print("\n");

    g_Log.messageHeaderDot("modules", format("%d", numModules.load()));
    g_Log.messageHeaderDot("files", format("%d", numFiles.load()));
    g_Log.messageHeaderDot("source lines", format("%d", numLines.load()));
    g_Log.messageHeaderDot("lines/s", format("%d", (int) (numLines.load() / OS::timerToSeconds(totalTime.load()))));
    g_Log.messageHeaderDot("open files", format("%d", maxOpenFiles.load()));
    if (g_CommandLine.output)
        g_Log.messageHeaderDot("output modules", format("%d", numGenModules.load()));
    if (g_CommandLine.test)
        g_Log.messageHeaderDot("executed #test", format("%d", testFunctions.load()));
    g_Log.messageHeaderDot("executed #run", format("%d", runFunctions.load()));
    if (g_Workspace.numErrors)
        g_Log.messageHeaderDot("errors", format("%d", g_Workspace.numErrors.load()), LogColor::Red);
    g_Log.print("\n");

    g_Log.messageHeaderDot("instructions", format("%d", numInstructions.load()));
    float pc = (totalOptimsBC.load() * 100.0f) / (numInstructions.load());
    g_Log.messageHeaderDot("kicked", format("%d %.1f%%", totalOptimsBC.load(), pc));
    g_Log.messageHeaderDot("concrete types", format("%d", totalConcreteTypes.load()));
    g_Log.print("\n");

    g_Log.messageHeaderDot("syntax time", format("%.3fs", OS::timerToSeconds(syntaxTime.load())));
    g_Log.messageHeaderDot("read files", format("%.3fs", OS::timerToSeconds(readFilesTime.load())));
    g_Log.messageHeaderDot("semantic time", format("%.3fs", OS::timerToSeconds(semanticModuleTime.load())));
    g_Log.messageHeaderDot("run time", format("%.3fs", OS::timerToSeconds(runTime.load())));
    g_Log.messageHeaderDot("output time", format("%.3fs", OS::timerToSeconds(outputTime.load())));
    g_Log.messageHeaderDot("prep out time", format("%.3fs %.3fs", OS::timerToSeconds(prepOutputTimePass.load()), OS::timerToSeconds(prepOutputTimeJob.load())));
    g_Log.messageHeaderDot("gen out time", format("%.3fs %.3fs", OS::timerToSeconds(genOutputTimePass.load()), OS::timerToSeconds(genOutputTimeJob.load())));
    g_Log.messageHeaderDot("run test time", format("%.3fs", OS::timerToSeconds(runTestTime.load())));
    g_Log.messageHeaderDot("optim bc time", format("%.3fs", OS::timerToSeconds(optimBCTime.load())));
    g_Log.print("\n");

    g_Log.messageHeaderDot("allocator memory", format("%s", toNiceSize(allocatorMemory.load()).c_str()));
    g_Log.messageHeaderDot("mem wasted", format("%s", toNiceSize(wastedMemory.load()).c_str()));
    g_Log.messageHeaderDot("mem nodes", format("%s", toNiceSize(memNodes.load()).c_str()));
    g_Log.messageHeaderDot("mem scopes", format("%s", toNiceSize(memScopes.load()).c_str()));
    g_Log.messageHeaderDot("mem seg", format("%s", toNiceSize(memSeg.load()).c_str()));
    g_Log.messageHeaderDot("mem concat", format("%s", toNiceSize(memConcat.load()).c_str()));
    g_Log.messageHeaderDot("mem types", format("%s", toNiceSize(memTypes.load()).c_str()));
    g_Log.messageHeaderDot("mem instr", format("%s", toNiceSize(memInstructions.load()).c_str()));
    g_Log.messageHeaderDot("mem bcstack", format("%s", toNiceSize(memBcStack.load()).c_str()));
    g_Log.messageHeaderDot("mem symname", format("%s", toNiceSize(memSymName.load()).c_str()));
    g_Log.messageHeaderDot("mem symover", format("%s", toNiceSize(memSymOver.load()).c_str()));
    g_Log.messageHeaderDot("mem symtable", format("%s", toNiceSize(memSymTable.load()).c_str()));
    g_Log.messageHeaderDot("mem utf8", format("%s", toNiceSize(memUtf8.load()).c_str()));

    g_Log.setDefaultColor();
}
