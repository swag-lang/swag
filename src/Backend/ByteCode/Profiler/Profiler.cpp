#include "pch.h"

#ifdef SWAG_STATS
#include "Backend/ByteCode/ByteCode.h"
#include "Os/Os.h"
#include "Report/Log.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

constexpr int COL1 = 12;
constexpr int COL2 = 24;
constexpr int COL3 = 36;
constexpr int COL4 = 48;

namespace
{
    Utf8 getProfileBc(ByteCode* bc, int level)
    {
        Utf8 line;
        line += Log::colorToVTS(LogColor::Index);
        line += form("%d", bc->profileCallCount);

        line += Log::colorToVTS(LogColor::Value);
        while (Log::removeFormat(line).count < COL1)
            line += " ";
        line += form("%0.6f", OS::timerToSeconds(bc->profileCumTime));

        while (Log::removeFormat(line).count < COL2)
            line += " ";
        line += form("%0.6f", OS::timerToSeconds(bc->profileFFI));

        while (Log::removeFormat(line).count < COL3)
            line += " ";
        line += form("%0.6f", bc->profilePerCall);

        while (Log::removeFormat(line).count < COL4)
            line += " ";

        while (level--)
            line += "    ";

        line += Log::colorToVTS(LogColor::Location);
        if (bc->sourceFile)
        {
            line += bc->sourceFile->name;
            line += " -- ";
        }

        line += bc->getCallName();

        line += Log::colorToVTS(LogColor::Default);
        return line;
    }

    Utf8 getProfileFFI(const FFIStat& ffi)
    {
        Utf8 line;
        line += Log::colorToVTS(LogColor::Index);
        line += form("%d", ffi.count);

        while (Log::removeFormat(line).count < COL1)
            line += " ";

        line += Log::colorToVTS(LogColor::Value);
        line += form("%0.6f", OS::timerToSeconds(ffi.cum));

        while (Log::removeFormat(line).count < COL2)
            line += " ";

        line += Log::colorToVTS(LogColor::Location);
        line += ffi.name;

        line += Log::colorToVTS(LogColor::Default);
        return line;
    }

    void printChildren(const ByteCode* bc, int level)
    {
        if (level >= g_CommandLine.profileChildsLevel)
            return;

        level++;
        for (const auto child : bc->profileChildren)
        {
            Utf8 line = getProfileBc(child, level);
            g_Log.print(line);
            g_Log.writeEol();
            printChildren(child, level);
        }
    }
}

void profiler()
{
    if (!g_CommandLine.profile)
        return;

    g_Log.setColor(LogColor::Gray);
    g_Log.write("\n");

    // Collect
    //////////////////////////////////////////

    MapUtf8<FFIStat>  ffi;
    Vector<FFIStat>   linFFi;
    Vector<ByteCode*> bcs;

    for (const auto m : g_Workspace->modules)
    {
        for (auto bc : m->byteCodeFunc)
        {
            if (!bc->profileCallCount)
                continue;
            bc->profilePerCall = OS::timerToSeconds(bc->profileCumTime) / bc->profileCallCount;
            bcs.push_back(bc);

            for (const auto& it : bc->ffiProfile)
            {
                ffi[it.first].name = it.first;
                ffi[it.first].count += it.second.count;
                ffi[it.first].cum += it.second.cum;
            }
        }
    }

    // BC
    //////////////////////////////////////////

    std::ranges::sort(bcs, [](const ByteCode* a, const ByteCode* b) { return b->profileCumTime < a->profileCumTime; });
    while (!bcs.empty() && OS::timerToSeconds(bcs.back()->profileCumTime) <= g_CommandLine.profileMinTime)
        bcs.pop_back();

    Utf8 line;
    line += "#bc_calls";
    while (line.count < COL1)
        line += " ";
    line += "cum_time";
    while (line.count < COL2)
        line += " ";
    line += "ffi";
    while (line.count < COL3)
        line += " ";
    line += "per_call";
    while (line.count < COL4)
        line += " ";
    line += "name";
    g_Log.print(line);
    g_Log.writeEol();

    for (const auto bc : bcs)
    {
        if (!g_CommandLine.profileFilter.empty() && bc->name.find(g_CommandLine.profileFilter.c_str()) == -1)
            continue;
        line = getProfileBc(bc, 0);
        g_Log.print(line);
        g_Log.writeEol();
        printChildren(bc, 0);
    }

    // FFI
    //////////////////////////////////////////

    for (auto& val : ffi | std::views::values)
        linFFi.push_back(val);
    std::ranges::sort(linFFi, [](const FFIStat& a, const FFIStat& b) { return b.cum < a.cum; });
    while (!linFFi.empty() && OS::timerToSeconds(linFFi.back().cum) <= g_CommandLine.profileMinTime)
        linFFi.pop_back();

    g_Log.writeEol();
    line.clear();
    line += "#ffi_calls";
    while (line.count < COL1)
        line += " ";
    line += "cum_time";
    while (line.count < COL2)
        line += " ";
    line += "name";
    g_Log.print(line);
    g_Log.writeEol();

    for (auto& it : linFFi)
    {
        line = getProfileFFI(it);
        g_Log.print(line);
        g_Log.writeEol();
    }

    g_Log.setDefaultColor();
}

#endif // SWAG_STATS
