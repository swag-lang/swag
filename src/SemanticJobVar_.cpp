#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Generic.h"

uint32_t SemanticJob::alignOf(AstVarDecl* node)
{
    ComputedValue value;
    if (node->attributes.getValue("swag.align", "value", value))
        return value.reg.u8;
    return TypeManager::alignOf(TypeManager::concreteType(node->typeInfo));
}

// Will be called after solving the initial var affect, but before tuple unpacking
bool SemanticJob::resolveTupleUnpackBefore(SemanticContext* context)
{
    auto scopeNode = CastAst<AstNode>(context->node->parent, AstNodeKind::StatementNoScope, AstNodeKind::Statement);
    auto varDecl   = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);

    auto typeVar = TypeManager::concreteType(varDecl->typeInfo);
    SWAG_VERIFY(typeVar->kind == TypeInfoKind::Struct, context->report({varDecl, varDecl->token, format("cannot unpack type '%s' which is not a struct", typeVar->name.c_str())}));
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeVar, TypeInfoKind::Struct);
    auto numUnpack  = scopeNode->childs.size() - 1;
    SWAG_VERIFY(typeStruct->fields.size(), context->report({varDecl, varDecl->token, format("cannot unpack %s because it does not contain any field", typeStruct->getDisplayName().c_str())}));
    SWAG_VERIFY(numUnpack <= typeStruct->fields.size(), context->report({varDecl, varDecl->token, format("attempt to unpack '%u' variables, but %s contains only '%u' field(s)", numUnpack, typeStruct->getDisplayName().c_str(), typeStruct->fields.size())}));
    return true;
}

AstNode* SemanticJob::convertTypeToTypeExpression(SemanticContext* context, AstNode* parent, AstNode* assignment, TypeInfo* childType)
{
    auto sourceFile = context->sourceFile;
    auto orgType    = childType;

    /*if (childType->kind == TypeInfoKind::Lambda)
    {
        auto typeLambda         = Ast::newNode<AstTypeLambda>(nullptr, AstNodeKind::TypeLambda, sourceFile, parent);
        typeLambda->semanticFct = SemanticJob::resolveTypeLambda;
        return typeLambda;
    }*/

    auto typeExpression = Ast::newTypeExpression(sourceFile, parent);
    typeExpression->flags |= AST_NO_BYTECODE_CHILDS;

    if (childType->kind == TypeInfoKind::Pointer)
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
    }
    else if (childType->kind == TypeInfoKind::Reference)
    {
        auto typeInfoRef = CastTypeInfo<TypeInfoReference>(childType, TypeInfoKind::Reference);
        typeExpression->typeFlags |= TYPEFLAG_ISCONST | TYPEFLAG_ISREF;
        childType = typeInfoRef->pointedType;
        parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
    }

    // Convert typeinfo to TypeExpression
    switch (childType->kind)
    {
    case TypeInfoKind::Native:
        typeExpression->token.id    = TokenId::NativeType;
        typeExpression->literalType = childType;
        break;
    case TypeInfoKind::Enum:
        childType->computeScopedName();
        typeExpression->identifier = Ast::newIdentifierRef(sourceFile, childType->scopedName, typeExpression);
        parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
        break;
    case TypeInfoKind::Struct:
    case TypeInfoKind::TypeSet:
    case TypeInfoKind::Interface:
        childType->computeScopedName();
        typeExpression->identifier = Ast::newIdentifierRef(sourceFile, childType->scopedName, typeExpression);
        break;
    case TypeInfoKind::TypeListTuple:
    {
        AstStruct* inStructNode;
        if (!convertLiteralTupleToStructDecl(context, assignment, &inStructNode))
            return nullptr;
        typeExpression->identifier = Ast::newIdentifierRef(sourceFile, inStructNode->token.text, typeExpression);
        break;
    }
    default:
        context->report({assignment, format("tuple initialization with type '%s' is not supported", orgType->name.c_str()).c_str()});
        return nullptr;
    }

    return typeExpression;
}

bool SemanticJob::convertLiteralTupleToStructDecl(SemanticContext* context, AstNode* assignment, AstStruct** result)
{
    auto       sourceFile = context->sourceFile;
    AstStruct* structNode = Ast::newStructDecl(sourceFile, nullptr);
    *result               = structNode;

    auto contentNode = Ast::newNode(sourceFile, AstNodeKind::TupleContent, structNode);
    contentNode->allocateExtension();
    contentNode->extension->semanticBeforeFct = SemanticJob::preResolveStructContent;
    structNode->content                       = contentNode;

    auto typeList = CastTypeInfo<TypeInfoList>(assignment->typeInfo, TypeInfoKind::TypeListTuple);
    Utf8 varName;
    int  numChilds = (int) typeList->subTypes.size();
    for (int idx = 0; idx < numChilds; idx++)
    {
        auto typeParam = typeList->subTypes[idx];
        auto childType = typeParam->typeInfo;

        bool autoName = false;
        if (!typeParam->namedParam.empty())
            varName = typeParam->namedParam;
        else
        {
            autoName = true;
            varName  = format("item%u", idx);
        }

        auto paramNode = Ast::newVarDecl(sourceFile, varName, contentNode);
        if (autoName)
        {
            typeParam->flags |= TYPEINFO_AUTO_NAME;
            paramNode->flags |= AST_AUTO_NAME;
        }

        paramNode->type = convertTypeToTypeExpression(context, paramNode, assignment->childs[idx], childType);
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

    // Resolution of an affectation to an enum, without having to specific the enum name before
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

bool SemanticJob::resolveVarDeclBefore(SemanticContext* context)
{
    auto node = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

    // Collect all attributes for the variable
    SWAG_CHECK(collectAttributes(context, node, &node->attributes));

    if (node->assignment && node->kind == AstNodeKind::ConstDecl)
    {
        bool isGeneric = node->ownerMainNode && (node->ownerMainNode->flags & AST_IS_GENERIC);
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
    if (!exprList->forTuple)
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
    SWAG_VERIFY(!identifier->callParameters, context->report({assign, "structure cannot be initialized twice"}));

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
    identifier->callParameters->inheritOrFlag(varDecl->assignment, AST_CONST_EXPR);
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

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto module     = sourceFile->module;
    auto node       = static_cast<AstVarDecl*>(context->node);

    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node, SymbolKind::Variable));
    if (context->result == ContextResult::Pending)
        return true;

    bool isCompilerConstant = node->kind == AstNodeKind::ConstDecl ? true : false;
    bool isLocalConstant    = false;

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
    if (node->constAssign)
        symbolFlags |= OVERLOAD_CONST_ASSIGN;

    // Check public
    if (isCompilerConstant && (node->attributeFlags & ATTRIBUTE_PUBLIC))
    {
        if (!node->ownerMainNode || (node->ownerMainNode->kind != AstNodeKind::StructDecl &&
                                     node->ownerMainNode->kind != AstNodeKind::TypeSet &&
                                     node->ownerMainNode->kind != AstNodeKind::InterfaceDecl))
        {
            if (node->ownerScope->isGlobalOrImpl())
            {
                if (node->type || node->assignment)
                {
                    node->ownerScope->addPublicConst(node);
                }
            }
        }
    }

    auto concreteNodeType = node->type && node->type->typeInfo ? TypeManager::concreteType(node->type->typeInfo, CONCRETE_ALIAS) : nullptr;

    // Check for missing initialization
    // This is ok to not have an initialization for structs, as they are initialized by default
    if (!node->type || concreteNodeType->kind != TypeInfoKind::Struct)
    {
        if (isCompilerConstant && !node->assignment && !(node->flags & AST_VALUE_COMPUTED))
            return context->report({node, "a constant must be initialized"});
        if ((symbolFlags & OVERLOAD_CONST_ASSIGN) && !node->assignment && node->kind != AstNodeKind::FuncDeclParam)
            return context->report({node, "a non mutable variable must be initialized"});

        if (node->type && node->type->typeInfo->kind == TypeInfoKind::Reference && node->kind != AstNodeKind::FuncDeclParam)
        {
            if (!node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
                return context->report({node, "a reference must be initialized"});
        }
    }

    bool genericType = !node->type && !node->assignment;
    bool isGeneric   = node->ownerMainNode && (node->ownerMainNode->flags & AST_IS_GENERIC);

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
            }

            node->doneFlags |= AST_DONE_ASSIGN_COMPUTED;
            if (node->type &&
                concreteNodeType->kind != TypeInfoKind::Slice &&
                concreteNodeType->kind != TypeInfoKind::Pointer)
            {
                SWAG_VERIFY(node->assignment->typeInfo->kind != TypeInfoKind::Array, context->report({node->assignment, "affect not allowed from an array"}));
            }
        }
    }

    // A global variable or a constant must have its value computed at that point
    if (!(node->flags & AST_FROM_GENERIC))
    {
        if (!isGeneric && node->assignment && (isCompilerConstant || (symbolFlags & OVERLOAD_VAR_GLOBAL)))
        {
            SWAG_VERIFY(node->assignment->flags & AST_CONST_EXPR, context->report({node->assignment, "initialization expression cannot be evaluated at compile time"}));
        }
    }

    // Be sure that an array without a size has an initializer, to deduce its size
    if (concreteNodeType && concreteNodeType->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(concreteNodeType, TypeInfoKind::Array);
        SWAG_VERIFY(typeArray->count != UINT32_MAX || node->assignment, context->report({node, "missing initialization expression to deduce size of array"}));
        SWAG_VERIFY(!node->assignment || node->assignment->kind == AstNodeKind::ExpressionList || node->assignment->kind == AstNodeKind::ExplicitNoInit, context->report({node, "invalid initialization expression for an array"}));

        // Deduce size of array
        if (typeArray->count == UINT32_MAX)
        {
            typeArray->count      = (uint32_t) node->assignment->childs.size();
            typeArray->totalCount = typeArray->count;
            typeArray->sizeOf     = typeArray->count * typeArray->pointedType->sizeOf;
            typeArray->name       = format("[%d] %s", typeArray->count, typeArray->pointedType->name.c_str());
        }
    }

    if (node->flags & AST_EXPLICITLY_NOT_INITIALIZED)
    {
        SWAG_VERIFY(!isCompilerConstant, context->report({node, "a constant must be explicitly initialized"}));
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

            SWAG_CHECK(TypeManager::makeCompatibles(context, node->type->typeInfo, nullptr, node->assignment, CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST));
            if (context->result == ContextResult::Pending)
                return true;
        }
        else
        {
            if ((leftConcreteType->kind != rightConcreteType->kind) || !rightConcreteType->isSame(leftConcreteType, ISSAME_CAST))
            {
                if (!hasUserOp(context, "opAffect", node->type))
                {
                    Utf8 msg = format("'%s = %s' is impossible because special function 'opAffect' cannot be found in '%s'", leftConcreteType->name.c_str(), rightConcreteType->name.c_str(), node->type->typeInfo->name.c_str());
                    return context->report({node, msg});
                }

                SWAG_CHECK(resolveUserOp(context, "opAffect", nullptr, nullptr, node->type, node->assignment, false));
                if (context->result == ContextResult::Pending)
                    return true;
            }
        }

        node->typeInfo = node->type->typeInfo;

        // A slice initialized with an expression list must be immutable
        if (leftConcreteType->kind == TypeInfoKind::Slice && rightConcreteType->kind == TypeInfoKind::TypeListArray && (node->assignment->flags & AST_CONST_EXPR))
        {
            SWAG_VERIFY(leftConcreteType->isConst(), context->report({node->type, "'slice' must de declared as immutable with 'const'"}));
        }
    }
    else if (node->assignment && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        node->typeInfo = TypeManager::concreteType(node->assignment->typeInfo, CONCRETE_FUNC);
        SWAG_ASSERT(node->typeInfo);
        SWAG_VERIFY(node->typeInfo != g_TypeMgr.typeInfoVoid, context->report({node->assignment, "type of expression is 'void'"}));

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

    // Slices can't be constant. Use array
    SWAG_VERIFY(!isCompilerConstant || node->typeInfo->kind != TypeInfoKind::Slice, context->report({node, "cannot declare slices as 'const'. Use an array instead"}));

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
    SWAG_VERIFY(node->typeInfo != g_TypeMgr.typeInfoNull, context->report({node, node->token, "cannot deduce type from 'null'"}));

    // We should have a type here !
    SWAG_VERIFY(node->typeInfo, context->report({node, node->token, format("unable to deduce type of %s '%s'", AstNode::getKindName(node).c_str(), node->token.text.c_str())}));

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
    if (!node->assignment && !(symbolFlags & OVERLOAD_VAR_FUNC_PARAM) && !(node->flags & AST_EXPLICITLY_NOT_INITIALIZED))
    {
        if (typeInfo->kind == TypeInfoKind::Enum ||
            (typeInfo->kind == TypeInfoKind::Array && ((TypeInfoArray*) typeInfo)->pointedType->kind == TypeInfoKind::Enum))
        {
            return context->report({node, node->token, "an enum variable must be initialized"});
        }
    }

    typeInfo               = TypeManager::concreteType(node->typeInfo);
    uint32_t storageOffset = UINT32_MAX;
    if (isCompilerConstant)
    {
        node->flags |= AST_NO_BYTECODE | AST_R_VALUE;
        if (!isGeneric)
        {
            // A constant does nothing on backend, except if it can't be stored in a ComputedValue struct
            if (typeInfo->kind == TypeInfoKind::Array || typeInfo->kind == TypeInfoKind::Struct)
            {
                // Value already stored in the node
                if (node->flags & AST_VALUE_COMPUTED)
                    storageOffset = node->computedValue.reg.offset;
                else
                    SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->constantSegment));
            }

            node->inheritComputedValue(node->assignment);
            module->addGlobalVar(node, GlobalVarKind::Constant);
        }
    }
    else if (symbolFlags & OVERLOAD_VAR_GLOBAL)
    {
        SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->report({node, format("cannot instantiate variable because type '%s' is generic", node->typeInfo->name.c_str())}));
        SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_PUBLIC), context->report({node, "a global variable cannot be declared as 'public'"}));

        node->flags |= AST_R_VALUE;

        if (node->attributeFlags & ATTRIBUTE_COMPILER)
        {
            symbolFlags |= OVERLOAD_VAR_COMPILER;
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->compilerSegment));
        }
        else if (node->attributeFlags & AST_EXPLICITLY_NOT_INITIALIZED)
        {
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->mutableSegment));
        }
        else if (node->attributeFlags & ATTRIBUTE_NO_BSS)
        {
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->mutableSegment));
        }
        else if (!node->assignment && (typeInfo->kind == TypeInfoKind::Native || typeInfo->kind == TypeInfoKind::Array))
        {
            symbolFlags |= OVERLOAD_VAR_BSS;
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->bssSegment));
        }
        else if (node->assignment && typeInfo->kind == TypeInfoKind::Native && typeInfo->sizeOf <= 8 && node->assignment->isConstant0())
        {
            symbolFlags |= OVERLOAD_VAR_BSS;
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->bssSegment));
        }
        else if (!node->assignment &&
                 !(node->flags & AST_HAS_STRUCT_PARAMETERS) &&
                 !(node->flags & AST_HAS_FULL_STRUCT_PARAMETERS) &&
                 (node->typeInfo->kind == TypeInfoKind::Struct || node->typeInfo->kind == TypeInfoKind::TypeSet || node->typeInfo->kind == TypeInfoKind::Interface) &&
                 !(node->typeInfo->flags & (TYPEINFO_STRUCT_HAS_INIT_VALUES)))
        {
            symbolFlags |= OVERLOAD_VAR_BSS;
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->bssSegment));
        }
        else
        {
            SWAG_CHECK(collectAssignment(context, storageOffset, node, &module->mutableSegment));
        }

        if (!(symbolFlags & OVERLOAD_VAR_COMPILER))
            module->addGlobalVar(node, symbolFlags & OVERLOAD_VAR_BSS ? GlobalVarKind::Bss : GlobalVarKind::Mutable);
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
                storageOffset += c->resolvedSymbolOverload->storageOffset;
            }
        }

        // Reserse room on the stack, except for a retval
        else if (!(symbolFlags & OVERLOAD_RETVAL))
        {
            auto alignOf                     = SemanticJob::alignOf(node);
            node->ownerScope->startStackSize = (uint32_t) TypeManager::align(node->ownerScope->startStackSize, alignOf);
            storageOffset                    = node->ownerScope->startStackSize;
            node->ownerScope->startStackSize += typeInfo->sizeOf;
            node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
            node->ownerFct->stackSize = max(node->ownerFct->stackSize, 1); // Be sure we have a stack if a variable is declared, even if sizeof is null (for an empty struct for example)
        }

        node->byteCodeFct = ByteCodeGenJob::emitLocalVarDecl;
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
                                                                 isCompilerConstant ? &node->computedValue : nullptr,
                                                                 symbolFlags,
                                                                 nullptr,
                                                                 storageOffset);
    SWAG_CHECK(overload);
    node->resolvedSymbolOverload = overload;

    return true;
}
