#include "pch.h"
#include "Stats.h"
#include "Workspace.h"
#include "Version.h"
#include "Os.h"
#include "Module.h"
#include "ByteCode.h"

Stats g_Stats;

void Stats::print()
{
    if (!g_CommandLine.stats || g_CommandLine.silent)
        return;

    g_Log.setColor(LogColor::DarkCyan);
    g_Log.print("\n");

    if (g_CommandLine.statsWhat == StatsWhat::All || g_CommandLine.statsWhat == StatsWhat::Count)
    {
        g_Log.messageHeaderDot("workers", Fmt("%u", numWorkers));
        g_Log.messageHeaderDot("modules", Fmt("%u", numModules.load()));
        g_Log.messageHeaderDot("files", Fmt("%u", numFiles.load()));
        g_Log.messageHeaderDot("source lines", Fmt("%u", numLines.load()));
        g_Log.messageHeaderDot("lines/s", Fmt("%u", (int) (numLines.load() / OS::timerToSeconds(totalTime.load()))));
        g_Log.messageHeaderDot("tokens", Fmt("%u", numTokens.load()));
        g_Log.messageHeaderDot("ast nodes", Fmt("%u", numNodes.load()));
        if (g_CommandLine.output)
            g_Log.messageHeaderDot("output modules", Fmt("%u", numGenModules.load()));
        if (g_CommandLine.test)
            g_Log.messageHeaderDot("executed #test", Fmt("%u", testFunctions.load()));
        g_Log.messageHeaderDot("executed #run", Fmt("%u", runFunctions.load()));
        g_Log.messageHeaderDot("san passed", Fmt("%u", numSan.load()));
        if (g_Workspace->numErrors)
            g_Log.messageHeaderDot("errors", Fmt("%u", g_Workspace->numErrors.load()), LogColor::Red);
        if (g_Workspace->numWarnings)
            g_Log.messageHeaderDot("warnings", Fmt("%u", g_Workspace->numWarnings.load()), LogColor::Red);
        g_Log.print("\n");

        g_Log.messageHeaderDot("instructions", Fmt("%u", numInstructions.load()));
        float pc = (totalOptimsBC.load() * 100.0f) / (numInstructions.load());
        g_Log.messageHeaderDot("kicked", Fmt("%d %.1f%%", totalOptimsBC.load(), pc));
        g_Log.messageHeaderDot("total", Fmt("%u", numInstructions.load() - totalOptimsBC.load()));
        g_Log.print("\n");

        g_Log.messageHeaderDot("concrete types", Fmt("%u", totalConcreteTypes.load()));
        g_Log.messageHeaderDot("concrete struct types", Fmt("%u", totalConcreteStructTypes.load()));
        g_Log.messageHeaderDot("init ptr", Fmt("%u", numInitPtr.load()));
        g_Log.messageHeaderDot("init func ptr", Fmt("%u", numInitFuncPtr.load()));
        g_Log.print("\n");
    }

    if (g_CommandLine.statsWhat == StatsWhat::All || g_CommandLine.statsWhat == StatsWhat::Time)
    {
        g_Log.messageHeaderDot("cfg time", Fmt("%.3fs", OS::timerToSeconds(cfgTime.load())));
        g_Log.messageHeaderDot("tokenizer time", Fmt("%.3fs", OS::timerToSeconds(tokenizerTime.load())));
        g_Log.messageHeaderDot("syntax time", Fmt("%.3fs", OS::timerToSeconds(syntaxTime.load())));
        g_Log.messageHeaderDot("read files", Fmt("%.3fs", OS::timerToSeconds(readFilesTime.load())));
        g_Log.messageHeaderDot("semantic time", Fmt("%.3fs", OS::timerToSeconds(semanticTime.load())));
        g_Log.messageHeaderDot("run time", Fmt("%.3fs", OS::timerToSeconds(runTime.load())));
        g_Log.messageHeaderDot("run test time", Fmt("%.3fs", OS::timerToSeconds(runTestTime.load())));
        g_Log.messageHeaderDot("prep out 1 time", Fmt("%.3fs (genfunc: %.3fs)", OS::timerToSeconds(prepOutputStage1TimeJob.load()), OS::timerToSeconds(prepOutputTimeJob_GenFunc.load())));
        g_Log.messageHeaderDot("prep out 2 time", Fmt("%.3fs (saveobj: %.3fs)", OS::timerToSeconds(prepOutputStage2TimeJob.load()), OS::timerToSeconds(prepOutputTimeJob_SaveObj.load())));
        g_Log.messageHeaderDot("gen out time", Fmt("%.3fs", OS::timerToSeconds(genOutputTimeJob.load())));
        g_Log.messageHeaderDot("optim bc time", Fmt("%.3fs", OS::timerToSeconds(optimBCTime.load())));
        g_Log.messageHeaderDot("total time", Fmt("%.3fs", OS::timerToSeconds(totalTime.load())));
        g_Log.print("\n");
    }

    if (g_CommandLine.statsWhat == StatsWhat::All || g_CommandLine.statsWhat == StatsWhat::Memory)
    {
        g_Log.messageHeaderDot("mem total", Fmt("%s", Utf8::toNiceSize(allocatorMemory.load()).c_str()));
        g_Log.messageHeaderDot("mem wasted", Fmt("%s", Utf8::toNiceSize(wastedMemory.load()).c_str()));
        g_Log.print("\n");
        g_Log.messageHeaderDot("mem nodes", Fmt("%s", Utf8::toNiceSize(memNodes.load()).c_str()));
        g_Log.messageHeaderDot("mem nodes ext", Fmt("%s", Utf8::toNiceSize(memNodesExt.load()).c_str()));
        g_Log.messageHeaderDot("mem scopes", Fmt("%s", Utf8::toNiceSize(memScopes.load()).c_str()));
        g_Log.messageHeaderDot("mem seg", Fmt("%s", Utf8::toNiceSize(memSeg.load()).c_str()));
        g_Log.messageHeaderDot("mem concat", Fmt("%s", Utf8::toNiceSize(memConcat.load()).c_str()));
        g_Log.messageHeaderDot("mem types", Fmt("%s", Utf8::toNiceSize(memTypes.load()).c_str()));
        g_Log.messageHeaderDot("mem params", Fmt("%s", Utf8::toNiceSize(memParams.load()).c_str()));
        g_Log.messageHeaderDot("mem bcinstr", Fmt("%s", Utf8::toNiceSize(memInstructions.load()).c_str()));
        g_Log.messageHeaderDot("mem bcstack", Fmt("%s", Utf8::toNiceSize(memBcStack.load()).c_str()));
        g_Log.messageHeaderDot("mem symtable", Fmt("%s", Utf8::toNiceSize(memSymTable.load()).c_str()));
        g_Log.messageHeaderDot("mem symname", Fmt("%s", Utf8::toNiceSize(memSymName.load()).c_str()));
        g_Log.messageHeaderDot("mem symover", Fmt("%s", Utf8::toNiceSize(memSymOver.load()).c_str()));
        g_Log.messageHeaderDot("mem utf8", Fmt("%s", Utf8::toNiceSize(memUtf8.load()).c_str()));
        g_Log.messageHeaderDot("mem cstr", Fmt("%s", Utf8::toNiceSize(memUtf8CStr.load()).c_str()));
        g_Log.messageHeaderDot("mem new", Fmt("%s", Utf8::toNiceSize(memNew.load()).c_str()));

        if (g_CommandLine.backendGenType == BackendGenType::X64)
        {
            g_Log.print("\n");
            g_Log.messageHeaderDot("mem x64 dbg", Fmt("%s", Utf8::toNiceSize(sizeBackendDbg.load()).c_str()));
        }
    }

    if (g_CommandLine.statsWhat == StatsWhat::Module)
    {
        for (auto m : g_Workspace->modules)
        {
            if (m->isErrorModule)
                continue;

            // Nothing has been done
            if (m->byteCodeFunc.empty() &&
                !m->constantSegment.totalCount &&
                !m->mutableSegment.totalCount &&
                !m->tlsSegment.totalCount)
                continue;

            g_Log.print("\n");
            g_Log.messageHeaderDot("module", Fmt("%s", m->name.c_str()));
            g_Log.messageHeaderDot("func total", Fmt("%u", m->byteCodeFunc.size()));
            g_Log.messageHeaderDot("func emit", Fmt("%u", m->byteCodeFuncToGen.size()));
            g_Log.messageHeaderDot("func kicked", Fmt("%u", m->numKickedFunc));
            g_Log.messageHeaderDot("seg constant", Fmt("%s", Utf8::toNiceSize(m->constantSegment.totalCount).c_str()));
            g_Log.messageHeaderDot("seg mutable", Fmt("%s", Utf8::toNiceSize(m->mutableSegment.totalCount).c_str()));
            g_Log.messageHeaderDot("seg tls", Fmt("%s", Utf8::toNiceSize(m->tlsSegment.totalCount).c_str()));
        }
    }

    g_Log.setDefaultColor();
}
