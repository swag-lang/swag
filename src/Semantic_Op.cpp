#include "pch.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

bool Semantic::checkFuncPrototype(SemanticContext* context, AstFuncDecl* node)
{
    if (node->flags & AST_GENERATED)
        return true;
    SWAG_CHECK(checkFuncPrototypeOp(context, node));
    return true;
}

Utf8 Semantic::getSpecialOpSignature(AstFuncDecl* node)
{
    Utf8 result = "the signature should be ";

    if (node->token.text == g_LangSpec->name_opDrop)
        result += "'func opDrop(self)'";
    else if (node->token.text == g_LangSpec->name_opPostCopy)
        result += "'func opPostCopy(self)'";
    else if (node->token.text == g_LangSpec->name_opPostMove)
        result += "'func opPostMove(self)'";
    else if (node->token.text == g_LangSpec->name_opSlice)
        result += "'func opSlice(self, low, up: u64) -> <string or slice>'";
    else if (node->token.text == g_LangSpec->name_opIndex)
        result += "'func opIndex(self, index: u64) -> WhateverType'";
    else if (node->token.text == g_LangSpec->name_opCount)
        result += "'func opCount(self) -> u64'";
    else if (node->token.text == g_LangSpec->name_opData)
        result += "'func opData(self) -> *WhateverType'";
    else if (node->token.text == g_LangSpec->name_opCast)
        result += "'func opCast(self) -> WhateverType'";
    else if (node->token.text == g_LangSpec->name_opEquals)
        result += "'func opEquals(self, value: WhateverType) -> bool'";
    else if (node->token.text == g_LangSpec->name_opCmp)
        result += "'func opCmp(self, value: WhateverType) -> s32'";
    else if (node->token.text == g_LangSpec->name_opAffect)
        result += "'func opAffect(self, value: WhateverType)'";
    else if (node->token.text == g_LangSpec->name_opAffectSuffix)
        result += "'func(suffix: string) opAffectSuffix(self, value: WhateverType)'";
    else if (node->token.text == g_LangSpec->name_opIndexAffect)
        result += "'func opIndexAffect(self, index: u64, value: WhateverType)'";
    else if (node->token.text == g_LangSpec->name_opBinary)
        result += "'func(op: string) opBinary(self, other: WhateverType) -> Self'";
    else if (node->token.text == g_LangSpec->name_opUnary)
        result += "'func(op: string) opUnary(self) -> Self'";
    else if (node->token.text == g_LangSpec->name_opAssign)
        result += "'func(op: string) opAssign(self, value: WhateverType)'";
    else if (node->token.text == g_LangSpec->name_opIndexAssign)
        result += "'func(op: string) opIndexAssign(self, index: u64, value: WhateverType)'";
    else if (node->token.text.startsWith(g_LangSpec->name_opVisit))
        result += "'func(ptr: bool, back: bool) opVisit(self, stmt: code)'";
    else
        result = "";

    return result;
}

bool Semantic::checkFuncPrototypeOpNumParams(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t numWanted, bool exact)
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

bool Semantic::checkFuncPrototypeOpReturnType(SemanticContext* context, AstFuncDecl* node, TypeInfo* wanted)
{
    auto returnType = node->returnType->typeInfo->getConcreteAlias();

    // Whatever except nothing
    if (wanted == nullptr)
    {
        if (returnType->isVoid())
            return context->report({node, node->tokenName, Fmt(Err(Err0063), node->token.ctext())});
        return true;
    }

    if (!wanted->isVoid() && returnType->isVoid())
        return context->report({node, node->tokenName, Fmt(Err(Err0064), node->token.ctext(), wanted->getDisplayNameC())});

    if (!returnType->isSame(wanted, CASTFLAG_CAST))
    {
        if (node->returnType->childs.empty())
            return context->report({node->returnType, Fmt(Err(Err0065), node->token.ctext(), wanted->name.c_str(), returnType->getDisplayNameC())});
        return context->report({node->returnType->childs.front(), Fmt(Err(Err0065), node->token.ctext(), wanted->name.c_str(), returnType->getDisplayNameC())});
    }

    return true;
}

bool Semantic::checkFuncPrototypeOpParam(SemanticContext* context, AstFuncDecl* node, AstNode* parameters, uint32_t index, TypeInfo* wanted)
{
    auto typeParam = parameters->childs[index]->typeInfo->getConcreteAlias();
    if (!typeParam->isSame(wanted, CASTFLAG_CAST))
        return context->report({parameters->childs[index], Fmt(Err(Err0053), wanted->getDisplayNameC(), typeParam->getDisplayNameC())});
    return true;
}

bool Semantic::checkFuncPrototypeOp(SemanticContext* context, AstFuncDecl* node)
{
    if (!node->isSpecialFunctionName())
        return true;

    auto& name      = node->token.text;
    bool  isOpVisit = name.find(g_LangSpec->name_opVisit) == 0;
    auto  parent    = node->findParent(AstNodeKind::Impl);

    // opVisit variant should be a name
    if (isOpVisit && name != g_LangSpec->name_opVisit && !SWAG_IS_ALPHA(name[7]))
    {
        auto start = node->tokenName.startLocation;
        start.column += 7;
        Diagnostic diag{node->sourceFile, start, node->tokenName.endLocation, Fmt(Err(Err1214), node->token.ctext() + 7)};
        return context->report(diag);
    }

    // Special function outside an impl block
    if (!parent)
    {
        Diagnostic diag{node, node->tokenName, Fmt(Err(Err0067), node->token.ctext())};
        return context->report(diag);
    }

    if (node->ownerScope->kind == ScopeKind::Impl)
    {
        Diagnostic diag{node, node->tokenName, Fmt(Err(Err0494), node->token.ctext())};
        return context->report(diag);
    }

    if ((node->ownerScope->owner->attributeFlags & ATTRIBUTE_PUBLIC) && !(node->attributeFlags & ATTRIBUTE_PUBLIC))
    {
        Diagnostic diag{node, node->tokenName, Fmt(Err(Err0503), node->token.ctext())};
        auto       note = Diagnostic::note(Nte(Nte0126));
        return context->report(diag, note);
    }

    if (!(node->ownerScope->owner->attributeFlags & ATTRIBUTE_PUBLIC) && (node->attributeFlags & ATTRIBUTE_PUBLIC))
    {
        Diagnostic diag{node, node->tokenName, Fmt(Err(Err0506), node->token.ctext())};
        auto       note = Diagnostic::note(Nte(Nte0127));
        return context->report(diag, note);
    }

    PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, [node]()
                      { return getSpecialOpSignature(node); });

    auto      parameters = node->parameters;
    TypeInfo* typeStruct = nullptr;
    if (parent)
    {
        auto implNode = CastAst<AstImpl>(parent, AstNodeKind::Impl);

        // No need to raise an error, the semantic pass on the impl node will fail
        typeStruct = implNode->identifier->typeInfo;
        if (!typeStruct->isStruct())
            return true;

        // First parameter must be be struct
        SWAG_VERIFY(node->parameters, context->report({node, node->tokenName, Fmt(Err(Err0068), name.c_str())}));
        auto firstGen  = node->parameters->childs.front();
        auto firstType = firstGen->typeInfo;
        SWAG_VERIFY(firstType->isPointer(), context->report({firstGen, Fmt(Err(Err0069), name.c_str(), typeStruct->getDisplayNameC(), firstType->getDisplayNameC())}));
        auto firstTypePtr = CastTypeInfo<TypeInfoPointer>(firstType, firstType->kind);
        SWAG_VERIFY(firstTypePtr->pointedType->isSame(typeStruct, CASTFLAG_CAST), context->report({firstGen, Fmt(Err(Err0069), name.c_str(), typeStruct->getDisplayNameC(), firstType->getDisplayNameC())}));
    }

    // Generic operator must have one generic parameter of type string
    if (name == g_LangSpec->name_opBinary ||
        name == g_LangSpec->name_opUnary ||
        name == g_LangSpec->name_opAssign ||
        name == g_LangSpec->name_opIndexAssign ||
        name == g_LangSpec->name_opAffectSuffix)
    {
        SWAG_VERIFY(node->genericParameters, context->report({node, node->tokenName, Fmt(Err(Err0470), name.c_str())}));
        SWAG_VERIFY(node->genericParameters->childs.size() <= 2, context->report({node->genericParameters, Fmt(Err(Err0071), name.c_str())}));
        auto firstGen = node->genericParameters->childs.front();
        SWAG_VERIFY(firstGen->specFlags & AstVarDecl::SPECFLAG_GENERIC_CONSTANT, context->report({firstGen, Fmt(Err(Err0659), name.c_str(), firstGen->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr->typeInfoString, CASTFLAG_CAST), context->report({firstGen, Fmt(Err(Err0072), name.c_str(), firstGen->typeInfo->getDisplayNameC())}));
    }
    else if (isOpVisit)
    {
        SWAG_VERIFY(node->genericParameters, context->report({node, node->tokenName, Fmt(Err(Err0470), name.c_str())}));
        SWAG_VERIFY(node->genericParameters->childs.size() >= 2, context->report({node->genericParameters, Fmt(Err(Err0544), name.c_str())}));
        SWAG_VERIFY(node->genericParameters->childs.size() <= 3, context->report({node->genericParameters, Fmt(Err(Err0071), name.c_str())}));
        auto firstGen = node->genericParameters->childs[0];
        SWAG_VERIFY(firstGen->specFlags & AstVarDecl::SPECFLAG_GENERIC_CONSTANT, context->report({firstGen, Fmt(Err(Err0659), name.c_str(), firstGen->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr->typeInfoBool, CASTFLAG_CAST), context->report({firstGen, Fmt(Err(Err0074), name.c_str(), firstGen->typeInfo->getDisplayNameC())}));
        auto secondGen = node->genericParameters->childs[1];
        SWAG_VERIFY(secondGen->specFlags & AstVarDecl::SPECFLAG_GENERIC_CONSTANT, context->report({secondGen, Fmt(Err(Err0659), name.c_str(), secondGen->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(secondGen->typeInfo->isSame(g_TypeMgr->typeInfoBool, CASTFLAG_CAST), context->report({secondGen, Fmt(Err(Err0074), name.c_str(), secondGen->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(node->attributeFlags & ATTRIBUTE_MACRO, context->report({node, node->token, Err(Err0075)}));
    }
    else if (name == g_LangSpec->name_opCast)
    {
        SWAG_VERIFY(!node->genericParameters, context->report({node->genericParameters, Fmt(Err(Err0478), name.c_str())}));
    }
    else
    {
        SWAG_VERIFY(!node->genericParameters || node->genericParameters->childs.size() == 1, context->report({node->genericParameters, Fmt(Err(Err0450), name.c_str(), node->genericParameters->childs.size())}));
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
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoU64));
    }
    else if (name == g_LangSpec->name_opData)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 1));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid, TYPEINFO_CONST)));
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
        auto second = TypeManager::concretePtrRef(parameters->childs[1]->typeInfo);
        SWAG_VERIFY(!second->isSame(typeStruct, CASTFLAG_EXACT), context->report({parameters->childs[1], Fmt(Err(Err0077), name.c_str(), typeStruct->name.c_str())}));
    }
    else if (name == g_LangSpec->name_opAffectSuffix)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        auto second = TypeManager::concretePtrRef(parameters->childs[1]->typeInfo);
        SWAG_VERIFY(!second->isSame(typeStruct, CASTFLAG_EXACT), context->report({parameters->childs[1], Fmt(Err(Err0077), name.c_str(), typeStruct->name.c_str())}));
    }
    else if (name == g_LangSpec->name_opSlice)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
        auto returnType = node->returnType->typeInfo->getConcreteAlias();
        if (!returnType->isString() && !returnType->isSlice())
            return context->report({node->returnType, Fmt(Err(Err0126), node->returnType->typeInfo->getDisplayNameC())});
        SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, 1, g_TypeMgr->typeInfoU64));
        SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, 2, g_TypeMgr->typeInfoU64));
    }
    else if (name == g_LangSpec->name_opIndex)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
        for (uint32_t i = 1; i < (uint32_t) parameters->childs.size(); i++)
            SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, i, g_TypeMgr->typeInfoU64));
    }
    else if (name == g_LangSpec->name_opIndexAssign)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        for (uint32_t i = 1; i < (uint32_t) parameters->childs.size() - 1; i++)
            SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, i, g_TypeMgr->typeInfoU64));
    }
    else if (name == g_LangSpec->name_opIndexAffect)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        for (uint32_t i = 1; i < (uint32_t) parameters->childs.size() - 1; i++)
            SWAG_CHECK(checkFuncPrototypeOpParam(context, node, parameters, i, g_TypeMgr->typeInfoU64));
    }
    else if (name == g_LangSpec->name_opInit && node->sourceFile->isGenerated)
    {
    }
    else
    {
        Vector<Utf8> searchList;
        Vector<Utf8> best;

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

        SemanticError::findClosestMatches(node->tokenName.text, searchList, best);
        Utf8 appendMsg = SemanticError::findClosestMatchesMsg(node->tokenName.text, best);

        Diagnostic diag{node, node->tokenName, Fmt(Err(Err0078), name.c_str())};
        if (appendMsg.empty())
        {
            auto note = Diagnostic::note(Nte(Nte0114));
            return context->report(diag, note);
        }
        else
        {
            auto note  = Diagnostic::note(node, node->getTokenName(), appendMsg);
            auto note1 = Diagnostic::note(Nte(Nte0114));
            return context->report(diag, note, note1);
        }
    }

    return true;
}

bool Semantic::resolveUserOpCommutative(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    // Simple case
    if (leftTypeInfo->isStruct() && !rightTypeInfo->isStruct())
        return resolveUserOp(context, name, opConst, opType, left, right);

    bool okLeft  = false;
    bool okRight = false;
    if (leftTypeInfo->isStruct())
    {
        okLeft = resolveUserOp(context, name, opConst, opType, left, right, ROP_JUST_CHECK);
        YIELD();
    }

    if (rightTypeInfo->isStruct())
    {
        okRight = resolveUserOp(context, name, opConst, opType, right, left, ROP_JUST_CHECK);
        YIELD();
    }

    if (okLeft || (!okRight && leftTypeInfo->isStruct()))
        return resolveUserOp(context, name, opConst, opType, left, right);

    node->semFlags |= SEMFLAG_INVERSE_PARAMS;
    return resolveUserOp(context, name, opConst, opType, right, left);
}

bool Semantic::hasUserOp(SemanticContext* context, const Utf8& name, TypeInfoStruct* leftStruct, SymbolName** result)
{
    *result = nullptr;
    if (leftStruct->isTuple())
        return true;

    VectorNative<FindUserOp> results;
    SWAG_CHECK(hasUserOp(context, name, leftStruct, nullptr, results));
    YIELD();
    if (results.empty())
        return true;

    if (results.size() > 1)
    {
        Diagnostic                diag{context->node, Fmt(Err(Err0182), name.c_str())};
        Vector<const Diagnostic*> notes;
        notes.push_back(Diagnostic::note(context->node, Fmt(Nte(Nte1047), name.c_str())));
        for (auto& f : results)
        {
            auto note = Diagnostic::note(f.usingField->declNode, Fmt(Nte(Nte0017), name.c_str(), f.parentStruct->getDisplayNameC()));
            notes.push_back(note);
        }

        return context->report(diag, notes);
    }

    *result = results[0].symbol;
    return true;
}

bool Semantic::hasUserOp(SemanticContext* context, const Utf8& name, TypeInfoStruct* leftStruct, TypeInfoParam* parentField, VectorNative<FindUserOp>& result)
{
    // In case of a generic instance, symbols are defined in the original generic structure scope, not
    // in the instance
    if (leftStruct->fromGeneric)
        leftStruct = leftStruct->fromGeneric;

    auto symbol = leftStruct->scope->symTable.find(name);
    if (symbol)
        result.push_back({symbol, leftStruct, parentField});

    if (!symbol && name != g_LangSpec->name_opPostCopy && name != g_LangSpec->name_opPostMove && name != g_LangSpec->name_opDrop)
    {
        // Struct in using hierarchy
        for (auto field : leftStruct->fields)
        {
            if (!(field->flags & TYPEINFOPARAM_HAS_USING))
                continue;
            auto typeS = field->typeInfo->getStructOrPointedStruct();
            if (!typeS)
                continue;

            Semantic::waitAllStructSpecialMethods(context->baseJob, typeS);
            YIELD();

            SWAG_CHECK(hasUserOp(context, name, typeS, field, result));
            YIELD();
        }
    }

    return true;
}

bool Semantic::hasUserOp(SemanticContext* context, const Utf8& name, AstNode* left, SymbolName** result)
{
    auto leftType = TypeManager::concreteType(left->typeInfo);
    if (leftType->isArray())
        leftType = CastTypeInfo<TypeInfoArray>(leftType, TypeInfoKind::Array)->finalType;
    else if (leftType->isPointer())
        leftType = CastTypeInfo<TypeInfoPointer>(leftType, TypeInfoKind::Pointer)->pointedType;
    leftType        = leftType->getConcreteAlias();
    auto leftStruct = CastTypeInfo<TypeInfoStruct>(leftType, TypeInfoKind::Struct);
    return hasUserOp(context, name, leftStruct, result);
}

bool Semantic::waitUserOp(SemanticContext* context, const Utf8& name, AstNode* left, SymbolName** result)
{
    SWAG_CHECK(hasUserOp(context, name, left, result));
    if (!*result)
        return true;

    auto       symbol = *result;
    ScopedLock lkn(symbol->mutex);
    if (symbol->cptOverloads)
        Semantic::waitSymbolNoLock(context->baseJob, symbol);

    return true;
}

bool Semantic::resolveUserOpAffect(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo, AstNode* left, AstNode* right)
{
    // For a var, trick error system, because we want to point on '='
    AstVarDecl* varDecl = nullptr;
    Token       savedToken;
    if (context->node->kind == AstNodeKind::VarDecl)
    {
        varDecl    = CastAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);
        savedToken = varDecl->token;
        if (varDecl->assignToken.id != TokenId::Invalid)
            varDecl->token = varDecl->assignToken;
    }

    // opAffectSuffix
    if (right->semFlags & SEMFLAG_LITERAL_SUFFIX)
    {
        Utf8 suffix;

        SWAG_ASSERT(right->kind == AstNodeKind::Literal || right->kind == AstNodeKind::SingleOp);
        if (right->kind == AstNodeKind::Literal)
            suffix = right->childs.front()->token.text;
        else if (right->kind == AstNodeKind::SingleOp)
            suffix = right->childs.front()->childs.front()->token.text;

        SymbolName* symbol = nullptr;
        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opAffectSuffix, left, &symbol));
        if (!symbol)
        {
            YIELD();

            Diagnostic diag{context->node, context->node->token, Fmt(Err(Err0889), leftTypeInfo->getDisplayNameC(), rightTypeInfo->getDisplayNameC(), leftTypeInfo->getDisplayNameC())};
            auto       note0 = Diagnostic::note(right->childs.front(), Fmt(Nte(Nte0057), suffix.c_str()));
            auto       note1 = Diagnostic::hereIs(leftTypeInfo->declNode, true);
            diag.hint        = Fmt(Nte(Nte1047), g_LangSpec->name_opAffectSuffix.c_str());
            diag.addRange(left->token, Diagnostic::isType(leftTypeInfo));
            return context->report(diag, note0, note1);
        }

        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAffectSuffix, suffix, nullptr, left, right));
        if (varDecl)
            varDecl->token = savedToken;
        YIELD();
        right->semFlags &= ~SEMFLAG_LITERAL_SUFFIX;
    }

    // opAffect
    else
    {
        SymbolName* symbol = nullptr;
        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opAffect, left, &symbol));
        if (!symbol)
        {
            YIELD();

            Diagnostic diag{right, Fmt(Err(Err0908), leftTypeInfo->getDisplayNameC(), rightTypeInfo->getDisplayNameC())};
            diag.hint = Diagnostic::isType(rightTypeInfo);
            diag.addRange(left, Diagnostic::isType(leftTypeInfo));
            auto note  = Diagnostic::note(context->node, context->node->token, Fmt(Nte(Nte1047), g_LangSpec->name_opAffect.c_str()));
            auto note1 = Diagnostic::hereIs(leftTypeInfo->declNode->resolvedSymbolOverload);
            return context->report(diag, note, note1);
        }

        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAffect, nullptr, nullptr, left, right));
        if (varDecl)
            varDecl->token = savedToken;
    }

    return true;
}

bool Semantic::resolveUserOp(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right, uint32_t ropFlags)
{
    VectorNative<AstNode*> params;
    SWAG_ASSERT(left);
    params.push_back(left);
    if (right)
        params.push_back(right);
    return resolveUserOp(context, name, opConst, opType, left, params, ropFlags);
}

bool Semantic::resolveUserOp(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, VectorNative<AstNode*>& params, uint32_t ropFlags)
{
    SymbolName* symbol = nullptr;
    SWAG_CHECK(waitUserOp(context, name, left, &symbol));
    YIELD();

    bool justCheck = ropFlags & ROP_JUST_CHECK;
    auto leftType  = TypeManager::concretePtrRefType(left->typeInfo);

    if (!symbol)
    {
        if (justCheck)
            return false;

        Diagnostic* note = nullptr;
        if (leftType->declNode)
            note = Diagnostic::hereIs(leftType->declNode, true);

        if (!opConst)
        {
            Diagnostic diag{left->parent->sourceFile, left->parent->token, Fmt(Err(Err0079), name.c_str(), leftType->getDisplayNameC())};
            diag.hint = Fmt(Nte(Nte1047), name.c_str());
            diag.addRange(left, Diagnostic::isType(leftType));
            return context->report(diag, note);
        }
        else
        {
            Diagnostic diag{left->parent->sourceFile, left->parent->token, Fmt(Err(Err0186), name.c_str(), leftType->getDisplayNameC(), opConst)};
            diag.hint = Fmt(Nte(Nte1047), name.c_str());
            diag.addRange(left, Diagnostic::isType(leftType));
            return context->report(diag, note);
        }
    }

    auto node       = context->node;
    auto sourceFile = context->sourceFile;

    SymbolMatchContext symMatchContext;
    symMatchContext.flags |= SymbolMatchContext::MATCH_UNCONST; // Do not test const
    for (auto param : params)
        symMatchContext.parameters.push_back(param);

    // Generic string parameter
    AstNode* genericParameters = nullptr;
    AstNode  parameters;
    AstNode  literal;
    Ast::constructNode(&parameters);
    Ast::constructNode(&literal);
    ComputedValue cValue;
    parameters.sourceFile = left->sourceFile;
    parameters.inheritTokenLocation(left);
    parameters.inheritOwners(left);
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

    if (leftType->isGeneric())
    {
        PushErrCxtStep ec(
            context, left->parent, ErrCxtStepKind::Note, [&]()
            { return Fmt(Nte(Nte0051), name.c_str(), leftType->getDisplayNameC()); },
            true);
        Diagnostic diag(left, Fmt(Err(Err0715), name.c_str()));
        return context->report(diag);
    }

    auto& listTryMatch = context->cacheListTryMatch;
    while (true)
    {
        context->clearTryMatch();

        {
            SharedLock lk(symbol->mutex);
            for (auto overload : symbol->overloads)
            {
                auto t               = context->getTryMatch();
                t->symMatchContext   = symMatchContext;
                t->overload          = overload;
                t->genericParameters = genericParameters;
                t->callParameters    = left->parent;
                t->dependentVar      = nullptr;
                t->cptOverloads      = (uint32_t) symbol->overloads.size();
                t->cptOverloadsInit  = symbol->cptOverloadsInit;
                t->ufcs              = true;
                listTryMatch.push_back(t);
            }
        }

        {
            PushErrCxtStep ec(
                context, left->parent, ErrCxtStepKind::Note, [name, leftType]()
                { return Fmt(Nte(Nte0051), name.c_str(), leftType->getDisplayNameC()); },
                true);
            SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, nullptr, justCheck ? MIP_JUST_CHECK : 0));
        }

        if (context->result == ContextResult::Pending)
            return true;
        if (context->result != ContextResult::NewChilds)
            break;
        context->result = ContextResult::Done;
    }

    uint64_t castFlags = CASTFLAG_UNCONST | CASTFLAG_AUTO_OPCAST | CASTFLAG_UFCS | CASTFLAG_ACCEPT_PENDING | CASTFLAG_PARAMS;
    if (justCheck)
        castFlags |= CASTFLAG_JUST_CHECK;
    if (context->cacheMatches.empty())
        return true;

    auto oneMatch = context->cacheMatches[0];
    for (size_t i = 0; i < params.size(); i++)
    {
        if (i < oneMatch->solvedParameters.size() && oneMatch->solvedParameters[i])
        {
            auto toType = oneMatch->solvedParameters[i]->typeInfo;
            SWAG_CHECK(TypeManager::makeCompatibles(context, toType, nullptr, params[i], castFlags));
            YIELD();

            // :ConcreteRef
            if (params[i]->typeInfo->isPointerRef() && !toType->isPointerRef())
                setUnRef(params[i]);

            auto makePtrL = params[i];

            // If passing a closure
            // :FctCallParamClosure
            if (!(ropFlags & ROP_SIMPLE_CAST))
            {
                auto toTypeRef = toType->getConcreteAlias();
                if (makePtrL && toTypeRef && toTypeRef->isClosure())
                {
                    bool convert = false;
                    if (makePtrL->kind == AstNodeKind::MakePointer || makePtrL->kind == AstNodeKind::MakePointerLambda)
                        convert = true;
                    if (makePtrL->typeInfo && makePtrL->typeInfo->isLambda())
                        convert = true;
                    if (makePtrL->typeInfo->isPointerNull())
                        convert = true;
                    if (convert)
                    {
                        // Create a function call param node, and move the node inside it
                        auto oldParent = makePtrL->parent;
                        auto oldIdx    = makePtrL->childParentIdx();
                        Ast::removeFromParent(makePtrL);

                        auto nodeCall = Ast::newFuncCallParam(makePtrL->sourceFile, oldParent);
                        nodeCall->flags |= AST_REVERSE_SEMANTIC;
                        Ast::removeFromParent(nodeCall);
                        Ast::insertChild(oldParent, nodeCall, oldIdx);
                        Ast::addChildBack(nodeCall, makePtrL);

                        auto varNode = Ast::newVarDecl(sourceFile, Fmt("__ctmp_%d", g_UniqueID.fetch_add(1)), nodeCall);

                        if (makePtrL->typeInfo->isLambda())
                        {
                            varNode->assignment = Ast::clone(makePtrL, varNode);
                            Ast::removeFromParent(makePtrL);
                            varNode->allocateExtension(ExtensionKind::Owner);
                            varNode->extOwner()->nodesToFree.push_back(makePtrL);
                        }
                        else if (makePtrL->typeInfo->isPointerNull())
                        {
                            nodeCall->flags &= ~AST_VALUE_COMPUTED;
                            makePtrL->flags |= AST_NO_BYTECODE;
                            Ast::removeFromParent(makePtrL);
                            varNode->allocateExtension(ExtensionKind::Owner);
                            varNode->extOwner()->nodesToFree.push_back(makePtrL);
                        }
                        else
                        {
                            makePtrL->semFlags |= SEMFLAG_ONCE;
                            Ast::removeFromParent(makePtrL);
                            Ast::addChildBack(varNode, makePtrL);
                            varNode->assignment = makePtrL;
                        }

                        varNode->type           = Ast::newTypeExpression(sourceFile, varNode);
                        varNode->type->typeInfo = toType;
                        varNode->type->flags |= AST_NO_SEMANTIC;

                        auto idRef = Ast::newIdentifierRef(sourceFile, varNode->token.text, nodeCall);
                        idRef->allocateExtension(ExtensionKind::Misc);
                        idRef->extMisc()->exportNode = makePtrL;

                        // Put child front, because emitFuncCallParam wants the parameter to be the first
                        Ast::removeFromParent(idRef);
                        Ast::addChildFront(nodeCall, idRef);

                        // Add the 2 nodes to the semantic
                        context->baseJob->nodes.push_back(nodeCall);

                        context->result = ContextResult::NewChilds;
                        return true;
                    }
                }
            }
        }
    }

    // Make the real cast for all the call parameters
    if (!justCheck)
    {
        auto overload  = oneMatch->symbolOverload;
        node->typeInfo = overload->typeInfo;
        node->allocateExtension(ExtensionKind::Misc);
        node->extMisc()->resolvedUserOpSymbolOverload = overload;
        SWAG_ASSERT(symbol && symbol->kind == SymbolKind::Function);
        SWAG_ASSERT(overload);

        // Allocate room on the stack to store the result of the function call
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
        if (CallConv::returnNeedsStack(typeFunc))
        {
            allocateOnStack(node, typeFunc->concreteReturnType());
        }
    }

    return true;
}
