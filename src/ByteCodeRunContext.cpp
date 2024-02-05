#include "pch.h"
#include "ByteCodeRunContext.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "ByteCodeStack.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Statistics.h"

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
        *(void**) stack  = g_FirstFreeStack;
        g_FirstFreeStack = stack;
        stack            = nullptr;
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
            stack            = (uint8_t*) g_FirstFreeStack;
            g_FirstFreeStack = *(void**) g_FirstFreeStack;
        }
        else
        {
            stack = (uint8_t*) Allocator::alloc(g_CommandLine.limitStackBC);
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

    curRC   = -1;
    firstRC = -1;
}

void ByteCodeRunContext::stackOverflow()
{
    OS::raiseException(SWAG_EXCEPTION_TO_COMPILER_HANDLER, FMT(Err(Err0024), Utf8::toNiceSize(g_CommandLine.limitStackBC).c_str()));
}

int ByteCodeRunContext::getRegCount(int cur)
{
    if ((size_t) cur >= registersRC.size() - 1)
        return (int) bc->maxReservedRegisterRC;
    return (int) (registersRC[cur + 1] - registersRC[cur]);
}
