#include "pch.h"
#include "Statistics.h"
#include "Workspace.h"
#include "Os.h"
#include "Module.h"
#include "ThreadManager.h"

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
            g_Log.print(Fmt("%5d ", countOpFreq[bestI][(int) ByteCodeOp::End].load()));
            g_Log.setColor(LogColor::DarkYellow);
            g_Log.print(Fmt("%s\n", g_ByteCodeOpDesc[bestI].name));
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
            g_Log.print(Fmt("%5d ", countOpFreq[bestI][bestJ].load()));
            g_Log.setColor(LogColor::DarkYellow);
            g_Log.print(Fmt("%s ", g_ByteCodeOpDesc[bestI].name));
            g_Log.setColor(LogColor::DarkCyan);
            g_Log.print(Fmt("%s\n", g_ByteCodeOpDesc[bestJ].name));
            countOpFreq[bestI][bestJ] = 0;
        }
    }

    g_Log.setDefaultColor();
}

void Stats::print()
{
    if (!g_CommandLine.stats)
        return;

    g_Log.setColor(LogColor::DarkCyan);
    g_Log.eol();

    static const LogColor COLOR_HEADER = LogColor::Cyan;
    static const LogColor COLOR_VALUE  = LogColor::Yellow;

    /////////////////////////
    g_Log.messageHeaderDot("workers", Fmt("%u", g_ThreadMgr.numWorkers), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("modules", Fmt("%u", numModules.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("files", Fmt("%u", numFiles.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("source lines", Fmt("%u", numLines.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("lines/s", Fmt("%u", (int) (numLines.load() / OS::timerToSeconds(g_Workspace->totalTime.load()))), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("tokens", Fmt("%u", numTokens.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("ast nodes", Fmt("%u", numNodes.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("ast released nodes", Fmt("%u", releaseNodes.load()), COLOR_HEADER, COLOR_VALUE);
    if (g_CommandLine.output)
        g_Log.messageHeaderDot("output modules", Fmt("%u", numGenModules.load()), COLOR_HEADER, COLOR_VALUE);
    if (g_CommandLine.test)
        g_Log.messageHeaderDot("executed #test", Fmt("%u", testFunctions.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("executed #run", Fmt("%u", runFunctions.load()), COLOR_HEADER, COLOR_VALUE);

    if (g_Workspace->numErrors)
        g_Log.messageHeaderDot("errors", Fmt("%u", g_Workspace->numErrors.load()), LogColor::Red);
    if (g_Workspace->numWarnings)
        g_Log.messageHeaderDot("warnings", Fmt("%u", g_Workspace->numWarnings.load()), LogColor::Magenta);
    g_Log.eol();

    g_Log.messageHeaderDot("instructions", Fmt("%u", numInstructions.load()), COLOR_HEADER, COLOR_VALUE);
    float pc1 = (totalOptimsBC.load() * 100.0f) / (numInstructions.load());
    g_Log.messageHeaderDot("kicked", Fmt("%d %.1f%%", totalOptimsBC.load(), pc1), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("total", Fmt("%u", numInstructions.load() - totalOptimsBC.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();

    g_Log.messageHeaderDot("concrete types", Fmt("%u", totalConcreteTypes.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("concrete struct types", Fmt("%u", totalConcreteStructTypes.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("init ptr", Fmt("%u", numInitPtr.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("init func ptr", Fmt("%u", numInitFuncPtr.load()), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();

    /////////////////////////
    g_Log.messageHeaderDot("cfg time", Fmt("%.3fs", OS::timerToSeconds(cfgTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("tokenizer time", Fmt("%.3fs", OS::timerToSeconds(tokenizerTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("syntax time", Fmt("%.3fs", OS::timerToSeconds(syntaxTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("read files", Fmt("%.3fs", OS::timerToSeconds(readFilesTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("semantic time", Fmt("%.3fs", OS::timerToSeconds(semanticTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("run time", Fmt("%.3fs", OS::timerToSeconds(runTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("run test time", Fmt("%.3fs", OS::timerToSeconds(runTestTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("prep out 1 time", Fmt("%.3fs (genfunc: %.3fs)", OS::timerToSeconds(prepOutputStage1TimeJob.load()), OS::timerToSeconds(prepOutputTimeJob_GenFunc.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("prep out 2 time", Fmt("%.3fs (saveobj: %.3fs)", OS::timerToSeconds(prepOutputStage2TimeJob.load()), OS::timerToSeconds(prepOutputTimeJob_SaveObj.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("gen out time", Fmt("%.3fs", OS::timerToSeconds(genOutputTimeJob.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("optim bc time", Fmt("%.3fs", OS::timerToSeconds(optimBCTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("total time", Fmt("%.3fs", OS::timerToSeconds(g_Workspace->totalTime.load())), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();

    /////////////////////////
    g_Log.messageHeaderDot("mem max", Fmt("%s", Utf8::toNiceSize(maxAllocatedMemory.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem allocated", Fmt("%s", Utf8::toNiceSize(allocatedMemory.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();
    g_Log.messageHeaderDot("mem utf8", Fmt("%s", Utf8::toNiceSize(memUtf8.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem new", Fmt("%s", Utf8::toNiceSize(memNew.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem std", Fmt("%s", Utf8::toNiceSize(memStd.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.eol();

    g_Log.messageHeaderDot("mem nodes", Fmt("%s", Utf8::toNiceSize(memNodes.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem nodes ext", Fmt("%s", Utf8::toNiceSize(memNodesExt.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem concat", Fmt("%s", Utf8::toNiceSize(memConcat.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem types", Fmt("%s", Utf8::toNiceSize(memTypes.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem params", Fmt("%s", Utf8::toNiceSize(memParams.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem files", Fmt("%s", Utf8::toNiceSize(memFileBuffer.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem nodes literal", Fmt("%s", Utf8::toNiceSize(memNodesLiteral.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem scopes", Fmt("%s", Utf8::toNiceSize(memScopes.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem seg", Fmt("%s", Utf8::toNiceSize(memSeg.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem bc instr", Fmt("%s", Utf8::toNiceSize(memInstructions.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem bc stack", Fmt("%s", Utf8::toNiceSize(memBcStack.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem symtable", Fmt("%s", Utf8::toNiceSize(memSymTable.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem symname", Fmt("%s", Utf8::toNiceSize(memSymName.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem symover", Fmt("%s", Utf8::toNiceSize(memSymOver.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    g_Log.messageHeaderDot("mem cstr", Fmt("%s", Utf8::toNiceSize(memUtf8CStr.load()).c_str()), COLOR_HEADER, COLOR_VALUE);
    if (g_CommandLine.backendGenType == BackendGenType::X64)
        g_Log.messageHeaderDot("mem x64 dbg", Fmt("%s", Utf8::toNiceSize(sizeBackendDbg.load()).c_str()), COLOR_HEADER, COLOR_VALUE);

    /////////////////////////
    g_Log.eol();
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

        g_Log.messageHeaderDot(Fmt("%s", m->name.c_str()), "");

        Utf8 mdlStr;
        g_Log.print("func total: ", COLOR_HEADER);
        g_Log.print(Fmt("%u", m->byteCodeFunc.size()), COLOR_VALUE);
        g_Log.print(" func emit: ", COLOR_HEADER);
        g_Log.print(Fmt("%u", m->byteCodeFuncToGen.size()), COLOR_VALUE);

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