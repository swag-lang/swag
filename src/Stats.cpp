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

    g_Log.messageHeaderDot("swag version", format("%u.%u.%u", SWAG_BUILD_VERSION, SWAG_BUILD_REVISION, SWAG_BUILD_NUM));
    g_Log.messageHeaderDot("workers", format("%u", numWorkers));
    g_Log.print("\n");

    g_Log.messageHeaderDot("modules", format("%u", numModules.load()));
    g_Log.messageHeaderDot("files", format("%u", numFiles.load()));
    g_Log.messageHeaderDot("source lines", format("%u", numLines.load()));
    g_Log.messageHeaderDot("lines/s", format("%u", (int) (numLines.load() / OS::timerToSeconds(totalTime.load()))));
    g_Log.messageHeaderDot("open files", format("%u", maxOpenFiles.load()));
    if (g_CommandLine.output)
        g_Log.messageHeaderDot("output modules", format("%u", numGenModules.load()));
    if (g_CommandLine.test)
        g_Log.messageHeaderDot("executed #test", format("%u", testFunctions.load()));
    g_Log.messageHeaderDot("executed #run", format("%u", runFunctions.load()));
    if (g_Workspace.numErrors)
        g_Log.messageHeaderDot("errors", format("%u", g_Workspace.numErrors.load()), LogColor::Red);
    g_Log.print("\n");

    g_Log.messageHeaderDot("instructions", format("%u", numInstructions.load()));
    float pc = (totalOptimsBC.load() * 100.0f) / (numInstructions.load());
    g_Log.messageHeaderDot("kicked", format("%d %.1f%%", totalOptimsBC.load(), pc));
    g_Log.messageHeaderDot("total", format("%u", numInstructions.load() - totalOptimsBC.load()));
    g_Log.messageHeaderDot("concrete types", format("%u", totalConcreteTypes.load()));
    g_Log.messageHeaderDot("init ptr", format("%u", numInitPtr.load()));
    g_Log.print("\n");

    g_Log.messageHeaderDot("cfg time", format("%.3fs", OS::timerToSeconds(cfgTime.load())));
    g_Log.messageHeaderDot("syntax time", format("%.3fs", OS::timerToSeconds(syntaxTime.load())));
    g_Log.messageHeaderDot("read files", format("%.3fs", OS::timerToSeconds(readFilesTime.load())));
    g_Log.messageHeaderDot("semantic time", format("%.3fs", OS::timerToSeconds(semanticModuleTime.load())));
    g_Log.messageHeaderDot("run time", format("%.3fs", OS::timerToSeconds(runTime.load())));
    g_Log.messageHeaderDot("run test time", format("%.3fs", OS::timerToSeconds(runTestTime.load())));
    g_Log.messageHeaderDot("output time", format("%.3fs", OS::timerToSeconds(outputTime.load())));
    g_Log.messageHeaderDot("prep out time", format("%.3fs %.3fs", OS::timerToSeconds(prepOutputTimePass.load()), OS::timerToSeconds(prepOutputTimeJob.load())));
    g_Log.messageHeaderDot("gen out time", format("%.3fs %.3fs", OS::timerToSeconds(genOutputTimePass.load()), OS::timerToSeconds(genOutputTimeJob.load())));
    g_Log.messageHeaderDot("optim bc time", format("%.3fs", OS::timerToSeconds(optimBCTime.load())));
    g_Log.print("\n");

    g_Log.messageHeaderDot("mem total", format("%s", toNiceSize(allocatorMemory.load()).c_str()));
    g_Log.messageHeaderDot("mem wasted", format("%s", toNiceSize(wastedMemory.load()).c_str()));
    g_Log.messageHeaderDot("mem nodes", format("%s", toNiceSize(memNodes.load()).c_str()));
    g_Log.messageHeaderDot("mem scopes", format("%s", toNiceSize(memScopes.load()).c_str()));
    g_Log.messageHeaderDot("mem seg", format("%s", toNiceSize(memSeg.load()).c_str()));
    g_Log.messageHeaderDot("mem concat", format("%s", toNiceSize(memConcat.load()).c_str()));
    g_Log.messageHeaderDot("mem types", format("%s", toNiceSize(memTypes.load()).c_str()));
    g_Log.messageHeaderDot("mem bcinstr", format("%s", toNiceSize(memInstructions.load()).c_str()));
    g_Log.messageHeaderDot("mem bcstack", format("%s", toNiceSize(memBcStack.load()).c_str()));
    g_Log.messageHeaderDot("mem symtable", format("%s", toNiceSize(memSymTable.load()).c_str()));
    g_Log.messageHeaderDot("mem symname", format("%s", toNiceSize(memSymName.load()).c_str()));
    g_Log.messageHeaderDot("mem symover", format("%s", toNiceSize(memSymOver.load()).c_str()));
    g_Log.messageHeaderDot("mem utf8", format("%s", toNiceSize(memUtf8.load()).c_str()));

    if (g_CommandLine.backendType == BackendType::X64)
    {
        g_Log.print("\n");
        g_Log.messageHeaderDot("x64 size backend dbg", format("%s", toNiceSize(sizeBackendDbg.load()).c_str()));
    }

    g_Log.setDefaultColor();
}
