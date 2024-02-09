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

uint32_t Semantic::alignOf(AstVarDecl* node)
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
        varDecl->semFlags |= SEMFLAG_TUPLE_CONVERT;
        SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, varDecl, varDecl->assignment, &varDecl->type));
        context->result = ContextResult::NewChilds;
        context->baseJob->nodes.push_back(varDecl->type);
        return true;
    }
    else if (varDecl->semFlags & SEMFLAG_TUPLE_CONVERT)
    {
        SWAG_ASSERT(varDecl->resolvedSymbolOverload);
        varDecl->typeInfo                         = varDecl->type->typeInfo;
        varDecl->resolvedSymbolOverload->typeInfo = varDecl->type->typeInfo;
        typeVar                                   = varDecl->typeInfo;
    }

    if (!typeVar->isStruct())
    {
        Diagnostic diag{varDecl, varDecl->token, FMT(Err(Err0384), typeVar->getDisplayNameC())};
        if (varDecl->assignment)
            diag.addRange(varDecl->assignment, Diagnostic::isType(TypeManager::concreteType(varDecl->assignment->typeInfo)));
        else if (varDecl->type)
            diag.addRange(varDecl->type, Diagnostic::isType(TypeManager::concreteType(varDecl->type->typeInfo)));
        return context->report(diag);
    }

    const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
    const auto numUnpack  = scopeNode->childs.size() - 1;

    if (typeStruct->fields.empty())
    {
        const Diagnostic diag{varDecl, varDecl->token, Err(Err0383)};
        return context->report(diag);
    }

    if (numUnpack < typeStruct->fields.size())
    {
        Diagnostic diag{varDecl, varDecl->token, FMT(Err(Err0728), numUnpack, typeStruct->fields.size())};
        diag.addRange(varDecl->assignment, FMT(Nte(Nte0175), typeStruct->fields.size()));
        const auto note = Diagnostic::note(Nte(Nte0038));
        return context->report(diag, note);
    }

    if (numUnpack > typeStruct->fields.size())
    {
        Diagnostic diag{varDecl, varDecl->token, FMT(Err(Err0729), typeStruct->fields.size(), numUnpack)};
        diag.addRange(varDecl->assignment, FMT(Nte(Nte0175), typeStruct->fields.size()));
        return context->report(diag);
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
    while (parent && parent->kind != AstNodeKind::VarDecl && parent->kind != AstNodeKind::ConstDecl && parent->kind != AstNodeKind::FuncDeclParam)
        parent = parent->parent;
    SWAG_ASSERT(parent);
    const auto varDecl = (AstVarDecl*) parent;
    if (!varDecl->type || !varDecl->assignment)
        return true;

    if (parent->kind == AstNodeKind::FuncDeclParam)
    {
        if (varDecl->type->typeInfo->isTypedVariadic() ||
            varDecl->type->typeInfo->isVariadic() ||
            varDecl->type->typeInfo->isCVariadic())
        {
            const Diagnostic diag{varDecl, varDecl->assignToken, Err(Err0678)};
            return context->report(diag);
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

    // When exporting func inside interfaces, we need to "func" form to be typed, in order
    // to export correctly
    if (node->hasExtMisc() && node->extMisc()->exportNode)
        node->extMisc()->exportNode->typeInfo = node->type->typeInfo;

    // Ghosting check
    // We simulate a reference to the local variable, in the same context, to raise an error
    // if ambiguous. That way we have a direct error at the declaration, even if the variable
    // is not used later
    if (node->resolvedSymbolOverload->flags & OVERLOAD_VAR_LOCAL)
    {
        const auto id = createTmpId(context, node, node->token.text);
        SWAG_CHECK(resolveIdentifier(context, id, RI_FOR_GHOSTING));
        YIELD();
    }

    // :opAffectConstExpr
    if (node->resolvedSymbolOverload &&
        node->resolvedSymbolOverload->flags & OVERLOAD_STRUCT_AFFECT &&
        node->resolvedSymbolOverload->flags & (OVERLOAD_VAR_GLOBAL | OVERLOAD_VAR_STRUCT | OVERLOAD_CONSTANT))
    {
        const auto overload = node->resolvedSymbolOverload;
        SWAG_ASSERT(overload->flags & OVERLOAD_INCOMPLETE);

        node->removeFlag(AST_NO_BYTECODE);
        node->removeFlag(AST_VALUE_COMPUTED);
        node->assignment->removeFlag(AST_NO_BYTECODE);
        node->addFlag(AST_CONST_EXPR);
        node->semFlags |= SEMFLAG_EXEC_RET_STACK;

        node->byteCodeFct                     = ByteCodeGen::emitLocalVarDecl;
        overload->computedValue.storageOffset = 0;

        SWAG_CHECK(evaluateConstExpression(context, node));
        YIELD();

        node->addFlag(AST_NO_BYTECODE);
        node->assignment->addFlag(AST_NO_BYTECODE);

        SWAG_ASSERT(node->computedValue->storageSegment);
        SWAG_ASSERT(node->computedValue->storageOffset != UINT32_MAX);

        const auto wantStorageSegment = getConstantSegFromContext(node);

        // Copy value from compiler segment to real requested segment
        if (node->computedValue->storageSegment != wantStorageSegment)
        {
            const auto addrSrc = node->computedValue->getStorageAddr();
            uint8_t*   addrDst;
            const auto storageOffset            = wantStorageSegment->reserve(node->typeInfo->sizeOf, &addrDst);
            node->computedValue->storageSegment = wantStorageSegment;
            node->computedValue->storageOffset  = storageOffset;
            memcpy(addrDst, addrSrc, node->typeInfo->sizeOf);
        }

        // Will remove the incomplete flag, and finish the resolve
        AddSymbolTypeInfo toAdd;
        toAdd.node           = node;
        toAdd.typeInfo       = node->typeInfo;
        toAdd.kind           = overload->symbol->kind;
        toAdd.flags          = overload->flags & ~OVERLOAD_INCOMPLETE;
        toAdd.storageOffset  = node->computedValue->storageOffset;
        toAdd.storageSegment = node->computedValue->storageSegment;

        node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    }

    // Compiler #message
    if (node->resolvedSymbolOverload->flags & OVERLOAD_VAR_GLOBAL)
    {
        sendCompilerMsgGlobalVar(context);
    }

    return true;
}

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
    SWAG_CHECK(module->postCompilerMessage(context, msg));

    return true;
}

bool Semantic::resolveVarDeclBefore(SemanticContext* context)
{
    const auto node = castAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // Collect all attributes for the variable
    SWAG_CHECK(collectAttributes(context, node, &node->attributes));

    if (node->assignment && node->kind == AstNodeKind::ConstDecl)
    {
        bool isGeneric = false;
        if (node->flags & AST_STRUCT_MEMBER)
        {
            const auto parent = node->findParent(AstNodeKind::StructDecl);
            if (parent)
                isGeneric = parent->flags & AST_IS_GENERIC;
        }

        if (isGeneric)
        {
            node->assignment->typeInfo = g_TypeMgr->typeInfoS32;
            node->assignment->addFlag(AST_NO_SEMANTIC);
        }
        else
        {
            node->assignment->removeFlag(AST_NO_SEMANTIC);
        }
    }

    return true;
}

bool Semantic::resolveVarDeclAfterAssign(SemanticContext* context)
{
    const auto job = context->baseJob;

    auto parent = context->node->parent;
    while (parent && parent->kind != AstNodeKind::VarDecl && parent->kind != AstNodeKind::ConstDecl)
        parent = parent->parent;
    SWAG_ASSERT(parent);
    const auto varDecl = (AstVarDecl*) parent;

    const auto assign = varDecl->assignment;
    if (!assign || assign->kind != AstNodeKind::ExpressionList)
        return true;

    const auto exprList = castAst<AstExpressionList>(assign, AstNodeKind::ExpressionList);
    if (!exprList->hasSpecFlag(AstExpressionList::SPECFLAG_FOR_TUPLE))
        return true;

    // If there's an assignment, but no type, then we need to deduce/generate the type with
    // the assignment, then do the semantic on that type
    if (!varDecl->type)
    {
        SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, varDecl, assign, &varDecl->type));
        varDecl->addFlag(AST_HAS_FULL_STRUCT_PARAMETERS);
        context->result = ContextResult::Done;
        job->nodes.pop_back();
        job->nodes.push_back(varDecl->type);
        job->nodes.push_back(context->node);
    }

    // Here we will transform the struct literal to a type initialization
    const auto typeExpression = castAst<AstTypeExpression>(varDecl->type, AstNodeKind::TypeExpression);
    if (!typeExpression->identifier)
        return true;

    const auto identifier = castAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
    if (identifier->callParameters)
    {
        Diagnostic diag{assign, Err(Err0063)};
        diag.addRange(identifier->callParameters, Nte(Nte0164));
        return context->report(diag);
    }

    const auto sourceFile      = context->sourceFile;
    identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);

    const auto numParams = assign->childs.size();
    for (size_t i = 0; i < numParams; i++)
    {
        const auto child = assign->childs[0];
        const auto param = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        Ast::removeFromParent(child);
        Ast::addChildBack(param, child);
        child->semFlags |= SEMFLAG_TYPE_SOLVED;
        param->inheritTokenLocation(child);
    }

    identifier->callParameters->inheritTokenLocation(varDecl->assignment);
    identifier->callParameters->inheritOrFlag(varDecl->assignment, AST_CONST_EXPR | AST_SIDE_EFFECTS);
    identifier->callParameters->addSpecFlag(AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT);
    identifier->addFlag(AST_IN_TYPE_VAR_DECLARATION);
    typeExpression->removeFlag(AST_NO_BYTECODE);
    typeExpression->removeFlag(AST_NO_BYTECODE_CHILDS);
    typeExpression->removeFlag(AST_VALUE_COMPUTED);
    typeExpression->addSpecFlag(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS);

    Ast::removeFromParent(varDecl->assignment);
    // varDecl->assignment->release(); This is reference in 'originalParent' in case of errors, so keep it
    varDecl->assignment = nullptr;

    job->nodes.pop_back();
    typeExpression->semanticState = AstNodeResolveState::Enter;
    job->nodes.push_back(typeExpression);
    job->nodes.push_back(context->node);

    return true;
}

bool Semantic::convertTypeListToArray(SemanticContext* context, AstVarDecl* node, bool isCompilerConstant, uint32_t symbolFlags, uint32_t castFlags)
{
    const auto typeList  = castTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListArray);
    const auto typeArray = TypeManager::convertTypeListToArray(context, typeList, isCompilerConstant);
    node->typeInfo       = typeArray;

    // For a global variable, no need to collect in the constant segment, as we will collect directly to the mutable segment
    if (symbolFlags & OVERLOAD_VAR_GLOBAL)
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment, castFlags | CASTFLAG_NO_COLLECT));
    else
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment, castFlags));
    node->typeInfo->sizeOf = node->assignment->typeInfo->sizeOf;

    return true;
}

DataSegment* Semantic::getSegmentForVar(SemanticContext* context, const AstVarDecl* varNode)
{
    const auto module   = varNode->sourceFile->module;
    const auto typeInfo = TypeManager::concreteType(varNode->typeInfo);

    if (varNode->hasAttribute(ATTRIBUTE_TLS))
        return &module->tlsSegment;

    if (isCompilerContext(varNode))
        return &module->compilerSegment;

    if (varNode->isConstDecl())
        return &module->constantSegment;
    if (varNode->resolvedSymbolOverload && (varNode->resolvedSymbolOverload->flags & OVERLOAD_VAR_STRUCT))
        return &module->constantSegment;

    if (varNode->hasAttribute(AST_EXPLICITLY_NOT_INITIALIZED))
        return &module->mutableSegment;

    // An array of struct with default values should go to the mutable segment
    if (varNode->typeInfo->isArrayOfStruct())
    {
        const auto typeArr = castTypeInfo<TypeInfoArray>(varNode->typeInfo, TypeInfoKind::Array);
        if (typeArr->finalType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
            return &module->mutableSegment;
    }

    if (!varNode->assignment && (typeInfo->isNative() || typeInfo->isArray()))
        return &module->bssSegment;
    if (varNode->assignment && typeInfo->isNative() && typeInfo->sizeOf <= 8 && varNode->assignment->isConstant0())
        return &module->bssSegment;
    if (!varNode->assignment &&
        (!varNode->type || !varNode->type->hasSpecFlag(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS)) &&
        !(varNode->flags & AST_HAS_FULL_STRUCT_PARAMETERS) &&
        (varNode->typeInfo->isStruct() || varNode->typeInfo->isInterface()) &&
        !(varNode->typeInfo->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
        return &module->bssSegment;

    return &module->mutableSegment;
}

// :DeduceLambdaType
TypeInfo* Semantic::getDeducedLambdaType(SemanticContext*, const AstMakePointer* node)
{
    SWAG_ASSERT(node->hasSpecFlag(AstMakePointer::SPECFLAG_DEP_TYPE));

    TypeInfo* result = node->deducedLambdaType;
    if (result)
        return result;

    if (node->parent->kind == AstNodeKind::AffectOp)
    {
        result = node->parent->childs.front()->typeInfo;
    }
    else if (node->parent->kind == AstNodeKind::VarDecl)
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
    if (!mpl || !mpl->hasSpecFlag(AstMakePointer::SPECFLAG_DEP_TYPE))
        return true;

    auto frontType = getDeducedLambdaType(context, mpl);
    frontType      = TypeManager::concreteType(frontType);

    TypeInfoFuncAttr* typeLambda = nullptr;
    if (frontType->isStruct())
    {
        const auto op    = mpl->parent;
        const auto front = op->childs.front();
        SWAG_ASSERT(op->kind == AstNodeKind::AffectOp);

        typeLambda = mpl->deducedLambdaType;
        if (!typeLambda)
        {
            // Generate an undefined type to make the match
            if (!mpl->tryLambdaType)
            {
                const auto tryType = makeType<TypeInfoFuncAttr>();
                tryType->kind      = TypeInfoKind::LambdaClosure;
                if (nodeParam->ownerFct->captureParameters)
                    tryType->flags |= TYPEINFO_CLOSURE;

                for (size_t i = 0; i < nodeParam->ownerFct->parameters->childs.size(); i++)
                {
                    auto p      = TypeManager::makeParam();
                    p->typeInfo = g_TypeMgr->typeInfoUndefined;
                    tryType->parameters.push_back(p);
                }

                mpl->tryLambdaType = tryType;
            }

            // op match
            if (op->tokenId == TokenId::SymEqual)
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

    if (paramIdx >= (uint32_t) typeLambda->parameters.count)
    {
        const Diagnostic diag{nodeParam, FMT(Err(Err0630), (uint32_t) typeLambda->parameters.count, (uint32_t) nodeParam->parent->childs.count)};
        return context->report(diag);
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
    auto node       = static_cast<AstVarDecl*>(context->node);

    bool isCompilerConstant = node->kind == AstNodeKind::ConstDecl;
    bool isLocalConstant    = false;

    // Check #mixin
    if (!(node->flags & AST_GENERATED) && !node->ownerInline && node->token.text.find(g_LangSpec->name_atmixin) == 0)
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
            auto note = Diagnostic::note(Nte(Nte0002));
            return context->report({node, FMT(Err(Err0356), node->token.ctext())}, note);
        }
    }

    // Check #alias
    if (!(node->flags & AST_GENERATED) && !node->ownerInline && node->token.text.find(g_LangSpec->name_atalias) == 0)
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
            auto note = Diagnostic::note(Nte(Nte0001));
            return context->report({node, node->token, FMT(Err(Err0356), node->token.ctext())}, note);
        }
    }

    if (node->assignment && node->assignment->semFlags & SEMFLAG_LITERAL_SUFFIX)
    {
        if (!node->type || !node->type->typeInfo->isStruct())
            return context->report({node->assignment->childs.front(), FMT(Err(Err0403), node->assignment->childs.front()->token.ctext())});
    }

    uint32_t symbolFlags = 0;

    // Transform let to constant if possible
    if (node->hasSpecFlag(AstVarDecl::SPECFLAG_IS_LET))
    {
        if (node->assignment &&
            node->assignment->hasComputedValue() &&
            !node->assignment->typeInfo->isLambdaClosure() &&
            (!node->type || !node->type->typeInfo->isStruct()) &&
            (!node->assignment->typeInfo->isPointer() || node->assignment->typeInfo->isPointerToTypeInfo()))
        {
            node->addSpecFlag(AstVarDecl::SPECFLAG_IS_LET_TO_CONST);
            symbolFlags |= OVERLOAD_IS_LET;
            isCompilerConstant = true;
        }
        else
        {
            symbolFlags |= OVERLOAD_IS_LET;
        }
    }

    if (isCompilerConstant)
        symbolFlags |= OVERLOAD_CONSTANT;
    if (node->kind == AstNodeKind::FuncDeclParam)
        symbolFlags |= OVERLOAD_VAR_FUNC_PARAM | OVERLOAD_CONST_ASSIGN;
    else if (node->ownerScope->isGlobal() || node->hasAttribute(ATTRIBUTE_GLOBAL))
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->isGlobalOrImpl() && (node->flags & AST_IN_IMPL) && !(node->flags & AST_STRUCT_MEMBER))
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->kind == ScopeKind::Struct)
        symbolFlags |= OVERLOAD_VAR_STRUCT;
    else if (!isCompilerConstant)
        symbolFlags |= OVERLOAD_VAR_LOCAL;
    else
        isLocalConstant = true;
    if (node->hasSpecFlag(AstVarDecl::SPECFLAG_CONST_ASSIGN))
        symbolFlags |= OVERLOAD_CONST_ASSIGN;
    if (node->hasAttribute(ATTRIBUTE_TLS))
        symbolFlags |= OVERLOAD_VAR_TLS;
    if (node->assignment)
        symbolFlags |= OVERLOAD_VAR_HAS_ASSIGN;

    auto concreteNodeType = node->type && node->type->typeInfo ? TypeManager::concreteType(node->type->typeInfo, CONCRETE_FORCE_ALIAS) : nullptr;

    // Register public global constant
    if (isCompilerConstant && node->hasAttribute(ATTRIBUTE_PUBLIC))
    {
        if (node->ownerScope->isGlobalOrImpl() && (node->type || node->assignment))
            node->ownerScope->addPublicNode(node);
    }

    if (node->hasAttribute(ATTRIBUTE_DISCARDABLE) && !concreteNodeType->isLambdaClosure())
    {
        Diagnostic diag{node, node->token, FMT(Err(Err0489), concreteNodeType->getDisplayNameC())};
        auto       attr = node->findParentAttrUse(g_LangSpec->name_Swag_Discardable);
        auto       note = Diagnostic::note(attr, FMT(Nte(Nte0063), "attribute"));
        return context->report(diag, note);
    }

    // Check for missing initialization
    // This is ok to not have an initialization for structs, as they are initialized by default
    if (!node->assignment && (!node->type || !concreteNodeType->isStruct()))
    {
        // A constant must be initialized
        if (isCompilerConstant && !node->hasComputedValue())
            return context->report({node, Err(Err0562)});

        // A constant variable must be initialized
        if ((symbolFlags & OVERLOAD_CONST_ASSIGN) && node->kind != AstNodeKind::FuncDeclParam)
        {
            if (symbolFlags & OVERLOAD_IS_LET)
                return context->report({node, Err(Err0564)});
            return context->report({node, Err(Err0565)});
        }

        // A reference must be initialized
        if (concreteNodeType &&
            concreteNodeType->isPointerRef() &&
            node->kind != AstNodeKind::FuncDeclParam &&
            !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
            return context->report({node, Err(Err0563)});

        // Check an enum variable without initialization
        if (concreteNodeType &&
            concreteNodeType->isEnum() &&
            node->kind != AstNodeKind::FuncDeclParam &&
            !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
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
                        auto slice = (SwagSlice*) p->value->getStorageAddr();
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
                Diagnostic diag{node, FMT(Err(Err0566), node->token.ctext(), concreteTypeEnum->getDisplayNameC())};
                auto       note = Diagnostic::hereIs(concreteNodeType->declNode);
                return context->report(diag, note);
            }
        }
    }

    bool thisIsAGenericType = !node->type && !node->assignment;
    bool isGeneric          = false;
    if (node->flags & AST_STRUCT_MEMBER)
    {
        auto p = node->findParent(AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
        SWAG_ASSERT(p);
        isGeneric = p->flags & AST_IS_GENERIC;
    }

    // Evaluate type constraint
    if ((node->flags & AST_FROM_GENERIC) && node->typeConstraint)
    {
        SWAG_ASSERT(node->hasSpecFlag(AstVarDecl::SPECFLAG_GENERIC_TYPE));

        auto typeRet = TypeManager::concreteType(node->typeConstraint->typeInfo, CONCRETE_ALL | CONCRETE_FUNC);
        if (!typeRet->isBool())
        {
            Diagnostic diag{node->typeConstraint, FMT(Err(Err0398), typeRet->getDisplayNameC())};
            return context->report(diag);
        }

        SWAG_CHECK(checkIsConstExpr(context, node->typeConstraint, Err(Err0044)));
        SWAG_CHECK(evaluateConstExpression(context, node->typeConstraint));
        YIELD();
        SWAG_ASSERT(node->typeConstraint->computedValue);
        if (!node->typeConstraint->computedValue->reg.b)
        {
            Diagnostic diag(node->typeConstraint, FMT(Err(Err0088), node->typeInfo->getDisplayNameC()));
            if (node->genTypeComesFrom && node->typeConstraint->kind == AstNodeKind::IdentifierRef)
            {
                auto note = Diagnostic::note(node->genTypeComesFrom, FMT(Nte(Nte0139), node->typeInfo->getDisplayNameC(), node->typeConstraint->token.ctext()));
                return context->report(diag, note);
            }
            else if (node->genTypeComesFrom)
            {
                auto note = Diagnostic::note(node->genTypeComesFrom, FMT(Nte(Nte0140), node->typeInfo->getDisplayNameC()));
                return context->report(diag, note);
            }
            else
            {
                return context->report(diag);
            }
        }
    }

    // Value
    if (node->assignment &&
        node->assignment->kind != AstNodeKind::ExpressionList &&
        node->assignment->kind != AstNodeKind::ExplicitNoInit &&
        (!node->assignment->typeInfo->isStruct() || !(node->assignment->flags & AST_IN_FUNC_DECL_PARAMS)) &&
        !isGeneric)
    {
        // A generic identifier without a type but with a default value is a generic type
        if ((node->flags & AST_IS_GENERIC) && !node->type && !(node->flags & AST_R_VALUE) && !node->hasSpecFlag(AstVarDecl::SPECFLAG_GENERIC_CONSTANT))
        {
            thisIsAGenericType = true;
        }
        else if (!(node->flags & AST_FROM_GENERIC) || !(node->semFlags & SEMFLAG_ASSIGN_COMPUTED))
        {
            SWAG_CHECK(checkIsConcreteOrType(context, node->assignment));
            YIELD();

            if ((symbolFlags & OVERLOAD_VAR_GLOBAL) || (symbolFlags & OVERLOAD_VAR_FUNC_PARAM) || (node->assignment->flags & AST_CONST_EXPR))
            {
                SWAG_CHECK(evaluateConstExpression(context, node->assignment));
                YIELD();
                if (symbolFlags & OVERLOAD_VAR_GLOBAL)
                    node->assignment->addFlag(AST_NO_BYTECODE);
            }

            node->semFlags |= SEMFLAG_ASSIGN_COMPUTED;
        }
    }

    // A global variable or a constant must have its value computed at that point
    if (!(node->flags & AST_FROM_GENERIC))
    {
        if (!isGeneric && node->assignment && (isCompilerConstant || (symbolFlags & OVERLOAD_VAR_GLOBAL)))
        {
            if (node->assignment->typeInfo->isLambdaClosure())
            {
                SWAG_VERIFY(!isCompilerConstant, context->report({node->assignment, Err(Err0213)}));
                auto funcNode = castAst<AstFuncDecl>(node->assignment->typeInfo->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda);
                SWAG_CHECK(checkCanMakeFuncPointer(context, funcNode, node->assignment));
            }
            else
            {
                SWAG_CHECK(checkIsConstExpr(context, node->assignment->flags & AST_CONST_EXPR, node->assignment, Err(Err0041)));
            }
        }
    }

    // Be sure that an array without a size has an initializer, to deduce its size
    if (concreteNodeType && concreteNodeType->isArray())
    {
        auto typeArray = castTypeInfo<TypeInfoArray>(concreteNodeType, TypeInfoKind::Array);
        if (typeArray->count == UINT32_MAX && !node->assignment)
            return context->report({node->type, Err(Err0212)});

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count      = (uint32_t) node->assignment->childs.size();
            typeArray->totalCount = typeArray->count;
            typeArray->sizeOf     = typeArray->count * typeArray->pointedType->sizeOf;
            typeArray->name       = FMT("[%d] %s", typeArray->count, typeArray->pointedType->getDisplayNameC());
        }
    }

    if (node->flags & AST_EXPLICITLY_NOT_INITIALIZED)
    {
        symbolFlags |= OVERLOAD_NOT_INITIALIZED;

        if (isCompilerConstant)
        {
            Diagnostic diag{node->assignment, Err(Err0562)};
            return context->report(diag, Diagnostic::note(Nte(Nte0036)));
        }

        if (node->hasSpecFlag(AstVarDecl::SPECFLAG_IS_LET))
        {
            Diagnostic diag{node->assignment, Err(Err0564)};
            return context->report(diag, Diagnostic::note(Nte(Nte0036)));
        }
    }

    // Types and assignements are specified
    if (node->type && node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
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

            auto castFlags = CASTFLAG_TRY_COERCE | CASTFLAG_UN_CONST | CASTFLAG_AUTO_OP_CAST | CASTFLAG_PTR_REF | CASTFLAG_FOR_AFFECT | CASTFLAG_FOR_VAR_INIT |
                             CASTFLAG_ACCEPT_PENDING;
            if (node->type->flags & AST_FROM_GENERIC_REPLACE || (node->type->childs.count && node->type->childs.back()->flags & AST_FROM_GENERIC_REPLACE))
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
            if ((leftConcreteType->kind != rightConcreteType->kind) || !rightConcreteType->isSame(leftConcreteType, CASTFLAG_CAST))
            {
                SWAG_CHECK(resolveUserOpAffect(context, leftConcreteType, rightConcreteType, node->type, node->assignment));
                YIELD();

                // :opAffectConstExpr
                if (symbolFlags & (OVERLOAD_VAR_STRUCT | OVERLOAD_VAR_GLOBAL | OVERLOAD_CONSTANT))
                {
                    symbolFlags |= OVERLOAD_INCOMPLETE | OVERLOAD_STRUCT_AFFECT;
                    SWAG_ASSERT(node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload);
                    if (!node->extMisc()->resolvedUserOpSymbolOverload->node->hasAttribute(ATTRIBUTE_CONSTEXPR))
                    {
                        Diagnostic diag{node->assignment, Err(Err0040)};
                        diag.hint = FMT(Nte(Nte0178), leftConcreteType->getDisplayNameC());
                        diag.addRange(node->assignToken, FMT(Nte(Nte0144), g_LangSpec->name_opAffect.c_str()));
                        return context->report(diag);
                    }
                }
            }

            // :opAffectConstExp
            else if (symbolFlags & (OVERLOAD_VAR_STRUCT | OVERLOAD_VAR_GLOBAL | OVERLOAD_CONSTANT))
                symbolFlags |= OVERLOAD_INCOMPLETE | OVERLOAD_STRUCT_AFFECT;
        }

        node->typeInfo = node->type->typeInfo;
    }

    // Only assignement is specified, need to deduce type
    else if (node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        node->typeInfo = TypeManager::concreteType(node->assignment->typeInfo, CONCRETE_FUNC);
        SWAG_ASSERT(node->typeInfo);

        // When affect is from a const struct/array, remove the const
        if (node->assignment->kind != AstNodeKind::Cast)
        {
            if (node->typeInfo->isArray() || node->typeInfo->isStruct())
                node->typeInfo = g_TypeMgr->makeUnConst(node->typeInfo);
        }

        if (node->typeInfo->isVoid())
        {
            if (node->assignment->typeInfo->isFuncAttr() && node->assignment->resolvedSymbolOverload)
            {
                auto nodeWhere = node->assignment;
                auto over      = nodeWhere->resolvedSymbolOverload;
                if (nodeWhere->kind == AstNodeKind::IdentifierRef)
                    nodeWhere = nodeWhere->childs.back();
                Diagnostic diag{nodeWhere, nodeWhere->token, Err(Err0371)};
                return context->report(diag, Diagnostic::hereIs(over));
            }

            Diagnostic diag{node->assignment, Err(Err0386)};
            return context->report(diag);
        }

        // We need to decide which integer/float type it is
        node->typeInfo = TypeManager::solidifyUntyped(node->typeInfo);

        // Convert from initialization list to array
        if (node->typeInfo->isListArray())
        {
            uint32_t castFlags = 0;
            if (!isCompilerConstant)
                castFlags = CASTFLAG_FORCE_UN_CONST;
            SWAG_CHECK(convertTypeListToArray(context, node, isCompilerConstant, symbolFlags, castFlags));
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
        node->inheritOrFlag(node->type, AST_IS_GENERIC);
    if (node->flags & AST_IS_GENERIC)
    {
        symbolFlags |= OVERLOAD_GENERIC;
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
        if (node->ownerFct && node->kind == AstNodeKind::FuncDeclParam && node->ownerFct->hasSpecFlag(AstFuncDecl::SPECFLAG_IS_LAMBDA_EXPRESSION))
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
            if (node->ownerFct && node->ownerScope->kind != ScopeKind::Struct)
                node->ownerFct->semFlags |= SEMFLAG_PENDING_LAMBDA_TYPING;
        }
    }

    // We should have a type here !
    SWAG_VERIFY(node->typeInfo, context->report({node, FMT(Err(Err0413), Naming::kindName(node).c_str(), node->token.ctext())}));

    // Type should be a correct one
    SWAG_VERIFY(!node->typeInfo->isPointerNull(), context->report({node->assignment ? node->assignment : node, Err(Err0414)}));
    SWAG_VERIFY(!node->typeInfo->isVoid(), context->report({node->type ? node->type : node, Err(Err0412)}));

    // A 'let' for a struct make the type const
    if (node->hasSpecFlag(AstVarDecl::SPECFLAG_IS_LET) && node->typeInfo->isStruct())
        node->typeInfo = g_TypeMgr->makeConst(node->typeInfo);

    // Determine if the call parameters cover everything (to avoid calling default initialization)
    // i.e. set AST_HAS_FULL_STRUCT_PARAMETERS
    if (node->type && (node->type->hasSpecFlag(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS)))
    {
        auto typeExpression = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        while (typeExpression->typeFlags & TYPEFLAG_IS_SUB_TYPE)
            typeExpression = castAst<AstTypeExpression>(typeExpression->childs.back(), AstNodeKind::TypeExpression);
        auto identifier = castAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);

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
            if (identifier->callParameters->childs.size() == typeStruct->flattenFields.size())
                node->addFlag(AST_HAS_FULL_STRUCT_PARAMETERS);
        }
    }

    // Force a constant to have a constant type, to avoid modifying a type that is in fact stored in the data segment,
    // and has an address
    if (isCompilerConstant && !(node->flags & AST_FROM_GENERIC))
    {
        if ((symbolFlags & OVERLOAD_VAR_GLOBAL) || isLocalConstant)
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
    if (!isCompilerConstant || !(node->flags & AST_FROM_GENERIC))
    {
        if (isCompilerConstant || (symbolFlags & OVERLOAD_VAR_GLOBAL) || (symbolFlags & OVERLOAD_VAR_LOCAL))
        {
            waitTypeCompleted(context->baseJob, typeInfo);
            YIELD();
        }
    }

    uint32_t     storageOffset  = UINT32_MAX;
    DataSegment* storageSegment = nullptr;
    if (isCompilerConstant)
    {
        node->addFlag(AST_NO_BYTECODE | AST_R_VALUE);
        if (!isGeneric)
        {
            SWAG_CHECK(collectConstantAssignment(context, &storageSegment, &storageOffset, symbolFlags));
            if (node->ownerFct)
                node->ownerFct->localConstants.push_back(node);
            else
                module->addGlobalVar(node, GlobalVarKind::Constant);
        }
    }
    else if (symbolFlags & OVERLOAD_VAR_STRUCT)
    {
        storageOffset  = 0;
        storageSegment = &module->constantSegment;
    }
    else if (symbolFlags & OVERLOAD_VAR_GLOBAL)
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
                    typeNode = ((TypeInfoArray*) typeNode)->finalType;
                TypeInfoStruct* typeStruct = castTypeInfo<TypeInfoStruct>(typeNode, TypeInfoKind::Struct);
                if (typeStruct->opDrop || typeStruct->opUserDropFct)
                    isGlobalToDrop = true;
            }
        }

        if (node->hasAttribute(ATTRIBUTE_PUBLIC))
        {
            Diagnostic diag{node, node->getTokenName(), Err(Err0479)};
            auto       note = Diagnostic::hereIs(node->findParent(TokenId::KwdPublic));
            return context->report(diag, note);
        }

        node->addFlag(AST_R_VALUE);
        storageSegment = getSegmentForVar(context, node);
        switch (storageSegment->kind)
        {
        case SegmentKind::Compiler:
            symbolFlags |= OVERLOAD_VAR_COMPILER;
            break;
        case SegmentKind::Bss:
            symbolFlags |= OVERLOAD_VAR_BSS;
            break;
        default:
            break;
        }

        if (node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload)
        {
            storageOffset = 0;
            symbolFlags |= OVERLOAD_INCOMPLETE;
        }
        else
        {
            SWAG_CHECK(collectAssignment(context, storageSegment, storageOffset, node));
        }

        // Register global variable
        if (!(symbolFlags & OVERLOAD_VAR_COMPILER))
        {
            if (node->hasAttribute(ATTRIBUTE_GLOBAL))
            {
                if (node->ownerFct)
                    node->ownerFct->localGlobalVars.push_back(context->node);
            }
            else
            {
                module->addGlobalVar(node, symbolFlags & OVERLOAD_VAR_BSS ? GlobalVarKind::Bss : GlobalVarKind::Mutable);
            }

            // Register global variable in the list of global variables to drop if the variable is
            // a struct with a 'opDrop' function
            if (isGlobalToDrop)
                module->addGlobalVarToDrop(node, storageOffset, storageSegment);
        }
    }
    else if (symbolFlags & OVERLOAD_VAR_LOCAL)
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
        if (node->type && node->type->kind == AstNodeKind::TypeExpression)
        {
            auto typeExpr = castAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
            if (typeExpr->typeFlags & TYPEFLAG_IS_RETVAL)
            {
                auto ownerFct   = getFunctionForReturn(node);
                auto typeFunc   = castTypeInfo<TypeInfoFuncAttr>(ownerFct->typeInfo, TypeInfoKind::FuncAttr);
                auto returnType = typeFunc->concreteReturnType();
                waitStructGenerated(context->baseJob, returnType);
                YIELD();

                if (!CallConv::returnStructByValue(typeFunc))
                    symbolFlags |= OVERLOAD_RETVAL;
            }
        }

        // If this is a tuple unpacking, then we just compute the stack offset of the item
        // inside the tuple, so we do not have to generate bytecode !
        if (node->assignment && node->assignment->flags & AST_TUPLE_UNPACK)
        {
            node->addFlag(AST_NO_BYTECODE_CHILDS);
            SWAG_ASSERT(node->assignment->kind == AstNodeKind::IdentifierRef);
            symbolFlags |= OVERLOAD_TUPLE_UNPACK;
            storageOffset = 0;
            for (auto& c : node->assignment->childs)
            {
                SWAG_ASSERT(c->resolvedSymbolOverload);
                storageOffset += c->resolvedSymbolOverload->computedValue.storageOffset;
            }
        }

        // Reserve room on the stack, except for a retval
        else if (!(symbolFlags & OVERLOAD_RETVAL))
        {
            auto assignment = node->assignment;
            if (assignment && (assignment->kind == AstNodeKind::Catch || assignment->kind == AstNodeKind::Try || assignment->kind == AstNodeKind::Assume))
                assignment = assignment->childs.front();

            // :DirectInlineLocalVar
            if (assignment &&
                assignment->kind == AstNodeKind::IdentifierRef &&
                !assignment->childs.back()->childs.empty() &&
                assignment->typeInfo == node->typeInfo &&
                assignment->childs.back()->childs.back()->kind == AstNodeKind::Inline &&
                assignment->childs.back()->childs.back()->flags & AST_TRANSIENT)
            {
                SWAG_ASSERT(assignment->childs.back()->childs.back()->computedValue);
                storageOffset = assignment->childs.back()->childs.back()->computedValue->storageOffset;
                node->addSpecFlag(AstVarDecl::SPECFLAG_INLINE_STORAGE);
            }
            else
            {
                auto alignOf = Semantic::alignOf(node);

                // Because of 'visit' (at least), it can happen that this is not up to date because of order of evaluation.
                // So update it just in case (5294 bug)
                node->ownerScope->startStackSize = max(node->ownerScope->startStackSize, node->ownerScope->parentScope->startStackSize);

                node->ownerScope->startStackSize = (uint32_t) TypeManager::align(node->ownerScope->startStackSize, alignOf);
                storageOffset                    = node->ownerScope->startStackSize;
                node->ownerScope->startStackSize += typeInfo->isStruct() ? max(typeInfo->sizeOf, 8) : typeInfo->sizeOf;
                setOwnerMaxStackSize(node, node->ownerScope->startStackSize);
            }
        }

        node->setBcNotifyBefore(ByteCodeGen::emitLocalVarDeclBefore);
        node->byteCodeFct = ByteCodeGen::emitLocalVarDecl;
        node->addFlag(AST_R_VALUE);
    }
    else if (symbolFlags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->addFlag(AST_R_VALUE);
        TypeManager::convertStructParamToRef(node, typeInfo);
    }

    // A using on a variable
    if (node->flags & AST_DECL_USING)
    {
        SWAG_CHECK(resolveUsingVar(context, context->node, node->typeInfo));
    }

    // Register symbol with its type
    AddSymbolTypeInfo toAdd;
    toAdd.node           = node;
    toAdd.typeInfo       = node->typeInfo;
    toAdd.kind           = thisIsAGenericType ? SymbolKind::GenericType : SymbolKind::Variable;
    toAdd.computedValue  = isCompilerConstant ? node->computedValue : nullptr;
    toAdd.flags          = symbolFlags;
    toAdd.storageOffset  = storageOffset;
    toAdd.storageSegment = storageSegment;

    auto overload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    SWAG_CHECK(overload);
    node->resolvedSymbolOverload = overload;
    return true;
}
