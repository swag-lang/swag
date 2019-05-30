#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "Log.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"

bool SemanticJob::resolveCompilerRun(SemanticContext* context)
{
    auto node = context->node;
    auto expr = context->node->childs[0];

    node->typeInfo = expr->typeInfo;
    node->inheritAndFlag(expr, AST_CONST_EXPR);
    node->inheritComputedValue(expr);

    if (!(node->flags & AST_VALUE_COMPUTED))
    {
        ByteCodeRunContext runContext;
        SWAG_CHECK(g_Run.executeNode(&runContext, context, expr));
        node->computedValue.reg = runContext.stack[runContext.sp - 1].reg;
    }

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
