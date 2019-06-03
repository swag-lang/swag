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
    mapNodes[ByteCodeOp::PushBool]   = runPushBool;
    mapNodes[ByteCodeOp::PushS8]     = runPushS8;
    mapNodes[ByteCodeOp::PushS16]    = runPushS16;
    mapNodes[ByteCodeOp::PushS32]    = runPushS32;
    mapNodes[ByteCodeOp::PushS64]    = runPushS64;
    mapNodes[ByteCodeOp::PushU8]     = runPushU8;
    mapNodes[ByteCodeOp::PushU16]    = runPushU16;
    mapNodes[ByteCodeOp::PushU32]    = runPushU32;
    mapNodes[ByteCodeOp::PushU64]    = runPushU64;
    mapNodes[ByteCodeOp::PushF32]    = runPushF32;
    mapNodes[ByteCodeOp::PushF64]    = runPushF64;
    mapNodes[ByteCodeOp::PushString] = runPushString;

    mapNodes[ByteCodeOp::BinOpPlusS32] = runBinOpPlusS32;
    mapNodes[ByteCodeOp::BinOpPlusS64] = runBinOpPlusS64;
    mapNodes[ByteCodeOp::BinOpPlusU32] = runBinOpPlusU32;
    mapNodes[ByteCodeOp::BinOpPlusU64] = runBinOpPlusU64;
    mapNodes[ByteCodeOp::BinOpPlusF32] = runBinOpPlusF32;
    mapNodes[ByteCodeOp::BinOpPlusF64] = runBinOpPlusF64;

    mapNodes[ByteCodeOp::BinOpMinusS32] = runBinOpMinusS32;
    mapNodes[ByteCodeOp::BinOpMinusS64] = runBinOpMinusS64;
    mapNodes[ByteCodeOp::BinOpMinusU32] = runBinOpMinusU32;
    mapNodes[ByteCodeOp::BinOpMinusU64] = runBinOpMinusU64;
    mapNodes[ByteCodeOp::BinOpMinusF32] = runBinOpMinusF32;
    mapNodes[ByteCodeOp::BinOpMinusF64] = runBinOpMinusF64;

    mapNodes[ByteCodeOp::BinOpMulS32] = runBinOpMulS32;
    mapNodes[ByteCodeOp::BinOpMulS64] = runBinOpMulS64;
    mapNodes[ByteCodeOp::BinOpMulU32] = runBinOpMulU32;
    mapNodes[ByteCodeOp::BinOpMulU64] = runBinOpMulU64;
    mapNodes[ByteCodeOp::BinOpMulF32] = runBinOpMulF32;
    mapNodes[ByteCodeOp::BinOpMulF64] = runBinOpMulF64;

    mapNodes[ByteCodeOp::BinOpDivF32] = runBinOpDivF32;
    mapNodes[ByteCodeOp::BinOpDivF64] = runBinOpDivF64;

    mapNodes[ByteCodeOp::LocalFuncCall] = runLocalFuncCall;
    mapNodes[ByteCodeOp::Ret]           = runRet;

    mapNodes[ByteCodeOp::IntrinsicPrintS64]    = runIntrinsicPrintS64;
    mapNodes[ByteCodeOp::IntrinsicPrintF64]    = runIntrinsicPrintF64;
    mapNodes[ByteCodeOp::IntrinsicPrintChar]   = runIntrinsicPrintChar;
    mapNodes[ByteCodeOp::IntrinsicPrintString] = runIntrinsicPrintString;
    mapNodes[ByteCodeOp::IntrinsicAssert]      = runIntrinsicAssert;
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
        ByteCodeOp id = (ByteCodeOp)(*(uint16_t*) context->ep);
        if (id == ByteCodeOp::End)
            break;

        // Debug informations
        auto& out = context->bc->out;
        if (id == ByteCodeOp::DebugInfos)
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
