#include "pch.h"
#include "ByteCodeRunContext.h"
#include "ByteCode.h"
#include "AstNode.h"

ByteCodeRunContext::~ByteCodeRunContext()
{
    free(registersRR);
    free(stack);
}

void ByteCodeRunContext::setup(SourceFile* sf, AstNode* nd, uint32_t stackS)
{
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
    bc         = node->bc;
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
