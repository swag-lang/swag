#pragma once
struct ByteCode;
struct ByteCodeGenContext;

struct ByteCodeOptContext
{
    ByteCode* bc;
};

struct ByteCodeOptimizer
{
    static bool optimizeJumps(ByteCodeOptContext* context);
    static void optimize(ByteCodeGenContext* context);
};