#include "pch.h"
#include "ByteCodeRunContext.h"
#include "ByteCode.h"
#include "AstNode.h"

ByteCodeRunContext::~ByteCodeRunContext()
{
    if (g_Global.exiting)
        return;

    ::free(registersRR);
    ::free(stack);
}

void ByteCodeRunContext::setup(SourceFile* sf, AstNode* nd)
{
    auto stackS = max(g_CommandLine.stackSize, 1024);

    if (!registersRR)
    {
        registersRR = (Register*) malloc(MAX_ALLOC_RR * sizeof(Register));
        if (g_CommandLine.devMode)
            memset(registersRR, 0xFE, MAX_ALLOC_RR * sizeof(Register));
        if (g_CommandLine.stats)
            g_Stats.memBcStack += MAX_ALLOC_RR * sizeof(Register);
    }

    if (stackSize < stackS)
    {
        stackSize = stackS;
        stack     = (uint8_t*) realloc(stack, stackS);
        if (g_CommandLine.stats)
            g_Stats.memBcStack += stackS;
        if (g_CommandLine.devMode)
            memset(stack, 0xFE, stackS);
    }

    bp = stack + stackSize;
    sp = bp;

    sourceFile = sf;
    node       = nd;
    bc         = node->extension->bc;
    SWAG_ASSERT(bc);
    ip = bc->out;
    SWAG_ASSERT(ip);

    curRC    = -1;
    firstRC  = -1;
    hasError = false;
    errorLoc = nullptr;
    errorMsg.clear();
}

void ByteCodeRunContext::error(const Utf8& msg, ConcreteCompilerSourceLocation* loc)
{
    hasError = true;
    errorLoc = loc;
    errorMsg = msg;
}

void ByteCodeRunContext::stackOverflow()
{
    hasError = true;
    errorMsg = format("bytecode stack overflow (maximum stack size is '--stack-size:%s')", toNiceSize(stackSize).c_str());
}