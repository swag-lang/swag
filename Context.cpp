#include "pch.h"
#include "Os.h"
#include "Context.h"
#include "Workspace.h"
#include "ByteCodeRun.h"
#include "ByteCode.h"
#include "AstNode.h"
#include "Global.h"
#include "Module.h"
#include "ByteCodeStack.h"

uint64_t         g_tlsContextId = 0;
SwagContext      g_defaultContext;
SwagProcessInfos g_processInfos = {0};

static void byteCodeRun(void* byteCodePtr, ...)
{
    ByteCode*         bc       = (ByteCode*) undoByteCodeLambda(byteCodePtr);
    TypeInfoFuncAttr* typeFunc = CastTypeInfo<TypeInfoFuncAttr>(bc->node->typeInfo, TypeInfoKind::FuncAttr);

    VectorNative<Register*> returnRegisters;
    VectorNative<Register*> paramRegisters;
    va_list                 valist;

    // @todo, unsupported yet
    SWAG_ASSERT(!typeFunc->numReturnRegisters());

    va_start(valist, byteCodePtr);
    for (int i = 0; i < typeFunc->numReturnRegisters(); i++)
    {
        auto r = va_arg(valist, Register*);
        returnRegisters.push_back(r);
    }

    ByteCodeRunContext runContext;
    auto               node   = bc->node;
    auto               module = node->sourceFile->module;
    runContext.setup(node->sourceFile, node, module->runContext.stackSize);

    // Parameters
    for (int i = 0; i < typeFunc->numParamsRegisters(); i++)
    {
        auto r = va_arg(valist, Register*);
        paramRegisters.push_back(r);
    }

    while (!paramRegisters.empty())
    {
        auto r = paramRegisters.back();
        paramRegisters.pop_back();
        runContext.push(r->pointer);
    }

    // Dummy 24 bytes on the stack necessary before a call
    runContext.push(nullptr);
    runContext.push(nullptr);
    runContext.push(nullptr);

    // Run !
    runContext.bp = runContext.sp;
    bc->addCallStack(&runContext);
    bc->enterByteCode(&runContext);
    module->runner.run(&runContext);
}

static void threadRun(void* param)
{
    void** arr = (void**) param;
    byteCodeRun(arr[0], &param);
}

void initDefaultContext()
{
    g_tlsContextId = OS::tlsAlloc();
    OS::tlsSetValue(g_tlsContextId, &g_defaultContext);

    g_processInfos.arguments.addr  = g_CommandLine.userArgumentsSlice.first;
    g_processInfos.arguments.count = (uint64_t) g_CommandLine.userArgumentsSlice.second;
    g_processInfos.contextTlsId    = g_tlsContextId;
    g_processInfos.defaultContext  = &g_defaultContext;
    g_processInfos.byteCodeRun     = byteCodeRun;
    g_processInfos.threadRun       = threadRun;
}

uint64_t getDefaultContextFlags(Module* module)
{
    uint64_t flags = 0;
    if (module->fromTestsFolder)
        flags |= (uint64_t) ContextFlags::Test;
    if (g_CommandLine.devMode)
        flags |= (uint64_t) ContextFlags::DevMode;
    return flags;
}