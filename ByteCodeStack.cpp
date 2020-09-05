#include "pch.h"
#include "Global.h"
#include "Log.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "Os.h"
#include "ByteCodeStack.h"
#include "ByteCode.h"

thread_local ByteCodeStack g_byteCodeStack;

void ByteCodeStack::log()
{
    if (steps.empty())
        return;

    g_Log.setColor(LogColor::DarkYellow);
    for (int i = (int) steps.size() - 1; i >= 0; i--)
    {
        const auto& step = steps[i];
        g_Log.print(format("%s", step.ip->node->sourceFile->path.c_str()));
        g_Log.print(format(":%d: ", step.ip->getLocation(step.bc)->line + 1));
        g_Log.eol();
    }

    g_Log.setDefaultColor();
}

void ByteCodeStack::reportError(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print(msg);
    g_Log.eol();
    log();
    g_Log.setDefaultColor();
    g_Log.unlock();
}
