#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "SourceFile.h"
#include "PoolFactory.h"
#include "ThreadManager.h"
#include "LanguageSpec.h"
#include "Module.h"

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

    emitInstruction(context, ByteCodeOp::Ret);
    return true;
}

bool ByteCodeGenJob::emitIntrinsic(ByteCodeGenContext* context)
{
    AstNode* node       = context->node;
    auto     overload   = node->resolvedSymbolOverload;
    auto     typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FunctionAttribute);
    auto     callParams = CastAst<AstNode>(node->childs[0], AstNodeKind::FuncCallParams);

    switch (typeInfo->intrinsic)
    {
    case Intrisic::Print:
    {
        auto child0 = callParams->childs[0];
        context->sourceFile->module->freeRegisterRC(child0->resultRegisterRC);
        switch (child0->typeInfo->nativeType)
        {
        case NativeType::S64:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintS64, child0->resultRegisterRC);
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
        }
        break;
    }
    case Intrisic::Assert:
    {
        auto child0 = callParams->childs[0];
        context->sourceFile->module->freeRegisterRC(child0->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::IntrinsicAssert, child0->resultRegisterRC);
        break;
    }
    default:
        assert(false);
    }

    return true;
}

bool ByteCodeGenJob::emitIdentifierRef(ByteCodeGenContext* context)
{
    AstNode* node = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitLocalFuncCall(ByteCodeGenContext* context)
{
    AstNode* node       = context->node;
    auto     sourceFile = context->sourceFile;
    auto     overload   = node->resolvedSymbolOverload;
    auto     overnode   = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    {
        scoped_lock lk(overnode->mutex);

        // Need to wait for function full semantic resolve
        if (!(overnode->flags & AST_FULL_RESOLVE))
        {
            overnode->dependentJobs.push_back(context->job);
            context->result = ByteCodeResult::Pending;
            return true;
        }

        // Need to generate bytecode, if not already done or running
        if (!(overnode->flags & AST_BYTECODE_GENERATED))
        {
            if (!overnode->byteCodeJob)
            {
                overnode->byteCodeJob               = sourceFile->poolFactory->bytecodeJob.alloc();
                overnode->byteCodeJob->sourceFile   = sourceFile;
                overnode->byteCodeJob->originalNode = overnode;
                overnode->byteCodeJob->nodes.push_back(overnode);
                g_ThreadMgr.addJob(overnode->byteCodeJob);
            }

            overnode->byteCodeJob->dependentJobs.push_back(context->job);
            context->result = ByteCodeResult::Pending;
            return true;
        }
    }

    emitInstruction(context, ByteCodeOp::LocalFuncCall)->a.pointer = overnode->bc;

	// Copy result in a computing register
	node->resultRegisterRC = sourceFile->module->reserveRegisterRC();
    emitInstruction(context, ByteCodeOp::CopyRCxRRx, node->resultRegisterRC, 0);

    return true;
}
