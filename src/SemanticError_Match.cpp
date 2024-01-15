#include "pch.h"
#include "SemanticError.h"
#include "Ast.h"
#include "AstOutput.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Semantic.h"
#include "SyntaxColor.h"
#include "TypeManager.h"

static bool cannotMatchIdentifier(SemanticContext* context, MatchResult result, int paramIdx, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Vector<const Diagnostic*>& notes)
{
    if (tryMatches.empty())
        return false;

    // Filter matches depending on argument
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
    default:
        SWAG_ASSERT(false);
        break;
    }

    Concat                   concat;
    AstOutput::OutputContext outCxt;
    concat.init(10 * 1024);

    for (size_t i = 0; i < min(tryResult.size(), MAX_OVERLOADS); i++)
    {
        concat.clear();
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

        // Additional (more precise) information in case of bad signature
        if (result == MatchResult::BadSignature || result == MatchResult::BadGenericSignature)
        {
            Vector<const Diagnostic*> errs0, errs1;
            SemanticError::getDiagnosticForMatch(context, *tryResult[i], errs0, errs1);
            Vector<Utf8> parts;
            Diagnostic::tokenizeError(errs0[0]->textMsg, parts);

            if (tryResult.size() == 1)
            {
                if (parts.size() > 1)
                    note->textMsg = parts[1];
                else
                    note->textMsg = parts[0];
            }
            else
            {
                fn = "   => ";
                if (parts.size() > 1)
                    fn += parts[1];
                else
                    fn += parts[0];
                note->remarks.push_back(fn);
            }
        }
    }

    if (tryResult.size() >= MAX_OVERLOADS)
        note->remarks.push_back("...");

    // Locate to the first error
    Vector<const Diagnostic*> errs0, errs1;
    SemanticError::getDiagnosticForMatch(context, *tryResult[0], errs0, errs1);
    note->sourceFile    = errs0[0]->sourceFile;
    note->startLocation = errs0[0]->startLocation;
    note->endLocation   = errs0[0]->endLocation;
    note->canBeMerged   = false;

    notes.push_back(note);
    return true;
}

static bool cannotMatchSingle(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches, uint32_t getFlags)
{
    // Be sure this is not because of an invalid special function signature
    if (tryMatches[0]->overload->node->kind == AstNodeKind::FuncDecl)
        SWAG_CHECK(Semantic::checkFuncPrototype(context, CastAst<AstFuncDecl>(tryMatches[0]->overload->node, AstNodeKind::FuncDecl)));

    Vector<const Diagnostic*> errs0, errs1;
    SemanticError::getDiagnosticForMatch(context, *tryMatches[0], errs0, errs1, getFlags);
    SWAG_ASSERT(!errs0.empty());
    SemanticError::commonErrorNotes(context, tryMatches, node, const_cast<Diagnostic*>(errs0[0]), errs1);
    return context->report(*errs0[0], errs1);
}

static bool cannotMatchOverload(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches)
{
    // Multiple tryMatches
    Diagnostic                diag{node, node->token, Fmt(Err(Err0113), tryMatches.size(), tryMatches[0]->overload->symbol->name.c_str())};
    Vector<const Diagnostic*> notes;
    SemanticError::commonErrorNotes(context, tryMatches, node, &diag, notes);

    cannotMatchIdentifier(context, MatchResult::ValidIfFailed, 0, tryMatches, node, notes);
    cannotMatchIdentifier(context, MatchResult::NotEnoughParameters, 0, tryMatches, node, notes);
    cannotMatchIdentifier(context, MatchResult::TooManyParameters, 0, tryMatches, node, notes);
    cannotMatchIdentifier(context, MatchResult::NotEnoughGenericParameters, 0, tryMatches, node, notes);
    cannotMatchIdentifier(context, MatchResult::TooManyGenericParameters, 0, tryMatches, node, notes);

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
            if (!cannotMatchIdentifier(context, m, paramIdx++, tryMatches, node, notesTmp))
                break;
            notesSig = notesTmp;
        }
        notes.insert(notes.end(), notesSig.begin(), notesSig.end());
    }

    return context->report(diag, notes);
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

    // All errors are because of a const problem on the UFCS argument
    // Then just raise one error
    uint32_t getFlags     = GDFM_ALL;
    int      badConstUfcs = 0;
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

    if (tryMatches.size() == 1)
        return cannotMatchSingle(context, node, tryMatches, getFlags);
    else
        return cannotMatchOverload(context, node, tryMatches);
}

bool SemanticError::ambiguousGenericError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& overloads, VectorNative<OneGenericMatch*>& genericMatches)
{
    auto symbol = overloads[0]->overload->symbol;
    if (!node)
        node = context->node;

    Diagnostic diag{node, node->token, Fmt(Err(Err0115), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};

    Vector<const Diagnostic*> notes;
    for (auto match : genericMatches)
    {
        auto overload = match->symbolOverload;
        auto couldBe  = Fmt("could be of type '%s'", overload->typeInfo->getDisplayNameC());

        VectorNative<TypeInfoParam*> params;
        for (auto og : match->genericReplaceTypes)
        {
            auto p      = g_TypeMgr->makeParam();
            p->name     = og.first.c_str();
            p->typeInfo = og.second.typeInfoReplace;
            params.push_back(p);
        }

        Diagnostic* note  = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        note->canBeMerged = false;
        notes.push_back(note);
    }

    return context->report(diag, notes);
}

bool SemanticError::ambiguousIdentifierError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& overloads, VectorNative<OneMatch*>& matches, uint32_t flags)
{
    auto symbol = overloads[0]->overload->symbol;
    if (!node)
        node = context->node;

    if (flags & MIP_FOR_GHOSTING)
    {
        AstNode*   otherNode = nullptr;
        SymbolKind otherKind = SymbolKind::Invalid;
        for (auto match : matches)
        {
            if (match->symbolOverload->node != node && !match->symbolOverload->node->isParentOf(node))
            {
                otherKind = match->symbolOverload->symbol->kind;
                otherNode = match->symbolOverload->node;
                break;
            }
        }

        SWAG_ASSERT(otherNode);
        return duplicatedSymbolError(context, node->sourceFile, node->token, symbol->kind, symbol->name, otherKind, otherNode);
    }

    Diagnostic diag{node, node->token, Fmt(Err(Err0116), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};

    Vector<const Diagnostic*> notes;
    for (auto match : matches)
    {
        auto        overload = match->symbolOverload;
        Diagnostic* note     = nullptr;

        if (overload->typeInfo->isFuncAttr() && overload->typeInfo->flags & TYPEINFO_FROM_GENERIC)
        {
            auto couldBe = Fmt(Nte(Nte0045), overload->typeInfo->getDisplayNameC());
            note         = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }
        else if (overload->typeInfo->isFuncAttr())
        {
            auto couldBe = Fmt(Nte(Nte0048), overload->typeInfo->getDisplayNameC());
            note         = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }
        else if (overload->typeInfo->isStruct())
        {
            auto couldBe = Fmt(Nte(Nte0049), overload->typeInfo->getDisplayNameC());
            note         = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }
        else
        {
            auto concreteType = TypeManager::concreteType(overload->typeInfo, CONCRETE_ALIAS);
            auto couldBe      = Fmt(Nte(Nte0050), Naming::aKindName(match->symbolOverload).c_str(), concreteType->getDisplayNameC());
            note              = Diagnostic::note(overload->node, overload->node->getTokenName(), couldBe);
        }

        note->canBeMerged = false;
        notes.push_back(note);
    }

    return context->report(diag, notes);
}

bool SemanticError::ambiguousSymbolError(SemanticContext* context, AstIdentifier* identifier, SymbolName* symbol, VectorNative<OneSymbolMatch>& dependentSymbols)
{
    Diagnostic diag{identifier, Fmt(Err(Err0116), Naming::kindName(symbol->kind).c_str(), identifier->token.ctext())};

    Vector<const Diagnostic*> notes;
    for (auto& p1 : dependentSymbols)
    {
        auto note         = Diagnostic::note(p1.symbol->nodes[0], p1.symbol->nodes[0]->token, "could be");
        note->canBeMerged = false;
        notes.push_back(note);
    }

    return context->report(diag, notes);
}