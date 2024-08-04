#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

uint32_t Semantic::alignOf(const AstVarDecl* node)
{
    const auto value = node->attributes.getValue(g_LangSpec->name_Swag_Align, g_LangSpec->name_value);
    if (value)
        return value->reg.u8;
    return TypeManager::alignOf(TypeManager::concreteType(node->typeInfo));
}

// Will be called after solving the initial var affect, but before tuple unpacking
bool Semantic::resolveTupleUnpackBeforeVar(SemanticContext* context)
{
    SWAG_CHECK(resolveVarDeclAfter(context));
    SWAG_CHECK(resolveTupleUnpackBefore(context));
    return true;
}

// Will be called after solving the initial var affect, but before tuple unpacking
bool Semantic::resolveTupleUnpackBefore(SemanticContext* context)
{
    const auto scopeNode = castAst<AstNode>(context->node->parent, AstNodeKind::StatementNoScope, AstNodeKind::Statement);
    const auto varDecl   = castAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);

    auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
    if (typeVar->isListTuple() && !varDecl->type)
    {
        varDecl->addSemFlag(SEMFLAG_TUPLE_CONVERT);
        SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, varDecl, varDecl->assignment, &varDecl->type));
        context->result = ContextResult::NewChildren;
        context->baseJob->nodes.push_back(varDecl->type);
        return true;
    }

    if (varDecl->hasSemFlag(SEMFLAG_TUPLE_CONVERT))
    {
        SWAG_ASSERT(varDecl->resolvedSymbolOverload());
        varDecl->typeInfo                           = varDecl->type->typeInfo;
        varDecl->resolvedSymbolOverload()->typeInfo = varDecl->type->typeInfo;
        typeVar                                     = varDecl->typeInfo;
    }

    if (!typeVar->isStruct())
    {
        Diagnostic err{varDecl, varDecl->token, formErr(Err0379, typeVar->getDisplayNameC())};
        if (varDecl->assignment)
            err.addNote(varDecl->assignment, Diagnostic::isType(TypeManager::concreteType(varDecl->assignment->typeInfo)));
        else if (varDecl->type)
            err.addNote(varDecl->type, Diagnostic::isType(TypeManager::concreteType(varDecl->type->typeInfo)));
        return context->report(err);
    }

    const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
    const auto numUnpack  = scopeNode->childCount() - 1;

    if (typeStruct->fields.empty())
    {
        const Diagnostic err{varDecl, varDecl->token, toErr(Err0378)};
        return context->report(err);
    }

    if (numUnpack < typeStruct->fields.size())
    {
        Diagnostic err{varDecl, varDecl->token, formErr(Err0720, numUnpack, typeStruct->fields.size())};
        err.addNote(varDecl->assignment, formNte(Nte0189, typeStruct->fields.size()));
        err.addNote(toNte(Nte0040));
        return context->report(err);
    }

    if (numUnpack > typeStruct->fields.size())
    {
        Diagnostic err{varDecl, varDecl->token, formErr(Err0721, numUnpack, typeStruct->fields.size())};
        err.addNote(varDecl->assignment, formNte(Nte0189, typeStruct->fields.size()));
        return context->report(err);
    }

    return true;
}

void Semantic::setVarDeclResolve(AstVarDecl* varNode)
{
    varNode->allocateExtension(ExtensionKind::Semantic);
    varNode->extSemantic()->semanticBeforeFct = resolveVarDeclBefore;
    varNode->extSemantic()->semanticAfterFct  = resolveVarDeclAfter;

    if (varNode->assignment)
    {
        varNode->assignment->allocateExtension(ExtensionKind::Semantic);
        varNode->assignment->extSemantic()->semanticAfterFct = resolveVarDeclAfterAssign;
    }

    if (varNode->assignment && varNode->type)
    {
        varNode->type->allocateExtension(ExtensionKind::Semantic);
        varNode->type->extSemantic()->semanticAfterFct = resolveVarDeclAfterType;
    }
}

bool Semantic::resolveVarDeclAfterType(SemanticContext* context)
{
    // :DeduceLambdaType
    resolveAfterKnownType(context);

    auto parent = context->node->parent;
    while (parent && parent->isNot(AstNodeKind::VarDecl) && parent->isNot(AstNodeKind::ConstDecl) && parent->isNot(AstNodeKind::FuncDeclParam))
        parent = parent->parent;
    SWAG_ASSERT(parent);
    const auto varDecl = castAst<AstVarDecl>(parent);
    if (!varDecl->type || !varDecl->assignment)
        return true;

    if (parent->is(AstNodeKind::FuncDeclParam))
    {
        if (varDecl->type->typeInfo->isTypedVariadic() ||
            varDecl->type->typeInfo->isVariadic() ||
            varDecl->type->typeInfo->isCVariadic())
        {
            const Diagnostic err{varDecl, varDecl->assignToken, toErr(Err0672)};
            return context->report(err);
        }
    }

    // :AutoScope
    // Resolution of an affectation to an enum, without having to specify the enum name before
    // 'using', but just for affectation
    const auto typeInfo = varDecl->type->typeInfo->getConcreteAlias();
    if (typeInfo->isEnum())
    {
        const auto typeEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        varDecl->assignment->addAlternativeScope(typeEnum->scope);
    }
    else if (typeInfo->isArray())
    {
        const auto typeArr = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        if (typeArr->finalType->isEnum())
        {
            const auto typeEnum = castTypeInfo<TypeInfoEnum>(typeArr->finalType, TypeInfoKind::Enum);
            varDecl->assignment->addAlternativeScope(typeEnum->scope);
        }
    }

    return true;
}

bool Semantic::resolveVarDeclAfter(SemanticContext* context)
{
    const auto node = castAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // Ghosting check
    // We simulate a reference to the local variable, in the same context, to raise an error
    // if ambiguous. That way we have a direct error at the declaration, even if the variable
    // is not used later
    if (node->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_LOCAL))
    {
        const auto id = createTmpId(context, node, node->token.text);
        SWAG_CHECK(resolveIdentifier(context, id, RI_FOR_GHOSTING));
        YIELD();
    }

    // :opAffectConstExpr
    if (node->resolvedSymbolOverload() &&
        node->resolvedSymbolOverload()->hasFlag(OVERLOAD_STRUCT_AFFECT) &&
        node->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_GLOBAL | OVERLOAD_VAR_STRUCT | OVERLOAD_CONSTANT))
    {
        const auto overload = node->resolvedSymbolOverload();
        SWAG_ASSERT(overload->hasFlag(OVERLOAD_INCOMPLETE));

        node->removeAstFlag(AST_NO_BYTECODE);
        node->removeAstFlag(AST_COMPUTED_VALUE);
        node->assignment->removeAstFlag(AST_NO_BYTECODE);
        node->addAstFlag(AST_CONST_EXPR);
        node->addSemFlag(SEMFLAG_EXEC_RET_STACK);

        node->byteCodeFct                     = ByteCodeGen::emitLocalVarDecl;
        overload->computedValue.storageOffset = 0;

        SWAG_CHECK(evaluateConstExpression(context, node));
        YIELD();

        node->addAstFlag(AST_NO_BYTECODE);
        node->assignment->addAstFlag(AST_NO_BYTECODE);

        SWAG_ASSERT(node->computedValue()->storageSegment);
        SWAG_ASSERT(node->computedValue()->storageOffset != UINT32_MAX);

        const auto wantStorageSegment = getConstantSegFromContext(node);

        // Copy value from compiler segment to real requested segment
        if (node->computedValue()->storageSegment != wantStorageSegment)
        {
            const auto addrSrc = node->computedValue()->getStorageAddr();
            uint8_t*   addrDst;
            const auto storageOffset              = wantStorageSegment->reserve(node->typeInfo->sizeOf, &addrDst);
            node->computedValue()->storageSegment = wantStorageSegment;
            node->computedValue()->storageOffset  = storageOffset;
            std::copy_n(static_cast<uint8_t*>(addrSrc), node->typeInfo->sizeOf, addrDst);
        }

        // Will remove the incomplete flag, and finish the resolve
        AddSymbolTypeInfo toAdd;
        toAdd.node           = node;
        toAdd.typeInfo       = node->typeInfo;
        toAdd.kind           = overload->symbol->kind;
        toAdd.flags          = overload->flags.maskInvert(OVERLOAD_INCOMPLETE);
        toAdd.storageOffset  = node->computedValue()->storageOffset;
        toAdd.storageSegment = node->computedValue()->storageSegment;

        node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    }

    // Compiler #message
    if (node->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_GLOBAL))
    {
        sendCompilerMsgGlobalVar(context);
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::sendCompilerMsgGlobalVar(SemanticContext* context)
{
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;
    const auto node       = context->node;

    // Filter what we send
    if (module->is(ModuleKind::BootStrap) || module->is(ModuleKind::Runtime))
        return true;
    if (sourceFile->imported)
        return true;

    CompilerMessage msg;
    msg.concrete.kind        = CompilerMsgKind::SemGlobals;
    msg.concrete.name.buffer = node->token.text.buffer;
    msg.concrete.name.count  = node->token.text.length();
    msg.typeInfo             = node->typeInfo;
    SWAG_CHECK(module->postCompilerMessage(msg));

    return true;
}

bool Semantic::resolveVarDeclBefore(SemanticContext* context)
{
    const auto node = castAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // Collect all attributes for the variable
    SWAG_CHECK(collectAttributes(context, node, &node->attributes));

    if (node->assignment && node->is(AstNodeKind::ConstDecl))
    {
        bool isGeneric = false;
        if (node->hasAstFlag(AST_STRUCT_MEMBER))
        {
            const auto parent = node->findParent(AstNodeKind::StructDecl);
            if (parent)
                isGeneric = parent->hasAstFlag(AST_GENERIC);
        }

        if (isGeneric)
        {
            node->assignment->typeInfo = g_TypeMgr->typeInfoS32;
            node->assignment->addAstFlag(AST_NO_SEMANTIC);
        }
        else
        {
            node->assignment->removeAstFlag(AST_NO_SEMANTIC);
        }
    }

    return true;
}

bool Semantic::resolveVarDeclAfterAssign(SemanticContext* context)
{
    const auto job = context->baseJob;

    auto parent = context->node->parent;
    while (parent && parent->isNot(AstNodeKind::VarDecl) && parent->isNot(AstNodeKind::ConstDecl))
        parent = parent->parent;
    SWAG_ASSERT(parent);
    const auto varDecl = castAst<AstVarDecl>(parent);

    const auto assign = varDecl->assignment;
    if (!assign || assign->isNot(AstNodeKind::ExpressionList))
        return true;

    const auto exprList = castAst<AstExpressionList>(assign, AstNodeKind::ExpressionList);
    if (!exprList->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
        return true;

    // If there's an assignment, but no type, then we need to deduce/generate the type with
    // the assignment, then do the semantic on that type
    if (!varDecl->type)
    {
        SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, varDecl, assign, &varDecl->type));
        varDecl->addAstFlag(AST_HAS_FULL_STRUCT_PARAMETERS);
        context->result = ContextResult::Done;
        job->nodes.pop_back();
        job->nodes.push_back(varDecl->type);
        job->nodes.push_back(context->node);
    }

    // Here we will transform the struct literal to a type initialization
    const auto typeExpression = castAst<AstTypeExpression>(varDecl->type, AstNodeKind::TypeExpression);
    if (!typeExpression->identifier)
        return true;

    const auto identifier = castAst<AstIdentifier>(typeExpression->identifier->lastChild(), AstNodeKind::Identifier);
    if (identifier->callParameters)
    {
        Diagnostic err{assign, toErr(Err0061)};
        err.addNote(identifier->callParameters, toNte(Nte0177));
        return context->report(err);
    }

    identifier->callParameters = Ast::newFuncCallParams(nullptr, identifier);

    const auto numParams = assign->childCount();
    for (uint32_t i = 0; i < numParams; i++)
    {
        const auto child = assign->firstChild();
        const auto param = Ast::newFuncCallParam(nullptr, identifier->callParameters);
        Ast::removeFromParent(child);
        Ast::addChildBack(param, child);
        child->addSemFlag(SEMFLAG_TYPE_SOLVED);
        param->inheritTokenLocation(child->token);
    }

    identifier->callParameters->inheritTokenLocation(varDecl->assignment->token);
    identifier->callParameters->inheritAstFlagsOr(varDecl->assignment, AST_CONST_EXPR | AST_SIDE_EFFECTS);
    identifier->callParameters->addSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT);
    identifier->addAstFlag(AST_IN_TYPE_VAR_DECLARATION);
    typeExpression->removeAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN | AST_COMPUTED_VALUE);
    typeExpression->addSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS);

    Ast::removeFromParent(varDecl->assignment);
    // varDecl->assignment->release(); This is reference in 'originalParent' in case of errors, so keep it
    varDecl->assignment = nullptr;

    job->nodes.pop_back();
    typeExpression->semanticState = AstNodeResolveState::Enter;
    job->nodes.push_back(typeExpression);
    job->nodes.push_back(context->node);

    return true;
}

bool Semantic::convertTypeListToArray(SemanticContext* context, AstVarDecl* node, bool isCompilerConstant, OverloadFlags overFlags, CastFlags castFlags)
{
    const auto typeList  = castTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListArray);
    const auto typeArray = TypeManager::convertTypeListToArray(context, typeList, isCompilerConstant);
    node->typeInfo       = typeArray;

    // For a global variable, no need to collect in the constant segment, as we will collect directly to the mutable segment
    if (overFlags.has(OVERLOAD_VAR_GLOBAL))
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment, castFlags | CAST_FLAG_NO_COLLECT));
    else
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment, castFlags));
    node->typeInfo->sizeOf = node->assignment->typeInfo->sizeOf;

    return true;
}

DataSegment* Semantic::getSegmentForVar(SemanticContext*, const AstVarDecl* varNode)
{
    const auto module   = varNode->token.sourceFile->module;
    const auto typeInfo = TypeManager::concreteType(varNode->typeInfo);

    if (varNode->hasAttribute(ATTRIBUTE_TLS))
        return &module->tlsSegment;

    if (isCompilerContext(varNode))
        return &module->compilerSegment;

    if (varNode->isConstDecl())
        return &module->constantSegment;
    if (varNode->resolvedSymbolOverload() && varNode->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_STRUCT))
        return &module->constantSegment;

    if (varNode->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
        return &module->mutableSegment;

    // An array of struct with default values should go to the mutable segment
    if (varNode->typeInfo->isArrayOfStruct())
    {
        const auto typeArr = castTypeInfo<TypeInfoArray>(varNode->typeInfo, TypeInfoKind::Array);
        if (typeArr->finalType->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
            return &module->mutableSegment;
    }

    if (!varNode->assignment && (typeInfo->isNative() || typeInfo->isArray()))
        return &module->bssSegment;
    if (varNode->assignment && typeInfo->isNative() && typeInfo->sizeOf <= 8 && varNode->assignment->isConstant0())
        return &module->bssSegment;
    if (!varNode->assignment &&
        (!varNode->type || !varNode->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS)) &&
        !varNode->hasAstFlag(AST_HAS_FULL_STRUCT_PARAMETERS) &&
        (varNode->typeInfo->isStruct() || varNode->typeInfo->isInterface()) &&
        !varNode->typeInfo->hasFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES))
        return &module->bssSegment;

    return &module->mutableSegment;
}

// :DeduceLambdaType
TypeInfo* Semantic::getDeducedLambdaType(SemanticContext*, const AstMakePointer* node)
{
    SWAG_ASSERT(node->hasSpecFlag(AstMakePointer::SPEC_FLAG_DEP_TYPE));

    TypeInfo* result = node->deducedLambdaType;
    if (result)
        return result;

    if (node->parent->is(AstNodeKind::AffectOp))
    {
        result = node->parent->firstChild()->typeInfo;
    }
    else if (node->parent->is(AstNodeKind::VarDecl))
    {
        const auto varDecl = castAst<AstVarDecl>(node->parent, AstNodeKind::VarDecl);
        SWAG_ASSERT(varDecl->type);
        result = varDecl->type->typeInfo;
    }

    SWAG_ASSERT(result);
    return result->getConcreteAlias();
}

// :DeduceLambdaType
bool Semantic::deduceLambdaParamTypeFrom(SemanticContext* context, AstVarDecl* nodeParam, bool& lambdaExpr, bool& genericType)
{
    const auto mpl = nodeParam->ownerFct->makePointerLambda;
    if (!mpl || !mpl->hasSpecFlag(AstMakePointer::SPEC_FLAG_DEP_TYPE))
        return true;

    auto frontType = getDeducedLambdaType(context, mpl);
    frontType      = TypeManager::concreteType(frontType);

    TypeInfoFuncAttr* typeLambda = nullptr;
    if (frontType->isStruct())
    {
        const auto op    = mpl->parent;
        const auto front = op->firstChild();
        SWAG_ASSERT(op->is(AstNodeKind::AffectOp));

        typeLambda = mpl->deducedLambdaType;
        if (!typeLambda)
        {
            // Generate an undefined type to make the match
            if (!mpl->tryLambdaType)
            {
                const auto tryType = makeType<TypeInfoFuncAttr>();
                tryType->kind      = TypeInfoKind::LambdaClosure;
                if (nodeParam->ownerFct->captureParameters)
                    tryType->addFlag(TYPEINFO_CLOSURE);

                for (uint32_t i = 0; i < nodeParam->ownerFct->parameters->childCount(); i++)
                {
                    auto p      = TypeManager::makeParam();
                    p->typeInfo = g_TypeMgr->typeInfoUndefined;
                    tryType->parameters.push_back(p);
                }

                mpl->tryLambdaType = tryType;
            }

            // op match
            if (op->token.is(TokenId::SymEqual))
            {
                mpl->typeInfo = g_TypeMgr->typeInfoUndefined;
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAffect, nullptr, nullptr, front, mpl, ROP_SIMPLE_CAST));
                YIELD();
            }
            else
            {
                mpl->typeInfo = mpl->tryLambdaType;
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op->token.text, nullptr, front, mpl, ROP_SIMPLE_CAST));
                YIELD();
            }

            if (context->cacheMatches.empty() || context->cacheMatches.size() > 1)
                return true;

            const auto typeOverload = castTypeInfo<TypeInfoFuncAttr>(context->cacheMatches[0]->oneOverload->overload->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeOverload->parameters[1]->typeInfo->isLambdaClosure())
                return true;

            typeLambda = castTypeInfo<TypeInfoFuncAttr>(typeOverload->parameters[1]->typeInfo, TypeInfoKind::LambdaClosure);
            if (!typeLambda)
                return true;
        }
    }
    else
    {
        SWAG_ASSERT(frontType->isLambdaClosure());
        typeLambda = castTypeInfo<TypeInfoFuncAttr>(frontType, TypeInfoKind::LambdaClosure);
    }

    auto paramIdx = nodeParam->childParentIdx();

    // Do not deduce from the context closure generated parameter
    SWAG_ASSERT(typeLambda);
    if (typeLambda->isClosure() && !nodeParam->ownerFct->captureParameters)
        paramIdx += 1;

    if (paramIdx >= typeLambda->parameters.count)
    {
        const Diagnostic err{nodeParam, formErr(Err0623, typeLambda->parameters.count, nodeParam->parent->children.count)};
        return context->report(err);
    }

    mpl->deducedLambdaType = typeLambda;
    nodeParam->typeInfo    = typeLambda->parameters[paramIdx]->typeInfo;
    lambdaExpr             = false;
    genericType            = false;

    return true;
}

bool Semantic::checkForMissingInitialization(SemanticContext* context, AstVarDecl* node, OverloadFlags overFlags, TypeInfo* concreteNodeType, bool isCompilerConstant)
{
    // A constant must be initialized
    if (isCompilerConstant && !node->hasFlagComputedValue())
        return context->report({node, toErr(Err0551)});

    // A constant variable must be initialized
    if (overFlags.has(OVERLOAD_CONST_ASSIGN) && node->isNot(AstNodeKind::FuncDeclParam))
    {
        if (overFlags.has(OVERLOAD_IS_LET))
            return context->report({node, toErr(Err0553)});
        return context->report({node, toErr(Err0554)});
    }

    // A reference must be initialized
    if (concreteNodeType &&
        concreteNodeType->isPointerRef() &&
        node->isNot(AstNodeKind::FuncDeclParam) &&
        !node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
    {
        return context->report({node, toErr(Err0552)});
    }

    // Check an enum variable without initialization
    if (concreteNodeType &&
        concreteNodeType->isEnum() &&
        node->isNot(AstNodeKind::FuncDeclParam) &&
        !node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
    {
        const auto                  concreteTypeEnum = castTypeInfo<TypeInfoEnum>(concreteNodeType, TypeInfoKind::Enum);
        VectorNative<TypeInfoEnum*> collect;
        concreteTypeEnum->collectEnums(collect);

        bool ok = false;
        for (const auto typeEnum : collect)
        {
            if (ok)
                break;

            const auto rawType = TypeManager::concreteType(typeEnum->rawType, CONCRETE_ALIAS);
            if (rawType->isNativeFloat() || rawType->isNativeIntegerOrRune() || rawType->isBool())
            {
                for (const auto p : typeEnum->values)
                {
                    if (!p->value)
                        continue;
                    if (!p->value->reg.u64)
                    {
                        ok = true;
                        break;
                    }
                }
            }
            else if (rawType->isString())
            {
                for (const auto p : typeEnum->values)
                {
                    if (!p->value)
                        continue;
                    if (!p->value->text.buffer)
                    {
                        ok = true;
                        break;
                    }
                }
            }
            else if (rawType->isSlice())
            {
                for (const auto p : typeEnum->values)
                {
                    if (!p->value)
                        continue;
                    const auto slice = static_cast<SwagSlice*>(p->value->getStorageAddr());
                    if (!slice->buffer && !slice->count)
                    {
                        ok = true;
                        break;
                    }
                }
            }
            else
            {
                ok = true;
                break;
            }
        }

        if (!ok)
        {
            Diagnostic err{node, formErr(Err0555, node->token.c_str(), concreteTypeEnum->getDisplayNameC())};
            err.addNote(Diagnostic::hereIs(concreteNodeType->declNode));
            return context->report(err);
        }
    }

    return true;
}

bool Semantic::checkMixAlias(SemanticContext* context, AstVarDecl* node)
{
    // Check #mix
    if (!node->hasAstFlag(AST_GENERATED) && !node->hasOwnerInline() && node->token.text.find(g_LangSpec->name_sharpmix) == 0)
    {
        auto ownerFct = node->ownerFct;
        while (ownerFct)
        {
            if (ownerFct->hasAttribute(ATTRIBUTE_MIXIN))
                break;
            ownerFct = ownerFct->ownerFct;
        }

        if (!ownerFct)
        {
            Diagnostic err{node, toErr(Err0400)};
            err.addNote(toNte(Nte0002));
            return context->report(err);
        }
    }

    // Check #alias
    if (!node->hasAstFlag(AST_GENERATED) && !node->hasOwnerInline() && node->token.text.find(g_LangSpec->name_sharpalias) == 0)
    {
        auto ownerFct = node->ownerFct;
        while (ownerFct)
        {
            if (ownerFct->hasAttribute(ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN))
                break;
            ownerFct = ownerFct->ownerFct;
        }

        if (!ownerFct)
        {
            Diagnostic err{node, node->token, toErr(Err0400)};
            err.addNote(toNte(Nte0001));
            return context->report(err);
        }
    }

    return true;
}

bool Semantic::checkTypeSuffix(SemanticContext* context, const AstVarDecl* node)
{
    if (node->assignment && node->assignment->hasSemFlag(SEMFLAG_LITERAL_SUFFIX))
    {
        if (!node->type || !node->type->typeInfo->isStruct())
        {
            const Diagnostic err{node->assignment->firstChild(), formErr(Err0395, node->assignment->firstChild()->token.c_str())};
            return context->report(err);
        }
    }

    return true;
}

bool Semantic::resolveLocalVar(SemanticContext* context, AstVarDecl* node, OverloadFlags& overFlags, const TypeInfo* typeInfo, uint32_t& storageOffset)
{
    // For a struct, need to wait for special functions to be found
    if (typeInfo->isStruct() || typeInfo->isArrayOfStruct())
    {
        SWAG_CHECK(waitForStructUserOps(context, node));
        YIELD();
    }

    // Variable is still generic. Try to find default generic parameters to instantiate it
    if (node->typeInfo->isGeneric())
    {
        SWAG_CHECK(Generic::instantiateDefaultGenericVar(context, node));
        YIELD();
    }

    SWAG_ASSERT(node->ownerScope);

    // Do not allocate space on the stack for a 'retval' variable, because it's not really a variable
    if (node->type && node->type->is(AstNodeKind::TypeExpression))
    {
        const auto typeExpr = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        if (typeExpr->typeFlags.has(TYPEFLAG_IS_RETVAL))
        {
            const auto ownerFct   = getFunctionForReturn(node);
            auto       typeFunc   = castTypeInfo<TypeInfoFuncAttr>(ownerFct->typeInfo, TypeInfoKind::FuncAttr);
            auto       returnType = typeFunc->concreteReturnType();

            // If the function return type is not yet defined (short lambda), then we take are type as
            // the requested return type (as we are a retval, they should match)
            if (returnType->isVoid())
            {
                typeFunc             = castTypeInfo<TypeInfoFuncAttr>(typeFunc->clone(), TypeInfoKind::FuncAttr);
                typeFunc->returnType = node->type->typeInfo;
                returnType           = node->type->typeInfo;
            }

            waitStructGenerated(context->baseJob, returnType);
            YIELD();

            if (!CallConv::returnStructByValue(typeFunc))
                overFlags.add(OVERLOAD_RETVAL);
        }
    }

    // If this is a tuple unpacking, then we just compute the stack offset of the item
    // inside the tuple, so we do not have to generate bytecode !
    if (node->assignment && node->assignment->hasAstFlag(AST_TUPLE_UNPACK))
    {
        node->addAstFlag(AST_NO_BYTECODE_CHILDREN);
        SWAG_ASSERT(node->assignment->is(AstNodeKind::IdentifierRef));
        overFlags.add(OVERLOAD_TUPLE_UNPACK);
        storageOffset = 0;
        for (const auto& c : node->assignment->children)
        {
            SWAG_ASSERT(c->resolvedSymbolOverload());
            storageOffset += c->resolvedSymbolOverload()->computedValue.storageOffset;
        }
    }

    // Reserve room on the stack, except for a retval
    else if (!overFlags.has(OVERLOAD_RETVAL))
    {
        auto assignment = node->assignment;
        if (assignment && (assignment->is(AstNodeKind::Catch) || assignment->is(AstNodeKind::Try) || assignment->is(AstNodeKind::Assume)))
            assignment = assignment->firstChild();

        // :DirectInlineLocalVar
        if (assignment &&
            assignment->is(AstNodeKind::IdentifierRef) &&
            !assignment->lastChild()->children.empty() &&
            assignment->typeInfo == node->typeInfo &&
            assignment->lastChild()->lastChild()->is(AstNodeKind::Inline) &&
            assignment->lastChild()->lastChild()->hasAstFlag(AST_TRANSIENT))
        {
            SWAG_ASSERT(assignment->lastChild()->lastChild()->computedValue());
            storageOffset = assignment->lastChild()->lastChild()->computedValue()->storageOffset;
            node->addSpecFlag(AstVarDecl::SPEC_FLAG_INLINE_STORAGE);
        }
        else
        {
            const auto mySize = typeInfo->isStruct() ? max(typeInfo->sizeOf, 8) : typeInfo->sizeOf;

            if (node->hasSpecFlag(AstVarDecl::SPEC_FLAG_POST_STACK))
            {
                storageOffset = node->ownerFct->stackSize;
                node->ownerFct->stackSize += mySize;
                node->ownerFct->stackSize = static_cast<uint32_t>(TypeManager::align(node->ownerFct->stackSize, sizeof(void*)));

                auto parentScope = node->ownerScope;
                while (parentScope != node->ownerFct->scope)
                {
                    parentScope->startStackSize = max(parentScope->startStackSize, node->ownerFct->stackSize);
                    parentScope->startStackSize = static_cast<uint32_t>(TypeManager::align(parentScope->startStackSize, alignOf(node)));
                    parentScope                 = parentScope->parentScope;
                }
            }
            else
            {
                // Because of 'visit' (at least), it can happen that this is not up to date because of order of evaluation.
                // So update it just in case (5294)
                node->ownerScope->startStackSize = max(node->ownerScope->startStackSize, node->ownerScope->parentScope->startStackSize);
                node->ownerScope->startStackSize = static_cast<uint32_t>(TypeManager::align(node->ownerScope->startStackSize, alignOf(node)));

                storageOffset = node->ownerScope->startStackSize;
                node->ownerScope->startStackSize += mySize;

                setOwnerMaxStackSize(node, node->ownerScope->startStackSize);
            }
        }
    }

    node->setBcNotifyBefore(ByteCodeGen::emitLocalVarDeclBefore);
    node->byteCodeFct = ByteCodeGen::emitLocalVarDecl;
    node->addAstFlag(AST_R_VALUE);
    return true;
}

bool Semantic::resolveGlobalVar(SemanticContext* context, AstVarDecl* node, OverloadFlags& overFlags, DataSegment*& storageSegment, uint32_t& storageOffset)
{
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;

    // Variable is still generic. Try to find default generic parameters to instantiate it
    if (node->typeInfo->isGeneric())
    {
        SWAG_CHECK(Generic::instantiateDefaultGenericVar(context, node));
        YIELD();
    }

    // Register global variable in the list of global variables to drop if the variable is
    // a struct with a 'opDrop' function
    auto isGlobalToDrop = false;
    if (node->typeInfo)
    {
        auto typeNode = node->typeInfo;
        if (typeNode->isStruct() || typeNode->isArrayOfStruct())
        {
            waitStructGeneratedAlloc(context->baseJob, typeNode);
            YIELD();
            if (typeNode->isArrayOfStruct())
                typeNode = castTypeInfo<TypeInfoArray>(typeNode)->finalType;
            const TypeInfoStruct* typeStruct = castTypeInfo<TypeInfoStruct>(typeNode, TypeInfoKind::Struct);
            if (typeStruct->opDrop || typeStruct->opUserDropFct)
                isGlobalToDrop = true;
        }
    }

    if (node->hasAttribute(ATTRIBUTE_PUBLIC))
    {
        Diagnostic err{node, node->getTokenName(), toErr(Err0469)};
        err.addNote(Diagnostic::hereIs(node->findParent(TokenId::KwdPublic)));
        return context->report(err);
    }

    node->addAstFlag(AST_R_VALUE);
    storageSegment = getSegmentForVar(context, node);
    switch (storageSegment->kind)
    {
        case SegmentKind::Compiler:
            overFlags.add(OVERLOAD_VAR_COMPILER);
            break;
        case SegmentKind::Bss:
            overFlags.add(OVERLOAD_VAR_BSS);
            break;
    }

    if (node->hasExtraPointer(ExtraPointerKind::UserOp))
    {
        storageOffset = 0;
        overFlags.add(OVERLOAD_INCOMPLETE);
    }
    else
    {
        SWAG_CHECK(collectAssignment(context, storageSegment, storageOffset, node));
    }

    // Register global variable
    if (!overFlags.has(OVERLOAD_VAR_COMPILER))
    {
        if (node->hasAttribute(ATTRIBUTE_GLOBAL))
        {
            if (node->ownerFct)
                node->ownerFct->localGlobalVars.push_back(context->node);
        }
        else
        {
            module->addGlobalVar(node, overFlags.has(OVERLOAD_VAR_BSS) ? GlobalVarKind::Bss : GlobalVarKind::Mutable);
        }

        // Register global variable in the list of global variables to drop if the variable is
        // a struct with a 'opDrop' function
        if (isGlobalToDrop)
            module->addGlobalVarToDrop(node, storageOffset, storageSegment);
    }

    return true;
}

bool Semantic::resolveConstantVar(SemanticContext* context, AstVarDecl* node, OverloadFlags overFlags, DataSegment*& storageSegment, uint32_t& storageOffset, bool isLocalConstant, bool isGeneric)
{
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;

    // Force a constant to have a constant type, to avoid modifying a type that is in fact stored in the data segment,
    // and has an address
    if (!node->hasAstFlag(AST_FROM_GENERIC))
    {
        if (overFlags.has(OVERLOAD_VAR_GLOBAL) || isLocalConstant)
        {
            if (node->typeInfo->isStruct() ||
                node->typeInfo->isArray() ||
                node->typeInfo->isClosure())
            {
                node->typeInfo = g_TypeMgr->makeConst(node->typeInfo);
            }
        }
    }

    node->addAstFlag(AST_NO_BYTECODE | AST_R_VALUE);
    if (!isGeneric)
    {
        SWAG_CHECK(collectConstantAssignment(context, &storageSegment, &storageOffset, overFlags));
        if (node->ownerFct)
            node->ownerFct->localConstants.push_back(node);
        else
            module->addGlobalVar(node, GlobalVarKind::Constant);
    }

    return true;
}

bool Semantic::resolveVarDecl(SemanticContext* context)
{
    auto node = castAst<AstVarDecl>(context->node);

    SWAG_CHECK(checkMixAlias(context, node));
    SWAG_CHECK(checkTypeSuffix(context, node));

    const auto    sourceFile         = context->sourceFile;
    const auto    module             = sourceFile->module;
    bool          isCompilerConstant = node->is(AstNodeKind::ConstDecl);
    bool          isLocalConstant    = false;
    OverloadFlags overFlags          = 0;

    // Transform let to constant if possible
    if (node->hasSpecFlag(AstVarDecl::SPEC_FLAG_LET))
    {
        if (node->assignment &&
            node->assignment->hasFlagComputedValue() &&
            !node->assignment->typeInfo->isLambdaClosure() &&
            (!node->type || !node->type->typeInfo->isStruct()) &&
            (!node->assignment->typeInfo->isPointer() || node->assignment->typeInfo->isPointerToTypeInfo()))
        {
            node->addSpecFlag(AstVarDecl::SPEC_FLAG_LET_TO_CONST);
            overFlags.add(OVERLOAD_IS_LET);
            isCompilerConstant = true;
        }
        else
        {
            overFlags.add(OVERLOAD_IS_LET);
        }
    }

    if (isCompilerConstant)
        overFlags.add(OVERLOAD_CONSTANT);
    if (node->is(AstNodeKind::FuncDeclParam))
        overFlags.add(OVERLOAD_VAR_FUNC_PARAM | OVERLOAD_CONST_ASSIGN);
    else if (node->ownerScope->isGlobal() || node->hasAttribute(ATTRIBUTE_GLOBAL))
        overFlags.add(OVERLOAD_VAR_GLOBAL);
    else if (node->ownerScope->isGlobalOrImpl() && node->hasAstFlag(AST_IN_IMPL) && !node->hasAstFlag(AST_STRUCT_MEMBER))
        overFlags.add(OVERLOAD_VAR_GLOBAL);
    else if (node->ownerScope->is(ScopeKind::Struct))
        overFlags.add(OVERLOAD_VAR_STRUCT);
    else if (!isCompilerConstant)
        overFlags.add(OVERLOAD_VAR_LOCAL);
    else
        isLocalConstant = true;
    if (node->hasSpecFlag(AstVarDecl::SPEC_FLAG_CONST_ASSIGN))
        overFlags.add(OVERLOAD_CONST_ASSIGN);
    if (node->hasAttribute(ATTRIBUTE_TLS))
        overFlags.add(OVERLOAD_VAR_TLS);
    if (node->assignment)
        overFlags.add(OVERLOAD_VAR_HAS_ASSIGN);

    const auto concreteNodeType = node->type && node->type->typeInfo ? TypeManager::concreteType(node->type->typeInfo, CONCRETE_FORCE_ALIAS) : nullptr;

    // Register public global constant
    if (isCompilerConstant && node->hasAttribute(ATTRIBUTE_PUBLIC))
    {
        if (node->ownerScope->isGlobalOrImpl() && (node->type || node->assignment))
            node->ownerScope->addPublicNode(node);
    }

    if (node->hasAttribute(ATTRIBUTE_DISCARDABLE) && !concreteNodeType->isLambdaClosure())
    {
        Diagnostic err{node, node->token, formErr(Err0478, concreteNodeType->getDisplayNameC())};
        const auto attr = node->findParentAttrUse(g_LangSpec->name_Swag_Discardable);
        err.addNote(attr, formNte(Nte0063, "attribute"));
        return context->report(err);
    }

    // Check for missing initialization
    // This is ok to not have an initialization for structs, as they are initialized by default
    if (!node->assignment && (!node->type || !concreteNodeType->isStruct()))
    {
        SWAG_CHECK(checkForMissingInitialization(context, node, overFlags, concreteNodeType, isCompilerConstant));
    }

    bool thisIsAGenericType = !node->type && !node->assignment;
    bool isGeneric          = false;
    if (node->hasAstFlag(AST_STRUCT_MEMBER))
    {
        const auto p = node->findParent(AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
        SWAG_ASSERT(p);
        isGeneric = p->hasAstFlag(AST_GENERIC);
    }

    // Value
    if (node->assignment &&
        node->assignment->isNot(AstNodeKind::ExpressionList) &&
        node->assignment->isNot(AstNodeKind::ExplicitNoInit) &&
        (!node->assignment->typeInfo->isStruct() || !node->assignment->hasAstFlag(AST_IN_FUNC_DECL_PARAMS)) &&
        !isGeneric)
    {
        // A generic identifier without a type but with a default value is a generic type
        if (node->hasAstFlag(AST_GENERIC) && !node->type && !node->hasAstFlag(AST_R_VALUE) && !node->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_CONSTANT))
        {
            thisIsAGenericType = true;
        }
        else if (!node->hasAstFlag(AST_FROM_GENERIC) || !node->hasSemFlag(SEMFLAG_ASSIGN_COMPUTED))
        {
            SWAG_CHECK(checkIsConcreteOrType(context, node->assignment));
            YIELD();

            if (overFlags.has(OVERLOAD_VAR_GLOBAL) || overFlags.has(OVERLOAD_VAR_FUNC_PARAM) || node->assignment->hasAstFlag(AST_CONST_EXPR))
            {
                SWAG_CHECK(evaluateConstExpression(context, node->assignment));
                YIELD();
                if (overFlags.has(OVERLOAD_VAR_GLOBAL))
                    node->assignment->addAstFlag(AST_NO_BYTECODE);
            }

            node->addSemFlag(SEMFLAG_ASSIGN_COMPUTED);
        }
    }

    // A global variable or a constant must have its value computed at that point
    if (!node->hasAstFlag(AST_FROM_GENERIC))
    {
        if (!isGeneric && node->assignment && (isCompilerConstant || overFlags.has(OVERLOAD_VAR_GLOBAL)))
        {
            if (node->assignment->typeInfo->isLambdaClosure())
            {
                SWAG_VERIFY(!isCompilerConstant, context->report({node->assignment, toErr(Err0209)}));
                const auto funcNode = castAst<AstFuncDecl>(node->assignment->typeInfo->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda);
                SWAG_CHECK(checkCanMakeFuncPointer(context, funcNode, node->assignment));
            }
            else
            {
                SWAG_CHECK(checkIsConstExpr(context, node->assignment->hasAstFlag(AST_CONST_EXPR), node->assignment, toErr(Err0039)));
            }
        }
    }

    // Be sure that an array without a size has an initializer, to deduce its size
    if (concreteNodeType && concreteNodeType->isArray())
    {
        const auto typeArray = castTypeInfo<TypeInfoArray>(concreteNodeType, TypeInfoKind::Array);
        if (typeArray->count == UINT32_MAX && !node->assignment)
            return context->report({node->type, toErr(Err0208)});

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count      = node->assignment->childCount();
            typeArray->totalCount = typeArray->count;
            typeArray->sizeOf     = typeArray->count * typeArray->pointedType->sizeOf;
            typeArray->name       = form("[%d] %s", typeArray->count, typeArray->pointedType->getDisplayNameC());
        }
    }

    if (node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
    {
        overFlags.add(OVERLOAD_NOT_INITIALIZED);

        if (isCompilerConstant)
        {
            Diagnostic err{node->assignment, toErr(Err0551)};
            err.addNote(toNte(Nte0038));
            return context->report(err);
        }

        if (node->hasSpecFlag(AstVarDecl::SPEC_FLAG_LET))
        {
            Diagnostic err{node->assignment, toErr(Err0553)};
            err.addNote(toNte(Nte0038));
            return context->report(err);
        }
    }

    // Types and assignment are specified
    if (node->type && node->assignment && !node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
    {
        SWAG_ASSERT(node->type->typeInfo);

        const auto leftConcreteType  = node->type->typeInfo;
        const auto rightConcreteType = TypeManager::concretePtrRefType(node->assignment->typeInfo);

        // Do not cast for structs, as we can have special assignment with different types
        // Except if this is an initializer list {...}
        if (!leftConcreteType->isStruct() || rightConcreteType->isInitializerList() || rightConcreteType->isTuple())
        {
            // Cast from struct to interface : need to wait for all interfaces to be registered
            if (leftConcreteType->isInterface() && rightConcreteType->isStruct())
            {
                waitAllStructInterfaces(context->baseJob, rightConcreteType);
                YIELD();
            }

            constexpr auto castFlags = CAST_FLAG_TRY_COERCE | CAST_FLAG_UN_CONST | CAST_FLAG_AUTO_OP_CAST | CAST_FLAG_PTR_REF | CAST_FLAG_FOR_AFFECT | CAST_FLAG_FOR_VAR_INIT | CAST_FLAG_ACCEPT_PENDING;
            if (node->type->hasAstFlag(AST_FROM_GENERIC_REPLACE) || (node->type->children.count && node->type->lastChild()->hasAstFlag(AST_FROM_GENERIC_REPLACE)))
                SWAG_CHECK(TypeManager::makeCompatibles(context, node->type->typeInfo, nullptr, node->assignment, castFlags));
            else
                SWAG_CHECK(TypeManager::makeCompatibles(context, node->type->typeInfo, nullptr, node->assignment, castFlags));
            YIELD();

            // :ConcreteRef
            if (!leftConcreteType->isPointerRef() && TypeManager::concreteType(node->assignment->typeInfo)->isPointerRef())
                setUnRef(node->assignment);
        }
        else
        {
            if (leftConcreteType->kind != rightConcreteType->kind || !rightConcreteType->isSame(leftConcreteType, CAST_FLAG_CAST))
            {
                SWAG_CHECK(resolveUserOpAffect(context, leftConcreteType, rightConcreteType, node->type, node->assignment));
                YIELD();

                // :opAffectConstExpr
                if (overFlags.has(OVERLOAD_VAR_STRUCT | OVERLOAD_VAR_GLOBAL | OVERLOAD_CONSTANT))
                {
                    overFlags.add(OVERLOAD_INCOMPLETE | OVERLOAD_STRUCT_AFFECT);
                    SWAG_ASSERT(node->hasExtraPointer(ExtraPointerKind::UserOp));
                    const auto userOp = node->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
                    if (!userOp->node->hasAttribute(ATTRIBUTE_CONSTEXPR))
                    {
                        Diagnostic err{node->assignment, toErr(Err0038)};
                        err.hint = formNte(Nte0191, leftConcreteType->getDisplayNameC());
                        err.addNote(node->assignToken, formNte(Nte0155, g_LangSpec->name_opAffect.c_str()));
                        return context->report(err);
                    }
                }
            }

            // :opAffectConstExp
            else if (overFlags.has(OVERLOAD_VAR_STRUCT | OVERLOAD_VAR_GLOBAL | OVERLOAD_CONSTANT))
                overFlags.add(OVERLOAD_INCOMPLETE | OVERLOAD_STRUCT_AFFECT);
        }

        node->typeInfo = node->type->typeInfo;
    }

    // Only assignment is specified, need to deduce type
    else if (node->assignment && !node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
    {
        node->typeInfo = TypeManager::concreteType(node->assignment->typeInfo, CONCRETE_FUNC);
        SWAG_ASSERT(node->typeInfo);

        // When affect is from a const struct/array, remove the const
        if (node->assignment->isNot(AstNodeKind::Cast))
        {
            if (node->typeInfo->isArray() || node->typeInfo->isStruct())
                node->typeInfo = g_TypeMgr->makeUnConst(node->typeInfo);
        }

        if (node->typeInfo->isVoid())
        {
            if (node->assignment->typeInfo->isFuncAttr() && node->assignment->resolvedSymbolOverload())
            {
                auto       nodeWhere = node->assignment;
                const auto over      = nodeWhere->resolvedSymbolOverload();
                if (nodeWhere->is(AstNodeKind::IdentifierRef))
                    nodeWhere = nodeWhere->lastChild();
                const Diagnostic err{nodeWhere, nodeWhere->token, toErr(Err0363)};
                return context->report(err, Diagnostic::hereIs(over));
            }

            const Diagnostic err{node->assignment, toErr(Err0380)};
            return context->report(err);
        }

        // We need to decide which integer/float type it is
        node->typeInfo = TypeManager::promoteUntyped(node->typeInfo);

        // Convert from initialization list to array
        if (node->typeInfo->isListArray())
        {
            CastFlags castFlags = 0;
            if (!isCompilerConstant)
                castFlags = CAST_FLAG_FORCE_UN_CONST;
            SWAG_CHECK(convertTypeListToArray(context, node, isCompilerConstant, overFlags, castFlags));
        }

        // :ConcreteRef
        if (node->typeInfo->isPointerRef() && setUnRef(node->assignment))
        {
            const auto typePointer = castTypeInfo<TypeInfoPointer>(node->typeInfo, TypeInfoKind::Pointer);
            node->typeInfo         = typePointer->pointedType;
        }
    }

    // Only type is specified, this is it...
    else if (node->type)
    {
        node->typeInfo = node->type->typeInfo;
    }

    if (node->type)
        node->inheritAstFlagsOr(node->type, AST_GENERIC);
    if (node->hasAstFlag(AST_GENERIC))
    {
        overFlags.add(OVERLOAD_GENERIC);
        if (thisIsAGenericType && node->assignment)
        {
            const auto typeGeneric = makeType<TypeInfoGeneric>();
            typeGeneric->name      = node->token.text;
            typeGeneric->declNode  = node;
            typeGeneric->rawType   = node->typeInfo;
            node->typeInfo         = typeGeneric;
        }
        else if (!node->typeInfo)
        {
            const auto typeGeneric = makeType<TypeInfoGeneric>();
            typeGeneric->name      = node->token.text;
            typeGeneric->declNode  = node;
            node->typeInfo         = typeGeneric;
        }
    }

    if (!node->typeInfo || node->typeInfo == g_TypeMgr->typeInfoUndefined)
    {
        bool lambdaExpr = false;
        if (node->ownerFct && node->is(AstNodeKind::FuncDeclParam) && node->ownerFct->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IS_LAMBDA_EXPRESSION))
            lambdaExpr = true;
        if (lambdaExpr)
        {
            SWAG_CHECK(deduceLambdaParamTypeFrom(context, node, lambdaExpr, thisIsAGenericType));
            YIELD();
        }

        // If this is a lambda parameter in an expression, this is fine, we will try to deduce the type
        if (lambdaExpr || node->typeInfo == g_TypeMgr->typeInfoUndefined)
        {
            node->typeInfo     = g_TypeMgr->typeInfoUndefined;
            thisIsAGenericType = false;

            // AST_PENDING_LAMBDA_TYPING will stop semantic, forcing to not evaluate the content of the function,
            // until types are known
            if (node->ownerFct && node->ownerScope->isNot(ScopeKind::Struct))
                node->ownerFct->addSemFlag(SEMFLAG_PENDING_LAMBDA_TYPING);
        }
    }

    // We should have a type here !
    SWAG_VERIFY(node->typeInfo, context->report({node, formErr(Err0406, Naming::kindName(node).c_str(), node->token.c_str())}));

    // Type should be a correct one
    SWAG_VERIFY(!node->typeInfo->isPointerNull(), context->report({node->assignment ? node->assignment : node, toErr(Err0407)}));
    SWAG_VERIFY(!node->typeInfo->isVoid(), context->report({node->type ? node->type : node, toErr(Err0405)}));

    // A 'let' for a struct make the type const
    if (node->hasSpecFlag(AstVarDecl::SPEC_FLAG_LET) && node->typeInfo->isStruct())
        node->typeInfo = g_TypeMgr->makeConst(node->typeInfo);

    // Determine if the call parameters cover everything (to avoid calling default initialization)
    // i.e. set AST_HAS_FULL_STRUCT_PARAMETERS
    if (node->type && node->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS))
    {
        auto typeExpression = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        while (typeExpression->typeFlags.has(TYPEFLAG_IS_SUB_TYPE))
            typeExpression = castAst<AstTypeExpression>(typeExpression->lastChild(), AstNodeKind::TypeExpression);
        const auto identifier = castAst<AstIdentifier>(typeExpression->identifier->lastChild(), AstNodeKind::Identifier);

        TypeInfoStruct* typeStruct = nullptr;
        if (node->typeInfo->isStruct())
            typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        else if (node->typeInfo->isArray())
        {
            auto typeArray = castTypeInfo<TypeInfoArray>(node->typeInfo, TypeInfoKind::Array);
            while (typeArray->pointedType->isArray())
                typeArray = castTypeInfo<TypeInfoArray>(typeArray->pointedType, TypeInfoKind::Array);
            typeStruct = castTypeInfo<TypeInfoStruct>(typeArray->pointedType, TypeInfoKind::Struct);
        }

        if (typeStruct && identifier->callParameters)
        {
            typeStruct->flattenUsingFields();
            if (identifier->callParameters->childCount() == typeStruct->flattenFields.size())
                node->addAstFlag(AST_HAS_FULL_STRUCT_PARAMETERS);
        }
    }

    // In case of a struct (or array of structs), be sure struct is now completed before
    // Otherwise there's a chance, for example, that 'sizeof' is 0, which can lead to various
    // problems.
    if (!isCompilerConstant || !node->hasAstFlag(AST_FROM_GENERIC))
    {
        if (isCompilerConstant || overFlags.has(OVERLOAD_VAR_GLOBAL) || overFlags.has(OVERLOAD_VAR_LOCAL))
        {
            const auto typeInfo = TypeManager::concreteType(node->typeInfo);
            waitTypeCompleted(context->baseJob, typeInfo);
            YIELD();
        }
    }

    uint32_t     storageOffset  = UINT32_MAX;
    DataSegment* storageSegment = nullptr;

    if (isCompilerConstant)
    {
        SWAG_CHECK(resolveConstantVar(context, node, overFlags, storageSegment, storageOffset, isLocalConstant, isGeneric));
        YIELD();
    }
    else if (overFlags.has(OVERLOAD_VAR_STRUCT))
    {
        storageOffset  = 0;
        storageSegment = &module->constantSegment;
    }
    else if (overFlags.has(OVERLOAD_VAR_GLOBAL))
    {
        SWAG_CHECK(resolveGlobalVar(context, node, overFlags, storageSegment, storageOffset));
        YIELD();
    }
    else if (overFlags.has(OVERLOAD_VAR_LOCAL))
    {
        const auto typeInfo = TypeManager::concreteType(node->typeInfo);
        SWAG_CHECK(resolveLocalVar(context, node, overFlags, typeInfo, storageOffset));
        YIELD();
    }
    else if (overFlags.has(OVERLOAD_VAR_FUNC_PARAM))
    {
        const auto typeInfo = TypeManager::concreteType(node->typeInfo);
        node->addAstFlag(AST_R_VALUE);
        TypeManager::convertStructParamToRef(node, typeInfo);
    }

    // A using on a variable
    if (node->hasAstFlag(AST_DECL_USING))
    {
        SWAG_CHECK(resolveUsingVar(context, context->node, node->typeInfo));
    }

    // Register symbol with its type
    AddSymbolTypeInfo toAdd;
    toAdd.node           = node;
    toAdd.typeInfo       = node->typeInfo;
    toAdd.kind           = thisIsAGenericType ? SymbolKind::GenericType : SymbolKind::Variable;
    toAdd.computedValue  = isCompilerConstant ? node->computedValue() : nullptr;
    toAdd.flags          = overFlags;
    toAdd.storageOffset  = storageOffset;
    toAdd.storageSegment = storageSegment;

    const auto overload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(overload);
    node->setResolvedSymbolOverload(overload);
    return true;
}
