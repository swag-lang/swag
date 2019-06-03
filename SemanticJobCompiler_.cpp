#include "pch.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "Log.h"
#include "SourceFile.h"
#include "PoolFactory.h"
#include "Module.h"
#include "ThreadManager.h"

bool SemanticJob::executeNode(SemanticContext* context, AstNode* node)
{
    auto sourceFile = context->sourceFile;

    // No need to run, this is already baked
    if (node->flags & AST_VALUE_COMPUTED)
        return true;

    {
        // Need to generate bytecode, if not already done or running
        scoped_lock lk(node->mutex);
        if (!(node->flags & AST_BYTECODE_GENERATED))
        {
            if (!node->byteCodeJob)
            {
                node->byteCodeJob               = sourceFile->poolFactory->bytecodeJob.alloc();
                node->byteCodeJob->sourceFile   = sourceFile;
                node->byteCodeJob->originalNode = node;
                node->byteCodeJob->nodes.push_back(node);
                node->byteCodeJob->dependentJobs.push_back(context->job);
                g_ThreadMgr.addJob(node->byteCodeJob);
            }

            context->result = SemanticResult::Pending;
            return true;
        }
    }

    SWAG_CHECK(sourceFile->module->executeNode(sourceFile, node));
    return true;
}

bool SemanticJob::resolveCompilerRun(SemanticContext* context)
{
    auto node = context->node;
    auto expr = context->node->childs[0];

    node->typeInfo = expr->typeInfo;
    node->inheritAndFlag(expr, AST_CONST_EXPR);
    node->inheritComputedValue(expr);

    SWAG_CHECK(executeNode(context, expr));
    return true;
}

bool SemanticJob::resolveCompilerAssert(SemanticContext* context)
{
    auto node       = context->node;
    auto expr       = context->node->childs[0];
    auto sourceFile = context->sourceFile;

    node->typeInfo = expr->typeInfo;
    node->inheritAndFlag(expr, AST_CONST_EXPR);
    node->inheritComputedValue(expr);

    SWAG_VERIFY(node->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, node->childs[0]->token, "can't evaluate expression at compile time"}));
    SWAG_VERIFY(node->typeInfo == g_TypeMgr.typeInfoBool, sourceFile->report({sourceFile, node->childs[0]->token, "expression should be 'bool'"}));
    SWAG_VERIFY(node->computedValue.reg.b, sourceFile->report({sourceFile, node->token, "compiler assertion failed"}));
    return true;
}

bool SemanticJob::resolveCompilerPrint(SemanticContext* context)
{
    auto expr       = context->node->childs[0];
    auto sourceFile = context->sourceFile;

    SWAG_VERIFY(expr->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, expr->token, "can't evaluate expression at compile time"}));
    SWAG_CHECK(executeNode(context, expr));
    if (context->result == SemanticResult::Pending)
        return true;

    TypeInfo* typeInfo = expr->typeInfo;
    if (typeInfo->kind == TypeInfoKind::FunctionAttribute)
        typeInfo = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FunctionAttribute)->returnType;
    else
        typeInfo = TypeManager::flattenType(expr->typeInfo);

    g_Log.lock();
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
        assert(false);
        break;
    }

    g_Log.eol();
    g_Log.unlock();
    return true;
}
