#include "pch.h"
#include "Global.h"
#include "ByteCodeGen.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "ConcatBucket.h"
#include "Diagnostic.h"

void ByteCodeRun::setup()
{
    mapNodes[ByteCodeNodeId::PushBool] = runPushBool;
    mapNodes[ByteCodeNodeId::PushS8]   = runPushS8;
    mapNodes[ByteCodeNodeId::PushS16]  = runPushS16;
    mapNodes[ByteCodeNodeId::PushS32]  = runPushS32;
    mapNodes[ByteCodeNodeId::PushS64]  = runPushS64;
    mapNodes[ByteCodeNodeId::PushU8]   = runPushU8;
    mapNodes[ByteCodeNodeId::PushU16]  = runPushU16;
    mapNodes[ByteCodeNodeId::PushU32]  = runPushU32;
    mapNodes[ByteCodeNodeId::PushU64]  = runPushU64;
    mapNodes[ByteCodeNodeId::PushF32]  = runPushF32;
    mapNodes[ByteCodeNodeId::PushF64]  = runPushF64;

    mapNodes[ByteCodeNodeId::BinOpPlusS32]  = runBinOpPlusS32;
    mapNodes[ByteCodeNodeId::BinOpPlusS64]  = runBinOpPlusS64;
    mapNodes[ByteCodeNodeId::BinOpPlusU32]  = runBinOpPlusU32;
    mapNodes[ByteCodeNodeId::BinOpPlusU64]  = runBinOpPlusU64;
    mapNodes[ByteCodeNodeId::BinOpPlusF32]  = runBinOpPlusF32;
    mapNodes[ByteCodeNodeId::BinOpPlusF64]  = runBinOpPlusF64;

    mapNodes[ByteCodeNodeId::BinOpMinusS32] = runBinOpMinusS32;
    mapNodes[ByteCodeNodeId::BinOpMinusS64] = runBinOpMinusS64;
    mapNodes[ByteCodeNodeId::BinOpMinusU32] = runBinOpMinusU32;
    mapNodes[ByteCodeNodeId::BinOpMinusU64] = runBinOpMinusU64;
    mapNodes[ByteCodeNodeId::BinOpMinusF32] = runBinOpMinusF32;
    mapNodes[ByteCodeNodeId::BinOpMinusF64] = runBinOpMinusF64;

    mapNodes[ByteCodeNodeId::BinOpMulS32]   = runBinOpMulS32;
    mapNodes[ByteCodeNodeId::BinOpMulS64]   = runBinOpMulS64;
    mapNodes[ByteCodeNodeId::BinOpMulU32]   = runBinOpMulU32;
    mapNodes[ByteCodeNodeId::BinOpMulU64]   = runBinOpMulU64;
    mapNodes[ByteCodeNodeId::BinOpMulF32]   = runBinOpMulF32;
    mapNodes[ByteCodeNodeId::BinOpMulF64]   = runBinOpMulF64;
}

bool ByteCodeRun::executeNode(ByteCodeRunContext* runContext, SemanticContext* semanticContext, AstNode* node)
{
    // First we need to generate byte code
    ByteCodeGenContext genContext;
    genContext.semantic = semanticContext;
    genContext.bc       = &runContext->bc;
    SWAG_CHECK(ByteCodeGen::emitNode(&genContext, node));

    // Then we execute the resulting bytecode
    runContext->node       = node;
    runContext->sourceFile = semanticContext->sourceFile;
    runContext->bc.out.rewind();
    runContext->stack.resize(1024);
    runContext->sp = 0;

    run(runContext);

    return true;
}

bool ByteCodeRun::run(ByteCodeRunContext* context)
{
    auto& out   = context->bc.out;
    context->ep = context->bc.out.currentSP;
    while (true)
    {
        // Get instruction
        ByteCodeNodeId id = (ByteCodeNodeId)(*(uint16_t*) context->ep);
        if (id == ByteCodeNodeId::End)
            break;
        context->ep = out.seek(2);
        auto it     = mapNodes.find(id);
        if (it == mapNodes.end())
            return internalError(context);
        context->ep = it->second(context);
    }

    return true;
}

bool ByteCodeRun::internalError(ByteCodeRunContext* context)
{
    context->sourceFile->report({context->sourceFile, context->node->token, "internal compiler error during bytecode execution"});
    return false;
}