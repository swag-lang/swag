#include "pch.h"
#include "Global.h"
#include "ByteCodeGen.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "ConcatBucket.h"

void ByteCodeRun::setup()
{
    mapNodes[ByteCodeNodeId::PushS32] = runPushS32;
}

bool ByteCodeRun::executeNode(ByteCodeRunContext* runContext, SemanticContext* context, AstNode* node)
{
    // First we need to generate byte code
    ByteCodeGenContext genContext;
    genContext.semantic = context;
    genContext.bc       = &runContext->bc;
    SWAG_CHECK(ByteCodeGen::emitNode(&genContext, node));

    // Then we execute the resulting bytecode
    runContext->bc.out.rewind();
    runContext->stack.resize(1024);
    runContext->sp = 0;

    run(runContext);

    return true;
}

bool ByteCodeRun::run(ByteCodeRunContext* context)
{
    context->ep = context->bc.out.currentSP;
    while (true)
    {
        ByteCodeNodeId id = (ByteCodeNodeId)(*(uint16_t*) context->ep);
        if (id == ByteCodeNodeId::End)
            break;
        context->ep = context->bc.out.seek(2);
        context->ep = mapNodes[id](context);
    }

    return true;
}
