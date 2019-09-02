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

void ByteCodeGenJob::emitRASavedPush(ByteCodeGenContext* context, const vector<uint32_t>& regToSave)
{
    for (int r = 0; r < regToSave.size(); r++)
        emitInstruction(context, ByteCodeOp::PushRASaved, regToSave[r]);
}

void ByteCodeGenJob::emitRASavedPop(ByteCodeGenContext* context, const vector<uint32_t>& regToSave)
{
    for (int r = (int) regToSave.size() - 1; r >= 0; r--)
        emitInstruction(context, ByteCodeOp::PopRASaved, regToSave[r]);
}

bool ByteCodeGenJob::emitLocalFuncDecl(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    int  countRR  = 0;

    // Reserve one RR register for each return value
    if (typeInfo->returnType != g_TypeMgr.typeInfoVoid)
        countRR += typeInfo->returnType->numRegisters();

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

bool ByteCodeGenJob::emitReturn(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    // Copy result to RR0... registers
    if (!node->childs.empty())
    {
        auto front = node->childs.front();
        if (front->typeInfo->flags & TYPEINFO_RETURN_BY_COPY)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::CopyRR0, front->resultRegisterRC);
            inst->b.u32 = front->typeInfo->sizeOf;
        }
        else
        {
            for (auto child : node->childs)
            {
                for (int r = 0; r < child->resultRegisterRC.size(); r++)
                {
                    emitInstruction(context, ByteCodeOp::CopyRRxRCx, r, child->resultRegisterRC[r]);
                }
            }
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
    SWAG_ASSERT(!node->name.empty());
    auto intrinsic = g_LangSpec.intrinsics[node->name];
    switch (intrinsic)
    {
    case Intrinsic::IntrinsicPrint:
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
    case Intrinsic::IntrinsicAssert:
    {
        auto child0 = callParams->childs.front();
        if (!g_CommandLine.optimizeByteCode || !child0->isConstantTrue())
            emitInstruction(context, ByteCodeOp::IntrinsicAssert, child0->resultRegisterRC);
        freeRegisterRC(context, child0->resultRegisterRC);
        break;
    }
    case Intrinsic::IntrinsicAlloc:
    {
        auto child0            = callParams->childs.front();
        node->resultRegisterRC = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::IntrinsicAlloc, node->resultRegisterRC, child0->resultRegisterRC);
        break;
    }
    case Intrinsic::IntrinsicFree:
    {
        auto child0 = callParams->childs.front();
        emitInstruction(context, ByteCodeOp::IntrinsicFree, child0->resultRegisterRC);
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
    varNode->resultRegisterRC = node->resultRegisterRC;

    auto allParams = node->childs.empty() ? nullptr : node->childs.front();
    return emitLocalCall(context, allParams, nullptr, varNode);
}

bool ByteCodeGenJob::emitLocalCall(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     overload = node->resolvedSymbolOverload;
    auto     funcNode = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    auto allParams = node->childs.empty() ? nullptr : node->childs.back();
    return emitLocalCall(context, allParams, funcNode, nullptr);
}

void ByteCodeGenJob::askForByteCode(ByteCodeGenContext* context, AstFuncDecl* funcNode)
{
    if (!funcNode)
        return;

    auto        sourceFile = context->sourceFile;
    scoped_lock lk(funcNode->mutex);
    if (funcNode->byteCodeJob != context->job) // If true, then this is a simple recursive call
    {
        // Need to wait for function full semantic resolve
        if (!(funcNode->flags & AST_FULL_RESOLVE))
        {
            funcNode->dependentJobs.push_back(context->job);
            context->result = ByteCodeResult::Pending;
            return;
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

bool ByteCodeGenJob::emitLocalCall(ByteCodeGenContext* context, AstNode* allParams, AstFuncDecl* funcNode, AstVarDecl* varNode)
{
    AstNode*          node         = context->node;
    TypeInfoFuncAttr* typeInfoFunc = nullptr;
    if (funcNode)
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    else
        typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(varNode->typeInfo, TypeInfoKind::Lambda);

    // Be sure referenced function has bytecode
    askForByteCode(context, funcNode);
    if (context->result == ByteCodeResult::Pending)
        return true;

    int precallStack  = 0;
    int numCallParams = allParams ? (int) allParams->childs.size() : 0;

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

    // Store in RR0 the address of the stack to store the result
    if (typeInfoFunc->returnType && (typeInfoFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY))
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        auto inst              = emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC);
        inst->b.u32            = node->fctCallStorageOffset;
        emitInstruction(context, ByteCodeOp::CopyRRxRCxCall, 0, node->resultRegisterRC);
        context->bc->maxCallResults = max(context->bc->maxCallResults, 1);
        copyReservedRC.push_back(node->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::PushRASaved, node->resultRegisterRC);
    }

    // If we are in a function that need to keep the RR0 register alive, we need to save it
    bool rr0Saved = false;
    if (node->ownerFct &&
        node->ownerFct->returnType &&
        node->ownerFct->returnType->typeInfo &&
        (node->ownerFct->returnType->typeInfo->flags & TYPEINFO_RETURN_BY_COPY))
    {
        emitInstruction(context, ByteCodeOp::PushRRSaved, 0);
        rr0Saved = true;
    }

    // Sort childs by parameter index
    if (allParams && (allParams->flags & AST_MUST_SORT_CHILDS))
    {
        sort(allParams->childs.begin(), allParams->childs.end(), [](AstNode* n1, AstNode* n2) {
            AstFuncCallParam* p1 = CastAst<AstFuncCallParam>(n1, AstNodeKind::FuncCallParam);
            AstFuncCallParam* p2 = CastAst<AstFuncCallParam>(n2, AstNodeKind::FuncCallParam);
            return p1->index < p2->index;
        });
    }

    int numRegisters = 0;
    if (allParams && (typeInfoFunc->flags & TYPEINFO_VARIADIC))
    {
        // Store the offset (in registers) of each additional parameter
        int offset = 0;
        for (int i = 0; i < numCallParams; i++)
        {
            auto typeParam = allParams->childs[i]->typeInfo;
            offset += typeParam->numRegisters();
        }

        auto r0 = reserveRegisterRC(context);
        for (int i = (int) numCallParams - 1; i >= (int) typeInfoFunc->parameters.size() - 1; i--)
        {
            auto typeParam = allParams->childs[i]->typeInfo;
            offset -= typeParam->numRegisters();
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.u32 = offset;
            emitInstruction(context, ByteCodeOp::PushRAParam, r0, numRegisters);
            precallStack += sizeof(Register);
            numRegisters++;
        }

        freeRegisterRC(context, r0);
    }

    // Push missing default parameters
    uint64_t numPushParams = 0;
    if (numCallParams < typeInfoFunc->parameters.size())
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
                        numPushParams++;
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
                SWAG_ASSERT(defaultParam->assignment);
                context->node = defaultParam->assignment;
                SWAG_ASSERT(context->node->flags & AST_VALUE_COMPUTED);
                emitLiteral(context, defaultParam->typeInfo);
                context->node = node;
                for (int r = defaultParam->assignment->resultRegisterRC.size() - 1; r >= 0; r--)
                {
                    emitInstruction(context, ByteCodeOp::PushRAParam, defaultParam->assignment->resultRegisterRC[r], numRegisters);
                    precallStack += sizeof(Register);
                    numRegisters++;
                    numPushParams++;
                }

                freeRegisterRC(context, defaultParam->assignment->resultRegisterRC);
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
                numPushParams++;
            }

            freeRegisterRC(context, param->resultRegisterRC);
        }
    }

    // Variadic parameter is on top of stack
    if (typeInfoFunc->flags & TYPEINFO_VARIADIC)
    {
        auto r0          = reserveRegisterRC(context);
        auto numVariadic = (uint32_t)(numCallParams - typeInfoFunc->parameters.size()) + 1;

        // Store number of extra parameters
        emitInstruction(context, ByteCodeOp::CopyRAVB64, r0)->b.u64 = numVariadic | ((numPushParams + 1) << 32);
        emitInstruction(context, ByteCodeOp::PushRAParam, r0, numRegisters);

        // Store address on the stack of those parameters. This must be the last push
        emitInstruction(context, ByteCodeOp::MovRASP, r0, numRegisters + 1);
        emitInstruction(context, ByteCodeOp::PushRAParam, r0, numRegisters + 1);

        precallStack += 2 * sizeof(Register);
        numRegisters += 2;

        freeRegisterRC(context, r0);
    }

    // Remember the number of parameters, to allocate registers in backend
    context->bc->maxCallParameters = max(context->bc->maxCallParameters, numRegisters);

    if (funcNode)
    {
        auto inst       = emitInstruction(context, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) funcNode->bc;
        inst->b.u64     = numRegisters;
        inst->c.pointer = (uint8_t*) typeInfoFunc;
    }
    else
    {
		SWAG_ASSERT(varNode);
        auto inst       = emitInstruction(context, ByteCodeOp::LambdaCall, varNode->resultRegisterRC);
        inst->b.u64     = numRegisters;
        inst->c.pointer = (uint8_t*) typeInfoFunc;
    }

    // Copy result in a computing register
    if (typeInfoFunc->returnType && typeInfoFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        if (!(typeInfoFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY))
        {
            auto numRegs = typeInfoFunc->returnType->numRegisters();
            reserveRegisterRC(context, node->resultRegisterRC, numRegs);
            context->bc->maxCallResults = max(context->bc->maxCallResults, numRegs);
            for (int idx = 0; idx < node->resultRegisterRC.size(); idx++)
                emitInstruction(context, ByteCodeOp::CopyRCxRRx, node->resultRegisterRC[idx], idx);
        }
    }

    // Restore stack as it was before the call, before the parameters
    if (precallStack)
    {
        emitInstruction(context, ByteCodeOp::IncSP, precallStack);
    }

    // If we are in a function that need to keep the RR0 register alive, we need to restore it
    if (rr0Saved)
    {
        emitInstruction(context, ByteCodeOp::PopRRSaved, 0);
    }

    // Restore reserved registers
    for (auto it = copyReservedRC.rbegin(); it != copyReservedRC.rend(); ++it)
    {
        emitInstruction(context, ByteCodeOp::PopRASaved, *it);
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

    // Variadic parameter is the last one pushed on the stack
    int storageIndex = 0;
    if (funcNode->typeInfo->flags & TYPEINFO_VARIADIC)
    {
        auto param              = node->childs.back();
        auto resolved           = param->resolvedSymbolOverload;
        resolved->storageOffset = offset;
        offset += g_TypeMgr.typeInfoVariadic->sizeOf;
        resolved->storageIndex = 0; // Always the first one
        storageIndex += 2;
    }

    for (int i = 0; i < node->childs.size(); i++)
    {
        if ((i == node->childs.size() - 1) && funcNode->typeInfo->flags & TYPEINFO_VARIADIC)
            break;
        auto param              = node->childs[i];
        auto resolved           = param->resolvedSymbolOverload;
        resolved->storageOffset = offset;
        resolved->storageIndex  = storageIndex;

        auto typeInfo     = TypeManager::concreteType(resolved->typeInfo);
        int  numRegisters = typeInfo->numRegisters();
        offset += numRegisters * sizeof(Register);
        storageIndex += numRegisters;
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