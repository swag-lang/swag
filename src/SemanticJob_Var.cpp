#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Generic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Naming.h"

uint32_t SemanticJob::alignOf(AstVarDecl* node)
{
    auto value = node->attributes.getValue(g_LangSpec->name_Swag_Align, g_LangSpec->name_value);
    if (value)
        return value->reg.u8;
    return TypeManager::alignOf(TypeManager::concreteType(node->typeInfo));
}

// Will be called after solving the initial var affect, but before tuple unpacking
bool SemanticJob::resolveTupleUnpackBeforeVar(SemanticContext* context)
{
    SWAG_CHECK(resolveVarDeclAfter(context));
    SWAG_CHECK(resolveTupleUnpackBefore(context));
    return true;
}

// Will be called after solving the initial var affect, but before tuple unpacking
bool SemanticJob::resolveTupleUnpackBefore(SemanticContext* context)
{
    auto scopeNode = CastAst<AstNode>(context->node->parent, AstNodeKind::StatementNoScope, AstNodeKind::Statement);
    auto varDecl   = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);

    auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
    if (typeVar->isListTuple() && !varDecl->type)
    {
        varDecl->semFlags |= SEMFLAG_TUPLE_CONVERT;
        SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, varDecl, varDecl->assignment, &varDecl->type));
        context->result = ContextResult::NewChilds;
        context->job->nodes.push_back(varDecl->type);
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
        Diagnostic diag{varDecl, varDecl->token, Fmt(Err(Err0291), typeVar->getDisplayNameC())};
        diag.hint = Hnt(Hnt0066);
        if (varDecl->assignment)
            diag.addRange(varDecl->assignment, Diagnostic::isType(TypeManager::concreteType(varDecl->assignment->typeInfo)));
        else if (varDecl->type)
            diag.addRange(varDecl->type, Diagnostic::isType(TypeManager::concreteType(varDecl->type->typeInfo)));
        return context->report(diag);
    }

    auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
    auto numUnpack  = scopeNode->childs.size() - 1;

    if (typeStruct->fields.size() == 0)
    {
        Diagnostic diag{varDecl, varDecl->token, Err(Err0292)};
        diag.hint = Hnt(Hnt0066);
        diag.addRange(varDecl->assignment, Hnt(Hnt0069));
        return context->report(diag);
    }

    if (numUnpack < typeStruct->fields.size())
    {
        Diagnostic diag{varDecl, varDecl->token, Fmt(Err(Err0293), numUnpack, typeStruct->fields.size())};
        diag.hint = Fmt(Hnt(Hnt0067), numUnpack);
        diag.addRange(varDecl->assignment, Fmt(Hnt(Hnt0068), typeStruct->fields.size()));
        PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, []()
                          { return Nte(Hlp0033); });
        return context->report(diag);
    }

    if (numUnpack > typeStruct->fields.size())
    {
        Diagnostic diag{varDecl, varDecl->token, Fmt(Err(Err0713), numUnpack, typeStruct->fields.size())};
        diag.hint = Fmt(Hnt(Hnt0067), numUnpack);
        diag.addRange(varDecl->assignment, Fmt(Hnt(Hnt0068), typeStruct->fields.size()));
        return context->report(diag);
    }

    return true;
}

void SemanticJob::setVarDeclResolve(AstVarDecl* varNode)
{
    varNode->allocateExtension(ExtensionKind::Semantic);
    varNode->extSemantic()->semanticBeforeFct = SemanticJob::resolveVarDeclBefore;
    varNode->extSemantic()->semanticAfterFct  = SemanticJob::resolveVarDeclAfter;

    if (varNode->assignment)
    {
        varNode->assignment->allocateExtension(ExtensionKind::Semantic);
        varNode->assignment->extSemantic()->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
    }

    if (varNode->assignment && varNode->type)
    {
        varNode->type->allocateExtension(ExtensionKind::Semantic);
        varNode->type->extSemantic()->semanticAfterFct = SemanticJob::resolveVarDeclAfterType;
    }
}

bool SemanticJob::resolveVarDeclAfterType(SemanticContext* context)
{
    // :DeduceLambdaType
    resolveAfterKnownType(context);

    auto parent = context->node->parent;
    while (parent && parent->kind != AstNodeKind::VarDecl && parent->kind != AstNodeKind::ConstDecl && parent->kind != AstNodeKind::FuncDeclParam)
        parent = parent->parent;
    SWAG_ASSERT(parent);
    auto varDecl = (AstVarDecl*) parent;
    if (!varDecl->type || !varDecl->assignment)
        return true;

    if (parent->kind == AstNodeKind::FuncDeclParam)
    {
        if (varDecl->type->typeInfo->isTypedVariadic() ||
            varDecl->type->typeInfo->isVariadic() ||
            varDecl->type->typeInfo->isCVariadic())
        {
            Diagnostic diag{varDecl, varDecl->assignToken, Err(Err0685)};
            diag.hint = Hnt(Hnt0061);
            diag.addRange(varDecl->type, Diagnostic::isType(varDecl->type->typeInfo));
            return context->report(diag);
        }
    }

    // :AutoScope
    // Resolution of an affectation to an enum, without having to specify the enum name before
    // 'using', but just for affectation
    auto typeInfo = TypeManager::concreteType(varDecl->type->typeInfo, CONCRETE_FORCEALIAS);
    if (typeInfo->isEnum())
    {
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        varDecl->assignment->addAlternativeScope(typeEnum->scope);
    }
    else if (typeInfo->isArray())
    {
        auto typeArr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        if (typeArr->finalType->isEnum())
        {
            auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeArr->finalType, TypeInfoKind::Enum);
            varDecl->assignment->addAlternativeScope(typeEnum->scope);
        }
    }

    return true;
}

bool SemanticJob::resolveVarDeclAfter(SemanticContext* context)
{
    auto node = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

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
        auto id = createTmpId(context, node, node->token.text);
        SWAG_CHECK(resolveIdentifier(context, id, RI_FOR_GHOSTING));
        if (context->result != ContextResult::Done)
            return true;
    }

    // :opAffectConstExpr
    if (node->resolvedSymbolOverload &&
        node->resolvedSymbolOverload->flags & OVERLOAD_STRUCT_AFFECT &&
        node->resolvedSymbolOverload->flags & (OVERLOAD_VAR_GLOBAL | OVERLOAD_VAR_STRUCT | OVERLOAD_CONSTANT))
    {
        auto overload = node->resolvedSymbolOverload;
        SWAG_ASSERT(overload->flags & OVERLOAD_INCOMPLETE);

        node->flags &= ~AST_NO_BYTECODE;
        node->flags &= ~AST_VALUE_COMPUTED;
        node->assignment->flags &= ~AST_NO_BYTECODE;
        node->flags |= AST_CONST_EXPR;
        node->semFlags |= SEMFLAG_EXEC_RET_STACK;

        node->byteCodeFct                     = ByteCodeGenJob::emitLocalVarDecl;
        overload->computedValue.storageOffset = 0;

        SWAG_CHECK(evaluateConstExpression(context, node));
        if (context->result == ContextResult::Pending)
            return true;

        node->flags |= AST_NO_BYTECODE;
        node->assignment->flags |= AST_NO_BYTECODE;

        SWAG_ASSERT(node->computedValue->storageSegment);
        SWAG_ASSERT(node->computedValue->storageOffset != UINT32_MAX);

        auto wantStorageSegment = SemanticJob::getConstantSegFromContext(node);

        // Copy value from compiler segment to real requested segment
        if (node->computedValue->storageSegment != wantStorageSegment)
        {
            auto     addrSrc = node->computedValue->getStorageAddr();
            uint8_t* addrDst;
            auto     storageOffset              = wantStorageSegment->reserve(node->typeInfo->sizeOf, &addrDst);
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

bool SemanticJob::sendCompilerMsgGlobalVar(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto node       = context->node;

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

bool SemanticJob::resolveVarDeclBefore(SemanticContext* context)
{
    auto node = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // Collect all attributes for the variable
    SWAG_CHECK(collectAttributes(context, node, &node->attributes));

    if (node->assignment && node->kind == AstNodeKind::ConstDecl)
    {
        bool isGeneric = false;
        if (node->flags & AST_STRUCT_MEMBER)
        {
            auto parent = node->findParent(AstNodeKind::StructDecl);
            if (parent)
                isGeneric = parent->flags & AST_IS_GENERIC;
        }

        if (isGeneric)
        {
            node->assignment->typeInfo = g_TypeMgr->typeInfoS32;
            node->assignment->flags |= AST_NO_SEMANTIC;
        }
        else
        {
            node->assignment->flags &= ~AST_NO_SEMANTIC;
        }
    }

    return true;
}

bool SemanticJob::resolveVarDeclAfterAssign(SemanticContext* context)
{
    auto job = context->job;

    auto parent = context->node->parent;
    while (parent && parent->kind != AstNodeKind::VarDecl && parent->kind != AstNodeKind::ConstDecl)
        parent = parent->parent;
    SWAG_ASSERT(parent);
    auto varDecl = (AstVarDecl*) parent;

    auto assign = varDecl->assignment;
    if (!assign || assign->kind != AstNodeKind::ExpressionList)
        return true;

    auto exprList = CastAst<AstExpressionList>(assign, AstNodeKind::ExpressionList);
    if (!(exprList->specFlags & AstExpressionList::SPECFLAG_FOR_TUPLE))
        return true;

    // If there's an assignment, but no type, then we need to deduce/generate the type with
    // the assignment, then do the semmantic on that type
    if (!varDecl->type)
    {
        SWAG_CHECK(Ast::convertLiteralTupleToStructDecl(context, varDecl, assign, &varDecl->type));
        varDecl->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;
        context->result = ContextResult::Done;
        job->nodes.pop_back();
        job->nodes.push_back(varDecl->type);
        job->nodes.push_back(context->node);
    }

    // Here we will transform the struct literal to a type initialization
    auto typeExpression = CastAst<AstTypeExpression>(varDecl->type, AstNodeKind::TypeExpression);
    if (!typeExpression->identifier)
        return true;

    auto identifier = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
    if (identifier->callParameters)
    {
        Diagnostic diag{assign, Err(Err0295)};
        diag.hint = Hnt(Hnt0009);
        diag.addRange(identifier->callParameters, Hnt(Hnt0007));
        return context->report(diag);
    }

    auto sourceFile            = context->sourceFile;
    identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);

    auto numParams = assign->childs.size();
    for (size_t i = 0; i < numParams; i++)
    {
        auto child = assign->childs[0];
        auto param = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        Ast::removeFromParent(child);
        Ast::addChildBack(param, child);
        child->semFlags |= SEMFLAG_TYPE_SOLVED;
        param->inheritTokenLocation(child);
    }

    identifier->callParameters->inheritTokenLocation(varDecl->assignment);
    identifier->callParameters->inheritOrFlag(varDecl->assignment, AST_CONST_EXPR | AST_SIDE_EFFECTS);
    identifier->callParameters->specFlags |= AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT;
    identifier->flags |= AST_IN_TYPE_VAR_DECLARATION;
    typeExpression->flags &= ~AST_NO_BYTECODE;
    typeExpression->flags &= ~AST_NO_BYTECODE_CHILDS;
    typeExpression->flags &= ~AST_VALUE_COMPUTED;
    typeExpression->specFlags |= AstType::SPECFLAG_HAS_STRUCT_PARAMETERS;

    Ast::removeFromParent(varDecl->assignment);
    varDecl->assignment->release();
    varDecl->assignment = nullptr;

    job->nodes.pop_back();
    typeExpression->semanticState = AstNodeResolveState::Enter;
    job->nodes.push_back(typeExpression);
    job->nodes.push_back(context->node);

    return true;
}

bool SemanticJob::convertTypeListToArray(SemanticContext* context, AstVarDecl* node, bool isCompilerConstant, uint32_t symbolFlags, uint32_t castFlags)
{
    auto typeList  = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListArray);
    auto typeArray = TypeManager::convertTypeListToArray(context, typeList, isCompilerConstant);
    node->typeInfo = typeArray;

    // For a global variable, no need to collect in the constant segment, as we will collect directly to the mutable segment
    if (symbolFlags & OVERLOAD_VAR_GLOBAL)
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment, castFlags | CASTFLAG_NO_COLLECT));
    else
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment, castFlags));
    node->typeInfo->sizeOf = node->assignment->typeInfo->sizeOf;

    return true;
}

DataSegment* SemanticJob::getSegmentForVar(SemanticContext* context, AstVarDecl* varNode)
{
    auto module   = varNode->sourceFile->module;
    auto typeInfo = TypeManager::concreteType(varNode->typeInfo);

    if (varNode->attributeFlags & ATTRIBUTE_TLS)
        return &module->tlsSegment;

    if (isCompilerContext(varNode))
        return &module->compilerSegment;

    if (varNode->isConstDecl())
        return &module->constantSegment;
    if (varNode->resolvedSymbolOverload && (varNode->resolvedSymbolOverload->flags & OVERLOAD_VAR_STRUCT))
        return &module->constantSegment;

    if (varNode->attributeFlags & AST_EXPLICITLY_NOT_INITIALIZED)
        return &module->mutableSegment;

    // An array of struct with default values should go to the mutable segment
    if (varNode->typeInfo->isArrayOfStruct())
    {
        auto typeArr = CastTypeInfo<TypeInfoArray>(varNode->typeInfo, TypeInfoKind::Array);
        if (typeArr->finalType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
            return &module->mutableSegment;
    }

    if (!varNode->assignment && (typeInfo->isNative() || typeInfo->isArray()))
        return &module->bssSegment;
    if (varNode->assignment && typeInfo->isNative() && typeInfo->sizeOf <= 8 && varNode->assignment->isConstant0())
        return &module->bssSegment;
    if (!varNode->assignment &&
        (!varNode->type || !(varNode->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS)) &&
        !(varNode->flags & AST_HAS_FULL_STRUCT_PARAMETERS) &&
        (varNode->typeInfo->isStruct() || varNode->typeInfo->isInterface()) &&
        !(varNode->typeInfo->flags & (TYPEINFO_STRUCT_HAS_INIT_VALUES)))
        return &module->bssSegment;

    return &module->mutableSegment;
}

// :DeduceLambdaType
TypeInfo* SemanticJob::getDeducedLambdaType(SemanticContext* context, AstMakePointer* node)
{
    SWAG_ASSERT(node->specFlags & AstMakePointer::SPECFLAG_DEP_TYPE);

    TypeInfo* result = node->deducedLambdaType;
    if (result)
        return result;

    if (node->parent->kind == AstNodeKind::AffectOp)
    {
        result = node->parent->childs.front()->typeInfo;
    }
    else if (node->parent->kind == AstNodeKind::VarDecl)
    {
        auto varDecl = CastAst<AstVarDecl>(node->parent, AstNodeKind::VarDecl);
        SWAG_ASSERT(varDecl->type);
        result = varDecl->type->typeInfo;
    }

    SWAG_ASSERT(result);
    return TypeManager::concreteType(result, CONCRETE_FORCEALIAS);
}

// :DeduceLambdaType
bool SemanticJob::deduceLambdaParamTypeFrom(SemanticContext* context, AstVarDecl* nodeParam, bool& lambdaExpr, bool& genericType)
{
    auto mpl = nodeParam->ownerFct->makePointerLambda;
    if (!mpl || !(mpl->specFlags & AstMakePointer::SPECFLAG_DEP_TYPE))
        return true;

    auto frontType = getDeducedLambdaType(context, mpl);
    frontType      = TypeManager::concreteType(frontType);

    TypeInfoFuncAttr* typeLambda = nullptr;
    if (frontType->isStruct())
    {
        auto op    = mpl->parent;
        auto front = op->childs.front();
        SWAG_ASSERT(op->kind == AstNodeKind::AffectOp);

        typeLambda = mpl->deducedLambdaType;
        if (!typeLambda)
        {
            // Generate an undefined type to make the match
            if (!mpl->tryLambdaType)
            {
                auto tryType  = makeType<TypeInfoFuncAttr>();
                tryType->kind = TypeInfoKind::LambdaClosure;
                if (nodeParam->ownerFct->captureParameters)
                    tryType->flags |= TYPEINFO_CLOSURE;

                for (size_t i = 0; i < nodeParam->ownerFct->parameters->childs.size(); i++)
                {
                    auto p      = g_TypeMgr->makeParam();
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
                if (context->result != ContextResult::Done)
                    return true;
            }
            else
            {
                mpl->typeInfo = mpl->tryLambdaType;
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op->token.text, nullptr, front, mpl, ROP_SIMPLE_CAST));
                if (context->result != ContextResult::Done)
                    return true;
            }

            if (context->job->cacheMatches.empty() || context->job->cacheMatches.size() > 1)
                return true;

            auto typeOverload = CastTypeInfo<TypeInfoFuncAttr>(context->job->cacheMatches[0]->oneOverload->overload->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeOverload->parameters[1]->typeInfo->isLambdaClosure())
                return true;

            typeLambda = CastTypeInfo<TypeInfoFuncAttr>(typeOverload->parameters[1]->typeInfo, TypeInfoKind::LambdaClosure);
            if (!typeLambda)
                return true;
        }
    }
    else
    {
        SWAG_ASSERT(frontType->isLambdaClosure());
        typeLambda = CastTypeInfo<TypeInfoFuncAttr>(frontType, TypeInfoKind::LambdaClosure);
    }

    auto paramIdx = nodeParam->childParentIdx();

    // Do not deduce from the context closure generated parameter
    SWAG_ASSERT(typeLambda);
    if (typeLambda->isClosure() && !nodeParam->ownerFct->captureParameters)
        paramIdx += 1;

    if (paramIdx >= (uint32_t) typeLambda->parameters.count)
    {
        Diagnostic diag{nodeParam, Fmt(Err(Err0026), (uint32_t) typeLambda->parameters.count, (uint32_t) nodeParam->parent->childs.count)};
        diag.hint = Hnt(Hnt0026);
        return context->report(diag);
    }

    mpl->deducedLambdaType = typeLambda;
    nodeParam->typeInfo    = typeLambda->parameters[paramIdx]->typeInfo;
    lambdaExpr             = false;
    genericType            = false;

    return true;
}

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto node       = static_cast<AstVarDecl*>(context->node);

    bool isCompilerConstant = node->kind == AstNodeKind::ConstDecl ? true : false;
    bool isLocalConstant    = false;

    // Check @mixin
    if (!(node->flags & AST_GENERATED) && !(node->ownerInline) && node->token.text[0] == '@' && node->token.text.find(g_LangSpec->name_atmixin) == 0)
    {
        auto ownerFct = node->ownerFct;
        while (ownerFct)
        {
            if (ownerFct->attributeFlags & ATTRIBUTE_MIXIN)
                break;
            ownerFct = ownerFct->ownerFct;
        }

        if (!ownerFct)
        {
            auto note = Diagnostic::note(Nte(Hlp0020));
            return context->report({node, Fmt(Err(Syn0112), node->token.ctext())}, note);
        }
    }

    // Check @alias
    if (!(node->flags & AST_GENERATED) && !(node->ownerInline) && node->token.text[0] == '@' && node->token.text.find(g_LangSpec->name_atalias) == 0)
    {
        auto ownerFct = node->ownerFct;
        while (ownerFct)
        {
            if (ownerFct->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN))
                break;
            ownerFct = ownerFct->ownerFct;
        }

        if (!ownerFct)
        {
            auto note = Diagnostic::note(Nte(Hlp0010));
            return context->report({node, node->token, Fmt(Err(Syn0112), node->token.ctext())}, note);
        }
    }

    if (node->assignment && node->assignment->semFlags & SEMFLAG_LITERAL_SUFFIX)
    {
        if (!node->type || !node->type->typeInfo->isStruct())
            return context->report({node->assignment->childs.front(), Fmt(Err(Err0532), node->assignment->childs.front()->token.ctext())});
    }

    uint32_t symbolFlags = 0;

    // Transform let to constant if possible
    if (node->specFlags & AstVarDecl::SPECFLAG_IS_LET)
    {
        if (node->assignment &&
            node->assignment->hasComputedValue() &&
            !node->assignment->typeInfo->isLambdaClosure() &&
            (!node->type || !node->type->typeInfo->isStruct()) &&
            (!node->assignment->typeInfo->isPointer() || node->assignment->typeInfo->isPointerToTypeInfo()))
        {
            node->specFlags |= AstVarDecl::SPECFLAG_IS_LET_TO_CONST;
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
    else if (node->ownerScope->isGlobal() || (node->attributeFlags & ATTRIBUTE_GLOBAL))
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->isGlobalOrImpl() && (node->flags & AST_INSIDE_IMPL) && !(node->flags & AST_STRUCT_MEMBER))
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->kind == ScopeKind::Struct)
        symbolFlags |= OVERLOAD_VAR_STRUCT;
    else if (!isCompilerConstant)
        symbolFlags |= OVERLOAD_VAR_LOCAL;
    else
        isLocalConstant = true;
    if (node->specFlags & AstVarDecl::SPECFLAG_CONST_ASSIGN)
        symbolFlags |= OVERLOAD_CONST_ASSIGN;
    if (node->attributeFlags & ATTRIBUTE_TLS)
        symbolFlags |= OVERLOAD_VAR_TLS;
    if (node->assignment)
        symbolFlags |= OVERLOAD_VAR_HAS_ASSIGN;

    auto concreteNodeType = node->type && node->type->typeInfo ? TypeManager::concreteType(node->type->typeInfo, CONCRETE_FORCEALIAS) : nullptr;

    // Register public global constant
    if (isCompilerConstant && (node->attributeFlags & ATTRIBUTE_PUBLIC))
    {
        if (node->ownerScope->isGlobalOrImpl() && (node->type || node->assignment))
            node->ownerScope->addPublicNode(node);
    }

    if (node->attributeFlags & ATTRIBUTE_DISCARDABLE && !concreteNodeType->isLambdaClosure())
        return context->report({node, node->token, Fmt(Err(Err0297), concreteNodeType->getDisplayNameC())});

    // Check for missing initialization
    // This is ok to not have an initialization for structs, as they are initialized by default
    if (!node->assignment && (!node->type || !concreteNodeType->isStruct()))
    {
        // A constant must be initialized
        if (isCompilerConstant && !node->hasComputedValue())
            return context->report({node, Err(Err0298)});
        // A constant variable must be initialized
        if ((symbolFlags & OVERLOAD_CONST_ASSIGN) && node->kind != AstNodeKind::FuncDeclParam)
        {
            if (symbolFlags & OVERLOAD_IS_LET)
                return context->report({node, Err(Err0436)});
            return context->report({node, Err(Err0299)});
        }

        // A reference must be initialized
        if (concreteNodeType &&
            concreteNodeType->isPointerRef() &&
            node->kind != AstNodeKind::FuncDeclParam &&
            !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
            return context->report({node, Err(Err0300)});

        // Check an enum variable without initialization
        if (concreteNodeType &&
            concreteNodeType->isEnum() &&
            node->kind != AstNodeKind::FuncDeclParam &&
            !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
        {
            auto typeEnum = CastTypeInfo<TypeInfoEnum>(concreteNodeType, TypeInfoKind::Enum);

            bool ok = true;
            if (typeEnum->rawType->isNativeFloat() || typeEnum->rawType->isNativeIntegerOrRune() || typeEnum->rawType->isBool())
            {
                ok = false;
                for (auto p : typeEnum->values)
                {
                    if (!p->value->reg.u64)
                    {
                        ok = true;
                        break;
                    }
                }
            }
            else if (typeEnum->rawType->isString())
            {
                ok = false;
                for (auto p : typeEnum->values)
                {
                    if (!p->value->text.buffer)
                    {
                        ok = true;
                        break;
                    }
                }
            }
            else if (typeEnum->rawType->isSlice())
            {
                ok = false;
                for (auto p : typeEnum->values)
                {
                    SWAG_ASSERT(p->value);
                    auto slice = (SwagSlice*) p->value->getStorageAddr();
                    if (!slice->buffer && !slice->count)
                    {
                        ok = true;
                        break;
                    }
                }
            }

            if (!ok)
            {
                Diagnostic diag{node, Fmt(Err(Err0848), node->token.ctext(), typeEnum->getDisplayNameC())};
                diag.hint = Hnt(Hnt0055);
                auto note = Diagnostic::note(concreteNodeType->declNode, concreteNodeType->declNode->token, Fmt(Nte(Nte0027), concreteNodeType->getDisplayNameC()));
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
        SWAG_ASSERT(node->specFlags & AstVarDecl::SPECFLAG_GENERIC_TYPE);

        auto typeRet = TypeManager::concreteType(node->typeConstraint->typeInfo, CONCRETE_ALL);
        if (!typeRet->isBool())
        {
            Diagnostic diag{node->typeConstraint, Fmt(Err(Err0678), typeRet->getDisplayNameC())};
            diag.hint = Diagnostic::isType(node->typeConstraint);
            return context->report(diag);
        }

        SWAG_CHECK(checkIsConstExpr(context, node->typeConstraint, Err(Err0128)));
        SWAG_CHECK(evaluateConstExpression(context, node->typeConstraint));
        if (context->result != ContextResult::Done)
            return true;
        SWAG_ASSERT(node->typeConstraint->computedValue);
        if (!node->typeConstraint->computedValue->reg.b)
        {
            Diagnostic diag(node->typeConstraint, Fmt(Err(Err0118), node->typeInfo->getDisplayNameC()));
            if (node->genTypeComesFrom && node->typeConstraint->kind == AstNodeKind::IdentifierRef)
            {
                auto note = Diagnostic::note(node->genTypeComesFrom, Fmt(Nte(Nte0079), node->typeInfo->getDisplayNameC(), node->typeConstraint->token.ctext()));
                return context->report(diag, note);
            }
            else if (node->genTypeComesFrom)
            {
                auto note = Diagnostic::note(node->genTypeComesFrom, Fmt(Nte(Nte0080), node->typeInfo->getDisplayNameC()));
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
        if ((node->flags & AST_IS_GENERIC) && !node->type && !(node->flags & AST_R_VALUE) && !(node->specFlags & AstVarDecl::SPECFLAG_GENERIC_CONSTANT))
        {
            thisIsAGenericType = true;
        }
        else if (!(node->flags & AST_FROM_GENERIC) || !(node->semFlags & SEMFLAG_ASSIGN_COMPUTED))
        {
            SWAG_CHECK(checkIsConcreteOrType(context, node->assignment));
            if (context->result == ContextResult::Pending)
                return true;

            if ((symbolFlags & OVERLOAD_VAR_GLOBAL) || (symbolFlags & OVERLOAD_VAR_FUNC_PARAM) || (node->assignment->flags & AST_CONST_EXPR))
            {
                SWAG_CHECK(evaluateConstExpression(context, node->assignment));
                if (context->result == ContextResult::Pending)
                    return true;
                if (symbolFlags & OVERLOAD_VAR_GLOBAL)
                    node->assignment->flags |= AST_NO_BYTECODE;
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
                SWAG_VERIFY(!isCompilerConstant, context->report({node->assignment, Err(Err0160)}));
                auto funcNode = CastAst<AstFuncDecl>(node->assignment->typeInfo->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda);
                SWAG_CHECK(checkCanMakeFuncPointer(context, funcNode, node->assignment));
            }
            else
            {
                SWAG_CHECK(checkIsConstExpr(context, node->assignment->flags & AST_CONST_EXPR, node->assignment, Err(Err0670)));
            }
        }
    }

    // Be sure that an array without a size has an initializer, to deduce its size
    if (concreteNodeType && concreteNodeType->isArray())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(concreteNodeType, TypeInfoKind::Array);
        if (typeArray->count == UINT32_MAX && !node->assignment)
            return context->report({node->type, Err(Err0303), Hnt(Hnt0072)});

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count      = (uint32_t) node->assignment->childs.size();
            typeArray->totalCount = typeArray->count;
            typeArray->sizeOf     = typeArray->count * typeArray->pointedType->sizeOf;
            typeArray->name       = Fmt("[%d] %s", typeArray->count, typeArray->pointedType->getDisplayNameC());
        }
    }

    if (node->flags & AST_EXPLICITLY_NOT_INITIALIZED)
    {
        symbolFlags |= OVERLOAD_NOT_INITIALIZED;

        if (isCompilerConstant)
        {
            Diagnostic diag{node->assignment, Err(Err0298)};
            diag.hint = Hnt(Hnt0061);
            return context->report(diag);
        }

        if (node->specFlags & AstVarDecl::SPECFLAG_IS_LET)
        {
            Diagnostic diag{node->assignment, Err(Err0873)};
            diag.hint = Hnt(Hnt0061);
            return context->report(diag);
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
                context->job->waitAllStructInterfaces(rightConcreteType);
                if (context->result != ContextResult::Done)
                    return true;
            }

            auto castFlags = CASTFLAG_TRY_COERCE | CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST | CASTFLAG_PTR_REF | CASTFLAG_FOR_AFFECT | CASTFLAG_FOR_VAR_INIT;

            if (node->type->flags & AST_FROM_GENERIC_REPLACE || (node->type->childs.count && node->type->childs.back()->flags & AST_FROM_GENERIC_REPLACE))
            {
                PushErrCxtStep ec{context, node->type, ErrCxtStepKind::Hint2, [node]()
                                  { return Diagnostic::isType(node->type->typeInfo); }};
                SWAG_CHECK(TypeManager::makeCompatibles(context, node->type->typeInfo, nullptr, node->assignment, castFlags));
            }
            else
            {
                SWAG_CHECK(TypeManager::makeCompatibles(context, node->type->typeInfo, nullptr, node->assignment, castFlags));
            }

            if (context->result != ContextResult::Done)
                return true;

            // :ConcreteRef
            if (!leftConcreteType->isPointerRef() && TypeManager::concreteType(node->assignment->typeInfo)->isPointerRef())
                setUnRef(node->assignment);
        }
        else
        {
            if ((leftConcreteType->kind != rightConcreteType->kind) || !rightConcreteType->isSame(leftConcreteType, CASTFLAG_CAST))
            {
                SWAG_CHECK(resolveUserOpAffect(context, leftConcreteType, rightConcreteType, node->type, node->assignment));
                if (context->result != ContextResult::Done)
                    return true;

                // :opAffectConstExpr
                if (symbolFlags & (OVERLOAD_VAR_STRUCT | OVERLOAD_VAR_GLOBAL | OVERLOAD_CONSTANT))
                {
                    symbolFlags |= OVERLOAD_INCOMPLETE | OVERLOAD_STRUCT_AFFECT;
                    SWAG_ASSERT(node->hasExtMisc() && node->extMisc()->resolvedUserOpSymbolOverload);
                    if (!(node->extMisc()->resolvedUserOpSymbolOverload->node->attributeFlags & ATTRIBUTE_CONSTEXPR))
                    {
                        Diagnostic diag{node->assignment, Err(Err0906)};
                        diag.hint = Fmt(Hnt(Hnt0002), leftConcreteType->getDisplayNameC());
                        diag.addRange(node->assignToken, Fmt(Hnt(Hnt0047), g_LangSpec->name_opAffect.c_str()));
                        return context->report(diag);
                    }
                }
            }

            // :opAffectConstExp
            else if (symbolFlags & (OVERLOAD_VAR_STRUCT | OVERLOAD_VAR_GLOBAL | OVERLOAD_CONSTANT))
                symbolFlags |= OVERLOAD_INCOMPLETE | OVERLOAD_STRUCT_AFFECT;
        }

        node->typeInfo = node->type->typeInfo;

        // A slice initialized with an expression list must be immutable
        if (leftConcreteType->isSlice() && rightConcreteType->isListArray() && (node->assignment->flags & AST_CONST_EXPR))
        {
            if (!leftConcreteType->isConst())
            {
                Diagnostic diag{node->type, Err(Err0306), Fmt(Hnt(Hnt0004), leftConcreteType->getDisplayNameC())};
                diag.addRange(node->assignment, Diagnostic::isType(rightConcreteType));
                return context->report(diag);
            }
        }
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
                Diagnostic diag{nodeWhere, nodeWhere->token, Err(Err0163)};
                diag.hint = Hnt(Hnt0034);
                return context->report(diag, Diagnostic::hereIs(over));
            }

            Diagnostic diag{node->assignment, Err(Err0307)};
            return context->report(diag);
        }

        // We need to decide which integer/float type it is
        node->typeInfo = TypeManager::solidifyUntyped(node->typeInfo);

        // Convert from initialization list to array
        if (node->typeInfo->isListArray())
        {
            uint32_t castFlags = 0;
            if (!isCompilerConstant)
                castFlags = CASTFLAG_FORCE_UNCONST;
            SWAG_CHECK(convertTypeListToArray(context, node, isCompilerConstant, symbolFlags, castFlags));
        }

        // :ConcreteRef
        if (node->typeInfo->isPointerRef() && setUnRef(node->assignment))
        {
            auto typePointer = CastTypeInfo<TypeInfoPointer>(node->typeInfo, TypeInfoKind::Pointer);
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
        if (node->ownerFct && node->kind == AstNodeKind::FuncDeclParam && (node->ownerFct->specFlags & AstFuncDecl::SPECFLAG_IS_LAMBDA_EXPRESSION))
            lambdaExpr = true;
        if (lambdaExpr)
        {
            SWAG_CHECK(deduceLambdaParamTypeFrom(context, node, lambdaExpr, thisIsAGenericType));
            if (context->result != ContextResult::Done)
                return true;
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
    SWAG_VERIFY(node->typeInfo, context->report({node, Fmt(Err(Err0309), Naming::kindName(node).c_str(), node->token.ctext())}));

    // Type should be a correct one
    SWAG_VERIFY(!node->typeInfo->isPointerNull(), context->report({node, Err(Err0308)}));
    SWAG_VERIFY(!node->typeInfo->isVoid(), context->report({node, Err(Err0149)}));

    // A 'let' for a struct make the type const
    if (node->specFlags & AstVarDecl::SPECFLAG_IS_LET && node->typeInfo->isStruct())
        node->typeInfo = g_TypeMgr->makeConst(node->typeInfo);

    // Determine if the call parameters cover everything (to avoid calling default initialization)
    // i.e. set AST_HAS_FULL_STRUCT_PARAMETERS
    if (node->type && (node->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS))
    {
        auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        while (typeExpression->typeFlags & TYPEFLAG_IS_SUB_TYPE)
            typeExpression = CastAst<AstTypeExpression>(typeExpression->childs.back(), AstNodeKind::TypeExpression);
        auto identifier = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);

        TypeInfoStruct* typeStruct = nullptr;
        if (node->typeInfo->isStruct())
            typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        else if (node->typeInfo->isArray())
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(node->typeInfo, TypeInfoKind::Array);
            while (typeArray->pointedType->isArray())
                typeArray = CastTypeInfo<TypeInfoArray>(typeArray->pointedType, TypeInfoKind::Array);
            typeStruct = CastTypeInfo<TypeInfoStruct>(typeArray->pointedType, TypeInfoKind::Struct);
        }

        if (typeStruct && identifier->callParameters && identifier->callParameters->childs.size() == typeStruct->fields.size())
            node->flags |= AST_HAS_FULL_STRUCT_PARAMETERS;
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
            context->job->waitTypeCompleted(typeInfo);
            if (context->result == ContextResult::Pending)
                return true;
        }
    }

    uint32_t     storageOffset  = UINT32_MAX;
    DataSegment* storageSegment = nullptr;
    if (isCompilerConstant)
    {
        node->flags |= AST_NO_BYTECODE | AST_R_VALUE;
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
            if (context->result != ContextResult::Done)
                return true;
        }

        // Register global variable in the list of global variables to drop if the variable is
        // a struct with a 'opDrop' function
        auto isGlobalToDrop = false;
        if (node->typeInfo && node->typeInfo->isStruct())
        {
            context->job->waitStructGeneratedAlloc(node->typeInfo);
            if (context->result != ContextResult::Done)
                return true;
            TypeInfoStruct* typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            if (typeStruct->opDrop || typeStruct->opUserDropFct)
                isGlobalToDrop = true;
        }

        SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_PUBLIC), context->report({node, Err(Err0313)}));

        node->flags |= AST_R_VALUE;
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
            if (node->attributeFlags & ATTRIBUTE_GLOBAL)
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
            if (context->result != ContextResult::Done)
                return true;
        }

        // Variable is still generic. Try to find default generic parameters to instantiate it
        if (node->typeInfo->isGeneric())
        {
            SWAG_CHECK(Generic::instantiateDefaultGenericVar(context, node));
            if (context->result != ContextResult::Done)
                return true;
        }

        SWAG_ASSERT(node->ownerScope);

        // Do not allocate space on the stack for a 'retval' variable, because it's not really a variable
        if (node->type && node->type->kind == AstNodeKind::TypeExpression)
        {
            auto typeExpr = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
            if (typeExpr->typeFlags & TYPEFLAG_IS_RETVAL)
            {
                auto ownerFct   = getFunctionForReturn(node);
                auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(ownerFct->typeInfo, TypeInfoKind::FuncAttr);
                auto returnType = typeFunc->concreteReturnType();
                context->job->waitStructGenerated(returnType);
                if (context->result != ContextResult::Done)
                    return true;

                if (!CallConv::returnStructByValue(typeFunc))
                    symbolFlags |= OVERLOAD_RETVAL;
            }
        }

        // If this is a tuple unpacking, then we just compute the stack offset of the item
        // inside the tuple, so we do not have to generate bytecode !
        if (node->assignment && node->assignment->flags & AST_TUPLE_UNPACK)
        {
            node->flags |= AST_NO_BYTECODE_CHILDS;
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
                assignment->childs.back()->childs.size() &&
                assignment->typeInfo == node->typeInfo &&
                assignment->childs.back()->childs.back()->kind == AstNodeKind::Inline &&
                assignment->childs.back()->childs.back()->flags & AST_TRANSIENT)
            {
                SWAG_ASSERT(assignment->childs.back()->childs.back()->computedValue);
                storageOffset = assignment->childs.back()->childs.back()->computedValue->storageOffset;
                node->specFlags |= AstVarDecl::SPECFLAG_INLINE_STORAGE;
            }
            else
            {
                auto alignOf                     = SemanticJob::alignOf(node);
                node->ownerScope->startStackSize = (uint32_t) TypeManager::align(node->ownerScope->startStackSize, alignOf);
                storageOffset                    = node->ownerScope->startStackSize;
                node->ownerScope->startStackSize += typeInfo->isStruct() ? max(typeInfo->sizeOf, 8) : typeInfo->sizeOf;
                SemanticJob::setOwnerMaxStackSize(node, node->ownerScope->startStackSize);
            }
        }

        node->setBcNotifBefore(ByteCodeGenJob::emitLocalVarDeclBefore);
        node->byteCodeFct = ByteCodeGenJob::emitLocalVarDecl;
        node->flags |= AST_R_VALUE;
    }
    else if (symbolFlags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->flags |= AST_R_VALUE;
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
