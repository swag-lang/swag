#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/SourceFile.h"

bool Semantic::checkFuncPrototype(SemanticContext* context, AstFuncDecl* node)
{
    if (node->hasAstFlag(AST_GENERATED))
        return true;
    SWAG_CHECK(checkFuncPrototypeOp(context, node));
    return true;
}

Utf8 Semantic::getSpecialOpSignature(const AstFuncDecl* node)
{
    Utf8 result = "the signature should be ";

    if (node->token.is(g_LangSpec->name_opDrop))
        result += "[[func opDrop(me)]]";
    else if (node->token.is(g_LangSpec->name_opPostCopy))
        result += "[[func opPostCopy(me)]]";
    else if (node->token.is(g_LangSpec->name_opPostMove))
        result += "[[func opPostMove(me)]]";
    else if (node->token.is(g_LangSpec->name_opSlice))
        result += "[[func opSlice(me, low, up: u64) -> <string or slice>]]";
    else if (node->token.is(g_LangSpec->name_opIndex))
        result += "[[func opIndex(me, index: WhateverType) -> WhateverType]]";
    else if (node->token.is(g_LangSpec->name_opCount))
        result += "[[func opCount(me) -> u64]]";
    else if (node->token.is(g_LangSpec->name_opData))
        result += "[[func opData(me) -> *WhateverType]]";
    else if (node->token.is(g_LangSpec->name_opCast))
        result += "[[func opCast(me) -> WhateverType]]";
    else if (node->token.is(g_LangSpec->name_opEquals))
        result += "[[func opEquals(me, value: WhateverType) -> bool]]";
    else if (node->token.is(g_LangSpec->name_opCmp))
        result += "[[func opCmp(me, value: WhateverType) -> s32]]";
    else if (node->token.is(g_LangSpec->name_opAffect))
        result += "[[func opAffect(me, value: WhateverType)]]";
    else if (node->token.is(g_LangSpec->name_opAffectLiteral))
        result += "[[func(suffix: string) opAffectLiteral(me, value: WhateverType)]]";
    else if (node->token.is(g_LangSpec->name_opIndexAffect))
        result += "[[func opIndexAffect(me, index: WhateverType, value: WhateverType)]]";
    else if (node->token.is(g_LangSpec->name_opBinary))
        result += "[[func(op: string) opBinary(me, other: WhateverType) -> MyStruct]]";
    else if (node->token.is(g_LangSpec->name_opUnary))
        result += "[[func(op: string) opUnary(me) -> MyStruct]]";
    else if (node->token.is(g_LangSpec->name_opAssign))
        result += "[[func(op: string) opAssign(me, value: WhateverType)]]";
    else if (node->token.is(g_LangSpec->name_opIndexAssign))
        result += "[[func(op: string) opIndexAssign(me, index: WhateverType, value: WhateverType)]]";
    else if (node->token.text.startsWith(g_LangSpec->name_opVisit))
        result += "[[func(ptr: bool, back: bool) opVisit(me, stmt: code void)]]";
    else
        result = "";

    return result;
}

bool Semantic::checkFuncPrototypeOpNumParams(SemanticContext* context, const AstFuncDecl* node, AstNode* parameters, uint32_t numWanted, bool exact)
{
    const auto numCur = parameters->childCount();
    if (exact && numCur != numWanted)
    {
        if (numCur > numWanted)
            return context->report({parameters->children[numWanted], formErr(Err0743, node->token.cstr(), node->token.cstr(), numWanted, numCur)});
        return context->report({parameters, formErr(Err0728, node->token.cstr(), node->token.cstr(), numWanted, numCur)});
    }

    if (!exact && numCur < numWanted)
    {
        return context->report({parameters, formErr(Err0729, node->token.cstr(), node->token.cstr(), numWanted, numCur)});
    }

    return true;
}

bool Semantic::checkFuncPrototypeOpReturnType(SemanticContext* context, AstFuncDecl* node, TypeInfo* wanted)
{
    const auto returnType = node->returnType->typeInfo->getConcreteAlias();

    // Whatever except nothing
    if (wanted == nullptr)
    {
        if (returnType->isVoid())
            return context->report({node, node->getTokenName(), formErr(Err0706, node->token.cstr())});
        return true;
    }

    if (!wanted->isVoid() && returnType->isVoid())
        return context->report({node, node->getTokenName(), formErr(Err0705, wanted->getDisplayNameC(), node->token.cstr())});

    if (!returnType->isSame(wanted, CAST_FLAG_CAST))
    {
        auto childNode = node->returnType->children.empty() ? node->returnType : node->returnType->firstChild();
        auto msg       = formErr(Err0291, node->token.cstr(), returnType->getDisplayNameC(), node->token.cstr(), wanted->getDisplayNameC());
        return context->report({childNode, msg});
    }

    return true;
}

bool Semantic::checkFuncPrototypeOpParam(SemanticContext* context, AstNode* parameters, uint32_t index, TypeInfo* wanted)
{
    const auto typeParam = parameters->children[index]->typeInfo->getConcreteAlias();
    if (!typeParam->isSame(wanted, CAST_FLAG_CAST))
        return context->report({parameters->children[index], formErr(Err0745, wanted->getDisplayNameC(), typeParam->getDisplayNameC())});
    return true;
}

bool Semantic::checkFuncPrototypeOp(SemanticContext* context, AstFuncDecl* node)
{
    if (!node->isSpecialFunctionName())
        return true;

    const auto& name      = node->token.text;
    const bool  isOpVisit = name.find(g_LangSpec->name_opVisit) == 0;
    const auto  parent    = node->findParent(AstNodeKind::Impl);

    // opVisit variant should be a name
    if (isOpVisit && name != g_LangSpec->name_opVisit && !SWAG_IS_ALPHA(name[7]))
    {
        auto start = node->tokenName.startLocation;
        start.column += 7;
        const Diagnostic err{node->token.sourceFile, start, node->getTokenName().endLocation, formErr(Err0337, node->token.cstr() + 7)};
        return context->report(err);
    }

    if (isOpVisit && name != g_LangSpec->name_opVisit && !isupper(name[7]))
    {
        auto start = node->tokenName.startLocation;
        start.column += 7;
        const Diagnostic err{node->token.sourceFile, start, node->getTokenName().endLocation, formErr(Err0336, node->token.cstr() + 7)};
        return context->report(err);
    }

    // Special function outside an impl block
    if (!parent || node->ownerScope->is(ScopeKind::Impl))
    {
        const Diagnostic err{node, node->getTokenName(), formErr(Err0280, node->token.cstr(), node->token.cstr())};
        return context->report(err);
    }

    if (node->ownerScope->owner->hasAttribute(ATTRIBUTE_PUBLIC) && !node->hasAttribute(ATTRIBUTE_PUBLIC))
    {
        Diagnostic err{node, node->getTokenName(), formErr(Err0122, node->token.cstr())};
        err.addNote("a [[public]] struct should export all of its special functions");
        err.addNote(Diagnostic::hereIs(node->findParent(TokenId::KwdInternal)));
        return context->report(err);
    }

    if (!node->ownerScope->owner->hasAttribute(ATTRIBUTE_PUBLIC) && node->hasAttribute(ATTRIBUTE_PUBLIC))
    {
        Diagnostic err{node, node->getTokenName(), formErr(Err0123, node->token.cstr())};
        err.addNote("an [[internal]] struct cannot export its special functions");
        err.addNote(Diagnostic::hereIs(node->findParent(TokenId::KwdPublic)));
        return context->report(err);
    }

    PushErrCxtStep ec(context, nullptr, ErrCxtStepKind::Note, [node] { return getSpecialOpSignature(node); });

    const auto parameters = node->parameters;
    TypeInfo*  typeStruct = nullptr;
    if (parent)
    {
        const auto implNode = castAst<AstImpl>(parent, AstNodeKind::Impl);

        // No need to raise an error, the semantic pass on the impl node will fail
        typeStruct = implNode->identifier->typeInfo;
        if (!typeStruct->isStruct())
            return true;

        // First parameter must be struct
        SWAG_VERIFY(node->parameters, context->report({node, node->getTokenName(), formErr(Err0699, name.cstr())}));
        auto       firstGen  = node->parameters->firstChild();
        const auto firstType = firstGen->typeInfo;
        SWAG_VERIFY(firstType->isPointer(), context->report({firstGen, formErr(Err0428, typeStruct->getDisplayNameC(), name.cstr(), firstType->getDisplayNameC())}));
        const auto firstTypePtr = castTypeInfo<TypeInfoPointer>(firstType, firstType->kind);
        SWAG_VERIFY(firstTypePtr->pointedType->isSame(typeStruct, CAST_FLAG_CAST), context->report({firstGen, formErr(Err0428, typeStruct->getDisplayNameC(), name.cstr(), firstType->getDisplayNameC())}));
    }

    // Generic operator must have one generic parameter of type string
    if (name == g_LangSpec->name_opBinary ||
        name == g_LangSpec->name_opUnary ||
        name == g_LangSpec->name_opAssign ||
        name == g_LangSpec->name_opIndexAssign ||
        name == g_LangSpec->name_opAffectLiteral)
    {
        SWAG_VERIFY(node->genericParameters, context->report({node, node->getTokenName(), formErr(Err0674, name.cstr())}));
        SWAG_VERIFY(node->genericParameters->childCount() <= 2, context->report({node->genericParameters, formErr(Err0737, name.cstr())}));
        auto firstGen = node->genericParameters->firstChild();
        SWAG_VERIFY(firstGen->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_CONSTANT), context->report({firstGen, formErr(Err0366, name.cstr(), firstGen->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr->typeInfoString, CAST_FLAG_CAST), context->report({firstGen, formErr(Err0479, name.cstr(), firstGen->typeInfo->getDisplayNameC())}));
    }
    else if (isOpVisit)
    {
        SWAG_VERIFY(node->genericParameters, context->report({node, node->getTokenName(), formErr(Err0674, name.cstr())}));
        SWAG_VERIFY(node->genericParameters->childCount() >= 2, context->report({node->genericParameters, formErr(Err0726, name.cstr())}));
        SWAG_VERIFY(node->genericParameters->childCount() <= 3, context->report({node->genericParameters, formErr(Err0737, name.cstr())}));
        auto firstGen = node->genericParameters->firstChild();
        SWAG_VERIFY(firstGen->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_CONSTANT), context->report({firstGen, formErr(Err0366, name.cstr(), firstGen->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(firstGen->typeInfo->isSame(g_TypeMgr->typeInfoBool, CAST_FLAG_CAST), context->report({firstGen, formErr(Err0478, name.cstr(), firstGen->typeInfo->getDisplayNameC())}));
        auto secondGen = node->genericParameters->secondChild();
        SWAG_VERIFY(secondGen->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_CONSTANT), context->report({secondGen, formErr(Err0366, name.cstr(), secondGen->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(secondGen->typeInfo->isSame(g_TypeMgr->typeInfoBool, CAST_FLAG_CAST), context->report({secondGen, formErr(Err0478, name.cstr(), secondGen->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(node->hasAttribute(ATTRIBUTE_MACRO) && !node->hasAttribute(ATTRIBUTE_MIXIN), context->report({node, node->token, toErr(Err0581)}));
    }
    else if (name == g_LangSpec->name_opCast)
    {
        SWAG_VERIFY(!node->genericParameters, context->report({node->genericParameters, formErr(Err0055, name.cstr())}));
    }
    else
    {
        SWAG_VERIFY(!node->genericParameters || node->genericParameters->childCount() == 1, context->report({node->genericParameters, formErr(Err0738, name.cstr(), node->genericParameters->childCount())}));
    }

    // Check each function
    if (isOpVisit)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        SWAG_CHECK(checkFuncPrototypeOpParam(context, parameters, 1, g_TypeMgr->typeInfoCode));
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
        const auto second = TypeManager::concretePtrRef(parameters->secondChild()->typeInfo);
        SWAG_VERIFY(!second->isSame(typeStruct, CAST_FLAG_EXACT), context->report({parameters->secondChild(), formErr(Err0225, typeStruct->name.cstr(), name.cstr())}));
    }
    else if (name == g_LangSpec->name_opAffectLiteral)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
        const auto second = TypeManager::concretePtrRef(parameters->secondChild()->typeInfo);
        SWAG_VERIFY(!second->isSame(typeStruct, CAST_FLAG_EXACT), context->report({parameters->secondChild(), formErr(Err0225, typeStruct->name.cstr(), name.cstr())}));
    }
    else if (name == g_LangSpec->name_opSlice)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
        const auto returnType = node->returnType->typeInfo->getConcreteAlias();
        if (!returnType->isString() && !returnType->isSlice())
            return context->report({node->returnType, formErr(Err0335, node->returnType->typeInfo->getDisplayNameC())});
        SWAG_CHECK(checkFuncPrototypeOpParam(context, parameters, 1, g_TypeMgr->typeInfoU64));
        SWAG_CHECK(checkFuncPrototypeOpParam(context, parameters, 2, g_TypeMgr->typeInfoU64));
    }
    else if (name == g_LangSpec->name_opIndex)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 2, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, nullptr));
    }
    else if (name == g_LangSpec->name_opIndexAssign)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
    }
    else if (name == g_LangSpec->name_opIndexAffect)
    {
        SWAG_CHECK(checkFuncPrototypeOpNumParams(context, node, parameters, 3, false));
        SWAG_CHECK(checkFuncPrototypeOpReturnType(context, node, g_TypeMgr->typeInfoVoid));
    }
    else if (name == g_LangSpec->name_opInit && node->token.sourceFile->hasFlag(FILE_GENERATED))
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
        searchList.push_back(g_LangSpec->name_opAffectLiteral);
        searchList.push_back(g_LangSpec->name_opSlice);
        searchList.push_back(g_LangSpec->name_opIndex);
        searchList.push_back(g_LangSpec->name_opIndexAffect);
        searchList.push_back(g_LangSpec->name_opVisit);

        SemanticError::findClosestMatchesInList(node->tokenName.text, searchList, best);
        const Utf8 appendMsg = SemanticError::findClosestMatchesMsg(node->tokenName.text, best);

        Diagnostic err{node, node->getTokenName(), formErr(Err0430, name.cstr())};
        const auto n = "function names starting with [[op]] followed by an uppercase letter are reserved for struct methods";
        if (appendMsg.empty())
        {
            err.addNote(n);
            return context->report(err);
        }

        err.addNote(node, node->getTokenName(), appendMsg);
        err.addNote(n);
        return context->report(err);
    }

    return true;
}

bool Semantic::resolveUserOpCommutative(SemanticContext* context, const Utf8& name, const char* opConst, TypeInfo* opType, AstNode* left, AstNode* right, uint32_t ropFlags)
{
    const auto node          = context->node;
    const auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    const auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    // Simple case if the right expression is not a struct (and left is)
    // Only one way of solving.
    if (leftTypeInfo->isStruct() && !rightTypeInfo->isStruct())
        return resolveUserOp(context, name, opConst, opType, left, right, ropFlags);

    if (leftTypeInfo->isStruct())
    {
        g_SilentError++;
        resolveUserOp(context, name, opConst, opType, left, right, ropFlags);
        g_SilentError--;
        YIELD();
        if (node->hasExtraPointer(ExtraPointerKind::UserOp))
            return true;
    }

    if (rightTypeInfo->isStruct())
    {
        g_SilentError++;
        resolveUserOp(context, name, opConst, opType, right, left, ropFlags);
        g_SilentError--;
        YIELD();
        if (node->hasExtraPointer(ExtraPointerKind::UserOp))
        {
            node->addSemFlag(SEMFLAG_INVERSE_PARAMS);
            return true;
        }
    }

    // Will raise an error
    return resolveUserOp(context, name, opConst, opType, left, right, ropFlags);
}

bool Semantic::hasUserOp(SemanticContext* context, const Utf8& name, TypeInfoStruct* leftStruct, SymbolName** result)
{
    *result = nullptr;

    VectorNative<FindUserOp> results;
    SWAG_CHECK(hasUserOp(context, name, leftStruct, nullptr, results));
    YIELD();
    if (results.empty())
        return true;

    if (results.size() > 1)
    {
        Diagnostic err{context->node, formErr(Err0010, name.cstr())};
        err.addNote(context->node, form("there is an implicit call to [[%s]]", name.cstr()));
        for (const auto& f : results)
            err.addNote(f.usingField->declNode, form("[[%s]] was found in [[%s]] because of a [[using]] field", name.cstr(), f.parentStruct->getDisplayNameC()));
        return context->report(err);
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

    const auto symbol = leftStruct->scope->symTable.find(name);
    if (symbol)
        result.push_back({.symbol = symbol, .parentStruct = leftStruct, .usingField = parentField});

    if (!symbol && name != g_LangSpec->name_opPostCopy && name != g_LangSpec->name_opPostMove && name != g_LangSpec->name_opDrop)
    {
        // Struct in using hierarchy
        for (const auto field : leftStruct->fields)
        {
            if (!field->flags.has(TYPEINFOPARAM_HAS_USING))
                continue;
            const auto typeS = field->typeInfo->getStructOrPointedStruct();
            if (!typeS)
                continue;

            waitAllStructSpecialMethods(context->baseJob, typeS);
            YIELD();

            SWAG_CHECK(hasUserOp(context, name, typeS, field, result));
            YIELD();
        }
    }

    return true;
}

bool Semantic::hasUserOp(SemanticContext* context, const Utf8& name, const AstNode* left, SymbolName** result)
{
    auto leftType = TypeManager::concreteType(left->typeInfo);
    if (leftType->isArray())
        leftType = castTypeInfo<TypeInfoArray>(leftType, TypeInfoKind::Array)->finalType;
    else if (leftType->isPointer())
        leftType = castTypeInfo<TypeInfoPointer>(leftType, TypeInfoKind::Pointer)->pointedType;
    leftType              = leftType->getConcreteAlias();
    const auto leftStruct = castTypeInfo<TypeInfoStruct>(leftType, TypeInfoKind::Struct);
    return hasUserOp(context, name, leftStruct, result);
}

bool Semantic::waitUserOp(SemanticContext* context, const Utf8& name, const AstNode* left, SymbolName** result)
{
    SWAG_CHECK(hasUserOp(context, name, left, result));
    if (!*result)
        return true;

    const auto symbol = *result;
    ScopedLock lkn(symbol->mutex);
    if (symbol->cptOverloads)
        waitSymbolNoLock(context->baseJob, symbol);

    return true;
}

bool Semantic::resolveUserOpAffect(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo, AstNode* left, AstNode* right)
{
    // For a var, trick error system, because we want to point on '='
    AstVarDecl* varDecl = nullptr;
    Token       savedToken;
    if (context->node->is(AstNodeKind::VarDecl))
    {
        varDecl    = castAst<AstVarDecl>(context->node, AstNodeKind::VarDecl);
        savedToken = varDecl->token;
        if (varDecl->assignToken.isNot(TokenId::Invalid))
            varDecl->token = varDecl->assignToken;
    }

    // opAffectLiteral
    if (right->hasSemFlag(SEMFLAG_LITERAL_SUFFIX))
    {
        Utf8 suffix;

        SWAG_ASSERT(right->is(AstNodeKind::Literal) || right->is(AstNodeKind::SingleOp));
        if (right->is(AstNodeKind::Literal))
            suffix = right->firstChild()->token.text;
        else if (right->is(AstNodeKind::SingleOp))
            suffix = right->firstChild()->firstChild()->token.text;

        SymbolName* symbol = nullptr;
        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opAffectLiteral, left, &symbol));
        if (!symbol)
        {
            YIELD();

            Diagnostic err{context->node, context->node->token, formErr(Err0115, leftTypeInfo->getDisplayNameC(), leftTypeInfo->getDisplayNameC())};
            err.addNote(right->firstChild(), form("this is the literal suffix [[%s]]", suffix.cstr()));
            err.addNote(Diagnostic::hereIs(leftTypeInfo->declNode));
            return context->report(err);
        }

        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAffectLiteral, suffix, nullptr, left, right));
        if (varDecl)
            varDecl->token = savedToken;
        YIELD();
        right->removeSemFlag(SEMFLAG_LITERAL_SUFFIX);
    }

    // opAffect
    else
    {
        SymbolName* symbol = nullptr;
        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opAffect, left, &symbol));
        if (!symbol)
        {
            YIELD();

            Diagnostic err{right, formErr(Err0017, rightTypeInfo->getDisplayNameC(), leftTypeInfo->getDisplayNameC())};
            err.addNote(Diagnostic::isType(right));
            err.addNote(Diagnostic::isType(left));
            err.addNote(context->node, context->node->token, form("there is an implicit call to [[%s]]", g_LangSpec->name_opAffect.cstr()));
            err.addNote(Diagnostic::hereIs(leftTypeInfo->declNode->resolvedSymbolOverload()));
            return context->report(err);
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

    auto leftType = TypeManager::concretePtrRefType(left->typeInfo);

    if (!symbol)
    {
        const Diagnostic* note = nullptr;
        if (leftType->declNode)
            note = Diagnostic::hereIs(leftType->declNode);

        if (!opConst)
        {
            Diagnostic err{left->parent->token.sourceFile, left->parent->token, formErr(Err0077, name.cstr(), leftType->getDisplayNameC())};
            err.addNote(form("there is an implicit call to [[%s]]", name.cstr()));
            err.addNote(Diagnostic::isType(left));
            err.addNote(note);
            return context->report(err);
        }

        Diagnostic err{left->parent->token.sourceFile, left->parent->token, formErr(Err0472, name.cstr(), leftType->getDisplayNameC(), opConst)};
        err.addNote(form("there is an implicit call to [[%s]]", name.cstr()));
        err.addNote(Diagnostic::isType(left));
        err.addNote(note);
        return context->report(err);
    }

    const auto node = context->node;

    SymbolMatchContext symMatchContext;
    symMatchContext.matchFlags.add(SymbolMatchContext::MATCH_UN_CONST); // Do not test const
    for (auto param : params)
        symMatchContext.parameters.push_back(param);

    // Generic string parameter
    AstNode* genericParameters = nullptr;
    AstNode  parameters;
    AstNode  literal;
    Ast::constructNode(&parameters);
    Ast::constructNode(&literal);
    ComputedValue cValue;
    parameters.token.sourceFile = left->token.sourceFile;
    parameters.inheritTokenLocation(left->token);
    parameters.inheritOwners(left);
    literal.token.sourceFile = left->token.sourceFile;
    literal.inheritTokenLocation(left->token);
    literal.inheritOwners(left);
    literal.allocateComputedValue(); // LEAK

    if (opConst || opType)
    {
        literal.kind                  = AstNodeKind::Literal;
        literal.computedValue()->text = opConst ? opConst : "";
        literal.typeInfo              = opType ? opType : g_TypeMgr->typeInfoString;
        literal.addAstFlag(AST_COMPUTED_VALUE | AST_CONST_EXPR);
        symMatchContext.genericParameters.push_back(&literal);
        parameters.kind   = AstNodeKind::FuncDeclParams;
        genericParameters = &parameters;
        Ast::addChildBack(&parameters, &literal);
    }

    if (leftType->isGeneric())
    {
        PushErrCxtStep   ec(context, left->parent, ErrCxtStepKind::Note, [&] { return form("there is an implicit call to [[%s]] for the type [[%s]]", name.cstr(), leftType->getDisplayNameC()); }, true);
        const Diagnostic err(left, formErr(Err0668, name.cstr()));
        return context->report(err);
    }

    auto& listTryMatch = context->cacheListTryMatch;
    while (true)
    {
        context->clearTryMatch();

        {
            SharedLock lk(symbol->mutex);
            for (const auto overload : symbol->overloads)
            {
                auto t               = context->getTryMatch();
                t->symMatchContext   = symMatchContext;
                t->overload          = overload;
                t->genericParameters = genericParameters;
                t->callParameters    = left->parent;
                t->dependentVar      = nullptr;
                t->cptOverloads      = symbol->overloads.size();
                t->cptOverloadsInit  = symbol->cptOverloadsInit;
                t->ufcs              = true;
                listTryMatch.push_back(t);
            }
        }

        {
            PushErrCxtStep ec(context, left->parent, ErrCxtStepKind::Note, [name, leftType] { return form("there is an implicit call to [[%s]] for the type [[%s]]", name.cstr(), leftType->getDisplayNameC()); }, true);
            context->node = left->parent;
            SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, nullptr));
            context->node = node;
        }

        if (context->result == ContextResult::Pending)
            return true;
        if (context->result != ContextResult::NewChildren)
            break;
        context->result = ContextResult::Done;
    }

    if (context->cacheMatches.empty())
        return true;

    const auto oneMatch = context->cacheMatches[0];
    for (uint32_t i = 0; i < params.size(); i++)
    {
        if (i < oneMatch->solvedParameters.size() && oneMatch->solvedParameters[i])
        {
            const auto toType    = oneMatch->solvedParameters[i]->typeInfo;
            CastFlags  castFlags = CAST_FLAG_UN_CONST | CAST_FLAG_AUTO_OP_CAST | CAST_FLAG_UFCS | CAST_FLAG_ACCEPT_PENDING | CAST_FLAG_PARAMS;
            if (!oneMatch->solvedParameters[i]->flags.has(TYPEINFOPARAM_FROM_GENERIC))
                castFlags.add(CAST_FLAG_TRY_COERCE);
            SWAG_CHECK(TypeManager::makeCompatibles(context, toType, nullptr, params[i], castFlags));
            YIELD();

            const auto makePtrL = params[i];

            // @ConcreteRef
            if (makePtrL->typeInfo->isPointerRef() && !toType->isPointerRef())
            {
                setUnRef(makePtrL);
                if (makePtrL->typeInfoCast)
                {
                    const auto fromCast = TypeManager::concreteType(makePtrL->typeInfoCast, CONCRETE_FUNC | CONCRETE_ALIAS);
                    if (fromCast->isPointerRef())
                        makePtrL->typeInfoCast = fromCast->getFinalType();
                }
            }

            // If passing a closure
            // @FctCallParamClosure
            if (!(ropFlags & ROP_SIMPLE_CAST))
            {
                const auto toTypeRef = toType->getConcreteAlias();
                if (makePtrL && toTypeRef && toTypeRef->isClosure())
                {
                    bool convert = false;
                    if (makePtrL->is(AstNodeKind::MakePointer) || makePtrL->is(AstNodeKind::MakePointerLambda))
                        convert = true;
                    if (makePtrL->typeInfo && makePtrL->typeInfo->isLambda())
                        convert = true;
                    if (makePtrL->typeInfo->isPointerNull())
                        convert = true;
                    if (convert)
                    {
                        // Create a function call param node, and move the node inside it
                        const auto oldParent = makePtrL->parent;
                        const auto oldIdx    = makePtrL->childParentIdx();
                        Ast::removeFromParent(makePtrL);

                        const auto nodeCall = Ast::newFuncCallParam(nullptr, oldParent);
                        nodeCall->addAstFlag(AST_REVERSE_SEMANTIC);
                        Ast::removeFromParent(nodeCall);
                        Ast::insertChild(oldParent, nodeCall, oldIdx);
                        Ast::addChildBack(nodeCall, makePtrL);

                        const auto varNode = Ast::newVarDecl(form("__ctmp_%d", g_UniqueID.fetch_add(1)), nullptr, nodeCall);

                        if (makePtrL->typeInfo->isLambda())
                        {
                            varNode->assignment = Ast::clone(makePtrL, varNode);
                            Ast::removeFromParent(makePtrL);
                            varNode->allocateExtension(ExtensionKind::Owner);
                            varNode->extOwner()->nodesToFree.push_back(makePtrL);
                        }
                        else if (makePtrL->typeInfo->isPointerNull())
                        {
                            nodeCall->removeAstFlag(AST_COMPUTED_VALUE);
                            makePtrL->addAstFlag(AST_NO_BYTECODE);
                            Ast::removeFromParent(makePtrL);
                            varNode->allocateExtension(ExtensionKind::Owner);
                            varNode->extOwner()->nodesToFree.push_back(makePtrL);
                        }
                        else
                        {
                            makePtrL->addSemFlag(SEMFLAG_ONCE);
                            Ast::removeFromParent(makePtrL);
                            Ast::addChildBack(varNode, makePtrL);
                            varNode->assignment = makePtrL;
                        }

                        varNode->type           = Ast::newTypeExpression(nullptr, varNode);
                        varNode->type->typeInfo = toType;
                        varNode->type->addAstFlag(AST_NO_SEMANTIC);

                        const auto idRef = Ast::newIdentifierRef(varNode->token.text, nullptr, nodeCall);
                        idRef->addExtraPointer(ExtraPointerKind::ExportNode, makePtrL);

                        // Put child front, because emitFuncCallParam wants the parameter to be the first
                        Ast::removeFromParent(idRef);
                        Ast::addChildFront(nodeCall, idRef);

                        // Add the 2 nodes to the semantic
                        context->baseJob->nodes.push_back(nodeCall);

                        context->result = ContextResult::NewChildren;
                        return true;
                    }
                }
            }
        }
    }

    // Result
    const auto overload = oneMatch->symbolOverload;
    node->typeInfo      = overload->typeInfo;
    node->addExtraPointer(ExtraPointerKind::UserOp, overload);
    SWAG_ASSERT(symbol && symbol->is(SymbolKind::Function));
    SWAG_ASSERT(overload);

    // Allocate room on the stack to store the result of the function call
    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
    if (typeFunc->returnNeedsStack())
        allocateOnStack(node, typeFunc->concreteReturnType());

    return true;
}
