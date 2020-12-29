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
#include "LoadFileJob.h"

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
    SWAG_VERIFY(typeInfo->isNative(NativeTypeKind::String), context->report({expression, format("'#ast' expression is not 'string' ('%s' provided)", expression->typeInfo->name.c_str())}));

    SWAG_CHECK(executeNode(context, expression, true));
    if (context->result != ContextResult::Done)
        return true;

    SWAG_VERIFY(expression->flags & AST_VALUE_COMPUTED, context->report({expression, "expression cannot be evaluated at compile time"}));

    if (!expression->computedValue.text.empty())
    {
        SyntaxJob syntaxJob;
        node->childs.clear();
        syntaxJob.constructEmbedded(expression->computedValue.text, node, expression, node->embeddedKind, true);

        job->nodes.pop_back();
        for (int i = (int) node->childs.size() - 1; i >= 0; i--)
            job->nodes.push_back(node->childs[i]);
        job->nodes.push_back(node);
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
    auto node             = CastAst<AstCompilerInline>(context->node, AstNodeKind::CompilerInline);
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
        if (param->resolvedParameter->namedParam == expr->token.text)
        {
            auto typeCode = CastTypeInfo<TypeInfoCode>(param->typeInfo, TypeInfoKind::Code);

            CloneContext cloneContext;
            cloneContext.parent         = node;
            cloneContext.parentScope    = node->ownerScope;
            cloneContext.ownerBreakable = node->ownerBreakable;
            cloneContext.ownerInline    = node->ownerInline;
            cloneContext.replaceTokens  = node->replaceTokens;
            cloneContext.forceFlags     = AST_IN_MIXIN;
            auto cloneContent           = typeCode->content->clone(cloneContext);
            cloneContent->flags &= ~AST_NO_SEMANTIC;
            node->typeInfo = cloneContent->typeInfo;
            context->job->nodes.push_back(cloneContent);
            context->result = ContextResult::NewChilds;

            return true;
        }
    }

    return context->report({expr, format("unknown user code identifier '%s' (did you forget a back tick ?)", expr->token.text.c_str())});
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
    if (typeInfo->kind == TypeInfoKind::Native)
    {
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
        case NativeTypeKind::Int:
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
        case NativeTypeKind::UInt:
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
            g_Log.print(format("<%s>", typeInfo->name.c_str()));
            break;
        }
    }
    else
    {
        g_Log.print(format("<%s>", typeInfo->name.c_str()));
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

bool SemanticJob::resolveCompilerLoad(SemanticContext* context)
{
    auto job    = context->job;
    auto node   = context->node;
    auto module = context->sourceFile->module;
    auto back   = node->childs[0];

    SWAG_VERIFY(back->flags & AST_VALUE_COMPUTED, context->report({back, "filename cannot be evaluated at compile time"}));
    SWAG_VERIFY(back->typeInfo == g_TypeMgr.typeInfoString, context->report({back, format("'#load' parameter should be of type string ('%s' provided)", back->typeInfo->name.c_str())}));

    if (!(node->doneFlags & AST_DONE_LOAD))
    {
        node->doneFlags |= AST_DONE_LOAD;

        auto filename = back->computedValue.text.c_str();
        SWAG_VERIFY(fs::exists(filename), context->report({back, format("cannot find file '%s'", back->computedValue.text.c_str())}));

        struct stat stat_buf;
        int         rc = stat(filename, &stat_buf);
        SWAG_VERIFY(rc == 0, context->report({back, format("cannot open file '%s'", back->computedValue.text.c_str())}));
        SWAG_CHECK(checkSizeOverflow(context, "'#load'", stat_buf.st_size, SWAG_LIMIT_COMPILER_LOAD));

        auto newJob                    = g_Pool_loadFileJob.alloc();
        node->computedValue.reg.offset = module->tempSegment.reserve(stat_buf.st_size);
        newJob->destBuffer             = module->tempSegment.address(node->computedValue.reg.u32);
        newJob->sizeBuffer             = stat_buf.st_size;

        newJob->module       = module;
        newJob->sourcePath   = back->computedValue.text;
        newJob->dependentJob = job->dependentJob;
        newJob->addDependentJob(job);
        job->jobsToAdd.push_back(newJob);
        job->setPending(nullptr, "LOAD_FILE", node, nullptr);

        // Creates return type
        auto ptrArray         = allocType<TypeInfoArray>();
        ptrArray->count       = stat_buf.st_size;
        ptrArray->pointedType = g_TypeMgr.typeInfoU8;
        ptrArray->finalType   = g_TypeMgr.typeInfoU8;
        ptrArray->sizeOf      = ptrArray->count;
        ptrArray->totalCount  = stat_buf.st_size;
        ptrArray->computeName();
        node->typeInfo = ptrArray;

        return true;
    }

    node->setFlagsValueIsComputed();
    return true;
}

bool SemanticJob::resolveCompilerDefined(SemanticContext* context)
{
    auto node                 = context->node;
    node->computedValue.reg.b = node->childs.back()->resolvedSymbolOverload != nullptr;
    node->typeInfo            = g_TypeMgr.typeInfoBool;
    node->setFlagsValueIsComputed();
    return true;
}

bool SemanticJob::resolveCompilerScopeFct(SemanticContext* context)
{
    auto node          = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    auto identifierRef = node->identifierRef;

    SWAG_VERIFY(!identifierRef->startScope, context->report({node, "an identifier marked with '#scopefct' cannot be used in a scope"}));
    SWAG_VERIFY(node->ownerFct, context->report({node, "an identifier marked with '#scopefct' can only be used inside a function"}));

    node->semFlags |= AST_SEM_FORCE_SCOPE;
    node->typeInfo = g_TypeMgr.typeInfoVoid;

    identifierRef->previousResolvedNode = node;
    identifierRef->startScope           = node->ownerFct->scope;
    node->flags |= AST_NO_BYTECODE;

    return true;
}

bool SemanticJob::resolveCompilerSpecialFunction(SemanticContext* context)
{
    auto node = context->node;

    switch (node->token.id)
    {
    case TokenId::CompilerFunction:
        SWAG_VERIFY(node->ownerFct, context->report({node, "'#function' can only be called inside a function"}));
        node->computedValue.text = node->ownerFct->token.text;
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