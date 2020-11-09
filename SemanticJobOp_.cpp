#include "pch.h"
#include "TypeManager.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SourceFile.h"

bool SemanticJob::checkFuncPrototype(SemanticContext* context, AstFuncDecl* node)
{
    if (node->flags & AST_GENERATED)
        return true;
    SWAG_CHECK(checkFuncPrototypeProperty(context, node));
    SWAG_CHECK(checkFuncPrototypeOp(context, node));
    return true;
}

bool SemanticJob::checkFuncPrototypeProperty(SemanticContext* context, AstFuncDecl* node)
{
    if (!(node->attributeFlags & ATTRIBUTE_PROPERTY))
        return true;

    const auto& name       = node->name;
    auto        parameters = node->parameters;
    auto        numParams  = parameters ? parameters->childs.size() : 0;
    auto        returnType = node->returnType ? node->returnType->typeInfo : nullptr;

    SWAG_VERIFY(numParams, context->report({node, node->token, format("not enough parameters for property function '%s'", name.c_str())}));
    SWAG_VERIFY(numParams <= 2, context->report({parameters, format("too many parameters for property function '%s'", name.c_str())}));

    // Get
    if (numParams == 1)
    {
        SWAG_VERIFY(returnType && !returnType->isNative(NativeTypeKind::Void), context->report({node, node->token, format("get property '%s' must have a return type", name.c_str())}));
    }

    // Set
    if (numParams == 2)
    {
        SWAG_VERIFY(!returnType || returnType->isNative(NativeTypeKind::Void), context->report({node->returnType, format("set property '%s' cannot have a return type", name.c_str())}));
    }

    return true;
}

bool SemanticJob::checkFuncPrototypeOp(SemanticContext* context, AstFuncDecl* node)
{
    // Check operators
    const auto& name = node->name;
    if (name.length() < 3)
        return true;

    // A special function starts with 'op', and then there's an upper case letter
    if (name[0] != 'o' || name[1] != 'p' || name[2] < 'A' || name[2] > 'Z')
        return true;

    node->flags |= AST_IS_SPECIAL_FUNC;
    auto parent = node->parent;
    while (parent && parent->kind != AstNodeKind::Impl)
        parent = parent->parent;

    auto parameters = node->parameters;
    auto returnType = node->returnType;

    SWAG_VERIFY(parent, context->report({node, node->token, format("special function '%s' should be defined in a 'impl' scope", name.c_str())}));
    auto implNode = CastAst<AstImpl>(parent, AstNodeKind::Impl);

    // No need to raise an error, the semantic pass on the impl node will fail
    auto typeStruct = implNode->identifier->typeInfo;
    if (typeStruct->kind != TypeInfoKind::Struct)
        return true;

    // First parameter must be be struct
    SWAG_VERIFY(node->parameters, context->report({node, node->token, format("missing parameters for special function '%s'", name.c_str())}));
    auto firstType = node->parameters->childs.front()->typeInfo;
    SWAG_VERIFY(firstType->kind == TypeInfoKind::Pointer, context->report({node->parameters->childs.front(), format("invalid first parameter type for special function '%s' ('%s' expected, '%s' provided)", name.c_str(), typeStruct->name.c_str(), firstType->name.c_str())}));
    auto firstTypePtr = static_cast<TypeInfoPointer*>(firstType);
    SWAG_VERIFY(firstTypePtr->ptrCount == 1, context->report({node->parameters->childs.front(), format("invalid first parameter type for special function '%s' ('%s' expected, '%s' provided)", name.c_str(), typeStruct->name.c_str(), firstType->name.c_str())}));
    SWAG_VERIFY(firstTypePtr->finalType->isSame(typeStruct, ISSAME_CAST), context->report({node->parameters->childs.front(), format("invalid first parameter type for special function '%s' ('%s' expected, '%s' provided)", name.c_str(), typeStruct->name.c_str(), firstType->name.c_str())}));

    bool isOpVisit = name.find("opVisit") == 0;

    // Generic operator must have one generic parameter of type string
    if (name == "opBinary" || name == "opUnary" || name == "opAssign" || name == "opIndexAssign")
    {
        SWAG_VERIFY(node->genericParameters && node->genericParameters->childs.size() == 1, context->report({node, node->token, format("invalid number of generic parameters for special function '%s'", name.c_str())}));
        auto firstGen = node->genericParameters->childs.front();
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr.typeInfoString, ISSAME_CAST), context->report({firstGen, format("invalid generic parameter for special function '%s' ('string' expected, '%s' provided)", name.c_str(), firstGen->name.c_str())}));
    }
    else if (isOpVisit)
    {
        SWAG_VERIFY(node->genericParameters && node->genericParameters->childs.size() == 1, context->report({node, node->token, format("invalid number of generic parameters for special function '%s'", name.c_str())}));
        auto firstGen = node->genericParameters->childs.front();
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr.typeInfoBool, ISSAME_CAST), context->report({firstGen, format("invalid generic parameter for special function '%s' ('bool' expected, '%s' provided)", name.c_str(), firstGen->name.c_str())}));
        SWAG_VERIFY(node->attributeFlags & ATTRIBUTE_MACRO, context->report({node, node->token, "'opVisit' must have the 'swag.macro' attribute"}));
        SWAG_VERIFY(node->attributeFlags & ATTRIBUTE_NO_RETURN, context->report({node, node->token, "'opVisit' must have the 'swag.noreturn' attribute"}));
    }
    else if (name == "opCast")
    {
        SWAG_VERIFY(node->genericParameters && node->genericParameters->childs.size() == 1, context->report({node, node->token, format("invalid number of generic parameters for special function '%s'", name.c_str())}));
    }

    // Check each function
    if (name == "opCast")
    {
    }
    else if (isOpVisit)
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->report({returnType, format("invalid return type for special function '%s' ('void' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
        SWAG_VERIFY(parameters->childs[1]->typeInfo->isSame(g_TypeMgr.typeInfoCode, ISSAME_CAST), context->report({parameters->childs[1], format("invalid parameter '2' for special function '%s' ('code' expected, '%s' provided)", name.c_str(), parameters->childs[1]->typeInfo->name.c_str())}));
    }
    else if (name == "opInit")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 1, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
    }
    else if (name == "opEquals")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(g_TypeMgr.typeInfoBool, ISSAME_CAST), context->report({returnType, format("invalid return type for special function '%s' ('bool' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opCmp")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(g_TypeMgr.typeInfoS32, ISSAME_CAST), context->report({returnType, format("invalid return type for special function '%s' ('s32' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opBinary")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(typeStruct, ISSAME_CAST), context->report({returnType, format("invalid return type for special function '%s' ('%s' expected, '%s' provided)", name.c_str(), typeStruct->name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opUnary")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 1, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(typeStruct, ISSAME_CAST), context->report({returnType, format("invalid return type for special function '%s' ('%s' expected, '%s' provided)", name.c_str(), typeStruct->name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opPostCopy" || name == "opPostMove" || name == "opDrop")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 1, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->report({returnType, format("invalid return type for special function '%s' ('void' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opCount")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 1, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(g_TypeMgr.typeInfoU32, ISSAME_CAST), context->report({returnType, format("invalid return type for special function '%s' ('u32' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opData")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 1, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType && returnType->typeInfo->isSame(g_TypeMgr.typeInfoConstPVoid, ISSAME_CAST | ISSAME_EXACT), context->report({returnType, format("invalid return type for special function '%s' ('const *void' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opAssign")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->report({returnType, format("invalid return type for special function '%s' ('void' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else if (name == "opAffect")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 2, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->report({returnType, format("invalid return type for special function '%s' ('void' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
        SWAG_VERIFY(!parameters->childs[1]->typeInfo->isSame(typeStruct, ISSAME_CAST | ISSAME_EXACT), context->report({parameters->childs[1], format("invalid parameter '2' for special function '%s' (cannot be of type '%s')", name.c_str(), typeStruct->name.c_str())}));
    }
    else if (name == "opIndex")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() >= 2, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType->typeInfo != g_TypeMgr.typeInfoVoid, context->report({node, node->token, format("missing return type for special function '%s'", name.c_str())}));
        for (int i = 1; i < parameters->childs.size(); i++)
            SWAG_VERIFY(parameters->childs[i]->typeInfo->isSame(g_TypeMgr.typeInfoU32, ISSAME_CAST), context->report({parameters->childs[i], format("invalid parameter '%d' for special function '%s' ('u32' expected, '%s' provided)", i + 1, name.c_str(), parameters->childs[i]->typeInfo->name.c_str())}));
    }
    else if (name == "opSlice")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() == 3, context->report({ node, node->token, format("invalid number of arguments for special function '%s'", name.c_str()) }));
        SWAG_VERIFY(returnType->typeInfo != g_TypeMgr.typeInfoVoid, context->report({ node, node->token, format("missing return type for special function '%s'", name.c_str()) }));
        SWAG_VERIFY(parameters->childs[1]->typeInfo->isSame(g_TypeMgr.typeInfoU32, ISSAME_CAST), context->report({ parameters->childs[1], format("invalid parameter '2' for special function '%s' ('u32' expected, '%s' provided)", name.c_str(), parameters->childs[1]->typeInfo->name.c_str()) }));
        SWAG_VERIFY(parameters->childs[2]->typeInfo->isSame(g_TypeMgr.typeInfoU32, ISSAME_CAST), context->report({ parameters->childs[2], format("invalid parameter '3' for special function '%s' ('u32' expected, '%s' provided)", name.c_str(), parameters->childs[2]->typeInfo->name.c_str()) }));
    }
    else if (name == "opIndexAssign")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() >= 3, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->report({returnType, format("invalid return type for special function '%s' ('void' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
        for (int i = 2; i < parameters->childs.size(); i++)
            SWAG_VERIFY(parameters->childs[i]->typeInfo->isSame(g_TypeMgr.typeInfoU32, ISSAME_CAST), context->report({parameters->childs[i], format("invalid parameter '%d' for special function '%s' ('u32' expected, '%s' provided)", i + 1, name.c_str(), parameters->childs[i]->typeInfo->name.c_str())}));
    }
    else if (name == "opIndexAffect")
    {
        SWAG_VERIFY(parameters && parameters->childs.size() >= 3, context->report({node, node->token, format("invalid number of arguments for special function '%s'", name.c_str())}));
        SWAG_VERIFY(returnType->typeInfo == g_TypeMgr.typeInfoVoid, context->report({returnType, format("invalid return type for special function '%s' ('void' expected, '%s' provided)", name.c_str(), returnType->typeInfo->name.c_str())}));
    }
    else
    {
        return context->report({node, node->token, format("function '%s' does not match a special function/operator overload", name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveUserOp(SemanticContext* context, const char* name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right, bool optionnal)
{
    VectorNative<AstNode*> params;
    SWAG_ASSERT(left);
    params.push_back(left);
    if (right)
        params.push_back(right);
    return resolveUserOp(context, name, opConst, opType, left, params, optionnal);
}

SymbolName* SemanticJob::hasUserOp(const char* name, TypeInfoStruct* leftStruct)
{
    // In case of a generic instance, symbols are defined in the original generic structure scope, not
    // in the instance
    if (leftStruct->fromGeneric)
        leftStruct = leftStruct->fromGeneric;
    return leftStruct->scope->symTable.find(name);
}

SymbolName* SemanticJob::hasUserOp(SemanticContext* context, const char* name, AstNode* left)
{
    auto leftType   = TypeManager::concreteReferenceType(left->typeInfo);
    auto leftStruct = CastTypeInfo<TypeInfoStruct>(leftType, TypeInfoKind::Struct);
    return hasUserOp(name, leftStruct);
}

SymbolName* SemanticJob::waitUserOp(SemanticContext* context, const char* name, AstNode* left)
{
    auto symbol = hasUserOp(context, name, left);
    if (!symbol)
        return nullptr;

    scoped_lock lkn(symbol->mutex);
    if (symbol->cptOverloads)
        context->job->waitForSymbolNoLock(symbol);

    return symbol;
}

bool SemanticJob::resolveUserOp(SemanticContext* context, const char* name, const char* opConst, TypeInfo* opType, AstNode* left, VectorNative<AstNode*>& params, bool optionnal)
{
    auto symbol = waitUserOp(context, name, left);

    if (!symbol)
    {
        if (optionnal)
            return true;

        auto leftType = TypeManager::concreteType(left->typeInfo);
        return context->report({left->parent, format("cannot find special function '%s' in '%s'", name, leftType->name.c_str())});
    }

    if (context->result != ContextResult::Done)
        return true;

    auto node = context->node;
    auto job  = context->job;

    SymbolMatchContext symMatchContext;
    symMatchContext.flags |= SymbolMatchContext::MATCH_UNCONST; // Do not test const
    for (auto param : params)
        symMatchContext.parameters.push_back(param);

    // Generic string parameter
    AstNode* genericParameters = nullptr;
    AstNode  parameters;
    AstNode  literal;
    parameters.sourceFile = left->sourceFile;
    parameters.inheritTokenLocation(left->token);
    parameters.inheritOwners(left);
    literal.sourceFile = left->sourceFile;
    literal.inheritTokenLocation(left->token);
    literal.inheritOwners(left);

    if (opConst || opType)
    {
        literal.kind               = AstNodeKind::Literal;
        literal.computedValue.text = opConst ? opConst : "";
        literal.typeInfo           = opType ? opType : g_TypeMgr.typeInfoString;
        symMatchContext.genericParameters.push_back(&literal);
        parameters.kind   = AstNodeKind::FuncDeclGenericParams;
        genericParameters = &parameters;
        Ast::addChildBack(&parameters, &literal);
    }

    while (true)
    {
        vector<OneTryMatch> listTryMatch;

        {
            unique_lock lk(symbol->mutex);
            for (auto overload : symbol->overloads)
            {
                OneTryMatch t;
                t.symMatchContext   = symMatchContext;
                t.overload          = overload;
                t.genericParameters = genericParameters;
                t.callParameters    = left->parent;
                t.dependentVar      = nullptr;
                t.cptOverloads      = (uint32_t) symbol->overloads.size();
                listTryMatch.push_back(t);
            }
        }

        SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, nullptr));
        if (context->result == ContextResult::Pending)
            return true;
        if (context->result != ContextResult::NewChilds)
            break;
        context->result = ContextResult::Done;
    }

    // Make the real cast for all the call parameters
    auto& oneMatch = job->cacheMatches[0];
    for (int i = 0; i < params.size(); i++)
    {
        if (i < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[i])
            SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters[i]->typeInfo, nullptr, params[i], CASTFLAG_UNCONST));
    }

    auto overload = oneMatch.symbolOverload;
    if (!optionnal)
    {
        node->typeInfo                     = overload->typeInfo;
        node->resolvedUserOpSymbolName     = symbol;
        node->resolvedUserOpSymbolOverload = overload;
        SWAG_ASSERT(node->resolvedUserOpSymbolName && node->resolvedUserOpSymbolName->kind == SymbolKind::Function);
    }

    // Allocate room on the stack to store the result of the function call
    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
    if (typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY)
    {
        node->fctCallStorageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += typeFunc->returnType->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    return true;
}
