#include "pch.h"
#include "Global.h"
#include "Log.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "Os.h"
#include "DiagnosticInfos.h"
#include "ByteCode.h"

thread_local DiagnosticInfos g_diagnosticInfos;

void DiagnosticInfos::log()
{
    if (steps.empty())
        return;

    g_Log.setColor(LogColor::DarkYellow);
    for (int i = (int) steps.size() - 1; i >= 0; i--)
    {
        const auto& step = steps[i];

        if (step.node && step.node->sourceFile)
            g_Log.print(format("%s", step.node->sourceFile->path.c_str()));
        else if (step.ip && step.ip->node && step.ip->node->sourceFile)
            g_Log.print(format("%s", step.ip->node->sourceFile->path.c_str()));
        else if (step.sourceFile)
            g_Log.print(format("%s", step.sourceFile->path.c_str()));
        else
            g_Log.print("<unknown file>");
        if (step.node)
            g_Log.print(format(":%d: ", step.node->token.startLocation.line + 1));
        else if (step.ip)
            g_Log.print(format(":%d: ", step.ip->node->token.startLocation.line + 1));
        else
            g_Log.print(":<unknown line>: ");

        if (step.message)
            g_Log.print(step.message);
        g_Log.eol();
    }

    g_Log.setDefaultColor();
}

void DiagnosticInfos::reportError(const Utf8& msg)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print(msg);
    g_Log.eol();
    log();
    g_Log.setDefaultColor();
    g_Log.unlock();
}
