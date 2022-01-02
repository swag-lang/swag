#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "Module.h"
#include "ByteCodeGenJob.h"
#include "Workspace.h"
#include "LoadFileJob.h"
#include "ByteCode.h"
#include "Backend.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Os.h"

void SemanticJob::computeNonConstExprNotes(AstNode* node, vector<const Diagnostic*>& notes)
{
    auto                      childs = node->childs;
    VectorNative<SymbolName*> done;
    while (!childs.empty())
    {
        auto c = childs.back();
        childs.pop_back();
        if (c->flags & AST_CONST_EXPR)
            continue;

        if (c->resolvedSymbolName)
        {
            if (done.contains(c->resolvedSymbolName))
                continue;
            done.push_back(c->resolvedSymbolName);

            if (c->resolvedSymbolName->kind == SymbolKind::Function)
            {
                if (c->resolvedSymbolOverload && !(c->resolvedSymbolOverload->node->attributeFlags & ATTRIBUTE_CONSTEXPR))
                {
                    auto note = new Diagnostic(c, Utf8::format(g_E[Nte0042], c->resolvedSymbolName->name.c_str()), DiagnosticLevel::Note);
                    notes.push_back(note);
                }
            }

            if (c->resolvedSymbolName->kind == SymbolKind::Variable)
            {
                auto note = new Diagnostic(c, Utf8::format(g_E[Nte0041], c->resolvedSymbolName->name.c_str()), DiagnosticLevel::Note);
                notes.push_back(note);
            }
        }

        if (c->childs.size())
            childs.append(c->childs);
    }
}

bool SemanticJob::executeCompilerNode(SemanticContext* context, AstNode* node, bool onlyconstExpr)
{
    // No need to run, this is already baked
    if (node->flags & AST_VALUE_COMPUTED)
        return true;

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    if (onlyconstExpr)
    {
        // Given some clues about childs which are not constexpr
        if (!(node->flags & AST_CONST_EXPR))
        {
            Diagnostic                diag{node, g_E[Err0798]};
            vector<const Diagnostic*> notes;
            computeNonConstExprNotes(node, notes);
            return context->report(diag, notes);
        }
    }

    // Request to generate the corresponding bytecode
    context->expansionNodes.push_back({node, JobContext::ExpansionType::Node});
    ByteCodeGenJob::askForByteCode(context->job, node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED);
    context->expansionNodes.pop_back();
    if (context->result != ContextResult::Done)
        return true;

    // Be sure we can deal with the type at compile time
    ExecuteNodeParams execParams;
    if (!(node->semFlags & AST_SEM_EXEC_RET_STACK))
    {
        auto realType = TypeManager::concreteReferenceType(node->typeInfo);
        if (realType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            switch (realType->kind)
            {
            case TypeInfoKind::Struct:
            {
                if (realType->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
                    break;

                if (realType->flags & TYPEINFO_STRUCT_IS_TUPLE)
                    return context->report({node, Utf8::format(g_E[Err0321], realType->getDisplayName().c_str())});

                // It is possible to convert a complex struct to a constant static array of values if the struct
                // implements 'opCount' and 'opSlice'
                context->job->waitAllStructMethods(realType);
                if (context->result != ContextResult::Done)
                    return true;

                auto symCount = hasUserOp(g_LangSpec->name_opCount, (TypeInfoStruct*) realType);
                auto symSlice = hasUserOp(g_LangSpec->name_opSlice, (TypeInfoStruct*) realType);
                if (!symCount || !symSlice)
                    return context->report({node, Utf8::format(g_E[Err0281], realType->getDisplayName().c_str())});

                VectorNative<AstNode*> params;
                SWAG_ASSERT(!context->node->extension || !context->node->extension->resolvedUserOpSymbolOverload);

                // opCount
                params.push_back(node);
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opCount, nullptr, nullptr, node, params, false));
                if (context->result != ContextResult::Done)
                    return true;

                auto extension                          = context->node->extension;
                execParams.specReturnOpCount            = extension->resolvedUserOpSymbolOverload;
                extension->resolvedUserOpSymbolOverload = nullptr;
                SWAG_ASSERT(execParams.specReturnOpCount);

                context->expansionNodes.push_back({node, JobContext::ExpansionType::Node});
                ByteCodeGenJob::askForByteCode(context->job, execParams.specReturnOpCount->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
                context->expansionNodes.pop_back();
                if (context->result != ContextResult::Done)
                    return true;

                // opSlice
                AstNode tmpNode;
                memset(&tmpNode, 0, sizeof(AstNode));
                tmpNode.typeInfo = g_TypeMgr->typeInfoUInt;
                params.push_back(&tmpNode);
                params.push_back(&tmpNode);
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opSlice, nullptr, nullptr, node, params, false));
                if (context->result != ContextResult::Done)
                    return true;

                execParams.specReturnOpSlice            = extension->resolvedUserOpSymbolOverload;
                extension->resolvedUserOpSymbolOverload = nullptr;
                SWAG_ASSERT(execParams.specReturnOpSlice);

                context->expansionNodes.push_back({node, JobContext::ExpansionType::Node});
                ByteCodeGenJob::askForByteCode(context->job, execParams.specReturnOpSlice->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
                context->expansionNodes.pop_back();
                if (context->result != ContextResult::Done)
                    return true;

                // Is the type of the slice supported ?
                bool ok           = false;
                auto concreteType = TypeManager::concreteType(execParams.specReturnOpSlice->typeInfo);
                if (concreteType->isNative(NativeTypeKind::String))
                    ok = true;
                else if (concreteType->kind == TypeInfoKind::Slice)
                {
                    auto typeSlice        = CastTypeInfo<TypeInfoSlice>(concreteType, TypeInfoKind::Slice);
                    auto typeSliceContent = TypeManager::concreteType(typeSlice->pointedType);
                    if (typeSliceContent->isNative(NativeTypeKind::String) ||
                        typeSliceContent->isNative(NativeTypeKind::Bool) ||
                        typeSliceContent->isNativeIntegerOrRune() ||
                        typeSliceContent->isNativeFloat())
                        ok = true;
                    if (typeSliceContent->kind == TypeInfoKind::Struct && (typeSliceContent->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR))
                        ok = true;
                    if (!ok)
                        return context->report({node, Utf8::format(g_E[Err0059], typeSliceContent->getDisplayName().c_str())});
                }
                else
                {
                    return context->report({node, Utf8::format(g_E[Err0058], concreteType->getDisplayName().c_str())});
                }

                // opDrop
                if (hasUserOp(g_LangSpec->name_opDrop, (TypeInfoStruct*) realType))
                {
                    params.clear();
                    params.push_back(node);
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opDrop, nullptr, nullptr, node, params, false));
                    if (context->result != ContextResult::Done)
                        return true;

                    execParams.specReturnOpDrop             = extension->resolvedUserOpSymbolOverload;
                    extension->resolvedUserOpSymbolOverload = nullptr;
                    SWAG_ASSERT(execParams.specReturnOpDrop);

                    context->expansionNodes.push_back({node, JobContext::ExpansionType::Node});
                    ByteCodeGenJob::askForByteCode(context->job, execParams.specReturnOpDrop->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
                    context->expansionNodes.pop_back();
                    if (context->result != ContextResult::Done)
                        return true;
                }

                break;
            }

            case TypeInfoKind::Array:
                break;

            default:
                return context->report({node, Utf8::format(g_E[Err0280], realType->getDisplayName().c_str())});
            }
        }
    }

    // Before executing the node, we need to be sure that our dependencies have generated their dll
    // In case there's a foreign call somewhere...
    if (node->extension->bc->hasFunctionCalls)
    {
        if (!module->waitForDependenciesDone(context->job))
        {
            context->job->waitingKind = JobWaitKind::WaitDepDoneExec;
            context->result           = ContextResult::Pending;
            return true;
        }
    }

    context->expansionNodes.push_back({node, JobContext::ExpansionType::Node});
    SWAG_CHECK(module->executeNode(sourceFile, node, context, &execParams));
    context->expansionNodes.pop_back();
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
    SWAG_CHECK(executeCompilerNode(context, expression, false));
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
    SWAG_VERIFY(typeInfo->isNative(NativeTypeKind::Bool), context->report({expression, Utf8::format(g_E[Err0233], expression->typeInfo->getDisplayName().c_str())}));

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
    SWAG_VERIFY(typeInfo->isNative(NativeTypeKind::String), context->report({expression, Utf8::format(g_E[Err0234], expression->typeInfo->getDisplayName().c_str())}));

    SWAG_CHECK(executeCompilerNode(context, expression, true));
    if (context->result != ContextResult::Done)
        return true;

    SWAG_VERIFY(expression->flags & AST_VALUE_COMPUTED, context->report({expression, g_E[Err0798]}));

    node->childs.clear();

    if (!expression->computedValue->text.empty())
    {
        CompilerAstKind kind = CompilerAstKind::TopLevelInstruction;
        if (node->ownerScope->kind == ScopeKind::Struct)
            kind = CompilerAstKind::StructVarDecl;
        else if (node->ownerScope->kind == ScopeKind::Enum)
            kind = CompilerAstKind::EnumValue;
        else if (node->ownerScope->isGlobalOrImpl())
            kind = CompilerAstKind::TopLevelInstruction;
        else
            kind = CompilerAstKind::EmbeddedInstruction;

        SyntaxJob syntaxJob;
        syntaxJob.constructEmbedded(expression->computedValue->text, node, expression, kind, true);

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

    // Message ?
    auto expr = node->childs[0];
    if (node->childs.size() > 1)
    {
        auto msg     = node->childs[1];
        auto typeMsg = TypeManager::concreteType(msg->typeInfo, CONCRETE_FUNC);
        SWAG_VERIFY(typeMsg->isNative(NativeTypeKind::String), context->report({msg, Utf8::format(g_E[Err0236], msg->typeInfo->getDisplayName().c_str())}));
        SWAG_CHECK(evaluateConstExpression(context, msg));
        if (context->result != ContextResult::Done)
            return true;
        SWAG_VERIFY(msg->flags & AST_VALUE_COMPUTED, context->report({msg, g_E[Err0237]}));
    }

    // Expression to check
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, expr, CASTFLAG_AUTO_BOOL));
    SWAG_CHECK(executeCompilerNode(context, expr, true));
    if (context->result != ContextResult::Done)
        return true;

    node->flags |= AST_NO_BYTECODE;

    if (!expr->computedValue->reg.b)
    {
        if (node->childs.size() > 1)
        {
            auto msg = node->childs[1];
            context->report({node, msg->computedValue->text});
        }
        else
            context->report({node, g_E[Err0238]});
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
        return context->report({node, g_E[Err0239]});

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
    SWAG_VERIFY(expr->typeInfo->kind == TypeInfoKind::Code, context->report({expr, Utf8::format(g_E[Err0240], expr->typeInfo->getDisplayName().c_str())}));

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
    cloneContent->addAlternativeScope(typeCode->content->parent->ownerScope);
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
    return context->report({context->node, g_E[Err0241]});
}

bool SemanticJob::resolveCompilerPrint(SemanticContext* context)
{
    auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

    auto expr = context->node->childs[0];
    SWAG_CHECK(executeCompilerNode(context, expr, true));
    if (context->result != ContextResult::Done)
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
        ScopedLock lk(it.second->mutex);
        ScopedLock lk1(it.first->mutex);
        it.first->flags |= AST_NO_SEMANTIC | AST_NO_BYTECODE;
        it.first->semFlags |= AST_SEM_DISABLED;
        SymTable::disabledIfBlockOverloadNoLock(it.first, it.second);
    }

    // Decrease interfaces count to resolve
    for (auto typeStruct : block->interfacesCount)
    {
        sourceFile->module->decImplForToSolve(typeStruct);
        decreaseInterfaceRegCount(typeStruct);
        decreaseInterfaceCount(typeStruct);
    }

    // Decrease methods count to resolve
    set<TypeInfoStruct*> allStructs;
    for (auto& typeStructPair : block->methodsCount)
    {
        // Remove the corresponding method
        auto typeStruct = typeStructPair.typeInfo;
        allStructs.insert(typeStruct);

        typeStruct->methods[typeStructPair.methodIdx] = nullptr;

        // Then decrease wanted method number count
        decreaseMethodCount(typeStructPair.funcNode, typeStruct);
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
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    SWAG_CHECK(executeCompilerNode(context, node->boolExpression, true));
    if (context->result != ContextResult::Done)
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

    SWAG_VERIFY(back->flags & AST_VALUE_COMPUTED, context->report({back, g_E[Err0242]}));
    SWAG_VERIFY(back->typeInfo == g_TypeMgr->typeInfoString, context->report({back, Utf8::format(g_E[Err0243], back->typeInfo->getDisplayName().c_str())}));
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
                    return context->report({back, Utf8::format(g_E[Err0244], filename.c_str())});
            }
        }

        struct stat stat_buf;
        int         rc = stat(fullFileName, &stat_buf);
        SWAG_VERIFY(rc == 0, context->report({back, Utf8::format(g_E[Err0223], back->computedValue->text.c_str())}));
        SWAG_CHECK(context->checkSizeOverflow("'#load'", stat_buf.st_size, SWAG_LIMIT_COMPILER_LOAD));

        auto     newJob         = g_Allocator.alloc<LoadFileJob>();
        auto     storageSegment = getConstantSegFromContext(node);
        uint8_t* addrDst;
        node->computedValue->storageOffset  = storageSegment->reserve(stat_buf.st_size, &addrDst);
        node->computedValue->storageSegment = storageSegment;
        newJob->destBuffer                  = addrDst;
        newJob->sizeBuffer                  = stat_buf.st_size;

        newJob->module       = module;
        newJob->sourcePath   = fullFileName;
        newJob->dependentJob = job->dependentJob;
        newJob->addDependentJob(job);
        job->jobsToAdd.push_back(newJob);
        job->setPending(nullptr, JobWaitKind::LoadFile, node, nullptr);

        // Creates return type
        auto ptrArray         = allocType<TypeInfoArray>();
        ptrArray->count       = stat_buf.st_size;
        ptrArray->pointedType = g_TypeMgr->typeInfoU8;
        ptrArray->finalType   = g_TypeMgr->typeInfoU8;
        ptrArray->sizeOf      = ptrArray->count;
        ptrArray->totalCount  = stat_buf.st_size;
        ptrArray->computeName();
        node->typeInfo = ptrArray;

        return true;
    }

    return true;
}

bool SemanticJob::resolveIntrinsicDefined(SemanticContext* context)
{
    auto node = context->node;
    node->setFlagsValueIsComputed();
    node->computedValue->reg.b = node->childs.back()->resolvedSymbolOverload != nullptr;
    node->typeInfo             = g_TypeMgr->typeInfoBool;
    return true;
}

bool SemanticJob::resolveCompilerScopeFct(SemanticContext* context)
{
    auto node          = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    auto identifierRef = node->identifierRef;

    SWAG_VERIFY(!identifierRef->startScope, context->report({node, g_E[Err0246]}));
    SWAG_VERIFY(node->ownerFct, context->report({node, g_E[Err0247]}));

    node->semFlags |= AST_SEM_FORCE_SCOPE;
    node->typeInfo = g_TypeMgr->typeInfoVoid;

    identifierRef->previousResolvedNode = node;
    if (node->ownerInline)
        identifierRef->startScope = node->ownerInline->parametersScope;
    else
        identifierRef->startScope = node->ownerFct->scope;
    node->flags |= AST_NO_BYTECODE;

    return true;
}

Utf8 SemanticJob::getCompilerFunctionString(AstNode* node, TokenId id)
{
    switch (id)
    {
    case TokenId::CompilerFunction:
    case TokenId::CompilerCallerFunction:
        return node->ownerFct->getNameForUserCompiler();
    case TokenId::CompilerBuildCfg:
        return g_CommandLine->buildCfg;
    default:
        SWAG_ASSERT(false);
        break;
    }

    return "";
}

bool SemanticJob::resolveCompilerSpecialFunction(SemanticContext* context)
{
    auto node = context->node;
    switch (node->token.id)
    {
    case TokenId::CompilerSelf:
        SWAG_VERIFY(node->ownerFct, context->report({node, g_E[Err0348]}));
        while (node->ownerFct->flags & AST_SPECIAL_COMPILER_FUNC && node->ownerFct->parent->ownerFct)
            node = node->ownerFct->parent;
        SWAG_VERIFY(node, context->report({node, g_E[Err0348]}));

        if (node->ownerScope->kind == ScopeKind::Struct || node->ownerScope->kind == ScopeKind::Enum)
            node = node->ownerScope->owner;
        else
        {
            SWAG_VERIFY(node->ownerFct, context->report({node, g_E[Err0348]}));
            node = node->ownerFct;
        }

        context->node->resolvedSymbolOverload = node->resolvedSymbolOverload;
        context->node->resolvedSymbolName     = node->resolvedSymbolName;
        context->node->typeInfo               = node->typeInfo;
        return true;

    case TokenId::CompilerFunction:
        SWAG_VERIFY(node->ownerFct, context->report({node, g_E[Err0256]}));
        while (node->ownerFct->flags & AST_SPECIAL_COMPILER_FUNC && node->ownerFct->parent->ownerFct)
            node = node->ownerFct->parent;
        SWAG_VERIFY(node->ownerFct, context->report({node, g_E[Err0256]}));
        context->node->setFlagsValueIsComputed();
        context->node->computedValue->text = SemanticJob::getCompilerFunctionString(node, context->node->token.id);
        context->node->typeInfo            = g_TypeMgr->typeInfoString;
        return true;

    case TokenId::CompilerBackend:
        node->setFlagsValueIsComputed();
        switch (g_CommandLine->backendGenType)
        {
        case BackendGenType::X64:
            node->computedValue->reg.u64 = (uint64_t) SwagBackendGenType::X64;
            break;
        case BackendGenType::LLVM:
            node->computedValue->reg.u64 = (uint64_t) SwagBackendGenType::LLVM;
            break;
        }
        node->typeInfo = g_Workspace->swagScope.regTypeInfoBackend;
        SWAG_ASSERT(node->typeInfo);
        return true;

    case TokenId::CompilerArch:
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = (uint64_t) g_CommandLine->target.arch;
        node->typeInfo               = g_Workspace->swagScope.regTypeInfoTargetArch;
        SWAG_ASSERT(node->typeInfo);
        return true;

    case TokenId::CompilerOs:
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = (uint64_t) g_CommandLine->target.os;
        node->typeInfo               = g_Workspace->swagScope.regTypeInfoTargetOs;
        SWAG_ASSERT(node->typeInfo);
        return true;

    case TokenId::CompilerSwagOs:
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = (uint64_t) OS::getNativeTarget().os;
        node->typeInfo               = g_Workspace->swagScope.regTypeInfoTargetOs;
        SWAG_ASSERT(node->typeInfo);
        return true;

    case TokenId::CompilerBuildCfg:
        node->setFlagsValueIsComputed();
        node->computedValue->text = SemanticJob::getCompilerFunctionString(node, node->token.id);
        node->typeInfo            = g_TypeMgr->typeInfoString;
        return true;

    case TokenId::CompilerLocation:
    {
        node->typeInfo = g_Workspace->swagScope.regTypeInfoSourceLoc;
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
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, g_E[Err0254]}));
        node->typeInfo = g_Workspace->swagScope.regTypeInfoSourceLoc;
        return true;

    case TokenId::CompilerCallerFunction:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, g_E[Err0255]}));
        node->typeInfo = g_TypeMgr->typeInfoString;
        return true;

    default:
        return context->internalError("resolveCompilerFunction, unknown token");
    }
}