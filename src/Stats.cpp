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

    if (g_CommandLine.statsWhat == StatsWhat::All || g_CommandLine.statsWhat == StatsWhat::Count)
    {
        g_Log.messageHeaderDot("workers", Utf8::format("%u", numWorkers));
        g_Log.messageHeaderDot("modules", Utf8::format("%u", numModules.load()));
        g_Log.messageHeaderDot("files", Utf8::format("%u", numFiles.load()));
        g_Log.messageHeaderDot("source lines", Utf8::format("%u", numLines.load()));
        g_Log.messageHeaderDot("lines/s", Utf8::format("%u", (int) (numLines.load() / OS::timerToSeconds(totalTime.load()))));
        if (g_CommandLine.output)
            g_Log.messageHeaderDot("output modules", Utf8::format("%u", numGenModules.load()));
        if (g_CommandLine.test)
            g_Log.messageHeaderDot("executed #test", Utf8::format("%u", testFunctions.load()));
        g_Log.messageHeaderDot("executed #run", Utf8::format("%u", runFunctions.load()));
        if (g_Workspace.numErrors)
            g_Log.messageHeaderDot("errors", Utf8::format("%u", g_Workspace.numErrors.load()), LogColor::Red);
        g_Log.print("\n");

        g_Log.messageHeaderDot("instructions", Utf8::format("%u", numInstructions.load()));
        float pc = (totalOptimsBC.load() * 100.0f) / (numInstructions.load());
        g_Log.messageHeaderDot("kicked", Utf8::format("%d %.1f%%", totalOptimsBC.load(), pc));
        g_Log.messageHeaderDot("total", Utf8::format("%u", numInstructions.load() - totalOptimsBC.load()));
        g_Log.print("\n");

        g_Log.messageHeaderDot("concrete types", Utf8::format("%u", totalConcreteTypes.load()));
        g_Log.messageHeaderDot("concrete struct types", Utf8::format("%u", totalConcreteStructTypes.load()));
        g_Log.messageHeaderDot("init ptr", Utf8::format("%u", numInitPtr.load()));
        g_Log.messageHeaderDot("init func ptr", Utf8::format("%u", numInitFuncPtr.load()));
        g_Log.print("\n");
    }

    if (g_CommandLine.statsWhat == StatsWhat::All || g_CommandLine.statsWhat == StatsWhat::Time)
    {
        g_Log.messageHeaderDot("cfg time", Utf8::format("%.3fs", OS::timerToSeconds(cfgTime.load())));
        g_Log.messageHeaderDot("syntax time", Utf8::format("%.3fs", OS::timerToSeconds(syntaxTime.load())));
        g_Log.messageHeaderDot("read files", Utf8::format("%.3fs", OS::timerToSeconds(readFilesTime.load())));
        g_Log.messageHeaderDot("semantic time", Utf8::format("%.3fs", OS::timerToSeconds(semanticTime.load())));
        g_Log.messageHeaderDot("run time", Utf8::format("%.3fs", OS::timerToSeconds(runTime.load())));
        g_Log.messageHeaderDot("run test time", Utf8::format("%.3fs", OS::timerToSeconds(runTestTime.load())));
        g_Log.messageHeaderDot("prep out time", Utf8::format("%.3fs (genfunc: %.3fs saveobj: %.3fs)", OS::timerToSeconds(prepOutputTimeJob.load()), OS::timerToSeconds(prepOutputTimeJob_GenFunc.load()), OS::timerToSeconds(prepOutputTimeJob_SaveObj.load())));
        g_Log.messageHeaderDot("gen out time", Utf8::format("%.3fs", OS::timerToSeconds(genOutputTimeJob.load())));
        g_Log.messageHeaderDot("optim bc time", Utf8::format("%.3fs", OS::timerToSeconds(optimBCTime.load())));
        g_Log.messageHeaderDot("contention time", Utf8::format("%.3fs", OS::timerToSeconds(contentionTime.load())));
        g_Log.messageHeaderDot("alloc time", Utf8::format("%.3fs", OS::timerToSeconds(allocTime.load())));
        g_Log.messageHeaderDot("free time", Utf8::format("%.3fs", OS::timerToSeconds(freeTime.load())));
        g_Log.messageHeaderDot("total time", Utf8::format("%.3fs", OS::timerToSeconds(totalTime.load())));
        g_Log.print("\n");
    }

    if (g_CommandLine.statsWhat == StatsWhat::All || g_CommandLine.statsWhat == StatsWhat::Memory)
    {
        g_Log.messageHeaderDot("mem total", Utf8::format("%s", Utf8::toNiceSize(allocatorMemory.load()).c_str()));
        g_Log.messageHeaderDot("mem wasted", Utf8::format("%s", Utf8::toNiceSize(wastedMemory.load()).c_str()));
        g_Log.messageHeaderDot("mem nodes", Utf8::format("%s", Utf8::toNiceSize(memNodes.load()).c_str()));
        g_Log.messageHeaderDot("mem nodes ext", Utf8::format("%s", Utf8::toNiceSize(memNodesExt.load()).c_str()));
        g_Log.messageHeaderDot("mem scopes", Utf8::format("%s", Utf8::toNiceSize(memScopes.load()).c_str()));
        g_Log.messageHeaderDot("mem seg", Utf8::format("%s", Utf8::toNiceSize(memSeg.load()).c_str()));
        g_Log.messageHeaderDot("mem concat", Utf8::format("%s", Utf8::toNiceSize(memConcat.load()).c_str()));
        g_Log.messageHeaderDot("mem types", Utf8::format("%s", Utf8::toNiceSize(memTypes.load()).c_str()));
        g_Log.messageHeaderDot("mem bcinstr", Utf8::format("%s", Utf8::toNiceSize(memInstructions.load()).c_str()));
        g_Log.messageHeaderDot("mem bcstack", Utf8::format("%s", Utf8::toNiceSize(memBcStack.load()).c_str()));
        g_Log.messageHeaderDot("mem symtable", Utf8::format("%s", Utf8::toNiceSize(memSymTable.load()).c_str()));
        g_Log.messageHeaderDot("mem symname", Utf8::format("%s", Utf8::toNiceSize(memSymName.load()).c_str()));
        g_Log.messageHeaderDot("mem symover", Utf8::format("%s", Utf8::toNiceSize(memSymOver.load()).c_str()));
        g_Log.messageHeaderDot("mem utf8", Utf8::format("%s", Utf8::toNiceSize(memUtf8.load()).c_str()));

        if (g_CommandLine.backendType == BackendType::X64)
        {
            g_Log.print("\n");
            g_Log.messageHeaderDot("mem x64 dbg", Utf8::format("%s", Utf8::toNiceSize(sizeBackendDbg.load()).c_str()));
        }
    }

    g_Log.setDefaultColor();
}
