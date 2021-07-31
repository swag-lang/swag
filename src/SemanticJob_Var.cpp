#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Generic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

uint32_t SemanticJob::alignOf(AstVarDecl* node)
{
    ComputedValue value;
    if (node->attributes.getValue(g_LangSpec.name_Swag_Align, g_LangSpec.name_value, value))
        return value.reg.u8;
    return TypeManager::alignOf(TypeManager::concreteType(node->typeInfo));
}

DataSegment* SemanticJob::getConstantSegFromContext(AstNode* node, bool forceCompiler)
{
    if (forceCompiler)
        return &node->sourceFile->module->compilerSegment;
    return &node->sourceFile->module->constantSegment;
}

// Will be called after solving the initial var affect, but before tuple unpacking
bool SemanticJob::resolveTupleUnpackBefore(SemanticContext* context)
{
    auto scopeNode = CastAst<AstNode>(context->node->parent, AstNodeKind::StatementNoScope, AstNodeKind::Statement);
    auto varDecl   = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);

    auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
    if (typeVar->kind == TypeInfoKind::TypeListTuple && !varDecl->type)
    {
        varDecl->semFlags |= AST_SEM_TUPLE_CONVERT;
        SWAG_CHECK(convertLiteralTupleToStructDecl(context, varDecl, varDecl->assignment, &varDecl->type));
        context->result = ContextResult::NewChilds;
        context->job->nodes.push_back(varDecl->type);
        return true;
    }
    else if (varDecl->semFlags & AST_SEM_TUPLE_CONVERT)
    {
        SWAG_ASSERT(varDecl->resolvedSymbolOverload);
        varDecl->typeInfo                         = varDecl->type->typeInfo;
        varDecl->resolvedSymbolOverload->typeInfo = varDecl->type->typeInfo;
        typeVar                                   = varDecl->typeInfo;
    }

    SWAG_VERIFY(typeVar->kind == TypeInfoKind::Struct, context->report({varDecl, varDecl->token, Utf8::format(Msg0291, typeVar->name.c_str())}));
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
    auto numUnpack  = scopeNode->childs.size() - 1;
    SWAG_VERIFY(typeStruct->fields.size(), context->report({varDecl, varDecl->token, Utf8::format(Msg0292, typeStruct->getDisplayName().c_str())}));
    SWAG_VERIFY(numUnpack <= typeStruct->fields.size(), context->report({varDecl, varDecl->token, Utf8::format(Msg0293, numUnpack, typeStruct->getDisplayName().c_str(), typeStruct->fields.size())}));
    return true;
}

AstNode* SemanticJob::convertTypeToTypeExpression(SemanticContext* context, AstNode* parent, AstNode* assignment, TypeInfo* childType)
{
    auto sourceFile = context->sourceFile;
    auto orgType    = childType;

    // Tuple item is a lambda
    if (childType->kind == TypeInfoKind::Lambda)
    {
        auto typeLambda             = CastTypeInfo<TypeInfoFuncAttr>(childType, TypeInfoKind::Lambda);
        auto typeExprLambda         = Ast::newNode<AstTypeLambda>(nullptr, AstNodeKind::TypeLambda, sourceFile, parent);
        typeExprLambda->semanticFct = SemanticJob::resolveTypeLambda;
        if (childType->flags & TYPEINFO_CAN_THROW)
            typeExprLambda->specFlags |= AST_SPEC_TYPELAMBDA_CANTHROW;

        // Parameters
        auto params                = Ast::newNode<AstNode>(nullptr, AstNodeKind::FuncDeclParams, sourceFile, typeExprLambda);
        typeExprLambda->parameters = params;
        for (auto p : typeLambda->parameters)
        {
            auto typeParam = convertTypeToTypeExpression(context, params, assignment, p->typeInfo);
            if (!typeParam)
                return nullptr;
        }

        // Return type
        if (typeLambda->returnType && !typeLambda->returnType->isNative(NativeTypeKind::Void))
        {
            typeExprLambda->returnType = convertTypeToTypeExpression(context, typeExprLambda, assignment, typeLambda->returnType);
            if (!typeExprLambda->returnType)
                return nullptr;
        }

        return typeExprLambda;
    }

    auto typeExpression = Ast::newTypeExpression(sourceFile, parent);
    typeExpression->flags |= AST_NO_BYTECODE_CHILDS;
    if (childType->isConst())
        typeExpression->typeFlags |= TYPEFLAG_ISCONST;

    switch (childType->kind)
    {
    case TypeInfoKind::FuncAttr:
    {
        auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(childType, TypeInfoKind::FuncAttr);
        childType         = typeInfoFunc->returnType;
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeInfoSlice = CastTypeInfo<TypeInfoSlice>(childType, TypeInfoKind::Slice);
        typeExpression->typeFlags |= TYPEFLAG_ISSLICE;
        childType = typeInfoSlice->pointedType;
        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typeInfoPointer        = CastTypeInfo<TypeInfoPointer>(childType, TypeInfoKind::Pointer);
        typeExpression->ptrCount    = 1;
        typeExpression->ptrFlags[0] = typeInfoPointer->isConst() ? AstTypeExpression::PTR_CONST : 0;
        childType                   = typeInfoPointer->pointedType;

        while (childType->kind == TypeInfoKind::Pointer)
        {
            typeInfoPointer                                    = CastTypeInfo<TypeInfoPointer>(childType, TypeInfoKind::Pointer);
            typeExpression->ptrFlags[typeExpression->ptrCount] = typeInfoPointer->isConst() ? AstTypeExpression::PTR_CONST : 0;
            typeExpression->ptrCount++;
            childType = typeInfoPointer->pointedType;
        }

        if (childType->kind == TypeInfoKind::Reference)
        {
            typeExpression->ptrFlags[typeExpression->ptrCount - 1] |= AstTypeExpression::PTR_REF | AstTypeExpression::PTR_CONST;
            auto typeInfoRef = CastTypeInfo<TypeInfoReference>(childType, TypeInfoKind::Reference);
            childType        = typeInfoRef->pointedType;
        }
        break;
    }

    case TypeInfoKind::Reference:
    {
        auto typeInfoRef = CastTypeInfo<TypeInfoReference>(childType, TypeInfoKind::Reference);
        typeExpression->typeFlags |= TYPEFLAG_ISCONST | TYPEFLAG_ISREF;
        childType = typeInfoRef->pointedType;
        parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
        break;
    }
    }

    // Convert typeinfo to TypeExpression
    switch (childType->kind)
    {
    case TypeInfoKind::Native:
        typeExpression->token.id    = TokenId::NativeType;
        typeExpression->literalType = childType;
        break;

    case TypeInfoKind::Enum:
    {
        scoped_lock lk(childType->mutex); // race condition with 'name'
        typeExpression->identifier = Ast::newIdentifierRef(sourceFile, childType->name, typeExpression);
        parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
        break;
    }

    case TypeInfoKind::Struct:
    case TypeInfoKind::Interface:
    {
        scoped_lock lk(childType->mutex); // race condition with 'name'
        typeExpression->identifier = Ast::newIdentifierRef(sourceFile, childType->name, typeExpression);
        break;
    }

    case TypeInfoKind::TypeListTuple:
    {
        AstStruct* inStructNode;
        if (!convertLiteralTupleToStructDecl(context, assignment, &inStructNode))
            return nullptr;
        typeExpression->identifier = Ast::newIdentifierRef(sourceFile, inStructNode->token.text, typeExpression);
        break;
    }

    default:
        context->report({assignment, Utf8::format(Msg0294, orgType->getDisplayName().c_str()).c_str()});
        return nullptr;
    }

    return typeExpression;
}

bool SemanticJob::convertLiteralTupleToStructDecl(SemanticContext* context, AstNode* assignment, AstStruct** result)
{
    auto       sourceFile = context->sourceFile;
    AstStruct* structNode = Ast::newStructDecl(sourceFile, nullptr);
    *result               = structNode;
    structNode->flags |= AST_GENERATED;

    auto contentNode = Ast::newNode(sourceFile, AstNodeKind::TupleContent, structNode);
    contentNode->allocateExtension();
    contentNode->extension->semanticBeforeFct = SemanticJob::preResolveStructContent;
    contentNode->extension->alternativeScopes.push_back(assignment->ownerScope);
    structNode->content = contentNode;

    auto typeList = CastTypeInfo<TypeInfoList>(assignment->typeInfo, TypeInfoKind::TypeListTuple);
    Utf8 varName;
    int  numChilds = (int) typeList->subTypes.size();
    for (int idx = 0; idx < numChilds; idx++)
    {
        auto typeParam = typeList->subTypes[idx];
        auto childType = typeParam->typeInfo;
        auto subAffect = assignment->childs[idx];

        bool autoName = false;
        // User specified name
        if (!typeParam->namedParam.empty())
            varName = typeParam->namedParam;

        // If this is a single identifier, then we take the identifier name
        else if (subAffect->kind == AstNodeKind::IdentifierRef && subAffect->childs.back()->kind == AstNodeKind::Identifier)
        {
            varName               = subAffect->childs.back()->token.text;
            typeParam->namedParam = varName;
        }

        // Otherwise generate an 'item<num>' name
        else
        {
            autoName = true;
            varName  = Utf8::format("item%u", idx);
        }

        auto paramNode = Ast::newVarDecl(sourceFile, varName, contentNode);
        if (autoName)
        {
            typeParam->flags |= TYPEINFO_AUTO_NAME;
            paramNode->flags |= AST_AUTO_NAME;
        }

        paramNode->type = convertTypeToTypeExpression(context, paramNode, subAffect, childType);

        // This can avoid some initialization before assignment, because everything will be covered
        // as this is a tuple
        paramNode->flags |= AST_EXPLICITLY_NOT_INITIALIZED;

        if (!paramNode->type)
            return false;
    }

    // Compute structure name
    structNode->token.text = move(typeList->computeTupleName(context));

    // Add struct type and scope
    structNode->inheritOwners(sourceFile->astRoot);
    Scope*      rootScope = structNode->ownerScope;
    scoped_lock lk(rootScope->symTable.mutex);
    auto        symbol = rootScope->symTable.findNoLock(structNode->token.text);
    if (symbol)
    {
        // Must release struct node, it's useless
    }
    else
    {
        auto typeInfo        = allocType<TypeInfoStruct>();
        auto newScope        = Ast::newScope(structNode, structNode->token.text, ScopeKind::Struct, rootScope, true);
        typeInfo->declNode   = structNode;
        typeInfo->name       = structNode->token.text;
        typeInfo->structName = structNode->token.text;
        typeInfo->scope      = newScope;
        typeInfo->flags |= TYPEINFO_STRUCT_IS_TUPLE;
        structNode->typeInfo = typeInfo;
        structNode->scope    = newScope;
        Ast::visit(structNode->content, [&](AstNode* n) {
            n->ownerStructScope = newScope;
            n->ownerScope       = newScope;
        });

        rootScope->symTable.registerSymbolNameNoLock(context, structNode, SymbolKind::Struct);
        Ast::addChildBack(sourceFile->astRoot, structNode);
        SemanticJob::newJob(context->job->dependentJob, sourceFile, structNode, true);
    }

    return true;
}

bool SemanticJob::convertLiteralTupleToStructDecl(SemanticContext* context, AstNode* parent, AstNode* assignment, AstNode** result)
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

void SemanticJob::setVarDeclResolve(AstVarDecl* varNode)
{
    varNode->allocateExtension();
    varNode->extension->semanticBeforeFct = SemanticJob::resolveVarDeclBefore;
    varNode->extension->semanticAfterFct  = SemanticJob::resolveVarDeclAfter;

    if (varNode->assignment)
    {
        varNode->assignment->allocateExtension();
        varNode->assignment->extension->semanticAfterFct = SemanticJob::resolveVarDeclAfterAssign;
    }

    if (varNode->assignment && varNode->type)
    {
        varNode->type->allocateExtension();
        varNode->type->extension->semanticAfterFct = SemanticJob::resolveVarDeclAfterType;
    }
}

bool SemanticJob::resolveVarDeclAfterType(SemanticContext* context)
{
    auto parent = context->node->parent;
    while (parent && parent->kind != AstNodeKind::VarDecl && parent->kind != AstNodeKind::ConstDecl)
        parent = parent->parent;
    SWAG_ASSERT(parent);
    auto varDecl = (AstVarDecl*) parent;
    if (!varDecl->type || !varDecl->assignment)
        return true;

    // :AutoScope
    // Resolution of an affectation to an enum, without having to specify the enum name before
    // 'using', but just for affectation
    auto typeInfo = TypeManager::concreteType(varDecl->type->typeInfo, CONCRETE_ALIAS);
    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        varDecl->assignment->allocateExtension();
        varDecl->assignment->extension->alternativeScopes.push_front(typeEnum->scope);
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        if (typeArr->finalType->kind == TypeInfoKind::Enum)
        {
            auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeArr->finalType, TypeInfoKind::Enum);
            varDecl->assignment->allocateExtension();
            varDecl->assignment->extension->alternativeScopes.push_front(typeEnum->scope);
        }
    }

    return true;
}

bool SemanticJob::resolveVarDeclAfter(SemanticContext* context)
{
    auto node = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // Ghosting check
    // We simulate a reference to the local variable, in the same context, to raise an error
    // if ambiguous. That way we have a direct error at the declaration, even if the variable
    // is not used later
    if (node->resolvedSymbolOverload->flags & OVERLOAD_VAR_LOCAL)
    {
        if (!context->job->tmpIdRef)
        {
            context->job->tmpIdRef = Ast::newIdentifierRef(context->sourceFile, node->token.text, nullptr, nullptr);
            context->job->tmpIdRef->flags |= AST_SILENT_CHECK;
        }

        auto idRef     = context->job->tmpIdRef;
        idRef->parent  = node->parent;
        auto id        = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
        id->sourceFile = context->sourceFile;
        id->token.text = node->token.text;
        id->inheritOwners(node);
        id->inheritTokenLocation(node->token);
        SWAG_CHECK(resolveIdentifier(context, id, true));
        if (context->result != ContextResult::Done)
            return true;
    }

    // :opAffectConstExpr
    if (node->extension &&
        node->extension->resolvedUserOpSymbolOverload &&
        node->assignment &&
        node->assignment->flags & AST_VALUE_COMPUTED &&
        node->resolvedSymbolOverload->flags & (OVERLOAD_VAR_GLOBAL | OVERLOAD_VAR_STRUCT))
    {
        auto overload = node->resolvedSymbolOverload;

        node->flags &= ~AST_NO_BYTECODE;
        node->flags &= ~AST_VALUE_COMPUTED;
        node->assignment->flags &= ~AST_NO_BYTECODE;
        node->flags |= AST_CONST_EXPR;
        node->semFlags |= AST_SEM_EXEC_RET_STACK;

        node->byteCodeFct                     = ByteCodeGenJob::emitLocalVarDecl;
        overload->computedValue.storageOffset = 0;

        SWAG_CHECK(evaluateConstExpression(context, node));
        if (context->result == ContextResult::Pending)
            return true;

        node->flags |= AST_NO_BYTECODE;
        node->assignment->flags |= AST_NO_BYTECODE;

        auto module = node->sourceFile->module;

        DataSegment* storageSegment = overload->computedValue.storageSegment;
        SWAG_ASSERT(storageSegment);

        // If request segment is compiler, then nothing to do, the value is already stored there
        if (storageSegment->kind == SegmentKind::Compiler)
        {
            overload->computedValue.storageOffset = node->computedValue->storageOffset;
        }

        // Copy value from compiler segment to real requested segment
        else
        {
            uint8_t* addrDest;
            auto     storageOffset                = storageSegment->reserve(node->typeInfo->sizeOf, &addrDest);
            overload->computedValue.storageOffset = storageOffset;
            auto addrSrc                          = module->compilerSegment.address(node->computedValue->storageOffset);
            memcpy(addrDest, addrSrc, node->typeInfo->sizeOf);
        }

        node->computedValue->storageOffset = node->resolvedSymbolOverload->computedValue.storageOffset;

        // Will remove the incomplete flag, and finish the resolve
        node->ownerScope->symTable.addSymbolTypeInfo(context,
                                                     node,
                                                     node->typeInfo,
                                                     overload->symbol->kind,
                                                     nullptr,
                                                     overload->flags & ~OVERLOAD_INCOMPLETE,
                                                     nullptr,
                                                     overload->computedValue.storageOffset,
                                                     overload->computedValue.storageSegment);
    }

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
            isGeneric = node->findParent(AstNodeKind::StructDecl)->flags & AST_IS_GENERIC;

        if (isGeneric)
        {
            node->assignment->typeInfo = g_TypeMgr.typeInfoS32;
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
    if (!(exprList->specFlags & AST_SPEC_EXPRLIST_FORTUPLE))
        return true;

    // If there's an assignment, but no type, then we need to deduce/generate the type with
    // the assignment, then do the semmantic on that type
    if (!varDecl->type)
    {
        SWAG_CHECK(convertLiteralTupleToStructDecl(context, varDecl, assign, &varDecl->type));
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
    SWAG_VERIFY(!identifier->callParameters, context->report({assign, Msg0295}));

    auto sourceFile            = context->sourceFile;
    identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);

    auto numParams = assign->childs.size();
    for (int i = 0; i < numParams; i++)
    {
        auto child = assign->childs[0];

        // This happens if the structure has been generated from the assignment expression
        if (child->kind == AstNodeKind::TypeExpression)
            continue;

        auto param = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        Ast::removeFromParent(child);
        Ast::addChildBack(param, child);
        param->inheritTokenLocation(child->token);
    }

    identifier->callParameters->inheritTokenLocation(varDecl->assignment->token);
    identifier->callParameters->inheritOrFlag(varDecl->assignment, AST_CONST_EXPR | AST_SIDE_EFFECTS);
    identifier->callParameters->flags |= AST_CALL_FOR_STRUCT;
    identifier->flags |= AST_IN_TYPE_VAR_DECLARATION;
    typeExpression->flags &= ~AST_NO_BYTECODE;
    typeExpression->flags &= ~AST_NO_BYTECODE_CHILDS;
    typeExpression->flags &= ~AST_VALUE_COMPUTED;
    typeExpression->flags |= AST_HAS_STRUCT_PARAMETERS;

    Ast::removeFromParent(varDecl->assignment);
    varDecl->assignment = nullptr;

    job->nodes.pop_back();
    typeExpression->semanticState = AstNodeResolveState::Enter;
    job->nodes.push_back(typeExpression);
    job->nodes.push_back(context->node);

    return true;
}

bool SemanticJob::convertTypeListToArray(SemanticContext* context, AstVarDecl* node, bool isCompilerConstant, uint32_t symbolFlags)
{
    auto typeList  = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListArray);
    auto typeArray = TypeManager::convertTypeListToArray(context, typeList, isCompilerConstant);
    node->typeInfo = typeArray;

    // For a global variable, no need to collect in the constant segment, as we will collect directly to the mutable segment
    if (symbolFlags & OVERLOAD_VAR_GLOBAL)
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment, CASTFLAG_NO_COLLECT));
    else
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->typeInfo, nullptr, node->assignment));
    node->typeInfo->sizeOf = node->assignment->typeInfo->sizeOf;

    return true;
}

DataSegment* SemanticJob::getSegmentForVar(SemanticContext* context, AstVarDecl* node)
{
    auto module   = node->sourceFile->module;
    auto typeInfo = TypeManager::concreteType(node->typeInfo);

    if (node->attributeFlags & ATTRIBUTE_TLS)
        return &module->tlsSegment;

    if (node->attributeFlags & ATTRIBUTE_COMPILER)
        return &module->compilerSegment;

    if (node->kind == AstNodeKind::ConstDecl)
        return &module->constantSegment;
    if (node->resolvedSymbolOverload && (node->resolvedSymbolOverload->flags & OVERLOAD_VAR_STRUCT))
        return &module->constantSegment;

    if (node->attributeFlags & AST_EXPLICITLY_NOT_INITIALIZED)
        return &module->mutableSegment;

    if (!node->assignment && (typeInfo->kind == TypeInfoKind::Native || typeInfo->kind == TypeInfoKind::Array))
        return &module->bssSegment;
    if (node->assignment && typeInfo->kind == TypeInfoKind::Native && typeInfo->sizeOf <= 8 && node->assignment->isConstant0())
        return &module->bssSegment;
    if (!node->assignment &&
        !(node->flags & AST_HAS_STRUCT_PARAMETERS) &&
        !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS) &&
        (node->typeInfo->kind == TypeInfoKind::Struct || node->typeInfo->kind == TypeInfoKind::Interface) &&
        !(node->typeInfo->flags & (TYPEINFO_STRUCT_HAS_INIT_VALUES)))
        return &module->bssSegment;

    return &module->mutableSegment;
}

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto node       = static_cast<AstVarDecl*>(context->node);

    bool isCompilerConstant = node->kind == AstNodeKind::ConstDecl ? true : false;
    bool isLocalConstant    = false;

    // Check alias
    if (node->token.text.find("@alias") == 0 && !(node->flags & AST_GENERATED) && !(node->ownerInline))
    {
        auto ownerFct = node->ownerFct;
        while (ownerFct)
        {
            if (ownerFct->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN))
                break;
            ownerFct = ownerFct->ownerFct;
        }
        if (!ownerFct)
            return context->report({node, node->token, Utf8::format(Msg0296, node->token.text.c_str())});
    }

    uint32_t symbolFlags = 0;
    if (node->kind == AstNodeKind::FuncDeclParam)
        symbolFlags |= OVERLOAD_VAR_FUNC_PARAM | OVERLOAD_CONST_ASSIGN;
    else if (node->ownerScope->isGlobal() || (node->attributeFlags & ATTRIBUTE_GLOBAL))
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->isGlobalOrImpl() && (node->flags & AST_INSIDE_IMPL))
        symbolFlags |= OVERLOAD_VAR_GLOBAL;
    else if (node->ownerScope->kind == ScopeKind::Struct)
        symbolFlags |= OVERLOAD_VAR_STRUCT;
    else if (!isCompilerConstant)
        symbolFlags |= OVERLOAD_VAR_LOCAL;
    else
        isLocalConstant = true;
    if (node->specFlags & AST_SPEC_VARDECL_CONSTASSIGN)
        symbolFlags |= OVERLOAD_CONST_ASSIGN;
    if (node->attributeFlags & ATTRIBUTE_TLS)
        symbolFlags |= OVERLOAD_VAR_TLS;

    auto concreteNodeType = node->type && node->type->typeInfo ? TypeManager::concreteType(node->type->typeInfo, CONCRETE_ALIAS) : nullptr;

    // Check attributes
    if (node->attributeFlags & ATTRIBUTE_TLS && node->attributeFlags & ATTRIBUTE_COMPILER)
        return context->report({node, Msg0159});

    // Register public global constant
    if (isCompilerConstant && (node->attributeFlags & ATTRIBUTE_PUBLIC))
    {
        if (node->ownerScope->isGlobalOrImpl() && (node->type || node->assignment))
            node->ownerScope->addPublicConst(node);
    }

    if (node->attributeFlags & ATTRIBUTE_DISCARDABLE && concreteNodeType->kind != TypeInfoKind::Lambda)
        return context->report({node, Utf8::format(Msg0297, concreteNodeType->getDisplayName().c_str())});

    // Check for missing initialization
    // This is ok to not have an initialization for structs, as they are initialized by default
    if (!node->type || concreteNodeType->kind != TypeInfoKind::Struct)
    {
        if (isCompilerConstant && !node->assignment && !(node->flags & AST_VALUE_COMPUTED))
            return context->report({node, Msg0298});
        if ((symbolFlags & OVERLOAD_CONST_ASSIGN) && !node->assignment && node->kind != AstNodeKind::FuncDeclParam)
            return context->report({node, Msg0299});

        if (node->type && node->type->typeInfo->kind == TypeInfoKind::Reference && node->kind != AstNodeKind::FuncDeclParam)
        {
            if (!node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
                return context->report({node, Msg0300});
        }
    }

    // If this is a reference, be sure we can take address of it
    if (node->type && node->type->typeInfo->kind == TypeInfoKind::Reference)
    {
        if (node->assignment)
        {
            SWAG_CHECK(checkCanTakeAddress(context, node->assignment));
            SyntaxJob::forceTakeAddress(node->assignment);
        }
    }

    bool genericType = !node->type && !node->assignment;
    bool isGeneric   = false;
    if (node->flags & AST_STRUCT_MEMBER)
    {
        auto p = node->findParent(AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
        SWAG_ASSERT(p);
        isGeneric = p->flags & AST_IS_GENERIC;
    }

    // Value
    if (node->assignment &&
        node->assignment->kind != AstNodeKind::ExpressionList &&
        node->assignment->kind != AstNodeKind::ExplicitNoInit &&
        !isGeneric)
    {
        // A generic type with a default value is a generic type
        if ((node->flags & AST_IS_GENERIC) && !node->type && !(node->flags & AST_R_VALUE))
            genericType = true;
        else if (!(node->flags & AST_FROM_GENERIC) || !(node->doneFlags & AST_DONE_ASSIGN_COMPUTED))
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

            node->doneFlags |= AST_DONE_ASSIGN_COMPUTED;
            if (node->type &&
                concreteNodeType->kind != TypeInfoKind::Slice &&
                concreteNodeType->kind != TypeInfoKind::Pointer)
            {
                SWAG_VERIFY(node->assignment->typeInfo->kind != TypeInfoKind::Array, context->report({node->assignment, Msg0301}));
            }
        }
    }

    // A global variable or a constant must have its value computed at that point
    if (!(node->flags & AST_FROM_GENERIC))
    {
        if (!isGeneric && node->assignment && (isCompilerConstant || (symbolFlags & OVERLOAD_VAR_GLOBAL)))
        {
            if (node->assignment->typeInfo->kind == TypeInfoKind::Lambda)
            {
                SWAG_VERIFY(!isCompilerConstant, context->report({node->assignment, Msg0160}));
                auto funcNode = CastAst<AstFuncDecl>(node->assignment->typeInfo->declNode, AstNodeKind::FuncDecl, AstNodeKind::TypeLambda);
                SWAG_CHECK(checkCanMakeFuncPointer(context, funcNode, node->assignment));
            }
            else
            {
                SWAG_VERIFY(node->assignment->flags & AST_CONST_EXPR, context->report({node->assignment, Msg0670}));
            }
        }
    }

    // Be sure that an array without a size has an initializer, to deduce its size
    if (concreteNodeType && concreteNodeType->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(concreteNodeType, TypeInfoKind::Array);
        SWAG_VERIFY(typeArray->count != UINT32_MAX || node->assignment, context->report({node, Msg0303}));
        SWAG_VERIFY(!node->assignment || node->assignment->kind == AstNodeKind::ExpressionList || node->assignment->kind == AstNodeKind::ExplicitNoInit, context->report({node, Msg0304}));

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count      = (uint32_t) node->assignment->childs.size();
            typeArray->totalCount = typeArray->count;
            typeArray->sizeOf     = typeArray->count * typeArray->pointedType->sizeOf;
            typeArray->name       = Utf8::format("[%d] %s", typeArray->count, typeArray->pointedType->getDisplayName().c_str());
        }
    }

    if (node->flags & AST_EXPLICITLY_NOT_INITIALIZED)
    {
        SWAG_VERIFY(!isCompilerConstant, context->report({node, Msg0305}));
    }

    // Find type
    if (node->type && node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        SWAG_ASSERT(node->type->typeInfo);

        auto      leftConcreteType = node->type->typeInfo;
        TypeInfo* rightConcreteType;
        if (leftConcreteType->kind != TypeInfoKind::Reference)
            rightConcreteType = TypeManager::concreteReferenceType(node->assignment->typeInfo);
        else
            rightConcreteType = TypeManager::concreteType(node->assignment->typeInfo);

        // Do not cast for structs, as we can have special assignment with different types
        // Except if this is an initializer list {...}
        if (leftConcreteType->kind != TypeInfoKind::Struct || rightConcreteType->isInitializerList())
        {
            // Cast from struct to interface : need to wait for all interfaces to be registered
            if (leftConcreteType->kind == TypeInfoKind::Interface && rightConcreteType->kind == TypeInfoKind::Struct)
            {
                context->job->waitForAllStructInterfaces(rightConcreteType);
                if (context->result == ContextResult::Pending)
                    return true;
            }

            SWAG_CHECK(TypeManager::makeCompatibles(context, node->type->typeInfo, nullptr, node->assignment, CASTFLAG_TRY_COERCE | CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST));
            if (context->result == ContextResult::Pending)
                return true;
        }
        else
        {
            if ((leftConcreteType->kind != rightConcreteType->kind) || !rightConcreteType->isSame(leftConcreteType, ISSAME_CAST))
            {
                if (!hasUserOp(context, "opAffect", node->type))
                {
                    Utf8 msg = Utf8::format(Msg0908, leftConcreteType->getDisplayName().c_str(), rightConcreteType->getDisplayName().c_str(), node->type->typeInfo->getDisplayName().c_str());
                    return context->report({node, msg});
                }

                SWAG_CHECK(resolveUserOp(context, "opAffect", nullptr, nullptr, node->type, node->assignment, false));
                if (context->result == ContextResult::Pending)
                    return true;

                // :opAffectConstExpr
                if (symbolFlags & (OVERLOAD_VAR_STRUCT | OVERLOAD_VAR_GLOBAL))
                {
                    symbolFlags |= OVERLOAD_INCOMPLETE;
                    SWAG_ASSERT(node->extension && node->extension->resolvedUserOpSymbolOverload);
                    if (!(node->extension->resolvedUserOpSymbolOverload->node->attributeFlags & ATTRIBUTE_CONSTEXPR))
                    {
                        PushErrHint errh(Utf8::format(Hnt0002, leftConcreteType->getDisplayName().c_str()));
                        return context->report({node->assignment, Msg0906});
                    }
                }
            }
        }

        node->typeInfo = node->type->typeInfo;

        // A slice initialized with an expression list must be immutable
        if (leftConcreteType->kind == TypeInfoKind::Slice && rightConcreteType->kind == TypeInfoKind::TypeListArray && (node->assignment->flags & AST_CONST_EXPR))
        {
            SWAG_VERIFY(leftConcreteType->isConst(), context->report({node->type, Msg0306}));
        }
    }
    else if (node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        node->typeInfo = TypeManager::concreteReferenceType(node->assignment->typeInfo, CONCRETE_FUNC);
        SWAG_ASSERT(node->typeInfo);
        SWAG_VERIFY(node->typeInfo != g_TypeMgr.typeInfoVoid, context->report({node->assignment, Msg0307}));

        // We need to decide which integer/float type it is
        node->typeInfo = TypeManager::solidifyUntyped(node->typeInfo);

        // Convert from initialization list to array
        if (node->typeInfo->kind == TypeInfoKind::TypeListArray)
            SWAG_CHECK(convertTypeListToArray(context, node, isCompilerConstant, symbolFlags));
    }
    else if (node->type)
    {
        node->typeInfo = node->type->typeInfo;
    }

    if (node->type)
        node->inheritOrFlag(node->type, AST_IS_GENERIC);
    if (node->flags & AST_IS_GENERIC)
    {
        symbolFlags |= OVERLOAD_GENERIC;
        if (genericType && node->assignment)
        {
            auto typeGeneric     = allocType<TypeInfoGeneric>();
            typeGeneric->name    = node->token.text;
            typeGeneric->rawType = node->typeInfo;
            node->typeInfo       = typeGeneric;
        }
        else if (!node->typeInfo)
        {
            node->typeInfo       = allocType<TypeInfoGeneric>();
            node->typeInfo->name = node->token.text;
        }
    }

    if (!node->typeInfo || node->typeInfo == g_TypeMgr.typeInfoUndefined)
    {
        // If this is a lambda parameter in an expression, this is fine, we will try to deduce the type
        if ((node->ownerFct && node->kind == AstNodeKind::FuncDeclParam && (node->ownerFct->flags & AST_IS_LAMBDA_EXPRESSION)) ||
            node->typeInfo == g_TypeMgr.typeInfoUndefined)
        {
            node->typeInfo = g_TypeMgr.typeInfoUndefined;
            genericType    = false;

            // AST_PENDING_LAMBDA_TYPING will stop semantic, forcing to not evaluate the content of the function,
            // until types are known
            if (node->ownerFct && node->ownerScope->kind != ScopeKind::Struct)
                node->ownerFct->flags |= AST_PENDING_LAMBDA_TYPING;
        }
    }

    // Type should be a correct one
    SWAG_VERIFY(node->typeInfo != g_TypeMgr.typeInfoNull, context->report({node, node->token, Msg0308}));

    // We should have a type here !
    SWAG_VERIFY(node->typeInfo, context->report({node, node->token, Utf8::format(Msg0309, AstNode::getKindName(node).c_str(), node->token.text.c_str())}));

    // Determine if the call parameters cover everything (to avoid calling default initialization)
    // i.e. set AST_HAS_FULL_STRUCT_PARAMETERS
    if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
    {
        auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);

        TypeInfoStruct* typeStruct = nullptr;
        if (node->typeInfo->kind == TypeInfoKind::Struct)
            typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        else if (node->typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(node->typeInfo, TypeInfoKind::Array);
            while (typeArray->pointedType->kind == TypeInfoKind::Array)
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
            if (node->typeInfo->flags & TYPEINFO_RETURN_BY_COPY)
            {
                node->typeInfo = TypeManager::makeConst(node->typeInfo);
            }
        }
    }

    auto typeInfo = TypeManager::concreteType(node->typeInfo, CONCRETE_ALIAS);

    // An enum must be initialized
    if (!node->assignment &&
        node->parent->kind != AstNodeKind::FuncDeclParams && // generic, better test ?
        !(symbolFlags & OVERLOAD_VAR_FUNC_PARAM) &&
        !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        if (typeInfo->kind == TypeInfoKind::Enum ||
            (typeInfo->kind == TypeInfoKind::Array && ((TypeInfoArray*) typeInfo)->pointedType->kind == TypeInfoKind::Enum))
        {
            return context->report({node, node->token, Msg0310});
        }
    }

    typeInfo = TypeManager::concreteType(node->typeInfo);

    // In case of a struct (or array of structs), be sure struct is now completed before
    // parsing variable.
    // Otherwise there's a chance, for example, that 'sizeof' is 0, which can lead to various
    // problems.
    if (isCompilerConstant || (symbolFlags & OVERLOAD_VAR_GLOBAL) || (symbolFlags & OVERLOAD_VAR_LOCAL))
    {
        context->job->waitTypeCompleted(typeInfo);
        if (context->result == ContextResult::Pending)
            return true;
    }

    uint32_t     storageOffset  = UINT32_MAX;
    DataSegment* storageSegment = nullptr;
    if (isCompilerConstant)
    {
        node->flags |= AST_NO_BYTECODE | AST_R_VALUE;
        if (!isGeneric)
        {
            SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->report({node, Utf8::format(Msg0311, node->typeInfo->getDisplayName().c_str())}));

            storageSegment = getSegmentForVar(context, node);
            if (node->extension && node->extension->resolvedUserOpSymbolOverload)
            {
                storageOffset = 0;
                symbolFlags |= OVERLOAD_INCOMPLETE;
            }
            else if (typeInfo->kind == TypeInfoKind::Array || typeInfo->kind == TypeInfoKind::Struct)
            {
                if (node->assignment && node->assignment->flags & AST_VALUE_COMPUTED)
                {
                    storageOffset  = node->assignment->computedValue->storageOffset;
                    storageSegment = node->assignment->computedValue->storageSegment;
                }
                else if (node->flags & AST_VALUE_COMPUTED)
                {
                    storageOffset  = node->computedValue->storageOffset;
                    storageSegment = node->computedValue->storageSegment;
                }
                else
                {
                    node->allocateComputedValue();
                    SWAG_CHECK(collectAssignment(context, storageSegment, storageOffset, node));
                }
            }
            else if (typeInfo->kind == TypeInfoKind::Slice)
            {
                if (!(node->flags & AST_VALUE_COMPUTED))
                {
                    node->assignment->setFlagsValueIsComputed();
                    SWAG_CHECK(reserveAndStoreToSegment(context, storageSegment, storageOffset, node->assignment->computedValue, node->assignment->typeInfo, node->assignment));

                    auto typeList                                   = CastTypeInfo<TypeInfoList>(node->assignment->typeInfo, TypeInfoKind::TypeListArray);
                    node->assignment->computedValue->reg.u64        = typeList->subTypes.size();
                    node->assignment->computedValue->storageOffset  = storageOffset;
                    node->assignment->computedValue->storageSegment = storageSegment;
                }
                else
                {
                    storageOffset  = node->computedValue->storageOffset;
                    storageSegment = node->computedValue->storageSegment;
                }
            }

            node->inheritComputedValue(node->assignment);
            SWAG_ASSERT(node->computedValue);
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
        SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->report({node, Utf8::format(Msg0312, node->typeInfo->getDisplayName().c_str())}));
        SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_PUBLIC), context->report({node, Msg0313}));

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
        }

        if (node->extension && node->extension->resolvedUserOpSymbolOverload)
        {
            storageOffset = 0;
            symbolFlags |= OVERLOAD_INCOMPLETE;
        }
        else
        {
            SWAG_CHECK(collectAssignment(context, storageSegment, storageOffset, node));
        }

        // Register variable for debug
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
        }
    }
    else if (symbolFlags & OVERLOAD_VAR_LOCAL)
    {
        // For a struct, need to wait for special functions to be found
        if (typeInfo->kind == TypeInfoKind::Struct || typeInfo->isArrayOfStruct())
        {
            SWAG_CHECK(waitForStructUserOps(context, node));
            if (context->result == ContextResult::Pending)
                return true;
        }

        if (node->typeInfo->flags & TYPEINFO_GENERIC)
        {
            SWAG_CHECK(Generic::instantiateDefaultGeneric(context, node));
            if (context->result != ContextResult::Done)
                return true;
        }

        SWAG_ASSERT(node->ownerScope);
        SWAG_ASSERT(node->ownerFct);

        // Do not allocate space on the stack for a 'retval' variable, because it's not really a variable
        if (node->type && node->type->kind == AstNodeKind::TypeExpression)
        {
            auto typeExpr = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
            if (typeExpr->typeFlags & TYPEFLAG_RETVAL)
                symbolFlags |= OVERLOAD_RETVAL;
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
            auto alignOf                     = SemanticJob::alignOf(node);
            node->ownerScope->startStackSize = (uint32_t) TypeManager::align(node->ownerScope->startStackSize, alignOf);
            storageOffset                    = node->ownerScope->startStackSize;
            node->ownerScope->startStackSize += typeInfo->sizeOf;
            node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
            // Be sure we have a stack if a variable is declared, even if sizeof is null (for an empty struct for example)
            node->ownerFct->stackSize = max(node->ownerFct->stackSize, 1);
        }

        node->allocateExtension();
        node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitLocalVarDeclBefore;
        node->byteCodeFct                  = ByteCodeGenJob::emitLocalVarDecl;
        node->flags |= AST_R_VALUE;
    }
    else if (symbolFlags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->flags |= AST_R_VALUE;

        // A struct/interface is forced to be a const reference
        if (!(node->typeInfo->flags & TYPEINFO_GENERIC))
        {
            if (typeInfo->kind == TypeInfoKind::Struct)
            {
                // If this has been transformed to an alias cause of const, take the original
                // type to make the reference
                if (typeInfo->flags & TYPEINFO_FAKE_ALIAS)
                    typeInfo = ((TypeInfoAlias*) typeInfo)->rawType;

                auto typeRef          = allocType<TypeInfoReference>();
                typeRef->flags        = typeInfo->flags | TYPEINFO_CONST;
                typeRef->pointedType  = typeInfo;
                typeRef->originalType = node->typeInfo;
                typeRef->computeName();
                node->typeInfo = typeRef;
            }
        }
    }

    // A using on a variable
    if (node->flags & AST_DECL_USING)
    {
        SWAG_CHECK(resolveUsingVar(context, context->node, node->typeInfo));
    }

    // Register symbol with its type
    auto overload = node->ownerScope->symTable.addSymbolTypeInfo(context,
                                                                 node,
                                                                 node->typeInfo,
                                                                 genericType ? SymbolKind::GenericType : SymbolKind::Variable,
                                                                 isCompilerConstant ? node->computedValue : nullptr,
                                                                 symbolFlags,
                                                                 nullptr,
                                                                 storageOffset,
                                                                 storageSegment);
    SWAG_CHECK(overload);
    node->resolvedSymbolOverload = overload;

    return true;
}
