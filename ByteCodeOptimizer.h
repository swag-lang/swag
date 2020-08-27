#pragma once
struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeInstruction;

struct ByteCodeOptContext
{
    ByteCode*                          bc;
    VectorNative<ByteCodeInstruction*> jumps;
    VectorNative<ByteCodeInstruction*> nops;
};

struct ByteCodeOptimizer
{
    static bool optimizeJumps(ByteCodeOptContext* context);
    static void optimize(ByteCodeGenContext* context);
};