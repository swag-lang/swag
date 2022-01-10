#include "pch.h"
#include "TypeManager.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "SourceFile.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Mutex.h"

bool SemanticJob::checkFuncPrototype(SemanticContext* context, AstFuncDecl* node)
{
    if (node->flags & AST_GENERATED)
        return true;
    SWAG_CHECK(checkFuncPrototypeOp(context, node));
    return true;
}

Utf8 SemanticJob::getSpecialOpSignature(AstFuncDecl* node)
{
    Utf8 result = "the signature should be ";

    if (node->token.text == g_LangSpec->name_opDrop)
        result += "`func opDrop(self)`";
    else if (node->token.text == g_LangSpec->name_opPostCopy)
        result += "`func opPostCopy(self)`";
    else if (node->token.text == g_LangSpec->name_opPostMove)
        result += "`func opPostMove(self)`";
    else if (node->token.text == g_LangSpec->name_opSlice)
        result += "`func opSlice(self, low, up: uint) -> <string or slice>`";
    else if (node->token.text == g_LangSpec->name_opIndex)
        result += "`func opIndex(self, index: uint) -> WhateverType`";
    else if (node->token.text == g_LangSpec->name_opCount)
        result += "`func opCount(self) -> uint`";
    else if (node->token.text == g_LangSpec->name_opData)
        result += "`func opData(self) -> *WhateverType`";
    else if (node->token.text == g_LangSpec->name_opCast)
        result += "`func opCast(self) -> WhateverType`";
    else if (node->token.text == g_LangSpec->name_opEquals)
        result += "`func opEquals(self, value: WhateverType) -> bool`";
    else if (node->token.text == g_LangSpec->name_opCmp)
        result += "`func opCmp(self, value: WhateverType) -> s32`";
    else if (node->token.text == g_LangSpec->name_opAffect)
        result += "`func opAffect(self, value: WhateverType)`";
    else if (node->token.text == g_LangSpec->name_opAffectSuffix)
        result += "`func(suffix: string) opAffectSuffix(self, value: WhateverType)`";
    else if (node->token.text == g_LangSpec->name_opIndexAffect)
        result += "`func opIndexAffect(self, index: uint, value: WhateverType)`";
    else if (node->token.text == g_LangSpec->name_opBinary)
        result += "`func(op: string) opBinary(self, other: WhateverType) -> Self`";
    else if (node->token.text == g_LangSpec->name_opUnary)
        result += "`func(op: string) opUnary(self) -> Self`";
    else if (node->token.text == g_LangSpec->name_opAssign)
        result += "`func(op: string) opAssign(self, value: WhateverType)`";
    else if (node->token.text == g_LangSpec->name_opIndexAssign)
        result += "`func(op: string) opIndexAssign(self, index: uint, value: WhateverType)`";
    else if (node->token.text.find(g_LangSpec->name_opVisit) == 0)
        result += "`func(ptr: bool) opVisit(self, stmt: code)`";
    else
        result = "";

    return result;
}

bool SemanticJob::checkFuncPrototypeOpNumParams(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t numWanted, bool exact)
{
    auto numCur = parameters->childs.size();
    if (exact && (numCur != numWanted))
    {
        if (numCur > numWanted)
            return context->report({parameters->childs[numWanted], Fmt(Err(Err0043), node->token.ctext(), numWanted, numCur)});
        return context->report({parameters, Fmt(Err(Err0061), node->token.ctext(), numWanted, numCur)});
    }

    if (!exact && (numCur < numWanted))
    {
        return context->report({parameters, Fmt(Err(Err0062), node->token.ctext(), numWanted, numCur)});
    }

    return true;
}

bool SemanticJob::checkFuncPrototypeOpReturnType(SemanticContext* context, AstFuncDecl* node, TypeInfo* wanted)
{
    auto returnType = TypeManager::concreteType(node->returnType->typeInfo, CONCRETE_ALIAS);

    // Whatever except nothing
    if (wanted == nullptr)
    {
        if (returnType == g_TypeMgr->typeInfoVoid)
            return context->report({node, Fmt(Err(Err0063), node->token.ctext())});
        return true;
    }

    if (wanted != g_TypeMgr->typeInfoVoid && returnType == g_TypeMgr->typeInfoVoid)
        return context->report({node, Fmt(Err(Err0064), node->token.ctext(), wanted->getDisplayNameC())});

    if (!returnType->isSame(wanted, ISSAME_CAST))
        return context->report({node->returnType->childs.front(), Fmt(Err(Err0065), node->token.ctext(), wanted->name.c_str(), returnType->getDisplayNameC())});

    return true;
}

bool SemanticJob::checkFuncPrototypeOpParam(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t index, TypeInfo* wanted)
{
    auto typeParam = TypeManager::concreteType(parameters->childs[index]->typeInfo, CONCRETE_ALIAS);
    if (!typeParam->isSame(wanted, ISSAME_CAST))
        return context->report({parameters->childs[index], Fmt(Err(Err0066), getTheNiceParameterRank(index + 1).c_str(), node->token.ctext(), wanted->getDisplayNameC(), typeParam->getDisplayNameC())});
    return true;
}

bool SemanticJob::checkFuncPrototypeOp(SemanticContext* context, AstFuncDecl* node)
{
    if (!node->isSpecialFunctionName())
        return true;

    auto& name      = node->token.text;
    bool  isOpVisit = name.find(g_LangSpec->name_opVisit) == 0;
    auto  parent    = node->findParent(AstNodeKind::Impl);

    // Special function outside an impl block
    if (!parent)
    {
        Diagnostic note{Hlp(Hlp0015), DiagnosticLevel::Help};
        Diagnostic diag{node, node->tokenName, Fmt(Err(Err0067), node->token.ctext())};
        return context->report(diag, &note);
    }

    PushErrContext ec(context, nullptr, getSpecialOpSignature(node), DiagnosticLevel::Help);

    auto      parameters = node->parameters;
    TypeInfo* typeStruct = nullptr;
    if (parent)
    {
        auto implNode = CastAst<AstImpl>(parent, AstNodeKind::Impl);

        // No need to raise an error, the semantic pass on the impl node will fail
        typeStruct = implNode->identifier->typeInfo;
        if (typeStruct->kind != TypeInfoKind::Struct)
            return true;

        // First parameter must be be struct
        SWAG_VERIFY(node->parameters, context->report({node, Fmt(Err(Err0068), name.c_str())}));
        auto firstType = node->parameters->childs.front()->typeInfo;
        SWAG_VERIFY(firstType->kind == TypeInfoKind::Pointer, context->report({node->parameters->childs.front(), Fmt(Err(Err0069), name.c_str(), typeStruct->getDisplayNameC(), firstType->getDisplayNameC())}));
        auto firstTypePtr = CastTypeInfo<TypeInfoPointer>(firstType, firstType->kind);
        SWAG_VERIFY(firstTypePtr->pointedType->isSame(typeStruct, ISSAME_CAST), context->report({node->parameters->childs.front(), Fmt(Err(Err0069), name.c_str(), typeStruct->getDisplayNameC(), firstType->getDisplayNameC())}));
    }

    // Generic operator must have one generic parameter of type string
    if (name == g_LangSpec->name_opBinary ||
        name == g_LangSpec->name_opUnary ||
        name == g_LangSpec->name_opAssign ||
        name == g_LangSpec->name_opIndexAssign ||
        name == g_LangSpec->name_opAffectSuffix)
    {
        SWAG_VERIFY(node->genericParameters && node->genericParameters->childs.size() <= 2, context->report({node, Fmt(Err(Err0071), name.c_str())}));
        auto firstGen = node->genericParameters->childs.front();
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr->typeInfoString, ISSAME_CAST), context->report({firstGen, Fmt(Err(Err0072), name.c_str(), firstGen->typeInfo->getDisplayNameC())}));
    }
    else if (isOpVisit)
    {
        SWAG_VERIFY(node->genericParameters && node->genericParameters->childs.size() <= 2, context->report({node, Fmt(Err(Err0073), name.c_str())}));
        auto firstGen = node->genericParameters->childs.front();
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr->typeInfoBool, ISSAME_CAST), context->report({firstGen, Fmt(Err(Err0074), name.c_str(), firstGen->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(node->attributeFlags & ATTRIBUTE_MACRO, context->report(node, Err(Err0075)));
    }
    else if (name == g_LangSpec->name_opCast)
    {
        SWAG_VERIFY(!node->genericParameters, context->report({node, Fmt(Err(Err0478), name.c_str())}));
    }
    else
    {
        SWAG_VERIFY(!node->genericParameters || node->genericParameters->childs.size() == 1, context->report({node, Fmt(Err(Err0450), name.c_str())}));
    }

    // Check each function
    if (isOpVisit)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, 1, g_TypeMgr->typeInfoCode));
    }
    else if (name == g_LangSpec->name_opCast)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 1));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
    }
    else if (name == g_LangSpec->name_opEquals)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoBool));
    }
    else if (name == g_LangSpec->name_opCmp)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoS32));
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
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
    }
    else if (name == g_LangSpec->name_opCount)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 1));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opData)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 1));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoConstPointers[(int) NativeTypeKind::Void]));
    }
    else if (name == g_LangSpec->name_opAssign)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
    }
    else if (name == g_LangSpec->name_opAffect)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        SWAG_VERIFY(!parameters->childs[1]->typeInfo->isSame(typeStruct, ISSAME_CAST | ISSAME_EXACT), context->report({parameters->childs[1], Fmt(Err(Err0077), name.c_str(), typeStruct->name.c_str())}));
    }
    else if (name == g_LangSpec->name_opAffectSuffix)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        SWAG_VERIFY(!parameters->childs[1]->typeInfo->isSame(typeStruct, ISSAME_CAST | ISSAME_EXACT), context->report({parameters->childs[1], Fmt(Err(Err0077), name.c_str(), typeStruct->name.c_str())}));
    }
    else if (name == g_LangSpec->name_opSlice)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
        auto returnType = TypeManager::concreteType(node->returnType->typeInfo, CONCRETE_ALIAS);
        if (!returnType->isNative(NativeTypeKind::String) && returnType->kind != TypeInfoKind::Slice)
            return context->report({node, Fmt(Err(Err0126), node->returnType->typeInfo->getDisplayNameC())});
        SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, 1, g_TypeMgr->typeInfoUInt));
        SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, 2, g_TypeMgr->typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opIndex)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
        for (int i = 1; i < parameters->childs.size(); i++)
            SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, i, g_TypeMgr->typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opIndexAssign)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        for (int i = 1; i < parameters->childs.size() - 1; i++)
            SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, i, g_TypeMgr->typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opIndexAffect)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        for (int i = 1; i < parameters->childs.size() - 1; i++)
            SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, i, g_TypeMgr->typeInfoUInt));
    }
    else if (name == g_LangSpec->name_opInit && node->sourceFile->isGenerated)
    {
    }
    else
    {
        vector<Utf8> searchList;
        vector<Utf8> best;

        searchList.push_back(g_LangSpec->name_opBinary);
        searchList.push_back(g_LangSpec->name_opUnary);
        searchList.push_back(g_LangSpec->name_opAssign);
        searchList.push_back(g_LangSpec->name_opIndexAssign);
        searchList.push_back(g_LangSpec->name_opCast);
        searchList.push_back(g_LangSpec->name_opEquals);
        searchList.push_back(g_LangSpec->name_opCmp);
        searchList.push_back(g_LangSpec->name_opPostCopy);
        searchList.push_back(g_LangSpec->name_opPostMove);
        searchList.push_back(g_LangSpec->name_opDrop);
        searchList.push_back(g_LangSpec->name_opCount);
        searchList.push_back(g_LangSpec->name_opData);
        searchList.push_back(g_LangSpec->name_opAffect);
        searchList.push_back(g_LangSpec->name_opAffectSuffix);
        searchList.push_back(g_LangSpec->name_opSlice);
        searchList.push_back(g_LangSpec->name_opIndex);
        searchList.push_back(g_LangSpec->name_opIndexAffect);
        searchList.push_back(g_LangSpec->name_opVisit);

        findClosestMatches(context, node->tokenName.text, searchList, best);
        Utf8 appendMsg = findClosestMatchesMsg(context, best);

        Diagnostic diag{node, node->tokenName, Fmt(Err(Err0078), name.c_str())};
        Diagnostic note{Hlp(Hlp0014), DiagnosticLevel::Help};
        return context->report(appendMsg, diag, &note);
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
        return context->report({left->parent, Fmt(Err(Err0079), name.c_str(), leftType->getDisplayNameC())});
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
    parameters.inheritTokenLocation(left);
    parameters.inheritOwners(left);
    literal.flags      = 0;
    literal.sourceFile = left->sourceFile;
    literal.inheritTokenLocation(left);
    literal.inheritOwners(left);
    literal.computedValue = &cValue;

    if (opConst || opType)
    {
        literal.kind                = AstNodeKind::Literal;
        literal.computedValue->text = opConst ? opConst : "";
        literal.typeInfo            = opType ? opType : g_TypeMgr->typeInfoString;
        literal.flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
        symMatchContext.genericParameters.push_back(&literal);
        parameters.kind   = AstNodeKind::FuncDeclParams;
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

        SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, nullptr, justCheck ? MIP_JUST_CHECK : 0));
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
