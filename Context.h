#pragma once
#include "Os.h"
#include "swag_runtime.h"
union Register;
struct ByteCode;
struct Module;

extern uint64_t         g_tlsContextId;
extern SwagContext      g_defaultContext;
extern SwagProcessInfos g_processInfos;

extern void     initDefaultContext();
extern uint64_t getDefaultContextFlags(Module* module);