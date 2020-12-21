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
#include "ByteCodeOp.h"

uint64_t                        g_tlsContextId = 0;
SwagContext                     g_defaultContext;
SwagProcessInfos                g_processInfos = {0};
thread_local ByteCodeRunContext g_runContext;

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

    auto saveNode       = g_runContext.node;
    auto saveSourceFile = g_runContext.sourceFile;

    auto node   = bc->node;
    auto module = node->sourceFile->module;
    if (!g_runContext.stack)
        g_runContext.setup(node->sourceFile, node, module->buildParameters.buildCfg->byteCodeStackSize);
    else
    {
        g_runContext.sourceFile = node->sourceFile;
        g_runContext.node       = node;
    }

    // Parameters
    for (int i = 0; i < typeFunc->numParamsRegisters(); i++)
    {
        auto r = va_arg(valist, Register*);
        paramRegisters.push_back(r);
    }

    auto saveSp      = g_runContext.sp;
    auto saveFirstRC = g_runContext.firstRC;

    while (!paramRegisters.empty())
    {
        auto r = paramRegisters.back();
        paramRegisters.pop_back();
        g_runContext.push(r->pointer);
    }

    // Simulate a LocalCall
    g_runContext.push(g_runContext.bp);
    g_runContext.push(g_runContext.bc);
    g_runContext.push(g_runContext.ip);
    g_runContext.bc      = bc;
    g_runContext.ip      = bc->out;
    g_runContext.bp      = g_runContext.sp;
    g_runContext.firstRC = g_runContext.curRC;
    g_runContext.bc->enterByteCode(&g_runContext);

    module->runner.run(&g_runContext);

    g_runContext.sp         = saveSp;
    g_runContext.node       = saveNode;
    g_runContext.sourceFile = saveSourceFile;
    g_runContext.firstRC    = saveFirstRC;
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
    if (module->kind == ModuleKind::Test)
        flags |= (uint64_t) ContextFlags::Test;
    if (g_CommandLine.devMode)
        flags |= (uint64_t) ContextFlags::DevMode;
    return flags;
}