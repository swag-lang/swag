#pragma once
#include "Os.h"
#include "Runtime.h"
union Register;
struct ByteCode;
struct Module;

extern uint64_t         g_tlsContextId;
extern SwagContext      g_defaultContext;
extern SwagProcessInfos g_processInfos;

extern void     initDefaultContext();
extern uint64_t getDefaultContextFlags(Module* module);
extern void*    makeCallback(void* lambda);

struct PushSwagContext
{
    PushSwagContext()
    {
        copy = g_defaultContext;
        OS::tlsSetValue(g_tlsContextId, &copy);
    }

    ~PushSwagContext()
    {
        OS::tlsSetValue(g_tlsContextId, &g_defaultContext);
    }

    SwagContext copy;
};