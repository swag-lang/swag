#include "pch.h"
#include "ByteCode.h"
#include "Module.h"
#include "Workspace.h"
#ifdef SWAG_STATS

const int COL1 = 12;
const int COL2 = 24;
const int COL3 = 36;
const int COL4 = 48;

static Utf8 getProfileBc(ByteCode* bc, int level)
{
    Utf8 line;
    line += Fmt("%d", bc->profileCallCount);

    while (line.count < COL1)
        line += " ";
    line += Fmt("%0.6f", OS::timerToSeconds(bc->profileCumTime));

    while (line.count < COL2)
        line += " ";
    line += Fmt("%0.6f", OS::timerToSeconds(bc->profileFFI));

    while (line.count < COL3)
        line += " ";
    line += Fmt("%0.6f", bc->profilePerCall);

    while (line.count < COL4)
        line += " ";

    while (level--)
        line += "    ";

    if (bc->sourceFile)
    {
        line += bc->sourceFile->name;
        line += " -- ";
    }

    line += bc->getCallName();
    return line;
}

static Utf8 getProfileFFI(FFIStat& ffi, int level)
{
    Utf8 line;
    line += Fmt("%d", ffi.count);

    while (line.count < COL1)
        line += " ";
    line += Fmt("%0.6f", OS::timerToSeconds(ffi.cum));

    while (line.count < COL2)
        line += " ";

    line += ffi.name;
    return line;
}

static void printChilds(ByteCode* bc, int level)
{
    if (level >= g_CommandLine.profileChildsLevel)
        return;

    level++;
    for (auto child : bc->profileChilds)
    {
        Utf8 line = getProfileBc(child, level);
        g_Log.print(line);
        g_Log.eol();
        printChilds(child, level);
    }
}

void profiler()
{
    if (!g_CommandLine.profile)
        return;

    g_Log.setColor(LogColor::Gray);
    g_Log.print("\n");

    // Collect
    //////////////////////////////////////////

    MapUtf8<FFIStat>  ffi;
    Vector<FFIStat>   linFFi;
    Vector<ByteCode*> bcs;

    for (auto m : g_Workspace->modules)
    {
        for (auto bc : m->byteCodeFunc)
        {
            if (!bc->profileCallCount)
                continue;
            bc->profilePerCall = OS::timerToSeconds(bc->profileCumTime) / bc->profileCallCount;
            bcs.push_back(bc);

            for (auto it : bc->ffiProfile)
            {
                ffi[it.first].name = it.first;
                ffi[it.first].count += it.second.count;
                ffi[it.first].cum += it.second.cum;
            }
        }
    }

    // BC
    //////////////////////////////////////////

    sort(bcs.begin(), bcs.end(), [](ByteCode* a, ByteCode* b)
         { return b->profileCumTime < a->profileCumTime; });
    while (!bcs.empty() && OS::timerToSeconds(bcs.back()->profileCumTime) <= g_CommandLine.profileMinTime)
        bcs.pop_back();

    Utf8 line;
    line += "#bccalls";
    while (line.count < COL1)
        line += " ";
    line += "cumtime";
    while (line.count < COL2)
        line += " ";
    line += "FFI";
    while (line.count < COL3)
        line += " ";
    line += "percall";
    while (line.count < COL4)
        line += " ";
    line += "name";
    g_Log.print(line);
    g_Log.eol();

    for (auto bc : bcs)
    {
        if (!g_CommandLine.profileFilter.empty() && bc->name.find(g_CommandLine.profileFilter.c_str()) == -1)
            continue;
        line = getProfileBc(bc, 0);
        g_Log.print(line);
        g_Log.eol();
        printChilds(bc, 0);
    }

    // FFI
    //////////////////////////////////////////

    for (auto& it : ffi)
        linFFi.push_back(it.second);
    sort(linFFi.begin(), linFFi.end(), [](const FFIStat& a, const FFIStat& b)
         { return b.cum < a.cum; });
    while (!linFFi.empty() && OS::timerToSeconds(linFFi.back().cum) <= g_CommandLine.profileMinTime)
        linFFi.pop_back();

    g_Log.eol();
    line.clear();
    line += "#fficalls";
    while (line.count < COL1)
        line += " ";
    line += "cumtime";
    while (line.count < COL2)
        line += " ";
    line += "name";
    g_Log.print(line);
    g_Log.eol();

    for (auto& it : linFFi)
    {
        line = getProfileFFI(it, 0);
        g_Log.print(line);
        g_Log.eol();
    }
}

#endif // SWAG_STATS