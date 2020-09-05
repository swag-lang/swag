#pragma once
#include "CommandLine.h"
struct SourceFile;
struct AstNode;
struct Utf8;
struct ByteCodeInstruction;

struct ByteCodeStackStep
{
    const char*          message    = nullptr;
    SourceFile*          sourceFile = nullptr;
    AstNode*             node       = nullptr;
    ByteCodeInstruction* ip         = nullptr;
};

struct ByteCodeStack
{
    void push(ByteCodeStackStep& step)
    {
        steps.emplace_back(step);
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
    }

    void reportError(const Utf8& msg);
    void log();

    vector<ByteCodeStackStep> steps;
};

extern thread_local ByteCodeStack g_byteCodeStack;
