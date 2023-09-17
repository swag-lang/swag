#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Os.h"
#include "LanguageSpec.h"
#include "Naming.h"

void SemanticJob::allocateOnStack(AstNode* node, TypeInfo* typeInfo)
{
    node->allocateComputedValue();
    node->computedValue->storageOffset = node->ownerScope->startStackSize;
    node->ownerScope->startStackSize += typeInfo->isStruct() ? max(typeInfo->sizeOf, 8) : typeInfo->sizeOf;
    SemanticJob::setOwnerMaxStackSize(node, node->ownerScope->startStackSize);
}

bool SemanticJob::setupFuncDeclParams(SemanticContext* context, TypeInfoFuncAttr* typeInfo, AstNode* funcNode, AstNode* parameters, bool forGenerics)
{
    if (!parameters || (funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC))
        return true;

    bool   defaultValueDone = false;
    size_t index            = 0;

    // If we have a tuple as a default parameter, without a user defined type, then we need to convert it to a tuple struct
    // and wait for the type to be solved.
    for (auto param : parameters->childs)
    {
        auto nodeParam = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        if (!nodeParam->assignment)
            continue;
        if (nodeParam->assignment->kind != AstNodeKind::ExpressionList)
            continue;

        auto nodeExpr = CastAst<AstExpressionList>(nodeParam->assignment, AstNodeKind::ExpressionList);
        if (!(nodeExpr->specFlags & AstExpressionList::SPECFLAG_FOR_TUPLE))
            continue;

        if (!nodeParam->type)
        {
            nodeParam->semFlags |= SEMFLAG_TUPLE_CONVERT;
            SWAG_ASSERT(nodeParam->typeInfo->isListTuple());
            SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, nodeParam, nodeParam->assignment, &nodeParam->type));
            context->result = ContextResult::NewChilds;
            context->job->nodes.push_back(nodeParam->type);
            return true;
        }
        else if (nodeParam->semFlags & SEMFLAG_TUPLE_CONVERT)
        {
            SWAG_ASSERT(nodeParam->resolvedSymbolOverload);
            nodeParam->typeInfo                         = nodeParam->type->typeInfo;
            nodeParam->resolvedSymbolOverload->typeInfo = nodeParam->typeInfo;
        }
    }

    if (forGenerics)
    {
        typeInfo->genericParameters.clear();
        typeInfo->genericParameters.reserve((int) parameters->childs.size());
        typeInfo->flags |= TYPEINFO_GENERIC;
    }
    else
    {
        typeInfo->parameters.clear();
        typeInfo->parameters.reserve((int) parameters->childs.size());
    }

    AstNode* firstParamWithDef = nullptr;
    auto     sourceFile        = context->sourceFile;
    for (auto param : parameters->childs)
    {
        auto nodeParam        = CastAst<AstVarDecl>(param, AstNodeKind::FuncDeclParam);
        auto funcParam        = g_TypeMgr->makeParam();
        funcParam->name       = param->token.text;
        funcParam->typeInfo   = param->typeInfo;
        funcParam->index      = index++;
        funcParam->declNode   = nodeParam;
        funcParam->attributes = nodeParam->attributes;
        if (nodeParam->specFlags & AstVarDecl::SPECFLAG_GENERIC_TYPE)
            funcParam->flags |= TYPEINFOPARAM_GENERIC_TYPE;
        else if (nodeParam->specFlags & AstVarDecl::SPECFLAG_GENERIC_CONSTANT)
            funcParam->flags |= TYPEINFOPARAM_GENERIC_CONSTANT;

        auto paramType     = nodeParam->typeInfo;
        auto paramNodeType = nodeParam->type ? nodeParam->type : nodeParam;

        // Code is only valid for a macro or mixin
        if (paramType->isCode())
            SWAG_VERIFY(funcNode->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN), context->report({paramNodeType, Err(Err0729)}));

        // Not everything is possible for types for attributes
        if (param->ownerScope->kind == ScopeKind::Attribute)
        {
            SWAG_VERIFY(!funcParam->typeInfo->isAny(), context->report({nodeParam, Fmt(Err(Err0731), funcParam->typeInfo->getDisplayNameC())}));

            if (!funcParam->typeInfo->isNative() &&
                !funcParam->typeInfo->isEnum() &&
                !funcParam->typeInfo->isPointerToTypeInfo() &&
                !funcParam->typeInfo->isTypedVariadic())
            {
                return context->report({nodeParam, Fmt(Err(Err0731), funcParam->typeInfo->getDisplayNameC())});
            }

            if (funcParam->typeInfo->isTypedVariadic())
            {
                auto typeVar = CastTypeInfo<TypeInfoVariadic>(funcParam->typeInfo, TypeInfoKind::TypedVariadic);
                SWAG_VERIFY(!typeVar->isAny(), context->report({paramNodeType, Fmt(Err(Err0731), funcParam->typeInfo->getDisplayNameC())}));
            }
        }

        parameters->inheritOrFlag(nodeParam->type, AST_IS_GENERIC);

        // Variadic must be the last one
        if (paramType->isVariadic())
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, Err(Err0733)}));
            typeInfo->flags |= TYPEINFO_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, Err(Err0734)});
        }
        else if (paramType->isTypedVariadic())
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, Err(Err0733)}));
            typeInfo->flags |= TYPEINFO_TYPED_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, Err(Err0734)});
        }
        else if (paramType->isCVariadic())
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({sourceFile, nodeParam->token, Err(Err0733)}));
            typeInfo->flags |= TYPEINFO_C_VARIADIC;
            if (index != parameters->childs.size())
                return context->report({nodeParam, Err(Err0734)});
        }

        // Default parameter value
        if (nodeParam->assignment)
        {
            if (!defaultValueDone)
            {
                defaultValueDone               = true;
                typeInfo->firstDefaultValueIdx = (uint32_t) (index - 1);
                firstParamWithDef              = nodeParam;
            }

            if (nodeParam->assignment->kind == AstNodeKind::CompilerSpecialValue)
            {
                switch (nodeParam->assignment->tokenId)
                {
                case TokenId::CompilerCallerLocation:
                case TokenId::CompilerCallerFunction:
                case TokenId::CompilerBuildCfg:
                case TokenId::CompilerOs:
                case TokenId::CompilerArch:
                case TokenId::CompilerCpu:
                case TokenId::CompilerSwagOs:
                case TokenId::CompilerBackend:
                    break;

                default:
                    context->report({nodeParam->assignment, Fmt(Err(Err0737), nodeParam->assignment->token.ctext())});
                    break;
                }
            }
        }
        else if (!nodeParam->typeInfo->isCode())
        {
            if (defaultValueDone)
            {
                Diagnostic diag{nodeParam, Fmt(Err(Err0738), Naming::niceParameterRank((int) index).c_str())};
                diag.hint = Nte(Nte1089);
                diag.addRange(firstParamWithDef, Nte(Nte1088));
                return context->report(diag);
            }
        }

        if (forGenerics)
            typeInfo->genericParameters.push_back(funcParam);
        else
            typeInfo->parameters.push_back(funcParam);
    }

    return true;
}

bool SemanticJob::resolveFuncDeclParams(SemanticContext* context)
{
    auto node = context->node;
    node->inheritOrFlag(AST_IS_GENERIC);
    node->byteCodeFct = ByteCodeGenJob::emitFuncDeclParams;
    return true;
}

bool SemanticJob::sendCompilerMsgFuncDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;

    // Filter what we send
    if (module->kind == ModuleKind::BootStrap || module->kind == ModuleKind::Runtime)
        return true;
    if (sourceFile->imported && !sourceFile->isEmbedded)
        return true;
    if (!context->node->ownerScope->isGlobalOrImpl())
        return true;
    if (context->node->attributeFlags & ATTRIBUTE_GENERATED_FUNC)
        return true;
    if (context->node->flags & (AST_IS_GENERIC | AST_FROM_GENERIC))
        return true;

    auto funcNode = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

    CompilerMessage msg;
    msg.concrete.kind        = CompilerMsgKind::SemFunctions;
    msg.concrete.name.buffer = funcNode->token.text.buffer;
    msg.concrete.name.count  = funcNode->token.text.length();
    msg.typeInfo             = typeInfo;
    SWAG_CHECK(module->postCompilerMessage(context, msg));

    return true;
}

bool SemanticJob::resolveFuncDeclAfterSI(SemanticContext* context)
{
    auto saveNode = context->node;
    if (context->node->parent->kind == AstNodeKind::Inline)
    {
        auto node     = CastAst<AstInline>(context->node->parent, AstNodeKind::Inline);
        context->node = node->func;
    }
    else
    {
        auto node = CastAst<AstFuncDecl>(context->node->parent, AstNodeKind::FuncDecl);
        SWAG_ASSERT(node->content == context->node);
        context->node = node;
    }

    resolveFuncDecl(context);
    resolveScopedStmtAfter(context);
    context->node = saveNode;
    return true;
}

bool SemanticJob::resolveFuncDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto funcNode   = CastAst<AstFuncDecl>(context->node, AstNodeKind::FuncDecl);
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);

    // Only one main per module !
    if (funcNode->attributeFlags & ATTRIBUTE_MAIN_FUNC)
    {
        ScopedLock lk(sourceFile->module->mutexFile);
        if (sourceFile->module->mainIsDefined)
        {
            Diagnostic diag{funcNode, Err(Err0739)};
            auto       note = Diagnostic::note(module->mainIsDefined, Nte(Nte0036));
            return context->report(diag, note);
        }

        sourceFile->module->mainIsDefined = funcNode;
    }

    // No semantic on a generic function, or a macro
    if (funcNode->flags & AST_IS_GENERIC)
    {
        if ((funcNode->attributeFlags & ATTRIBUTE_PUBLIC) && !(funcNode->flags & AST_GENERATED))
            funcNode->ownerScope->addPublicNode(funcNode);
        return true;
    }

    // Check that there is no 'hole' in alias names
    if (funcNode->aliasMask && (funcNode->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN)))
    {
        auto mask = funcNode->aliasMask;
        auto maxN = OS::bitcountlz(funcNode->aliasMask);
        for (uint32_t n = 0; n < 32 - maxN; n++)
        {
            if ((mask & 1) == 0)
                return context->report({funcNode, Fmt(Err(Err0741), funcNode->token.ctext(), n)});
            mask >>= 1;
        }
    }

    if (funcNode->attributeFlags & ATTRIBUTE_MACRO)
    {
        if ((funcNode->attributeFlags & ATTRIBUTE_PUBLIC) && !(funcNode->flags & AST_GENERATED) && !(funcNode->flags & AST_FROM_GENERIC))
            funcNode->ownerScope->addPublicNode(funcNode);
        SWAG_CHECK(setFullResolve(context, funcNode));
        return true;
    }

    // An inline function will not have bytecode, so need to register public by hand now
    if ((funcNode->attributeFlags & ATTRIBUTE_PUBLIC) && (funcNode->attributeFlags & ATTRIBUTE_INLINE) && !(funcNode->flags & AST_FROM_GENERIC))
        funcNode->ownerScope->addPublicNode(funcNode);

    funcNode->byteCodeFct = ByteCodeGenJob::emitLocalFuncDecl;
    typeInfo->stackSize   = funcNode->stackSize;

    // Check attributes
    if ((funcNode->isForeign()) && funcNode->content)
        return context->report({funcNode, Err(Err0742)});

    if (!(funcNode->attributeFlags & ATTRIBUTE_GENERATED_FUNC))
    {
        if (funcNode->attributeFlags & ATTRIBUTE_TEST_FUNC)
        {
            SWAG_VERIFY(module->kind == ModuleKind::Test, context->report({funcNode, Err(Err0744)}));
        }
    }

    // Can be null for intrinsics etc...
    if (funcNode->content)
        funcNode->content->setBcNotifBefore(ByteCodeGenJob::emitBeforeFuncDeclContent);

    // Do we have a return value
    if (funcNode->content && funcNode->returnType && !funcNode->returnType->typeInfo->isVoid())
    {
        if (!(funcNode->content->flags & AST_NO_SEMANTIC))
        {
            if (!(funcNode->semFlags & SEMFLAG_SCOPE_HAS_RETURN))
            {
                if (funcNode->semFlags & SEMFLAG_FCT_HAS_RETURN)
                    return context->report({funcNode->returnType, Fmt(Err(Err0748), funcNode->getDisplayNameC())});
                return context->report({funcNode->returnType, Fmt(Err(Err0749), funcNode->getDisplayNameC(), funcNode->returnType->typeInfo->getDisplayNameC())});
            }
        }
    }

    // Content semantic can have been disabled (#validif). In that case, we're not done yet, so
    // do not set the FULL_RESOLVE flag and do not generate bytecode
    if (funcNode->content && (funcNode->content->flags & AST_NO_SEMANTIC))
    {
        ScopedLock lk(funcNode->funcMutex);
        funcNode->specFlags |= AstFuncDecl::SPECFLAG_PARTIAL_RESOLVE;
        funcNode->dependentJobs.setRunning();
        return true;
    }

    if (funcNode->specFlags & AstFuncDecl::SPECFLAG_IMPL)
    {
        bool implFor = false;
        if (funcNode->ownerScope && funcNode->ownerScope->kind == ScopeKind::Impl)
        {
            auto implNode = CastAst<AstImpl>(funcNode->ownerScope->owner, AstNodeKind::Impl);
            if (implNode->identifierFor)
            {
                auto forId = implNode->identifier->childs.back();
                implFor    = true;

                // Be sure interface has been fully solved
                {
                    ScopedLock lk(forId->mutex);
                    ScopedLock lk1(forId->resolvedSymbolName->mutex);
                    if (forId->resolvedSymbolName->cptOverloads)
                    {
                        context->job->waitSymbolNoLock(forId->resolvedSymbolName);
                        return true;
                    }
                }

                {
                    auto       typeBaseInterface = CastTypeInfo<TypeInfoStruct>(forId->resolvedSymbolOverload->typeInfo, TypeInfoKind::Interface);
                    auto       typeInterface     = CastTypeInfo<TypeInfoStruct>(typeBaseInterface->itable, TypeInfoKind::Struct);
                    ScopedLock lk(typeInterface->mutex);

                    // We need to search the function (as a variable) in the interface
                    // If not found, then this is a normal function...
                    auto symbolName = typeInterface->findChildByNameNoLock(funcNode->token.text); // O(n) !
                    if (symbolName)
                    {
                        funcNode->fromItfSymbol = symbolName;
                    }
                }
            }
        }

        // Be sure 'impl' is justified
        SWAG_VERIFY(implFor, context->report({funcNode->sourceFile, funcNode->implLoc, funcNode->implLoc, Err(Err0289)}));
    }

    // Warnings
    SWAG_CHECK(warnUnusedVariables(context, funcNode->scope));

    // Now the full function has been solved, so we wakeup jobs depending on that
    SWAG_CHECK(setFullResolve(context, funcNode));

    // Ask for bytecode
    bool genByteCode = true;
    if ((funcNode->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
        genByteCode = false;
    if (funcNode->token.text[0] == '@' && !(funcNode->flags & AST_DEFINED_INTRINSIC))
        genByteCode = false;
    if (funcNode->isForeign())
        genByteCode = false;
    if (funcNode->flags & AST_IS_GENERIC)
        genByteCode = false;
    if (funcNode->attributeFlags & ATTRIBUTE_INLINE)
        genByteCode = false;
    if (!funcNode->content)
        genByteCode = false;
    if (genByteCode)
        ByteCodeGenJob::askForByteCode(context->job, funcNode, 0);

    return true;
}

bool SemanticJob::setFullResolve(SemanticContext* context, AstFuncDecl* funcNode)
{
    ScopedLock lk(funcNode->funcMutex);
    funcNode->specFlags |= AstFuncDecl::SPECFLAG_FULL_RESOLVE | AstFuncDecl::SPECFLAG_PARTIAL_RESOLVE;
    funcNode->dependentJobs.setRunning();
    return true;
}

void SemanticJob::setFuncDeclParamsIndex(AstFuncDecl* funcNode)
{
    if (funcNode->parameters)
    {
        int storageIndex = 0;
        if (funcNode->typeInfo->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
        {
            auto param             = funcNode->parameters->childs.back();
            auto resolved          = param->resolvedSymbolOverload;
            resolved->storageIndex = 0; // Always the first one
            storageIndex += 2;
        }

        auto childSize = funcNode->parameters->childs.size();
        for (size_t i = 0; i < childSize; i++)
        {
            if ((i == childSize - 1) && funcNode->typeInfo->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
                break;
            auto param             = funcNode->parameters->childs[i];
            auto resolved          = param->resolvedSymbolOverload;
            resolved->storageIndex = storageIndex;

            auto typeParam    = TypeManager::concreteType(resolved->typeInfo);
            int  numRegisters = typeParam->numRegisters();
            storageIndex += numRegisters;
        }
    }
}

bool SemanticJob::resolveFuncDeclType(SemanticContext* context)
{
    auto typeNode = context->node;
    auto funcNode = CastAst<AstFuncDecl>(typeNode->parent, AstNodeKind::FuncDecl);

    // This is a lambda that was waiting for a match.
    // We are now awake, so everything has been done already
    if (funcNode->pendingLambdaJob)
    {
        setFuncDeclParamsIndex(funcNode);
        funcNode->pendingLambdaJob = nullptr;
        return true;
    }

    // If this is a #message function, we must have a flag mask as parameters
    if ((funcNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC) && funcNode->parameters)
    {
        auto parameters = funcNode->parameters;
        auto paramType  = TypeManager::concreteType(parameters->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
        SWAG_VERIFY(paramType->isEnum(), context->report({parameters, Fmt(Err(Err0804), paramType->getDisplayNameC()), Diagnostic::isType(paramType)}));
        paramType->computeScopedName();
        SWAG_VERIFY(paramType->scopedName == g_LangSpec->name_Swag_CompilerMsgMask, context->report({parameters, Fmt(Err(Err0804), paramType->getDisplayNameC())}));
        SWAG_CHECK(evaluateConstExpression(context, parameters));
        if (context->result != ContextResult::Done)
            return true;
        funcNode->parameters->flags |= AST_NO_BYTECODE;
    }

    // Return type
    if (!typeNode->childs.empty())
    {
        auto front         = typeNode->childs.front();
        typeNode->typeInfo = front->typeInfo;
        if (typeNode->typeInfo->getCA()->isVoid())
        {
            Diagnostic diag{typeNode->sourceFile, typeNode->token.startLocation, front->token.endLocation, Err(Err0732)};
            diag.hint = Nte(Nte1026);
            return context->report(diag);
        }
    }
    else
    {
        typeNode->typeInfo = g_TypeMgr->typeInfoVoid;

        // :DeduceLambdaType
        if (funcNode->makePointerLambda &&
            (funcNode->makePointerLambda->specFlags & AstMakePointer::SPECFLAG_DEP_TYPE) &&
            !(funcNode->specFlags & AstFuncDecl::SPECFLAG_SHORT_LAMBDA))
        {
            auto deducedType = getDeducedLambdaType(context, funcNode->makePointerLambda);
            if (deducedType->isLambdaClosure())
            {
                auto typeFct       = CastTypeInfo<TypeInfoFuncAttr>(deducedType, TypeInfoKind::LambdaClosure);
                typeNode->typeInfo = typeFct->returnType;
            }
        }
    }

    // Collect function attributes
    auto       typeInfo = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    ScopedLock lkT(typeInfo->mutex);

    SWAG_ASSERT(funcNode->semanticState == AstNodeResolveState::ProcessingChilds);
    SWAG_CHECK(collectAttributes(context, funcNode, &typeInfo->attributes));

    // Check attributes
    if (funcNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        funcNode->flags |= AST_CONST_EXPR;
    if (funcNode->ownerFct)
        funcNode->attributeFlags |= funcNode->ownerFct->attributeFlags & ATTRIBUTE_COMPILER;

    if (!(funcNode->flags & AST_FROM_GENERIC) && !(funcNode->specFlags & AstFuncDecl::SPECFLAG_CHECK_ATTR))
    {
        // Can be called multiple times in case of a mixin/macro inside another inlined function
        funcNode->specFlags |= AstFuncDecl::SPECFLAG_CHECK_ATTR;

        if (funcNode->attributeFlags & ATTRIBUTE_MACRO)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, Fmt(Err(Err0757), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MIXIN), context->report({funcNode, funcNode->token, Fmt(Err(Err0758), funcNode->getDisplayNameC())}));
            funcNode->attributeFlags |= ATTRIBUTE_INLINE;
        }

        if (funcNode->attributeFlags & ATTRIBUTE_MIXIN)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, Fmt(Err(Err0759), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MACRO), context->report({funcNode, funcNode->token, Fmt(Err(Err0758), funcNode->getDisplayNameC())}));
            funcNode->attributeFlags |= ATTRIBUTE_INLINE;
            funcNode->attributeFlags |= ATTRIBUTE_MACRO;
        }

        if (funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MACRO), context->report({funcNode, funcNode->token, Fmt(Err(Err0761), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_MIXIN), context->report({funcNode, funcNode->token, Fmt(Err(Err0762), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_INLINE), context->report({funcNode, funcNode->token, Fmt(Err(Err0763), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC), context->report({funcNode, funcNode->token, Fmt(Err(Err0860), funcNode->getDisplayNameC())}));
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_CALLEE_RETURN), context->report({funcNode, funcNode->token, Fmt(Err(Err0512), funcNode->getDisplayNameC())}));
        }
    }

    SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_COMPLETE) || funcNode->token.text == g_LangSpec->name_opAffect || funcNode->token.text == g_LangSpec->name_opAffectSuffix, context->report({funcNode, funcNode->token, Fmt(Err(Err0753), funcNode->token.ctext())}));
    SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_IMPLICIT) || funcNode->token.text == g_LangSpec->name_opAffect || funcNode->token.text == g_LangSpec->name_opAffectSuffix || funcNode->token.text == g_LangSpec->name_opCast, context->report({funcNode, funcNode->token, Fmt(Err(Err0754), funcNode->token.ctext())}));
    SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_CALLEE_RETURN) || (funcNode->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO)), context->report({funcNode, funcNode->token, Fmt(Err(Err0755), funcNode->token.ctext())}));

    // Implicit attribute cannot be used on a generic function
    // This is because "extra" generic parameters must be specified and not deduced, and this is not possible for an implicit cast
    if (funcNode->attributeFlags & ATTRIBUTE_IMPLICIT && (funcNode->flags & (AST_IS_GENERIC | AST_FROM_GENERIC)))
    {
        bool ok = false;
        if (funcNode->token.text == g_LangSpec->name_opAffectSuffix && funcNode->genericParameters->childs.size() <= 1)
            ok = true;
        if (funcNode->token.text == g_LangSpec->name_opAffect && !funcNode->genericParameters)
            ok = true;
        if (!ok)
            return context->report({funcNode, funcNode->token, Fmt(Err(Err0756), funcNode->getDisplayNameC())});
    }

    if (!(funcNode->flags & AST_FROM_GENERIC))
    {
        // Determine if function is generic
        if (funcNode->ownerStructScope && (funcNode->ownerStructScope->owner->flags & AST_IS_GENERIC) && !(funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC))
            funcNode->flags |= AST_IS_GENERIC;
        if (funcNode->ownerFct && (funcNode->ownerFct->flags & AST_IS_GENERIC) && !(funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC))
            funcNode->flags |= AST_IS_GENERIC;

        if (funcNode->parameters)
            funcNode->inheritOrFlag(funcNode->parameters, AST_IS_GENERIC);

        if (funcNode->genericParameters)
        {
            SWAG_VERIFY(!(funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC), context->report({funcNode->genericParameters, Fmt(Err(Err0752), funcNode->token.ctext()), Nte(Nte1026)}));
            funcNode->flags |= AST_IS_GENERIC;
        }

        if (funcNode->flags & AST_IS_GENERIC)
            typeInfo->flags |= TYPEINFO_GENERIC;

        if ((funcNode->attributeFlags & ATTRIBUTE_NOT_GENERIC) && funcNode->flags & AST_IS_GENERIC)
            return context->report({funcNode, funcNode->token, Fmt(Err(Err0751), funcNode->token.ctext())});

        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->genericParameters, true));
        if (context->result != ContextResult::Done)
            return true;
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, funcNode, funcNode->parameters, false));
        if (context->result != ContextResult::Done)
            return true;
    }

    // If a lambda function will wait for a match, then no need to deduce the return type
    // It will be done in the same way as parameters
    if (!(funcNode->flags & AST_IS_GENERIC))
    {
        if ((funcNode->semFlags & SEMFLAG_PENDING_LAMBDA_TYPING) && typeNode->typeInfo->isVoid())
        {
            typeNode->typeInfo = g_TypeMgr->typeInfoUndefined;
            funcNode->specFlags &= ~AstFuncDecl::SPECFLAG_SHORT_LAMBDA;
        }
    }

    // Short lambda without a return type we must deduced
    // In that case, symbol registration will not be done at the end of that function but once the return expression
    // has been evaluated, and the type deduced
    bool shortLambda = false;
    if ((funcNode->specFlags & AstFuncDecl::SPECFLAG_SHORT_LAMBDA) && !(funcNode->returnType->specFlags & AstFuncDecl::SPECFLAG_RETURN_DEFINED))
        shortLambda = true;

    // :RunGeneratedExp
    bool mustDeduceReturnType = false;
    if (funcNode->attributeFlags & ATTRIBUTE_RUN_GENERATED_EXP)
        mustDeduceReturnType = true;

    // No semantic on content if function is generic
    if (funcNode->flags & AST_IS_GENERIC)
    {
        shortLambda = false;
        funcNode->content->flags |= AST_NO_SEMANTIC;
    }

    // Macro will not evaluate its content before being inline
    if ((funcNode->attributeFlags & ATTRIBUTE_MACRO) && !shortLambda)
        funcNode->content->flags |= AST_NO_SEMANTIC;

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
        return context->report({typeNode->childs.front(), Fmt(Err(Err0764), typeInfo->returnType->getDisplayNameC())});
    }

    typeInfo->name.clear();
    typeInfo->displayName.clear();
    typeInfo->computeWhateverNameNoLock(COMPUTE_NAME);

    // Special functions registration
    if (funcNode->parameters && funcNode->parameters->childs.size() == 1)
    {
        if (funcNode->token.text == g_LangSpec->name_opInit)
        {
            auto       typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto       typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserInitFct = funcNode;
        }
        else if (funcNode->token.text == g_LangSpec->name_opDrop)
        {
            auto       typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto       typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserDropFct = funcNode;
            SWAG_VERIFY(!(typeStruct->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR), context->report({funcNode, funcNode->token, Fmt(Err(Err0199), typeStruct->getDisplayNameC())}));
        }
        else if (funcNode->token.text == g_LangSpec->name_opPostCopy)
        {
            auto       typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto       typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserPostCopyFct = funcNode;
            SWAG_VERIFY(!(typeStruct->flags & TYPEINFO_STRUCT_NO_COPY), context->report({funcNode, funcNode->token, Fmt(Err(Err0765), typeStruct->name.c_str())}));
        }
        else if (funcNode->token.text == g_LangSpec->name_opPostMove)
        {
            auto       typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto       typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            ScopedLock lk(typeStruct->mutex);
            typeStruct->opUserPostMoveFct = funcNode;
        }
    }

    // If this is a lambda waiting for a match to know the types of its parameters, need to wait
    // Function SemanticJob::setSymbolMatch will wake us up as soon as a valid match is found
    if (funcNode->semFlags & SEMFLAG_PENDING_LAMBDA_TYPING)
    {
        if (!(funcNode->flags & AST_IS_GENERIC))
        {
            funcNode->pendingLambdaJob = context->job;
            context->job->setPending(JobWaitKind::PendingLambdaTyping, nullptr, funcNode, nullptr);
        }
    }

    // Set storageIndex of each parameters
    setFuncDeclParamsIndex(funcNode);

    // To avoid ambiguity, we do not want a function to declare a generic type 'T' if the struct
    // has the same generic parameter name (this is useless and implicit)
    if (funcNode->genericParameters && funcNode->ownerStructScope)
    {
        auto structDecl = CastAst<AstStruct>(funcNode->ownerStructScope->owner, AstNodeKind::StructDecl);
        if (structDecl->typeInfo->isGeneric())
        {
            for (auto c : funcNode->genericParameters->childs)
            {
                if (!c->resolvedSymbolOverload)
                    continue;

                for (auto sc : structDecl->genericParameters->childs)
                {
                    if (!sc->resolvedSymbolOverload)
                        continue;

                    if (c->resolvedSymbolOverload->node->token.text == sc->resolvedSymbolOverload->node->token.text)
                    {
                        Diagnostic diag{c, Fmt(Err(Err0893), c->resolvedSymbolOverload->node->token.ctext())};
                        auto       note = Diagnostic::note(sc->resolvedSymbolOverload->node, Nte(Nte0037));
                        return context->report(diag, note);
                    }
                }
            }
        }
    }

    // Do we have capture parameters ? If it's the case, then we need to register all symbols as variables in the function scope
    if (funcNode->captureParameters)
    {
        uint32_t storageOffset = 0;
        for (auto c : funcNode->captureParameters->childs)
        {
            Utf8 name = c->token.text;
            if (c->kind == AstNodeKind::MakePointer)
                name = c->childs.front()->token.text;

            AddSymbolTypeInfo toAdd;
            toAdd.node      = c;
            toAdd.typeInfo  = c->typeInfo;
            toAdd.kind      = SymbolKind::Variable;
            toAdd.flags     = OVERLOAD_VAR_CAPTURE;
            toAdd.aliasName = &name;

            auto overload = funcNode->scope->symTable.addSymbolTypeInfo(context, toAdd);
            if (!overload)
                return false;
            c->resolvedSymbolOverload             = overload;
            overload->computedValue.storageOffset = storageOffset;
            storageOffset += overload->typeInfo->sizeOf;
        }
    }

    // Register runtime libc function type, by name
    if (funcNode->sourceFile && funcNode->sourceFile->isRuntimeFile && funcNode->isEmptyFct())
    {
        ScopedLock lk(funcNode->sourceFile->module->mutexFile);
        funcNode->sourceFile->module->mapRuntimeFctsTypes[funcNode->token.text] = typeInfo;
    }

    // We should never reference an empty function
    // So consider this is a placeholder. This will generate an error in case the empty function is not replaced by a
    // real function at some point.
    if (funcNode->isEmptyFct() && !funcNode->isForeign() && funcNode->token.text[0] != '@')
    {
        ScopedLock lk(funcNode->resolvedSymbolName->mutex);

        // We need to be sure that we only have empty functions, and not a real one.
        // As we can have multiple times the same empty function prototype, count them.
        size_t cptEmpty = 0;
        for (auto n : funcNode->resolvedSymbolName->nodes)
        {
            if (!n->isEmptyFct())
                break;
            cptEmpty++;
        }

        if (cptEmpty == funcNode->resolvedSymbolName->nodes.size() && funcNode->resolvedSymbolName->cptOverloads == 1)
        {
            funcNode->resolvedSymbolName->kind = SymbolKind::PlaceHolder;
            return true;
        }
    }

    // For a short lambda without a specified return type, we need to defer the symbol registration, as we
    // need to infer it from the lambda expression
    uint32_t overFlags = 0;
    if (shortLambda || mustDeduceReturnType)
        overFlags |= OVERLOAD_INCOMPLETE;
    SWAG_CHECK(registerFuncSymbol(context, funcNode, overFlags));

    return true;
}

bool SemanticJob::registerFuncSymbol(SemanticContext* context, AstFuncDecl* funcNode, uint32_t symbolFlags)
{
    if (!(symbolFlags & OVERLOAD_INCOMPLETE))
    {
        SWAG_CHECK(checkFuncPrototype(context, funcNode));

        // Function inherits access from it's return type too (in case of type deduction)
        auto retType = funcNode->returnType->typeInfo;
        while (retType->isPointer())
            retType = CastTypeInfo<TypeInfoPointer>(retType, TypeInfoKind::Pointer)->pointedType;
        if (retType->declNode)
            doInheritAccess(funcNode, retType->declNode);

        // The function wants to return something, but has the 'Swag.CalleeReturn' attribute
        if (!funcNode->returnType->typeInfo->isVoid() && (funcNode->attributeFlags & ATTRIBUTE_CALLEE_RETURN))
        {
            Diagnostic diag{funcNode->returnType->childs.front(), Err(Err0766)};
            diag.hint = Nte(Nte1026);
            return context->report(diag);
        }
        // The function returns nothing but has the 'Swag.Discardable' attribute
        if (funcNode->returnType->typeInfo->isVoid() && funcNode->attributeFlags & ATTRIBUTE_DISCARDABLE)
            return context->report({funcNode, Fmt(Err(Err0767), funcNode->token.ctext())});
    }

    if (funcNode->flags & AST_IS_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = funcNode;
    toAdd.typeInfo = funcNode->typeInfo;
    toAdd.kind     = SymbolKind::Function;
    toAdd.flags    = symbolFlags;

    funcNode->resolvedSymbolOverload = funcNode->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    funcNode->resolvedSymbolName     = toAdd.symbolName;
    SWAG_CHECK(funcNode->resolvedSymbolOverload);

    // If the function returns a struct, register a type alias "retval". This way we can resolve an identifier
    // named retval for "var result: retval{xx, xxx}" syntax
    auto returnType = TypeManager::concreteType(funcNode->returnType->typeInfo, CONCRETE_FORCEALIAS);
    if (returnType->isStruct())
    {
        Utf8              retVal = g_LangSpec->name_retval;
        AddSymbolTypeInfo toAdd1;
        toAdd1.node      = funcNode->returnType;
        toAdd1.typeInfo  = returnType;
        toAdd1.kind      = SymbolKind::TypeAlias;
        toAdd1.flags     = symbolFlags | OVERLOAD_RETVAL;
        toAdd1.aliasName = &retVal;
        funcNode->scope->symTable.addSymbolTypeInfo(context, toAdd1);
    }

    // Register method
    if (!(symbolFlags & OVERLOAD_INCOMPLETE) && isMethod(funcNode))
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(funcNode->ownerStructScope->owner->typeInfo, TypeInfoKind::Struct);

        {
            ScopedLock lk(typeStruct->mutex);
            SWAG_ASSERT(funcNode->methodParam);
            auto typeFunc                     = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
            funcNode->methodParam->attributes = typeFunc->attributes;
        }

        context->job->decreaseMethodCount(funcNode, typeStruct);
    }

    resolveSubDecls(context, funcNode);
    return true;
}

bool SemanticJob::isMethod(AstFuncDecl* funcNode)
{
    if (funcNode->ownerStructScope &&
        funcNode->parent->kind != AstNodeKind::CompilerAst &&
        funcNode->parent->kind != AstNodeKind::CompilerRun &&
        funcNode->parent->kind != AstNodeKind::CompilerRunExpression &&
        funcNode->parent->kind != AstNodeKind::CompilerValidIf &&
        funcNode->parent->kind != AstNodeKind::CompilerValidIfx &&
        !(funcNode->flags & AST_FROM_GENERIC) &&
        !(funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC) &&
        (funcNode->ownerScope->kind == ScopeKind::Struct) &&
        (funcNode->ownerStructScope->owner->typeInfo->isStruct()))
    {
        return true;
    }

    return false;
}

void SemanticJob::launchResolveSubDecl(JobContext* context, AstNode* node)
{
    if (node->flags & (AST_SPEC_SEMANTIC1 | AST_SPEC_SEMANTIC2))
        return;

    // If SEMFLAG_FILE_JOB_PASS is set, then the file job has already seen the sub declaration, ignored it
    // because of AST_NO_SEMANTIC, but the attribute context is ok. So we need to trigger the job by hand.
    // If SEMFLAG_FILE_JOB_PASS is not set, then we just have to remove the AST_NO_SEMANTIC flag, and the
    // file job will trigger the resolve itself
    node->flags &= ~AST_NO_SEMANTIC;
    if (node->semFlags & SEMFLAG_FILE_JOB_PASS)
    {
        auto job          = Allocator::alloc<SemanticJob>();
        job->sourceFile   = context->sourceFile;
        job->module       = context->sourceFile->module;
        job->dependentJob = context->baseJob->dependentJob;
        job->nodes.push_back(node);
        g_ThreadMgr.addJob(job);
    }
}

void SemanticJob::resolveSubDecls(JobContext* context, AstFuncDecl* funcNode)
{
    if (!funcNode)
        return;

    // If we have sub declarations, then now we can solve them, except for a generic function.
    // Because for a generic function, the sub declarations will be cloned and solved after instanciation.
    // Otherwise, we can have a race condition by solving a generic sub declaration and cloning it for instancation
    // at the same time.
    if (!(funcNode->flags & AST_IS_GENERIC) && funcNode->content && !(funcNode->content->flags & AST_NO_SEMANTIC))
    {
        for (auto f : funcNode->subDecls)
        {
            ScopedLock lk(f->mutex);

            // Disabled by #if block
            if (f->semFlags & SEMFLAG_DISABLED)
                continue;
            f->semFlags |= SEMFLAG_DISABLED; // To avoid multiple resolutions

            if (f->hasExtOwner() && f->extOwner()->ownerCompilerIfBlock && f->extOwner()->ownerCompilerIfBlock->ownerFct == funcNode)
            {
                ScopedLock lk1(f->extOwner()->ownerCompilerIfBlock->mutex);
                f->extOwner()->ownerCompilerIfBlock->subDecls.push_back(f);
            }
            else
                launchResolveSubDecl(context, f);
        }
    }
}

bool SemanticJob::resolveCaptureFuncCallParams(SemanticContext* context)
{
    auto node = CastAst<AstFuncCallParams>(context->node, AstNodeKind::FuncCallParams);
    node->inheritOrFlag(AST_IS_GENERIC);
    node->inheritAndFlag1(AST_CONST_EXPR);

    // Check capture types
    for (auto c : node->childs)
    {
        auto typeField = c->typeInfo;

        if (typeField->isArray())
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeField, TypeInfoKind::Array);
            typeField      = typeArray->finalType;
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
            if (context->result == ContextResult::Pending)
                return true;
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeField, TypeInfoKind::Struct);
            if (!typeStruct->isPlainOldData())
                return context->report({c, Fmt(Err(Err0884), c->token.ctext())});
            continue;
        }

        if (typeField->isClosure())
            return context->report({c, Fmt(Err(Err0875), c->token.ctext())});
        return context->report({c, Fmt(Err(Err0887), c->token.ctext(), typeField->getDisplayNameC()), Diagnostic::isType(c->typeInfo)});
    }

    // As this is the capture block resolved in the right context, we can now evaluate the corresponding slosure
    auto mpl = CastAst<AstMakePointer>(node->parent, AstNodeKind::MakePointerLambda);
    SWAG_ASSERT(mpl->lambda);

    ScopedLock lk(mpl->lambda->mutex);
    if (mpl->lambda->flags & AST_SPEC_SEMANTIC1)
    {
        mpl->lambda->flags &= ~AST_SPEC_SEMANTIC1;
        launchResolveSubDecl(context, mpl->lambda);
    }

    return true;
}

bool SemanticJob::resolveFuncCallGenParams(SemanticContext* context)
{
    auto node = context->node;
    node->inheritOrFlag(AST_IS_GENERIC);
    node->inheritAndFlag1(AST_CONST_EXPR);

    if (node->flags & AST_IS_GENERIC)
        return true;

    for (auto c : node->childs)
    {
        if (c->hasComputedValue())
            continue;

        auto symbol = c->childs.front()->resolvedSymbolName;
        if (!symbol)
            continue;

        if (symbol->kind == SymbolKind::Variable ||
            symbol->kind == SymbolKind::Namespace ||
            symbol->kind == SymbolKind::Attribute)
        {
            auto note = Diagnostic::note(Nte(Nte0121));
            return context->report({c, Fmt(Err(Err0815), Naming::aKindName(symbol->kind).c_str(), symbol->name.c_str())}, note);
        }
    }

    return true;
}

bool SemanticJob::resolveFuncCallParams(SemanticContext* context)
{
    auto node = context->node;
    node->inheritOrFlag(AST_IS_GENERIC);
    node->inheritAndFlag1(AST_CONST_EXPR);
    return true;
}

bool SemanticJob::resolveFuncCallParam(SemanticContext* context)
{
    auto node      = CastAst<AstFuncCallParam>(context->node, AstNodeKind::FuncCallParam);
    auto child     = node->childs.front();
    node->typeInfo = child->typeInfo;

    SWAG_VERIFY(!node->typeInfo->isCVariadic(), context->report({node, Err(Err0446)}));

    // Force const if necessary
    // func([.., ...]) must be const
    if (child->kind == AstNodeKind::ExpressionList)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
        if (typeList->isListArray())
            node->typeInfo->setConst();
    }

    node->byteCodeFct = ByteCodeGenJob::emitFuncCallParam;

    // Can be called for generic parameters in type definition, in that case, we are a type, so no
    // test for concrete must be done
    bool checkForConcrete = true;
    if (node->parent->flags & AST_NO_BYTECODE)
        checkForConcrete = false;
    if (checkForConcrete)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, child));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = child->typeInfo;
        node->flags |= AST_R_VALUE;
    }

    node->inheritComputedValue(child);
    node->inheritOrFlag(child, AST_CONST_EXPR | AST_IS_GENERIC | AST_VALUE_IS_GENTYPEINFO | AST_OPAFFECT_CAST | AST_TRANSIENT);
    if (node->childs.front()->semFlags & SEMFLAG_LITERAL_SUFFIX)
        node->semFlags |= SEMFLAG_LITERAL_SUFFIX;

    // Inherit the original type in case of computed values, in order to make the cast if necessary
    if (node->flags & (AST_VALUE_COMPUTED | AST_OPAFFECT_CAST))
        node->castedTypeInfo = child->castedTypeInfo;

    if (checkForConcrete & !(node->flags & AST_OPAFFECT_CAST))
    {
        SWAG_CHECK(evaluateConstExpression(context, node));
        if (context->result == ContextResult::Pending)
            return true;
    }

    node->resolvedSymbolName     = child->resolvedSymbolName;
    node->resolvedSymbolOverload = child->resolvedSymbolOverload;

    if (child->hasExtMisc() && child->extMisc()->resolvedUserOpSymbolOverload)
    {
        node->allocateExtension(ExtensionKind::Misc);
        node->extMisc()->resolvedUserOpSymbolOverload = child->extMisc()->resolvedUserOpSymbolOverload;
    }

    // If the call has been generated because of a 'return tuple', then we force a move
    // instead of a copy, in case the parameter to the tuple init is a local variable
    if (node->autoTupleReturn)
    {
        if (node->resolvedSymbolOverload && (node->resolvedSymbolOverload->flags & OVERLOAD_VAR_LOCAL))
        {
            node->flags |= AST_FORCE_MOVE | AST_NO_RIGHT_DROP;
            node->autoTupleReturn->forceNoDrop.push_back(child->resolvedSymbolOverload);
        }
    }

    return true;
}

bool SemanticJob::resolveRetVal(SemanticContext* context)
{
    auto node    = context->node;
    auto fctDecl = node->ownerInline ? node->ownerInline->func : node->ownerFct;

    SWAG_VERIFY(fctDecl, context->report({node, Err(Err0769)}));
    SWAG_VERIFY(node->ownerScope && node->ownerScope->kind != ScopeKind::Function, context->report({node, Err(Err0769)}));

    auto fct     = CastAst<AstFuncDecl>(fctDecl, AstNodeKind::FuncDecl);
    auto typeFct = CastTypeInfo<TypeInfoFuncAttr>(fct->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_VERIFY(typeFct->returnType && !typeFct->returnType->isVoid(), context->report({node, Err(Err0771)}));

    // :WaitForPOD
    if (typeFct->returnType->isStruct())
    {
        context->job->waitStructGenerated(typeFct->returnType);
        if (context->result != ContextResult::Done)
            return true;
    }

    // If this is a simple return type, remove the retval stuff.
    // Variable will behaves normally, in the stack
    if (!CallConv::returnByStackAddress(typeFct))
    {
        auto parentNode = node;
        if (parentNode->kind == AstNodeKind::Identifier)
            parentNode = parentNode->findParent(AstNodeKind::TypeExpression);
        auto typeExpr = CastAst<AstTypeExpression>(parentNode, AstNodeKind::TypeExpression);
        typeExpr->typeFlags &= ~TYPEFLAG_IS_RETVAL;
    }

    node->typeInfo = typeFct->returnType;
    return true;
}

void SemanticJob::propagateReturn(AstNode* node)
{
    auto stopFct = node->ownerFct ? node->ownerFct->parent : nullptr;
    if (node->semFlags & SEMFLAG_EMBEDDED_RETURN)
        stopFct = node->ownerInline->parent;
    SWAG_ASSERT(stopFct);

    AstNode* scanNode = node;

    // Search if we are in an infinit loop
    auto breakable = node->ownerBreakable;
    while (breakable)
    {
        if (breakable->kind == AstNodeKind::Loop)
        {
            auto loopNode = CastAst<AstLoop>(breakable, AstNodeKind::Loop);
            if (!loopNode->expression)
                loopNode->breakableFlags |= BREAKABLE_RETURN_IN_INFINIT_LOOP;
            break;
        }

        if (breakable->kind == AstNodeKind::While)
        {
            auto whileNode = CastAst<AstWhile>(breakable, AstNodeKind::While);
            if ((whileNode->boolExpression->hasComputedValue()) && (whileNode->boolExpression->computedValue->reg.b))
                whileNode->breakableFlags |= BREAKABLE_RETURN_IN_INFINIT_LOOP;
            break;
        }

        if (breakable->kind == AstNodeKind::For)
        {
            auto forNode = CastAst<AstFor>(breakable, AstNodeKind::For);
            if ((forNode->boolExpression->hasComputedValue()) && (forNode->boolExpression->computedValue->reg.b))
                forNode->breakableFlags |= BREAKABLE_RETURN_IN_INFINIT_LOOP;
            break;
        }

        breakable = breakable->ownerBreakable;
    }

    // Propage the return in the corresponding scope
    while (scanNode && scanNode != stopFct)
    {
        if (scanNode->semFlags & SEMFLAG_SCOPE_HAS_RETURN && !(scanNode->semFlags & SEMFLAG_SCOPE_FORCE_HAS_RETURN))
            break;
        scanNode->semFlags |= SEMFLAG_SCOPE_HAS_RETURN;
        if (scanNode->parent && scanNode->parent->kind == AstNodeKind::If)
        {
            auto ifNode = CastAst<AstIf>(scanNode->parent, AstNodeKind::If);
            if (ifNode->elseBlock != scanNode)
                break;
            if (!(ifNode->ifBlock->semFlags & SEMFLAG_SCOPE_HAS_RETURN))
                break;
        }
        else if (scanNode->kind == AstNodeKind::SwitchCase)
        {
            auto sc = CastAst<AstSwitchCase>(scanNode, AstNodeKind::SwitchCase);
            if (sc->specFlags & AstSwitchCase::SPECFLAG_IS_DEFAULT)
                sc->ownerSwitch->semFlags |= SEMFLAG_SCOPE_FORCE_HAS_RETURN;
        }
        else if (scanNode->kind == AstNodeKind::Switch)
        {
            if (!(scanNode->semFlags & SEMFLAG_SCOPE_FORCE_HAS_RETURN))
                break;
        }
        else if (scanNode->kind == AstNodeKind::While ||
                 scanNode->kind == AstNodeKind::Loop ||
                 scanNode->kind == AstNodeKind::For)
        {
            break;
        }

        scanNode = scanNode->parent;
    }

    // To tell that the function as at least one return (this will change the error message)
    while (scanNode && scanNode != stopFct)
    {
        if (scanNode->semFlags & SEMFLAG_FCT_HAS_RETURN)
            break;
        scanNode->semFlags |= SEMFLAG_FCT_HAS_RETURN;
        scanNode = scanNode->parent;
    }
}

AstFuncDecl* SemanticJob::getFunctionForReturn(AstNode* node)
{
    // For a return inside an inline block, take the inlined function, except for a mixin or
    // if the inlined function is flagged with 'Swag.CalleeReturn' (in that case we take the owner function)
    auto funcNode = node->ownerFct;
    if (node->ownerInline && node->ownerInline->isParentOf(node))
    {
        if (!(node->ownerInline->func->attributeFlags & ATTRIBUTE_CALLEE_RETURN) && !(node->flags & AST_IN_MIXIN))
        {
            if (node->kind == AstNodeKind::Return)
                node->semFlags |= SEMFLAG_EMBEDDED_RETURN;
            funcNode = node->ownerInline->func;
        }
    }

    return funcNode;
}

bool SemanticJob::resolveReturn(SemanticContext* context)
{
    SWAG_CHECK(warnUnreachableCode(context));

    auto node     = CastAst<AstReturn>(context->node, AstNodeKind::Return);
    auto funcNode = getFunctionForReturn(node);

    node->byteCodeFct      = ByteCodeGenJob::emitReturn;
    node->resolvedFuncDecl = funcNode;
    auto funcReturnType    = TypeManager::concreteType(funcNode->returnType->typeInfo);

    // As the type of the function is deduced from the return type, be sure they match in case
    // of multiple returns
    if (funcNode->returnTypeDeducedNode)
    {
        // We return nothing, but the previous return had something
        if (node->childs.empty())
        {
            if (!funcReturnType->isVoid())
            {
                Diagnostic diag{node, Fmt(Err(Err0779), funcReturnType->getDisplayNameC())};
                auto       note = Diagnostic::note(funcNode->returnTypeDeducedNode->childs.front(), Nte(Nte0063));
                return context->report(diag, note);
            }

            return true;
        }

        auto child     = node->childs[0];
        auto childType = TypeManager::concreteType(child->typeInfo);

        // We try to return something, but the previous return had nothing
        if (funcReturnType->isVoid() && !childType->isVoid())
        {
            Diagnostic diag{child, Fmt(Err(Err0773), childType->getDisplayNameC())};
            auto       note = Diagnostic::note(funcNode->returnTypeDeducedNode, Nte(Nte0063));
            return context->report(diag, note);
        }

        uint64_t castFlags = CASTFLAG_JUST_CHECK | CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST | CASTFLAG_TRY_COERCE | CASTFLAG_FOR_AFFECT;
        if (!TypeManager::makeCompatibles(context, funcNode->returnType->typeInfo, nullptr, child, castFlags))
        {
            Diagnostic diag{child, Fmt(Err(Err0770), funcNode->returnType->typeInfo->getDisplayNameC(), child->typeInfo->getDisplayNameC())};
            auto       note = Diagnostic::note(funcNode->returnTypeDeducedNode->childs.front(), Nte(Nte0063));
            return context->report(diag, note);
        }
    }

    // Nothing to return
    if (funcNode->returnType->typeInfo->isVoid() && node->childs.empty())
    {
        if (funcNode->attributeFlags & ATTRIBUTE_RUN_GENERATED_EXP)
        {
            funcNode->returnType->typeInfo  = g_TypeMgr->typeInfoVoid;
            funcNode->returnTypeDeducedNode = node;
        }

        return true;
    }

    // Deduce return type
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(funcNode->typeInfo, TypeInfoKind::FuncAttr);
    bool lateRegister = funcNode->specFlags & AstFuncDecl::SPECFLAG_FORCE_LATE_REGISTER;
    if (funcReturnType->isVoid() && !(funcNode->specFlags & AstFuncDecl::SPECFLAG_LATE_REGISTER_DONE))
    {
        // This is a short lambda without a specified return type. We now have it
        bool tryDeduce = false;
        if ((funcNode->specFlags & AstFuncDecl::SPECFLAG_SHORT_LAMBDA) && !(funcNode->returnType->specFlags & AstFuncDecl::SPECFLAG_RETURN_DEFINED))
            tryDeduce = true;
        if (funcNode->attributeFlags & ATTRIBUTE_RUN_GENERATED_EXP)
            tryDeduce = true;
        if (tryDeduce)
        {
            funcNode->specFlags |= AstFuncDecl::SPECFLAG_FORCE_LATE_REGISTER;
            typeInfoFunc->returnType = TypeManager::concreteType(node->childs.front()->typeInfo, CONCRETE_FUNC);
            typeInfoFunc->returnType = TypeManager::promoteUntyped(typeInfoFunc->returnType);
            auto concreteReturn      = TypeManager::concreteType(typeInfoFunc->returnType);
            if (concreteReturn->isListTuple())
            {
                SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, funcNode->content, node->childs.front(), &funcNode->returnType));
                Ast::setForceConstType(funcNode->returnType);
                context->job->nodes.push_back(funcNode->returnType);
                context->result = ContextResult::NewChilds;
                return true;
            }

            typeInfoFunc->forceComputeName();
            funcNode->returnType->typeInfo  = typeInfoFunc->returnType;
            funcNode->returnTypeDeducedNode = node;
            funcNode->specFlags |= AstFuncDecl::SPECFLAG_LATE_REGISTER_DONE;
            lateRegister = true;
        }
    }

    if (node->childs.empty())
    {
        Diagnostic diag{node, Fmt(Err(Err0772), funcNode->returnType->typeInfo->getDisplayNameC())};
        auto       note = Diagnostic::note(funcNode->returnType->childs.front(), Fmt(Nte(Nte0067), typeInfoFunc->returnType->getDisplayNameC()));
        return context->report(diag, note);
    }

    auto returnType = funcNode->returnType->typeInfo;

    // Check types
    auto child = node->childs[0];
    SWAG_CHECK(checkIsConcreteOrType(context, child));
    if (context->result != ContextResult::Done)
        return true;

    auto concreteType = TypeManager::concreteType(child->typeInfo);

    // No return value in a #run block
    if (!concreteType->isVoid())
    {
        if (funcNode->attributeFlags & (ATTRIBUTE_RUN_FUNC |
                                        ATTRIBUTE_RUN_GENERATED_FUNC |
                                        ATTRIBUTE_MAIN_FUNC |
                                        ATTRIBUTE_INIT_FUNC |
                                        ATTRIBUTE_DROP_FUNC |
                                        ATTRIBUTE_PREMAIN_FUNC |
                                        ATTRIBUTE_TEST_FUNC))
        {
            if (funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC)
                return context->report({child, Fmt(Err(Err0052), funcNode->getDisplayNameC()), Nte(Nte1026)});
        }
    }

    // Be sure we do not specify a return value, and the function does not have a return type
    // (better error message than just letting the makeCompatibles do its job)
    if (returnType->isVoid() && !concreteType->isVoid())
    {
        Diagnostic diag{child, Fmt(Err(Err0774), concreteType->getDisplayNameC(), funcNode->getDisplayNameC())};
        diag.hint = Nte(Nte1026);

        if (node->ownerInline && !(node->semFlags & SEMFLAG_EMBEDDED_RETURN))
        {
            auto note = Diagnostic::note(funcNode, Fmt(Nte(Nte0011), node->ownerInline->func->getDisplayNameC(), funcNode->getDisplayNameC()));
            return context->report(diag, note);
        }

        return context->report(diag);
    }

    // :WaitForPOD
    if (returnType && returnType->isStruct())
    {
        context->job->waitAllStructSpecialMethods(returnType);
        if (context->result != ContextResult::Done)
            return true;
    }

    // If returning retval, then returning nothing, as we will change the return parameter value in place
    if (child->resolvedSymbolOverload && child->resolvedSymbolOverload->flags & OVERLOAD_RETVAL)
    {
        node->typeInfo = child->typeInfo;
    }
    else
    {
        // If we are returning an interface, be sure they are defined before casting
        if (returnType->isInterface())
        {
            context->job->waitAllStructInterfaces(child->typeInfo);
            if (context->result != ContextResult::Done)
                return true;
        }

        uint64_t castFlags = CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST | CASTFLAG_TRY_COERCE | CASTFLAG_FOR_AFFECT | CASTFLAG_PTR_REF;

        if (funcNode->attributeFlags & ATTRIBUTE_AST_FUNC)
        {
            PushErrCxtStep ec{context, funcNode, ErrCxtStepKind::Note, []()
                              { return Nte(Nte0005); },
                              true};
            SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, castFlags));
        }
        else if (funcNode->attributeFlags & ATTRIBUTE_SHARP_FUNC)
        {
            PushErrCxtStep ec{context, funcNode, ErrCxtStepKind::Note, [returnType]()
                              { return Fmt(Nte(Nte0067), returnType->getDisplayNameC()); },
                              true};
            SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, castFlags));
        }
        else
        {
            auto nodeErr = funcNode->returnType;
            if (nodeErr->kind == AstNodeKind::FuncDeclType && !funcNode->returnType->childs.empty())
                nodeErr = funcNode->returnType->childs.front();
            PushErrCxtStep ec{context, nodeErr, ErrCxtStepKind::Note, [returnType]()
                              { return Fmt(Nte(Nte0067), returnType->getDisplayNameC()); }};
            SWAG_CHECK(TypeManager::makeCompatibles(context, returnType, nullptr, child, castFlags));
        }
    }

    if (child->kind == AstNodeKind::ExpressionList)
    {
        child->flags |= AST_TRANSIENT;
    }

    // When returning a struct, we need to know if postcopy or postmove are here, and wait for them to resolve
    if (returnType && (returnType->isStruct() || returnType->isArrayOfStruct()))
    {
        SWAG_CHECK(waitForStructUserOps(context, funcNode->returnType));
        if (context->result == ContextResult::Pending)
            return true;
    }

    // :opAffectParam
    if (child->hasExtMisc() && child->extMisc()->resolvedUserOpSymbolOverload)
    {
        auto overload = child->extMisc()->resolvedUserOpSymbolOverload;
        if (overload->symbol->name == g_LangSpec->name_opAffect || overload->symbol->name == g_LangSpec->name_opAffectSuffix)
        {
            SWAG_ASSERT(child->castedTypeInfo);
            child->extMisc()->resolvedUserOpSymbolOverload = nullptr;
            child->castedTypeInfo                          = nullptr;

            auto varNode = Ast::newVarDecl(context->sourceFile, Fmt("__2tmp_%d", g_UniqueID.fetch_add(1)), node);
            varNode->inheritTokenLocation(child);

            auto typeExpr      = Ast::newTypeExpression(context->sourceFile, varNode);
            typeExpr->typeInfo = child->typeInfo;
            typeExpr->flags |= AST_NO_SEMANTIC;
            varNode->type = typeExpr;

            CloneContext cloneContext;
            cloneContext.parent      = varNode;
            cloneContext.parentScope = child->ownerScope;
            varNode->assignment      = child->clone(cloneContext);

            Ast::removeFromParent(child);

            Ast::removeFromParent(varNode);
            auto idRef = Ast::newIdentifierRef(context->sourceFile, varNode->token.text, node);
            Ast::addChildBack(node, varNode);

            idRef->allocateExtension(ExtensionKind::Misc);
            idRef->extMisc()->exportNode = child;
            idRef->allocateExtension(ExtensionKind::Owner);
            idRef->extOwner()->nodesToFree.push_back(child);

            context->job->nodes.push_back(node->childs.front());
            context->job->nodes.push_back(varNode);
            varNode->semFlags |= SEMFLAG_ONCE;
            context->result = ContextResult::NewChilds;
            return true;
        }
    }

    // If we are returning a local variable, we can do a move
    if (child->resolvedSymbolOverload && (child->resolvedSymbolOverload->flags & OVERLOAD_VAR_LOCAL))
    {
        child->flags |= AST_FORCE_MOVE | AST_NO_RIGHT_DROP;
        node->forceNoDrop.push_back(child->resolvedSymbolOverload);
    }

    if (child->resolvedSymbolOverload && (child->resolvedSymbolOverload->flags & OVERLOAD_RETVAL))
    {
        child->flags |= AST_NO_BYTECODE;
        node->forceNoDrop.push_back(child->resolvedSymbolOverload);
    }

    // Propagate return so that we can detect if some paths are missing one
    propagateReturn(node);

    // Register symbol now that we have inferred the return type
    if (lateRegister)
    {
        funcNode->specFlags &= ~AstFuncDecl::SPECFLAG_FORCE_LATE_REGISTER;
        typeInfoFunc->returnType = funcNode->returnType->typeInfo;
        typeInfoFunc->forceComputeName();
        SWAG_CHECK(registerFuncSymbol(context, funcNode));
    }

    return true;
}

uint32_t SemanticJob::getMaxStackSize(AstNode* node)
{
    auto decSP = node->ownerScope->startStackSize;

    if (node->flags & AST_SPEC_STACKSIZE)
    {
        auto p = node;
        while (p->parent && p->parent->kind != AstNodeKind::File)
            p = p->parent;
        SWAG_ASSERT(p);
        ScopedLock mk(p->mutex);
        p->allocateExtensionNoLock(ExtensionKind::Misc);
        decSP = max(decSP, p->extMisc()->stackSize);
        return decSP;
    }

    if (node->ownerFct)
        decSP = max(decSP, node->ownerFct->stackSize);
    return decSP;
}

void SemanticJob::setOwnerMaxStackSize(AstNode* node, uint32_t size)
{
    size = max(size, 1);
    size = (uint32_t) TypeManager::align(size, sizeof(void*));

    if (node->flags & AST_SPEC_STACKSIZE)
    {
        auto p = node;
        while (p->parent && p->parent->kind != AstNodeKind::File)
            p = p->parent;
        SWAG_ASSERT(p);
        ScopedLock mk(p->mutex);
        p->allocateExtensionNoLock(ExtensionKind::Misc);
        p->extMisc()->stackSize = max(p->extMisc()->stackSize, size);
    }
    else if (node->ownerFct)
    {
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, size);
    }
}

bool SemanticJob::makeInline(JobContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    CloneContext cloneContext;

    // Be sure this is not recursive
    auto ownerInline = identifier->ownerInline;
    while (ownerInline)
    {
        if (ownerInline->func == funcDecl)
            return context->report({identifier, identifier->token, Fmt(Err(Err0775), identifier->token.ctext())});
        ownerInline = ownerInline->ownerInline;
    }

    // The content will be inline in its separated syntax block
    auto inlineNode            = Ast::newInline(identifier->sourceFile, identifier);
    inlineNode->attributeFlags = funcDecl->attributeFlags;
    inlineNode->func           = funcDecl;
    inlineNode->scope          = identifier->ownerScope;
    inlineNode->typeInfo       = TypeManager::concreteType(funcDecl->typeInfo);

    if (identifier->hasExtOwner() && identifier->extOwner()->ownerTryCatchAssume)
    {
        inlineNode->allocateExtension(ExtensionKind::Owner);
        inlineNode->extOwner()->ownerTryCatchAssume = identifier->extOwner()->ownerTryCatchAssume;
    }

    if (funcDecl->hasExtMisc())
    {
        SharedLock lk1(funcDecl->extMisc()->mutexAltScopes);
        if (funcDecl->extMisc()->alternativeScopes.size() || funcDecl->extMisc()->alternativeScopesVars.size())
        {
            inlineNode->allocateExtension(ExtensionKind::Misc);
            ScopedLock lk(inlineNode->extMisc()->mutexAltScopes);
            inlineNode->extMisc()->alternativeScopes     = funcDecl->extMisc()->alternativeScopes;
            inlineNode->extMisc()->alternativeScopesVars = funcDecl->extMisc()->alternativeScopesVars;
        }
    }

    // Try/Assume
    if (inlineNode->hasExtOwner() &&
        inlineNode->extOwner()->ownerTryCatchAssume &&
        (inlineNode->func->typeInfo->flags & TYPEINFO_CAN_THROW))
    {
        switch (inlineNode->extOwner()->ownerTryCatchAssume->kind)
        {
        case AstNodeKind::Try:
            inlineNode->setBcNotifAfter(ByteCodeGenJob::emitTry);
            break;
        case AstNodeKind::TryCatch:
            inlineNode->setBcNotifAfter(ByteCodeGenJob::emitTryCatch);
            break;
        case AstNodeKind::Assume:
            inlineNode->setBcNotifAfter(ByteCodeGenJob::emitAssume);
            break;
        default:
            break;
        }

        // Reset emit from the modifier if it exists, as the inline block will deal with that
        if (identifier->hasExtByteCode())
        {
            auto extension = identifier->extByteCode();
            if (extension->byteCodeAfterFct == ByteCodeGenJob::emitTry)
                extension->byteCodeAfterFct = nullptr;
            else if (extension->byteCodeAfterFct == ByteCodeGenJob::emitTryCatch)
                extension->byteCodeAfterFct = nullptr;
            else if (extension->byteCodeAfterFct == ByteCodeGenJob::emitAssume)
                extension->byteCodeAfterFct = nullptr;
        }
    }

    // We need to add the parent scope of the inline function (the global one), in order for
    // the inline content to be resolved in the same context as the original function
    auto globalScope = funcDecl->ownerScope;
    while (!globalScope->isGlobalOrImpl())
        globalScope = globalScope->parentScope;
    inlineNode->addAlternativeScope(globalScope);

    // We also need to add all alternatives scopes (using), in case the function relies on them to
    // be solved
    AstNode* parentNode = funcDecl;
    while (parentNode)
    {
        if (parentNode->hasExtMisc() && !parentNode->extMisc()->alternativeScopes.empty())
            inlineNode->addAlternativeScopes(parentNode->extMisc()->alternativeScopes);
        parentNode = parentNode->parent;
    }

    // If function has generic parameters, then the block resolution of identifiers needs to be able to find the generic parameters
    // so we register all those generic parameters in a special scope (we cannot just register the scope of the function because
    // they are other stuff here)
    if (funcDecl->genericParameters)
    {
        Scope* scope = Ast::newScope(inlineNode, "", ScopeKind::Statement, nullptr);
        inlineNode->addAlternativeScope(scope);
        for (auto child : funcDecl->genericParameters->childs)
        {
            auto symName = scope->symTable.registerSymbolNameNoLock(context, child, SymbolKind::Variable);
            symName->addOverloadNoLock(child, child->typeInfo, &child->resolvedSymbolOverload->computedValue);
            symName->cptOverloads = 0; // Simulate a done resolution
        }
    }

    // A mixin behave exactly like if it is in the caller scope, so do not create a subscope for them
    Scope* newScope = identifier->ownerScope;
    if (!(funcDecl->attributeFlags & ATTRIBUTE_MIXIN))
    {
        newScope          = Ast::newScope(inlineNode, Fmt("__inline%d", identifier->ownerScope->childScopes.size()), ScopeKind::Inline, identifier->ownerScope);
        inlineNode->scope = newScope;
    }

    // Create a scope that will contain all parameters.
    // Parameters are specific to each inline block, that's why we create it even for mixins
    inlineNode->parametersScope = Ast::newScope(inlineNode, "", ScopeKind::Statement, nullptr);

    // Clone the function body
    cloneContext.parent         = inlineNode;
    cloneContext.ownerInline    = inlineNode;
    cloneContext.ownerFct       = identifier->ownerFct;
    cloneContext.ownerBreakable = identifier->ownerBreakable;
    cloneContext.parentScope    = newScope;
    cloneContext.forceFlags |= identifier->flags & AST_NO_BACKEND;
    cloneContext.forceFlags |= identifier->flags & AST_RUN_BLOCK;
    cloneContext.forceFlags |= identifier->flags & AST_IN_DEFER;
    cloneContext.removeFlags |= AST_R_VALUE;
    cloneContext.cloneFlags |= CLONE_FORCE_OWNER_FCT;

    // Here we inline a call in a global declaration, like a variable/constant initialization
    // We do not want the function to be the original one, in case of local variables, because we
    // do not want to change the stackSize of the original function because of local variables.
    if (!cloneContext.ownerFct)
    {
        identifier->flags |= AST_SPEC_STACKSIZE;
        if (identifier->kind == AstNodeKind::Identifier)
            identifier->parent->flags |= AST_SPEC_STACKSIZE;
        cloneContext.forceFlags |= AST_SPEC_STACKSIZE;
    }

    // Register all aliases
    if (identifier->kind == AstNodeKind::Identifier)
    {
        // Replace user aliases of the form @alias?
        // Can come from the identifier itself (for visit) or from call parameters (for macros/mixins)
        auto id = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);

        int idx = 0;
        if (id->identifierExtension)
        {
            for (auto& alias : id->identifierExtension->aliasNames)
                cloneContext.replaceNames[Fmt("@alias%d", idx++)] = alias.text;
        }

        idx = 0;
        for (auto& alias : id->callParameters->aliasNames)
            cloneContext.replaceNames[Fmt("@alias%d", idx++)] = alias.text;

        // Replace user @mixin
        if (funcDecl->specFlags & AstFuncDecl::SPECFLAG_SPEC_MIXIN)
        {
            for (int i = 0; i < 10; i++)
                cloneContext.replaceNames[Fmt("@mixin%d", i)] = Fmt("__mixin%d", g_UniqueID.fetch_add(1));
        }

        for (auto child : id->callParameters->childs)
        {
            auto param = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            if (!param->resolvedParameter)
                continue;

            // Transmit code type
            if (param->typeInfo->isCode())
            {
                AddSymbolTypeInfo toAdd;
                toAdd.node      = param;
                toAdd.typeInfo  = param->typeInfo;
                toAdd.kind      = SymbolKind::Variable;
                toAdd.aliasName = &param->resolvedParameter->name;

                inlineNode->parametersScope->symTable.addSymbolTypeInfo(context, toAdd);
            }
        }
    }

    // Clone !
    auto newContent = funcDecl->content->clone(cloneContext);

    if (newContent->hasExtByteCode())
    {
        newContent->extByteCode()->byteCodeBeforeFct = nullptr;
        if (funcDecl->attributeFlags & ATTRIBUTE_MIXIN)
            newContent->extByteCode()->byteCodeAfterFct = nullptr; // Do not release the scope, as there's no specific scope
    }

    if (newContent->kind == AstNodeKind::Try || newContent->kind == AstNodeKind::TryCatch || newContent->kind == AstNodeKind::Assume)
    {
        if (funcDecl->attributeFlags & ATTRIBUTE_MIXIN && newContent->childs.front()->extension)
        {
            auto front = newContent->childs.front();
            if (front->hasExtByteCode())
                front->extByteCode()->byteCodeAfterFct = nullptr; // Do not release the scope, as there's no specific scope
        }
    }

    newContent->flags &= ~AST_NO_SEMANTIC;

    // Sub declarations in the inline block, like sub functions
    if (!funcDecl->subDecls.empty())
    {
        PushErrCxtStep ec(context, identifier, ErrCxtStepKind::Inline, nullptr);
        SWAG_VERIFY(inlineNode->ownerFct, context->report({funcDecl, Fmt(Err(Err0781), identifier->token.ctext())}));

        // Authorize a sub function to access inline parameters, if possible
        // This will work for compile time values, otherwise we will have an out of stack frame when generating the code
        cloneContext.alternativeScope = inlineNode->parametersScope;

        SWAG_CHECK(funcDecl->cloneSubDecls(context, cloneContext, funcDecl->content, inlineNode->ownerFct, newContent));
    }

    // Need to reevaluate the identifier (if this is an identifier) because the makeInline can be called
    // for something else, like a loop node for example (opCount). In that case, we let the specific node
    // deal with the (re)evaluation.
    if (identifier->kind == AstNodeKind::Identifier)
    {
        // Do not reevaluate function parameters
        auto castId = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);
        if (castId->callParameters)
            castId->callParameters->flags |= AST_NO_SEMANTIC;

        identifier->semanticState = AstNodeResolveState::Enter;
        identifier->bytecodeState = AstNodeResolveState::Enter;
    }

    // Check used aliases
    // Error if an alias has been defined, but not 'eaten' by the function
    if (identifier->kind == AstNodeKind::Identifier)
    {
        if (cloneContext.replaceNames.size() != cloneContext.usedReplaceNames.size())
        {
            PushErrCxtStep ec(context, identifier, ErrCxtStepKind::Inline, nullptr);
            auto           id = CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier);
            for (auto& r : cloneContext.replaceNames)
            {
                auto it = cloneContext.usedReplaceNames.find(r.second);
                if (it == cloneContext.usedReplaceNames.end())
                {
                    if (id->identifierExtension)
                    {
                        for (auto& alias : id->identifierExtension->aliasNames)
                        {
                            if (alias.text == r.second)
                            {
                                Diagnostic diag{id, alias, Fmt(Err(Err0780), alias.ctext())};
                                diag.hint = Nte(Nte1026);
                                return context->report(diag);
                            }
                        }
                    }

                    for (auto& alias : id->callParameters->aliasNames)
                    {
                        if (alias.text == r.second)
                        {
                            Diagnostic diag{id, alias, Fmt(Err(Err0780), alias.ctext())};
                            diag.hint = Nte(Nte1026);
                            return context->report(diag);
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool SemanticJob::makeInline(SemanticContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    SWAG_CHECK(makeInline((JobContext*) context, funcDecl, identifier));
    context->result = ContextResult::NewChilds;
    return true;
}