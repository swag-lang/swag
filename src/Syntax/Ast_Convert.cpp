#include "pch.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Scope.h"
#include "Semantic/SemanticJob.h"
#include "Wmf/SourceFile.h"
#include "Semantic/Type/TypeManager.h"

std::atomic<uint32_t> g_UniqueID;

bool Ast::convertLiteralTupleToStructVar(JobContext* context, TypeInfo* toType, AstNode* fromNode, bool fromType)
{
    if (fromNode->hasAstFlag(AST_STRUCT_CONVERT))
        return true;
    fromNode->addAstFlag(AST_STRUCT_CONVERT);

    const auto typeStruct = castTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Struct);

    if (fromNode->is(AstNodeKind::FuncCallParam))
    {
        fromNode->setPassThrough();
        fromNode = fromNode->firstChild();
        if (fromNode->isNot(AstNodeKind::ExpressionList))
            return true;
    }

    // Parent of the identifier that will reference the new variable
    auto parentForRef = fromNode;
    if (fromNode->parent->is(AstNodeKind::FuncCallParam))
        parentForRef = fromNode->parent;

    // Declare a variable
    const auto varNode = newVarDecl(form("__6tmp_%d", g_UniqueID.fetch_add(1)), nullptr, fromNode->parent);

    // The variable will be inserted after its reference (below), so we need to inverse the order of evaluation.
    // Seems a little bit like a hack. Not sure if this will always work.
    // :ReverseLiteralStruct
    fromNode->parent->addAstFlag(AST_REVERSE_SEMANTIC);

    varNode->inheritTokenLocation(fromNode->token);
    varNode->addAstFlag(AST_GENERATED);

    const auto typeNode = newTypeExpression(nullptr, varNode);
    typeNode->inheritTokenLocation(fromNode->token);
    typeNode->addSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS);
    varNode->type = typeNode;

    SWAG_ASSERT(typeStruct->declNode);
    varNode->addAlternativeScope(typeStruct->declNode->ownerScope);
    typeNode->identifier = newIdentifierRef(typeStruct->declNode->token.text, nullptr, typeNode);

    typeNode->identifier->addAstFlag(AST_GENERATED);
    typeNode->identifier->inheritTokenLocation(fromNode->token);

    const auto back = typeNode->identifier->lastChild();
    back->removeAstFlag(AST_NO_BYTECODE);
    back->addAstFlag(AST_IN_TYPE_VAR_DECLARATION);

    // If this is in a return expression, then force the identifier type to be retval
    if (fromNode->inSimpleReturn())
        typeNode->typeFlags.add(TYPEFLAG_IS_RETVAL);

    // And make a reference to that variable
    const auto identifierRef = newIdentifierRef(varNode->token.text, nullptr, parentForRef);
    identifierRef->addAstFlag(AST_R_VALUE | AST_TRANSIENT | AST_GENERATED);

    // Make parameters
    const auto identifier = castAst<AstIdentifier>(typeNode->identifier->lastChild(), AstNodeKind::Identifier);
    identifier->inheritTokenLocation(fromNode->token);
    identifier->callParameters = newFuncCallParams(nullptr, identifier);
    identifier->callParameters->addSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT);

    uint32_t countParams = fromNode->childCount();
    if (parentForRef == fromNode)
        countParams--;
    for (uint32_t i = 0; i < countParams; i++)
    {
        const auto   oneChild = fromNode->children[i];
        const auto   oneParam = newFuncCallParam(nullptr, identifier->callParameters);
        CloneContext cloneContext;
        cloneContext.parent = oneParam;
        oneParam->inheritTokenLocation(oneChild->token);
        oneChild->clone(cloneContext);
        oneChild->addAstFlag(AST_NO_BYTECODE | AST_NO_SEMANTIC);
        if (oneChild->is(AstNodeKind::Identifier))
            oneChild->addSpecFlag(AstIdentifier::SPEC_FLAG_NO_INLINE);

        const auto isNamed = oneChild->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
        if (isNamed)
            oneParam->addExtraPointer(ExtraPointerKind::IsNamed, isNamed);

        // If this is for a return, remember it, in order to make a move or a copy
        if (typeStruct->isTuple() && fromNode->parent->is(AstNodeKind::Return))
            oneParam->autoTupleReturn = castAst<AstReturn>(fromNode->parent, AstNodeKind::Return);
    }

    // For a tuple initialization, every parameter must be covered
    if (!fromType && typeStruct->isTuple())
    {
        const uint32_t maxCount = typeStruct->fields.size();
        if (countParams > maxCount)
        {
            const Diagnostic err{fromNode->children[maxCount], formErr(Err0636, maxCount, countParams)};
            return context->report(err);
        }

        if (countParams < maxCount)
        {
            const Diagnostic err{fromNode->lastChild(), formErr(Err0596, maxCount, countParams)};
            return context->report(err);
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
        const auto b = context->baseJob->nodes.back();
        context->baseJob->nodes.pop_back();
        context->baseJob->nodes.push_back(identifierRef);
        context->baseJob->nodes.push_back(varNode);
        context->baseJob->nodes.push_back(b);
    }

    return true;
}

bool Ast::convertLiteralTupleToStructType(JobContext* context, AstNode* paramNode, TypeInfoStruct* toType, AstNode* fromNode)
{
    const auto sourceFile = context->sourceFile;
    const auto typeList   = castTypeInfo<TypeInfoList>(fromNode->typeInfo, TypeInfoKind::TypeListTuple);

    // :SubDeclParent
    auto newParent = fromNode->parent;
    while (newParent != sourceFile->astRoot && !newParent->hasAstFlag(AST_GLOBAL_NODE) && newParent->isNot(AstNodeKind::Namespace))
    {
        newParent = newParent->parent;
        SWAG_ASSERT(newParent);
    }

    const auto structNode = newStructDecl(nullptr, fromNode);
    structNode->addAstFlag(AST_INTERNAL | AST_GENERATED);
    structNode->addSemFlag(SEMFLAG_FILE_JOB_PASS);
    removeFromParent(structNode);
    addChildBack(newParent, structNode);
    structNode->originalParent = newParent;
    structNode->allocateExtension(ExtensionKind::Semantic);
    structNode->extSemantic()->semanticBeforeFct = Semantic::preResolveGeneratedStruct;
    structNode->addExtraPointer(ExtraPointerKind::ExportNode, paramNode);

    const auto contentNode = Ast::newNode<AstNode>(AstNodeKind::TupleContent, nullptr, structNode);
    structNode->content    = contentNode;
    contentNode->allocateExtension(ExtensionKind::Semantic);
    contentNode->extSemantic()->semanticBeforeFct = Semantic::preResolveStructContent;

    Utf8 name = sourceFile->scopeFile->name + "_tpl1_";
    name += form("%d", g_UniqueID.fetch_add(1));
    structNode->token.text = std::move(name);

    // Add struct type and scope
    Scope* rootScope;
    if (sourceFile->hasFlag(FILE_FROM_TESTS))
        rootScope = sourceFile->scopeFile;
    else
        rootScope = newParent->ownerScope;
    structNode->allocateExtension(ExtensionKind::Misc);
    structNode->addAlternativeScope(fromNode->parent->ownerScope);
    structNode->addExtraPointer(ExtraPointerKind::AlternativeNode, newParent);

    const auto newScope = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
    structNode->scope   = newScope;

    // Create type
    const auto typeInfo  = makeType<TypeInfoStruct>();
    structNode->typeInfo = typeInfo;
    typeInfo->addFlag(TYPEINFO_STRUCT_IS_TUPLE | TYPEINFO_GENERATED_TUPLE);
    typeInfo->declNode   = structNode;
    typeInfo->name       = structNode->token.text;
    typeInfo->structName = structNode->token.text;
    typeInfo->scope      = newScope;

    const auto countParams = typeList->subTypes.size();
    const auto maxCount    = toType->fields.size();
    if (countParams > maxCount)
        return context->report({fromNode->firstChild()->children[maxCount], formErr(Err0636, maxCount, countParams)});
    if (countParams < maxCount)
        return context->report({fromNode->firstChild()->lastChild(), formErr(Err0596, maxCount, countParams)});

    // Each field of the toType struct must have a type given by the tuple.
    // But as that tuple can have named parameters, we need to find the correct type depending
    // on the name (if specified).
    {
        int i = 0;
        for (const auto p : toType->fields)
        {
            auto p1        = typeList->subTypes[i];
            auto typeField = p1->typeInfo;
            Utf8 nameVar   = p->name;
            for (const auto& subType : typeList->subTypes)
            {
                if (nameVar == subType->name)
                {
                    p1        = subType;
                    typeField = p1->typeInfo;
                    break;
                }
            }

            if (nameVar.empty())
                nameVar = form("item%u", i);
            i++;

            if (typeField->isListArray())
                typeField = TypeManager::convertTypeListToArray(context, castTypeInfo<TypeInfoList>(typeField), false);
            if (typeField->isListTuple())
                return context->report({fromNode, toErr(Err0739)});

            // This is used for generic automatic deduction. We can use typeInfo->genericParameters, or we would
            // have to construct a struct AST with generic parameters too, and this is not possible as the struct
            // is not generic in all cases (generic types used in the struct can come from the function for example).
            if (p->typeInfo->isGeneric())
                typeInfo->deducedGenericParameters.push_back(typeField);

            const auto varNode = newVarDecl(nameVar, nullptr, contentNode);
            SWAG_ASSERT(varNode->token.sourceFile);
            const auto typeNode = newTypeExpression(nullptr, varNode);
            varNode->addAstFlag(AST_GENERATED | AST_STRUCT_MEMBER);
            varNode->type       = typeNode;
            varNode->ownerScope = newScope;
            structNode->setResolvedSymbolName(newScope->symTable.registerSymbolNameNoLock(context, structNode, SymbolKind::Variable));

            typeNode->typeInfo = typeField;
            typeNode->addAstFlag(AST_NO_SEMANTIC);
        }
    }

    SWAG_CHECK(convertLiteralTupleToStructVar(context, typeInfo, fromNode, true));

    context->baseJob->nodes.push_back(structNode);
    context->result = ContextResult::NewChildren1;

    return true;
}

bool Ast::convertLiteralTupleToStructDecl(JobContext* context, AstNode* assignment, AstStruct** result)
{
    const auto sourceFile        = context->sourceFile;
    AstStruct* structNode        = newStructDecl(nullptr, nullptr);
    structNode->token.sourceFile = sourceFile;
    *result                      = structNode;
    structNode->addAstFlag(AST_GENERATED);

    // A capture block is packed
    if (assignment->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_CAPTURE))
        structNode->packing = 1;

    const auto contentNode = Ast::newNode<AstNode>(AstNodeKind::TupleContent, nullptr, structNode);
    contentNode->allocateExtension(ExtensionKind::Semantic);
    contentNode->extSemantic()->semanticBeforeFct = Semantic::preResolveStructContent;
    contentNode->addAlternativeScope(assignment->ownerScope);
    structNode->content        = contentNode;
    structNode->originalParent = assignment;

    const auto     typeList = castTypeInfo<TypeInfoList>(assignment->typeInfo, TypeInfoKind::TypeListTuple);
    Utf8           varName;
    const uint32_t numChildren = typeList->subTypes.size();
    for (uint32_t idx = 0; idx < numChildren; idx++)
    {
        const auto typeParam = typeList->subTypes[idx];
        const auto childType = TypeManager::concreteType(typeParam->typeInfo, CONCRETE_FUNC);
        const auto subAffect = assignment->children[idx];

        bool autoName = false;

        // User specified name
        if (!typeParam->name.empty())
        {
            varName = typeParam->name;
        }

        // If this is a single identifier, then we take the identifier name
        else if (subAffect->is(AstNodeKind::IdentifierRef) && subAffect->lastChild()->is(AstNodeKind::Identifier))
        {
            varName         = subAffect->lastChild()->token.text;
            typeParam->name = varName;
        }

        // Otherwise generate an 'item<num>' name
        else
        {
            autoName = true;
            varName  = form("item%u", idx);
        }

        const auto paramNode = newVarDecl(varName, nullptr, contentNode);
        paramNode->inheritTokenLocation(subAffect->token);
        SWAG_ASSERT(paramNode->token.sourceFile);

        if (autoName)
        {
            typeParam->flags.add(TYPEINFOPARAM_AUTO_NAME);
        }

        paramNode->type = convertTypeToTypeExpression(context, paramNode, subAffect, childType, !assignment->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_CAPTURE));

        // Special case for tuple capture. If type is null (type not compatible with tuple), put undefined,
        // as we will catch the error later
        if (!paramNode->type && assignment->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_CAPTURE))
        {
            static AstNode fakeNode;
            constructNode(&fakeNode);
            fakeNode.typeInfo = g_TypeMgr->typeInfoBool;
            paramNode->type   = &fakeNode;
        }

        // This can avoid some initialization before assignment, because everything will be covered
        // as this is a tuple
        paramNode->addAstFlag(AST_EXPLICITLY_NOT_INITIALIZED);

        if (!paramNode->type)
            return false;
    }

    // Compute structure name
    Utf8 name = sourceFile->scopeFile->name + "_tpl2_";
    name += form("%d", g_UniqueID.fetch_add(1));
    structNode->token.text = name;

    // Add struct type and scope
    structNode->inheritOwners(sourceFile->astRoot);
    Scope* rootScope = structNode->ownerScope;

    const auto typeInfo  = makeType<TypeInfoStruct>();
    const auto newScope  = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
    typeInfo->declNode   = structNode;
    typeInfo->name       = structNode->token.text;
    typeInfo->structName = structNode->token.text;
    typeInfo->scope      = newScope;
    typeInfo->addFlag(TYPEINFO_STRUCT_IS_TUPLE);
    structNode->typeInfo = typeInfo;
    structNode->scope    = newScope;
    visit(structNode->content, [&](AstNode* n) {
        n->ownerStructScope = newScope;
        n->ownerScope       = newScope;
    });

    rootScope->symTable.registerSymbolName(context, structNode, SymbolKind::Struct);
    addChildBack(sourceFile->astRoot, structNode);
    SemanticJob::newJob(context->baseJob->dependentJob, sourceFile, structNode, true);

    return true;
}

bool Ast::convertLiteralTupleToStructDecl(JobContext* context, AstNode* parent, AstNode* assignment, AstNode** result)
{
    AstStruct* structNode;
    SWAG_CHECK(convertLiteralTupleToStructDecl(context, assignment, &structNode));

    // Reference to that generated structure
    const auto typeExpression = newTypeExpression(nullptr, parent);
    typeExpression->addAstFlag(AST_NO_BYTECODE_CHILDREN | AST_GENERATED);
    typeExpression->identifier = newIdentifierRef(structNode->token.text, nullptr, typeExpression);
    *result                    = typeExpression;
    return true;
}

void Ast::convertTypeStructToStructDecl(JobContext* context, TypeInfoStruct* typeStruct)
{
    // Generate some fake nodes
    // This peace of code is necessary to solve something like :
    // let s = [{1, 2}, {3, 4}]
    const auto structDecl        = newStructDecl(nullptr, nullptr);
    structDecl->token.sourceFile = context->sourceFile;
    structDecl->addExtraPointer(ExtraPointerKind::ExportNode, typeStruct->declNode);
    typeStruct->declNode = structDecl;
    typeStruct->declNode->addAstFlag(AST_GENERATED);
    typeStruct->declNode->typeInfo   = typeStruct;
    typeStruct->declNode->ownerScope = context->sourceFile->scopeFile;
    typeStruct->scope                = newScope(typeStruct->declNode, "", ScopeKind::Struct, context->sourceFile->scopeFile);
    typeStruct->scope->owner         = typeStruct->declNode;
    typeStruct->alignOf              = 1;
    structDecl->scope                = typeStruct->scope;

    int idx = 0;
    for (const auto f : typeStruct->fields)
    {
        f->declNode           = newVarDecl(f->name, nullptr, typeStruct->declNode);
        f->declNode->typeInfo = f->typeInfo;
        f->declNode->addAstFlag(AST_GENERATED);
        SWAG_ASSERT(f->declNode->token.sourceFile);

        AddSymbolTypeInfo toAdd;
        toAdd.kind          = SymbolKind::Variable;
        toAdd.node          = f->declNode;
        toAdd.typeInfo      = f->typeInfo;
        toAdd.storageOffset = f->offset;
        toAdd.flags         = OVERLOAD_VAR_STRUCT;
        f->declNode->setResolvedSymbolOverload(typeStruct->scope->symTable.addSymbolTypeInfo(context, toAdd));

        if (!f->flags.has(TYPEINFOPARAM_AUTO_NAME))
        {
            toAdd.aliasName = form("item%d", idx);
            f->declNode->setResolvedSymbolOverload(typeStruct->scope->symTable.addSymbolTypeInfo(context, toAdd));
        }

        f->declNode->setResolvedSymbolName(f->declNode->resolvedSymbolOverload()->symbol);
        idx++;
    }
}

bool Ast::convertStructParamsToTmpVar(JobContext* context, AstIdentifier* identifier)
{
    const auto node  = context->node;
    auto       callP = identifier->callParameters;
    identifier->addAstFlag(AST_R_VALUE | AST_NO_BYTECODE);

    // Be sure it's the NAME{} syntax
    if (!identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
        return context->report({callP, formErr(Err0377, identifier->typeInfo->name.c_str())});

    auto varParent = identifier->identifierRef()->parent;
    while (varParent->is(AstNodeKind::ExpressionList))
        varParent = varParent->parent;

    // Declare a variable
    const auto varNode = newVarDecl(form("__1tmp_%d", g_UniqueID.fetch_add(1)), nullptr, varParent);

    // Inherit alternative scopes.
    if (identifier->parent->hasExtMisc())
        varNode->addAlternativeScopes(identifier->parent->extMisc());

    // If we are in a const declaration, that temporary variable should be a const too...
    if (identifier->parent->parent->is(AstNodeKind::ConstDecl))
        varNode->kind = AstNodeKind::ConstDecl;

    // At global scope, this should be a constant declaration, not a variable, as we cannot assign a global variable to
    // another global variable at compile time
    else if (identifier->ownerScope->isGlobalOrImpl())
        varNode->kind = AstNodeKind::ConstDecl;

    const auto typeNode = newTypeExpression(nullptr, varNode);
    typeNode->addSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS);
    typeNode->addSpecFlag(AstType::SPEC_FLAG_CREATED_STRUCT_PARAMETERS);
    varNode->addAstFlag(AST_GENERATED);
    varNode->type = typeNode;
    CloneContext cloneContext;
    cloneContext.parent     = typeNode;
    cloneContext.cloneFlags = CLONE_RAW;
    typeNode->identifier    = identifier->identifierRef()->clone(cloneContext);
    const auto back         = castAst<AstIdentifier>(typeNode->identifier->lastChild(), AstNodeKind::Identifier);
    back->removeAstFlag(AST_NO_BYTECODE);
    back->addAstFlag(AST_IN_TYPE_VAR_DECLARATION);

    // :StructParamsNoSem
    // Call parameters have already been evaluated, so do not reevaluate them again
    back->callParameters->addAstFlag(AST_NO_SEMANTIC);
    back->callParameters->addSemFlag(node->semFlags.mask(SEMFLAG_ACCESS_MASK));

    // :DupGen :StructParamsNoSem
    // Type has already been evaluated
    typeNode->identifier->addAstFlag(AST_NO_SEMANTIC);
    typeNode->identifier->addSemFlag(node->semFlags.mask(SEMFLAG_ACCESS_MASK));

    // If this is in a return expression, then force the identifier type to be "retval"
    if (node->parent && node->parent->inSimpleReturn())
        typeNode->typeFlags.add(TYPEFLAG_IS_RETVAL);

    // And make a reference to that variable
    const auto identifierRef = identifier->identifierRef();
    identifierRef->addExtraPointer(ExtraPointerKind::ExportNode, identifier);

    identifierRef->allocateExtension(ExtensionKind::Owner);
    for (auto c : identifierRef->children)
        identifierRef->extOwner()->nodesToFree.push_back(c);
    identifierRef->children.clear();
    const auto idNode = newIdentifier(identifierRef, varNode->token.text, nullptr, identifierRef);
    idNode->addAstFlag(AST_R_VALUE | AST_TRANSIENT);

    // Reset parsing
    identifierRef->startScope = nullptr;

    // The variable will be inserted after its reference (below), so we need to inverse the order of evaluation.
    // Seems a little bit like a hack. Not sure if this will always work.
    varParent->addAstFlag(AST_REVERSE_SEMANTIC);

    // Add the 2 nodes to the semantic
    context->baseJob->nodes.pop_back();
    context->baseJob->nodes.push_back(idNode);
    context->baseJob->nodes.push_back(varNode);
    context->result = ContextResult::NewChildren;

    return true;
}

AstNode* Ast::convertTypeToTypeExpression(JobContext* context, AstNode* parent, AstNode* assignment, TypeInfo* childType, bool raiseErrors)
{
    // Tuple item is a lambda
    if (childType->isLambdaClosure())
    {
        const auto typeLambda       = castTypeInfo<TypeInfoFuncAttr>(childType, TypeInfoKind::LambdaClosure);
        const auto typeExprLambda   = Ast::newNode<AstTypeLambda>(AstNodeKind::TypeLambda, nullptr, parent);
        typeExprLambda->semanticFct = Semantic::resolveTypeLambdaClosure;
        if (childType->hasFlag(TYPEINFO_CAN_THROW))
            typeExprLambda->addSpecFlag(AstTypeLambda::SPEC_FLAG_CAN_THROW);

        // Parameters
        const auto params          = Ast::newNode<AstNode>(AstNodeKind::FuncDeclParams, nullptr, typeExprLambda);
        typeExprLambda->parameters = params;
        for (const auto p : typeLambda->parameters)
        {
            const auto typeParam = convertTypeToTypeExpression(context, params, assignment, p->typeInfo, raiseErrors);
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

    const auto typeExpression = newTypeExpression(nullptr, parent);
    typeExpression->addAstFlag(AST_NO_BYTECODE_CHILDREN);
    if (childType->isConst())
        typeExpression->typeFlags.add(TYPEFLAG_IS_CONST);

    if (childType->isListTuple())
    {
        AstStruct* inStructNode;
        if (!convertLiteralTupleToStructDecl(context, assignment, &inStructNode))
            return nullptr;
        typeExpression->identifier = newIdentifierRef(inStructNode->token.text, nullptr, typeExpression);
        return typeExpression;
    }

    typeExpression->typeInfo = childType;
    typeExpression->addAstFlag(AST_NO_SEMANTIC);
    return typeExpression;
}
