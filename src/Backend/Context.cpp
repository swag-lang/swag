#include "pch.h"
#include "Backend/Context.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Report/Diagnostic.h"
#include "Semantic/Type/TypeInfo.h"
#include "Wmf/Module.h"
#ifdef SWAG_DEV_MODE
#include "Wmf/ModuleManager.h"
#endif

SwagContext                      g_DefaultContext;
SwagProcessInfos                 g_ProcessInfos;
uint64_t                         g_TlsContextId         = 0;
uint64_t                         g_TlsThreadLocalId     = 0;
void*                            g_SystemAllocatorTable = nullptr;
void*                            g_DebugAllocatorTable  = nullptr;
thread_local ByteCodeRunContext  g_RunContextVal;
thread_local ByteCodeRunContext* g_RunContext = &g_RunContextVal;

namespace
{
    Mutex    g_MakeCallbackMutex;
    uint32_t g_MakeCallbackCount = 0;

    void byteCodeRun(bool /*forCallback*/, void* byteCodePtr, va_list vaList)
    {
#ifdef SWAG_DEV_MODE
        SWAG_ASSERT(byteCodePtr != g_SwagPatchMarker);
#endif
        const auto        bc       = static_cast<ByteCode*>(ByteCode::undoByteCodeLambda(byteCodePtr));
        TypeInfoFuncAttr* typeFunc = castTypeInfo<TypeInfoFuncAttr>(bc->node ? bc->node->typeInfo : bc->typeInfoFunc, TypeInfoKind::FuncAttr);

        VectorNative<Register*> returnRegisters;
        VectorNative<Register*> paramRegisters;
        VectorNative<Register>  fakeRegisters;

        for (uint32_t i = 0; i < typeFunc->numReturnRegisters(); i++)
        {
            auto r = va_arg(vaList, Register*);
            returnRegisters.push_back(r);
        }

        const auto saveNode       = g_RunContext->node;
        const auto saveSourceFile = g_RunContext->jc.sourceFile;

        const auto node = bc->node ? bc->node : saveNode;
        SWAG_ASSERT(node);

        const auto module         = node->token.sourceFile->module;
        bool       stackAllocated = false;

        if (!g_RunContext->stack)
        {
            SWAG_ASSERT(node->hasExtByteCode() && node->extByteCode()->bc);
            g_RunContext->setup(node->token.sourceFile, node, node->extByteCode()->bc);
            stackAllocated = true;
        }
        else
        {
            g_RunContext->jc.sourceFile = node->token.sourceFile;
            g_RunContext->node          = node;
        }

        // Parameters
        // As we have values/registers values, we need to make a copy in a temporary register
        fakeRegisters.reserve(typeFunc->numParamsRegisters());
        for (uint32_t i = 0; i < typeFunc->numParamsRegisters(); i++)
        {
            fakeRegisters.count++;
            auto& r = fakeRegisters.back();
            r.u64   = va_arg(vaList, uint64_t);
            paramRegisters.push_back(&fakeRegisters.back());
        }

        const auto saveSp      = g_RunContext->sp;
        const auto saveSpAlt   = g_RunContext->spAlt;
        const auto saveFirstRC = g_RunContext->firstRC;

        while (!paramRegisters.empty())
        {
            const auto r = paramRegisters.back();
            paramRegisters.pop_back();
            g_RunContext->push(r->pointer);
        }

        // Return value, in case of an address to the result
        if (CallConv::returnByStackAddress(typeFunc))
            g_RunContext->registersRR[0] = *returnRegisters[0];

        // Simulate a LocalCall
        g_RunContext->push(g_RunContext->bp);
        g_RunContext->push(g_RunContext->bc);
        g_RunContext->push(g_RunContext->ip);
        g_RunContext->oldBc   = nullptr;
        g_RunContext->bc      = bc;
        g_RunContext->ip      = bc->out;
        g_RunContext->bp      = g_RunContext->sp;
        g_RunContext->firstRC = g_RunContext->curRC;
        ByteCodeRun::enterByteCode(g_RunContext, g_RunContext->bc);

        g_ByteCodeStackTrace->push({nullptr, nullptr});
        const auto result = module->runner.run(g_RunContext);

        g_RunContext->sp            = saveSp;
        g_RunContext->spAlt         = saveSpAlt;
        g_RunContext->node          = saveNode;
        g_RunContext->jc.sourceFile = saveSourceFile;
        g_RunContext->firstRC       = saveFirstRC;

        // Get result
        for (uint32_t i = 0; i < returnRegisters.size(); i++)
        {
            const auto r = returnRegisters[i];
            *r           = g_RunContext->registersRR[i];
        }

        if (stackAllocated)
            g_RunContext->releaseStack();

        if (!result)
            OS::raiseException(SWAG_EXCEPTION_TO_PREV_HANDLER);
    }

    void byteCodeRun(void* byteCodePtr, ...)
    {
        va_list vaList;
        va_start(vaList, byteCodePtr);
        byteCodeRun(false, byteCodePtr, vaList);
    }

    void byteCodeRunCB(void* byteCodePtr, ...)
    {
        va_list vaList;
        va_start(vaList, byteCodePtr);
        byteCodeRun(true, byteCodePtr, vaList);
    }

    // Callback stuff. This is tricky !
    // The problem: we want an external library to be able to call a callback defined in swag.
    // When swag code is native, no problem.
    // The problem starts when that callback is bytecode, at compile time.
    // We need a way to associate a native function that will be called by the external library to a bytecode.
    //
    // The only way I have found : make the native function generic (always the same parameters), and attach
    // the address of that function to bytecode. One native callback for one specific bytecode.
    // That's why we have here lots of native identical functions, which are dynamically associated to a bytecode
    // when calling @mkcallback

    void* doCallback(FuncCB cb, void* p1, void* p2, void* p3, void* p4, void* p5, void* p6, void* p7, void* p8, void* p9, void* p10);

    struct Callback
    {
        void*  bytecode;
        FuncCB cb;
    };

#define DECL_CB(__idx)                                                                                                           \
    void* __callback##__idx(void* p1, void* p2, void* p3, void* p4, void* p5, void* p6, void* p7, void* p8, void* p9, void* p10) \
    {                                                                                                                            \
        return doCallback(&__callback##__idx, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);                                          \
    }

#define USE_CB(__idx)              \
    {                              \
        nullptr, __callback##__idx \
    }

    // WARNING WARNING WARNING WARNING WARNING WARNING
    // WARNING WARNING WARNING WARNING WARNING WARNING
    // WARNING WARNING WARNING WARNING WARNING WARNING

    // If /OPT:ICF is enabled in the linker (COMDAT FOLDING), this will not work
    // as the linker will make only one __callback function, and all pointers in
    // g_CallbackArr will be the same !!!

    DECL_CB(a);
    DECL_CB(b);
    DECL_CB(c);
    DECL_CB(d);
    DECL_CB(e);
    DECL_CB(f);
    DECL_CB(g);
    DECL_CB(h);
    DECL_CB(i);
    DECL_CB(aa);
    DECL_CB(ab);
    DECL_CB(ac);
    DECL_CB(ad);
    DECL_CB(ae);
    DECL_CB(af);
    DECL_CB(ag);
    DECL_CB(ah);
    DECL_CB(ai);
    DECL_CB(aaa);
    DECL_CB(aab);
    DECL_CB(aac);
    DECL_CB(aad);
    DECL_CB(aae);
    DECL_CB(aaf);
    DECL_CB(aag);
    DECL_CB(aah);
    DECL_CB(aai);

    Callback g_CallbackArr[] = {
        USE_CB(a),
        USE_CB(b),
        USE_CB(c),
        USE_CB(d),
        USE_CB(e),
        USE_CB(f),
        USE_CB(g),
        USE_CB(h),
        USE_CB(i),
        USE_CB(aa),
        USE_CB(ab),
        USE_CB(ac),
        USE_CB(ad),
        USE_CB(ae),
        USE_CB(af),
        USE_CB(ag),
        USE_CB(ah),
        USE_CB(ai),
        USE_CB(aaa),
        USE_CB(aab),
        USE_CB(aac),
        USE_CB(aad),
        USE_CB(aae),
        USE_CB(aaf),
        USE_CB(aag),
        USE_CB(aah),
        USE_CB(aai),
    };

    // This is the actual callback that will be called by external libraries
    void* doCallback(FuncCB cb, void* p1, void* p2, void* p3, void* p4, void* p5, void* p6, void* p7, void* p8, void* p9, void* p10)
    {
        uint32_t cbIndex = UINT32_MAX;

        // Find the slot that corresponds to the native callback, because
        // we need the bytecode pointer that matches 'cb'
        {
            ScopedLock lk(g_MakeCallbackMutex);
            for (uint32_t i = 0; i < g_MakeCallbackCount; i++)
            {
                if (g_CallbackArr[i].cb == cb)
                {
                    cbIndex = i;
                    break;
                }
            }
        }

        SWAG_ASSERT(cbIndex != UINT32_MAX);

        void*                   result   = nullptr;
        const ByteCode*         bc       = static_cast<ByteCode*>(ByteCode::undoByteCodeLambda(g_CallbackArr[cbIndex].bytecode));
        const TypeInfoFuncAttr* typeFunc = castTypeInfo<TypeInfoFuncAttr>(bc->node->typeInfo, TypeInfoKind::FuncAttr);
        SWAG_ASSERT(typeFunc->numReturnRegisters() <= 1);

        if (typeFunc->numReturnRegisters())
            byteCodeRunCB(g_CallbackArr[cbIndex].bytecode, &result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        else
            byteCodeRunCB(g_CallbackArr[cbIndex].bytecode, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);

        return result;
    }
}

// Runtime function called by user code
FuncCB makeCallback(void* lambda)
{
    ScopedLock lk(g_MakeCallbackMutex);

    // Search if the lambda pointer has already been associated with a given callback
    for (uint32_t i = 0; i < g_MakeCallbackCount; i++)
    {
        if (g_CallbackArr[i].bytecode == lambda)
            return g_CallbackArr[i].cb;
    }

    // No more room
    if (g_MakeCallbackCount == std::size(g_CallbackArr))
        return nullptr;

    // Use a new slot
    g_CallbackArr[g_MakeCallbackCount].bytecode = lambda;
    return g_CallbackArr[g_MakeCallbackCount++].cb;
}

void initDefaultContext()
{
    g_TlsContextId     = OS::tlsAlloc();
    g_TlsThreadLocalId = OS::tlsAlloc();
    OS::tlsSetValue(g_TlsContextId, &g_DefaultContext);

    g_ProcessInfos.modules.buffer = nullptr;
    g_ProcessInfos.modules.count  = 0;

    static Utf8 args;
    args += g_CommandLine.exePathStr;
    args += " ";
    args += g_CommandLine.userArguments;

    g_ProcessInfos.args.buffer = args.data();
    g_ProcessInfos.args.count  = static_cast<uint64_t>(args.length());

    g_ProcessInfos.contextTlsId   = g_TlsContextId;
    g_ProcessInfos.defaultContext = &g_DefaultContext;
    g_ProcessInfos.byteCodeRun    = byteCodeRun;
    g_ProcessInfos.makeCallback   = makeCallback;
    g_ProcessInfos.backendKind    = SwagBackendGenType::ByteCode;
}

uint64_t getDefaultContextFlags(const Module* module)
{
    uint64_t flags = 0;
    if (module->is(ModuleKind::Test))
        flags |= static_cast<uint64_t>(ContextFlags::Test);
    return flags;
}
