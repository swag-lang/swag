#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Naming.h"
#include "Semantic.h"
#include "TypeManager.h"

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
        Diagnostic err{varDecl, varDecl->token, formErr(Err0384, typeVar->getDisplayNameC())};
        if (varDecl->assignment)
            err.addNote(varDecl->assignment, Diagnostic::isType(TypeManager::concreteType(varDecl->assignment->typeInfo)));
        else if (varDecl->type)
            err.addNote(varDecl->type, Diagnostic::isType(TypeManager::concreteType(varDecl->type->typeInfo)));
        return context->report(err);
    }

    const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
    const auto numUnpack  = scopeNode->children.size() - 1;

    if (typeStruct->fields.empty())
    {
        const Diagnostic err{varDecl, varDecl->token, toErr(Err0383)};
        return context->report(err);
    }

    if (numUnpack < typeStruct->fields.size())
    {
        Diagnostic err{varDecl, varDecl->token, formErr(Err0728, numUnpack, typeStruct->fields.size())};
        err.addNote(varDecl->assignment, formNte(Nte0175, typeStruct->fields.size()));
        err.addNote(toNte(Nte0038));
        return context->report(err);
    }

    if (numUnpack > typeStruct->fields.size())
    {
        Diagnostic err{varDecl, varDecl->token, formErr(Err0729, typeStruct->fields.size(), numUnpack)};
        err.addNote(varDecl->assignment, formNte(Nte0175, typeStruct->fields.size()));
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
            const Diagnostic err{varDecl, varDecl->assignToken, toErr(Err0678)};
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

    // When exporting func inside interfaces, we need the "func" form to be typed, in order
    // to export correctly
    const auto exportNode = node->extraPointer<AstNode>(ExtraPointerKind::ExportNode);
    if (exportNode)
        exportNode->typeInfo = node->type->typeInfo;

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
    if (module->kind == ModuleKind::BootStrap || module->kind == ModuleKind::Runtime)
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
                isGeneric = parent->hasAstFlag(AST_IS_GENERIC);
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

    const auto identifier = castAst<AstIdentifier>(typeExpression->identifier->children.back(), AstNodeKind::Identifier);
    if (identifier->callParameters)
    {
        Diagnostic err{assign, toErr(Err0063)};
        err.addNote(identifier->callParameters, toNte(Nte0164));
        return context->report(err);
    }

    identifier->callParameters = Ast::newFuncCallParams(nullptr, identifier);

    const auto numParams = assign->children.size();
    for (uint32_t i = 0; i < numParams; i++)
    {
        const auto child = assign->children[0];
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
        result = node->parent->children.front()->typeInfo;
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
        const auto front = op->children.front();
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

                for (uint32_t i = 0; i < nodeParam->ownerFct->parameters->children.size(); i++)
                {
                    auto p      = TypeManager::makeParam();
                    p->typeInfo = g_TypeMgr->typeInfoUndefined;
                    tryType->parameters.push_back(p);
                }

                mpl->tryLambdaType = tryType;
            }

            // op match
            if (op->token.id == TokenId::SymEqual)
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
        const Diagnostic err{nodeParam, formErr(Err0630, typeLambda->parameters.count, nodeParam->parent->children.count)};
        return context->report(err);
    }

    mpl->deducedLambdaType = typeLambda;
    nodeParam->typeInfo    = typeLambda->parameters[paramIdx]->typeInfo;
    lambdaExpr             = false;
    genericType            = false;

    return true;
}

bool Semantic::resolveVarDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto node       = castAst<AstVarDecl>(context->node);

    bool isCompilerConstant = node->is(AstNodeKind::ConstDecl);
    bool isLocalConstant    = false;

    // Check #mixin
    if (!node->hasAstFlag(AST_GENERATED) && !node->hasOwnerInline() && node->token.text.find(g_LangSpec->name_atmixin) == 0)
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
            Diagnostic err{node, formErr(Err0356, node->token.c_str())};
            err.addNote(toNte(Nte0002));
            return context->report(err);
        }
    }

    // Check #alias
    if (!node->hasAstFlag(AST_GENERATED) && !node->hasOwnerInline() && node->token.text.find(g_LangSpec->name_atalias) == 0)
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
            Diagnostic err{node, node->token, formErr(Err0356, node->token.c_str())};
            err.addNote(toNte(Nte0001));
            return context->report(err);
        }
    }

    if (node->assignment && node->assignment->hasSemFlag(SEMFLAG_LITERAL_SUFFIX))
    {
        if (!node->type || !node->type->typeInfo->isStruct())
            return context->report({node->assignment->children.front(), formErr(Err0403, node->assignment->children.front()->token.c_str())});
    }

    OverloadFlags overFlags = 0;

    // Transform let to constant if possible
    if (node->hasSpecFlag(AstVarDecl::SPEC_FLAG_IS_LET))
    {
        if (node->assignment &&
            node->assignment->hasFlagComputedValue() &&
            !node->assignment->typeInfo->isLambdaClosure() &&
            (!node->type || !node->type->typeInfo->isStruct()) &&
            (!node->assignment->typeInfo->isPointer() || node->assignment->typeInfo->isPointerToTypeInfo()))
        {
            node->addSpecFlag(AstVarDecl::SPEC_FLAG_IS_LET_TO_CONST);
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

    auto concreteNodeType = node->type && node->type->typeInfo ? TypeManager::concreteType(node->type->typeInfo, CONCRETE_FORCE_ALIAS) : nullptr;

    // Register public global constant
    if (isCompilerConstant && node->hasAttribute(ATTRIBUTE_PUBLIC))
    {
        if (node->ownerScope->isGlobalOrImpl() && (node->type || node->assignment))
            node->ownerScope->addPublicNode(node);
    }

    if (node->hasAttribute(ATTRIBUTE_DISCARDABLE) && !concreteNodeType->isLambdaClosure())
    {
        Diagnostic err{node, node->token, formErr(Err0489, concreteNodeType->getDisplayNameC())};
        auto       attr = node->findParentAttrUse(g_LangSpec->name_Swag_Discardable);
        err.addNote(attr, formNte(Nte0063, "attribute"));
        return context->report(err);
    }

    // Check for missing initialization
    // This is ok to not have an initialization for structs, as they are initialized by default
    if (!node->assignment && (!node->type || !concreteNodeType->isStruct()))
    {
        // A constant must be initialized
        if (isCompilerConstant && !node->hasFlagComputedValue())
            return context->report({node, toErr(Err0562)});

        // A constant variable must be initialized
        if (overFlags.has(OVERLOAD_CONST_ASSIGN) && node->isNot(AstNodeKind::FuncDeclParam))
        {
            if (overFlags.has(OVERLOAD_IS_LET))
                return context->report({node, toErr(Err0564)});
            return context->report({node, toErr(Err0565)});
        }

        // A reference must be initialized
        if (concreteNodeType &&
            concreteNodeType->isPointerRef() &&
            node->isNot(AstNodeKind::FuncDeclParam) &&
            !node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
            return context->report({node, toErr(Err0563)});

        // Check an enum variable without initialization
        if (concreteNodeType &&
            concreteNodeType->isEnum() &&
            node->isNot(AstNodeKind::FuncDeclParam) &&
            !node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
        {
            auto                        concreteTypeEnum = castTypeInfo<TypeInfoEnum>(concreteNodeType, TypeInfoKind::Enum);
            VectorNative<TypeInfoEnum*> collect;
            concreteTypeEnum->collectEnums(collect);

            bool ok = false;
            for (auto typeEnum : collect)
            {
                if (ok)
                    break;

                auto rawType = TypeManager::concreteType(typeEnum->rawType, CONCRETE_ALIAS);
                if (rawType->isNativeFloat() || rawType->isNativeIntegerOrRune() || rawType->isBool())
                {
                    for (auto p : typeEnum->values)
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
                    for (auto p : typeEnum->values)
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
                    for (auto p : typeEnum->values)
                    {
                        if (!p->value)
                            continue;
                        auto slice = static_cast<SwagSlice*>(p->value->getStorageAddr());
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
                Diagnostic err{node, formErr(Err0566, node->token.c_str(), concreteTypeEnum->getDisplayNameC())};
                err.addNote(Diagnostic::hereIs(concreteNodeType->declNode));
                return context->report(err);
            }
        }
    }

    bool thisIsAGenericType = !node->type && !node->assignment;
    bool isGeneric          = false;
    if (node->hasAstFlag(AST_STRUCT_MEMBER))
    {
        auto p = node->findParent(AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
        SWAG_ASSERT(p);
        isGeneric = p->hasAstFlag(AST_IS_GENERIC);
    }

    // Evaluate type constraint
    if (node->hasAstFlag(AST_FROM_GENERIC) && node->typeConstraint)
    {
        SWAG_ASSERT(node->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_TYPE));

        auto typeRet = TypeManager::concreteType(node->typeConstraint->typeInfo, CONCRETE_ALL | CONCRETE_FUNC);
        if (!typeRet->isBool())
        {
            Diagnostic err{node->typeConstraint, formErr(Err0398, typeRet->getDisplayNameC())};
            return context->report(err);
        }

        SWAG_CHECK(checkIsConstExpr(context, node->typeConstraint, toErr(Err0044)));
        SWAG_CHECK(evaluateConstExpression(context, node->typeConstraint));
        YIELD();
        SWAG_ASSERT(node->typeConstraint->hasFlagComputedValue());
        if (!node->typeConstraint->computedValue()->reg.b)
        {
            Diagnostic err(node->typeConstraint, formErr(Err0088, node->typeInfo->getDisplayNameC()));
            if (node->genTypeComesFrom && node->typeConstraint->is(AstNodeKind::IdentifierRef))
            {
                err.addNote(node->genTypeComesFrom, formNte(Nte0139, node->typeInfo->getDisplayNameC(), node->typeConstraint->token.c_str()));
                return context->report(err);
            }

            if (node->genTypeComesFrom)
            {
                err.addNote(node->genTypeComesFrom, formNte(Nte0140, node->typeInfo->getDisplayNameC()));
                return context->report(err);
            }

            return context->report(err);
        }
    }

    // Value
    if (node->assignment &&
        node->assignment->isNot(AstNodeKind::ExpressionList) &&
        node->assignment->isNot(AstNodeKind::ExplicitNoInit) &&
        (!node->assignment->typeInfo->isStruct() || !node->assignment->hasAstFlag(AST_IN_FUNC_DECL_PARAMS)) &&
        !isGeneric)
    {
        // A generic identifier without a type but with a default value is a generic type
        if (node->hasAstFlag(AST_IS_GENERIC) && !node->type && !node->hasAstFlag(AST_R_VALUE) && !node->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_CONSTANT))
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
                SWAG_VERIFY(!isCompilerConstant, context->report({node->assignment, toErr(Err0213)}));
                auto funcNode = castAst<AstFuncDecl>(node->assignment->typeInfo->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda);
                SWAG_CHECK(checkCanMakeFuncPointer(context, funcNode, node->assignment));
            }
            else
            {
                SWAG_CHECK(checkIsConstExpr(context, node->assignment->hasAstFlag(AST_CONST_EXPR), node->assignment, toErr(Err0041)));
            }
        }
    }

    // Be sure that an array without a size has an initializer, to deduce its size
    if (concreteNodeType && concreteNodeType->isArray())
    {
        auto typeArray = castTypeInfo<TypeInfoArray>(concreteNodeType, TypeInfoKind::Array);
        if (typeArray->count == UINT32_MAX && !node->assignment)
            return context->report({node->type, toErr(Err0212)});

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count      = node->assignment->children.size();
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
            Diagnostic err{node->assignment, toErr(Err0562)};
            err.addNote(toNte(Nte0036));
            return context->report(err);
        }

        if (node->hasSpecFlag(AstVarDecl::SPEC_FLAG_IS_LET))
        {
            Diagnostic err{node->assignment, toErr(Err0564)};
            err.addNote(toNte(Nte0036));
            return context->report(err);
        }
    }

    // Types and assignment are specified
    if (node->type && node->assignment && !node->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
    {
        SWAG_ASSERT(node->type->typeInfo);

        auto leftConcreteType  = node->type->typeInfo;
        auto rightConcreteType = TypeManager::concretePtrRefType(node->assignment->typeInfo);

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

            auto castFlags = CAST_FLAG_TRY_COERCE | CAST_FLAG_UN_CONST | CAST_FLAG_AUTO_OP_CAST | CAST_FLAG_PTR_REF | CAST_FLAG_FOR_AFFECT | CAST_FLAG_FOR_VAR_INIT |
                             CAST_FLAG_ACCEPT_PENDING;
            if (node->type->hasAstFlag(AST_FROM_GENERIC_REPLACE) || (node->type->children.count && node->type->children.back()->hasAstFlag(AST_FROM_GENERIC_REPLACE)))
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
                        Diagnostic err{node->assignment, toErr(Err0040)};
                        err.hint = formNte(Nte0178, leftConcreteType->getDisplayNameC());
                        err.addNote(node->assignToken, formNte(Nte0144, g_LangSpec->name_opAffect.c_str()));
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
                auto nodeWhere = node->assignment;
                auto over      = nodeWhere->resolvedSymbolOverload();
                if (nodeWhere->is(AstNodeKind::IdentifierRef))
                    nodeWhere = nodeWhere->children.back();
                Diagnostic err{nodeWhere, nodeWhere->token, toErr(Err0371)};
                return context->report(err, Diagnostic::hereIs(over));
            }

            Diagnostic err{node->assignment, toErr(Err0386)};
            return context->report(err);
        }

        // We need to decide which integer/float type it is
        node->typeInfo = TypeManager::solidifyUntyped(node->typeInfo);

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
            auto typePointer = castTypeInfo<TypeInfoPointer>(node->typeInfo, TypeInfoKind::Pointer);
            node->typeInfo   = typePointer->pointedType;
        }
    }

    // Only type is specified, this is it...
    else if (node->type)
    {
        node->typeInfo = node->type->typeInfo;
    }

    if (node->type)
        node->inheritAstFlagsOr(node->type, AST_IS_GENERIC);
    if (node->hasAstFlag(AST_IS_GENERIC))
    {
        overFlags.add(OVERLOAD_GENERIC);
        if (thisIsAGenericType && node->assignment)
        {
            auto typeGeneric     = makeType<TypeInfoGeneric>();
            typeGeneric->name    = node->token.text;
            typeGeneric->rawType = node->typeInfo;
            node->typeInfo       = typeGeneric;
        }
        else if (!node->typeInfo)
        {
            node->typeInfo       = makeType<TypeInfoGeneric>();
            node->typeInfo->name = node->token.text;
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
    SWAG_VERIFY(node->typeInfo, context->report({node, formErr(Err0413, Naming::kindName(node).c_str(), node->token.c_str())}));

    // Type should be a correct one
    SWAG_VERIFY(!node->typeInfo->isPointerNull(), context->report({node->assignment ? node->assignment : node, toErr(Err0414)}));
    SWAG_VERIFY(!node->typeInfo->isVoid(), context->report({node->type ? node->type : node, toErr(Err0412)}));

    // A 'let' for a struct make the type const
    if (node->hasSpecFlag(AstVarDecl::SPEC_FLAG_IS_LET) && node->typeInfo->isStruct())
        node->typeInfo = g_TypeMgr->makeConst(node->typeInfo);

    // Determine if the call parameters cover everything (to avoid calling default initialization)
    // i.e. set AST_HAS_FULL_STRUCT_PARAMETERS
    if (node->type && node->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS))
    {
        auto typeExpression = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        while (typeExpression->typeFlags.has(TYPEFLAG_IS_SUB_TYPE))
            typeExpression = castAst<AstTypeExpression>(typeExpression->children.back(), AstNodeKind::TypeExpression);
        auto identifier = castAst<AstIdentifier>(typeExpression->identifier->children.back(), AstNodeKind::Identifier);

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
            if (identifier->callParameters->children.size() == typeStruct->flattenFields.size())
                node->addAstFlag(AST_HAS_FULL_STRUCT_PARAMETERS);
        }
    }

    // Force a constant to have a constant type, to avoid modifying a type that is in fact stored in the data segment,
    // and has an address
    if (isCompilerConstant && !node->hasAstFlag(AST_FROM_GENERIC))
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

    auto typeInfo = TypeManager::concreteType(node->typeInfo);

    // In case of a struct (or array of structs), be sure struct is now completed before
    // Otherwise there's a chance, for example, that 'sizeof' is 0, which can lead to various
    // problems.
    if (!isCompilerConstant || !node->hasAstFlag(AST_FROM_GENERIC))
    {
        if (isCompilerConstant || overFlags.has(OVERLOAD_VAR_GLOBAL) || overFlags.has(OVERLOAD_VAR_LOCAL))
        {
            waitTypeCompleted(context->baseJob, typeInfo);
            YIELD();
        }
    }

    uint32_t     storageOffset  = UINT32_MAX;
    DataSegment* storageSegment = nullptr;
    if (isCompilerConstant)
    {
        node->addAstFlag(AST_NO_BYTECODE | AST_R_VALUE);
        if (!isGeneric)
        {
            SWAG_CHECK(collectConstantAssignment(context, &storageSegment, &storageOffset, overFlags));
            if (node->ownerFct)
                node->ownerFct->localConstants.push_back(node);
            else
                module->addGlobalVar(node, GlobalVarKind::Constant);
        }
    }
    else if (overFlags.has(OVERLOAD_VAR_STRUCT))
    {
        storageOffset  = 0;
        storageSegment = &module->constantSegment;
    }
    else if (overFlags.has(OVERLOAD_VAR_GLOBAL))
    {
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
                TypeInfoStruct* typeStruct = castTypeInfo<TypeInfoStruct>(typeNode, TypeInfoKind::Struct);
                if (typeStruct->opDrop || typeStruct->opUserDropFct)
                    isGlobalToDrop = true;
            }
        }

        if (node->hasAttribute(ATTRIBUTE_PUBLIC))
        {
            Diagnostic err{node, node->getTokenName(), toErr(Err0479)};
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
            default:
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
    }
    else if (overFlags.has(OVERLOAD_VAR_LOCAL))
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
            auto typeExpr = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
            if (typeExpr->typeFlags.has(TYPEFLAG_IS_RETVAL))
            {
                auto ownerFct   = getFunctionForReturn(node);
                auto typeFunc   = castTypeInfo<TypeInfoFuncAttr>(ownerFct->typeInfo, TypeInfoKind::FuncAttr);
                auto returnType = typeFunc->concreteReturnType();
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
            for (auto& c : node->assignment->children)
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
                assignment = assignment->children.front();

            // :DirectInlineLocalVar
            if (assignment &&
                assignment->is(AstNodeKind::IdentifierRef) &&
                !assignment->children.back()->children.empty() &&
                assignment->typeInfo == node->typeInfo &&
                assignment->children.back()->children.back()->is(AstNodeKind::Inline) &&
                assignment->children.back()->children.back()->hasAstFlag(AST_TRANSIENT))
            {
                SWAG_ASSERT(assignment->children.back()->children.back()->computedValue());
                storageOffset = assignment->children.back()->children.back()->computedValue()->storageOffset;
                node->addSpecFlag(AstVarDecl::SPEC_FLAG_INLINE_STORAGE);
            }
            else
            {
                auto alignOf = Semantic::alignOf(node);

                // Because of 'visit' (at least), it can happen that this is not up to date because of order of evaluation.
                // So update it just in case (5294 bug)
                node->ownerScope->startStackSize = max(node->ownerScope->startStackSize, node->ownerScope->parentScope->startStackSize);

                node->ownerScope->startStackSize = static_cast<uint32_t>(TypeManager::align(node->ownerScope->startStackSize, alignOf));
                storageOffset                    = node->ownerScope->startStackSize;
                node->ownerScope->startStackSize += typeInfo->isStruct() ? max(typeInfo->sizeOf, 8) : typeInfo->sizeOf;
                setOwnerMaxStackSize(node, node->ownerScope->startStackSize);
            }
        }

        node->setBcNotifyBefore(ByteCodeGen::emitLocalVarDeclBefore);
        node->byteCodeFct = ByteCodeGen::emitLocalVarDecl;
        node->addAstFlag(AST_R_VALUE);
    }
    else if (overFlags.has(OVERLOAD_VAR_FUNC_PARAM))
    {
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

    auto overload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(overload);
    node->setResolvedSymbolOverload(overload);
    return true;
}
