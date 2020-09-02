#pragma once
#include "Os.h"
#include "swag_runtime.h"
union Register;
struct ByteCode;
struct Module;

extern uint64_t             g_tlsContextId;
extern swag_context_t       g_defaultContext;
extern swag_process_infos_t g_processInfos;

extern void     initDefaultContext();
extern uint64_t getDefaultContextFlags(Module* module);