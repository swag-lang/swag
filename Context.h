#pragma once
struct ByteCode;

struct Context
{
    ByteCode* allocator;
};

extern uint32_t g_tlsContextId;
extern Context  g_defaultContext;
