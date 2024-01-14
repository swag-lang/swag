#include "pch.h"
#include "SemanticError.h"
#include "Ast.h"
#include "AstOutput.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "Semantic.h"
#include "SyntaxColor.h"
#include "TypeManager.h"

void SemanticError::symbolErrorNotes(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag, Vector<const Diagnostic*>& notes)
{
    if (!node)
        return;
    if (node->kind != AstNodeKind::Identifier && node->kind != AstNodeKind::FuncCall)
        return;
    auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier, AstNodeKind::FuncCall);

    // Symbol has been found with a using : display it
    if (tryMatches.size() == 1 && tryMatches[0]->dependentVar)
    {
        // Do not generate a note if this is a generated 'using' in case of methods
        if (!tryMatches[0]->dependentVar->isGeneratedSelf())
        {
            auto note = Diagnostic::note(tryMatches[0]->dependentVar, Fmt(Nte(Nte0013), tryMatches[0]->overload->symbol->name.c_str()));
            notes.push_back(note);
        }
    }

    // Additional error if the first parameter does not match, or if nothing matches
    bool badUfcs = tryMatches.empty();
    for (auto over : tryMatches)
    {
        if (over->symMatchContext.result == MatchResult::BadSignature && over->symMatchContext.badSignatureInfos.badSignatureParameterIdx == 0)
        {
            badUfcs = true;
            break;
        }
    }

    if (badUfcs && !identifier->identifierRef()->startScope)
    {
        // There's something before (identifier is not the only one in the identifierRef).
        if (identifier != identifier->parent->childs.front())
        {
            auto idIdx = identifier->childParentIdx();
            auto prev  = identifier->identifierRef()->childs[idIdx - 1];
            if (prev->resolvedSymbolName)
            {
                if (prev->hasExtMisc() && prev->extMisc()->resolvedUserOpSymbolOverload)
                {
                    auto typeInfo = TypeManager::concreteType(prev->extMisc()->resolvedUserOpSymbolOverload->typeInfo);
                    auto note     = Diagnostic::note(prev, Fmt(Nte(Nte0018), prev->extMisc()->resolvedUserOpSymbolOverload->symbol->name.c_str(), typeInfo->getDisplayNameC()));
                    note->hint    = Diagnostic::isType(typeInfo);
                    notes.push_back(note);
                    return;
                }

                if (prev->kind == AstNodeKind::ArrayPointerIndex)
                {
                    auto api = CastAst<AstArrayPointerIndex>(prev, AstNodeKind::ArrayPointerIndex);
                    if (api->array->typeInfo)
                    {
                        prev           = api->array;
                        auto typeArray = CastTypeInfo<TypeInfoArray>(api->array->typeInfo, TypeInfoKind::Array);
                        auto note      = Diagnostic::note(prev, Fmt(Nte(Nte0000), prev->token.ctext(), typeArray->finalType->getDisplayNameC()));
                        notes.push_back(note);
                    }
                }
                else
                {
                    Diagnostic* note = nullptr;
                    if (prev->typeInfo)
                        note = Diagnostic::note(prev, Fmt(Nte(Nte0001), prev->token.ctext(), Naming::aKindName(prev->resolvedSymbolName->kind).c_str(), prev->typeInfo->getDisplayNameC()));
                    else
                        note = Diagnostic::note(prev, Fmt(Nte(Nte0010), prev->token.ctext(), Naming::aKindName(prev->resolvedSymbolName->kind).c_str()));
                    notes.push_back(note);
                    if (prev->resolvedSymbolOverload)
                        notes.push_back(Diagnostic::hereIs(prev));
                }
            }
        }
    }
}

void SemanticError::symbolErrorRemarks(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag)
{
    if (!node)
        return;
    if (node->kind != AstNodeKind::Identifier && node->kind != AstNodeKind::FuncCall)
        return;
    if (tryMatches.empty())
        return;

    // If we have an UFCS call, and the match does not come from its symtable, then that means that we have not found the
    // symbol in the original struct also.
    auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    if (identifier->identifierRef()->startScope && !tryMatches.empty())
    {
        size_t notFound = 0;
        for (auto tryMatch : tryMatches)
        {
            if (tryMatch->ufcs &&
                tryMatch->overload->node->ownerStructScope &&
                identifier->ownerStructScope &&
                tryMatch->overload->node->ownerStructScope->owner != identifier->identifierRef()->startScope->owner)
                notFound++;
        }

        if (notFound == tryMatches.size())
        {
            if (identifier->identifierRef()->typeInfo)
            {
                auto over = tryMatches.front()->overload;
                auto msg  = Fmt(Nte(Nte0043),
                               Naming::kindName(over).c_str(),
                               node->token.ctext(),
                               identifier->identifierRef()->typeInfo->getDisplayNameC(),
                               over->node->ownerStructScope->owner->token.ctext());
                diag->remarks.push_back(msg);
            }

            for (auto s : identifier->identifierRef()->startScope->childScopes)
            {
                if (s->kind == ScopeKind::Impl)
                {
                    if (s->symTable.find(node->token.text))
                    {
                        diag->remarks.push_back(Fmt(Nte(Nte0044), node->token.ctext(), s->getFullName().c_str()));
                    }
                }
            }
        }
    }
}

bool SemanticError::cannotMatchIdentifierError(SemanticContext* context, MatchResult result, int paramIdx, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Vector<const Diagnostic*>& notes)
{
    if (tryMatches.empty())
        return false;

    bool                       hasCorrectResult = false;
    VectorNative<OneTryMatch*> tryResult;
    for (size_t i = 0; i < tryMatches.size(); i++)
    {
        auto one = tryMatches[i];
        if (one->symMatchContext.result == result)
        {
            hasCorrectResult = true;
            bool take        = true;
            if (result == MatchResult::BadSignature || result == MatchResult::BadGenericSignature)
                take = one->symMatchContext.badSignatureInfos.badSignatureParameterIdx == paramIdx;
            if (take)
            {
                tryResult.push_back(one);
                tryMatches.erase(i);
                i--;
            }
        }
    }

    if (tryResult.empty())
        return hasCorrectResult;

    static const int MAX_OVERLOADS = 5;
    Diagnostic*      note          = nullptr;

    switch (result)
    {
    case MatchResult::ValidIfFailed:
        note = Diagnostic::note(node, node->token, "all '#validif' have failed");
        break;
    case MatchResult::TooManyParameters:
        note = Diagnostic::note(node, node->token, "too many arguments");
        break;
    case MatchResult::TooManyGenericParameters:
        note = Diagnostic::note(node, node->token, "too many generic arguments");
        break;
    case MatchResult::NotEnoughParameters:
        note = Diagnostic::note(node, node->token, "not enough arguments");
        break;
    case MatchResult::NotEnoughGenericParameters:
        note = Diagnostic::note(node, node->token, "not enough generic arguments");
        break;
    case MatchResult::BadSignature:
        if (tryResult[0]->ufcs && paramIdx == 0)
            note = Diagnostic::note(node, node->token, "the UFCS argument does not match");
        else
            note = Diagnostic::note(node, node->token, Fmt("the %s does not match", Naming::niceArgumentRank(paramIdx + 1).c_str()));
        break;
    case MatchResult::BadGenericSignature:
        note = Diagnostic::note(node, node->token, Fmt("the generic %s does not match", Naming::niceArgumentRank(paramIdx + 1).c_str()));
        break;
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    Concat                   concat;
    AstOutput::OutputContext outCxt;
    concat.init(10 * 1024);

    for (size_t i = 0; i < min(tryResult.size(), MAX_OVERLOADS); i++)
    {
        if (tryResult[i]->overload->node->kind == AstNodeKind::FuncDecl)
        {
            auto funcNode = CastAst<AstFuncDecl>(tryResult[i]->overload->node, AstNodeKind::FuncDecl);
            AstOutput::outputFuncSignature(outCxt, concat, funcNode, funcNode->genericParameters, funcNode->parameters, nullptr);
        }
        else if (tryResult[i]->overload->node->kind == AstNodeKind::VarDecl)
        {
            auto varNode = CastAst<AstVarDecl>(tryResult[i]->overload->node, AstNodeKind::VarDecl);
            auto lambda  = CastAst<AstTypeLambda>(varNode->typeInfo->declNode, AstNodeKind::TypeLambda);
            AstOutput::outputFuncSignature(outCxt, concat, varNode, nullptr, lambda->parameters, nullptr);
        }
        else
        {
            SWAG_ASSERT(false);
        }

        Utf8 n = Utf8{(const char*) concat.firstBucket->datas, concat.bucketCount(concat.firstBucket)};
        if (n.back() == '\n')
            n.count--;
        if (n.back() == ';')
            n.count--;

        if (g_CommandLine.logColors)
        {
            SyntaxColorContext cxt;
            n = syntaxColor(n, cxt);
        }

        Utf8 fn = Fmt("%d: %s", i + 1, n.c_str());

        note->remarks.push_back(fn);
        fn.clear();

        if (result == MatchResult::BadSignature || result == MatchResult::BadGenericSignature)
        {
            Vector<const Diagnostic*> errs0, errs1;
            getDiagnosticForMatch(context, *tryResult[i], errs0, errs1);
            fn += "        ";

            Vector<Utf8> parts;
            Diagnostic::tokenizeError(errs0[0]->textMsg, parts);
            if (parts.size() > 1)
                fn += parts[1];
            else
                fn += parts[0];
        }

        note->remarks.push_back(fn);
        concat.clear();
    }

    if (tryResult.size() >= MAX_OVERLOADS)
        note->remarks.push_back("...");

    // Locate to the first error
    Vector<const Diagnostic*> errs0, errs1;
    getDiagnosticForMatch(context, *tryResult[0], errs0, errs1);
    note->sourceFile      = errs0[0]->sourceFile;
    note->startLocation   = errs0[0]->startLocation;
    note->endLocation     = errs0[0]->endLocation;
    note->forceSourceFile = true;

    notes.push_back(note);
    return true;
}

bool SemanticError::cannotMatchIdentifierError(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node)
{
    AstIdentifier* identifier        = nullptr;
    AstNode*       genericParameters = nullptr;

    // node can be null when we try to resolve a userOp
    if (node && (node->kind == AstNodeKind::Identifier || node->kind == AstNodeKind::FuncCall))
    {
        identifier        = CastAst<AstIdentifier>(node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
        genericParameters = identifier->genericParameters;
    }
    else
        node = context->node;

    // Take non generic errors in priority
    {
        Vector<OneTryMatch*> n;
        for (auto oneMatch : tryMatches)
        {
            auto& one = *oneMatch;
            switch (one.symMatchContext.result)
            {
            case MatchResult::BadSignature:
            case MatchResult::DuplicatedNamedParameter:
            case MatchResult::InvalidNamedParameter:
            case MatchResult::MissingNamedParameter:
            case MatchResult::MissingParameters:
            case MatchResult::NotEnoughParameters:
            case MatchResult::TooManyParameters:
            case MatchResult::ValidIfFailed:
            case MatchResult::NotEnoughGenericParameters:
                n.push_back(oneMatch);
                break;
            default:
                break;
            }
        }
        if (!n.empty())
            tryMatches = n;
    }

    // Remove duplicates overloads, as we can have one with ufcs, and one without, as we
    // could have tried both
    {
        Vector<OneTryMatch*> n;
        for (auto oneMatch : tryMatches)
        {
            for (auto oneMatch1 : tryMatches)
            {
                if (oneMatch == oneMatch1)
                    continue;
                if (oneMatch->overload != oneMatch1->overload)
                    continue;

                // If the ufcs version has matched the UFCS parameter, then take that one
                if (oneMatch->ufcs && oneMatch->symMatchContext.badSignatureInfos.badSignatureParameterIdx > 0)
                    oneMatch1->overload = nullptr;

                // If the UFCS has failed because of constness, takes the UFCS error
                else if (oneMatch->ufcs && oneMatch->symMatchContext.badSignatureInfos.badSignatureParameterIdx == 0 && oneMatch->symMatchContext.badSignatureInfos.castErrorType == CastErrorType::Const)
                    oneMatch1->overload = nullptr;

                // If this is a lambda call that comes from a struct, then this is ambiguous. Do we keep the error where the struct
                // has been used to find the lambda and UFCS, or do we consider the struct is just there to find the lambda ?
                // We consider that UFCS of 'struct.lambda.call(struct)' has less priority than other errors
                else if (oneMatch->ufcs && oneMatch->scope && oneMatch->scope->kind == ScopeKind::Struct && oneMatch->overload->symbol->kind == SymbolKind::Variable)
                    oneMatch->overload = nullptr;

                // Otherwise, if with UFCS we do not have enough argument, we use UFCS in priority
                else if (oneMatch->ufcs && oneMatch1->symMatchContext.result == MatchResult::NotEnoughParameters)
                    oneMatch1->overload = nullptr;

                // Otherwise we remove the UFCS error and take the other one
                else if (oneMatch->ufcs)
                    oneMatch->overload = nullptr;

                break;
            }

            if (oneMatch->overload)
                n.push_back(oneMatch);
        }
        if (!n.empty())
            tryMatches = n;
    }

    // If we do not have generic parameters, then eliminate generic fail
    if (!genericParameters)
    {
        Vector<OneTryMatch*> n;
        for (auto oneMatch : tryMatches)
        {
            auto& one = *oneMatch;
            if (!(one.overload->flags & OVERLOAD_GENERIC))
                n.push_back(oneMatch);
        }
        if (!n.empty())
            tryMatches = n;
    }

    // If we have generic parameters, then eliminate non generic fail
    if (genericParameters)
    {
        Vector<OneTryMatch*> n;
        for (auto oneMatch : tryMatches)
        {
            auto& one = *oneMatch;
            if (one.overload->flags & OVERLOAD_GENERIC)
                n.push_back(oneMatch);
        }
        if (!n.empty())
            tryMatches = n;
    }

    // Take validif if failed in priority
    {
        Vector<OneTryMatch*> n;
        for (auto oneMatch : tryMatches)
        {
            auto& one = *oneMatch;
            if (one.symMatchContext.result == MatchResult::ValidIfFailed)
                n.push_back(oneMatch);
        }
        if (!n.empty())
            tryMatches = n;
    }

    uint32_t getFlags = GDFM_ALL;

    // All errors are because of a constness problem on the UFCS argument
    // Then just raise one error
    int badConstUfcs = 0;
    for (auto& tm : tryMatches)
    {
        if (tm->ufcs &&
            tm->symMatchContext.badSignatureInfos.badSignatureParameterIdx == 0 &&
            tm->symMatchContext.badSignatureInfos.castErrorType == CastErrorType::Const)
            badConstUfcs += 1;
    }
    if (badConstUfcs == (int) tryMatches.size())
    {
        while (tryMatches.size() > 1)
            tryMatches.pop_back();
        getFlags &= ~GDFM_HERE_IS;
    }

    // One single overload
    if (tryMatches.size() == 1)
    {
        // Be sure this is not because of an invalid special function signature
        if (tryMatches[0]->overload->node->kind == AstNodeKind::FuncDecl)
            SWAG_CHECK(Semantic::checkFuncPrototype(context, CastAst<AstFuncDecl>(tryMatches[0]->overload->node, AstNodeKind::FuncDecl)));

        Vector<const Diagnostic*> errs0, errs1;
        getDiagnosticForMatch(context, *tryMatches[0], errs0, errs1, getFlags);
        SWAG_ASSERT(!errs0.empty());
        symbolErrorRemarks(context, tryMatches, node, const_cast<Diagnostic*>(errs0[0]));
        symbolErrorNotes(context, tryMatches, node, const_cast<Diagnostic*>(errs0[0]), errs1);
        return context->report(*errs0[0], errs1);
    }

    // Multiple tryMatches
    Diagnostic diag{node, node->token, Fmt(Err(Err0113), tryMatches.size(), tryMatches[0]->overload->symbol->name.c_str())};
    symbolErrorRemarks(context, tryMatches, node, &diag);

    Vector<const Diagnostic*> notes;
    symbolErrorNotes(context, tryMatches, node, &diag, notes);

    cannotMatchIdentifierError(context, MatchResult::ValidIfFailed, 0, tryMatches, node, notes);
    cannotMatchIdentifierError(context, MatchResult::NotEnoughParameters, 0, tryMatches, node, notes);
    cannotMatchIdentifierError(context, MatchResult::TooManyParameters, 0, tryMatches, node, notes);
    cannotMatchIdentifierError(context, MatchResult::NotEnoughGenericParameters, 0, tryMatches, node, notes);
    cannotMatchIdentifierError(context, MatchResult::TooManyGenericParameters, 0, tryMatches, node, notes);

    // For a bad signature, only show the ones with the greatest match
    int paramIdx;
    for (int what = 0; what < 2; what++)
    {
        Vector<const Diagnostic*> notesSig;
        paramIdx = 0;
        while (true)
        {
            Vector<const Diagnostic*> notesTmp;
            auto                      m = what == 0 ? MatchResult::BadSignature : MatchResult::BadGenericSignature;
            if (!cannotMatchIdentifierError(context, m, paramIdx++, tryMatches, node, notesTmp))
                break;
            notesSig = notesTmp;
        }
        notes.insert(notes.end(), notesSig.begin(), notesSig.end());
    }

    return context->report(diag, notes);
}

void SemanticError::unknownIdentifier(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node)
{
    auto& scopeHierarchy     = context->cacheScopeHierarchy;
    auto& scopeHierarchyVars = context->cacheScopeHierarchyVars;

    // What kind of thing to we search for ?
    auto searchFor = IdentifierSearchFor::Whatever;
    if (node->parent->parent && node->parent->parent->kind == AstNodeKind::TypeExpression && node->parent->childs.back() == node)
        searchFor = IdentifierSearchFor::Type;
    else if (node->parent->parent && node->parent->parent->kind == AstNodeKind::AttrUse && node->parent->childs.back() == node)
        searchFor = IdentifierSearchFor::Attribute;
    else if (node->callParameters && (node->callParameters->specFlags & AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT))
        searchFor = IdentifierSearchFor::Struct;
    else if (node->callParameters)
        searchFor = IdentifierSearchFor::Function;

    // Find best matches
    if (identifierRef->startScope)
    {
        scopeHierarchy.clear();
        Semantic::addAlternativeScopeOnce(scopeHierarchy, identifierRef->startScope);
    }
    else
    {
        Semantic::collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, node, COLLECT_ALL);
    }

    Vector<const Diagnostic*> notes;
    Diagnostic*               diag = nullptr;

    // Message
    switch (searchFor)
    {
    case IdentifierSearchFor::Function:
        if (node->token.text[0] == '#')
            diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0140), node->token.ctext())};
        else if (node->token.text[0] == '@')
            diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0129), node->token.ctext())};
        else
            diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0228), node->token.ctext())};
        break;
    case IdentifierSearchFor::Attribute:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0251), node->token.ctext())};
        break;
    case IdentifierSearchFor::Type:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0165), node->token.ctext())};
        break;
    case IdentifierSearchFor::Struct:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0393), node->token.ctext())};
        break;
    default:
        diag = new Diagnostic{node->sourceFile, node->token, Fmt(Err(Err0122), node->token.ctext())};
        break;
    }

    // Error in scope context
    if (identifierRef->startScope)
    {
        auto typeWhere = identifierRef->startScope->owner->typeInfo;
        auto typeRef   = TypeManager::concreteType(identifierRef->typeInfo);
        if (typeRef && typeRef->isPointer())
            typeRef = CastTypeInfo<TypeInfoPointer>(typeRef, TypeInfoKind::Pointer)->pointedType;

        if (typeRef && typeRef->isTuple())
        {
            diag            = new Diagnostic{node, Fmt(Err(Err0093), node->token.ctext())};
            auto structNode = CastAst<AstStruct>(identifierRef->startScope->owner, AstNodeKind::StructDecl);
            auto note       = Diagnostic::note(structNode->originalParent ? structNode->originalParent : identifierRef->startScope->owner, Nte(Nte0030));
            notes.push_back(note);
        }
        else
        {
            Utf8           displayName;
            AstIdentifier* prevIdentifier = nullptr;
            if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->kind == AstNodeKind::Identifier)
                prevIdentifier = CastAst<AstIdentifier>(identifierRef->previousResolvedNode, AstNodeKind::Identifier);
            if (!(identifierRef->startScope->flags & SCOPE_FILE))
                displayName = identifierRef->startScope->getDisplayFullName();
            if (displayName.empty() && !identifierRef->startScope->name.empty())
                displayName = identifierRef->startScope->name;
            if (displayName.empty() && typeRef)
                displayName = typeRef->name;
            if (!displayName.empty())
            {
                auto varDecl = node->findParent(AstNodeKind::VarDecl);
                auto idRef   = node->identifierRef();
                if (idRef && idRef->flags & AST_TUPLE_UNPACK && varDecl)
                {
                    diag = new Diagnostic{node, Fmt(Err(Err0821), varDecl->token.ctext(), displayName.c_str())};
                }
                else if (prevIdentifier && prevIdentifier->identifierExtension && prevIdentifier->identifierExtension->alternateEnum)
                {
                    auto altEnum    = prevIdentifier->identifierExtension->alternateEnum;
                    diag            = new Diagnostic{node, node->token, Fmt(Err(Err0492), node->token.ctext(), altEnum->getDisplayNameC(), Naming::kindName(identifierRef->startScope->kind).c_str(), displayName.c_str())};
                    auto note       = Diagnostic::hereIs(altEnum->declNode, false, true);
                    note->showRange = false;
                    notes.push_back(note);
                }
                else if (typeWhere)
                {
                    if (typeWhere->kind == TypeInfoKind::Struct && node->callParameters)
                    {
                        if (node->token.text.startsWith(g_LangSpec->name_opVisit) && node->token.text != g_LangSpec->name_opVisit)
                        {
                            Utf8 variant{node->token.text.buffer + g_LangSpec->name_opVisit.length()};
                            diag = new Diagnostic{node, node->token, Fmt(Err(Err1139), variant.c_str(), typeWhere->getDisplayNameC())};
                        }
                        else if (node->token.text == g_LangSpec->name_opVisit)
                        {
                            diag = new Diagnostic{node, node->token, Fmt(Err(Err0557), typeWhere->getDisplayNameC())};
                        }
                        else
                            diag = new Diagnostic{node, node->token, Fmt(Err(Err1107), node->token.ctext(), typeWhere->getDisplayNameC())};
                    }
                    else if (typeWhere->kind == TypeInfoKind::Enum)
                    {
                        diag = new Diagnostic{node, node->token, Fmt(Err(Err0144), node->token.ctext(), typeWhere->getDisplayNameC())};
                    }
                    else
                    {
                        diag = new Diagnostic{node, node->token, Fmt(Err(Err0112), node->token.ctext(), typeWhere->getDisplayNameC())};
                    }

                    if (prevIdentifier && prevIdentifier->resolvedSymbolName && prevIdentifier->resolvedSymbolName->kind == SymbolKind::Variable)
                        diag->addRange(prevIdentifier, Diagnostic::isType(prevIdentifier));
                }
                else
                {
                    Utf8 where = Naming::kindName(identifierRef->startScope->kind).c_str();
                    diag       = new Diagnostic{node, node->token, Fmt(Err(Err0110), node->token.ctext(), where.c_str(), displayName.c_str())};
                }

                switch (identifierRef->startScope->owner->kind)
                {
                case AstNodeKind::StructDecl:
                case AstNodeKind::InterfaceDecl:
                case AstNodeKind::EnumDecl:
                {
                    auto note = Diagnostic::hereIs(identifierRef->startScope->owner, false, true);
                    notes.push_back(note);
                    break;
                }
                default:
                    break;
                }
            }
        }
    }

    VectorNative<OneTryMatch*> v;
    symbolErrorRemarks(context, v, node, diag);
    symbolErrorNotes(context, v, node, diag, notes);

    Utf8 appendMsg = findClosestMatchesMsg(node->token.text, scopeHierarchy, searchFor);
    if (!appendMsg.empty())
        notes.push_back(Diagnostic::note(appendMsg));

    context->report(*diag, notes);
}

bool SemanticError::notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg, AstNode* hintType)
{
    Utf8 text = Fmt(Err(Err0005), node->token.ctext(), typeInfo->getDisplayNameC());
    if (msg)
    {
        text += " ";
        text += msg;
    }

    Diagnostic diag{node, node->token, text};
    if (hintType)
        diag.addRange(hintType, Diagnostic::isType(typeInfo));
    return context->report(diag);
}

bool SemanticError::duplicatedSymbolError(ErrorContext* context, SourceFile* sourceFile, Token& token, SymbolKind thisKind, const Utf8& thisName, SymbolKind otherKind, AstNode* otherSymbolDecl)
{
    Utf8 as;
    if (thisKind != otherKind)
        as = Fmt("as %s", Naming::aKindName(otherKind).c_str());

    Diagnostic diag{sourceFile, token, Fmt(Err(Err0305), Naming::kindName(thisKind).c_str(), thisName.c_str(), as.c_str())};
    auto       note = Diagnostic::note(otherSymbolDecl, otherSymbolDecl->getTokenName(), Nte(Nte0036));
    return context->report(diag, note);
}

bool SemanticError::error(SemanticContext* context, const Utf8& msg)
{
    return context->report({context->node, msg});
}
