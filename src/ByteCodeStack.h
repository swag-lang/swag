#pragma once
#include "CommandLine.h"
struct ByteCode;
struct ByteCodeInstruction;
struct ByteCodeRunContext;

struct ByteCodeStackStep
{
    ByteCode*            bc = nullptr;
    ByteCodeInstruction* ip = nullptr;
};

struct ByteCodeStack
{
    void push(const ByteCodeStackStep& step)
    {
        steps.push_back(step);
    }

    void pop()
    {
        if (!steps.empty())
            steps.pop_back();
    }

    ByteCodeStackStep& last()
    {
        return steps.back();
    }

    void clear()
    {
        steps.clear();
        currentContext = nullptr;
    }

    void     reportError(const Utf8& msg);
    uint32_t maxLevel(ByteCodeRunContext* context);
    void     logStep(int level, bool current, ByteCodeStackStep& step);
    void     log();

    vector<ByteCodeStackStep> steps;
    ByteCodeRunContext*       currentContext = nullptr;
};

extern thread_local ByteCodeStack g_byteCodeStack;
