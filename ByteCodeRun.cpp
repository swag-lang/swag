#include "pch.h"
#include "Global.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "Diagnostic.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "ByteCode.h"

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

    mapNodes[ByteCodeNodeId::BinOpPlusS32] = runBinOpPlusS32;
    mapNodes[ByteCodeNodeId::BinOpPlusS64] = runBinOpPlusS64;
    mapNodes[ByteCodeNodeId::BinOpPlusU32] = runBinOpPlusU32;
    mapNodes[ByteCodeNodeId::BinOpPlusU64] = runBinOpPlusU64;
    mapNodes[ByteCodeNodeId::BinOpPlusF32] = runBinOpPlusF32;
    mapNodes[ByteCodeNodeId::BinOpPlusF64] = runBinOpPlusF64;

    mapNodes[ByteCodeNodeId::BinOpMinusS32] = runBinOpMinusS32;
    mapNodes[ByteCodeNodeId::BinOpMinusS64] = runBinOpMinusS64;
    mapNodes[ByteCodeNodeId::BinOpMinusU32] = runBinOpMinusU32;
    mapNodes[ByteCodeNodeId::BinOpMinusU64] = runBinOpMinusU64;
    mapNodes[ByteCodeNodeId::BinOpMinusF32] = runBinOpMinusF32;
    mapNodes[ByteCodeNodeId::BinOpMinusF64] = runBinOpMinusF64;

    mapNodes[ByteCodeNodeId::BinOpMulS32] = runBinOpMulS32;
    mapNodes[ByteCodeNodeId::BinOpMulS64] = runBinOpMulS64;
    mapNodes[ByteCodeNodeId::BinOpMulU32] = runBinOpMulU32;
    mapNodes[ByteCodeNodeId::BinOpMulU64] = runBinOpMulU64;
    mapNodes[ByteCodeNodeId::BinOpMulF32] = runBinOpMulF32;
    mapNodes[ByteCodeNodeId::BinOpMulF64] = runBinOpMulF64;

    mapNodes[ByteCodeNodeId::BinOpDivF32] = runBinOpDivF32;
    mapNodes[ByteCodeNodeId::BinOpDivF64] = runBinOpDivF64;

    mapNodes[ByteCodeNodeId::LocalFuncCall] = runLocalFuncCall;
    mapNodes[ByteCodeNodeId::Ret]           = runRet;
}

bool ByteCodeRun::run(ByteCodeRunContext* context)
{
    context->ep = context->bc->out.currentSP;

    uint32_t       nodeSourceFileIdx = UINT32_MAX;
    SourceLocation startNodeLocation;
    SourceLocation endNodeLocation;

    while (true)
    {
        // Get instruction
        ByteCodeNodeId id = (ByteCodeNodeId)(*(uint16_t*) context->ep);
        if (id == ByteCodeNodeId::End)
            break;

        // Debug informations
        auto& out = context->bc->out;
        if (id == ByteCodeNodeId::DebugInfos)
        {
            context->ep = out.seek(sizeof(uint16_t));

            nodeSourceFileIdx = (*(uint32_t*) context->ep);
            context->ep       = out.seek(sizeof(uint32_t));

            startNodeLocation.line          = (*(uint32_t*) context->ep);
            context->ep                     = out.seek(sizeof(uint32_t));
            startNodeLocation.column        = (*(uint32_t*) context->ep);
            context->ep                     = out.seek(sizeof(uint32_t));
            startNodeLocation.seekStartLine = (*(uint32_t*) context->ep);
            context->ep                     = out.seek(sizeof(uint32_t));

            endNodeLocation.line          = (*(uint32_t*) context->ep);
            context->ep                   = out.seek(sizeof(uint32_t));
            endNodeLocation.column        = (*(uint32_t*) context->ep);
            context->ep                   = out.seek(sizeof(uint32_t));
            endNodeLocation.seekStartLine = (*(uint32_t*) context->ep);
            context->ep                   = out.seek(sizeof(uint32_t));
            continue;
        }

        context->ep = out.seek(sizeof(uint16_t));
        auto it     = mapNodes.find(id);
        if (it == mapNodes.end())
            return internalError(context);

        // Execute instruction
        context->ep = it->second(context);

        // Error ?
        if (context->hasError)
        {
            return context->sourceFile->report({context->sourceFile, startNodeLocation, endNodeLocation, context->errorMsg});
        }
    }

    return true;
}

bool ByteCodeRun::internalError(ByteCodeRunContext* context)
{
    context->sourceFile->report({context->sourceFile, context->node->token, "internal compiler error during bytecode execution"});
    return false;
}
