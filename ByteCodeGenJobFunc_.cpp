#include "pch.h"
#include "ThreadManager.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "SourceFile.h"
#include "ByteCode.h"
#include "Ast.h"
#include "SymTable.h"
#include "TypeManager.h"

bool ByteCodeGenJob::emitLocalFuncDecl(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    int  countRR  = 0;

    // Reserve one RR register for each return value
    if (typeInfo->returnType != g_TypeMgr.typeInfoVoid)
        countRR++;

    // And one per parameter
    countRR += (int) typeInfo->parameters.size();

    context->sourceFile->module->reserveRegisterRR(countRR);

    if (node->stackSize)
        emitInstruction(context, ByteCodeOp::IncSP)->a.s32 = node->stackSize;
    emitInstruction(context, ByteCodeOp::Ret);
    return true;
}

bool ByteCodeGenJob::emitFuncCallParam(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitFuncCallParams(ByteCodeGenContext* context)
{
    return true;
}

bool ByteCodeGenJob::emitReturn(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    // Copy result to RR0... registers
    if (!node->childs.empty())
    {
        auto child = node->childs[0];
        assert(child->typeInfo->kind == TypeInfoKind::Native);
        emitInstruction(context, ByteCodeOp::CopyRRxRCx, 0, child->resultRegisterRC);
    }

    assert(node->ownerFct);
    if (node->ownerFct->stackSize)
        emitInstruction(context, ByteCodeOp::IncSP)->a.s32 = node->ownerFct->stackSize;
    emitInstruction(context, ByteCodeOp::Ret);
    return true;
}

bool ByteCodeGenJob::emitIntrinsic(ByteCodeGenContext* context)
{
    AstNode* node       = context->node;
    auto     overload   = node->resolvedSymbolOverload;
    auto     typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
    auto     callParams = CastAst<AstNode>(node->childs[0], AstNodeKind::FuncCallParams);

    switch (typeInfo->intrinsic)
    {
    case Intrisic::Print:
    {
        auto child0 = callParams->childs[0];
        switch (TypeManager::concreteType(child0->typeInfo)->nativeType)
        {
        case NativeType::S32:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintS32, child0->resultRegisterRC);
            break;
        case NativeType::S64:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintS64, child0->resultRegisterRC);
            break;
        case NativeType::F32:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintF32, child0->resultRegisterRC);
            break;
        case NativeType::F64:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintF64, child0->resultRegisterRC);
            break;
        case NativeType::Char:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintChar, child0->resultRegisterRC);
            break;
        case NativeType::String:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintString, child0->resultRegisterRC);
            break;
        default:
            return internalError(context, "emitIntrinsic, @print invalid type");
        }

        freeRegisterRC(context, child0->resultRegisterRC);
        break;
    }
    case Intrisic::Assert:
    {
        auto child0 = callParams->childs[0];
        emitInstruction(context, ByteCodeOp::IntrinsicAssert, child0->resultRegisterRC);
        freeRegisterRC(context, child0->resultRegisterRC);
        break;
    }
    default:
        return internalError(context, "emitIntrinsic, unknown intrinsic");
    }

    return true;
}

bool ByteCodeGenJob::emitLocalCall(ByteCodeGenContext* context)
{
    AstNode* node         = context->node;
    auto     sourceFile   = context->sourceFile;
    auto     overload     = node->resolvedSymbolOverload;
    auto     funcNode     = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);
    auto     typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

    {
        scoped_lock lk(funcNode->mutex);
        if (funcNode->byteCodeJob != context->job) // If true, then this is a simple recursive call
        {
            // Need to wait for function full semantic resolve
            if (!(funcNode->flags & AST_FULL_RESOLVE))
            {
                funcNode->dependentJobs.push_back(context->job);
                context->result = ByteCodeResult::Pending;
                return true;
            }

            // Need to generate bytecode, if not already done or running
            if (!(funcNode->flags & AST_BYTECODE_GENERATED))
            {
                context->job->dependentNodes.push_back(funcNode);
                if (!funcNode->byteCodeJob)
                {
                    funcNode->byteCodeJob               = g_Pool_byteCodeGenJob.alloc();
                    funcNode->byteCodeJob->sourceFile   = sourceFile;
                    funcNode->byteCodeJob->originalNode = funcNode;
                    funcNode->byteCodeJob->nodes.push_back(funcNode);
                    setupBC(sourceFile->module, funcNode);
                    g_ThreadMgr.addJob(funcNode->byteCodeJob);
                }
            }
        }
    }

    int  precallStack  = 0;
    auto params        = node->childs.empty() ? nullptr : node->childs.front();
    int  numCallParams = params ? (int) params->childs.size() : 0;

    // Push current used registers
    const auto&      reservedRC = context->job->reservedRC;
    vector<uint32_t> copyReservedRC;
    for (auto it = reservedRC.begin(); it != reservedRC.end(); ++it)
    {
        bool isParam = false;
        if (params)
        {
            for (int i = 0; !isParam && (i < (int) params->childs.size()); i++)
            {
                auto oneChild = params->childs[i];
                for (int r = 0; r < (int) oneChild->resultRegisterRC.size(); r++)
                {
                    if (oneChild->resultRegisterRC[r] == *it)
                    {
                        isParam = true;
                        break;
                    }
                }
            }
        }

        if (!isParam)
        {
            copyReservedRC.push_back(*it);
            emitInstruction(context, ByteCodeOp::PushRCxSaved, *it);
        }
    }

    // Push missing default parameters
    int numParameters = 0;
    if (numCallParams != typeInfoFunc->parameters.size())
    {
        int index = 0;
        for (int i = (int) typeInfoFunc->parameters.size() - 1; i >= numCallParams; i--)
        {
            auto defaultParam = CastAst<AstVarDecl>(funcNode->parameters->childs[i], AstNodeKind::FuncDeclParam);
            context->node     = defaultParam->astAssignment;
            assert(context->node->flags & AST_VALUE_COMPUTED);
            emitLiteral(context);
            context->node = node;
            for (int r = defaultParam->astAssignment->resultRegisterRC.size() - 1; r >= 0; r--)
            {
                emitInstruction(context, ByteCodeOp::PushRCxParam, defaultParam->astAssignment->resultRegisterRC[r], index);
                precallStack += sizeof(Register);
                index++;
                numParameters++;
            }

            freeRegisterRC(context, defaultParam->astAssignment->resultRegisterRC);
        }
    }

    // Push parameters
    if (params)
    {
        int index = 0;
        for (int i = numCallParams - 1; i >= 0; i--)
        {
            auto param = params->childs[i];
            for (int r = param->resultRegisterRC.size() - 1; r >= 0; r--)
            {
                emitInstruction(context, ByteCodeOp::PushRCxParam, param->resultRegisterRC[r], index);
                precallStack += sizeof(Register);
                index++;
                numParameters++;
            }

            freeRegisterRC(context, param->resultRegisterRC);
        }
    }

    // Remember the number of parameters, to allocate registers in backend
    context->bc->maxCallParameters = max(context->bc->maxCallParameters, numParameters);

    emitInstruction(context, ByteCodeOp::LocalCall, 0, numParameters)->a.pointer = (uint8_t*) funcNode->bc;

    // Copy result in a computing register
    if (typeInfoFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::CopyRCxRRx, node->resultRegisterRC, 0);
    }

    // Restore stack as it was before the call, before the parameters
    if (precallStack)
    {
        emitInstruction(context, ByteCodeOp::IncSP, precallStack);
    }

    // Restore reserved registers
    for (auto it = copyReservedRC.rbegin(); it != copyReservedRC.rend(); ++it)
    {
        emitInstruction(context, ByteCodeOp::PopRCxSaved, *it);
    }

    return true;
}

bool ByteCodeGenJob::emitFuncDeclParams(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto funcNode = node->ownerFct;
    assert(funcNode);

    // 3 pointers are already on that stack after BP : saved BP, BC and IP.
    int offset = 3 * sizeof(void*);

    // Then add the full stack size of the function
    offset += funcNode->stackSize;

    int index = 0;
    for (auto param : node->childs)
    {
        auto resolved           = param->resolvedSymbolOverload;
        resolved->storageOffset = offset;
        resolved->storageIndex  = index;

        switch (resolved->typeInfo->nativeType)
        {
        case NativeType::String:
            offset += 2 * sizeof(Register);
            index += 2;
            break;
        default:
            offset += sizeof(Register);
            index++;
            break;
        }
    }

    return true;
}

bool ByteCodeGenJob::emitBeforeFuncDeclContent(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto funcNode = node->ownerFct;
    assert(funcNode);

    if (funcNode->stackSize)
    {
        emitInstruction(context, ByteCodeOp::DecSP)->a.u32 = funcNode->stackSize;
        emitInstruction(context, ByteCodeOp::MovSPBP);
    }

    return true;
}

bool ByteCodeGenJob::emitForeignCall(ByteCodeGenContext* context)
{
    AstNode* node         = context->node;
    auto     overload     = node->resolvedSymbolOverload;
    auto     funcNode     = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);
    auto     typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

    auto params        = node->childs.empty() ? nullptr : node->childs.front();
    int  numCallParams = params ? (int) params->childs.size() : 0;
    if (params)
    {
        for (int i = numCallParams - 1; i >= 0; i--)
        {
            auto param = params->childs[i];
            emitInstruction(context, ByteCodeOp::PushRCxParam, param->resultRegisterRC, i);
            freeRegisterRC(context, param->resultRegisterRC);
        }
    }

    auto inst       = emitInstruction(context, ByteCodeOp::ForeignCall);
    inst->a.pointer = (uint8_t*) funcNode;
    inst->b.pointer = (uint8_t*) typeInfoFunc;

    // Copy result in a computing register
    if (typeInfoFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::CopyRCxRRx, node->resultRegisterRC, 0);
    }

    return true;
}