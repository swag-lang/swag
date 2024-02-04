#include "pch.h"
#include "Backend.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "Context.h"
#include "FileJob.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Parser.h"
#include "Report.h"
#include "Semantic.h"
#include "TypeManager.h"
#include "Workspace.h"

Diagnostic* Semantic::computeNonConstExprNote(AstNode* node)
{
    VectorNative<AstNode*>        childs;
    Ast::visit(node, [&](AstNode* n)
    {
        childs.push_back(n);
    });

    for (int i = (int) childs.size() - 1; i >= 0; i--)
    {
        const auto c = childs[i];
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
                        const auto result = Diagnostic::note(c, c->token, Fmt(Nte(Nte0117), c->resolvedSymbolName->name.c_str()));
                        result->hint      = Nte(Nte0079);
                        return result;
                    }

                    return Diagnostic::note(c, c->token, Nte(Nte0153));
                }
            }

            if (c->resolvedSymbolName->kind == SymbolKind::Variable)
            {
                return Diagnostic::note(c, c->token, Fmt(Nte(Nte0005), c->resolvedSymbolName->name.c_str()));
            }

            return Diagnostic::note(c, Nte(Nte0056));
        }
    }

    return nullptr;
}

bool Semantic::executeCompilerNode(SemanticContext* context, AstNode* node, bool onlyConstExpr)
{
    // No need to run, this is already baked
    if (node->hasComputedValue())
        return true;

    if (onlyConstExpr)
        SWAG_CHECK(checkIsConstExpr(context, node));

    // In case of error, it's not always relevant to show the context
    bool showContext = true;
    if (node->token.text.length() > 1 && node->token.text[0] == '_' && node->token.text[1] == '_')
        showContext = false;
    if (node->token.text[0] == '@')
        showContext = false;

    // Push a copy of the default context, in case the user code changes it (or push a new one)
    // :PushDefaultCxt
    PushSwagContext cxt;

    if (showContext)
    {
        PushErrCxtStep ec(context, node, ErrCxtStepKind::CompileTime, nullptr);
        return doExecuteCompilerNode(context, node, onlyConstExpr);
    }
    else
    {
        return doExecuteCompilerNode(context, node, onlyConstExpr);
    }
}

bool Semantic::doExecuteCompilerNode(SemanticContext* context, AstNode* node, bool onlyConstExpr)
{
    // No need to run, this is already baked
    if (node->hasComputedValue())
        return true;

    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    // Request to generate the corresponding bytecode
    {
        ByteCodeGen::askForByteCode(context->baseJob, node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED);
        YIELD();
    }

    // :CheckConstExprFuncReturnType
    // Be sure we can deal with the type at compile time
    ExecuteNodeParams execParams;
    execParams.forConstExpr = onlyConstExpr;

    if (!(node->semFlags & SEMFLAG_EXEC_RET_STACK))
    {
        auto realType = TypeManager::concreteType(node->typeInfo);
        if (realType && realType->isStruct() && !(realType->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR))
        {
            if (realType->isTuple())
            {
                Diagnostic diag{node, Err(Err0043)};
                return context->report(diag);
            }

            // It is possible to convert a complex struct to a constant static array of values if the struct
            // implements 'opCount' and 'opSlice'
            Semantic::waitAllStructMethods(context->baseJob, realType);
            YIELD();

            if (node->hasSpecialFuncCall())
            {
                Diagnostic diag{node, Fmt(Err(Err0042), realType->getDisplayNameC())};
                diag.hint = Fmt(Nte(Nte0144), node->extMisc()->resolvedUserOpSymbolOverload->symbol->name.c_str());
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
                if (node->flags & AST_IN_RUN_BLOCK)
                {
                    node->semFlags |= SEMFLAG_FORCE_CONST_EXPR;
                }
                else
                {
                    Diagnostic diag{node, Fmt(Err(Err0042), realType->getDisplayNameC())};
                    diag.hint = Nte(Nte0079);
                    return context->report(diag);
                }
            }
            else
            {
                SWAG_ASSERT(!context->node->hasExtMisc() || !context->node->extMisc()->resolvedUserOpSymbolOverload);
                auto saveTypeStruct = node->typeInfo;

                // opCount
                VectorNative<AstNode*> params;
                params.push_back(node);
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opCount, nullptr, nullptr, node, params));
                node->typeInfo = saveTypeStruct;
                YIELD();

                SWAG_ASSERT(context->node->hasExtMisc());

                auto extension                          = context->node->extMisc();
                execParams.specReturnOpCount            = extension->resolvedUserOpSymbolOverload;
                extension->resolvedUserOpSymbolOverload = nullptr;
                SWAG_ASSERT(execParams.specReturnOpCount);

                ByteCodeGen::askForByteCode(context->baseJob, execParams.specReturnOpCount->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
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

                execParams.specReturnOpSlice            = extension->resolvedUserOpSymbolOverload;
                extension->resolvedUserOpSymbolOverload = nullptr;
                SWAG_ASSERT(execParams.specReturnOpSlice);

                ByteCodeGen::askForByteCode(context->baseJob, execParams.specReturnOpSlice->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
                YIELD();

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
                        Diagnostic diag{node, Fmt(Err(Err0032), typeSliceContent->getDisplayNameC())};
                        return context->report(diag);
                    }
                }
                else
                {
                    Diagnostic diag{node, Fmt(Err(Err0029), concreteType->getDisplayNameC())};
                    return context->report(diag);
                }

                // opPostMove
                SymbolName* symPostMove = nullptr;
                SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opPostMove, (TypeInfoStruct*) realType, &symPostMove));
                if (symPostMove)
                {
                    params.clear();
                    params.push_back(node);
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opPostMove, nullptr, nullptr, node, params));
                    node->typeInfo = saveTypeStruct;
                    YIELD();

                    execParams.specReturnOpPostMove         = extension->resolvedUserOpSymbolOverload;
                    extension->resolvedUserOpSymbolOverload = nullptr;
                    SWAG_ASSERT(execParams.specReturnOpPostMove);

                    {
                        ByteCodeGen::askForByteCode(context->baseJob, execParams.specReturnOpPostMove->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
                        YIELD();
                    }
                }

                // opDrop
                SymbolName* symDrop = nullptr;
                SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opDrop, (TypeInfoStruct*) realType, &symDrop));
                if (symDrop)
                {
                    params.clear();
                    params.push_back(node);
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opDrop, nullptr, nullptr, node, params));
                    node->typeInfo = saveTypeStruct;
                    YIELD();

                    execParams.specReturnOpDrop             = extension->resolvedUserOpSymbolOverload;
                    extension->resolvedUserOpSymbolOverload = nullptr;
                    SWAG_ASSERT(execParams.specReturnOpDrop);

                    ByteCodeGen::askForByteCode(context->baseJob, execParams.specReturnOpDrop->node, ASKBC_WAIT_DONE | ASKBC_WAIT_RESOLVED | ASKBC_WAIT_SEMANTIC_RESOLVED);
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
    if (node->attributeFlags & ATTRIBUTE_PRINT_GEN_BC)
    {
        ByteCodePrintOptions opt;
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
    module->addForeignLib(node->childs.front()->token.text);
    return true;
}

bool Semantic::resolveCompilerRun(SemanticContext* context)
{
    const auto node = CastAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerRun, AstNodeKind::CompilerRunExpression);
    if (node->flags & AST_IS_GENERIC)
        return true;

    const auto expression = context->node->childs.back();

    if (!expression->typeInfo)
        expression->typeInfo = g_TypeMgr->typeInfoVoid;

    SWAG_CHECK(executeCompilerNode(context, expression, false));
    YIELD();

    context->node->inheritComputedValue(expression);
    if (context->node->hasComputedValue())
        context->node->flags &= ~AST_NO_BYTECODE;

    context->node->typeInfo = expression->typeInfo;
    return true;
}

bool Semantic::resolveCompilerValidIfExpression(SemanticContext* context)
{
    const auto node = CastAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerValidIf, AstNodeKind::CompilerValidIfx);
    if (node->flags & AST_IS_GENERIC)
        return true;

    const auto expression = context->node->childs.back();
    const auto typeInfo   = TypeManager::concreteType(expression->typeInfo);
    if (!typeInfo->isBool())
    {
        const Diagnostic diag{expression, Fmt(Err(Err0191), node->token.ctext(), typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    return true;
}

bool Semantic::resolveCompilerAstExpression(SemanticContext* context)
{
    const auto node = CastAst<AstCompilerSpecFunc>(context->node, AstNodeKind::CompilerAst);
    if (node->flags & AST_IS_GENERIC)
        return true;

    // Do it once (in case of inline)
    if (node->specFlags & AstCompilerSpecFunc::SPECFLAG_AST_BLOCK)
        return true;

    const auto job        = context->baseJob;
    auto       expression = context->node->childs.back();
    const auto typeInfo   = TypeManager::concreteType(expression->typeInfo);
    SWAG_VERIFY(typeInfo->isString(), context->report({expression, Fmt(Err(Err0620), expression->typeInfo->getDisplayNameC())}));

    SWAG_CHECK(executeCompilerNode(context, expression, false));
    YIELD();
    node->addSpecFlags(AstCompilerSpecFunc::SPECFLAG_AST_BLOCK);

    SWAG_CHECK(checkIsConstExpr(context, expression->hasComputedValue(), expression));

    node->allocateExtension(ExtensionKind::Owner);
    node->extOwner()->nodesToFree.append(node->childs);
    node->childs.clear();

    if (!expression->computedValue->text.empty())
    {
        CompilerAstKind kind;
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

bool Semantic::resolveCompilerError(SemanticContext* context)
{
    const auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

    const auto msg = node->childs.front();
    SWAG_CHECK(evaluateConstExpression(context, msg));
    YIELD();
    SWAG_CHECK(checkIsConstExpr(context, msg->hasComputedValue(), msg, Fmt(Err(Err0034), node->token.ctext())));
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

    const Diagnostic diag{node, node->token, Fmt(Err(Err0001), msg->computedValue->text.c_str()), DiagnosticLevel::Error};
    return context->report(diag);
}

bool Semantic::resolveCompilerWarning(SemanticContext* context)
{
    const auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

    const auto msg = node->childs.front();
    SWAG_CHECK(evaluateConstExpression(context, msg));
    YIELD();
    SWAG_CHECK(checkIsConstExpr(context, msg->hasComputedValue(), msg, Fmt(Err(Err0034), node->token.ctext())));
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

    const Diagnostic diag{node, node->token, msg->computedValue->text, DiagnosticLevel::Warning};
    return context->report(diag);
}

bool Semantic::resolveCompilerAssert(SemanticContext* context)
{
    auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

    // Expression to check
    const auto expr = node->childs[0];
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, expr, CASTFLAG_AUTO_BOOL | CASTFLAG_JUST_CHECK));
    SWAG_CHECK(executeCompilerNode(context, expr, true));
    YIELD();

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, expr, CASTFLAG_AUTO_BOOL));
    node->flags |= AST_NO_BYTECODE;

    if (!expr->computedValue->reg.b)
        return context->report({node, node->token, Err(Err0045)});
    return true;
}

bool Semantic::resolveCompilerMacro(SemanticContext* context)
{
    auto       node       = context->node;
    const auto scope      = node->childs.back()->ownerScope;
    scope->startStackSize = node->ownerScope->startStackSize;

    node->setBcNotifyAfter(ByteCodeGen::emitLeaveScope);

    // Be sure #macro is used inside a macro
    if (!node->ownerInline || (node->ownerInline->attributeFlags & ATTRIBUTE_MIXIN) || !(node->ownerInline->attributeFlags & ATTRIBUTE_MACRO))
        return context->report({node, Err(Err0444)});

    return true;
}

bool Semantic::resolveCompilerMixin(SemanticContext* context)
{
    const auto node = CastAst<AstCompilerMixin>(context->node, AstNodeKind::CompilerMixin);

    if (node->semFlags & SEMFLAG_COMPILER_INSERT)
    {
        node->typeInfo = node->childs.back()->typeInfo;
        return true;
    }

    node->semFlags |= SEMFLAG_COMPILER_INSERT;

    auto expr = node->childs[0];
    SWAG_VERIFY(expr->typeInfo->isCode(), context->report({expr, Fmt(Err(Err0193), expr->typeInfo->getDisplayNameC())}));

    node->setBcNotifyBefore(ByteCodeGen::emitDebugNop);
    node->byteCodeFct = ByteCodeGen::emitDebugNop;
    expr->flags |= AST_NO_BYTECODE;

    const auto typeCode = CastTypeInfo<TypeInfoCode>(expr->typeInfo, TypeInfoKind::Code);
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
    const auto cloneContent             = typeCode->content->clone(cloneContext);
    cloneContent->allocateExtension(ExtensionKind::Misc);
    cloneContent->extMisc()->alternativeNode = typeCode->content->parent;
    cloneContent->addAlternativeScope(typeCode->content->parent->ownerScope);
    cloneContent->flags &= ~AST_NO_SEMANTIC;
    node->typeInfo = cloneContent->typeInfo;
    context->baseJob->nodes.push_back(cloneContent);
    context->result = ContextResult::NewChilds;

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::preResolveCompilerInstruction(SemanticContext* context)
{
    const auto node = context->node;
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
        for (const auto& c : node->childs)
            c->flags |= AST_NO_SEMANTIC;
        node->semFlags |= SEMFLAG_ON_CLONE;
    }

    return true;
}

bool Semantic::resolveCompilerPrint(SemanticContext* context)
{
    const auto node = context->node;
    if (node->flags & AST_IS_GENERIC)
        return true;

    const auto expr = context->node->childs[0];
    SWAG_CHECK(executeCompilerNode(context, expr, true));
    YIELD();

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

void Semantic::disableCompilerIfBlock(SemanticContext* context, AstCompilerIfBlock* block)
{
    block->flags |= AST_NO_BYTECODE;
    block->flags |= AST_NO_SEMANTIC;

    // Revert test errors in case #global testerror is inside a disabled #if branch
    const auto sourceFile = context->sourceFile;

    // Unregister one overload
    for (const auto it : block->symbols)
    {
        ScopedLock lk(it.second->mutex);
        ScopedLock lk1(it.first->mutex);
        it.first->flags |= AST_NO_SEMANTIC | AST_NO_BYTECODE;
        it.first->semFlags |= SEMFLAG_DISABLED;
        SymTable::disabledIfBlockOverloadNoLock(it.first, it.second);
    }

    // Decrease interfaces count to resolve
    for (const auto typeStruct : block->interfacesCount)
    {
        sourceFile->module->decImplForToSolve(typeStruct);
        decreaseInterfaceRegCount(typeStruct);
        decreaseInterfaceCount(typeStruct);
    }

    // Decrease methods count to resolve
    Set<TypeInfoStruct*> allStructs;
    for (const auto& typeStructPair : block->methodsCount)
    {
        // Remove the corresponding method
        auto typeStruct = typeStructPair.typeInfo;
        allStructs.insert(typeStruct);

        typeStruct->methods[typeStructPair.methodIdx] = nullptr;

        // Then decrease wanted method number count
        decreaseMethodCount(typeStructPair.funcNode, typeStruct);
    }

    // Clean array
    for (const auto typeStruct : allStructs)
    {
        for (size_t i = 0; i < typeStruct->methods.size(); i++)
        {
            if (!typeStruct->methods[i])
            {
                typeStruct->methods.erase_unordered(i);
                i--;
            }
        }
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
    const auto node = CastAst<AstIf>(context->node->parent, AstNodeKind::CompilerIf);
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    SWAG_CHECK(executeCompilerNode(context, node->boolExpression, true));
    YIELD();

    node->boolExpression->flags |= AST_NO_BYTECODE;
    AstCompilerIfBlock* validatedNode;
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
        for (const auto n : validatedNode->subDecls)
            launchResolveSubDecl(context, n);
    }

    return true;
}

bool Semantic::resolveCompilerInclude(SemanticContext* context)
{
    const auto job    = context->baseJob;
    const auto node   = context->node;
    const auto module = context->sourceFile->module;
    auto       back   = node->childs[0];

    SWAG_CHECK(checkIsConstExpr(context, back->hasComputedValue(), back, Err(Err0030)));
    SWAG_VERIFY(back->typeInfo == g_TypeMgr->typeInfoString, context->report({back, Fmt(Err(Err0192), back->typeInfo->getDisplayNameC())}));
    node->setFlagsValueIsComputed();

    if (!(node->semFlags & SEMFLAG_LOAD))
    {
        node->semFlags |= SEMFLAG_LOAD;

        const auto filename = back->computedValue->text;

        // Search first in the same folder as the source file
        Path fullFileName = node->sourceFile->path.parent_path();
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
                    return context->report({back, Fmt(Err(Err0710), filename.c_str())});
            }
        }

        struct stat stat_buf;
        const int   rc = stat(fullFileName.string().c_str(), &stat_buf);
        SWAG_VERIFY(rc == 0, context->report({back, Fmt(Err(Err0097), back->computedValue->text.c_str())}));
        SWAG_CHECK(context->checkSizeOverflow("[[#load]]", stat_buf.st_size, SWAG_LIMIT_COMPILER_LOAD));

        const auto newJob         = Allocator::alloc<LoadFileJob>();
        const auto storageSegment = getConstantSegFromContext(node);
        uint8_t*   addrDst;
        node->computedValue->storageOffset  = storageSegment->reserve(stat_buf.st_size, &addrDst);
        node->computedValue->storageSegment = storageSegment;
        newJob->destBuffer                  = addrDst;
        newJob->sizeBuffer                  = stat_buf.st_size;

        newJob->module       = module;
        newJob->sourcePath   = fullFileName;
        newJob->dependentJob = job->dependentJob;
        newJob->addDependentJob(job);
        job->jobsToAdd.push_back(newJob);
        job->setPending(JobWaitKind::LoadFile, nullptr, node, nullptr);

        // Creates return type
        const auto ptrArray   = makeType<TypeInfoArray>();
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

bool Semantic::resolveIntrinsicLocation(SemanticContext* context)
{
    const auto node    = context->node;
    auto       locNode = node->childs.front();
    node->typeInfo     = g_TypeMgr->makeConst(g_Workspace->swagScope.regTypeInfoSourceLoc);

    if (locNode->isValidIfParam(locNode->resolvedSymbolOverload))
    {
        node->flags &= ~AST_NO_BYTECODE;
        locNode->flags |= AST_NO_BYTECODE;
        node->byteCodeFct = ByteCodeGen::emitIntrinsicLocationSI;
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
            const auto id = CastAst<AstIdentifier>(locNode->childs.back()->childs.back(), AstNodeKind::Identifier);
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
            const auto id = CastAst<AstIdentifier>(locNode->childs.back(), AstNodeKind::Identifier);
            if (id->identifierExtension && id->identifierExtension->fromAlternateVar)
            {
                locNode = id->identifierExtension->fromAlternateVar;
                done    = true;
                continue;
            }
        }

        if (locNode->ownerFct && locNode->ownerFct->typeInfo && locNode->ownerFct->requestedGeneric)
        {
            const auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(locNode->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
            for (const auto& it : typeFunc->replaceTypes)
            {
                bool fromGen = false;
                if (it.second.typeInfoReplace == locNode->typeInfo)
                    fromGen = true;
                else if (locNode->typeInfo->isPointer() && it.second.typeInfoReplace == ((TypeInfoPointer*) locNode->typeInfo)->pointedType)
                    fromGen = true;
                else if (locNode->typeInfo->isSlice() && it.second.typeInfoReplace == ((TypeInfoSlice*) locNode->typeInfo)->pointedType)
                    fromGen = true;
                else if (locNode->typeInfo->isArray() && it.second.typeInfoReplace == ((TypeInfoArray*) locNode->typeInfo)->finalType)
                    fromGen = true;

                if (fromGen && it.second.fromNode)
                {
                    locNode = it.second.fromNode;
                    done    = true;
                    continue;
                }
            }
        }

        break;
    }

    // If identifier is an inline param call replacement, take it
    if (locNode->resolvedSymbolOverload && !done)
        locNode = locNode->resolvedSymbolOverload->node;

    node->setFlagsValueIsComputed();
    ByteCodeGen::computeSourceLocation(context, locNode, &node->computedValue->storageOffset, &node->computedValue->storageSegment);
    SWAG_CHECK(setupIdentifierRef(context, node));
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveIntrinsicDefined(SemanticContext* context)
{
    const auto node = context->node;
    node->setFlagsValueIsComputed();
    node->computedValue->reg.b = node->childs.back()->resolvedSymbolOverload != nullptr;
    node->typeInfo             = g_TypeMgr->typeInfoBool;
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
    switch (node->tokenId)
    {
    case TokenId::CompilerBackend:
        node->setFlagsValueIsComputed();
        switch (g_CommandLine.backendGenType)
        {
        case BackendGenType::SCBE:
            node->computedValue->reg.u64 = (uint64_t) SwagBackendGenType::SCBE;
            break;
        case BackendGenType::LLVM:
            node->computedValue->reg.u64 = (uint64_t) SwagBackendGenType::LLVM;
            break;
        }
        node->typeInfo = g_Workspace->swagScope.regTypeInfoBackend;
        SWAG_ASSERT(node->typeInfo);
        return true;

    case TokenId::CompilerOs:
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = (uint64_t) g_CommandLine.target.os;
        node->typeInfo               = g_Workspace->swagScope.regTypeInfoTargetOs;
        SWAG_ASSERT(node->typeInfo);
        return true;
    case TokenId::CompilerArch:
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = (uint64_t) g_CommandLine.target.arch;
        node->typeInfo               = g_Workspace->swagScope.regTypeInfoTargetArch;
        SWAG_ASSERT(node->typeInfo);
        return true;
    case TokenId::CompilerCpu:
        node->setFlagsValueIsComputed();
        node->computedValue->text = Semantic::getCompilerFunctionString(node, node->tokenId);
        node->typeInfo            = g_TypeMgr->typeInfoString;
        return true;

    case TokenId::CompilerSwagOs:
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = (uint64_t) OS::getNativeTarget().os;
        node->typeInfo               = g_Workspace->swagScope.regTypeInfoTargetOs;
        SWAG_ASSERT(node->typeInfo);
        return true;

    case TokenId::CompilerBuildCfg:
        node->setFlagsValueIsComputed();
        node->computedValue->text = Semantic::getCompilerFunctionString(node, node->tokenId);
        node->typeInfo            = g_TypeMgr->typeInfoString;
        return true;

    case TokenId::CompilerLocation:
        node->typeInfo = g_TypeMgr->makeConst(g_Workspace->swagScope.regTypeInfoSourceLoc);
        node->setFlagsValueIsComputed();
        ByteCodeGen::computeSourceLocation(context, node, &node->computedValue->storageOffset, &node->computedValue->storageSegment);
        SWAG_CHECK(setupIdentifierRef(context, node));
        return true;

    case TokenId::CompilerCallerLocation:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, Err(Err0498)}));
        node->typeInfo = g_Workspace->swagScope.regTypeInfoSourceLoc;
        return true;

    case TokenId::CompilerCallerFunction:
        SWAG_VERIFY(node->parent->kind == AstNodeKind::FuncDeclParam, context->report({node, Err(Err0497)}));
        node->typeInfo = g_TypeMgr->typeInfoString;
        return true;

    default:
        return Report::internalError(context->node, "resolveCompilerFunction, unknown token");
    }
}
