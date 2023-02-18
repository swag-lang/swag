#include "pch.h"
#include "Stats.h"
#include "Workspace.h"
#include "Version.h"
#include "Os.h"
#include "Module.h"
#include "ByteCode.h"
#ifdef SWAG_STATS

static Utf8 getProfileBc(ByteCode* bc, int level)
{
    Utf8 line;
    line += Fmt("%d", bc->profileCallCount);
    while (line.count < 12)
        line += " ";

    line += Fmt("%0.6f", OS::timerToSeconds(bc->profileCumTime));
    while (line.count < 24)
        line += " ";

    line += Fmt("%0.6f", bc->profilePerCall);
    while (line.count < 36)
        line += " ";

    while (level--)
        line += "    ";

    line += bc->name;
    if (bc->getCallType())
    {
        line += " -- ";
        line += bc->getCallType()->name.c_str();
    }

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

    Vector<ByteCode*> bcs;
    for (auto m : g_Workspace->modules)
    {
        for (auto bc : m->byteCodeFunc)
        {
            if (!bc->profileCallCount)
                continue;
            bc->profilePerCall = OS::timerToSeconds(bc->profileCumTime) / bc->profileCallCount;
            bcs.push_back(bc);
        }
    }

    sort(bcs.begin(), bcs.end(), [](ByteCode* a, ByteCode* b)
         { return b->profileCumTime < a->profileCumTime; });

    Utf8 line;
    line += "#calls";
    while (line.count < 12)
        line += " ";
    line += "cumtime";
    while (line.count < 24)
        line += " ";
    line += "percall";
    while (line.count < 36)
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
}

#endif // SWAG_STATS