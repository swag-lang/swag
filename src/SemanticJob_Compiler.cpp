#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "Module.h"
#include "ByteCodeGenJob.h"
#include "Workspace.h"
#include "FileJob.h"
#include "ByteCode.h"
#include "Backend.h"
#include "Report.h"
#include "LanguageSpec.h"
#include "Parser.h"

Diagnostic* SemanticJob::computeNonConstExprNote(AstNode* node)
{
    VectorNative<AstNode*> childs;
    Ast::visit(node, [&](AstNode* n)
               { childs.push_back(n); });

    for (int i = (int) childs.size() - 1; i >= 0; i--)
    {
        auto c = childs[i];
        if (c->flags & AST_CONST_EXPR)
            continue;

        if (c->resolvedSymbolName)
        {
            if (c->resolvedSymbolName->kind == SymbolKind::Function)
            {
                if (c->resolvedSymbolOverload)
                {
                    if (!(c->resolvedSymbolOverload->node->attributeFlags & ATTRIBUTE_CONSTEXPR))
                    {
                        auto result  = Diagnostic::note(c, c->token, Fmt(Nte(Nte0042), c->resolvedSymbolName->name.c_str()));
                        result->hint = Hnt(Hnt0046);
                        return result;
                    }

                    return Diagnostic::note(c, c->token, Nte(Nte0071));
                }
            }

            if (c->resolvedSymbolName->kind == SymbolKind::Variable)
            {
                return Diagnostic::note(c, c->token, Fmt(Nte(Nte0041), c->resolvedSymbolName->name.c_str()));
            }

            return Diagnostic::note(c, Nte(Nte0070));
        }
    }

    return nullptr;
}

bool SemanticJob::executeCompilerNode(SemanticContext* context, AstNode* node, bool onlyConstExpr)
{
    // No need to run, this is already baked
    if (node->flags & AST_VALUE_COMPUTED)
        return true;

    if (onlyConstExpr)
        SWAG_CHECK(checkIsConstExpr(context, node));

    // In case of error, it's not always relevant to show the context
    bool showContext = true;
    if (node->token.text.length() > 1 && node->token.text[0] == '_' && node->token.text[1] == '_')
        showContext = false;
    if (node->token.text[0] == '@')
        showContext = false;

    if (showContext)
    {
        PushErrCxtStep ec(context, node, ErrCxtStepKind::CompileTime, nullptr);
        return executeCompilerNode(context, node);
    }
    else
    {
        return executeCompilerNode(context, node);
    }
}

bool SemanticJob::executeCompilerNode(SemanticContext* context, AstNode* node)
{
    // No need to run, this is already baked
    if (node->flags & AST_VALUE_COMPUTED)
        return true;

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    // Request to generate the corresponding bytecode
    {
        ByteCodeGenJob::askForByteCode(context->job, node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED);
        if (context->result != ContextResult::Done)
            return true;
    }

    // :CheckConstExprFuncReturnType
    // Be sure we can deal with the type at compile time
    ExecuteNodeParams execParams;
    if (!(node->semFlags & AST_SEM_EXEC_RET_STACK))
    {
        auto realType = TypeManager::concreteType(node->typeInfo);
        if (realType && realType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            switch (realType->kind)
            {
            case TypeInfoKind::Struct:
            {
                // Type is forced constexpr, evaluate it "as is"
                if (realType->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
                    break;

                if (realType->isTuple())
                {
                    Diagnostic diag{node, Err(Err0321)};
                    return context->report(diag);
                }

                // It is possible to convert a complex struct to a constant static array of values if the struct
                // implements 'opCount' and 'opSlice'
                context->job->waitAllStructMethods(realType);
                if (context->result != ContextResult::Done)
                    return true;

                if (node->hasSpecialFuncCall())
                {
                    Diagnostic diag{node, Fmt(Err(Err0281), realType->getDisplayNameC())};
                    diag.hint = Fmt(Hnt(Hnt0047), node->extMisc()->resolvedUserOpSymbolOverload->symbol->name.c_str());
                    return context->report(diag);
                }

                SymbolName* symCount   = nullptr;
                SymbolName* symSlice   = nullptr;
                auto        typeStruct = CastTypeInfo<TypeInfoStruct>(realType, TypeInfoKind::Struct);
                SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opCount, typeStruct, &symCount));
                SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opSlice, typeStruct, &symSlice));

                if (!symCount || !symSlice)
                {
                    // Force evaluation by a #run
                    if (node->flags & AST_RUN_BLOCK)
                    {
                        node->semFlags |= AST_SEM_FORCE_CONST_EXPR;
                        break;
                    }

                    Diagnostic diag{node, Fmt(Err(Err0281), realType->getDisplayNameC())};
                    diag.hint = Hlp(Hlp0036);
                    return context->report(diag);
                }

                SWAG_ASSERT(!context->node->hasExtMisc() || !context->node->extMisc()->resolvedUserOpSymbolOverload);

                // opCount
                VectorNative<AstNode*> params;
                params.push_back(node);
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opCount, nullptr, nullptr, node, params));
                if (context->result != ContextResult::Done)
                    return true;

                SWAG_ASSERT(context->node->hasExtMisc());

                auto extension                          = context->node->extMisc();
                execParams.specReturnOpCount            = extension->resolvedUserOpSymbolOverload;
                extension->resolvedUserOpSymbolOverload = nullptr;
                SWAG_ASSERT(execParams.specReturnOpCount);

                {
                    ByteCodeGenJob::askForByteCode(context->job, execParams.specReturnOpCount->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
                    if (context->result != ContextResult::Done)
                        return true;
                }

                // opSlice
                AstNode tmpNode;
                memset(&tmpNode, 0, sizeof(AstNode));
                tmpNode.typeInfo = g_TypeMgr->typeInfoU64;
                params.push_back(&tmpNode);
                params.push_back(&tmpNode);
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opSlice, nullptr, nullptr, node, params));
                if (context->result != ContextResult::Done)
                    return true;

                execParams.specReturnOpSlice            = extension->resolvedUserOpSymbolOverload;
                extension->resolvedUserOpSymbolOverload = nullptr;
                SWAG_ASSERT(execParams.specReturnOpSlice);

                {
                    ByteCodeGenJob::askForByteCode(context->job, execParams.specReturnOpSlice->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
                    if (context->result != ContextResult::Done)
                        return true;
                }

                // Is the type of the slice supported ?
                bool ok           = false;
                auto concreteType = TypeManager::concreteType(execParams.specReturnOpSlice->typeInfo);
                if (concreteType->isString())
                    ok = true;
                else if (concreteType->isSlice())
                {
                    auto typeSlice        = CastTypeInfo<TypeInfoSlice>(concreteType, TypeInfoKind::Slice);
                    auto typeSliceContent = TypeManager::concreteType(typeSlice->pointedType);
                    if (typeSliceContent->isString() ||
                        typeSliceContent->isBool() ||
                        typeSliceContent->isNativeIntegerOrRune() ||
                        typeSliceContent->isNativeFloat())
                        ok = true;
                    if (typeSliceContent->isStruct() && (typeSliceContent->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR))
                        ok = true;
                    if (!ok)
                    {
                        Diagnostic diag{node, Fmt(Err(Err0059), typeSliceContent->getDisplayNameC())};
                        return context->report(diag);
                    }
                }
                else
                {
                    Diagnostic diag{node, Fmt(Err(Err0058), concreteType->getDisplayNameC())};
                    return context->report(diag);
                }

                // opDrop
                SymbolName* symDrop = nullptr;
                SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opDrop, (TypeInfoStruct*) realType, &symDrop));
                if (symDrop)
                {
                    params.clear();
                    params.push_back(node);
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opDrop, nullptr, nullptr, node, params));
                    if (context->result != ContextResult::Done)
                        return true;

                    execParams.specReturnOpDrop             = extension->resolvedUserOpSymbolOverload;
                    extension->resolvedUserOpSymbolOverload = nullptr;
                    SWAG_ASSERT(execParams.specReturnOpDrop);

                    {
                        ByteCodeGenJob::askForByteCode(context->job, execParams.specReturnOpDrop->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
                        if (context->result != ContextResult::Done)
                            return true;
                    }
                }

                break;
            }

            case TypeInfoKind::Array:
                break;

            default:
                return context->report({node, Fmt(Err(Err0280), realType->getDisplayNameC())});
            }
        }
    }

    // Before executing the node, we need to be sure that our dependencies have generated their dll
    // In case there's a foreign call somewhere...
    if (node->extByteCode()->bc->hasForeignFunctionCalls)
    {
        if (!module->waitForDependenciesDone(context->job))
        {
            context->job->waitingKind = JobWaitKind::WaitDepDoneExec;
            context->result           = ContextResult::Pending;
            return true;
        }
    }
    else if (!node->extByteCode()->bc->hasForeignFunctionCallsModules.empty())
    {
        if (!module->waitForDependenciesDone(context->job, node->extByteCode()->bc->hasForeignFunctionCallsModules))
        {
            context->job->waitingKind = JobWaitKind::WaitDepDoneExec;
            context->result           = ContextResult::Pending;
            return true;
        }
    }

    SWAG_CHECK(module->executeNode(sourceFile, node, context, &execParams));
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
    auto node = CastAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerRun, AstNodeKind::CompilerRunExpression);
    if (node->flags & AST_IS_GENERIC)
        return true;

    auto expression = context->node->childs.back();

    if (!expression->typeInfo)
        expression->typeInfo = g_TypeMgr->typeInfoVoid;

    SWAG_CHECK(executeCompilerNode(context, expression, false));
    if (context->result != ContextResult::Done)
        return true;

    context->node->inheritComputedValue(expression);
    if (context->node->flags & AST_VALUE_COMPUTED)
        context->node->flags &= ~AST_NO_BYTECODE;

    context->node->typeInfo = expression->typeInfo;
    return true;
}

bool SemanticJob::resolveCompilerValidIfExpression(SemanticContext* context)
{
    auto node = CastAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerValidIf, AstNodeKind::CompilerValidIfx);
    if (node->flags & AST_IS_GENERIC)
        return true;

    auto expression = context->node->childs.back();
    auto typeInfo   = TypeManager::concreteType(expression->typeInfo);
    if (!typeInfo->isBool())
    {
        Diagnostic diag{expression, Fmt(Err(Err0233), node->token.ctext(), expression->typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    return true;
}

bool SemanticJob::resolveCompilerAstExpression(SemanticContext* context)
{
    auto node = CastAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerAst);
    if (node->flags & AST_IS_GENERIC)
        return true;
    // Do it once (in case of inline)
    if (node->doneFlags & AST_DONE_AST_BLOCK)
        return true;

    auto job        = context->job;
    auto expression = context->node->childs.back();
    auto typeInfo   = TypeManager::concreteType(expression->typeInfo);
    SWAG_VERIFY(typeInfo->isString(), context->report({expression, Fmt(Err(Err0234), expression->typeInfo->getDisplayNameC())}));

    SWAG_CHECK(executeCompilerNode(context, expression, false));
    if (context->result != ContextResult::Done)
        return true;
    node->doneFlags |= AST_DONE_AST_BLOCK;

    SWAG_CHECK(checkIsConstExpr(context, expression->flags & AST_VALUE_COMPUTED, expression));

    node->allocateExtension(ExtensionKind::Owner);
    node->extOwner()->nodesToFree.append(node->childs);
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

        Parser parser;
        parser.setup(context, context->sourceFile->module, context->sourceFile);
        SWAG_CHECK(parser.constructEmbeddedAst(expression->computedValue->text, node, expression, kind, true));

        job->nodes.pop_back();
        for (int i = (int) node->childs.size() - 1; i >= 0; i--)
            job->nodes.push_back(node->childs[i]);
        job->nodes.push_back(node);
    }

    return true;
}

bool SemanticJob::resolveCompilerError(SemanticContext* context)
{
    auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

    auto msg     = node->childs.front();
    auto typeMsg = TypeManager::concreteType(msg->typeInfo, CONCRETE_FUNC);
    SWAG_VERIFY(typeMsg->isString(), context->report({msg, Fmt(Err(Err0236), node->token.ctext(), msg->typeInfo->getDisplayNameC()), Diagnostic::isType(msg->typeInfo).c_str()}));
    SWAG_CHECK(evaluateConstExpression(context, msg));
    if (context->result != ContextResult::Done)
        return true;
    SWAG_CHECK(checkIsConstExpr(context, msg->flags & AST_VALUE_COMPUTED, msg, Fmt(Err(Err0237), node->token.ctext())));
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

    Diagnostic diag{node, node->token, msg->computedValue->text, DiagnosticLevel::Error};
    return context->report(diag);
}

bool SemanticJob::resolveCompilerWarning(SemanticContext* context)
{
    auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

    auto msg     = node->childs.front();
    auto typeMsg = TypeManager::concreteType(msg->typeInfo, CONCRETE_FUNC);
    SWAG_VERIFY(typeMsg->isString(), context->report({msg, Fmt(Err(Err0236), node->token.ctext(), msg->typeInfo->getDisplayNameC()), Diagnostic::isType(msg->typeInfo).c_str()}));
    SWAG_CHECK(evaluateConstExpression(context, msg));
    if (context->result != ContextResult::Done)
        return true;
    SWAG_CHECK(checkIsConstExpr(context, msg->flags & AST_VALUE_COMPUTED, msg, Fmt(Err(Err0237), node->token.ctext())));
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

    Diagnostic diag{node, node->token, msg->computedValue->text, DiagnosticLevel::Warning};
    return context->report(diag);
}

bool SemanticJob::resolveCompilerAssert(SemanticContext* context)
{
    auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

    // Expression to check
    auto expr = node->childs[0];
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, expr, CASTFLAG_AUTO_BOOL));
    SWAG_CHECK(executeCompilerNode(context, expr, true));
    if (context->result != ContextResult::Done)
        return true;

    node->flags |= AST_NO_BYTECODE;

    if (!expr->computedValue->reg.b)
        return context->report({node, node->token, Err(Err0238)});
    return true;
}

bool SemanticJob::resolveCompilerMacro(SemanticContext* context)
{
    auto node             = context->node;
    auto scope            = node->childs.back()->ownerScope;
    scope->startStackSize = node->ownerScope->startStackSize;

    node->allocateExtension(ExtensionKind::ByteCode);
    node->extByteCode()->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

    // Be sure #macro is used inside a macro
    if (!node->ownerInline || (node->ownerInline->attributeFlags & ATTRIBUTE_MIXIN) || !(node->ownerInline->attributeFlags & ATTRIBUTE_MACRO))
        return context->report({node, Err(Err0239)});

    return true;
}

bool SemanticJob::resolveCompilerInline(SemanticContext* context)
{
    auto node             = CastAst<AstCompilerInline>(context->node, AstNodeKind::CompilerInline);
    auto scope            = node->childs.back()->ownerScope;
    scope->startStackSize = node->ownerScope->startStackSize;

    node->allocateExtension(ExtensionKind::ByteCode);
    node->extByteCode()->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

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
    SWAG_VERIFY(expr->typeInfo->isCode(), context->report({expr, Fmt(Err(Err0240), expr->typeInfo->getDisplayNameC())}));

    node->allocateExtension(ExtensionKind::ByteCode);
    node->extByteCode()->byteCodeBeforeFct = ByteCodeGenJob::emitDebugNop;
    node->byteCodeFct                      = ByteCodeGenJob::emitDebugNop;
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
    cloneContent->allocateExtension(ExtensionKind::Misc);
    cloneContent->extMisc()->alternativeNode = typeCode->content->parent;
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
        for (auto& c : node->childs)
            c->flags |= AST_NO_SEMANTIC;
        node->semFlags |= AST_SEM_ON_CLONE;
    }

    return true;
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
    if (typeInfo->isNative())
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
            g_Log.print(Fmt("<%s>", typeInfo->getDisplayNameC()));
            break;
        }
    }
    else
    {
        g_Log.print(Fmt("<%s>", typeInfo->getDisplayNameC()));
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
    Set<TypeInfoStruct*> allStructs;
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

    // Eliminate includes
    for (auto imp : block->includes)
        sourceFile->module->removeInclude(imp);

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

bool SemanticJob::resolveCompilerInclude(SemanticContext* context)
{
    auto job    = context->job;
    auto node   = context->node;
    auto module = context->sourceFile->module;
    auto back   = node->childs[0];

    SWAG_CHECK(checkIsConstExpr(context, back->flags & AST_VALUE_COMPUTED, back, Err(Err0242)));
    SWAG_VERIFY(back->typeInfo == g_TypeMgr->typeInfoString, context->report({back, Fmt(Err(Err0243), back->typeInfo->getDisplayNameC())}));
    node->setFlagsValueIsComputed();

    if (!(node->doneFlags & AST_DONE_LOAD))
    {
        node->doneFlags |= AST_DONE_LOAD;

        auto filename = back->computedValue->text;
        Path fullFileName;

        // Search first in the same folder as the source file
        fullFileName = node->sourceFile->path.parent_path();
        fullFileName.append(filename.c_str());
        error_code err;
        if (!filesystem::exists(fullFileName, err))
        {
            // Search relative to the module path
            fullFileName = node->sourceFile->module->path;
            fullFileName.append(filename.c_str());
            if (!filesystem::exists(fullFileName, err))
            {
                // Search the file itself, without any special path
                fullFileName = filename;
                if (!filesystem::exists(fullFileName, err))
                    return context->report({back, Fmt(Err(Err0244), filename.c_str())});
            }
        }

        struct stat stat_buf;
        int         rc = stat(fullFileName.string().c_str(), &stat_buf);
        SWAG_VERIFY(rc == 0, context->report({back, Fmt(Err(Err0223), back->computedValue->text.c_str())}));
        SWAG_CHECK(context->checkSizeOverflow("'#load'", stat_buf.st_size, SWAG_LIMIT_COMPILER_LOAD));

        auto     newJob         = Allocator::alloc<LoadFileJob>();
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
        auto ptrArray         = makeType<TypeInfoArray>();
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

bool SemanticJob::resolveIntrinsicLocation(SemanticContext* context)
{
    auto node      = context->node;
    auto locNode   = node->childs.front();
    node->typeInfo = g_TypeMgr->makeConst(g_Workspace->swagScope.regTypeInfoSourceLoc);

    if (locNode->isValidIfParam(locNode->resolvedSymbolOverload))
    {
        node->flags &= ~AST_NO_BYTECODE;
        locNode->flags |= AST_NO_BYTECODE;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicLocationSI;
        return true;
    }

    bool done = false;
    while (true)
    {
        // If identifier is an inline param call replacement, take it
        if (locNode->resolvedSymbolOverload && locNode->resolvedSymbolOverload->fromInlineParam)
        {
            locNode = locNode->resolvedSymbolOverload->fromInlineParam;
            done    = true;
            continue;
        }

        if (locNode->kind == AstNodeKind::FuncCallParam && locNode->childs.front()->kind == AstNodeKind::IdentifierRef)
        {
            auto id = CastAst<AstIdentifier>(locNode->childs.back()->childs.back(), AstNodeKind::Identifier);
            if (id->identifierExtension && id->identifierExtension->fromAlternateVar)
            {
                locNode = id->identifierExtension->fromAlternateVar;
                done    = true;
                continue;
            }
        }

        // :ForLocationInValidIf
        if (locNode->kind == AstNodeKind::IdentifierRef)
        {
            auto id = CastAst<AstIdentifier>(locNode->childs.back(), AstNodeKind::Identifier);
            if (id->identifierExtension && id->identifierExtension->fromAlternateVar)
            {
                locNode = id->identifierExtension->fromAlternateVar;
                done    = true;
                continue;
            }
        }

        break;
    }

    // If identifier is an inline param call replacement, take it
    if (locNode->resolvedSymbolOverload && !done)
        locNode = locNode->resolvedSymbolOverload->node;

    node->setFlagsValueIsComputed();
    ByteCodeGenJob::computeSourceLocation(context, locNode, &node->computedValue->storageOffset, &node->computedValue->storageSegment);
    SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
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

Utf8 SemanticJob::getCompilerFunctionString(AstNode* node, TokenId id)
{
    switch (id)
    {
    case TokenId::CompilerCallerFunction:
        return node->ownerFct->getNameForUserCompiler();
    case TokenId::CompilerBuildCfg:
        return g_CommandLine.buildCfg;
    default:
        SWAG_ASSERT(false);
        break;
    }

    return "";
}

bool SemanticJob::resolveCompilerSpecialValue(SemanticContext* context)
{
    auto node = context->node;
    switch (node->token.id)
    {
    case TokenId::CompilerBackend:
        node->setFlagsValueIsComputed();
        switch (g_CommandLine.backendGenType)
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
        node->computedValue->reg.u64 = (uint64_t) g_CommandLine.target.arch;
        node->typeInfo               = g_Workspace->swagScope.regTypeInfoTargetArch;
        SWAG_ASSERT(node->typeInfo);
        return true;

    case TokenId::CompilerOs:
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = (uint64_t) g_CommandLine.target.os;
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
        node->typeInfo = g_TypeMgr->makeConst(g_Workspace->swagScope.regTypeInfoSourceLoc);
        node->setFlagsValueIsComputed();
        ByteCodeGenJob::computeSourceLocation(context, node, &node->computedValue->storageOffset, &node->computedValue->storageSegment);
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;

    case TokenId::CompilerCallerLocation:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, Err(Err0254)}));
        node->typeInfo = g_Workspace->swagScope.regTypeInfoSourceLoc;
        return true;

    case TokenId::CompilerCallerFunction:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, Err(Err0255)}));
        node->typeInfo = g_TypeMgr->typeInfoString;
        return true;

    default:
        return Report::internalError(context->node, "resolveCompilerFunction, unknown token");
    }
}