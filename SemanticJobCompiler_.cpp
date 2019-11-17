#include "pch.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
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
        SWAG_VERIFY(node->flags & AST_CONST_EXPR, context->report({node, "expression cannot be evaluated at compile time"}));
    }

    {
        ByteCodeGenJob::askForByteCode(context->job, node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED);
        if (context->result == ContextResult::Pending)
            return true;
    }

    SWAG_CHECK(sourceFile->module->executeNode(sourceFile, node));
    return true;
}

bool SemanticJob::resolveCompilerRun(SemanticContext* context)
{
    auto expression = context->node->childs.front();
    SWAG_CHECK(executeNode(context, expression, false));
    context->node->inheritComputedValue(expression);
    context->node->typeInfo = expression->typeInfo;
    return true;
}

bool SemanticJob::resolveCompilerInsert(SemanticContext* context)
{
    auto node = context->node;

    if (node->doneFlags & AST_DONE_COMPILER_INSERT)
        return true;
    node->doneFlags |= AST_DONE_COMPILER_INSERT;

    auto expr = node->childs[0];
    SWAG_VERIFY(expr->typeInfo->kind == TypeInfoKind::Code, context->report({expr, format("expression should be of type 'code' ('%s' provided)", expr->typeInfo->name.c_str())}));

    node->byteCodeFct = &ByteCodeGenJob::emitPassThrough;
    expr->flags |= AST_NO_BYTECODE;

    auto identifier = CastAst<AstIdentifier>(node->ownerInline->parent, AstNodeKind::Identifier);
    for (auto child : identifier->callParameters->childs)
    {
        auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
        if (param->resolvedParameter->namedParam == expr->name)
        {
            auto typeCode     = CastTypeInfo<TypeInfoCode>(param->typeInfo, TypeInfoKind::Code);

			CloneContext cloneContext;
			cloneContext.parent = node;
			cloneContext.parentScope = node->ownerScope;
			auto cloneContent = typeCode->content->clone(cloneContext);
            cloneContent->flags &= ~AST_DISABLED;
            node->typeInfo = cloneContent->typeInfo;
            context->job->nodes.push_back(cloneContent);
            context->result = ContextResult::NewChilds;
            return true;
        }
    }

    SWAG_ASSERT(false);
    return true;
}

bool SemanticJob::resolveCompilerAssert(SemanticContext* context)
{
    auto node = context->node;
    auto expr = node->childs[0];

    if (node->childs.size() > 1)
    {
        auto msg = node->childs[1];
        SWAG_VERIFY(msg->typeInfo->isNative(NativeTypeKind::String), context->report({msg, "message expression is not a string"}));
        SWAG_VERIFY(msg->flags & AST_VALUE_COMPUTED, context->report({msg, "message expression cannot be evaluated at compile time"}));
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, expr, CASTFLAG_AUTO_BOOL));
    SWAG_CHECK(executeNode(context, expr, true));
    if (context->result == ContextResult::Pending)
        return true;

    if (!expr->computedValue.reg.b)
    {
        if (node->childs.size() > 1)
        {
            auto msg = node->childs[1];
            context->report({expr, format("compiler assertion failed: %s", msg->computedValue.text.c_str())});
        }
        else
            context->report({expr, "compiler assertion failed"});
        return false;
    }

    return true;
}

bool SemanticJob::resolveCompilerPrint(SemanticContext* context)
{
    auto expr = context->node->childs[0];
    SWAG_CHECK(executeNode(context, expr, true));
    if (context->result == ContextResult::Pending)
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

    // Do the same for all embedded blocks
    for (auto p : block->blocks)
        disableCompilerIfBlock(context, p);
}

bool SemanticJob::resolveCompilerIf(SemanticContext* context)
{
    auto node = CastAst<AstIf>(context->node->parent, AstNodeKind::CompilerIf);
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));
    SWAG_VERIFY(node->boolExpression->flags & AST_VALUE_COMPUTED, context->report({node->boolExpression, "expression cannot be evaluated at compile time"}));

    node->flags |= AST_COMPILER_IF_DONE;
    node->boolExpression->flags |= AST_NO_BYTECODE;
    AstNode* validatedNode = nullptr;
    if (node->boolExpression->computedValue.reg.b)
    {
        validatedNode = node->ifBlock;
        if (node->elseBlock)
            disableCompilerIfBlock(context, (AstCompilerIfBlock*) node->elseBlock);
    }
    else
    {
        validatedNode = node->elseBlock;
        disableCompilerIfBlock(context, (AstCompilerIfBlock*) node->ifBlock);
    }

    return true;
}

bool SemanticJob::resolveCompilerSpecialFunction(SemanticContext* context)
{
    auto node = context->node;

    switch (node->token.id)
    {
    case TokenId::CompilerFunction:
        SWAG_VERIFY(node->ownerFct, context->report({node, "'#function' can only be called inside a function"}));
        node->computedValue.text = node->ownerFct->name;
        node->typeInfo           = g_TypeMgr.typeInfoString;
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
        return true;

    case TokenId::CompilerConfiguration:
        node->computedValue.text = context->sourceFile->module->buildParameters.target.config;
        node->typeInfo           = g_TypeMgr.typeInfoString;
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
        return true;
    case TokenId::CompilerPlatform:
        node->computedValue.text = context->sourceFile->module->buildParameters.target.arch;
        node->typeInfo           = g_TypeMgr.typeInfoString;
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
        return true;

    case TokenId::CompilerCallerLine:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, "'#callerline' can only be set in a function parameter declaration"}));
        node->typeInfo = g_TypeMgr.typeInfoU32;
        return true;
    case TokenId::CompilerCallerFile:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, "'#callerfile' can only be set in a function parameter declaration"}));
        node->typeInfo = g_TypeMgr.typeInfoString;
        return true;
    case TokenId::CompilerCallerFunction:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, "'#callerfile' can only be set in a function parameter declaration"}));
        node->typeInfo = g_TypeMgr.typeInfoString;
        return true;

    default:
        return internalError(context, "resolveCompilerFunction, unknown token");
    }
}