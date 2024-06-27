#include "pch.h"

#ifdef SWAG_STATS
#include "Main/Statistics.h"
#include "Os/Os.h"
#include "Report/Log.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

Stats g_Stats;

void Stats::printFreq()
{
    if (!g_CommandLine.statsFreq && g_CommandLine.statsFreqOp0.empty() && g_CommandLine.statsFreqOp1.empty())
        return;

    g_Log.writeEol();

    Utf8 str0 = g_CommandLine.statsFreqOp0;
    str0.makeLower();
    Utf8 str1 = g_CommandLine.statsFreqOp1;
    str1.makeLower();

    constexpr auto endOp = static_cast<uint32_t>(ByteCodeOp::End);

    for (uint32_t cpt = 0; cpt < g_CommandLine.statsFreqCount; cpt++)
    {
        uint32_t best  = 0;
        uint32_t bestI = UINT32_MAX;

        for (uint32_t i = 0; i < endOp; i++)
        {
            // Filter by name
            if (!str0.empty())
            {
                Utf8 str2 = g_ByteCodeOpDesc[i].name;
                str2.makeLower();
                if (str2.find(str0) == -1)
                    continue;
            }
            else if (!str1.empty())
            {
                Utf8 str2 = g_ByteCodeOpDesc[i].name;
                str2.makeLower();
                if (str2.find(str1) == -1)
                    continue;
            }

            if (countOpFreq[i][endOp] > best)
            {
                best  = countOpFreq[i][endOp];
                bestI = i;
            }
        }

        if (bestI != UINT32_MAX && countOpFreq[bestI][endOp].load())
        {
            if (g_CommandLine.statsFreq)
            {
                g_Log.setColor(LogColor::Index);
                g_Log.print(form("%5d ", countOpFreq[bestI][endOp].load()));
                g_Log.setColor(LogColor::Name);
                g_Log.print(form("%s", g_ByteCodeOpDesc[bestI].name));
                g_Log.writeEol();
            }

            countOpFreq[bestI][endOp] = 0;
        }
    }

    if (g_CommandLine.statsFreq)
        g_Log.writeEol();

    for (uint32_t cpt = 0; cpt < g_CommandLine.statsFreqCount; cpt++)
    {
        uint32_t best  = 0;
        uint32_t bestI = UINT32_MAX;
        uint32_t bestJ = UINT32_MAX;
        for (uint32_t i = 0; i < endOp; i++)
        {
            // Filter by name
            if (!str0.empty())
            {
                Utf8 str2 = g_ByteCodeOpDesc[i].name;
                str2.makeLower();
                if (str2.find(str0) == -1)
                    continue;
            }

            for (uint32_t j = 0; j < endOp; j++)
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

        if (bestI != UINT32_MAX && countOpFreq[bestI][bestJ].load())
        {
            g_Log.setColor(LogColor::Index);
            g_Log.print(form("%5d ", countOpFreq[bestI][bestJ].load()));
            g_Log.setColor(LogColor::Name);
            g_Log.print(form("%s ", g_ByteCodeOpDesc[bestI].name));
            g_Log.setColor(LogColor::Location);
            g_Log.print(form("%s", g_ByteCodeOpDesc[bestJ].name));
            g_Log.writeEol();
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
    g_Log.writeEol();

    /////////////////////////
    g_Log.messageHeaderDot("workers", form("%u", g_ThreadMgr.numWorkers), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("modules", form("%u", numModules.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("files", form("%u", numFiles.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("source lines", form("%u", numLines.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("lines/s", form("%u", static_cast<int>(numLines.load() / OS::timerToSeconds(g_Workspace->totalTime.load()))), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("tokens", form("%u", numTokens.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("custom 0", form("%u", numCustom0.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("custom 1", form("%u", numCustom1.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("ast nodes", form("%u", numNodes.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("ast released nodes", form("%u", releaseNodes.load()), LogColor::Header, LogColor::Value);
    if (g_CommandLine.output)
        g_Log.messageHeaderDot("output modules", form("%u", numGenModules.load()), LogColor::Header, LogColor::Value);
    if (g_CommandLine.test)
        g_Log.messageHeaderDot("executed #test", form("%u", testFunctions.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("executed #run", form("%u", runFunctions.load()), LogColor::Header, LogColor::Value);

    if (g_Workspace->numErrors)
        g_Log.messageHeaderDot("errors", form("%u", g_Workspace->numErrors.load()), LogColor::Red);
    if (g_Workspace->numWarnings)
        g_Log.messageHeaderDot("warnings", form("%u", g_Workspace->numWarnings.load()), LogColor::Magenta);
    g_Log.writeEol();

    g_Log.messageHeaderDot("instructions", form("%u", numInstructions.load()), LogColor::Header, LogColor::Value);
    const float pc1 = static_cast<float>(totalOptimBC.load()) * 100.0f / static_cast<float>(numInstructions.load());
    g_Log.messageHeaderDot("kicked", form("%d %.1f%%", totalOptimBC.load(), pc1), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("total", form("%u", numInstructions.load() - totalOptimBC.load()), LogColor::Header, LogColor::Value);
    g_Log.writeEol();

    g_Log.messageHeaderDot("concrete types", form("%u", totalConcreteTypes.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("concrete struct types", form("%u", totalConcreteStructTypes.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("init ptr", form("%u", numInitPtr.load()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("init func ptr", form("%u", numInitFuncPtr.load()), LogColor::Header, LogColor::Value);
    g_Log.writeEol();

    /////////////////////////
    g_Log.messageHeaderDot("cfg time", form("%.3fs", OS::timerToSeconds(cfgTime.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("tokenizer time", form("%.3fs", OS::timerToSeconds(tokenizerTime.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("syntax time", form("%.3fs", OS::timerToSeconds(syntaxTime.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("read files", form("%.3fs", OS::timerToSeconds(readFilesTime.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("semantic time", form("%.3fs", OS::timerToSeconds(semanticTime.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("run time", form("%.3fs", OS::timerToSeconds(runTime.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("run test time", form("%.3fs", OS::timerToSeconds(runTestTime.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("prep out 1 time", form("%.3fs (gen func: %.3fs)", OS::timerToSeconds(prepOutputStage1TimeJob.load()), OS::timerToSeconds(prepOutputTimeJobGenFunc.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("prep out 2 time", form("%.3fs (save obj: %.3fs)", OS::timerToSeconds(prepOutputStage2TimeJob.load()), OS::timerToSeconds(prepOutputTimeJobSaveObj.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("gen out time", form("%.3fs", OS::timerToSeconds(genOutputTimeJob.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("optim bc time", form("%.3fs", OS::timerToSeconds(optimBCTime.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("ffi gen time", form("%.3fs", OS::timerToSeconds(ffiGenTime.load())), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("total time", form("%.3fs", OS::timerToSeconds(g_Workspace->totalTime.load())), LogColor::Header, LogColor::Value);
    g_Log.writeEol();

    /////////////////////////
    g_Log.messageHeaderDot("mem max", form("%s", Utf8::toNiceSize(maxAllocatedMemory.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem allocated", form("%s", Utf8::toNiceSize(allocatedMemory.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.writeEol();
    g_Log.messageHeaderDot("mem utf8", form("%s", Utf8::toNiceSize(memUtf8.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem c str", form("%s", Utf8::toNiceSize(memUtf8CStr.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem new", form("%s", Utf8::toNiceSize(memNew.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem std", form("%s", Utf8::toNiceSize(memStd.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.writeEol();
    g_Log.messageHeaderDot("mem ast nodes", form("%s", Utf8::toNiceSize(memNodes.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem ast nodes ext", form("%s", Utf8::toNiceSize(memNodesExt.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem ast nodes literal", form("%s", Utf8::toNiceSize(memNodesLiteral.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem ast identifiers ext", form("%s", Utf8::toNiceSize(memIdentifiersExt.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.writeEol();
    g_Log.messageHeaderDot("mem concat", form("%s", Utf8::toNiceSize(memConcat.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem types", form("%s", Utf8::toNiceSize(memTypes.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem params", form("%s", Utf8::toNiceSize(memParams.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem files", form("%s", Utf8::toNiceSize(memFileBuffer.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem scopes", form("%s", Utf8::toNiceSize(memScopes.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem seg", form("%s", Utf8::toNiceSize(memSeg.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem bc instr", form("%s", Utf8::toNiceSize(memInstructions.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem bc stack", form("%s", Utf8::toNiceSize(memBcStack.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem sym table", form("%s", Utf8::toNiceSize(memSymTable.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem sym name", form("%s", Utf8::toNiceSize(memSymName.load()).c_str()), LogColor::Header, LogColor::Value);
    g_Log.messageHeaderDot("mem sym over", form("%s", Utf8::toNiceSize(memSymOver.load()).c_str()), LogColor::Header, LogColor::Value);
    if (g_CommandLine.backendGenType == BackendGenType::SCBE)
        g_Log.messageHeaderDot("mem x64 dbg", form("%s", Utf8::toNiceSize(sizeBackendDbg.load()).c_str()), LogColor::Header, LogColor::Value);

    /////////////////////////
    g_Log.writeEol();
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

        g_Log.messageHeaderDot(form("%s", m->name.c_str()), "", LogColor::Header);

        Utf8 mdlStr;
        g_Log.print("func total: ", LogColor::Header);
        g_Log.print(form("%u", m->byteCodeFunc.size()), LogColor::Value);
        g_Log.print(" func emit: ", LogColor::Header);
        g_Log.print(form("%u", m->byteCodeFuncToGen.size()), LogColor::Value);

        g_Log.print(" seg constant: ", LogColor::Header);
        g_Log.print(Utf8::toNiceSize(m->constantSegment.totalCount), LogColor::Value);
        g_Log.print(" seg mutable: ", LogColor::Header);
        g_Log.print(Utf8::toNiceSize(m->mutableSegment.totalCount), LogColor::Value);
        g_Log.print(" seg tls: ", LogColor::Header);
        g_Log.print(Utf8::toNiceSize(m->tlsSegment.totalCount), LogColor::Value);
        g_Log.writeEol();
    }

    g_Log.setDefaultColor();
}

#endif // SWAG_STATS
