#include "pch.h"
#include "Global.h"
#include "ByteCodeGen.h"
#include "ByteCodeRun.h"
#include "ConcatBucket.h"

bool ByteCodeRun::executeNode(SemanticContext* context, AstNode* node)
{
    // First we need to generate byte code
    ByteCodeGenContext genContext;
    genContext.semantic = context;
    ByteCodeGen gen;
    SWAG_CHECK(gen.emitNode(&genContext, node));

    // Then we execute the resulting bytecode
    ByteCodeRunContext runContext;
    runContext.bc              = &genContext.bc;
    runContext.executionOffset = 0;
    runContext.executionBucket = runContext.bc->out.firstBucket;
    run(&runContext);

    return true;
}

bool ByteCodeRun::run(ByteCodeRunContext* context)
{
    uint8_t* ep = context->executionBucket->datas + context->executionOffset;
    while (true)
    {
        ByteCodeNodeId id = (ByteCodeNodeId) * (uint16_t*) ep;
        if (id == ByteCodeNodeId::End)
            break;

        int incEP = 6;

        context->executionOffset += incEP;
        ep += incEP;

        if (context->executionOffset >= context->executionBucket->count)
        {
            context->executionOffset = 0;
            context->executionBucket = context->executionBucket->nextBucket;
            ep                       = context->executionBucket->datas;
        }
    }

    return true;
}
