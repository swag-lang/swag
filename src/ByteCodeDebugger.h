#pragma once
struct ByteCodeRunContext;

struct ByteCodeDebugger
{
    static bool step(ByteCodeRunContext* context);
};
