#include "pch.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Module.h"
#include "ByteCodeGenJob.h"

bool SemanticJob::executeNode(SemanticContext* context, AstNode* node, bool onlyconstExpr)
{
    // No need to run, this is already baked
    if (node->flags & AST_VALUE_COMPUTED)
        return true;

    auto sourceFile = context->sourceFile;
    if (onlyconstExpr)
    {
        SWAG_VERIFY(node->flags & AST_CONST_EXPR, context->errorContext.report({node, "expression cannot be evaluated at compile time"}));
    }

    {
        auto job = context->job;
        // Need to generate bytecode, if not already done or running
        scoped_lock lk(node->mutex);
        if (!(node->flags & AST_BYTECODE_GENERATED))
        {
            if (!node->byteCodeJob)
            {
                node->byteCodeJob               = g_Pool_byteCodeGenJob.alloc();
                node->byteCodeJob->sourceFile   = sourceFile;
                node->byteCodeJob->originalNode = node;
                node->byteCodeJob->nodes.push_back(node);
                node->byteCodeJob->dependentJobs.add(context->job);
                ByteCodeGenJob::setupBC(sourceFile->module, node);
                g_ThreadMgr.addJob(node->byteCodeJob);
            }

            job->setPending();
            return true;
        }

        if (!(node->flags & AST_BYTECODE_RESOLVED))
        {
            node->byteCodeJob->dependentJobs.add(job);
            job->setPending();
            return true;
        }
    }

    SWAG_CHECK(sourceFile->module->executeNode(sourceFile, node));
    return true;
}

bool SemanticJob::forceExecuteNode(SemanticContext* context)
{
    SWAG_CHECK(executeNode(context, context->node, false));
    return true;
}

bool SemanticJob::resolveCompilerRun(SemanticContext* context)
{
    auto expr = context->node->childs[0];
    SWAG_CHECK(executeNode(context, expr, false));
    return true;
}

bool SemanticJob::resolveCompilerAssert(SemanticContext* context)
{
    auto node = context->node;
    auto expr = node->childs[0];

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, expr, CASTFLAG_AUTO_BOOL));
    SWAG_CHECK(executeNode(context, expr, true));
    if (context->result == SemanticResult::Pending)
        return true;

    if (!expr->computedValue.reg.b)
    {
        if (!node->name.empty())
            context->errorContext.report({expr, format("compiler assertion failed: %s", node->name.c_str())});
        else
            context->errorContext.report({expr, "compiler assertion failed"});
        return false;
    }

    return true;
}

bool SemanticJob::resolveCompilerPrint(SemanticContext* context)
{
    auto expr = context->node->childs[0];
    SWAG_CHECK(executeNode(context, expr, true));
    if (context->result == SemanticResult::Pending)
        return true;

    g_Log.lock();
    TypeInfo* typeInfo = TypeManager::concreteType(expr->typeInfo);
    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
        g_Log.print(expr->computedValue.reg.b ? "true" : "false");
        break;
    case NativeTypeKind::S8:
        g_Log.print(to_string(expr->computedValue.reg.s8));
        break;
    case NativeTypeKind::S16:
        g_Log.print(to_string(expr->computedValue.reg.s16));
        break;
    case NativeTypeKind::S32:
        g_Log.print(to_string(expr->computedValue.reg.s32));
        break;
    case NativeTypeKind::S64:
        g_Log.print(to_string(expr->computedValue.reg.s64));
        break;
    case NativeTypeKind::U8:
        g_Log.print(to_string(expr->computedValue.reg.u8));
        break;
    case NativeTypeKind::U16:
        g_Log.print(to_string(expr->computedValue.reg.u16));
        break;
    case NativeTypeKind::U32:
        g_Log.print(to_string(expr->computedValue.reg.u32));
        break;
    case NativeTypeKind::U64:
        g_Log.print(to_string(expr->computedValue.reg.u64));
        break;
    case NativeTypeKind::F32:
        g_Log.print(to_string(expr->computedValue.reg.f32));
        break;
    case NativeTypeKind::F64:
        g_Log.print(to_string(expr->computedValue.reg.f64));
        break;
    case NativeTypeKind::Char:
        g_Log.print(to_string(expr->computedValue.reg.ch));
        break;
    case NativeTypeKind::String:
        g_Log.print(expr->computedValue.text);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    g_Log.eol();
    g_Log.unlock();
    return true;
}

void SemanticJob::disableCompilerIfBlock(SemanticContext* context, AstCompilerIfBlock* block)
{
    block->flags |= AST_NO_BYTECODE;
    block->flags |= AST_DISABLED;

    // Unregister one overload
    for (auto symbol : block->symbols)
    {
        scoped_lock lk(symbol->mutex);
		SymTable::decreaseOverloadNoLock(symbol);
    }

	for (auto p : block->blocks)
		disableCompilerIfBlock(context, p);
}

bool SemanticJob::resolveCompilerIf(SemanticContext* context)
{
    auto node = CastAst<AstIf>(context->node->parent, AstNodeKind::CompilerIf);
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));
    SWAG_VERIFY(node->boolExpression->flags & AST_VALUE_COMPUTED, context->errorContext.report({node->boolExpression, "expression cannot be evaluated at compile time"}));

    node->boolExpression->flags |= AST_NO_BYTECODE;
    if (node->boolExpression->computedValue.reg.b)
    {
        if (node->elseBlock)
            disableCompilerIfBlock(context, (AstCompilerIfBlock*) node->elseBlock);
    }
    else
    {
        disableCompilerIfBlock(context, (AstCompilerIfBlock*) node->ifBlock);
    }

    return true;
}

bool SemanticJob::resolveCompilerFunction(SemanticContext* context)
{
    auto node = context->node;

    switch (node->token.id)
    {
    case TokenId::CompilerFunction:
        SWAG_VERIFY(node->ownerFct, context->errorContext.report({node, "'#function' can only be called inside a function"}));
        node->computedValue.text = node->ownerFct->name;
        node->typeInfo           = g_TypeMgr.typeInfoString;
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
        return true;

    case TokenId::CompilerCallerLine:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->errorContext.report({node, "'#callerline' can only be set in a function parameter declaration"}));
        node->typeInfo = g_TypeMgr.typeInfoU32;
        return true;
    case TokenId::CompilerCallerFile:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->errorContext.report({node, "'#callerfile' can only be set in a function parameter declaration"}));
        node->typeInfo = g_TypeMgr.typeInfoString;
        return true;
    case TokenId::CompilerCallerFunction:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->errorContext.report({node, "'#callerfile' can only be set in a function parameter declaration"}));
        node->typeInfo = g_TypeMgr.typeInfoString;
        return true;

    default:
        return internalError(context, "resolveCompilerFunction, unknown token");
    }
}