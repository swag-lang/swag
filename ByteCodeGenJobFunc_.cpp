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
#include "CommandLine.h"

bool ByteCodeGenJob::emitLocalFuncDecl(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    int  countRR  = 0;

    // Reserve one RR register for each return value
    if (typeInfo->returnType != g_TypeMgr.typeInfoVoid)
    {
        if (typeInfo->returnType->kind == TypeInfoKind::Native)
        {
            switch (typeInfo->returnType->nativeType)
            {
            case NativeType::String:
                countRR += 2;
                break;
            default:
                countRR++;
                break;
            }
        }
        else
        {
            internalError(context, "emitLocalFuncDecl, invalid return type");
        }
    }

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
		//for(auto child: node->childs)
        auto child    = node->childs[0];
        auto typeInfo = TypeManager::concreteType(child->typeInfo);
		if (typeInfo->kind == TypeInfoKind::Native)
		{
			for (int r = 0; r < child->resultRegisterRC.size(); r++)
				emitInstruction(context, ByteCodeOp::CopyRRxRCx, r, child->resultRegisterRC[r]);
		}
		else
		{
			return internalError(context, "emitReturn, type not native");
		}
    }

    SWAG_ASSERT(node->ownerFct);
    if (node->ownerFct->stackSize)
        emitInstruction(context, ByteCodeOp::IncSP)->a.s32 = node->ownerFct->stackSize;
    emitInstruction(context, ByteCodeOp::Ret);
    return true;
}

bool ByteCodeGenJob::emitIntrinsic(ByteCodeGenContext* context)
{
    AstNode* node       = context->node;
    auto     callParams = CastAst<AstNode>(node->childs[0], AstNodeKind::FuncCallParameters);

    switch (node->token.id)
    {
    case TokenId::IntrisicPrint:
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
    case TokenId::IntrisicAssert:
    {
        auto child0 = callParams->childs.front();
        if (!g_CommandLine.optimizeByteCode || !child0->isConstantTrue())
            emitInstruction(context, ByteCodeOp::IntrinsicAssert, child0->resultRegisterRC);
        freeRegisterRC(context, child0->resultRegisterRC);
        break;
    }
    default:
        return internalError(context, "emitIntrinsic, unknown intrinsic");
    }

    return true;
}

bool ByteCodeGenJob::emitLambdaCall(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     overload = node->resolvedSymbolOverload;
    auto     varNode  = (AstVarDecl*) overload->node;
    SWAG_CHECK(emitIdentifier(context));
    node->resultRegisterRC = node->resultRegisterRC;
    return emitLocalCall(context, nullptr, varNode);
}

bool ByteCodeGenJob::emitLocalCall(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     overload = node->resolvedSymbolOverload;
    auto     funcNode = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);
    return emitLocalCall(context, funcNode, nullptr);
}

bool ByteCodeGenJob::emitLocalCall(ByteCodeGenContext* context, AstFuncDecl* funcNode, AstVarDecl* varNode)
{
    AstNode* node       = context->node;
    auto     sourceFile = context->sourceFile;

    TypeInfoFuncAttr* typeInfoFunc = nullptr;
    if (funcNode)
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    else
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(varNode->typeInfo, TypeInfoKind::Lambda);

    if (funcNode)
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
    auto allParams     = node->childs.empty() ? nullptr : node->childs.front();
    int  numCallParams = allParams ? (int) allParams->childs.size() : 0;

    // Push current used registers before the call, to restore their value after the call
    // (as a function can change everything)
    const auto&      reservedRC = context->job->reservedRC;
    vector<uint32_t> copyReservedRC;
    for (auto it = reservedRC.begin(); it != reservedRC.end(); ++it)
    {
        bool isParam = false;
        if (allParams)
        {
            for (int i = 0; !isParam && (i < (int) allParams->childs.size()); i++)
            {
                auto oneChild = allParams->childs[i];
                for (int r = 0; r < (int) oneChild->resultRegisterRC.size(); r++)
                {
                    if (oneChild->resultRegisterRC[r] == *it)
                    {
                        // No need to store the register value, as this will be used for the parameter
                        isParam = true;
                        break;
                    }
                }
            }
        }

        if (!isParam)
        {
            copyReservedRC.push_back(*it);
            emitInstruction(context, ByteCodeOp::PushRASaved, *it);
        }
    }

    // Sort childs by parameter index
    if (allParams && (allParams->flags & AST_MUST_SORT_CHILDS))
    {
        sort(allParams->childs.begin(), allParams->childs.end(), [](AstNode* n1, AstNode* n2) {
            AstFuncCallParam* p1 = static_cast<AstFuncCallParam*>(n1);
            AstFuncCallParam* p2 = static_cast<AstFuncCallParam*>(n2);
            return p1->index < p2->index;
        });
    }

    // Push missing default parameters
    int numRegisters = 0;
    if (numCallParams != typeInfoFunc->parameters.size())
    {
        // Push all parameters, from end to start
        for (int i = (int) typeInfoFunc->parameters.size() - 1; i >= 0; i--)
        {
            // Determine if this parameter has been covered by the call
            bool covered = false;
            for (int j = 0; allParams && j < allParams->childs.size(); j++)
            {
                auto param = static_cast<AstFuncCallParam*>(allParams->childs[j]);
                if (param->index == i)
                {
                    for (int r = param->resultRegisterRC.size() - 1; r >= 0; r--)
                    {
                        emitInstruction(context, ByteCodeOp::PushRAParam, param->resultRegisterRC[r], numRegisters);
                        precallStack += sizeof(Register);
                        numRegisters++;
                    }

                    freeRegisterRC(context, param->resultRegisterRC);
                    covered = true;
                    break;
                }
            }

            // If not covered, then this is a default value
            if (!covered)
            {
                auto defaultParam = CastAst<AstVarDecl>(funcNode->parameters->childs[i], AstNodeKind::FuncDeclParam);
                context->node     = defaultParam->astAssignment;
                SWAG_ASSERT(context->node->flags & AST_VALUE_COMPUTED);
                emitLiteral(context, defaultParam->typeInfo);
                context->node = node;
                for (int r = defaultParam->astAssignment->resultRegisterRC.size() - 1; r >= 0; r--)
                {
                    emitInstruction(context, ByteCodeOp::PushRAParam, defaultParam->astAssignment->resultRegisterRC[r], numRegisters);
                    precallStack += sizeof(Register);
                    numRegisters++;
                }

                freeRegisterRC(context, defaultParam->astAssignment->resultRegisterRC);
            }
        }
    }

    // Fast call. No need to do fancy things, all the parameters are covered by the call
    else if (numCallParams)
    {
        for (int i = numCallParams - 1; i >= 0; i--)
        {
            auto param = allParams->childs[i];
            for (int r = param->resultRegisterRC.size() - 1; r >= 0; r--)
            {
                emitInstruction(context, ByteCodeOp::PushRAParam, param->resultRegisterRC[r], numRegisters);
                precallStack += sizeof(Register);
                numRegisters++;
            }

            freeRegisterRC(context, param->resultRegisterRC);
        }
    }

    // Remember the number of parameters, to allocate registers in backend
    context->bc->maxCallParameters = max(context->bc->maxCallParameters, numRegisters);

    if (funcNode)
    {
        auto inst       = emitInstruction(context, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) funcNode->bc;
        inst->b.u32     = numRegisters;
    }
    else
    {
        auto inst       = emitInstruction(context, ByteCodeOp::LambdaCall, node->resultRegisterRC);
        inst->b.u32     = numRegisters;
        inst->c.pointer = (uint8_t*) typeInfoFunc;
    }

    // Copy result in a computing register
    if (typeInfoFunc->returnType && typeInfoFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        if (typeInfoFunc->returnType->kind == TypeInfoKind::Native)
        {
            if (typeInfoFunc->returnType->nativeType == NativeType::String)
            {
                reserveRegisterRC(context, node->resultRegisterRC, 2);
                emitInstruction(context, ByteCodeOp::CopyRCxRRx, node->resultRegisterRC[0], 0);
                emitInstruction(context, ByteCodeOp::CopyRCxRRx, node->resultRegisterRC[1], 1);
                context->bc->maxCallResults = max(context->bc->maxCallResults, 2);
            }
            else
            {
                node->resultRegisterRC = reserveRegisterRC(context);
                emitInstruction(context, ByteCodeOp::CopyRCxRRx, node->resultRegisterRC, 0);
                context->bc->maxCallResults = max(context->bc->maxCallResults, 1);
            }
        }
        else
        {
            internalError(context, "emitLocalCall, bad return type");
        }
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
    SWAG_ASSERT(funcNode);

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

        auto typeInfo = TypeManager::concreteType(resolved->typeInfo);
        if (typeInfo->isNative(NativeType::String) || typeInfo->kind == TypeInfoKind::Slice)
        {
            offset += 2 * sizeof(Register);
            index += 2;
        }
        else if (typeInfo->kind == TypeInfoKind::Native ||
                 typeInfo->kind == TypeInfoKind::Array ||
                 typeInfo->kind == TypeInfoKind::Lambda ||
                 typeInfo->kind == TypeInfoKind::Pointer)
        {
            offset += sizeof(Register);
            index++;
        }
        else
        {
            return internalError(context, "emitFuncDeclParams, invalid parameter type", param);
        }
    }

    return true;
}

bool ByteCodeGenJob::emitBeforeFuncDeclContent(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto funcNode = node->ownerFct;
    SWAG_ASSERT(funcNode);

    if (funcNode->stackSize)
    {
        auto inst   = emitInstruction(context, ByteCodeOp::DecSP);
        inst->a.u32 = funcNode->stackSize;
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
            emitInstruction(context, ByteCodeOp::PushRAParam, param->resultRegisterRC, i);
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