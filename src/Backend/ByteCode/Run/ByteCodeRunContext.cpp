#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Run/ByteCodeRunContext.h"
#include "Backend/ByteCode/Run/ByteCodeStack.h"
#include "Main/Statistics.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"

Mutex g_FreeStackMutex;
void* g_FirstFreeStack = nullptr;

ByteCodeRunContext::~ByteCodeRunContext()
{
    if (g_Exiting)
        return;
    releaseStack();
}

void ByteCodeRunContext::releaseStack()
{
    if (stack && !sharedStack)
    {
        // To avoid wasting memory, we recycle bytecode stacks
        ScopedLock lk(g_FreeStackMutex);
        *reinterpret_cast<void**>(stack) = g_FirstFreeStack;
        g_FirstFreeStack                 = stack;
        stack                            = nullptr;
    }
}

void ByteCodeRunContext::setup(SourceFile* sf, AstNode* nd, ByteCode* nodeBC)
{
#ifdef SWAG_DEV_MODE
    memset(registersRR, 0xFE, MAX_ALLOC_RR * sizeof(Register));
#endif

    if (!stack)
    {
        // To avoid wasting memory, we recycle bytecode stacks
        ScopedLock lk(g_FreeStackMutex);
        if (g_FirstFreeStack)
        {
            stack            = static_cast<uint8_t*>(g_FirstFreeStack);
            g_FirstFreeStack = *static_cast<void**>(g_FirstFreeStack);
        }
        else
        {
            stack = Allocator::alloc_n<uint8_t>(g_CommandLine.limitStackBC);
#ifdef SWAG_STATS
            g_Stats.memBcStack += g_CommandLine.limitStackBC;
#endif
        }
    }

    jc.sourceFile = sf;
    node          = nd;

    bp    = stack + g_CommandLine.limitStackBC;
    sp    = bp;
    spAlt = stack;
    bc    = nodeBC;
    ip    = bc->out;
    SWAG_ASSERT(ip);

    registers.reserve(4096);
    registersRC.reserve(1024);
    registers.count   = 0;
    registersRC.count = 0;
    g_ByteCodeStackTrace->steps.reserve(4096);

    maxRecurse          = g_CommandLine.limitRecurseBC;
    internalPanicSymbol = nullptr;
    internalPanicHint.clear();

    curRC   = UINT32_MAX;
    firstRC = UINT32_MAX;
}

void ByteCodeRunContext::stackOverflow()
{
    OS::raiseException(SWAG_EXCEPTION_TO_COMPILER_HANDLER, formErr(Err0024, Utf8::toNiceSize(g_CommandLine.limitStackBC).c_str()));
}

uint32_t ByteCodeRunContext::getRegCount(uint32_t cur)
{
    if (static_cast<size_t>(cur) >= registersRC.size() - 1)
        return bc->maxReservedRegisterRC;
    return registersRC[cur + 1] - registersRC[cur];
}
