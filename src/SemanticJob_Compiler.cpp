#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "Module.h"
#include "ByteCodeGenJob.h"
#include "Workspace.h"
#include "LoadFileJob.h"
#include "ByteCode.h"
#include "Backend.h"
#include "Os.h"
#include "ErrorIds.h"

bool SemanticJob::executeNode(SemanticContext* context, AstNode* node, bool onlyconstExpr)
{
    // No need to run, this is already baked
    if (node->flags & AST_VALUE_COMPUTED)
        return true;

    auto sourceFile = context->sourceFile;
    if (onlyconstExpr)
    {
        SWAG_VERIFY(node->flags & AST_CONST_EXPR, context->report({node, Msg0798}));
    }

    // Request to generate the corresponding bytecode
    context->expansionNode.push_back({node, JobContext::ExpansionType::Node});
    ByteCodeGenJob::askForByteCode(context->job, node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED);
    if (context->result == ContextResult::Pending)
    {
        context->expansionNode.pop_back();
        return true;
    }

    // Before executing the node, we need to be sure that our dependencies have generated their dll
    // In case there's a foreign call somewhere...
    auto module = sourceFile->module;
    if (node->extension->bc->hasFunctionCalls)
    {
        if (!module->waitForDependenciesDone(context->job))
        {
            SWAG_ASSERT(node->extension->bc->hasFunctionCalls);
            context->expansionNode.pop_back();
            context->result = ContextResult::Pending;
            return true;
        }
    }

    SWAG_CHECK(module->executeNode(sourceFile, node, context));
    context->expansionNode.pop_back();
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
    auto node = CastAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerRun);
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

bool SemanticJob::resolveCompilerSelectIfExpression(SemanticContext* context)
{
    auto node = CastAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerSelectIf, AstNodeKind::CompilerCheckIf);
    if (node->flags & AST_IS_GENERIC)
        return true;

    auto expression = context->node->childs.back();
    auto typeInfo   = TypeManager::concreteType(expression->typeInfo);
    SWAG_VERIFY(typeInfo->isNative(NativeTypeKind::Bool), context->report({expression, Utf8::format(Msg0233, expression->typeInfo->getDisplayName().c_str())}));

    return true;
}

bool SemanticJob::resolveCompilerAstExpression(SemanticContext* context)
{
    auto node = CastAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerAst);
    if (node->flags & AST_IS_GENERIC)
        return true;

    auto job        = context->job;
    auto expression = context->node->childs.back();
    auto typeInfo   = TypeManager::concreteType(expression->typeInfo);
    SWAG_VERIFY(typeInfo->isNative(NativeTypeKind::String), context->report({expression, Utf8::format(Msg0234, expression->typeInfo->getDisplayName().c_str())}));

    SWAG_CHECK(executeNode(context, expression, true));
    if (context->result != ContextResult::Done)
        return true;

    SWAG_VERIFY(expression->flags & AST_VALUE_COMPUTED, context->report({expression, Msg0798}));

    if (!expression->computedValue->text.empty())
    {
        SyntaxJob syntaxJob;
        node->childs.clear();
        syntaxJob.constructEmbedded(expression->computedValue->text, node, expression, node->embeddedKind, true);

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
        SWAG_VERIFY(msg->typeInfo->isNative(NativeTypeKind::String), context->report({msg, Msg0236}));
        SWAG_VERIFY(msg->flags & AST_VALUE_COMPUTED, context->report({msg, Msg0237}));
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, expr, CASTFLAG_AUTO_BOOL));
    SWAG_CHECK(executeNode(context, expr, true));
    if (context->result == ContextResult::Pending)
        return true;

    node->flags |= AST_NO_BYTECODE;

    if (!expr->computedValue->reg.b)
    {
        if (node->childs.size() > 1)
        {
            auto msg = node->childs[1];
            context->report({node, node->token, Utf8::format("%s", msg->computedValue->text.c_str())});
        }
        else
            context->report({node, node->token, Msg0238});
        return false;
    }

    return true;
}

bool SemanticJob::resolveCompilerMacro(SemanticContext* context)
{
    auto node             = context->node;
    auto scope            = node->childs.back()->ownerScope;
    scope->startStackSize = node->ownerScope->startStackSize;

    node->allocateExtension();
    node->extension->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

    // Be sure #macro is used inside a macro
    if (!node->ownerInline || (node->ownerInline->attributeFlags & ATTRIBUTE_MIXIN) || !(node->ownerInline->attributeFlags & ATTRIBUTE_MACRO))
        return context->report({node, node->token, Msg0239});

    return true;
}

bool SemanticJob::resolveCompilerInline(SemanticContext* context)
{
    auto node             = CastAst<AstCompilerInline>(context->node, AstNodeKind::CompilerInline);
    auto scope            = node->childs.back()->ownerScope;
    scope->startStackSize = node->ownerScope->startStackSize;

    node->allocateExtension();
    node->extension->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

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
    SWAG_VERIFY(expr->typeInfo->kind == TypeInfoKind::Code, context->report({expr, Utf8::format(Msg0240, expr->typeInfo->getDisplayName().c_str())}));

    node->allocateExtension();
    node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitDebugNop;
    node->byteCodeFct                  = ByteCodeGenJob::emitDebugNop;
    expr->flags |= AST_NO_BYTECODE;

    auto typeCode = CastTypeInfo<TypeInfoCode>(expr->typeInfo, TypeInfoKind::Code);
    SWAG_ASSERT(typeCode->content);

    CloneContext cloneContext;
    cloneContext.parent                 = node;
    cloneContext.parentScope            = node->ownerScope;
    cloneContext.ownerBreakable         = node->ownerBreakable;
    cloneContext.replaceTokensBreakable = node->ownerBreakable;
    cloneContext.ownerInline            = node->ownerInline;
    cloneContext.replaceTokens          = node->replaceTokens;
    cloneContext.forceFlags             = AST_IN_MIXIN;
    cloneContext.ownerFct               = node->ownerFct;
    auto cloneContent                   = typeCode->content->clone(cloneContext);
    cloneContent->allocateExtension();
    cloneContent->extension->alternativeNode = typeCode->content->parent;
    cloneContent->extension->alternativeScopes.push_back(typeCode->content->parent->ownerScope);
    cloneContent->flags &= ~AST_NO_SEMANTIC;
    node->typeInfo = cloneContent->typeInfo;
    context->job->nodes.push_back(cloneContent);
    context->result = ContextResult::NewChilds;

    return true;
}

bool SemanticJob::preResolveCompilerInstruction(SemanticContext* context)
{
    auto node = context->node;
    if (!(node->flags & AST_FROM_GENERIC))
    {
        if (!node->ownerFct || !(node->ownerFct->flags & AST_FROM_GENERIC))
        {
            // If we are inside a generic structure, do not evaluate the instruction.
            // Will be done during instantiation
            if (node->ownerStructScope && node->ownerStructScope->owner->flags & AST_IS_GENERIC)
            {
                // Be sure we are inside a struct definition, and not in an impl block
                auto parent = node->parent;
                while (parent)
                {
                    if (parent->kind == AstNodeKind::StructDecl)
                    {
                        node->flags |= AST_IS_GENERIC;
                        break;
                    }

                    if (parent->kind == AstNodeKind::Impl)
                        break;
                    parent = parent->parent;
                }
            }

            // Same for a function
            if (node->ownerFct && (node->ownerFct->flags & AST_IS_GENERIC))
                node->flags |= AST_IS_GENERIC;
        }
    }

    if (node->flags & AST_IS_GENERIC)
    {
        node->childs.back()->flags |= AST_NO_SEMANTIC;
        node->semFlags |= AST_SEM_ON_CLONE;
    }

    return true;
}

bool SemanticJob::resolveCompilerTestError(SemanticContext* context)
{
    // Should never be reached
    return context->report({context->node, context->node->token, Msg0241});
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
            g_Log.print(expr->computedValue->reg.b ? "true" : "false");
            break;
        case NativeTypeKind::S8:
            g_Log.print(to_string(expr->computedValue->reg.s8));
            break;
        case NativeTypeKind::S16:
            g_Log.print(to_string(expr->computedValue->reg.s16));
            break;
        case NativeTypeKind::S32:
            g_Log.print(to_string(expr->computedValue->reg.s32));
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            g_Log.print(to_string(expr->computedValue->reg.s64));
            break;
        case NativeTypeKind::U8:
            g_Log.print(to_string(expr->computedValue->reg.u8));
            break;
        case NativeTypeKind::U16:
            g_Log.print(to_string(expr->computedValue->reg.u16));
            break;
        case NativeTypeKind::U32:
            g_Log.print(to_string(expr->computedValue->reg.u32));
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            g_Log.print(to_string(expr->computedValue->reg.u64));
            break;
        case NativeTypeKind::F32:
            g_Log.print(to_string(expr->computedValue->reg.f32));
            break;
        case NativeTypeKind::F64:
            g_Log.print(to_string(expr->computedValue->reg.f64));
            break;
        case NativeTypeKind::Rune:
            g_Log.print(to_string(expr->computedValue->reg.ch));
            break;
        case NativeTypeKind::String:
            g_Log.print(expr->computedValue->text);
            break;
        default:
            g_Log.print(Utf8::format("<%s>", typeInfo->getDisplayName().c_str()));
            break;
        }
    }
    else
    {
        g_Log.print(Utf8::format("<%s>", typeInfo->getDisplayName().c_str()));
    }

    g_Log.eol();
    g_Log.unlock();
    return true;
}

void SemanticJob::disableCompilerIfBlock(SemanticContext* context, AstCompilerIfBlock* block)
{
    block->flags |= AST_NO_BYTECODE;
    block->flags |= AST_NO_SEMANTIC;

    // Revert test errors in case #global testerror is inside a disabled #if branch
    auto sourceFile = context->sourceFile;
    sourceFile->numTestErrors -= block->numTestErrors;
    sourceFile->module->numTestErrors -= block->numTestErrors;
    sourceFile->numTestWarnings -= block->numTestWarnings;
    sourceFile->module->numTestWarnings -= block->numTestWarnings;

    // Unregister one overload
    for (auto it : block->symbols)
    {
        scoped_lock lk(it.second->mutex);
        scoped_lock lk1(it.first->mutex);
        it.first->flags |= AST_NO_SEMANTIC | AST_NO_BYTECODE;
        it.first->semFlags |= AST_SEM_DISABLED;
        SymTable::disabledIfBlockOverloadNoLock(it.first, it.second);
    }

    // Decrease interfaces count to resolve
    for (auto typeStruct : block->interfacesCount)
        decreaseInterfaceCount(typeStruct);

    // Decrease methods count to resolve
    set<TypeInfoStruct*> allStructs;
    for (auto typeStructPair : block->methodsCount)
    {
        // Remove the corresponding method
        auto typeStruct = typeStructPair.first;
        allStructs.insert(typeStruct);

        auto idx                 = typeStructPair.second;
        typeStruct->methods[idx] = nullptr;

        // Then decrease wanted method number count
        decreaseMethodCount(typeStruct);
    }

    // Clean array
    for (auto typeStruct : allStructs)
    {
        for (int i = 0; i < typeStruct->methods.size(); i++)
        {
            if (!typeStruct->methods[i])
            {
                typeStruct->methods.erase_unordered(i);
                i--;
            }
        }
    }

    // Eliminate imports
    for (auto imp : block->imports)
        sourceFile->module->removeDependency(imp);

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
    AstCompilerIfBlock* validatedNode = nullptr;
    if (node->boolExpression->computedValue->reg.b)
    {
        validatedNode = node->ifBlock;
        if (node->elseBlock)
            disableCompilerIfBlock(context, node->elseBlock);
    }
    else
    {
        validatedNode = node->elseBlock;
        disableCompilerIfBlock(context, node->ifBlock);
    }

    // We can know solve function sub declarations in that block
    if (validatedNode)
    {
        for (auto n : validatedNode->subDecls)
            launchResolveSubDecl(context, n);
    }

    return true;
}

bool SemanticJob::resolveCompilerLoad(SemanticContext* context)
{
    auto job    = context->job;
    auto node   = context->node;
    auto module = context->sourceFile->module;
    auto back   = node->childs[0];

    SWAG_VERIFY(back->flags & AST_VALUE_COMPUTED, context->report({back, Msg0242}));
    SWAG_VERIFY(back->typeInfo == g_TypeMgr.typeInfoString, context->report({back, Utf8::format(Msg0243, back->typeInfo->getDisplayName().c_str())}));
    node->setFlagsValueIsComputed();

    if (!(node->doneFlags & AST_DONE_LOAD))
    {
        node->doneFlags |= AST_DONE_LOAD;

        auto filename = back->computedValue->text;
        Utf8 fullFileName;

        // Search first in the same folder as the source file
        fullFileName = fs::path(node->sourceFile->path.c_str()).parent_path().string();
        fullFileName += "/";
        fullFileName += filename;
        if (!fs::exists(fullFileName.c_str()))
        {
            // Search relative to the module path
            fullFileName = node->sourceFile->module->path;
            fullFileName += "/";
            fullFileName += filename;
            if (!fs::exists(fullFileName.c_str()))
            {
                // Search the file itself, without any special path
                fullFileName = filename;
                if (!fs::exists(fullFileName.c_str()))
                    return context->report({back, Utf8::format(Msg0244, filename.c_str())});
            }
        }

        struct stat stat_buf;
        int         rc = stat(fullFileName, &stat_buf);
        SWAG_VERIFY(rc == 0, context->report({back, Utf8::format(Msg0223, back->computedValue->text.c_str())}));
        SWAG_CHECK(checkSizeOverflow(context, "'#load'", stat_buf.st_size, SWAG_LIMIT_COMPILER_LOAD));

        auto newJob                         = g_Allocator.alloc<LoadFileJob>();
        auto storageSegment                 = getConstantSegFromContext(node);
        node->computedValue->storageOffset  = storageSegment->reserve(stat_buf.st_size);
        node->computedValue->storageSegment = storageSegment;
        newJob->destBuffer                  = storageSegment->address(node->computedValue->storageOffset);
        newJob->sizeBuffer                  = stat_buf.st_size;

        newJob->module       = module;
        newJob->sourcePath   = fullFileName;
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

    return true;
}

bool SemanticJob::resolveCompilerDefined(SemanticContext* context)
{
    auto node = context->node;
    node->setFlagsValueIsComputed();
    node->computedValue->reg.b = node->childs.back()->resolvedSymbolOverload != nullptr;
    node->typeInfo             = g_TypeMgr.typeInfoBool;
    return true;
}

bool SemanticJob::resolveCompilerScopeFct(SemanticContext* context)
{
    auto node          = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    auto identifierRef = node->identifierRef;

    SWAG_VERIFY(!identifierRef->startScope, context->report({node, Msg0246}));
    SWAG_VERIFY(node->ownerFct, context->report({node, Msg0247}));

    node->semFlags |= AST_SEM_FORCE_SCOPE;
    node->typeInfo = g_TypeMgr.typeInfoVoid;

    identifierRef->previousResolvedNode = node;
    if (node->ownerInline)
        identifierRef->startScope = node->ownerInline->parametersScope;
    else
        identifierRef->startScope = node->ownerFct->scope;
    node->flags |= AST_NO_BYTECODE;

    return true;
}

bool SemanticJob::resolveCompilerSpecialFunction(SemanticContext* context)
{
    auto node = context->node;

    switch (node->token.id)
    {
    case TokenId::CompilerBuildCfg:
        node->setFlagsValueIsComputed();
        node->computedValue->text = g_CommandLine.buildCfg;
        node->typeInfo            = g_TypeMgr.typeInfoString;
        return true;

    case TokenId::CompilerArch:
        node->setFlagsValueIsComputed();
        node->computedValue->text = Backend::GetArchName();
        node->typeInfo            = g_TypeMgr.typeInfoString;
        return true;

    case TokenId::CompilerOs:
        node->setFlagsValueIsComputed();
        node->computedValue->text = Backend::GetOsName();
        node->typeInfo            = g_TypeMgr.typeInfoString;
        return true;

    case TokenId::CompilerAbi:
        node->setFlagsValueIsComputed();
        node->computedValue->text = Backend::GetAbiName();
        node->typeInfo            = g_TypeMgr.typeInfoString;
        return true;

    case TokenId::CompilerHasTag:
    {
        auto front = node->childs.front();
        SWAG_CHECK(evaluateConstExpression(context, front));
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_VERIFY(front->flags & AST_VALUE_COMPUTED, context->report({front, Msg0248}));
        SWAG_VERIFY(front->typeInfo->isNative(NativeTypeKind::String), context->report({front, Utf8::format(Msg0249, front->typeInfo->getDisplayName().c_str())}));
        auto tag       = g_Workspace.hasTag(front->computedValue->text);
        node->typeInfo = g_TypeMgr.typeInfoBool;
        node->setFlagsValueIsComputed();
        node->computedValue->reg.b = tag ? true : false;
        return true;
    }

    case TokenId::CompilerGetTag:
    {
        auto nameNode   = node->childs[0];
        auto typeNode   = node->childs[1];
        auto defaultVal = node->childs[2];
        SWAG_CHECK(evaluateConstExpression(context, nameNode));
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_CHECK(evaluateConstExpression(context, defaultVal));
        if (context->result == ContextResult::Pending)
            return true;

        SWAG_VERIFY(nameNode->flags & AST_VALUE_COMPUTED, context->report({nameNode, Msg0250}));
        SWAG_VERIFY(!(nameNode->flags & AST_VALUE_IS_TYPEINFO), context->report({nameNode, Msg0245}));
        SWAG_VERIFY(nameNode->typeInfo->isNative(NativeTypeKind::String), context->report({nameNode, Utf8::format(Msg0251, nameNode->typeInfo->getDisplayName().c_str())}));
        SWAG_VERIFY(!(defaultVal->flags & AST_VALUE_IS_TYPEINFO), context->report({defaultVal, Msg0283}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, defaultVal->typeInfo, nullptr, defaultVal, CASTFLAG_DEFAULT));

        node->typeInfo = typeNode->typeInfo;
        node->setFlagsValueIsComputed();

        auto tag = g_Workspace.hasTag(nameNode->computedValue->text);
        if (tag)
        {
            if (!TypeManager::makeCompatibles(context, typeNode->typeInfo, tag->type, nullptr, typeNode, CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            {
                Diagnostic diag(typeNode, typeNode->token, Utf8::format(Msg0252, typeNode->typeInfo->getDisplayName().c_str(), tag->type->getDisplayName().c_str(), tag->name.c_str()));
                Diagnostic note(typeNode, typeNode->token, Utf8::format(Msg0253, tag->cmdLine.c_str()), DiagnosticLevel::Note);
                note.hasFile     = false;
                note.printSource = false;
                return context->report(diag, &note);
            }

            node->typeInfo       = tag->type;
            *node->computedValue = tag->value;
        }
        else
        {
            node->computedValue = defaultVal->computedValue;
        }

        return true;
    }

    case TokenId::CompilerLocation:
    {
        node->typeInfo = g_Workspace.swagScope.regTypeInfoSourceLoc;
        auto locNode   = node;

        if (!node->childs.empty())
        {
            auto resolved = node->childs.front()->resolvedSymbolOverload;
            if (resolved)
                locNode = resolved->node;
        }

        node->setFlagsValueIsComputed();
        ByteCodeGenJob::computeSourceLocation(context, locNode, &node->computedValue->storageOffset, &node->computedValue->storageSegment);
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;
    }

    case TokenId::CompilerCallerLocation:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, Msg0254}));
        node->typeInfo = g_Workspace.swagScope.regTypeInfoSourceLoc;
        return true;

    case TokenId::CompilerCallerFunction:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, Msg0255}));
        node->typeInfo = g_TypeMgr.typeInfoString;
        return true;

    case TokenId::CompilerFunction:
    {
        SWAG_VERIFY(node->ownerFct, context->report({node, Msg0256}));
        node->typeInfo = g_TypeMgr.typeInfoString;
        node->setFlagsValueIsComputed();
        node->computedValue->text = node->ownerFct->getNameForUserCompiler();
        return true;
    }

    default:
        return internalError(context, "resolveCompilerFunction, unknown token");
    }
}