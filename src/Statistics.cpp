#include "pch.h"
#include "Statistics.h"
#include "Log.h"
#include "Module.h"
#include "Os.h"
#include "ThreadManager.h"
#include "Workspace.h"

#ifdef SWAG_STATS
Stats g_Stats;

void Stats::printFreq()
{
    if (!g_CommandLine.statsFreq)
        return;

    g_Log.eol();
    Utf8 str0 = g_CommandLine.statsFreqOp0;
    str0.makeLower();
    Utf8 str1 = g_CommandLine.statsFreqOp1;
    str1.makeLower();

    for (uint32_t cpt = 0; cpt < g_CommandLine.statsFreqCount; cpt++)
    {
        int best  = -1;
        int bestI = 0;

        for (int i = 0; i < (int) ByteCodeOp::End; i++)
        {
            // Filter by name
            if (!str0.empty())
            {
                Utf8 str2 = g_ByteCodeOpDesc[i].name;
                str2.makeLower();
                if (str2.find(str0) == -1)
                    continue;
            }

            if (countOpFreq[i][(int) ByteCodeOp::End] > best)
            {
                best  = countOpFreq[i][(int) ByteCodeOp::End];
                bestI = i;
            }
        }

        if (countOpFreq[bestI][(int) ByteCodeOp::End].load())
        {
            g_Log.setColor(LogColor::DarkCyan);
            g_Log.print(FMT("%5d ", countOpFreq[bestI][(int) ByteCodeOp::End].load()));
            g_Log.setColor(LogColor::DarkYellow);
            g_Log.print(FMT("%s\n", g_ByteCodeOpDesc[bestI].name));
            countOpFreq[bestI][(int) ByteCodeOp::End] = 0;
        }
    }

    g_Log.print("\n");

    for (uint32_t cpt = 0; cpt < g_CommandLine.statsFreqCount; cpt++)
    {
        int best  = -1;
        int bestI = 0;
        int bestJ = 0;
        for (int i = 0; i < (int) ByteCodeOp::End; i++)
        {
            // Filter by name
            if (!str0.empty())
            {
                Utf8 str2 = g_ByteCodeOpDesc[i].name;
                str2.makeLower();
                if (str2.find(str0) == -1)
                    continue;
            }

            for (int j = 0; j < (int) ByteCodeOp::End; j++)
            {
                // Filter by name
                if (!str1.empty())
                {
                    Utf8 str2 = g_ByteCodeOpDesc[j].name;
                    str2.makeLower();
                    if (str2.find(str1) == -1)
                        continue;
                }

                if (countOpFreq[i][j] > best)
                {
                    best  = countOpFreq[i][j];
                    bestI = i;
                    bestJ = j;
                }
            }
        }

        if (countOpFreq[bestI][bestJ].load())
        {
            g_Log.setColor(LogColor::DarkCyan);
            g_Log.print(FMT("%5d ", countOpFreq[bestI][bestJ].load()));
            g_Log.setColor(LogColor::DarkYellow);
            g_Log.print(FMT("%s ", g_ByteCodeOpDesc[bestI].name));
            g_Log.setColor(LogColor::DarkCyan);
            g_Log.print(FMT("%s\n", g_ByteCodeOpDesc[bestJ].name));
            countOpFreq[bestI][bestJ] = 0;
        }
    }

    g_Log.setDefaultColor();
}

void Stats::print() const
{
    if (!g_CommandLine.stats)
        return;

    g_Log.setColor(LogColor::DarkCyan);
    g_Log.eol();

    static constexpr LogColor COLOR_HEADER = LogColor::Cyan;
    static constexpr LogColor COLOR_VALUE  = LogColor::Yellow;

    /////////////////////////
    g_Log.messageHeaderDot("workers", FMT("%u", g_ThreadMgr.numWorkers), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("modules", FMT("%u", numModules.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("files", FMT("%u", numFiles.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("source lines", FMT("%u", numLines.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("lines/s", FMT("%u", (int) (numLines.load() / OS::timerToSeconds(g_Workspace->totalTime.load()))), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("tokens", FMT("%u", numTokens.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("ast nodes", FMT("%u", numNodes.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("ast released nodes", FMT("%u", releaseNodes.load()), COLOR_HEADER, COLOR_VALUE);
    if (g_CommandLine.output)
        g_Log.messageHeaderDot("output modules", FMT("%u", numGenModules.load()), COLOR_HEADER, COLOR_VALUE);
    if (g_CommandLine.test)
        g_Log.messageHeaderDot("executed #test", FMT("%u", testFunctions.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("executed #run", FMT("%u", runFunctions.load()), COLOR_HEADER, COLOR_VALUE);

    if (g_Workspace->numErrors)
        g_Log.messageHeaderDot("errors", FMT("%u", g_Workspace->numErrors.load()), LogColor::Red);
    if (g_Workspace->numWarnings)
        g_Log.messageHeaderDot("warnings", FMT("%u", g_Workspace->numWarnings.load()), LogColor::Magenta);
    g_Log.eol();

    g_Log.messageHeaderDot("instructions", FMT("%u", numInstructions.load()), COLOR_HEADER, COLOR_VALUE);
    const float pc1 = ((float) totalOptimsBC.load() * 100.0f) / (numInstructions.load());
    g_Log.messageHeaderDot("kicked", FMT("%d %.1f%%", totalOptimsBC.load(), pc1), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("total", FMT("%u", numInstructions.load() - totalOptimsBC.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();

    g_Log.messageHeaderDot("concrete types", FMT("%u", totalConcreteTypes.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("concrete struct types", FMT("%u", totalConcreteStructTypes.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("init ptr", FMT("%u", numInitPtr.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("init func ptr", FMT("%u", numInitFuncPtr.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();

    /////////////////////////
    g_Log.messageHeaderDot("cfg time", FMT("%.3fs", OS::timerToSeconds(cfgTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("tokenizer time", FMT("%.3fs", OS::timerToSeconds(tokenizerTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("syntax time", FMT("%.3fs", OS::timerToSeconds(syntaxTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("read files", FMT("%.3fs", OS::timerToSeconds(readFilesTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("semantic time", FMT("%.3fs", OS::timerToSeconds(semanticTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("run time", FMT("%.3fs", OS::timerToSeconds(runTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("run test time", FMT("%.3fs", OS::timerToSeconds(runTestTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("prep out 1 time", FMT("%.3fs (genfunc: %.3fs)", OS::timerToSeconds(prepOutputStage1TimeJob.load()), OS::timerToSeconds(prepOutputTimeJob_GenFunc.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("prep out 2 time", FMT("%.3fs (saveobj: %.3fs)", OS::timerToSeconds(prepOutputStage2TimeJob.load()), OS::timerToSeconds(prepOutputTimeJob_SaveObj.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("gen out time", FMT("%.3fs", OS::timerToSeconds(genOutputTimeJob.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("optim bc time", FMT("%.3fs", OS::timerToSeconds(optimBCTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("total time", FMT("%.3fs", OS::timerToSeconds(g_Workspace->totalTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();

    /////////////////////////
    g_Log.messageHeaderDot("mem max", FMT("%s", Utf8::toNiceSize(maxAllocatedMemory.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem allocated", FMT("%s", Utf8::toNiceSize(allocatedMemory.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();
    g_Log.messageHeaderDot("mem utf8", FMT("%s", Utf8::toNiceSize(memUtf8.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem new", FMT("%s", Utf8::toNiceSize(memNew.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem std", FMT("%s", Utf8::toNiceSize(memStd.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();

    g_Log.messageHeaderDot("mem nodes", FMT("%s", Utf8::toNiceSize(memNodes.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem nodes ext", FMT("%s", Utf8::toNiceSize(memNodesExt.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem concat", FMT("%s", Utf8::toNiceSize(memConcat.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem types", FMT("%s", Utf8::toNiceSize(memTypes.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem params", FMT("%s", Utf8::toNiceSize(memParams.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem files", FMT("%s", Utf8::toNiceSize(memFileBuffer.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem nodes literal", FMT("%s", Utf8::toNiceSize(memNodesLiteral.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem scopes", FMT("%s", Utf8::toNiceSize(memScopes.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem seg", FMT("%s", Utf8::toNiceSize(memSeg.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem bc instr", FMT("%s", Utf8::toNiceSize(memInstructions.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem bc stack", FMT("%s", Utf8::toNiceSize(memBcStack.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem sym table", FMT("%s", Utf8::toNiceSize(memSymTable.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem sym name", FMT("%s", Utf8::toNiceSize(memSymName.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem sym over", FMT("%s", Utf8::toNiceSize(memSymOver.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem c str", FMT("%s", Utf8::toNiceSize(memUtf8CStr.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    if (g_CommandLine.backendGenType == BackendGenType::SCBE)
        g_Log.messageHeaderDot("mem x64 dbg", FMT("%s", Utf8::toNiceSize(sizeBackendDbg.load()).c_str()), COLOR_HEADER, COLOR_VALUE);

    /////////////////////////
    g_Log.eol();
    for (const auto m : g_Workspace->modules)
    {
        if (m->isErrorModule)
            continue;

        // Nothing has been done
        if (m->byteCodeFunc.empty() &&
            !m->constantSegment.totalCount &&
            !m->mutableSegment.totalCount &&
            !m->tlsSegment.totalCount)
            continue;

        g_Log.messageHeaderDot(FMT("%s", m->name.c_str()), "");

        Utf8 mdlStr;
        g_Log.print("func total: ", COLOR_HEADER);
        g_Log.print(FMT("%u", m->byteCodeFunc.size()), COLOR_VALUE);
        g_Log.print(" func emit: ", COLOR_HEADER);
        g_Log.print(FMT("%u", m->byteCodeFuncToGen.size()), COLOR_VALUE);

        g_Log.print(" seg constant: ", COLOR_HEADER);
        g_Log.print(Utf8::toNiceSize(m->constantSegment.totalCount), COLOR_VALUE);
        g_Log.print(" seg mutable: ", COLOR_HEADER);
        g_Log.print(Utf8::toNiceSize(m->mutableSegment.totalCount), COLOR_VALUE);
        g_Log.print(" seg tls: ", COLOR_HEADER);
        g_Log.print(Utf8::toNiceSize(m->tlsSegment.totalCount), COLOR_VALUE);
        g_Log.eol();
    }

    g_Log.setDefaultColor();
}

#endif // SWAG_STATS
