#include "pch.h"
#include "SourceFile.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Module.h"
#include "ByteCodeGenJob.h"
#include "Workspace.h"
#include "SyntaxJob.h"

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

    // Request to generate the corresponding bytecode
    {
        ByteCodeGenJob::askForByteCode(context->job, node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED);
        if (context->result == ContextResult::Pending)
            return true;
    }

    // Before executing the node, we need to be sure that our dependencies have generated their dll
    auto module = sourceFile->module;
    if (!module->WaitForDependenciesDone(context->job))
    {
        context->result = ContextResult::Pending;
        return true;
    }

    SWAG_CHECK(module->executeNode(sourceFile, node, context));
    return true;
}

bool SemanticJob::resolveCompilerBake(SemanticContext* context)
{
    auto node = context->node;
    auto expr = node->childs.front();
    auto type = expr->typeInfo;

    SWAG_VERIFY(type->kind == TypeInfoKind::Struct, context->report({expr, expr->token, format("cannot bake type '%s'", type->name.c_str())}));
    SWAG_VERIFY(type->declNode->flags & AST_FROM_GENERIC, context->report({expr, expr->token, format("cannot bake type '%s' because it's not generic", type->name.c_str())}));

    // If flag AST_FROM_GENERIC is not set, that means that the generic has been instantiated by someone else, which is bad because if was
    // not done here, and a generic with a given type can only be instantiated once !
    if (!(node->flags & AST_FROM_GENERIC))
    {
        Diagnostic diag(expr, expr->token, format("fail to bake type '%s' because it's already instantiated", type->name.c_str()));
        SWAG_ASSERT(type->kind == TypeInfoKind::Struct);
        AstStruct* structNode = CastAst<AstStruct>(type->declNode, AstNodeKind::StructDecl);
        Diagnostic note(structNode->ownerGeneric, structNode->ownerGeneric->token, "this is where it was instantiated", DiagnosticLevel::Note);
        return context->report(diag, &note);
    }

    // Bake behave like a type alias when done
    SWAG_CHECK(resolveTypeAlias(context));
    return true;
}

bool SemanticJob::resolveCompilerForeignLib(SemanticContext* context)
{
    auto node   = context->node;
    auto module = context->sourceFile->module;
    module->addForeignLib(node->childs.front()->token.text);
    return true;
}

bool SemanticJob::resolveCompilerRun(SemanticContext* context)
{
    auto node = CastAst<AstCompilerRun>(context->node, AstNodeKind::CompilerRun);
    if (node->flags & AST_IS_GENERIC)
        return true;

    auto expression = context->node->childs.front();
    SWAG_CHECK(executeNode(context, expression, false));
    if (context->result != ContextResult::Done)
        return true;

    context->node->inheritComputedValue(expression);
    context->node->typeInfo = expression->typeInfo;
    return true;
}

bool SemanticJob::resolveCompilerAstExpression(SemanticContext* context)
{
    auto node = CastAst<AstCompilerAst>(context->node, AstNodeKind::CompilerAst);
    if (node->flags & AST_IS_GENERIC)
        return true;

    auto job        = context->job;
    auto expression = context->node->childs.back();
    auto typeInfo   = TypeManager::concreteType(expression->typeInfo);
    SWAG_VERIFY(typeInfo->isNative(NativeTypeKind::String), context->report({expression, format("#ast expression is not 'string' ('%s' provided)", expression->typeInfo->name.c_str())}));

    SWAG_CHECK(executeNode(context, expression, true));
    if (context->result != ContextResult::Done)
        return true;

    SWAG_VERIFY(expression->flags & AST_VALUE_COMPUTED, context->report({expression, "expression cannot be evaluated at compile time"}));

    if (!expression->computedValue.text.empty())
    {
        SyntaxJob syntaxJob;
        context->node->childs.clear();
        syntaxJob.constructEmbedded(expression->computedValue.text, context->node, expression, node->embeddedKind);

        job->nodes.pop_back();
        for (int i = (int) context->node->childs.size() - 1; i >= 0; i--)
            job->nodes.push_back(context->node->childs[i]);
        job->nodes.push_back(context->node);
    }

    return true;
}

bool SemanticJob::resolveCompilerAssert(SemanticContext* context)
{
    auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

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

    node->flags |= AST_NO_BYTECODE;

    if (!expr->computedValue.reg.b)
    {
        if (node->childs.size() > 1)
        {
            auto msg = node->childs[1];
            context->report({node, node->token, format("%s", msg->computedValue.text.c_str())});
        }
        else
            context->report({node, node->token, "compiler assertion failed"});
        return false;
    }

    return true;
}

bool SemanticJob::resolveCompilerMacro(SemanticContext* context)
{
    auto node             = context->node;
    auto scope            = node->childs.back()->ownerScope;
    scope->startStackSize = node->ownerScope->startStackSize;

    // Be sure #macro is used inside a macro
    if (!node->ownerInline || (node->ownerInline->attributeFlags & ATTRIBUTE_MIXIN) || !(node->ownerInline->attributeFlags & ATTRIBUTE_MACRO))
        return context->report({node, node->token, "'#macro' can only be used inside a 'swag.macro' function"});

    return true;
}

bool SemanticJob::resolveCompilerInline(SemanticContext* context)
{
    auto node             = context->node;
    auto scope            = node->childs.back()->ownerScope;
    scope->startStackSize = node->ownerScope->startStackSize;
    return true;
}

bool SemanticJob::resolveCompilerMixin(SemanticContext* context)
{
    auto node = CastAst<AstCompilerMixin>(context->node, AstNodeKind::CompilerMixin);

    if (node->doneFlags & AST_DONE_COMPILER_INSERT)
    {
        node->typeInfo = node->childs.back()->typeInfo;
        return true;
    }

    node->doneFlags |= AST_DONE_COMPILER_INSERT;

    auto expr = node->childs[0];
    SWAG_VERIFY(expr->typeInfo->kind == TypeInfoKind::Code, context->report({expr, format("expression should be of type 'code' ('%s' provided)", expr->typeInfo->name.c_str())}));

    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;
    expr->flags |= AST_NO_BYTECODE;

    auto identifier = CastAst<AstIdentifier>(node->ownerInline->parent, AstNodeKind::Identifier);
    for (auto child : identifier->callParameters->childs)
    {
        auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
        SWAG_ASSERT(param->resolvedParameter);
        if (param->resolvedParameter->namedParam == expr->name)
        {
            auto typeCode = CastTypeInfo<TypeInfoCode>(param->typeInfo, TypeInfoKind::Code);

            CloneContext cloneContext;
            cloneContext.parent         = node;
            cloneContext.parentScope    = node->ownerScope;
            cloneContext.ownerBreakable = node->ownerBreakable;
            cloneContext.ownerInline    = node->ownerInline;
            cloneContext.replaceTokens  = node->replaceTokens;
            auto cloneContent           = typeCode->content->clone(cloneContext);
            cloneContent->flags &= ~AST_NO_SEMANTIC;
            node->typeInfo = cloneContent->typeInfo;
            context->job->nodes.push_back(cloneContent);
            context->result = ContextResult::NewChilds;

            return true;
        }
    }

    return context->report({expr, format("unknown user code identifier '%s' (did you forget a back tick ?)", expr->name.c_str())});
}

bool SemanticJob::preResolveCompilerInstruction(SemanticContext* context)
{
    auto node = context->node;

    if (!(node->flags & AST_FROM_GENERIC))
    {
        // If we are inside a generic structure, do not evaluate the instruction. 
        // Will be done during instantiation
        if (node->ownerStructScope && node->ownerStructScope->owner->flags & AST_IS_GENERIC)
            node->flags |= AST_IS_GENERIC;
    }

    if (node->flags & AST_IS_GENERIC)
    {
        node->childs.back()->flags |= AST_NO_SEMANTIC;
        node->flags |= AST_SEMANTIC_ON_CLONE;
    }

    return true;
}

bool SemanticJob::resolveCompilerPrint(SemanticContext* context)
{
    auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

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
    block->flags |= AST_NO_SEMANTIC;

    // Unregister one overload
    for (auto symbol : block->symbols)
    {
        scoped_lock lk(symbol->mutex);
        SymTable::decreaseOverloadNoLock(symbol);
    }

    // Decrease interfaces count to resolve
    for (auto typeStruct : block->interfacesCount)
        decreaseInterfaceCount(typeStruct);

    // Decrease methods count to resolve
    for (auto typeStructPair : block->methodsCount)
    {
        // Remove the corresponding method
        auto typeStruct = typeStructPair.first;
        typeStruct->methods.erase_unordered(typeStructPair.second);

        // Then decrease wanted method number count
        decreaseMethodCount(typeStruct);
    }

    // Do the same for all embedded blocks
    for (auto p : block->blocks)
        disableCompilerIfBlock(context, p);
}

bool SemanticJob::resolveCompilerIf(SemanticContext* context)
{
    auto node = CastAst<AstIf>(context->node->parent, AstNodeKind::CompilerIf);
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    SWAG_CHECK(executeNode(context, node->boolExpression, true));
    if (context->result == ContextResult::Pending)
        return true;

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
        node->setFlagsValueIsComputed();
        return true;

    case TokenId::CompilerBuildCfg:
        node->computedValue.text = g_CommandLine.buildCfg;
        node->typeInfo           = g_TypeMgr.typeInfoString;
        node->setFlagsValueIsComputed();
        return true;
    case TokenId::CompilerArch:
        node->computedValue.text = g_Workspace.GetArchName();
        node->typeInfo           = g_TypeMgr.typeInfoString;
        node->setFlagsValueIsComputed();
        return true;
    case TokenId::CompilerOs:
        node->computedValue.text = g_Workspace.GetOsName();
        node->typeInfo           = g_TypeMgr.typeInfoString;
        node->setFlagsValueIsComputed();
        return true;
    case TokenId::CompilerHasTag:
    {
        auto front = node->childs.front();
        SWAG_CHECK(evaluateConstExpression(context, front));
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_VERIFY(front->flags & AST_VALUE_COMPUTED, context->report({front, "'#hastag' name cannot be evaluated at compile time"}));
        SWAG_VERIFY(front->typeInfo->isNative(NativeTypeKind::String), context->report({front, format("'#hastag' parameter must be a string ('%s' provided)", front->typeInfo->name.c_str())}));
        auto tag                  = g_Workspace.hasTag(front->computedValue.text);
        node->typeInfo            = g_TypeMgr.typeInfoBool;
        node->computedValue.reg.b = tag ? true : false;
        node->setFlagsValueIsComputed();
        return true;
    }
    case TokenId::CompilerTagVal:
    {
        auto front = node->childs.front();
        auto back  = node->childs.back();
        SWAG_CHECK(evaluateConstExpression(context, back));
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_VERIFY(back->flags & AST_VALUE_COMPUTED, context->report({back, "'#tagval' name cannot be evaluated at compile time"}));
        SWAG_VERIFY(back->typeInfo->isNative(NativeTypeKind::String), context->report({back, format("'#tagval' parameter must be a string ('%s' provided)", back->typeInfo->name.c_str())}));
        node->typeInfo = front->typeInfo;
        auto tag       = g_Workspace.hasTag(back->computedValue.text);
        if (tag)
        {
            if (!TypeManager::makeCompatibles(context, front->typeInfo, tag->type, nullptr, front, CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            {
                Diagnostic diag(front, front->token, format("type '%s' and type '%s' defined in the command line for '%s' are incompatible", front->typeInfo->name.c_str(), tag->type->name.c_str(), tag->name.c_str()));
                Diagnostic note(front, front->token, format("this is the related command line option: '%s'", tag->cmdLine.c_str()), DiagnosticLevel::Note);
                note.hasFile     = false;
                note.printSource = false;
                return context->report(diag, &note);
            }

            node->typeInfo      = tag->type;
            node->computedValue = tag->value;
        }
        node->setFlagsValueIsComputed();
        return true;
    }

    case TokenId::CompilerLocation:
        node->typeInfo                 = g_Workspace.swagScope.regTypeInfoSourceLoc;
        node->computedValue.reg.offset = ByteCodeGenJob::computeSourceLocation(node);
        node->setFlagsValueIsComputed();
        return true;
    case TokenId::CompilerCallerLocation:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, "'#callerlocation' can only be set in a function parameter declaration"}));
        node->typeInfo = g_Workspace.swagScope.regTypeInfoSourceLoc;
        return true;
    case TokenId::CompilerCallerFunction:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, "'#callerfunction' can only be set in a function parameter declaration"}));
        node->typeInfo = g_TypeMgr.typeInfoString;
        return true;

    default:
        return internalError(context, "resolveCompilerFunction, unknown token");
    }
}