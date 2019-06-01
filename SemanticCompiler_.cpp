#include "pch.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "Log.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "PoolFactory.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "ThreadManager.h"

bool SemanticJob::resolveCompilerRun(SemanticContext* context)
{
    auto node       = context->node;
    auto expr       = context->node->childs[0];
    auto sourceFile = context->sourceFile;

    node->typeInfo = expr->typeInfo;
    node->inheritAndFlag(expr, AST_CONST_EXPR);
    node->inheritComputedValue(expr);

	// No need to run, this is already baked
    if (node->flags & AST_VALUE_COMPUTED)
    {
        context->result = SemanticResult::Done;
        return true;
    }

    {
        scoped_lock lk(node->mutex);
        // Need to generate bytecode, if not already done or running
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

    ByteCodeRunContext runContext;
    runContext.node       = expr;
    runContext.sourceFile = sourceFile;
    runContext.bc         = node->bc;
    runContext.bc->out.rewind();
    runContext.stack.resize(1024);
    runContext.sp = 0;
    SWAG_CHECK(g_Run.run(&runContext));
    if (runContext.sp)
        node->computedValue.reg = runContext.stack[runContext.sp - 1].reg;

    context->result = SemanticResult::Done;
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
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveCompilerPrint(SemanticContext* context)
{
    auto node       = context->node;
    auto expr       = context->node->childs[0];
    auto sourceFile = context->sourceFile;

    node->typeInfo = expr->typeInfo;
    node->inheritAndFlag(expr, AST_CONST_EXPR);
    node->inheritComputedValue(expr);

    SWAG_VERIFY(node->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, node->childs[0]->token, "can't evaluate expression at compile time"}));

    auto typeInfo = TypeManager::flattenType(node->typeInfo);

    g_Log.lock();
    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
        g_Log.print(node->computedValue.reg.b ? "true" : "false");
        break;
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::SX:
        g_Log.print(to_string(node->computedValue.reg.s64));
        break;
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::UX:
        g_Log.print(to_string(node->computedValue.reg.u64));
        break;
    case NativeType::F32:
    case NativeType::F64:
    case NativeType::FX:
        g_Log.print(to_string(node->computedValue.reg.f64));
        break;
    case NativeType::Char:
        g_Log.print(to_string(node->computedValue.reg.ch));
        break;
    case NativeType::String:
        g_Log.print(node->computedValue.text);
        break;
    default:
        assert(false);
        break;
    }

    g_Log.eol();
    g_Log.unlock();
    context->result = SemanticResult::Done;
    return true;
}
