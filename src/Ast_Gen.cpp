#include "pch.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "SemanticJob.h"
#include "TypeManager.h"

atomic<int> g_UniqueID;

bool Ast::convertLiteralTupleToStructVar(SemanticContext* context, TypeInfo* toType, AstNode* fromNode, bool fromType)
{
    if (fromNode->flags & AST_STRUCT_CONVERT)
        return true;
    fromNode->flags |= AST_STRUCT_CONVERT;

    auto sourceFile = context->sourceFile;
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Struct);

    if (fromNode->kind == AstNodeKind::FuncCallParam)
    {
        fromNode->setPassThrough();
        fromNode = fromNode->childs.front();
        if (fromNode->kind != AstNodeKind::ExpressionList)
            return true;
    }

    // Parent of the identifier that will reference the new variable
    auto parentForRef = fromNode;
    if (fromNode->parent->kind == AstNodeKind::FuncCallParam)
        parentForRef = fromNode->parent;

    // Declare a variable
    auto varNode = Ast::newVarDecl(sourceFile, Fmt("__6tmp_%d", g_UniqueID.fetch_add(1)), fromNode->parent);

    // The variable will be inserted after its reference (below), so we need to inverse the order of evaluation.
    // Seems a little bit like a hack. Not sure this will always work.
    // :ReverseLiteralStruct
    fromNode->parent->flags |= AST_REVERSE_SEMANTIC;

    varNode->inheritTokenLocation(fromNode);
    varNode->flags |= AST_GENERATED;

    auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
    typeNode->inheritTokenLocation(fromNode);
    typeNode->addSpecFlags(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS);
    varNode->type = typeNode;

    SWAG_ASSERT(typeStruct->declNode);
    varNode->addAlternativeScope(typeStruct->declNode->ownerScope);
    typeNode->identifier = Ast::newIdentifierRef(sourceFile, typeStruct->declNode->token.text, typeNode);

    typeNode->identifier->flags |= AST_GENERATED;
    typeNode->identifier->inheritTokenLocation(fromNode);

    auto back = typeNode->identifier->childs.back();
    back->flags &= ~AST_NO_BYTECODE;
    back->flags |= AST_IN_TYPE_VAR_DECLARATION;

    // If this is in a return expression, then force the identifier type to be retval
    if (fromNode->inSimpleReturn())
        typeNode->typeFlags |= TYPEFLAG_IS_RETVAL;

    // And make a reference to that variable
    auto identifierRef = Ast::newIdentifierRef(sourceFile, varNode->token.text, parentForRef);
    identifierRef->flags |= AST_R_VALUE | AST_TRANSIENT | AST_GENERATED;

    // Make parameters
    auto identifier = CastAst<AstIdentifier>(typeNode->identifier->childs.back(), AstNodeKind::Identifier);
    identifier->inheritTokenLocation(fromNode);
    identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);
    identifier->callParameters->addSpecFlags(AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT);

    int countParams = (int) fromNode->childs.size();
    if (parentForRef == fromNode)
        countParams--;
    for (int i = 0; i < countParams; i++)
    {
        auto         oneChild = fromNode->childs[i];
        auto         oneParam = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        CloneContext cloneContext;
        cloneContext.parent = oneParam;
        oneParam->inheritTokenLocation(oneChild);
        oneChild->clone(cloneContext);
        oneChild->flags |= AST_NO_BYTECODE | AST_NO_SEMANTIC;
        if (oneChild->kind == AstNodeKind::Identifier)
            oneChild->addSpecFlags(AstIdentifier::SPECFLAG_NO_INLINE);

        if (oneChild->hasExtMisc() && oneChild->extMisc()->isNamed)
        {
            oneParam->allocateExtension(ExtensionKind::Misc);
            oneParam->extMisc()->isNamed = oneChild->extMisc()->isNamed;
        }

        // If this is for a return, remember it, in order to make a move or a copy
        if ((typeStruct->isTuple()) && fromNode->parent->kind == AstNodeKind::Return)
            oneParam->autoTupleReturn = CastAst<AstReturn>(fromNode->parent, AstNodeKind::Return);
    }

    // For a tuple initialization, every parameters must be covered
    if (!fromType && (typeStruct->isTuple()))
    {
        int maxCount = (int) typeStruct->fields.size();
        if (countParams > maxCount)
        {
            Diagnostic diag{fromNode->childs[maxCount], Fmt(Err(Err0636), maxCount, countParams)};
            return context->report(diag);
        }

        if (countParams < maxCount)
        {
            Diagnostic diag{fromNode->childs.back(), Fmt(Err(Err0596), maxCount, countParams)};
            return context->report(diag);
        }
    }

    fromNode->typeInfo = toType;
    fromNode->setPassThrough();

    // Add the 2 nodes to the semantic
    if (fromType)
    {
        fromNode->parent->typeInfo = toType;
        context->baseJob->nodes.push_back(identifierRef);
        context->baseJob->nodes.push_back(varNode);
    }
    else
    {
        auto b = context->baseJob->nodes.back();
        context->baseJob->nodes.pop_back();
        context->baseJob->nodes.push_back(identifierRef);
        context->baseJob->nodes.push_back(varNode);
        context->baseJob->nodes.push_back(b);
    }

    return true;
}

bool Ast::convertLiteralTupleToStructType(SemanticContext* context, TypeInfoStruct* toType, AstNode* fromNode)
{
    auto sourceFile = context->sourceFile;
    auto typeList   = CastTypeInfo<TypeInfoList>(fromNode->typeInfo, TypeInfoKind::TypeListTuple);

    // :SubDeclParent
    auto newParent = fromNode->parent;
    while (newParent != sourceFile->astRoot && !(newParent->flags & AST_GLOBAL_NODE) && (newParent->kind != AstNodeKind::Namespace))
    {
        newParent = newParent->parent;
        SWAG_ASSERT(newParent);
    }

    auto structNode = Ast::newStructDecl(sourceFile, fromNode, nullptr);
    structNode->flags |= AST_INTERNAL | AST_GENERATED;
    structNode->semFlags |= SEMFLAG_FILE_JOB_PASS;
    Ast::removeFromParent(structNode);
    Ast::addChildBack(newParent, structNode);
    structNode->originalParent = newParent;
    structNode->allocateExtension(ExtensionKind::Semantic);
    structNode->extSemantic()->semanticBeforeFct = Semantic::preResolveGeneratedStruct;

    auto contentNode    = Ast::newNode<AstNode>(nullptr, AstNodeKind::TupleContent, sourceFile, structNode);
    structNode->content = contentNode;
    contentNode->allocateExtension(ExtensionKind::Semantic);
    contentNode->extSemantic()->semanticBeforeFct = Semantic::preResolveStructContent;

    Utf8 name = sourceFile->scopeFile->name + "_tuple_";
    name += Fmt("%d", g_UniqueID.fetch_add(1));
    structNode->token.text = std::move(name);

    // Add struct type and scope
    Scope* rootScope;
    if (sourceFile->fromTests)
        rootScope = sourceFile->scopeFile;
    else
        rootScope = newParent->ownerScope;
    structNode->allocateExtension(ExtensionKind::Misc);
    structNode->addAlternativeScope(fromNode->parent->ownerScope);
    structNode->extMisc()->alternativeNode = newParent;

    auto newScope     = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
    structNode->scope = newScope;

    // Create type
    auto typeInfo        = makeType<TypeInfoStruct>();
    structNode->typeInfo = typeInfo;
    typeInfo->flags |= TYPEINFO_STRUCT_IS_TUPLE | TYPEINFO_GENERATED_TUPLE;
    typeInfo->declNode   = structNode;
    typeInfo->name       = structNode->token.text;
    typeInfo->structName = structNode->token.text;
    typeInfo->scope      = newScope;

    int countParams = (int) typeList->subTypes.size();
    int maxCount    = (int) toType->fields.size();
    if (countParams > maxCount)
        return context->report({fromNode->childs.front()->childs[maxCount], Fmt(Err(Err0636), maxCount, countParams)});
    if (countParams < maxCount)
        return context->report({fromNode->childs.front()->childs.back(), Fmt(Err(Err0596), maxCount, countParams)});

    // Each field of the toType struct must have a type given by the tuple.
    // But as that tuple can have named parameters, we need to find the correct type depending
    // on the name (if specified).
    {
        int i = 0;
        for (auto p : toType->fields)
        {
            auto p1        = typeList->subTypes[i];
            auto typeField = p1->typeInfo;
            Utf8 nameVar   = p->name;
            for (size_t j = 0; j < typeList->subTypes.size(); j++)
            {
                if (nameVar == typeList->subTypes[j]->name)
                {
                    p1        = typeList->subTypes[j];
                    typeField = p1->typeInfo;
                    break;
                }
            }

            if (nameVar.empty())
                nameVar = Fmt("item%u", i);
            i++;

            if (typeField->isListArray())
                typeField = TypeManager::convertTypeListToArray(context, (TypeInfoList*) typeField, false);
            if (typeField->isListTuple())
                return context->report({fromNode, Err(Err0739)});

            // This is used for generic automatic deduction. We can use typeInfo->genericParameters, or we would
            // have to construct a struct AST with generic parameters too, and this is not possible as the struct
            // is not generic in all cases (generic types used in the struct can come from the function for example).
            if (p->typeInfo->isGeneric())
                typeInfo->deducedGenericParameters.push_back(typeField);

            auto varNode  = Ast::newVarDecl(sourceFile, nameVar, contentNode);
            auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
            varNode->flags |= AST_GENERATED | AST_STRUCT_MEMBER;
            varNode->type                  = typeNode;
            varNode->ownerScope            = newScope;
            structNode->resolvedSymbolName = newScope->symTable.registerSymbolNameNoLock(context, structNode, SymbolKind::Variable);

            typeNode->typeInfo = typeField;
            typeNode->flags |= AST_NO_SEMANTIC;
        }
    }

    SWAG_CHECK(convertLiteralTupleToStructVar(context, typeInfo, fromNode, true));

    context->baseJob->nodes.push_back(structNode);
    context->result = ContextResult::NewChilds1;

    return true;
}

bool Ast::convertLiteralTupleToStructDecl(SemanticContext* context, AstNode* assignment, AstStruct** result)
{
    auto       sourceFile = context->sourceFile;
    AstStruct* structNode = Ast::newStructDecl(sourceFile, nullptr);
    *result               = structNode;
    structNode->flags |= AST_GENERATED;

    // A capture block is packed
    if (assignment->specFlags & AstExpressionList::SPECFLAG_FOR_CAPTURE)
        structNode->packing = 1;

    auto contentNode = Ast::newNode<AstNode>(nullptr, AstNodeKind::TupleContent, sourceFile, structNode);
    contentNode->allocateExtension(ExtensionKind::Semantic);
    contentNode->extSemantic()->semanticBeforeFct = Semantic::preResolveStructContent;
    contentNode->addAlternativeScope(assignment->ownerScope);
    structNode->content        = contentNode;
    structNode->originalParent = assignment;

    auto typeList = CastTypeInfo<TypeInfoList>(assignment->typeInfo, TypeInfoKind::TypeListTuple);
    Utf8 varName;
    int  numChilds = (int) typeList->subTypes.size();
    for (int idx = 0; idx < numChilds; idx++)
    {
        auto typeParam = typeList->subTypes[idx];
        auto childType = TypeManager::concreteType(typeParam->typeInfo, CONCRETE_FUNC);
        auto subAffect = assignment->childs[idx];

        bool autoName = false;

        // User specified name
        if (!typeParam->name.empty())
        {
            varName = typeParam->name;
        }

        // If this is a single identifier, then we take the identifier name
        else if (subAffect->kind == AstNodeKind::IdentifierRef && subAffect->childs.back()->kind == AstNodeKind::Identifier)
        {
            varName         = subAffect->childs.back()->token.text;
            typeParam->name = varName;
        }

        // Otherwise generate an 'item<num>' name
        else
        {
            autoName = true;
            varName  = Fmt("item%u", idx);
        }

        auto paramNode = Ast::newVarDecl(sourceFile, varName, contentNode);
        paramNode->inheritTokenLocation(subAffect);

        if (autoName)
        {
            typeParam->flags |= TYPEINFOPARAM_AUTO_NAME;
        }

        paramNode->type = convertTypeToTypeExpression(context, paramNode, subAffect, childType, !(assignment->specFlags & AstExpressionList::SPECFLAG_FOR_CAPTURE));

        // Special case for tuple capture. If type is null (type not compatible with tuple), put undefined,
        // as we will catch the error later
        if (!paramNode->type && assignment->specFlags & AstExpressionList::SPECFLAG_FOR_CAPTURE)
        {
            static AstNode fakeNode;
            Ast::constructNode(&fakeNode);
            fakeNode.typeInfo = g_TypeMgr->typeInfoBool;
            paramNode->type   = &fakeNode;
        }

        // This can avoid some initialization before assignment, because everything will be covered
        // as this is a tuple
        paramNode->flags |= AST_EXPLICITLY_NOT_INITIALIZED;

        if (!paramNode->type)
            return false;
    }

    // Compute structure name
    Utf8 name = sourceFile->scopeFile->name + "_tuple_";
    name += Fmt("%d", g_UniqueID.fetch_add(1));
    structNode->token.text = name;

    // Add struct type and scope
    structNode->inheritOwners(sourceFile->astRoot);
    Scope* rootScope = structNode->ownerScope;

    auto typeInfo        = makeType<TypeInfoStruct>();
    auto newScope        = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
    typeInfo->declNode   = structNode;
    typeInfo->name       = structNode->token.text;
    typeInfo->structName = structNode->token.text;
    typeInfo->scope      = newScope;
    typeInfo->flags |= TYPEINFO_STRUCT_IS_TUPLE;
    structNode->typeInfo = typeInfo;
    structNode->scope    = newScope;
    Ast::visit(structNode->content, [&](AstNode* n)
               {
                        n->ownerStructScope = newScope;
                        n->ownerScope = newScope; });

    rootScope->symTable.registerSymbolName(context, structNode, SymbolKind::Struct);
    Ast::addChildBack(sourceFile->astRoot, structNode);
    SemanticJob::newJob(context->baseJob->dependentJob, sourceFile, structNode, true);

    return true;
}

bool Ast::convertLiteralTupleToStructDecl(SemanticContext* context, AstNode* parent, AstNode* assignment, AstNode** result)
{
    auto       sourceFile = context->sourceFile;
    AstStruct* structNode;
    SWAG_CHECK(convertLiteralTupleToStructDecl(context, assignment, &structNode));

    // Reference to that generated structure
    auto typeExpression = Ast::newTypeExpression(sourceFile, parent);
    typeExpression->flags |= AST_NO_BYTECODE_CHILDS | AST_GENERATED;
    typeExpression->identifier = Ast::newIdentifierRef(sourceFile, structNode->token.text, typeExpression);
    *result                    = typeExpression;
    return true;
}

bool Ast::convertStructParamsToTmpVar(SemanticContext* context, AstIdentifier* identifier)
{
    auto node       = context->node;
    auto sourceFile = identifier->sourceFile;
    auto callP      = identifier->callParameters;
    identifier->flags |= AST_R_VALUE | AST_GENERATED | AST_NO_BYTECODE;

    // Be sure it's the NAME{} syntax
    if (!(identifier->callParameters->specFlags & AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT))
        return context->report({callP, Fmt(Err(Err0377), identifier->typeInfo->name.c_str())});

    auto varParent = identifier->identifierRef()->parent;
    while (varParent->kind == AstNodeKind::ExpressionList)
        varParent = varParent->parent;

    // Declare a variable
    auto varNode = Ast::newVarDecl(sourceFile, Fmt("__1tmp_%d", g_UniqueID.fetch_add(1)), varParent);

    // Inherit alternative scopes.
    if (identifier->parent->hasExtMisc())
        varNode->addAlternativeScopes(identifier->parent->extMisc());

    // If we are in a const declaration, that temporary variable should be a const too...
    if (identifier->parent->parent->kind == AstNodeKind::ConstDecl)
        varNode->kind = AstNodeKind::ConstDecl;

    // At global scope, this should be a constant declaration, not a variable, as we cannot assign a global variable to
    // another global variable at compile time
    else if (identifier->ownerScope->isGlobalOrImpl())
        varNode->kind = AstNodeKind::ConstDecl;

    auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
    typeNode->addSpecFlags(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS);
    typeNode->addSpecFlags(AstType::SPECFLAG_CREATED_STRUCT_PARAMETERS);
    varNode->flags |= AST_GENERATED;
    varNode->type = typeNode;
    CloneContext cloneContext;
    cloneContext.parent     = typeNode;
    cloneContext.cloneFlags = CLONE_RAW;
    typeNode->identifier    = identifier->identifierRef()->clone(cloneContext);
    auto back               = CastAst<AstIdentifier>(typeNode->identifier->childs.back(), AstNodeKind::Identifier);
    back->flags &= ~AST_NO_BYTECODE;
    back->flags |= AST_IN_TYPE_VAR_DECLARATION;

    // :StructParamsNoSem
    // Call parameters have already been evaluated, so do not reevaluate them again
    back->callParameters->flags |= AST_NO_SEMANTIC;
    back->callParameters->semFlags |= node->semFlags & SEMFLAG_ACCESS_MASK;

    // :DupGen :StructParamsNoSem
    // Type has already been evaluated
    typeNode->identifier->flags |= AST_NO_SEMANTIC;
    typeNode->identifier->semFlags |= node->semFlags & SEMFLAG_ACCESS_MASK;

    // If this is in a return expression, then force the identifier type to be retval
    if (node->parent && node->parent->inSimpleReturn())
        typeNode->typeFlags |= TYPEFLAG_IS_RETVAL;

    // And make a reference to that variable
    auto identifierRef = identifier->identifierRef();

    identifierRef->allocateExtension(ExtensionKind::Owner);
    for (auto c : identifierRef->childs)
        identifierRef->extOwner()->nodesToFree.push_back(c);
    identifierRef->childs.clear();
    auto idNode = Ast::newIdentifier(sourceFile, varNode->token.text, identifierRef, identifierRef);
    idNode->flags |= AST_R_VALUE | AST_TRANSIENT;

    // Reset parsing
    identifierRef->startScope = nullptr;

    // The variable will be inserted after its reference (below), so we need to inverse the order of evaluation.
    // Seems a little bit like a hack. Not sure this will always work.
    varParent->flags |= AST_REVERSE_SEMANTIC;

    // Add the 2 nodes to the semantic
    context->baseJob->nodes.pop_back();
    context->baseJob->nodes.push_back(idNode);
    context->baseJob->nodes.push_back(varNode);
    context->result = ContextResult::NewChilds;

    return true;
}

AstNode* Ast::convertTypeToTypeExpression(SemanticContext* context, AstNode* parent, AstNode* assignment, TypeInfo* childType, bool raiseErrors)
{
    auto sourceFile = context->sourceFile;

    // Tuple item is a lambda
    if (childType->isLambdaClosure())
    {
        auto typeLambda             = CastTypeInfo<TypeInfoFuncAttr>(childType, TypeInfoKind::LambdaClosure);
        auto typeExprLambda         = Ast::newNode<AstTypeLambda>(nullptr, AstNodeKind::TypeLambda, sourceFile, parent);
        typeExprLambda->semanticFct = Semantic::resolveTypeLambdaClosure;
        if (childType->flags & TYPEINFO_CAN_THROW)
            typeExprLambda->addSpecFlags(AstTypeLambda::SPECFLAG_CAN_THROW);

        // Parameters
        auto params                = Ast::newNode<AstNode>(nullptr, AstNodeKind::FuncDeclParams, sourceFile, typeExprLambda);
        typeExprLambda->parameters = params;
        for (auto p : typeLambda->parameters)
        {
            auto typeParam = convertTypeToTypeExpression(context, params, assignment, p->typeInfo, raiseErrors);
            if (!typeParam)
                return nullptr;
        }

        // Return type
        if (typeLambda->returnType && !typeLambda->returnType->isVoid())
        {
            typeExprLambda->returnType = convertTypeToTypeExpression(context, typeExprLambda, assignment, typeLambda->returnType, raiseErrors);
            if (!typeExprLambda->returnType)
                return nullptr;
        }

        return typeExprLambda;
    }

    auto typeExpression = Ast::newTypeExpression(sourceFile, parent);
    typeExpression->flags |= AST_NO_BYTECODE_CHILDS;
    if (childType->isConst())
        typeExpression->typeFlags |= TYPEFLAG_IS_CONST;

    if (childType->isListTuple())
    {
        AstStruct* inStructNode;
        if (!convertLiteralTupleToStructDecl(context, assignment, &inStructNode))
            return nullptr;
        typeExpression->identifier = Ast::newIdentifierRef(sourceFile, inStructNode->token.text, typeExpression);
        return typeExpression;
    }

    typeExpression->typeInfo = childType;
    typeExpression->flags |= AST_NO_SEMANTIC;
    return typeExpression;
}