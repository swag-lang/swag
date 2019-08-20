#include "pch.h"
#include "Global.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "SemanticJob.h"
#include "TypeInfo.h"
#include "Ast.h"
#include "Scope.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Log.h"
#include "Workspace.h"
#include "ByteCodeGenJob.h"

bool SemanticJob::executeNode(SemanticContext* context, AstNode* node, bool onlyconstExpr)
{
    auto sourceFile = context->sourceFile;

    // No need to run, this is already baked
    if (node->flags & AST_VALUE_COMPUTED)
        return true;

    if (onlyconstExpr)
    {
        SWAG_VERIFY(node->flags & AST_CONST_EXPR, sourceFile->report({sourceFile, node, "expression cannot be evaluated at compile time"}));
    }

    {
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
                node->byteCodeJob->dependentJobs.push_back(context->job);
                ByteCodeGenJob::setupBC(sourceFile->module, node);
                g_ThreadMgr.addJob(node->byteCodeJob);
            }

            context->result = SemanticResult::Pending;
            return true;
        }

        if (!(node->flags & AST_BYTECODE_RESOLVED))
        {
            node->byteCodeJob->dependentJobs.push_back(context->job);
            context->result = SemanticResult::Pending;
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
    auto expr       = context->node->childs[0];
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(expr->typeInfo);

    SWAG_VERIFY(typeInfo == g_TypeMgr.typeInfoBool, sourceFile->report({sourceFile, expr->token, "expression should be 'bool'"}));

    SWAG_CHECK(executeNode(context, expr, true));
    if (context->result == SemanticResult::Pending)
        return true;
    SWAG_VERIFY(expr->computedValue.reg.b, sourceFile->report({sourceFile, expr, "compiler assertion failed"}));
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
    case NativeType::Bool:
        g_Log.print(expr->computedValue.reg.b ? "true" : "false");
        break;
    case NativeType::S8:
        g_Log.print(to_string(expr->computedValue.reg.s8));
        break;
    case NativeType::S16:
        g_Log.print(to_string(expr->computedValue.reg.s16));
        break;
    case NativeType::S32:
        g_Log.print(to_string(expr->computedValue.reg.s32));
        break;
    case NativeType::S64:
        g_Log.print(to_string(expr->computedValue.reg.s64));
        break;
    case NativeType::U8:
        g_Log.print(to_string(expr->computedValue.reg.u8));
        break;
    case NativeType::U16:
        g_Log.print(to_string(expr->computedValue.reg.u16));
        break;
    case NativeType::U32:
        g_Log.print(to_string(expr->computedValue.reg.u32));
        break;
    case NativeType::U64:
        g_Log.print(to_string(expr->computedValue.reg.u64));
        break;
    case NativeType::F32:
        g_Log.print(to_string(expr->computedValue.reg.f32));
        break;
    case NativeType::F64:
        g_Log.print(to_string(expr->computedValue.reg.f64));
        break;
    case NativeType::Char:
        g_Log.print(to_string(expr->computedValue.reg.ch));
        break;
    case NativeType::String:
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

bool SemanticJob::resolveCompilerIf(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstIf>(context->node->parent, AstNodeKind::If);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoBool, node->boolExpression));
    SWAG_VERIFY(node->boolExpression->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, node->boolExpression, "expression cannot be evaluated at compile time"}));

    // Do not generate backend if 'if' is constant, and has already been evaluated
    node->boolExpression->flags |= AST_NO_BYTECODE;
    if (node->boolExpression->computedValue.reg.b)
    {
		if (node->elseBlock)
		{
			node->elseBlock->flags |= AST_NO_BYTECODE;
			node->elseBlock->flags |= AST_DISABLED;
		}
    }
    else
    {
        node->ifBlock->flags |= AST_NO_BYTECODE;
		node->ifBlock->flags |= AST_DISABLED;
    }

    return true;
}