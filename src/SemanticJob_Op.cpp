#include "pch.h"
#include "TypeManager.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SourceFile.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "ScopedLock.h"

bool SemanticJob::checkFuncPrototype(SemanticContext* context, AstFuncDecl* node)
{
    if (node->flags & AST_GENERATED)
        return true;
    SWAG_CHECK(checkFuncPrototypeOp(context, node));
    return true;
}

bool SemanticJob::checkFuncPrototypeOpNumParams(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t numWanted, bool exact)
{
    auto numCur = parameters->childs.size();
    if (exact && (numCur != numWanted))
        return context->report({parameters, Utf8::format(Msg0061, node->token.text.c_str(), numWanted, numCur)});
    if (!exact && (numCur < numWanted))
        return context->report({parameters, Utf8::format(Msg0062, node->token.text.c_str(), numWanted, numCur)});
    return true;
}

bool SemanticJob::checkFuncPrototypeOpReturnType(SemanticContext* context, AstFuncDecl* node, TypeInfo* wanted)
{
    auto returnType = TypeManager::concreteType(node->returnType->typeInfo, CONCRETE_ALIAS);

    // Whatever except nothing
    if (wanted == nullptr)
    {
        if (returnType == g_TypeMgr.typeInfoVoid)
            return context->report({node, node->token, Utf8::format(Msg0063, node->token.text.c_str())});
        return true;
    }

    if (wanted != g_TypeMgr.typeInfoVoid && returnType == g_TypeMgr.typeInfoVoid)
        return context->report({node, node->token, Utf8::format(Msg0064, node->token.text.c_str(), wanted->getDisplayName().c_str())});
    if (!returnType->isSame(wanted, ISSAME_CAST))
        return context->report({node->returnType, Utf8::format(Msg0065, node->token.text.c_str(), wanted->name.c_str(), returnType->getDisplayName().c_str())});
    return true;
}

bool SemanticJob::checkFuncPrototypeOpParam(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t index, TypeInfo* wanted)
{
    auto typeParam = TypeManager::concreteType(parameters->childs[index]->typeInfo, CONCRETE_ALIAS);
    if (!typeParam->isSame(wanted, ISSAME_CAST))
        return context->report({parameters->childs[index], Utf8::format(Msg0066, index + 1, node->token.text.c_str(), wanted->getDisplayName().c_str(), typeParam->getDisplayName().c_str())});
    return true;
}

bool SemanticJob::checkFuncPrototypeOp(SemanticContext* context, AstFuncDecl* node)
{
    if (!node->isSpecialFunctionName())
        return true;

    auto& name      = node->token.text;
    bool  isOpVisit = name.find("opVisit") == 0;

    auto parent = node->parent;
    while (parent && parent->kind != AstNodeKind::Impl)
        parent = parent->parent;

    auto parameters = node->parameters;

    // Special function outside an impl block. This is valid from some...
    if (!parent)
        return context->report({node, node->token, Utf8::format(Msg0067, name.c_str())});

    TypeInfo* typeStruct = nullptr;
    if (parent)
    {
        auto implNode = CastAst<AstImpl>(parent, AstNodeKind::Impl);

        // No need to raise an error, the semantic pass on the impl node will fail
        typeStruct = implNode->identifier->typeInfo;
        if (typeStruct->kind != TypeInfoKind::Struct)
            return true;

        // First parameter must be be struct
        SWAG_VERIFY(node->parameters, context->report({node, node->token, Utf8::format(Msg0068, name.c_str())}));
        auto firstType = node->parameters->childs.front()->typeInfo;
        SWAG_VERIFY(firstType->kind == TypeInfoKind::Pointer, context->report({node->parameters->childs.front(), Utf8::format(Msg0069, name.c_str(), typeStruct->getDisplayName().c_str(), firstType->getDisplayName().c_str())}));
        auto firstTypePtr = CastTypeInfo<TypeInfoPointer>(firstType, firstType->kind);
        SWAG_VERIFY(firstTypePtr->pointedType->isSame(typeStruct, ISSAME_CAST), context->report({node->parameters->childs.front(), Utf8::format(Msg0069, name.c_str(), typeStruct->getDisplayName().c_str(), firstType->getDisplayName().c_str())}));
    }

    // Generic operator must have one generic parameter of type string
    if (name == g_LangSpec->name_opBinary || name == g_LangSpec->name_opUnary || name == g_LangSpec->name_opAssign || name == g_LangSpec->name_opIndexAssign)
    {
        SWAG_VERIFY(node->genericParameters && node->genericParameters->childs.size() == 1, context->report({node, node->token, Utf8::format(Msg0071, name.c_str())}));
        auto firstGen = node->genericParameters->childs.front();
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr.typeInfoString, ISSAME_CAST), context->report({firstGen, Utf8::format(Msg0072, name.c_str(), firstGen->typeInfo->getDisplayName().c_str())}));
    }
    else if (isOpVisit)
    {
        SWAG_VERIFY(node->genericParameters && node->genericParameters->childs.size() == 1, context->report({node, node->token, Utf8::format(Msg0073, name.c_str())}));
        auto firstGen = node->genericParameters->childs.front();
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr.typeInfoBool, ISSAME_CAST), context->report({firstGen, Utf8::format(Msg0074, name.c_str(), firstGen->typeInfo->getDisplayName().c_str())}));
        SWAG_VERIFY(node->attributeFlags & ATTRIBUTE_MACRO, context->report({node, node->token, Msg0075}));
    }

    // Check each function
    if (isOpVisit)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoVoid));
        SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, 1, g_TypeMgr.typeInfoCode));
    }
    else if (name == g_LangSpec->name_opCast)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 1));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
    }
    else if (name == g_LangSpec->name_opEquals)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoBool));
    }
    else if (name == g_LangSpec->name_opCmp)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoS32));
    }
    else if (name == g_LangSpec->name_opBinary)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, typeStruct));
    }
    else if (name == g_LangSpec->name_opUnary)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 1));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, typeStruct));
    }
    else if (name == g_LangSpec->name_opPostCopy || name == g_LangSpec->name_opPostMove || name == g_LangSpec->name_opDrop)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 1));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoVoid));
    }
    else if (name == g_LangSpec->name_opCount)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 1));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opData)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 1));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoConstPVoid));
    }
    else if (name == g_LangSpec->name_opAssign)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoVoid));
    }
    else if (name == g_LangSpec->name_opAffect)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoVoid));
        SWAG_VERIFY(!parameters->childs[1]->typeInfo->isSame(typeStruct, ISSAME_CAST | ISSAME_EXACT), context->report({parameters->childs[1], Utf8::format(Msg0077, name.c_str(), typeStruct->name.c_str())}));
    }
    else if (name == g_LangSpec->name_opSlice)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
        SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, 1, g_TypeMgr.typeInfoUInt));
        SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, 2, g_TypeMgr.typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opIndex)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
        for (int i = 1; i < parameters->childs.size(); i++)
            SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, i, g_TypeMgr.typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opIndexAssign)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoVoid));
        for (int i = 1; i < parameters->childs.size() - 1; i++)
            SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, i, g_TypeMgr.typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opIndexAffect)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr.typeInfoVoid));
        for (int i = 1; i < parameters->childs.size() - 1; i++)
            SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, i, g_TypeMgr.typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opInit && node->sourceFile->isGenerated)
    {
    }
    else
    {
        return context->report({node, node->token, Utf8::format(Msg0078, name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveUserOpCommutative(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

    // Simple case
    if (leftTypeInfo->kind == TypeInfoKind::Struct && rightTypeInfo->kind != TypeInfoKind::Struct)
        return resolveUserOp(context, name, opConst, opType, left, right, false);

    bool okLeft  = false;
    bool okRight = false;
    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        okLeft = resolveUserOp(context, name, opConst, opType, left, right, true);
        if (context->result != ContextResult::Done)
            return true;
    }

    if (rightTypeInfo->kind == TypeInfoKind::Struct)
    {
        okRight = resolveUserOp(context, name, opConst, opType, right, left, true);
        if (context->result != ContextResult::Done)
            return true;
    }

    if (okLeft || (!okRight && leftTypeInfo->kind == TypeInfoKind::Struct))
        return resolveUserOp(context, name, opConst, opType, left, right, false);

    node->semFlags |= AST_SEM_INVERSE_PARAMS;
    return resolveUserOp(context, name, opConst, opType, right, left, false);
}

SymbolName* SemanticJob::hasUserOp(const Utf8& name, TypeInfoStruct* leftStruct)
{
    // In case of a generic instance, symbols are defined in the original generic structure scope, not
    // in the instance
    if (leftStruct->fromGeneric)
        leftStruct = leftStruct->fromGeneric;
    return leftStruct->scope->symTable.find(name);
}

SymbolName* SemanticJob::hasUserOp(SemanticContext* context, const Utf8& name, AstNode* left)
{
    auto leftType = TypeManager::concreteReferenceType(left->typeInfo);
    if (leftType->kind == TypeInfoKind::Array)
        leftType = CastTypeInfo<TypeInfoArray>(leftType, TypeInfoKind::Array)->finalType;
    auto leftStruct = CastTypeInfo<TypeInfoStruct>(leftType, TypeInfoKind::Struct);
    return hasUserOp(name, leftStruct);
}

SymbolName* SemanticJob::waitUserOp(SemanticContext* context, const Utf8& name, AstNode* left)
{
    auto symbol = hasUserOp(context, name, left);
    if (!symbol)
        return nullptr;

    ScopedLock lkn(symbol->mutex);
    if (symbol->cptOverloads)
        context->job->waitSymbolNoLock(symbol);

    return symbol;
}

bool SemanticJob::resolveUserOp(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right, bool justCheck)
{
    VectorNative<AstNode*> params;
    SWAG_ASSERT(left);
    params.push_back(left);
    if (right)
        params.push_back(right);
    return resolveUserOp(context, name, opConst, opType, left, params, justCheck);
}

bool SemanticJob::resolveUserOp(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, VectorNative<AstNode*>& params, bool justCheck)
{
    auto symbol = waitUserOp(context, name, left);

    if (!symbol)
    {
        if (justCheck)
            return false;

        auto leftType = TypeManager::concreteType(left->typeInfo);
        return context->report({left->parent, Utf8::format(Msg0079, name.c_str(), leftType->getDisplayName().c_str())});
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
    AstNode*      genericParameters = nullptr;
    AstNode       parameters;
    AstNode       literal;
    ComputedValue cValue;
    parameters.flags      = 0;
    parameters.sourceFile = left->sourceFile;
    parameters.inheritTokenLocation(left->token);
    parameters.inheritOwners(left);
    literal.flags      = 0;
    literal.sourceFile = left->sourceFile;
    literal.inheritTokenLocation(left->token);
    literal.inheritOwners(left);
    literal.computedValue = &cValue;

    if (opConst || opType)
    {
        literal.kind                = AstNodeKind::Literal;
        literal.computedValue->text = opConst ? opConst : "";
        literal.typeInfo            = opType ? opType : g_TypeMgr.typeInfoString;
        literal.flags |= AST_VALUE_COMPUTED;
        symMatchContext.genericParameters.push_back(&literal);
        parameters.kind   = AstNodeKind::FuncDeclGenericParams;
        genericParameters = &parameters;
        Ast::addChildBack(&parameters, &literal);
    }

    auto& listTryMatch = job->cacheListTryMatch;
    while (true)
    {
        job->clearTryMatch();

        {
            SharedLock lk(symbol->mutex);
            for (auto overload : symbol->overloads)
            {
                auto t               = job->getTryMatch();
                t->symMatchContext   = symMatchContext;
                t->overload          = overload;
                t->genericParameters = genericParameters;
                t->callParameters    = left->parent;
                t->dependentVar      = nullptr;
                t->cptOverloads      = (uint32_t) symbol->overloads.size();
                t->cptOverloadsInit  = symbol->cptOverloadsInit;
                listTryMatch.push_back(t);
            }
        }

        SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, nullptr, justCheck));
        if (context->result == ContextResult::Pending)
            return true;
        if (context->result != ContextResult::NewChilds)
            break;
        context->result = ContextResult::Done;
    }

    uint32_t castFlags = CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST;
    if (justCheck)
        castFlags |= CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR;
    if (job->cacheMatches.empty())
        return true;

    auto oneMatch = job->cacheMatches[0];
    for (int i = 0; i < params.size(); i++)
    {
        if (i < oneMatch->solvedParameters.size() && oneMatch->solvedParameters[i])
            SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch->solvedParameters[i]->typeInfo, nullptr, params[i], castFlags));
    }

    // Make the real cast for all the call parameters
    if (!justCheck)
    {
        auto overload  = oneMatch->symbolOverload;
        node->typeInfo = overload->typeInfo;
        node->allocateExtension();
        node->extension->resolvedUserOpSymbolOverload = overload;
        SWAG_ASSERT(symbol && symbol->kind == SymbolKind::Function);

        // Allocate room on the stack to store the result of the function call
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
        if (typeFunc->returnType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            node->allocateComputedValue();
            node->computedValue->storageOffset = node->ownerScope->startStackSize;
            node->ownerScope->startStackSize += typeFunc->returnType->sizeOf;
            node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
        }
    }

    return true;
}
