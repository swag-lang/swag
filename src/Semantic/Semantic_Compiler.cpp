#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/Context.h"
#include "Error/SemanticError.h"
#include "Format/FormatAst.h"
#include "Jobs/FileJob.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Parser/Parser.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

Diagnostic* Semantic::computeNonConstExprNote(AstNode* node)
{
    VectorNative<AstNode*> children;
    Ast::visit(node, [&](AstNode* n) { children.push_back(n); });

    for (auto i = children.size() - 1; i != UINT32_MAX; i--)
    {
        const auto c = children[i];
        if (c->hasAstFlag(AST_CONST_EXPR))
            continue;

        const auto symbolName = c->resolvedSymbolName();
        if (symbolName)
        {
            if (symbolName->is(SymbolKind::Function))
            {
                if (c->resolvedSymbolOverload())
                {
                    if (!c->resolvedSymbolOverload()->node->hasAttribute(ATTRIBUTE_CONSTEXPR))
                    {
                        const auto result = Diagnostic::note(c, c->token, formNte(Nte0120, symbolName->name.cstr()));
                        result->hint      = toNte(Nte0039);
                        return result;
                    }

                    return Diagnostic::note(c, c->token, toNte(Nte0168));
                }
            }

            if (symbolName->is(SymbolKind::Variable))
                return Diagnostic::note(c, c->token, formNte(Nte0005, symbolName->name.cstr()));

            return Diagnostic::note(c, toNte(Nte0081));
        }
    }

    return nullptr;
}

bool Semantic::executeCompilerNode(SemanticContext* context, AstNode* node, bool onlyConstExpr)
{
    // No need to run, this is already baked
    if (node->hasFlagComputedValue())
        return true;

    if (onlyConstExpr)
        SWAG_CHECK(checkIsConstExpr(context, node));

    // In case of error, it's not always relevant to show the context
    bool showContext = true;
    if (Parser::isGeneratedName(node->token.text))
        showContext = false;
    if (node->token.text[0] == '@')
        showContext = false;

    // Push a copy of the default context, in case the user code changes it (or push a new one)
    // :PushDefaultCxt
    PushSwagContext cxt;

    if (showContext)
    {
        PushErrCxtStep ec(context, node, ErrCxtStepKind::DuringCompileTime, nullptr);
        return doExecuteCompilerNode(context, node, onlyConstExpr);
    }

    return doExecuteCompilerNode(context, node, onlyConstExpr);
}

bool Semantic::doExecuteCompilerNode(SemanticContext* context, AstNode* node, bool onlyConstExpr)
{
    // No need to run, this is already baked
    if (node->hasFlagComputedValue())
        return true;

    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;

    // Request to generate the corresponding bytecode
    {
        ByteCodeGen::askForByteCode(context, node, ASK_BC_WAIT_DONE | ASK_BC_WAIT_RESOLVED, true, nullptr);
        YIELD();
    }

    // :CheckConstExprFuncReturnType
    // Be sure we can deal with the type at compile time
    ExecuteNodeParams execParams;
    execParams.forConstExpr = onlyConstExpr;

    if (!node->hasSemFlag(SEMFLAG_EXEC_RET_STACK))
    {
        const auto realType = TypeManager::concreteType(node->typeInfo);
        if (realType && realType->isStruct() && !realType->declNode->hasAttribute(ATTRIBUTE_CONSTEXPR))
        {
            // It is possible to convert a complex struct to a constant static array of values if the struct
            // implements 'opCount' and 'opSlice'
            waitAllStructMethods(context->baseJob, realType);
            YIELD();

            if (node->hasSpecialFuncCall())
            {
                Diagnostic err{node, formErr(Err0044, realType->getDisplayNameC())};
                const auto userOp = node->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
                err.hint          = formNte(Nte0155, userOp->symbol->name.cstr());
                return context->report(err);
            }

            SymbolName* symCount   = nullptr;
            SymbolName* symSlice   = nullptr;
            const auto  typeStruct = castTypeInfo<TypeInfoStruct>(realType, TypeInfoKind::Struct);
            SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opCount, typeStruct, &symCount));
            SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opSlice, typeStruct, &symSlice));

            if (!symCount || !symSlice)
            {
                // Force evaluation by a #run
                if (node->hasAstFlag(AST_IN_RUN_BLOCK))
                {
                    node->addSemFlag(SEMFLAG_FORCE_CONST_EXPR);
                }
                else
                {
                    Diagnostic err{node, formErr(Err0044, realType->getDisplayNameC())};
                    err.hint = toNte(Nte0039);
                    return context->report(err);
                }
            }
            else
            {
                SWAG_ASSERT(!context->node->hasExtraPointer(ExtraPointerKind::UserOp));
                const auto saveTypeStruct = node->typeInfo;

                // opCount
                VectorNative<AstNode*> params;
                params.push_back(node);
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opCount, nullptr, nullptr, node, params));
                node->typeInfo = saveTypeStruct;
                YIELD();

                SWAG_ASSERT(context->node->hasExtMisc());

                execParams.specReturnOpCount = context->node->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
                context->node->addExtraPointer(ExtraPointerKind::UserOp, nullptr);
                SWAG_ASSERT(execParams.specReturnOpCount);

                ByteCodeGen::askForByteCode(context, execParams.specReturnOpCount->node, ASK_BC_WAIT_DONE | ASK_BC_WAIT_RESOLVED | ASK_BC_WAIT_SEMANTIC_RESOLVED, true, nullptr);
                YIELD();

                // opSlice
                AstNode tmpNode;
                Ast::constructNode(&tmpNode);
                tmpNode.typeInfo = g_TypeMgr->typeInfoU64;
                params.push_back(&tmpNode);
                params.push_back(&tmpNode);
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opSlice, nullptr, nullptr, node, params));
                node->typeInfo = saveTypeStruct;
                YIELD();

                execParams.specReturnOpSlice = context->node->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
                context->node->addExtraPointer(ExtraPointerKind::UserOp, nullptr);
                SWAG_ASSERT(execParams.specReturnOpSlice);

                ByteCodeGen::askForByteCode(context, execParams.specReturnOpSlice->node, ASK_BC_WAIT_DONE | ASK_BC_WAIT_RESOLVED | ASK_BC_WAIT_SEMANTIC_RESOLVED, true, nullptr);
                YIELD();

                // Is the type of the slice supported ?
                const auto concreteType = TypeManager::concreteType(execParams.specReturnOpSlice->typeInfo);
                if (concreteType->isSlice())
                {
                    bool       ok               = false;
                    const auto typeSlice        = castTypeInfo<TypeInfoSlice>(concreteType, TypeInfoKind::Slice);
                    const auto typeSliceContent = TypeManager::concreteType(typeSlice->pointedType);
                    if (typeSliceContent->isString() ||
                        typeSliceContent->isBool() ||
                        typeSliceContent->isNativeIntegerOrRune() ||
                        typeSliceContent->isNativeFloat())
                        ok = true;
                    if (typeSliceContent->isStruct() && typeSliceContent->declNode->hasAttribute(ATTRIBUTE_CONSTEXPR))
                        ok = true;
                    if (!ok)
                    {
                        const Diagnostic err{node, formErr(Err0030, typeSliceContent->getDisplayNameC())};
                        return context->report(err);
                    }
                }
                else if (!concreteType->isString())
                {
                    const Diagnostic err{node, formErr(Err0033, concreteType->getDisplayNameC())};
                    return context->report(err);
                }

                // opPostMove
                SymbolName* symPostMove = nullptr;
                SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opPostMove, castTypeInfo<TypeInfoStruct>(realType), &symPostMove));
                if (symPostMove)
                {
                    params.clear();
                    params.push_back(node);
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opPostMove, nullptr, nullptr, node, params));
                    node->typeInfo = saveTypeStruct;
                    YIELD();

                    execParams.specReturnOpPostMove = context->node->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
                    context->node->addExtraPointer(ExtraPointerKind::UserOp, nullptr);
                    SWAG_ASSERT(execParams.specReturnOpPostMove);
                    ByteCodeGen::askForByteCode(context, execParams.specReturnOpPostMove->node, ASK_BC_WAIT_DONE | ASK_BC_WAIT_RESOLVED | ASK_BC_WAIT_SEMANTIC_RESOLVED, true, nullptr);
                    YIELD();
                }

                // opDrop
                SymbolName* symDrop = nullptr;
                SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opDrop, castTypeInfo<TypeInfoStruct>(realType), &symDrop));
                if (symDrop)
                {
                    params.clear();
                    params.push_back(node);
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opDrop, nullptr, nullptr, node, params));
                    node->typeInfo = saveTypeStruct;
                    YIELD();

                    execParams.specReturnOpDrop = context->node->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
                    context->node->addExtraPointer(ExtraPointerKind::UserOp, nullptr);
                    SWAG_ASSERT(execParams.specReturnOpDrop);
                    ByteCodeGen::askForByteCode(context, execParams.specReturnOpDrop->node, ASK_BC_WAIT_DONE | ASK_BC_WAIT_RESOLVED | ASK_BC_WAIT_SEMANTIC_RESOLVED, true, nullptr);
                    YIELD();
                }
            }
        }
    }

    // Before executing the node, we need to be sure that our dependencies have generated their dll
    // In case there's a foreign call somewhere...
    if (node->extByteCode()->bc->hasForeignFunctionCalls)
    {
        if (!module->waitForDependenciesDone(context->baseJob))
        {
            context->baseJob->setPendingInfos(JobWaitKind::WaitDepDoneExec);
            context->result = ContextResult::Pending;
            return true;
        }
    }
    else if (!node->extByteCode()->bc->hasForeignFunctionCallsModules.empty())
    {
        if (!module->waitForDependenciesDone(context->baseJob, node->extByteCode()->bc->hasForeignFunctionCallsModules))
        {
            context->baseJob->setPendingInfos(JobWaitKind::WaitDepDoneExec);
            context->result = ContextResult::Pending;
            return true;
        }
    }

    SWAG_CHECK(collectAttributes(context, node, nullptr));
    if (node->hasAttribute(ATTRIBUTE_PRINT_BC_GEN))
    {
        constexpr ByteCodePrintOptions opt;
        node->extByteCode()->bc->print(opt);
    }

    SWAG_CHECK(module->executeNode(sourceFile, node, context, &execParams));
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveCompilerForeignLib(SemanticContext* context)
{
    const auto node   = context->node;
    const auto module = context->sourceFile->module;
    module->addForeignLib(node->firstChild()->token.text);
    return true;
}

bool Semantic::resolveCompilerRun(SemanticContext* context)
{
    const auto node = castAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerRun, AstNodeKind::CompilerRunExpression);
    if (node->hasAstFlag(AST_GENERIC))
        return true;

    const auto expression = context->node->lastChild();

    if (!expression->typeInfo)
        expression->typeInfo = g_TypeMgr->typeInfoVoid;

    SWAG_CHECK(executeCompilerNode(context, expression, false));
    YIELD();

    context->node->inheritComputedValue(expression);
    if (context->node->hasFlagComputedValue())
        context->node->removeAstFlag(AST_NO_BYTECODE);

    context->node->typeInfo = expression->typeInfo;
    return true;
}

bool Semantic::resolveCompilerAstExpression(SemanticContext* context)
{
    const auto node = castAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerAst);
    if (node->hasAstFlag(AST_GENERIC))
        return true;

    // Do it once (in case of inline)
    if (node->hasSpecFlag(AstCompilerSpecFunc::SPEC_FLAG_AST_BLOCK))
        return true;

    const auto job        = context->baseJob;
    auto       expression = context->node->lastChild();
    const auto typeInfo   = TypeManager::concreteType(expression->typeInfo);
    SWAG_VERIFY(typeInfo->isString(), context->report({expression, formErr(Err0604, expression->typeInfo->getDisplayNameC())}));

    SWAG_CHECK(executeCompilerNode(context, expression, false));
    YIELD();
    node->addSpecFlag(AstCompilerSpecFunc::SPEC_FLAG_AST_BLOCK);

    SWAG_CHECK(checkIsConstExpr(context, expression->hasFlagComputedValue(), expression));

    node->allocateExtension(ExtensionKind::Owner);
    node->extOwner()->nodesToFree.append(node->children);
    node->children.clear();

    if (!expression->computedValue()->text.empty())
    {
        CompilerAstKind kind;
        if (node->ownerScope->is(ScopeKind::Struct))
            kind = CompilerAstKind::StructVarDecl;
        else if (node->ownerScope->is(ScopeKind::Enum))
            kind = CompilerAstKind::EnumValue;
        else if (node->ownerScope->isGlobalOrImpl())
            kind = CompilerAstKind::TopLevelInstruction;
        else
            kind = CompilerAstKind::EmbeddedInstruction;

        Parser parser;
        parser.setup(context, context->sourceFile->module, context->sourceFile, PARSER_DEFAULT);
        SWAG_CHECK(parser.constructEmbeddedAst(expression->computedValue()->text, node, expression, kind, true));

        job->nodes.pop_back();
        for (uint32_t i = node->childCount() - 1; i != UINT32_MAX; i--)
            job->nodes.push_back(node->children[i]);
        job->nodes.push_back(node);
    }

    return true;
}

bool Semantic::resolveCompilerError(SemanticContext* context)
{
    const auto node = context->node;
    if (node->hasAstFlag(AST_GENERIC))
        return true;

    const auto msg = node->firstChild();
    SWAG_CHECK(evaluateConstExpression(context, msg));
    YIELD();
    SWAG_CHECK(checkIsConstExpr(context, msg->hasFlagComputedValue(), msg, formErr(Err0036, node->token.cstr())));
    node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);

    const Diagnostic err{node, node->token, formErr(Err0001, msg->computedValue()->text.cstr()), DiagnosticLevel::Error};
    return context->report(err);
}

bool Semantic::resolveCompilerWarning(SemanticContext* context)
{
    const auto node = context->node;
    if (node->hasAstFlag(AST_GENERIC))
        return true;

    const auto msg = node->firstChild();
    SWAG_CHECK(evaluateConstExpression(context, msg));
    YIELD();
    SWAG_CHECK(checkIsConstExpr(context, msg->hasFlagComputedValue(), msg, formErr(Err0036, node->token.cstr())));
    node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);

    const Diagnostic err{node, node->token, msg->computedValue()->text, DiagnosticLevel::Warning};
    return context->report(err);
}

bool Semantic::resolveCompilerAssert(SemanticContext* context)
{
    auto node = context->node;
    if (node->hasAstFlag(AST_GENERIC))
        return true;

    // Expression to check
    const auto expr = node->firstChild();
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, expr, CAST_FLAG_AUTO_BOOL | CAST_FLAG_JUST_CHECK));
    SWAG_CHECK(executeCompilerNode(context, expr, true));
    YIELD();

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, expr, CAST_FLAG_AUTO_BOOL));
    node->addAstFlag(AST_NO_BYTECODE);

    if (!expr->computedValue()->reg.b)
        return context->report({node, node->token, toErr(Err0029)});
    return true;
}

bool Semantic::resolveCompilerMacro(SemanticContext* context)
{
    auto       node       = context->node;
    const auto scope      = node->lastChild()->ownerScope;
    scope->startStackSize = node->ownerScope->startStackSize;

    node->setBcNotifyAfter(ByteCodeGen::emitLeaveScope);

    // Be sure #macro is used inside a macro
    if (!node->hasOwnerInline() || node->ownerInline()->hasAttribute(ATTRIBUTE_MIXIN) || !node->ownerInline()->hasAttribute(ATTRIBUTE_MACRO))
        return context->report({node, toErr(Err0308)});

    return true;
}

bool Semantic::resolveCompilerInject(SemanticContext* context)
{
    const auto node = castAst<AstCompilerMixin>(context->node, AstNodeKind::CompilerInject);

    if (node->hasSemFlag(SEMFLAG_COMPILER_INSERT))
    {
        node->typeInfo = node->lastChild()->typeInfo;
        return true;
    }

    node->addSemFlag(SEMFLAG_COMPILER_INSERT);

    auto expr = node->firstChild();
    SWAG_VERIFY(expr->typeInfo->isCode(), context->report({expr, formErr(Err0549, expr->typeInfo->getDisplayNameC())}));

    node->setBcNotifyBefore(ByteCodeGen::emitDebugNop);
    node->byteCodeFct = ByteCodeGen::emitDebugNop;
    expr->addAstFlag(AST_NO_BYTECODE);

    const auto typeCode = castTypeInfo<TypeInfoCode>(expr->typeInfo, TypeInfoKind::Code);
    SWAG_ASSERT(typeCode->content);

    CloneContext cloneContext;
    cloneContext.parent                 = node;
    cloneContext.parentScope            = node->ownerScope;
    cloneContext.ownerBreakable         = node->safeOwnerBreakable();
    cloneContext.replaceTokensBreakable = node->safeOwnerBreakable();
    cloneContext.ownerInline            = node->safeOwnerInline();
    cloneContext.replaceTokens          = node->replaceTokens;
    cloneContext.forceFlags             = AST_IN_MIXIN;
    cloneContext.removeFlags            = AST_R_VALUE;
    cloneContext.ownerFct               = node->ownerFct;
    cloneContext.cloneFlags             = CLONE_INJECT;
    const auto cloneContent             = typeCode->content->clone(cloneContext);

    if (node->hasAstFlag(AST_DISCARD))
        Ast::setDiscard(cloneContent);

    cloneContent->addExtraPointer(ExtraPointerKind::AlternativeNode, typeCode->content->parent);
    cloneContent->addAlternativeScope(typeCode->content->parent->ownerScope);
    cloneContent->removeAstFlag(AST_NO_SEMANTIC | AST_NO_BYTECODE);
    node->typeInfo = cloneContent->typeInfo;
    context->baseJob->nodes.push_back(cloneContent);
    context->result = ContextResult::NewChildren;
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::preResolveCompilerInstruction(SemanticContext* context)
{
    const auto node = context->node;
    if (!node->hasAstFlag(AST_FROM_GENERIC))
    {
        if (!node->ownerFct || !node->ownerFct->hasAstFlag(AST_FROM_GENERIC))
        {
            // If we are inside a generic structure, do not evaluate the instruction.
            // Will be done during instantiation
            if (node->ownerStructScope && node->ownerStructScope->owner->hasAstFlag(AST_GENERIC))
            {
                // Be sure we are inside a struct definition, and not in an impl block
                auto parent = node->parent;
                while (parent)
                {
                    if (parent->is(AstNodeKind::StructDecl))
                    {
                        node->addAstFlag(AST_GENERIC);
                        break;
                    }

                    if (parent->is(AstNodeKind::Impl))
                        break;
                    parent = parent->parent;
                }
            }

            // Same for a function
            if (node->ownerFct && node->ownerFct->hasAstFlag(AST_GENERIC))
                node->addAstFlag(AST_GENERIC);
        }
    }

    if (node->hasAstFlag(AST_GENERIC))
    {
        for (const auto& c : node->children)
            c->addAstFlag(AST_NO_SEMANTIC);
        node->addSemFlag(SEMFLAG_ON_CLONE);
    }

    return true;
}

bool Semantic::resolveCompilerPrint(SemanticContext* context)
{
    const auto node = context->node;
    if (node->hasAstFlag(AST_GENERIC))
        return true;

    const auto expr = context->node->firstChild();
    SWAG_CHECK(executeCompilerNode(context, expr, true));
    YIELD();

    g_Log.lock();
    TypeInfo* typeInfo = TypeManager::concreteType(expr->typeInfo);
    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
                g_Log.write(expr->computedValue()->reg.b ? "true" : "false");
                break;
            case NativeTypeKind::S8:
                g_Log.print(std::to_string(expr->computedValue()->reg.s8));
                break;
            case NativeTypeKind::S16:
                g_Log.print(std::to_string(expr->computedValue()->reg.s16));
                break;
            case NativeTypeKind::S32:
                g_Log.print(std::to_string(expr->computedValue()->reg.s32));
                break;
            case NativeTypeKind::S64:
                g_Log.print(std::to_string(expr->computedValue()->reg.s64));
                break;
            case NativeTypeKind::U8:
                g_Log.print(std::to_string(expr->computedValue()->reg.u8));
                break;
            case NativeTypeKind::U16:
                g_Log.print(std::to_string(expr->computedValue()->reg.u16));
                break;
            case NativeTypeKind::U32:
                g_Log.print(std::to_string(expr->computedValue()->reg.u32));
                break;
            case NativeTypeKind::U64:
                g_Log.print(std::to_string(expr->computedValue()->reg.u64));
                break;
            case NativeTypeKind::F32:
                g_Log.print(std::to_string(expr->computedValue()->reg.f32));
                break;
            case NativeTypeKind::F64:
                g_Log.print(std::to_string(expr->computedValue()->reg.f64));
                break;
            case NativeTypeKind::Rune:
                g_Log.print(std::to_string(expr->computedValue()->reg.ch));
                break;
            case NativeTypeKind::String:
                g_Log.print(expr->computedValue()->text);
                break;
            default:
                g_Log.print(form("<%s>", typeInfo->getDisplayNameC()));
                break;
        }
    }
    else
    {
        g_Log.print(form("<%s>", typeInfo->getDisplayNameC()));
    }

    g_Log.writeEol();
    g_Log.unlock();
    return true;
}

void Semantic::disableCompilerIfBlock(SemanticContext* context, AstCompilerIfBlock* block)
{
    block->addAstFlag(AST_NO_BYTECODE);
    block->addAstFlag(AST_NO_SEMANTIC);

    // Revert test errors in case the '#global testerror' is inside a disabled #if branch
    const auto sourceFile = context->sourceFile;

    // Unregister one overload
    for (const auto it : block->symbols)
    {
        ScopedLock lk(it.second->mutex);
        ScopedLock lk1(it.first->mutex);
        it.first->addAstFlag(AST_NO_SEMANTIC | AST_NO_BYTECODE);
        it.first->addSemFlag(SEMFLAG_DISABLED);
        SymTable::disabledIfBlockOverloadNoLock(it.first, it.second);
    }

    // Decrease interfaces count to resolve
    for (const auto typeStruct : block->interfacesCount)
    {
        sourceFile->module->decImplForToSolve(typeStruct);
        decreaseInterfaceRegCount(typeStruct);
        decreaseInterfaceCount(typeStruct);
    }

    // Remove methods
    Set<TypeInfoStruct*> allStructs;
    for (const auto& typeStructPair : block->methodsCount)
    {
        auto typeStruct = typeStructPair.typeInfo;
        allStructs.insert(typeStruct);
        typeStruct->methods[typeStructPair.methodIdx] = nullptr;
    }

    // Clean array of methods
    for (const auto typeStruct : allStructs)
    {
        for (uint32_t i = 0; i < typeStruct->methods.size(); i++)
        {
            if (!typeStruct->methods[i])
            {
                typeStruct->methods.erase_unordered(i);
                i--;
            }
        }
    }

    // Decrease methods count to resolve
    // Do this after the clean, to avoid race conditions on the 'methods' array
    for (const auto& typeStructPair : block->methodsCount)
    {
        const auto typeStruct = typeStructPair.typeInfo;
        decreaseMethodCount(typeStructPair.funcNode, typeStruct);
    }

    // Eliminate imports
    for (const auto imp : block->imports)
        sourceFile->module->removeDependency(imp);

    // Eliminate includes
    for (const auto imp : block->includes)
        sourceFile->module->removeFileToLoad(imp);

    // Do the same for all embedded blocks
    for (const auto p : block->blocks)
        disableCompilerIfBlock(context, p);
}

bool Semantic::resolveCompilerIf(SemanticContext* context)
{
    const auto node = castAst<AstIf>(context->node->parent, AstNodeKind::CompilerIf);
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, node->boolExpression, CAST_FLAG_AUTO_BOOL));

    SWAG_CHECK(executeCompilerNode(context, node->boolExpression, true));
    YIELD();

    node->boolExpression->addAstFlag(AST_NO_BYTECODE);
    AstNode* validatedNode;
    if (node->boolExpression->computedValue()->reg.b)
    {
        validatedNode = node->ifBlock;
        if (node->elseBlock)
            disableCompilerIfBlock(context, castAst<AstCompilerIfBlock>(node->elseBlock, AstNodeKind::CompilerIfBlock));
    }
    else
    {
        validatedNode = node->elseBlock;
        disableCompilerIfBlock(context, castAst<AstCompilerIfBlock>(node->ifBlock, AstNodeKind::CompilerIfBlock));
    }

    // We can now solve function sub declarations in that block
    if (validatedNode)
    {
        const auto ifBlock = castAst<AstCompilerIfBlock>(validatedNode, AstNodeKind::CompilerIfBlock);
        for (const auto n : ifBlock->subDecl)
            launchResolveSubDecl(context, n);
    }

    SWAG_CHECK(SemanticError::warnElseDoIf(context, node));

    return true;
}

bool Semantic::resolveCompilerInclude(SemanticContext* context)
{
    const auto job    = context->baseJob;
    const auto node   = context->node;
    const auto module = context->sourceFile->module;
    auto       back   = node->firstChild();

    SWAG_CHECK(checkIsConstExpr(context, back->hasFlagComputedValue(), back, toErr(Err0034)));
    SWAG_VERIFY(back->typeInfo == g_TypeMgr->typeInfoString, context->report({back, formErr(Err0548, back->typeInfo->getDisplayNameC())}));
    node->setFlagsValueIsComputed();

    if (!node->hasSemFlag(SEMFLAG_LOAD))
    {
        node->addSemFlag(SEMFLAG_LOAD);

        const auto filename = back->computedValue()->text;

        // Search first in the same folder as the source file
        Path fullFileName = node->token.sourceFile->path.parent_path();
        fullFileName.append(filename.cstr());
        std::error_code err;
        if (!std::filesystem::exists(fullFileName, err))
        {
            // Search relative to the module path
            fullFileName = node->token.sourceFile->module->path;
            fullFileName.append(filename.cstr());
            if (!std::filesystem::exists(fullFileName, err))
            {
                // Search the file itself, without any special path
                fullFileName = filename;
                if (!std::filesystem::exists(fullFileName, err))
                    return context->report({back, formErr(Err0678, filename.cstr())});
            }
        }

        struct stat statBuf;
        const int   rc = stat(fullFileName, &statBuf);
        SWAG_VERIFY(rc == 0, context->report({back, formErr(Err0729, back->computedValue()->text.cstr())}));
        SWAG_CHECK(context->checkSizeOverflow("[[#load]]", statBuf.st_size, SWAG_LIMIT_COMPILER_LOAD));

        const auto newJob         = Allocator::alloc<LoadFileJob>();
        const auto storageSegment = getConstantSegFromContext(node);
        uint8_t*   addrDst;
        node->computedValue()->storageOffset  = storageSegment->reserve(statBuf.st_size, &addrDst);
        node->computedValue()->storageSegment = storageSegment;
        newJob->destBuffer                    = addrDst;
        newJob->sizeBuffer                    = statBuf.st_size;

        newJob->module       = module;
        newJob->sourcePath   = fullFileName;
        newJob->dependentJob = job->dependentJob;
        newJob->addDependentJob(job);
        job->jobsToAdd.push_back(newJob);
        job->setPending(JobWaitKind::LoadFile, nullptr, node, nullptr);

        // Creates return type
        const auto ptrArray   = makeType<TypeInfoArray>();
        ptrArray->count       = statBuf.st_size;
        ptrArray->pointedType = g_TypeMgr->typeInfoU8;
        ptrArray->finalType   = g_TypeMgr->typeInfoU8;
        ptrArray->sizeOf      = ptrArray->count;
        ptrArray->totalCount  = statBuf.st_size;
        ptrArray->computeName();
        node->typeInfo = ptrArray;

        return true;
    }

    return true;
}

bool Semantic::resolveIntrinsicLocation(SemanticContext* context)
{
    const auto node    = context->node;
    auto       locNode = node->firstChild();
    node->typeInfo     = g_TypeMgr->makeConst(g_Workspace->swagScope.regTypeInfoSourceLoc);

    if (locNode->isWhereParam(locNode->resolvedSymbolOverload()))
    {
        node->removeAstFlag(AST_NO_BYTECODE);
        locNode->addAstFlag(AST_NO_BYTECODE);
        node->byteCodeFct = ByteCodeGen::emitIntrinsicLocationSI;
        return true;
    }

    bool done = false;
    while (true)
    {
        // If identifier is an inline param call replacement, take it
        if (locNode->resolvedSymbolOverload() && locNode->resolvedSymbolOverload()->fromInlineParam)
        {
            locNode = locNode->resolvedSymbolOverload()->fromInlineParam;
            done    = true;
            continue;
        }

        if (locNode->is(AstNodeKind::FuncCallParam) && locNode->firstChild()->is(AstNodeKind::IdentifierRef))
        {
            const auto id = castAst<AstIdentifier>(locNode->lastChild()->lastChild(), AstNodeKind::Identifier);
            if (id->identifierExtension && id->identifierExtension->fromAlternateVar)
            {
                locNode = id->identifierExtension->fromAlternateVar;
                done    = true;
                continue;
            }
        }

        // :ForLocationInWhere
        if (locNode->is(AstNodeKind::IdentifierRef))
        {
            const auto id = castAst<AstIdentifier>(locNode->lastChild(), AstNodeKind::Identifier);
            if (id->identifierExtension && id->identifierExtension->fromAlternateVar)
            {
                locNode = id->identifierExtension->fromAlternateVar;
                done    = true;
                continue;
            }
        }

        if (locNode->ownerFct && locNode->ownerFct->typeInfo && locNode->ownerFct->requestedGeneric)
        {
            const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(locNode->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
            for (const auto& val : typeFunc->replaceTypes | std::views::values)
            {
                bool fromGen = false;
                if (val.typeInfoReplace == locNode->typeInfo)
                    fromGen = true;
                else if (locNode->typeInfo->isPointer() && val.typeInfoReplace == castTypeInfo<TypeInfoPointer>(locNode->typeInfo)->pointedType)
                    fromGen = true;
                else if (locNode->typeInfo->isSlice() && val.typeInfoReplace == castTypeInfo<TypeInfoSlice>(locNode->typeInfo)->pointedType)
                    fromGen = true;
                else if (locNode->typeInfo->isArray() && val.typeInfoReplace == castTypeInfo<TypeInfoArray>(locNode->typeInfo)->finalType)
                    fromGen = true;

                if (fromGen && val.fromNode)
                {
                    locNode = val.fromNode;
                    done    = true;
                    continue;
                }
            }
        }

        break;
    }

    // If identifier is an inline param call replacement, take it
    if (locNode->resolvedSymbolOverload() && !done)
        locNode = locNode->resolvedSymbolOverload()->node;

    node->setFlagsValueIsComputed();
    ByteCodeGen::computeSourceLocation(context, locNode, &node->computedValue()->storageOffset, &node->computedValue()->storageSegment);
    SWAG_CHECK(setupIdentifierRef(node));
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveIntrinsicDefined(SemanticContext* context)
{
    const auto node = context->node;
    node->setFlagsValueIsComputed();
    node->computedValue()->reg.b = node->lastChild()->resolvedSymbolOverload() != nullptr;
    node->typeInfo               = g_TypeMgr->typeInfoBool;
    return true;
}

Utf8 Semantic::getCompilerFunctionString(const AstNode* node, TokenId id)
{
    switch (id)
    {
        case TokenId::CompilerCallerFunction:
            return node->ownerFct->getNameForUserCompiler();
        case TokenId::CompilerBuildCfg:
            return g_CommandLine.buildCfg;
        case TokenId::CompilerCpu:
            return g_CommandLine.target.cpu;
        default:
            SWAG_ASSERT(false);
            break;
    }

    return "";
}

bool Semantic::resolveCompilerSpecialValue(SemanticContext* context)
{
    auto node = context->node;
    switch (node->token.id)
    {
        case TokenId::CompilerBackend:
            node->setFlagsValueIsComputed();
            switch (g_CommandLine.backendGenType)
            {
                case BackendGenType::SCBE:
                    node->computedValue()->reg.u64 = static_cast<uint64_t>(SwagBackendGenType::SCBE);
                    break;
                case BackendGenType::LLVM:
                    node->computedValue()->reg.u64 = static_cast<uint64_t>(SwagBackendGenType::LLVM);
                    break;
            }
            node->typeInfo = g_Workspace->swagScope.regTypeInfoBackend;
            SWAG_ASSERT(node->typeInfo);
            return true;

        case TokenId::CompilerOs:
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.u64 = static_cast<uint64_t>(g_CommandLine.target.os);
            node->typeInfo                 = g_Workspace->swagScope.regTypeInfoTargetOs;
            SWAG_ASSERT(node->typeInfo);
            return true;

        case TokenId::CompilerArch:
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.u64 = static_cast<uint64_t>(g_CommandLine.target.arch);
            node->typeInfo                 = g_Workspace->swagScope.regTypeInfoTargetArch;
            SWAG_ASSERT(node->typeInfo);
            return true;

        case TokenId::CompilerCpu:
            node->setFlagsValueIsComputed();
            node->computedValue()->text = getCompilerFunctionString(node, node->token.id);
            node->typeInfo              = g_TypeMgr->typeInfoString;
            return true;

        case TokenId::CompilerSwagOs:
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.u64 = static_cast<uint64_t>(OS::getNativeTarget().os);
            node->typeInfo                 = g_Workspace->swagScope.regTypeInfoTargetOs;
            SWAG_ASSERT(node->typeInfo);
            return true;

        case TokenId::CompilerBuildCfg:
            node->setFlagsValueIsComputed();
            node->computedValue()->text = getCompilerFunctionString(node, node->token.id);
            node->typeInfo              = g_TypeMgr->typeInfoString;
            return true;

        case TokenId::CompilerCurLocation:
            node->typeInfo = g_TypeMgr->makeConst(g_Workspace->swagScope.regTypeInfoSourceLoc);
            node->setFlagsValueIsComputed();
            ByteCodeGen::computeSourceLocation(context, node, &node->computedValue()->storageOffset, &node->computedValue()->storageSegment);
            SWAG_CHECK(setupIdentifierRef(node));
            return true;

        case TokenId::CompilerCallerLocation:
            SWAG_VERIFY(node->parent->is(AstNodeKind::FuncDeclParam), context->report({node, toErr(Err0385)}));
            node->typeInfo = g_Workspace->swagScope.regTypeInfoSourceLoc;
            return true;

        case TokenId::CompilerCallerFunction:
            SWAG_VERIFY(node->parent->is(AstNodeKind::FuncDeclParam), context->report({node, toErr(Err0384)}));
            node->typeInfo = g_TypeMgr->typeInfoString;
            return true;

        default:
            return Report::internalError(context->node, "resolveCompilerFunction, unknown token");
    }
}

#define CHECK_SAFETY_NAME(__name, __flag)                                        \
    do                                                                           \
    {                                                                            \
        if (w == g_LangSpec->__name)                                             \
        {                                                                        \
            done                         = true;                                 \
            node->computedValue()->reg.b = module->mustEmitSafety(node, __flag); \
        }                                                                        \
    } while (0)

bool Semantic::resolveCompilerIntrinsicTag(SemanticContext* context)
{
    const auto node = context->node;
    switch (node->token.id)
    {
        case TokenId::CompilerIntrinsicSafety:
        {
            auto front = node->firstChild();
            SWAG_CHECK(evaluateConstExpression(context, front));
            YIELD();
            SWAG_CHECK(checkIsConstExpr(context, front->hasFlagComputedValue(), front, toErr(Err0035), node->token.text));
            SWAG_VERIFY(front->typeInfo->isString(), context->report({front, formErr(Err0565, node->token.cstr(), front->typeInfo->getDisplayNameC())}));
            node->typeInfo = g_TypeMgr->typeInfoBool;
            node->setFlagsValueIsComputed();

            const auto& w      = front->computedValue()->text;
            const auto  module = node->token.sourceFile->module;
            bool        done   = false;

            CHECK_SAFETY_NAME(name_boundcheck, SAFETY_BOUND_CHECK);
            CHECK_SAFETY_NAME(name_overflow, SAFETY_OVERFLOW);
            CHECK_SAFETY_NAME(name_math, SAFETY_MATH);
            CHECK_SAFETY_NAME(name_switch, SAFETY_SWITCH);
            CHECK_SAFETY_NAME(name_unreachable, SAFETY_UNREACHABLE);
            CHECK_SAFETY_NAME(name_dyncast, SAFETY_DYN_CAST);
            CHECK_SAFETY_NAME(name_bool, SAFETY_BOOL);
            CHECK_SAFETY_NAME(name_nan, SAFETY_NAN);
            CHECK_SAFETY_NAME(name_null, SAFETY_NULL);

            if (!done)
            {
                return context->report({front, front->token, formErr(Err0698, w.cstr())});
            }

            return true;
        }

        case TokenId::CompilerIntrinsicHasTag:
        {
            auto front = node->firstChild();
            SWAG_CHECK(evaluateConstExpression(context, front));
            YIELD();
            SWAG_CHECK(checkIsConstExpr(context, front->hasFlagComputedValue(), front, toErr(Err0035), node->token.text));
            SWAG_VERIFY(front->typeInfo->isString(), context->report({front, formErr(Err0565, node->token.cstr(), front->typeInfo->getDisplayNameC())}));
            const auto tag = g_Workspace->hasTag(front->computedValue()->text);
            node->typeInfo = g_TypeMgr->typeInfoBool;
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.b = tag ? true : false;
            return true;
        }

        case TokenId::CompilerIntrinsicGetTag:
        {
            auto       nameNode   = node->firstChild();
            const auto typeNode   = node->secondChild();
            auto       defaultVal = node->children[2];
            SWAG_CHECK(evaluateConstExpression(context, nameNode));
            YIELD();
            SWAG_CHECK(evaluateConstExpression(context, defaultVal));
            YIELD();

            SWAG_CHECK(checkIsConstExpr(context, nameNode->hasFlagComputedValue(), nameNode, toErr(Err0035), node->token.text));
            SWAG_VERIFY(nameNode->typeInfo->isString(), context->report({nameNode, formErr(Err0565, node->token.cstr(), nameNode->typeInfo->getDisplayNameC())}));
            SWAG_VERIFY(defaultVal->hasComputedValue(), context->report({defaultVal, formErr(Err0158, Naming::aKindName(defaultVal).cstr())}));

            {
                PushErrCxtStep ec(context, typeNode, ErrCxtStepKind::Note, [] { return toNte(Nte0112); });
                SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, defaultVal->typeInfo, nullptr, defaultVal, CAST_FLAG_DEFAULT));
            }

            node->setFlagsValueIsComputed();
            node->typeInfo = typeNode->typeInfo;

            const auto tag = g_Workspace->hasTag(nameNode->computedValue()->text);
            if (tag)
            {
                if (!TypeManager::makeCompatibles(context, typeNode->typeInfo, tag->type, nullptr, typeNode, CAST_FLAG_JUST_CHECK))
                {
                    Diagnostic err{typeNode, formErr(Err0766, typeNode->typeInfo->getDisplayNameC(), tag->type->getDisplayNameC(), tag->name.cstr())};
                    err.addNote(formNte(Nte0117, tag->cmdLine.cstr()));
                    return context->report(err);
                }

                node->typeInfo         = tag->type;
                *node->computedValue() = tag->value;
            }
            else
            {
                node->extSemantic()->computedValue = defaultVal->extSemantic()->computedValue;
            }

            return true;
        }
    }
    return true;
}

bool Semantic::resolveCompilerIntrinsicStringOf(SemanticContext* context)
{
    const auto node     = context->node;
    auto       expr     = node->firstChild();
    const auto typeInfo = expr->typeInfo;

    node->setFlagsValueIsComputed();
    SWAG_CHECK(checkIsConcreteOrType(context, expr, true));
    YIELD();

    if (expr->hasFlagComputedValue())
    {
        if (expr->isConstantGenTypeInfo())
            node->computedValue()->text = Utf8{expr->getConstantGenTypeInfo()->fullName};
        else if (typeInfo->isString())
            node->computedValue()->text = expr->computedValue()->text;
        else if (typeInfo->isNative())
            node->computedValue()->text = Ast::literalToString(typeInfo, *expr->computedValue());
        else if (typeInfo->isEnum())
            node->computedValue()->text = Ast::enumToString(typeInfo, expr->computedValue()->text, expr->computedValue()->reg);
        else
            return context->report({expr, toErr(Err0784)});
    }
    else if (expr->typeInfo->isCode())
    {
        FormatAst     fmtAst;
        FormatContext fmtCxt;
        const auto    typeCode = castTypeInfo<TypeInfoCode>(expr->typeInfo, TypeInfoKind::Code);
        fmtAst.outputNode(fmtCxt, typeCode->content);
        node->computedValue()->text = fmtAst.getUtf8();
    }
    else if (expr->resolvedSymbolName())
        node->computedValue()->text = expr->resolvedSymbolName()->getFullName();
    else if (expr->resolvedSymbolOverload())
        node->computedValue()->text = expr->resolvedSymbolOverload()->symbol->getFullName();
    else
        return context->report({expr, toErr(Err0784)});

    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool Semantic::resolveCompilerIntrinsicNameOf(SemanticContext* context)
{
    const auto node = context->node;
    auto       expr = node->firstChild();

    node->setFlagsValueIsComputed();
    SWAG_CHECK(checkIsConcreteOrType(context, expr, true));
    YIELD();

    if (expr->isConstantGenTypeInfo())
        node->computedValue()->text = Utf8{expr->getConstantGenTypeInfo()->name};
    else if (expr->resolvedSymbolName())
        node->computedValue()->text = expr->resolvedSymbolName()->name;
    else if (expr->resolvedSymbolOverload())
        node->computedValue()->text = expr->resolvedSymbolOverload()->symbol->name;
    else
        return context->report({expr, toErr(Err0783)});

    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool Semantic::resolveCompilerIntrinsicTypeOf(SemanticContext* context)
{
    const auto node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto       expr     = node->firstChild();
    const auto typeInfo = expr->typeInfo;

    SWAG_VERIFY(!typeInfo->isKindGeneric(), context->report({expr, toErr(Err0252)}));

    expr->addAstFlag(AST_NO_BYTECODE);

    SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo));
    YIELD();
    node->inheritComputedValue(expr);
    SWAG_CHECK(setupIdentifierRef(node));
    return true;
}

bool Semantic::resolveCompilerIntrinsicDeclType(SemanticContext* context)
{
    const auto node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto       expr     = node->firstChild();
    auto       typeInfo = expr->typeInfo;

    SWAG_VERIFY(!typeInfo->isKindGeneric(), context->report({expr, toErr(Err0252)}));

    if (expr->hasAstFlag(AST_CONST_EXPR))
    {
        SWAG_CHECK(executeCompilerNode(context, expr, true));
        YIELD();
    }

    expr->addAstFlag(AST_NO_BYTECODE);

    // @mktype on a typeinfo will give back the original compiler type
    if (typeInfo->isPointerToTypeInfo() &&
        expr->hasFlagComputedValue() &&
        expr->computedValue()->storageOffset != UINT32_MAX &&
        expr->computedValue()->storageSegment != nullptr)
    {
        const auto addr        = expr->computedValue()->getStorageAddr();
        const auto newTypeInfo = context->sourceFile->module->typeGen.getRealType(expr->computedValue()->storageSegment, static_cast<ExportedTypeInfo*>(addr));
        if (newTypeInfo)
            typeInfo = newTypeInfo;
    }

    // Should be a lambda
    if (typeInfo->isFuncAttr() && !typeInfo->hasFlag(TYPEINFO_FUNC_IS_ATTR))
    {
        typeInfo         = typeInfo->clone();
        typeInfo->kind   = TypeInfoKind::LambdaClosure;
        typeInfo->sizeOf = sizeof(void*);
    }

    node->typeInfo = typeInfo;
    return true;
}

bool Semantic::resolveCompilerIntrinsicRunes(SemanticContext* context)
{
    const auto node     = context->node;
    auto       expr     = node->firstChild();
    const auto typeInfo = expr->typeInfo;

    SWAG_CHECK(checkIsConstExpr(context, expr->hasFlagComputedValue(), expr));
    SWAG_VERIFY(typeInfo->isString(), context->report({expr, formErr(Err0551, typeInfo->getDisplayNameC())}));
    node->setFlagsValueIsComputed();

    // Convert
    VectorNative<uint32_t> runes;
    const char*            pz  = expr->computedValue()->text.buffer;
    uint32_t               cpt = 0;
    while (cpt < expr->computedValue()->text.count)
    {
        uint32_t c;
        uint32_t offset;
        pz = Utf8::decodeUtf8(pz, c, offset);
        runes.push_back(c);
        cpt += offset;
    }

    // :SliceLiteral
    const auto storageSegment             = getConstantSegFromContext(context->node);
    node->computedValue()->storageSegment = storageSegment;

    SwagSlice* slice;
    node->computedValue()->storageOffset = storageSegment->reserve(sizeof(SwagSlice), reinterpret_cast<uint8_t**>(&slice));
    slice->count                         = runes.size();

    uint8_t* addrDst;
    storageSegment->reserve(runes.size() * sizeof(uint32_t), &addrDst);
    slice->buffer = addrDst;

    // Setup array
    std::copy_n(runes.buffer, runes.size(), reinterpret_cast<uint32_t*>(addrDst));

    node->typeInfo = g_TypeMgr->typeInfoSliceRunes;
    return true;
}

bool Semantic::resolveCompilerIntrinsicIsConstExpr(const SemanticContext* context)
{
    const auto node = context->node;
    const auto expr = node->firstChild();
    node->typeInfo  = g_TypeMgr->typeInfoBool;
    expr->addAstFlag(AST_NO_BYTECODE);

    // Special case for a function parameter in a where block, should be done at runtime
    if (expr->isWhereParam(expr->resolvedSymbolOverload()))
    {
        node->byteCodeFct = ByteCodeGen::emitIntrinsicIsConstExprSI;
    }
    else
    {
        node->setFlagsValueIsComputed();
        node->computedValue()->reg.b = expr->hasFlagComputedValue();
    }

    return true;
}

bool Semantic::resolveCompilerIntrinsicSizeOf(SemanticContext* context)
{
    const auto node = context->node;
    auto       expr = node->firstChild();
    SWAG_VERIFY(!expr->typeInfo->isGeneric(), context->report({expr, toErr(Err0122)}));
    node->setFlagsValueIsComputed();
    node->computedValue()->reg.u64 = expr->typeInfo->sizeOf;
    if (node->computedValue()->reg.u64 > UINT32_MAX)
        node->typeInfo = g_TypeMgr->typeInfoU64;
    else
        node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
    return true;
}

bool Semantic::resolveCompilerIntrinsicAlignOf(SemanticContext* context)
{
    const auto node = context->node;
    auto       expr = node->firstChild();
    SWAG_VERIFY(!expr->typeInfo->isGeneric(), context->report({expr, toErr(Err0119)}));
    node->setFlagsValueIsComputed();
    node->computedValue()->reg.u64 = TypeManager::alignOf(expr->typeInfo);
    if (node->computedValue()->reg.u64 > UINT32_MAX)
        node->typeInfo = g_TypeMgr->typeInfoU64;
    else
        node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
    return true;
}

bool Semantic::resolveCompilerIntrinsicOffsetOf(SemanticContext* context)
{
    const auto node = context->node;
    const auto expr = node->firstChild();
    SWAG_CHECK(Semantic::checkIsConstExpr(context, expr->resolvedSymbolOverload(), expr));
    node->setFlagsValueIsComputed();
    node->computedValue()->reg.u64 = expr->resolvedSymbolOverload()->computedValue.storageOffset;
    if (node->computedValue()->reg.u64 > UINT32_MAX)
        node->typeInfo = g_TypeMgr->typeInfoU64;
    else
        node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
    return true;
}
