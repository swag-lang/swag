#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Os/Os.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

void Semantic::allocateOnStack(AstNode* node, const TypeInfo* typeInfo)
{
    node->allocateComputedValue();
    node->computedValue()->storageOffset = node->ownerScope->startStackSize;
    node->ownerScope->startStackSize += typeInfo->isStruct() ? std::max(typeInfo->sizeOf, static_cast<uint32_t>(8)) : typeInfo->sizeOf;
    setOwnerMaxStackSize(node, node->ownerScope->startStackSize);
}

bool Semantic::setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, const AstNode* funcNode, AstNode* parameters, bool forGenerics)
{
    if (!parameters || funcNode->hasAttribute(ATTRIBUTE_MESSAGE_FUNC))
        return true;

    bool     defaultValueDone = false;
    uint32_t index            = 0;

    // If we have a tuple as a default parameter, without a user defined type, then we need to convert it to a tuple struct
    // and wait for the type to be solved.
    for (const auto param : parameters->children)
    {
        const auto nodeParam = castAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        if (!nodeParam->assignment)
            continue;
        if (nodeParam->assignment->isNot(AstNodeKind::ExpressionList))
            continue;

        const auto nodeExpr = castAst<AstExpressionList>(nodeParam->assignment, AstNodeKind::ExpressionList);
        if (!nodeExpr->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
            continue;

        if (!nodeParam->type)
        {
            nodeParam->addSemFlag(SEMFLAG_TUPLE_CONVERT);
            SWAG_ASSERT(nodeParam->typeInfo->isListTuple());
            SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, nodeParam, nodeParam->assignment, &nodeParam->type));
            context->result = ContextResult::NewChildren;
            context->baseJob->nodes.push_back(nodeParam->type);
            return true;
        }

        if (nodeParam->hasSemFlag(SEMFLAG_TUPLE_CONVERT))
        {
            SWAG_ASSERT(nodeParam->resolvedSymbolOverload());
            nodeParam->typeInfo                           = nodeParam->type->typeInfo;
            nodeParam->resolvedSymbolOverload()->typeInfo = nodeParam->typeInfo;
        }
    }

    if (forGenerics)
    {
        typeInfo->genericParameters.clear();
        typeInfo->genericParameters.reserve(parameters->childCount());
        typeInfo->addFlag(TYPEINFO_GENERIC);
    }
    else
    {
        typeInfo->parameters.clear();
        typeInfo->parameters.reserve(parameters->childCount());
    }

    const AstNode* firstParamWithDef = nullptr;
    auto           sourceFile        = context->sourceFile;
    for (const auto param : parameters->children)
    {
        auto nodeParam        = castAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        auto funcParam        = TypeManager::makeParam();
        funcParam->name       = param->token.text;
        funcParam->typeInfo   = param->typeInfo;
        funcParam->index      = index++;
        funcParam->declNode   = nodeParam;
        funcParam->attributes = nodeParam->attributes;
        if (nodeParam->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_TYPE))
            funcParam->flags.add(TYPEINFOPARAM_GENERIC_TYPE);
        else if (nodeParam->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_CONSTANT))
            funcParam->flags.add(TYPEINFOPARAM_GENERIC_CONSTANT);

        const auto paramType     = nodeParam->typeInfo;
        auto       paramNodeType = nodeParam->type ? nodeParam->type : nodeParam;

        // Code is only valid for a macro or mixin
        if (paramType->isCode())
            SWAG_VERIFY(funcNode->hasAttribute(ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN), context->report({paramNodeType, toErr(Err0410)}));

        // Not everything is possible for types for attributes
        if (param->ownerScope->is(ScopeKind::Attribute))
        {
            auto toTest = paramType;
            if (paramType->isTypedVariadic())
                toTest = castTypeInfo<TypeInfoVariadic>(paramType, TypeInfoKind::TypedVariadic)->rawType;

            if ((!toTest->isNative() || toTest->isAny()) &&
                !toTest->isEnum() &&
                !toTest->isPointerToTypeInfo())
            {
                Diagnostic err{nodeParam->type ? nodeParam->type : nodeParam, formErr(Err0256, toTest->getDisplayNameC())};
                err.addNote(toNte(Nte0098));
                return context->report(err);
            }
        }

        parameters->inheritAstFlagsOr(nodeParam->type, AST_GENERIC);

        // Variadic must be the last one
        if (paramType->isVariadic())
        {
            SWAG_VERIFY(!funcNode->hasAttribute(ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, toErr(Err0413)}));
            typeInfo->addFlag(TYPEINFO_VARIADIC);
            if (index != parameters->childCount())
                return context->report({nodeParam, toErr(Err0412)});
        }
        else if (paramType->isTypedVariadic())
        {
            SWAG_VERIFY(!funcNode->hasAttribute(ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, toErr(Err0413)}));
            typeInfo->addFlag(TYPEINFO_TYPED_VARIADIC);
            if (index != parameters->childCount())
                return context->report({nodeParam, toErr(Err0412)});
        }
        else if (paramType->isCVariadic())
        {
            SWAG_VERIFY(!funcNode->hasAttribute(ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, toErr(Err0413)}));
            typeInfo->addFlag(TYPEINFO_C_VARIADIC);
            if (index != parameters->childCount())
                return context->report({nodeParam, toErr(Err0412)});
        }

        // Default parameter value
        if (nodeParam->assignment)
        {
            if (!defaultValueDone)
            {
                defaultValueDone               = true;
                typeInfo->firstDefaultValueIdx = index - 1;
                firstParamWithDef              = nodeParam;
            }
        }
        else if (!nodeParam->typeInfo->isCode())
        {
            if (defaultValueDone)
            {
                Diagnostic err{nodeParam, formErr(Err0444, Naming::niceParameterRank(index).cstr())};
                err.addNote(firstParamWithDef, toNte(Nte0206));
                return context->report(err);
            }
        }

        if (forGenerics)
            typeInfo->genericParameters.push_back(funcParam);
        else
            typeInfo->parameters.push_back(funcParam);
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveFuncDeclParams(SemanticContext* context)
{
    const auto node = context->node;
    node->inheritAstFlagsOr(AST_GENERIC);
    node->byteCodeFct = ByteCodeGen::emitFuncDeclParams;
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::sendCompilerMsgFuncDecl(SemanticContext* context)
{
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;

    // Filter what we send
    if (module->is(ModuleKind::BootStrap) || module->is(ModuleKind::Runtime))
        return true;
    if (sourceFile->imported && !sourceFile->hasFlag(FILE_EMBEDDED))
        return true;
    if (!context->node->ownerScope->isGlobalOrImpl())
        return true;
    if (context->node->hasAttribute(ATTRIBUTE_GENERATED_FUNC))
        return true;
    if (context->node->hasAstFlag(AST_GENERIC | AST_FROM_GENERIC))
        return true;

    const auto funcNode = castAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

    CompilerMessage msg;
    msg.concrete.kind        = CompilerMsgKind::SemFunctions;
    msg.concrete.name.buffer = funcNode->token.text.buffer;
    msg.concrete.name.count  = funcNode->token.text.length();
    msg.typeInfo             = typeInfo;
    SWAG_CHECK(module->postCompilerMessage(msg));

    return true;
}

bool Semantic::resolveFuncDeclAfterSI(SemanticContext* context)
{
    const auto saveNode = context->node;
    if (context->node->parent->is(AstNodeKind::Inline))
    {
        const auto node = castAst<AstInline>(context->node->parent, AstNodeKind::Inline);
        context->node   = node->func;
    }
    else
    {
        const auto node = castAst<AstFuncDecl>(context->node->parent, AstNodeKind::FuncDecl);
        SWAG_ASSERT(node->content == context->node);
        context->node = node;
    }

    resolveFuncDecl(context);
    resolveScopedStmtAfter(context);
    context->node = saveNode;
    return true;
}

bool Semantic::preResolveFuncDecl(SemanticContext* context)
{
    const auto funcNode = castAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

    {
        ScopedLock lkT(typeInfo->mutex);
        SWAG_CHECK(collectAttributes(context, funcNode, &typeInfo->attributes));
    }

    {
        ScopedLock lk(funcNode->mutex);
        funcNode->addSemFlag(SEMFLAG_PRE_RESOLVE);
        funcNode->dependentJobs.setRunning();
    }

    return true;
}

bool Semantic::resolveFuncDecl(SemanticContext* context)
{
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;
    auto       funcNode   = castAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);

    // Only one main per module!
    if (funcNode->hasAttribute(ATTRIBUTE_MAIN_FUNC))
    {
        ScopedLock lk(sourceFile->module->mutexFile);
        if (sourceFile->module->mainIsDefined)
        {
            Diagnostic err{funcNode, toErr(Err0006)};
            err.addNote(module->mainIsDefined, module->mainIsDefined->getTokenName(), toNte(Nte0194));
            return context->report(err);
        }

        sourceFile->module->mainIsDefined = funcNode;
    }

    // No semantic on a generic function, or a macro
    if (funcNode->hasAstFlag(AST_GENERIC))
    {
        if (funcNode->hasAttribute(ATTRIBUTE_PUBLIC) && !funcNode->hasAstFlag(AST_GENERATED))
            funcNode->ownerScope->addPublicNode(funcNode);
        return true;
    }

    // Check that there is no 'hole' in alias names
    if (funcNode->aliasMask && funcNode->hasAttribute(ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN))
    {
        auto       mask = funcNode->aliasMask;
        const auto maxN = OS::bitCountLz(funcNode->aliasMask);
        for (uint32_t n = 0; n < 32 - maxN; n++)
        {
            if ((mask & 1) == 0)
                return context->report({funcNode, funcNode->tokenName, formErr(Err0482, funcNode->token.cstr(), n)});
            mask >>= 1;
        }
    }

    if (funcNode->hasAttribute(ATTRIBUTE_MACRO))
    {
        if (funcNode->hasAttribute(ATTRIBUTE_PUBLIC) && !funcNode->hasAstFlag(AST_GENERATED) && !funcNode->hasAstFlag(AST_FROM_GENERIC))
            funcNode->ownerScope->addPublicNode(funcNode);
        SWAG_CHECK(setFullResolve(context, funcNode));
        return true;
    }

    // An inline function will not have bytecode, so need to register public by hand now
    if (funcNode->hasAttribute(ATTRIBUTE_PUBLIC) && funcNode->hasAttribute(ATTRIBUTE_INLINE) && !funcNode->hasAstFlag(AST_FROM_GENERIC))
        funcNode->ownerScope->addPublicNode(funcNode);
    else if (funcNode->hasAttribute(ATTRIBUTE_PUBLIC) && funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
        SWAG_VERIFY(funcNode->returnType->hasSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED), context->report({funcNode, funcNode->getTokenName(), toErr(Err0468)}));

    funcNode->byteCodeFct = ByteCodeGen::emitLocalFuncDecl;

    // Check attributes
    if (funcNode->isForeign() && funcNode->content)
    {
        Diagnostic err{funcNode, funcNode->getTokenName(), toErr(Err0634)};
        err.addNote(funcNode->content, funcNode->content->token, toNte(Nte0201));
        const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_Foreign);
        err.addNote(attr, formNte(Nte0181, "attribute"));
        return context->report(err);
    }

    if (!funcNode->hasAttribute(ATTRIBUTE_GENERATED_FUNC))
    {
        if (funcNode->hasAttribute(ATTRIBUTE_TEST_FUNC))
        {
            SWAG_VERIFY(module->is(ModuleKind::Test), context->report({funcNode, toErr(Err0754)}));
        }
    }

    // Can be null for intrinsics etc...
    if (funcNode->content)
        funcNode->content->setBcNotifyBefore(ByteCodeGen::emitBeforeFuncDeclContent);

    // Do we have a return value
    if (funcNode->content && funcNode->returnType && !funcNode->returnType->typeInfo->isVoid())
    {
        if (!funcNode->content->hasAstFlag(AST_NO_SEMANTIC))
        {
            if (!funcNode->hasSemFlag(SEMFLAG_SCOPE_HAS_RETURN))
            {
                if (funcNode->hasSemFlag(SEMFLAG_FCT_HAS_RETURN))
                    return context->report({funcNode->returnType, formErr(Err0475, funcNode->getDisplayNameC())});
                return context->report({funcNode->returnType, formErr(Err0472, funcNode->getDisplayNameC(), funcNode->returnType->typeInfo->getDisplayNameC())});
            }
        }
    }

    // Content semantic can have been disabled (where). In that case, we're not done yet, so
    // do not set the FULL_RESOLVE flag and do not generate bytecode
    if (funcNode->content && funcNode->content->hasAstFlag(AST_NO_SEMANTIC))
    {
        ScopedLock lk(funcNode->funcMutex);
        funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_PARTIAL_RESOLVE);
        funcNode->dependentJobs.setRunning();
        return true;
    }

    if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL))
    {
        bool implFor = false;
        if (funcNode->ownerScope && funcNode->ownerScope->is(ScopeKind::Impl))
        {
            const auto implNode = castAst<AstImpl>(funcNode->ownerScope->owner, AstNodeKind::Impl);
            if (implNode->identifierFor)
            {
                const auto forId = implNode->identifier->lastChild();
                implFor          = true;

                // Be sure interface has been fully solved
                {
                    ScopedLock lk(forId->mutex);
                    ScopedLock lk1(forId->resolvedSymbolName()->mutex);
                    if (forId->resolvedSymbolName()->cptOverloads)
                    {
                        waitSymbolNoLock(context->baseJob, forId->resolvedSymbolName());
                        return true;
                    }
                }

                {
                    const auto typeBaseInterface = castTypeInfo<TypeInfoStruct>(forId->resolvedSymbolOverload()->typeInfo, TypeInfoKind::Interface);
                    const auto typeInterface     = castTypeInfo<TypeInfoStruct>(typeBaseInterface->itable, TypeInfoKind::Struct);
                    ScopedLock lk(typeInterface->mutex);

                    // We need to search the function (as a variable) in the interface
                    // If not found, then this is a normal function...
                    const auto symbolName = typeInterface->findChildByNameNoLock(funcNode->token.text); // O(n)!
                    if (symbolName)
                    {
                        funcNode->fromItfSymbol = symbolName;
                    }
                }
            }
        }

        // Be sure 'impl' is justified
        SWAG_VERIFY(implFor, context->report({funcNode->token.sourceFile, funcNode->implLoc, funcNode->implLoc, toErr(Err0331)}));
    }

    // Warnings
    SWAG_CHECK(SemanticError::warnUnusedVariables(context, funcNode->scope));

    // Now the full function has been solved, so we wakeup jobs depending on that
    SWAG_CHECK(setFullResolve(context, funcNode));
    
    // Ask for bytecode
    bool genByteCode = true;
    if (funcNode->hasAttribute(ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
        genByteCode = false;
    if (funcNode->hasIntrinsicName() && !funcNode->hasAstFlag(AST_DEFINED_INTRINSIC))
        genByteCode = false;
    if (funcNode->isForeign())
        genByteCode = false;
    if (funcNode->hasAstFlag(AST_GENERIC))
        genByteCode = false;
    if (!funcNode->hasIntrinsicName() && funcNode->hasAttribute(ATTRIBUTE_INLINE))
        genByteCode = false;
    if (!funcNode->content)
        genByteCode = false;
    if (genByteCode)
        SWAG_CHECK(ByteCodeGen::askForByteCode(context, funcNode, ASK_BC_ZERO, true, nullptr));

    return true;
}

bool Semantic::setFullResolve(SemanticContext*, AstFuncDecl* funcNode)
{
    ScopedLock lk(funcNode->funcMutex);
    SharedLock lk1(funcNode->mutex);
    computeAccess(funcNode);
    funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_FULL_RESOLVE | AstFuncDecl::SPEC_FLAG_PARTIAL_RESOLVE);
    funcNode->dependentJobs.setRunning();
    return true;
}

void Semantic::setFuncDeclParamsIndex(const AstFuncDecl* funcNode)
{
    if (funcNode->parameters)
    {
        uint32_t storageIndex = 0;
        if (funcNode->typeInfo->hasFlag(TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
        {
            const auto param       = funcNode->parameters->lastChild();
            const auto resolved    = param->resolvedSymbolOverload();
            resolved->storageIndex = 0; // Always the first one
            storageIndex += 2;
        }

        const auto childSize = funcNode->parameters->childCount();
        for (uint32_t i = 0; i < childSize; i++)
        {
            if (i == childSize - 1 && funcNode->typeInfo->hasFlag(TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
                break;
            const auto param       = funcNode->parameters->children[i];
            const auto resolved    = param->resolvedSymbolOverload();
            resolved->storageIndex = storageIndex;

            const auto typeParam    = TypeManager::concreteType(resolved->typeInfo);
            const auto numRegisters = typeParam->numRegisters();
            storageIndex += numRegisters;
        }
    }
}

bool Semantic::resolveFuncDeclType(SemanticContext* context)
{
    const auto typeNode = context->node;
    auto       funcNode = castAst<AstFuncDecl>(typeNode->parent, AstNodeKind::FuncDecl);

    // This is a lambda that was waiting for a match.
    // We are now awake, so everything has been done already
    if (funcNode->pendingLambdaJob)
    {
        setFuncDeclParamsIndex(funcNode);
        funcNode->pendingLambdaJob = nullptr;

        ScopedLock lk(funcNode->resolvedSymbolName()->mutex);

        // We were not a short lambda, so just wakeup our dependencies
        if (!funcNode->resolvedSymbolOverload()->hasFlag(OVERLOAD_UNDEFINED))
        {
            funcNode->resolvedSymbolName()->dependentJobs.setRunning();
        }

        // We were a short lambda, and the return type is now valid
        // So we just wake up our dependencies, by decreasing the count
        // (because the registration was the same as an incomplete one)
        else if (!funcNode->returnType->typeInfo->isGeneric())
        {
            funcNode->resolvedSymbolOverload()->flags.remove(OVERLOAD_UNDEFINED);
            funcNode->resolvedSymbolName()->decreaseOverloadNoLock();
        }

        // Return type is generic. We must evaluate the content to deduce it, so we
        // pass to short lambda mode again, so that the return evaluation will update
        // the type.
        else
        {
            funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA);
        }

        return true;
    }

    // If this is a #message function, we must have a flag mask as parameters
    if (funcNode->hasAttribute(ATTRIBUTE_MESSAGE_FUNC) && funcNode->parameters)
    {
        auto       parameters = funcNode->parameters;
        const auto paramType  = TypeManager::concreteType(parameters->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
        SWAG_VERIFY(paramType->isEnum(), context->report({parameters, formErr(Err0538, paramType->getDisplayNameC())}));
        paramType->computeScopedName();
        SWAG_VERIFY(paramType->scopedName == g_LangSpec->name_Swag_CompilerMsgMask, context->report({parameters, formErr(Err0538, paramType->getDisplayNameC())}));
        SWAG_CHECK(evaluateConstExpression(context, parameters));
        YIELD();
        funcNode->parameters->addAstFlag(AST_NO_BYTECODE);
    }

    // Return type
    if (!typeNode->children.empty())
    {
        const auto front    = typeNode->firstChild();
        typeNode->typeInfo  = front->typeInfo;
        const auto concrete = typeNode->typeInfo->getConcreteAlias();
        if (concrete->isVoid())
        {
            Diagnostic err{typeNode->token.sourceFile, typeNode->token.startLocation, front->token.endLocation, toErr(Err0232)};
            if (typeNode->typeInfo->isAlias())
                err.addNote(Diagnostic::note(typeNode, Diagnostic::isType(concrete)));
            return context->report(err);
        }
    }
    else
    {
        typeNode->typeInfo = g_TypeMgr->typeInfoVoid;

        // @DeduceLambdaType
        if (funcNode->makePointerLambda &&
            funcNode->makePointerLambda->hasSpecFlag(AstMakePointer::SPEC_FLAG_DEP_TYPE) &&
            !funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA))
        {
            const auto deducedType = getDeducedLambdaType(context, funcNode->makePointerLambda);
            if (deducedType->isLambdaClosure())
            {
                const auto typeFct = castTypeInfo<TypeInfoFuncAttr>(deducedType, TypeInfoKind::LambdaClosure);
                typeNode->typeInfo = typeFct->returnType;
            }
        }
    }

    // Treat function attributes
    const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    ScopedLock lkT(typeInfo->mutex);
    SWAG_ASSERT(funcNode->semanticState == AstNodeResolveState::ProcessingChildren);

    // Check attributes
    if (funcNode->hasAttribute(ATTRIBUTE_CONSTEXPR))
        funcNode->addAstFlag(AST_CONST_EXPR);
    if (funcNode->ownerFct)
        funcNode->inheritAttribute(funcNode->ownerFct, ATTRIBUTE_COMPILER);

    if (!funcNode->hasAstFlag(AST_FROM_GENERIC) && !funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_CHECK_ATTR))
    {
        // Can be called multiple times in case of a mixin/macro inside another inlined function
        funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_CHECK_ATTR);

        if (funcNode->hasAttribute(ATTRIBUTE_MACRO))
            funcNode->addAttribute(ATTRIBUTE_INLINE);
        if (funcNode->hasAttribute(ATTRIBUTE_MIXIN))
            funcNode->addAttribute(ATTRIBUTE_INLINE | ATTRIBUTE_MACRO);

        if (funcNode->hasAttribute(ATTRIBUTE_SHARP_FUNC))
        {
            if (funcNode->hasAttribute(ATTRIBUTE_MIXIN))
            {
                Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0373, funcNode->getDisplayNameC())};
                const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_Mixin);
                err.addNote(attr, formNte(Nte0181, "attribute"));
                return context->report(err);
            }

            if (funcNode->hasAttribute(ATTRIBUTE_MACRO))
            {
                Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0372, funcNode->getDisplayNameC())};
                const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_Macro);
                err.addNote(attr, formNte(Nte0181, "attribute"));
                return context->report(err);
            }

            if (funcNode->hasAttribute(ATTRIBUTE_INLINE))
            {
                Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0371, funcNode->getDisplayNameC())};
                const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_Inline);
                err.addNote(attr, formNte(Nte0181, "attribute"));
                return context->report(err);
            }

            if (funcNode->hasAttribute(ATTRIBUTE_NOT_GENERIC))
            {
                Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0374, funcNode->getDisplayNameC())};
                const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_NotGeneric);
                err.addNote(attr, formNte(Nte0181, "attribute"));
                return context->report(err);
            }

            if (funcNode->hasAttribute(ATTRIBUTE_CALLEE_RETURN))
            {
                Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0364, funcNode->getDisplayNameC())};
                const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_CalleeReturn);
                err.addNote(attr, formNte(Nte0181, "attribute"));
                return context->report(err);
            }
        }
    }

    if (funcNode->hasAttribute(ATTRIBUTE_COMPLETE) &&
        funcNode->token.text != g_LangSpec->name_opAffect &&
        funcNode->token.text != g_LangSpec->name_opAffectLiteral)
    {
        Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0367, funcNode->token.cstr())};
        const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_Complete);
        err.addNote(attr, formNte(Nte0181, "attribute"));
        return context->report(err);
    }

    if (funcNode->hasAttribute(ATTRIBUTE_IMPLICIT) &&
        funcNode->token.text != g_LangSpec->name_opAffect &&
        funcNode->token.text != g_LangSpec->name_opAffectLiteral &&
        funcNode->token.text != g_LangSpec->name_opCast)
    {
        Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0370, funcNode->token.cstr())};
        const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_Implicit);
        err.addNote(attr, formNte(Nte0181, "attribute"));
        context->report(err);
    }

    if (funcNode->hasAttribute(ATTRIBUTE_CALLEE_RETURN) && !funcNode->hasAttribute(ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
    {
        Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0365, funcNode->token.cstr())};
        const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_CalleeReturn);
        err.addNote(attr, formNte(Nte0181, "attribute"));
        return context->report(err);
    }

    // Implicit attribute cannot be used on a generic function
    // This is because "extra" generic parameters must be specified and not deduced, and this is not possible for an implicit cast
    if (funcNode->hasAttribute(ATTRIBUTE_IMPLICIT) && funcNode->hasAstFlag(AST_GENERIC | AST_FROM_GENERIC))
    {
        bool ok = false;
        if (funcNode->token.is(g_LangSpec->name_opAffectLiteral) && funcNode->genericParameters->childCount() <= 1)
            ok = true;
        if (funcNode->token.is(g_LangSpec->name_opAffect) && !funcNode->genericParameters)
            ok = true;
        if (!ok)
        {
            Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0369, funcNode->getDisplayNameC())};
            const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_Implicit);
            err.addNote(attr, formNte(Nte0181, "attribute"));
            return context->report(err);
        }
    }

    if (!funcNode->hasAstFlag(AST_FROM_GENERIC))
    {
        // Determine if function is generic
        if (funcNode->ownerStructScope && funcNode->ownerStructScope->owner->hasAstFlag(AST_GENERIC) && !funcNode->hasAttribute(ATTRIBUTE_NOT_GENERIC))
            funcNode->addAstFlag(AST_GENERIC);
        if (funcNode->ownerFct && funcNode->ownerFct->hasAstFlag(AST_GENERIC) && !funcNode->hasAttribute(ATTRIBUTE_NOT_GENERIC))
            funcNode->addAstFlag(AST_GENERIC);

        if (funcNode->parameters)
            funcNode->inheritAstFlagsOr(funcNode->parameters, AST_GENERIC);

        if (funcNode->genericParameters)
        {
            if (funcNode->hasAttribute(ATTRIBUTE_NOT_GENERIC))
            {
                Diagnostic err{funcNode->genericParameters, formErr(Err0641, funcNode->token.cstr())};
                const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_NotGeneric);
                err.addNote(attr, formNte(Nte0181, "attribute"));
                return context->report(err);
            }

            funcNode->addAstFlag(AST_GENERIC);
        }

        if (funcNode->hasAstFlag(AST_GENERIC))
            typeInfo->addFlag(TYPEINFO_GENERIC);

        if (funcNode->hasAttribute(ATTRIBUTE_NOT_GENERIC) && funcNode->hasAstFlag(AST_GENERIC))
        {
            Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0639, funcNode->token.cstr())};
            const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_NotGeneric);
            err.addNote(attr, formNte(Nte0181, "attribute"));
            return context->report(err);
        }

        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->genericParameters, true));
        YIELD();
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->parameters, false));
        YIELD();
    }
    else
    {
        for (const auto t : typeInfo->parameters)
        {
            if (t->typeInfo->isGeneric())
            {
                t->typeInfo = funcNode->parameters->children[t->index]->typeInfo;
            }
        }
    }

    // If a lambda function will wait for a match, then no need to deduce the return type
    // It will be done in the same way as parameters
    bool shortLambdaPendingTyping = false;
    if (!funcNode->hasAstFlag(AST_GENERIC))
    {
        if (funcNode->hasSemFlag(SEMFLAG_PENDING_LAMBDA_TYPING) && typeNode->typeInfo->isVoid())
        {
            shortLambdaPendingTyping = funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA);
            typeNode->typeInfo       = g_TypeMgr->typeInfoUndefined;
            funcNode->removeSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA);
        }
    }

    // Short lambda without a return type we must deduced
    // In that case, symbol registration will not be done at the end of that function but once the return expression
    // has been evaluated, and the type deduced
    bool shortLambda = false;
    if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA) && !funcNode->returnType->hasSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED))
        shortLambda = true;

    // @RunGeneratedExp
    bool mustDeduceReturnType = false;
    if (funcNode->hasAttribute(ATTRIBUTE_RUN_GENERATED_EXP))
        mustDeduceReturnType = true;

    // No semantic on content if function is generic
    if (funcNode->hasAstFlag(AST_GENERIC))
    {
        shortLambda = false;
        if (funcNode->content)
            funcNode->content->addAstFlag(AST_NO_SEMANTIC);
    }

    // Macro will not evaluate its content before being inline
    if (funcNode->hasAttribute(ATTRIBUTE_MACRO) && !shortLambda && funcNode->content)
        funcNode->content->addAstFlag(AST_NO_SEMANTIC);

    // Register symbol
    typeInfo->returnType = typeNode->typeInfo;

    // Be sure this is a valid return type
    if (!typeInfo->returnType->isNative() &&
        !typeInfo->returnType->isListTuple() &&
        !typeInfo->returnType->isListArray() &&
        !typeInfo->returnType->isStruct() &&
        !typeInfo->returnType->isGeneric() &&
        !typeInfo->returnType->isAlias() &&
        !typeInfo->returnType->isLambdaClosure() &&
        !typeInfo->returnType->isSlice() &&
        !typeInfo->returnType->isEnum() &&
        !typeInfo->returnType->isInterface() &&
        !typeInfo->returnType->isArray() &&
        !typeInfo->returnType->isPointer())
    {
        return context->report({typeNode->firstChild(), formErr(Err0745, typeInfo->returnType->getDisplayNameC())});
    }

    typeInfo->name.clear();
    typeInfo->displayName.clear();
    typeInfo->computeWhateverNameNoLock(ComputeNameKind::Name);

    // Special functions registration
    if (funcNode->parameters && funcNode->parameters->childCount() == 1)
    {
        if (funcNode->token.is(g_LangSpec->name_opInit))
        {
            const auto typePointer = castTypeInfo<TypeInfoPointer>(funcNode->parameters->firstChild()->typeInfo, TypeInfoKind::Pointer);
            const auto typeStruct  = castTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserInitFct = funcNode;
            funcNode->addSemFlag(SEMFLAG_NO_PUBLIC);
        }
        else if (funcNode->token.is(g_LangSpec->name_opDrop))
        {
            const auto typePointer = castTypeInfo<TypeInfoPointer>(funcNode->parameters->firstChild()->typeInfo, TypeInfoKind::Pointer);
            const auto typeStruct  = castTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserDropFct = funcNode;
            funcNode->addSemFlag(SEMFLAG_NO_PUBLIC);
            SWAG_VERIFY(!typeStruct->declNode->hasAttribute(ATTRIBUTE_CONSTEXPR), context->report({funcNode, funcNode->tokenName, formErr(Err0084, typeStruct->getDisplayNameC())}));
        }
        else if (funcNode->token.is(g_LangSpec->name_opPostCopy))
        {
            const auto typePointer = castTypeInfo<TypeInfoPointer>(funcNode->parameters->firstChild()->typeInfo, TypeInfoKind::Pointer);
            const auto typeStruct  = castTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserPostCopyFct = funcNode;
            funcNode->addSemFlag(SEMFLAG_NO_PUBLIC);
            SWAG_VERIFY(!typeStruct->hasFlag(TYPEINFO_STRUCT_NO_COPY), context->report({funcNode, funcNode->tokenName, formErr(Err0085, typeStruct->getDisplayNameC())}));
        }
        else if (funcNode->token.is(g_LangSpec->name_opPostMove))
        {
            const auto typePointer = castTypeInfo<TypeInfoPointer>(funcNode->parameters->firstChild()->typeInfo, TypeInfoKind::Pointer);
            const auto typeStruct  = castTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserPostMoveFct = funcNode;
            funcNode->addSemFlag(SEMFLAG_NO_PUBLIC);
        }
    }

    // If this is a lambda waiting for a match to know the types of its parameters, need to wait
    // Function Semantic::setSymbolMatch will wake us up as soon as a valid match is found
    if (funcNode->hasSemFlag(SEMFLAG_PENDING_LAMBDA_TYPING))
    {
        if (!funcNode->hasAstFlag(AST_GENERIC))
        {
            funcNode->pendingLambdaJob = context->baseJob;
            context->baseJob->setPending(JobWaitKind::PendingLambdaTyping, nullptr, funcNode, nullptr);
        }
    }

    // Set storageIndex of each parameter
    setFuncDeclParamsIndex(funcNode);

    // To avoid ambiguity, we do not want a function to declare a generic type 'T' if the struct
    // has the same generic parameter name (this is useless and implicit)
    if (funcNode->genericParameters && funcNode->ownerStructScope && funcNode->ownerStructScope->owner->is(AstNodeKind::StructDecl))
    {
        const auto structDecl = castAst<AstStruct>(funcNode->ownerStructScope->owner, AstNodeKind::StructDecl);
        if (structDecl->typeInfo->isGeneric())
        {
            for (const auto c : funcNode->genericParameters->children)
            {
                if (!c->resolvedSymbolOverload())
                    continue;

                for (const auto sc : structDecl->genericParameters->children)
                {
                    if (!sc->resolvedSymbolOverload())
                        continue;

                    if (c->resolvedSymbolOverload()->node->token.is(sc->resolvedSymbolOverload()->node->token.text))
                    {
                        Diagnostic err{c, formErr(Err0012, c->resolvedSymbolOverload()->node->token.cstr())};
                        err.addNote(sc->resolvedSymbolOverload()->node, toNte(Nte0196));
                        return context->report(err);
                    }
                }
            }
        }
    }

    // Do we have capture parameters? If it's the case, then we need to register all symbols as variables in the function scope
    if (funcNode->captureParameters)
    {
        uint32_t storageOffset = 0;
        for (const auto c : funcNode->captureParameters->children)
        {
            Utf8 name = c->token.text;
            if (c->is(AstNodeKind::MakePointer))
                name = c->firstChild()->token.text;

            AddSymbolTypeInfo toAdd;
            toAdd.node      = c;
            toAdd.typeInfo  = c->typeInfo;
            toAdd.kind      = SymbolKind::Variable;
            toAdd.flags     = OVERLOAD_VAR_CAPTURE;
            toAdd.aliasName = name;

            const auto overload = funcNode->scope->symTable.addSymbolTypeInfo(context, toAdd);
            if (!overload)
                return false;
            c->setResolvedSymbolOverload(overload);
            overload->computedValue.storageOffset = storageOffset;
            storageOffset += overload->typeInfo->sizeOf;
        }
    }

    // Register runtime libc function type, by name
    if (funcNode->token.sourceFile && funcNode->token.sourceFile->hasFlag(FILE_RUNTIME) && funcNode->isEmptyFct())
    {
        ScopedLock lk(funcNode->token.sourceFile->module->mutexFile);
        funcNode->token.sourceFile->module->mapRuntimeFctTypes[funcNode->token.text] = typeInfo;
    }

    // We should never reference an empty function
    // So consider this is a placeholder. This will generate an error in case the empty function is not replaced by a
    // real function at some point.
    if (funcNode->hasAttribute(ATTRIBUTE_PLACEHOLDER))
    {
        const auto symbolName = funcNode->resolvedSymbolName();
        ScopedLock lk(symbolName->mutex);

        // We need to be sure that we only have empty functions, and not a real one.
        // As we can have multiple times the same empty function prototype, count them.
        size_t cptEmpty = 0;
        for (const auto n : symbolName->nodes)
        {
            if (!n->isEmptyFct())
                break;
            cptEmpty++;
        }

        if (cptEmpty == symbolName->nodes.size() && symbolName->cptOverloads == 1)
        {
            symbolName->kind = SymbolKind::PlaceHolder;
            return true;
        }
    }

    // For a short lambda without a specified return type, we need to defer the symbol registration, as we
    // need to infer it from the lambda expression
    OverloadFlags overFlags = 0;
    if (shortLambda || mustDeduceReturnType)
        overFlags.add(OVERLOAD_INCOMPLETE);
    if (shortLambdaPendingTyping)
        overFlags.add(OVERLOAD_UNDEFINED);
    SWAG_CHECK(registerFuncSymbol(context, funcNode, overFlags));

    return true;
}

bool Semantic::registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode, OverloadFlags overFlags)
{
    if (!overFlags.has(OVERLOAD_INCOMPLETE))
    {
        SWAG_CHECK(checkFuncPrototype(context, funcNode));

        // The function wants to return something, but has the 'Swag.CalleeReturn' attribute
        if (!funcNode->returnType->typeInfo->isVoid() && funcNode->hasAttribute(ATTRIBUTE_CALLEE_RETURN))
        {
            Diagnostic err{funcNode->returnType->firstChild(), toErr(Err0653)};
            const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_CalleeReturn);
            err.addNote(attr, formNte(Nte0181, "attribute"));
            return context->report(err);
        }

        // The function returns nothing but has the 'Swag.Discardable' attribute
        if (funcNode->returnType->typeInfo->isVoid() && funcNode->hasAttribute(ATTRIBUTE_DISCARDABLE))
        {
            Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0471, funcNode->token.cstr())};
            const auto attr = funcNode->findParentAttrUse(g_LangSpec->name_Swag_Discardable);
            err.addNote(attr, formNte(Nte0181, "attribute"));
            return context->report(err);
        }
    }

    if (funcNode->hasAstFlag(AST_GENERIC))
        overFlags.add(OVERLOAD_GENERIC);

    AddSymbolTypeInfo toAdd;
    toAdd.node     = funcNode;
    toAdd.typeInfo = funcNode->typeInfo;
    toAdd.kind     = SymbolKind::Function;
    toAdd.flags    = overFlags;

    funcNode->setResolvedSymbol(toAdd.symbolName, funcNode->ownerScope->symTable.addSymbolTypeInfo(context, toAdd));
    SWAG_CHECK(funcNode->resolvedSymbolOverload());

    // Be sure an overloaded function has the attribute
    if (!funcNode->hasAstFlag(AST_FROM_GENERIC))
    {
        SharedLock lk(funcNode->ownerScope->symTable.mutex);
        const auto symbolName = funcNode->resolvedSymbolName();
        if (symbolName->overloads.size() > 1 && !funcNode->hasAttribute(ATTRIBUTE_OVERLOAD))
        {
            AstFuncDecl* other = nullptr;
            for (const auto n : symbolName->nodes)
            {
                if (n != funcNode && n->is(AstNodeKind::FuncDecl))
                {
                    if (!n->hasAstFlag(AST_FROM_GENERIC))
                    {
                        other = castAst<AstFuncDecl>(n, AstNodeKind::FuncDecl);
                    }
                }
            }

            if (other)
            {
                const Diagnostic err{funcNode, funcNode->tokenName, formErr(Err0635, funcNode->token.cstr())};
                return context->report(err, Diagnostic::hereIs(other));
            }
        }
    }

    // If the function returns a struct, register a type alias "retval". This way we can resolve an identifier
    // named retval for "var result: retval{xx, xxx}" syntax
    const auto returnType = funcNode->returnType->typeInfo->getConcreteAlias();
    if (returnType->isStruct())
    {
        const Utf8        retVal = g_LangSpec->name_retval;
        AddSymbolTypeInfo toAdd1;
        toAdd1.node      = funcNode->returnType;
        toAdd1.typeInfo  = returnType;
        toAdd1.kind      = SymbolKind::TypeAlias;
        toAdd1.flags     = overFlags | OVERLOAD_RETVAL;
        toAdd1.aliasName = retVal;
        funcNode->scope->symTable.addSymbolTypeInfo(context, toAdd1);
    }

    // Register method
    if (!overFlags.has(OVERLOAD_INCOMPLETE) && isMethod(funcNode))
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(funcNode->ownerStructScope->owner->typeInfo, TypeInfoKind::Struct);
        const auto typeFunc   = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
        SWAG_ASSERT(funcNode->methodParam);

        if (!typeFunc->attributes.empty())
        {
            ScopedLock lk(typeStruct->mutex);
            funcNode->methodParam->attributes = typeFunc->attributes;
        }

        decreaseMethodCount(funcNode, typeStruct);
    }

    resolveSubDecls(context, funcNode);
    return true;
}

bool Semantic::isMethod(const AstFuncDecl* funcNode)
{
    if (funcNode->ownerStructScope &&
        funcNode->parent->isNot(AstNodeKind::CompilerAst) &&
        funcNode->parent->isNot(AstNodeKind::CompilerRun) &&
        funcNode->parent->isNot(AstNodeKind::CompilerRunExpression) &&
        funcNode->parent->isNot(AstNodeKind::WhereConstraint) &&
        funcNode->parent->isNot(AstNodeKind::VerifyConstraint) &&
        !funcNode->hasAstFlag(AST_FROM_GENERIC) &&
        !funcNode->hasAttribute(ATTRIBUTE_SHARP_FUNC) &&
        funcNode->ownerScope->is(ScopeKind::Struct) &&
        funcNode->ownerStructScope->owner->typeInfo->isStruct())
    {
        return true;
    }

    return false;
}

void Semantic::launchResolveSubDecl(const JobContext* context, AstNode* node)
{
    if (node->hasAstFlag(AST_SPEC_SEMANTIC1 | AST_SPEC_SEMANTIC2 | AST_SPEC_SEMANTIC3))
        return;

    // If SEMFLAG_FILE_JOB_PASS is set, then the file job has already seen the sub declaration, ignored it
    // because of AST_NO_SEMANTIC, but the attribute context is ok. So we need to trigger the job by hand.
    // If SEMFLAG_FILE_JOB_PASS is not set, then we just have to remove the AST_NO_SEMANTIC flag, and the
    // file job will trigger the resolve itself
    node->removeAstFlag(AST_NO_SEMANTIC);
    if (node->hasSemFlag(SEMFLAG_FILE_JOB_PASS))
    {
        SemanticJob::newJob(context->baseJob->dependentJob, context->sourceFile, node, true);
    }
}

void Semantic::resolveSubDecls(const JobContext* context, AstFuncDecl* funcNode)
{
    if (!funcNode)
        return;

    // If we have sub declarations, then now we can solve them, except for a generic function.
    // Because for a generic function, the sub declarations will be cloned and solved after instantiation.
    // Otherwise, we can have a race condition by solving a generic sub declaration and cloning it for instantiation
    // at the same time.
    if (!funcNode->hasAstFlag(AST_GENERIC) && funcNode->content && !funcNode->content->hasAstFlag(AST_NO_SEMANTIC))
    {
        for (auto f : funcNode->subDecl)
        {
            ScopedLock lk(f->mutex);

            // Disabled by #if block
            if (f->hasSemFlag(SEMFLAG_DISABLED))
                continue;
            f->addSemFlag(SEMFLAG_DISABLED); // To avoid multiple resolutions

            if (f->hasOwnerCompilerIfBlock() && f->ownerCompilerIfBlock()->ownerFct == funcNode)
            {
                ScopedLock lk1(f->ownerCompilerIfBlock()->mutex);
                f->ownerCompilerIfBlock()->subDecl.push_back(f);
            }
            else
                launchResolveSubDecl(context, f);
        }
    }
}

bool Semantic::resolveCaptureFuncCallParams(SemanticContext* context)
{
    const auto node = castAst<AstFuncCallParams>(context->node, AstNodeKind::FuncCallParams);
    node->inheritAstFlagsOr(AST_GENERIC);
    node->inheritAstFlagsAnd(AST_CONST_EXPR);

    // Check capture types
    for (auto c : node->children)
    {
        auto typeField = c->typeInfo;

        if (typeField->isArray())
        {
            const auto typeArray = castTypeInfo<TypeInfoArray>(typeField, TypeInfoKind::Array);
            typeField            = typeArray->finalType;
        }

        if (typeField->isPointer())
            continue;
        if (typeField->isNative())
            continue;
        if (typeField->isSlice())
            continue;
        if (typeField->isInterface())
            continue;
        if (typeField->isLambda())
            continue;

        if (typeField->isStruct())
        {
            SWAG_CHECK(waitForStructUserOps(context, c));
            YIELD();
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeField, TypeInfoKind::Struct);
            if (!typeStruct->isPlainOldData())
                return context->report({c, formErr(Err0169, c->token.cstr())});
            continue;
        }

        const auto aKindName = Naming::aKindName(typeField);
        return context->report({c, formErr(Err0168, c->token.cstr(), aKindName.cstr(), aKindName.cstr())});
    }

    // As this is the capture block resolved in the right context, we can now evaluate the corresponding closure
    const auto mpl = castAst<AstMakePointer>(node->parent, AstNodeKind::MakePointerLambda);
    SWAG_ASSERT(mpl->lambda);

    ScopedLock lk(mpl->lambda->mutex);
    if (mpl->lambda->hasAstFlag(AST_SPEC_SEMANTIC1))
    {
        mpl->lambda->removeAstFlag(AST_SPEC_SEMANTIC1);
        launchResolveSubDecl(context, mpl->lambda);
    }

    return true;
}

bool Semantic::resolveFuncCallGenParams(SemanticContext* context)
{
    const auto node = context->node;
    node->inheritAstFlagsOr(AST_GENERIC);
    node->inheritAstFlagsAnd(AST_CONST_EXPR);

    if (node->hasAstFlag(AST_GENERIC))
        return true;

    for (auto c : node->children)
    {
        if (c->hasFlagComputedValue())
            continue;

        const auto symbol = c->firstChild()->resolvedSymbolName();
        if (!symbol)
            continue;

        if (symbol->is(SymbolKind::Variable) ||
            symbol->is(SymbolKind::Namespace) ||
            symbol->is(SymbolKind::Attribute))
        {
            return context->report({c, formErr(Err0194, Naming::aKindName(symbol->kind).cstr(), symbol->name.cstr())});
        }
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveFuncCallParams(SemanticContext* context)
{
    const auto node = context->node;
    node->inheritAstFlagsOr(AST_GENERIC);
    node->inheritAstFlagsAnd(AST_CONST_EXPR);
    return true;
}

bool Semantic::resolveFuncCallParam(SemanticContext* context)
{
    auto       node  = castAst<AstFuncCallParam>(context->node, AstNodeKind::FuncCallParam);
    const auto child = node->firstChild();
    node->typeInfo   = child->typeInfo;

    SWAG_VERIFY(!node->typeInfo->isCVariadic(), context->report({node, toErr(Err0483)}));

    // Force const if necessary
    // func([.., ...]) must be const
    if (child->is(AstNodeKind::ExpressionList))
    {
        const auto typeList = castTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
        if (typeList->isListArray())
            node->typeInfo = g_TypeMgr->makeConst(node->typeInfo);
    }

    node->byteCodeFct = ByteCodeGen::emitFuncCallParam;

    // Can be called for generic parameters in type definition, in that case, we are a type, so no
    // test for concrete must be done
    bool checkForConcrete = true;
    if (node->parent->hasAstFlag(AST_NO_BYTECODE))
        checkForConcrete = false;
    if (checkForConcrete)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, child));
        YIELD();
        node->typeInfo = child->typeInfo;
        node->addAstFlag(AST_R_VALUE);
    }

    node->inheritComputedValue(child);
    node->inheritAstFlagsOr(child, AST_CONST_EXPR | AST_GENERIC | AST_VALUE_GEN_TYPEINFO | AST_OP_AFFECT_CAST | AST_TRANSIENT);
    if (node->firstChild()->hasSemFlag(SEMFLAG_LITERAL_SUFFIX))
        node->addSemFlag(SEMFLAG_LITERAL_SUFFIX);

    // Inherit the original type in case of computed values, to make the cast if necessary
    if (node->hasAstFlag(AST_COMPUTED_VALUE | AST_OP_AFFECT_CAST))
        node->typeInfoCast = child->typeInfoCast;

    if (checkForConcrete && !node->hasAstFlag(AST_OP_AFFECT_CAST))
    {
        SWAG_CHECK(evaluateConstExpression(context, node));
        YIELD();
    }

    node->setResolvedSymbol(child->resolvedSymbolName(), child->resolvedSymbolOverload());

    const auto userOp = child->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
    if (userOp)
        node->addExtraPointer(ExtraPointerKind::UserOp, userOp);

    // If the call has been generated because of a 'return tuple', then we force a move
    // instead of a copy, in case the parameter to the tuple init is a local variable
    if (node->autoTupleReturn)
    {
        if (node->resolvedSymbolOverload() && node->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_LOCAL))
        {
            node->addAstFlag(AST_FORCE_MOVE | AST_NO_RIGHT_DROP);
            node->autoTupleReturn->forceNoDrop.push_back(child->resolvedSymbolOverload());
        }
    }

    return true;
}

bool Semantic::resolveRetVal(SemanticContext* context)
{
    auto       node    = context->node;
    const auto fctDecl = node->hasOwnerInline() ? node->ownerInline()->func : node->ownerFct;

    SWAG_VERIFY(fctDecl, context->report({node, toErr(Err0343)}));
    SWAG_VERIFY(node->ownerScope && node->ownerScope->isNot(ScopeKind::Function), context->report({node, toErr(Err0343)}));

    const auto fct     = castAst<AstFuncDecl>(fctDecl, AstNodeKind::FuncDecl);
    const auto typeFct = castTypeInfo<TypeInfoFuncAttr>(fct->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_VERIFY(typeFct->returnType && !typeFct->returnType->isVoid(), context->report({node, toErr(Err0342)}));

    // @WaitForPOD
    if (typeFct->returnType->isStruct())
    {
        waitStructGenerated(context->baseJob, typeFct->returnType);
        YIELD();
    }

    // If this is a simple return type, remove the retval stuff.
    // Variable will behaves normally, in the stack
    if (!typeFct->returnByStackAddress())
    {
        auto parentNode = node;
        if (parentNode->is(AstNodeKind::Identifier))
            parentNode = parentNode->findParent(AstNodeKind::TypeExpression);
        const auto typeExpr = castAst<AstTypeExpression>(parentNode, AstNodeKind::TypeExpression);
        typeExpr->typeFlags.remove(TYPE_FLAG_IS_RETVAL);
    }

    node->typeInfo = typeFct->returnType;
    return true;
}

void Semantic::propagateReturn(AstNode* node)
{
    auto stopFct = node->ownerFct ? node->ownerFct->parent : nullptr;
    if (node->hasSemFlag(SEMFLAG_EMBEDDED_RETURN))
        stopFct = node->ownerInline()->parent;
    SWAG_ASSERT(stopFct);

    AstNode* scanNode = node;

    // Search if we are in an infinite loop
    auto breakable = node->safeOwnerBreakable();
    while (breakable)
    {
        if (breakable->is(AstNodeKind::Loop))
        {
            const auto loopNode = castAst<AstLoop>(breakable, AstNodeKind::Loop);
            if (!loopNode->expression)
                loopNode->breakableFlags.add(BREAKABLE_RETURN_IN_INFINITE_LOOP);
            break;
        }

        if (breakable->is(AstNodeKind::While))
        {
            const auto whileNode = castAst<AstWhile>(breakable, AstNodeKind::While);
            if (whileNode->boolExpression->hasFlagComputedValue() && whileNode->boolExpression->computedValue()->reg.b)
                whileNode->breakableFlags.add(BREAKABLE_RETURN_IN_INFINITE_LOOP);
            break;
        }

        if (breakable->is(AstNodeKind::For))
        {
            const auto forNode = castAst<AstFor>(breakable, AstNodeKind::For);
            if (forNode->boolExpression->hasFlagComputedValue() && forNode->boolExpression->computedValue()->reg.b)
                forNode->breakableFlags.add(BREAKABLE_RETURN_IN_INFINITE_LOOP);
            break;
        }

        breakable = breakable->safeOwnerBreakable();
    }

    // Propagate the return in the corresponding scope
    while (scanNode && scanNode != stopFct)
    {
        if (scanNode->hasSemFlag(SEMFLAG_SCOPE_HAS_RETURN) && !scanNode->hasSemFlag(SEMFLAG_SCOPE_FORCE_HAS_RETURN))
            break;
        scanNode->addSemFlag(SEMFLAG_SCOPE_HAS_RETURN);
        if (scanNode->parent && scanNode->parent->is(AstNodeKind::If))
        {
            const auto ifNode = castAst<AstIf>(scanNode->parent, AstNodeKind::If);
            if (ifNode->elseBlock != scanNode)
                break;
            if (!ifNode->ifBlock->hasSemFlag(SEMFLAG_SCOPE_HAS_RETURN))
                break;
        }
        else if (scanNode->is(AstNodeKind::SwitchCase))
        {
            const auto sc = castAst<AstSwitchCase>(scanNode, AstNodeKind::SwitchCase);
            if (sc->hasSpecFlag(AstSwitchCase::SPEC_FLAG_IS_DEFAULT))
                sc->ownerSwitch->addSemFlag(SEMFLAG_SCOPE_FORCE_HAS_RETURN);
        }
        else if (scanNode->is(AstNodeKind::Switch))
        {
            if (!scanNode->hasSemFlag(SEMFLAG_SCOPE_FORCE_HAS_RETURN))
                break;
        }
        else if (scanNode->is(AstNodeKind::While) ||
                 scanNode->is(AstNodeKind::Loop) ||
                 scanNode->is(AstNodeKind::For))
        {
            break;
        }

        scanNode = scanNode->parent;
    }

    // To tell that the function as at least one return (this will change the error message)
    while (scanNode && scanNode != stopFct)
    {
        if (scanNode->hasSemFlag(SEMFLAG_FCT_HAS_RETURN))
            break;
        scanNode->addSemFlag(SEMFLAG_FCT_HAS_RETURN);
        scanNode = scanNode->parent;
    }
}

AstFuncDecl* Semantic::getFunctionForReturn(AstNode* node)
{
    // For a return inside an inline block, take the inlined function, except for a mixin or
    // if the inlined function is flagged with 'Swag.CalleeReturn' (in that case we take the owner function)
    auto funcNode = node->ownerFct;
    if (node->hasOwnerInline() && node->ownerInline()->isParentOf(node))
    {
        if (!node->ownerInline()->func->hasAttribute(ATTRIBUTE_CALLEE_RETURN) && !node->hasAstFlag(AST_IN_MIXIN))
        {
            if (node->is(AstNodeKind::Return))
                node->addSemFlag(SEMFLAG_EMBEDDED_RETURN);
            funcNode = node->ownerInline()->func;
        }
    }

    return funcNode;
}

bool Semantic::resolveReturn(SemanticContext* context)
{
    SWAG_CHECK(SemanticError::warnUnreachableCode(context));

    const auto node     = castAst<AstReturn>(context->node, AstNodeKind::Return);
    const auto funcNode = getFunctionForReturn(node);

    node->byteCodeFct         = ByteCodeGen::emitReturn;
    node->resolvedFuncDecl    = funcNode;
    const auto funcReturnType = TypeManager::concreteType(funcNode->returnType->typeInfo);

    // As the type of the function is deduced from the return type, be sure they match in case
    // of multiple returns
    if (funcNode->returnTypeDeducedNode)
    {
        // We return nothing, but the previous return had something
        if (node->children.empty())
        {
            if (!funcReturnType->isVoid())
            {
                Diagnostic err{node, formErr(Err0473, funcReturnType->getDisplayNameC())};
                err.addNote(funcNode->returnTypeDeducedNode->firstChild(), toNte(Nte0195));
                return context->report(err);
            }

            return true;
        }

        const auto child     = node->firstChild();
        const auto childType = TypeManager::concreteType(child->typeInfo);

        // We try to return something, but the previous return had nothing
        if (funcReturnType->isVoid() && !childType->isVoid())
        {
            Diagnostic err{child, formErr(Err0603, childType->getDisplayNameC())};
            err.addNote(funcNode->returnTypeDeducedNode, toNte(Nte0195));
            return context->report(err);
        }

        constexpr CastFlags castFlags = CAST_FLAG_JUST_CHECK | CAST_FLAG_UN_CONST | CAST_FLAG_AUTO_OP_CAST | CAST_FLAG_TRY_COERCE | CAST_FLAG_FOR_AFFECT;
        if (!TypeManager::makeCompatibles(context, funcNode->returnType->typeInfo, nullptr, child, castFlags))
        {
            Diagnostic err{child, formErr(Err0602, child->typeInfo->getDisplayNameC(), funcNode->returnType->typeInfo->getDisplayNameC())};
            err.addNote(funcNode->returnTypeDeducedNode->firstChild(), toNte(Nte0195));
            return context->report(err);
        }
    }

    // Nothing to return
    if (funcNode->returnType->typeInfo->isVoid() && node->children.empty())
    {
        if (funcNode->hasAttribute(ATTRIBUTE_RUN_GENERATED_EXP))
        {
            funcNode->returnType->typeInfo  = g_TypeMgr->typeInfoVoid;
            funcNode->returnTypeDeducedNode = node;
        }

        return true;
    }

    // Deduce return type
    const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    bool       lateRegister = funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_FORCE_LATE_REGISTER);
    if (funcReturnType->isVoid() || funcReturnType->isGeneric())
    {
        if (!funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_LATE_REGISTER_DONE))
        {
            // This is a short lambda without a specified return type. We now have it
            bool tryDeduce = false;
            if (funcNode->hasSpecFlag(AstFuncDecl::SPEC_FLAG_SHORT_LAMBDA) && !funcNode->returnType->hasSpecFlag(AstFuncDecl::SPEC_FLAG_RETURN_DEFINED))
                tryDeduce = true;
            if (funcNode->hasAttribute(ATTRIBUTE_RUN_GENERATED_EXP))
                tryDeduce = true;
            if (tryDeduce)
            {
                funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_FORCE_LATE_REGISTER);
                typeInfoFunc->returnType  = TypeManager::concreteType(node->firstChild()->typeInfo, CONCRETE_FUNC);
                typeInfoFunc->returnType  = TypeManager::promoteUntyped(typeInfoFunc->returnType);
                const auto concreteReturn = TypeManager::concreteType(typeInfoFunc->returnType);
                if (concreteReturn->isListTuple())
                {
                    SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, funcNode->content, node->firstChild(), &funcNode->returnType));
                    Ast::setForceConstType(funcNode->returnType);
                    context->baseJob->nodes.push_back(funcNode->returnType);
                    context->result = ContextResult::NewChildren;
                    return true;
                }

                typeInfoFunc->forceComputeName();
                funcNode->returnType->typeInfo  = typeInfoFunc->returnType;
                funcNode->returnTypeDeducedNode = node;
                funcNode->addSpecFlag(AstFuncDecl::SPEC_FLAG_LATE_REGISTER_DONE);
                lateRegister = true;
            }
        }
    }

    if (node->children.empty())
    {
        Diagnostic err{node, formErr(Err0474, funcNode->returnType->typeInfo->getDisplayNameC())};
        err.addNote(funcNode->returnType->firstChild(), formNte(Nte0149, typeInfoFunc->returnType->getDisplayNameC()));
        return context->report(err);
    }

    auto returnType = funcNode->returnType->typeInfo;
    SWAG_ASSERT(returnType);

    // Check types
    auto child = node->firstChild();
    SWAG_CHECK(checkIsConcreteOrType(context, child));
    YIELD();

    const auto concreteType = TypeManager::concreteType(child->typeInfo);

    // No return value in a #run block
    if (!concreteType->isVoid())
    {
        if (funcNode->hasAttribute(ATTRIBUTE_RUN_FUNC |
                                   ATTRIBUTE_RUN_GENERATED_FUNC |
                                   ATTRIBUTE_MAIN_FUNC |
                                   ATTRIBUTE_INIT_FUNC |
                                   ATTRIBUTE_DROP_FUNC |
                                   ATTRIBUTE_PREMAIN_FUNC |
                                   ATTRIBUTE_TEST_FUNC))
        {
            if (funcNode->hasAttribute(ATTRIBUTE_SHARP_FUNC))
                return context->report({child, formErr(Err0654, funcNode->getDisplayNameC())});
        }
    }

    // Be sure we do not specify a return value, and the function does not have a return type
    // (better error message than just letting the makeCompatibles do its job)
    if (returnType->isVoid() && !concreteType->isVoid())
    {
        Utf8 msg;
        if (Parser::isGeneratedName(funcNode->token.text))
            msg = formErr(Err0657, concreteType->getDisplayNameC());
        else
            msg = formErr(Err0658, concreteType->getDisplayNameC(), funcNode->token.cstr());

        Diagnostic err{child, msg};

        if (node->hasOwnerInline() && !node->hasSemFlag(SEMFLAG_EMBEDDED_RETURN))
            err.addNote(funcNode, funcNode->getTokenName(), formNte(Nte0121, node->ownerInline()->func->token.cstr(), funcNode->token.cstr()));

        err.addNote(funcNode, funcNode->getTokenName(), formNte(Nte0032, concreteType->name.cstr()));
        return context->report(err);
    }

    // @WaitForPOD
    if (returnType->isStruct())
    {
        waitAllStructSpecialMethods(context->baseJob, returnType);
        YIELD();
    }

    // If returning retval, then returning nothing, as we will change the return parameter value in place
    if (child->resolvedSymbolOverload() && child->resolvedSymbolOverload()->hasFlag(OVERLOAD_RETVAL))
    {
        node->typeInfo = child->typeInfo;
    }
    else
    {
        // If we are returning an interface, be sure they are defined before casting
        if (returnType->isInterface())
        {
            waitAllStructInterfaces(context->baseJob, child->typeInfo);
            YIELD();
        }

        constexpr CastFlags castFlags = CAST_FLAG_UN_CONST | CAST_FLAG_AUTO_OP_CAST | CAST_FLAG_TRY_COERCE | CAST_FLAG_FOR_AFFECT | CAST_FLAG_PTR_REF | CAST_FLAG_ACCEPT_PENDING;

        if (funcNode->hasAttribute(ATTRIBUTE_AST_FUNC))
        {
            PushErrCxtStep ec{context, funcNode, ErrCxtStepKind::Note, [] { return toNte(Nte0141); }, true};
            SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, castFlags));
            YIELD();
        }
        else if (funcNode->hasAttribute(ATTRIBUTE_SHARP_FUNC))
        {
            PushErrCxtStep ec{context, funcNode, ErrCxtStepKind::Note, [returnType] { return formNte(Nte0149, returnType->getDisplayNameC()); }, true};
            SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, castFlags));
            YIELD();
        }
        else
        {
            auto nodeErr = funcNode->returnType;
            if (nodeErr->is(AstNodeKind::FuncDeclType) && !funcNode->returnType->children.empty())
                nodeErr = funcNode->returnType->firstChild();
            PushErrCxtStep ec{context, nodeErr, ErrCxtStepKind::Note, [returnType] {
                                  return formNte(Nte0149, returnType->getDisplayNameC());
                              }};
            SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, castFlags));
            YIELD();
        }
    }

    if (child->is(AstNodeKind::ExpressionList))
    {
        child->addAstFlag(AST_TRANSIENT);
    }

    const auto childType = TypeManager::concreteType(child->typeInfo);
    if (!returnType->isPointerRef() && childType->isPointerRef())
        setUnRef(child);

    // When returning a struct, we need to know if postcopy or postmove are here, and wait for them to resolve
    if (returnType && (returnType->isStruct() || returnType->isArrayOfStruct()))
    {
        SWAG_CHECK(waitForStructUserOps(context, funcNode->returnType));
        YIELD();
    }

    // @opAffectParam
    if (const auto userOp = child->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp))
    {
        if (userOp->symbol->name == g_LangSpec->name_opAffect || userOp->symbol->name == g_LangSpec->name_opAffectLiteral)
        {
            SWAG_ASSERT(child->typeInfoCast);
            child->addExtraPointer(ExtraPointerKind::UserOp, nullptr);
            child->typeInfoCast = nullptr;

            const auto varNode = Ast::newVarDecl(form("__2tmp_%d", g_UniqueID.fetch_add(1)), nullptr, node);
            varNode->addAstFlag(AST_GENERATED);
            varNode->inheritTokenLocation(child->token);

            const auto typeExpr = Ast::newTypeExpression(nullptr, varNode);
            typeExpr->typeInfo  = child->typeInfo;
            typeExpr->addAstFlag(AST_NO_SEMANTIC);
            varNode->type = typeExpr;

            CloneContext cloneContext;
            cloneContext.parent      = varNode;
            cloneContext.parentScope = child->ownerScope;
            varNode->assignment      = child->clone(cloneContext);

            Ast::removeFromParent(child);

            Ast::removeFromParent(varNode);
            const auto idRef = Ast::newIdentifierRef(varNode->token.text, nullptr, node);
            Ast::addChildBack(node, varNode);

            idRef->addExtraPointer(ExtraPointerKind::ExportNode, child);
            idRef->allocateExtension(ExtensionKind::Owner);
            idRef->extOwner()->nodesToFree.push_back(child);

            context->baseJob->nodes.push_back(node->firstChild());
            context->baseJob->nodes.push_back(varNode);
            node->addAstFlag(AST_REVERSE_SEMANTIC);
            varNode->addSemFlag(SEMFLAG_ONCE);
            context->result = ContextResult::NewChildren;
            return true;
        }
    }

    // If we are returning a local variable, we can do a move
    if (child->resolvedSymbolOverload() && child->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_LOCAL))
    {
        child->addAstFlag(AST_FORCE_MOVE | AST_NO_RIGHT_DROP);
        node->forceNoDrop.push_back(child->resolvedSymbolOverload());
    }

    if (child->resolvedSymbolOverload() && child->resolvedSymbolOverload()->hasFlag(OVERLOAD_RETVAL))
    {
        child->addAstFlag(AST_NO_BYTECODE);
        node->forceNoDrop.push_back(child->resolvedSymbolOverload());
    }

    // Propagate return so that we can detect if some paths are missing one
    propagateReturn(node);

    // Register symbol now that we have inferred the return type
    if (lateRegister)
    {
        funcNode->removeSpecFlag(AstFuncDecl::SPEC_FLAG_FORCE_LATE_REGISTER);
        typeInfoFunc->returnType = funcNode->returnType->typeInfo;
        typeInfoFunc->forceComputeName();
        SWAG_CHECK(registerFuncSymbol(context, funcNode));
    }

    return true;
}

uint32_t Semantic::getMaxStackSize(AstNode* node)
{
    const auto decSP = node->ownerScope->startStackSize;

    if (!node->ownerFct)
    {
        const auto p = castAst<AstFile>(node->findParentOrMe(AstNodeKind::File), AstNodeKind::File);
        return std::max(decSP, p->getStackSize());
    }

    return std::max(decSP, node->ownerFct->stackSize);
}

void Semantic::setOwnerMaxStackSize(AstNode* node, uint32_t size)
{
    size = std::max(size, static_cast<uint32_t>(1));
    size = static_cast<uint32_t>(TypeManager::align(size, sizeof(void*)));

    if (!node->ownerFct)
    {
        const auto p = castAst<AstFile>(node->findParentOrMe(AstNodeKind::File), AstNodeKind::File);
        p->setStackSize(std::max(p->getStackSize(), size));
    }
    else
    {
        node->ownerFct->stackSize = std::max(node->ownerFct->stackSize, size);
    }
}
