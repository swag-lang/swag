#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/Naming.h"
#include "Syntax/SyntaxColor.h"
#pragma optimize("", off)

namespace
{
    bool cannotMatchIdentifier(SemanticContext* context, MatchResult result, uint32_t paramIdx, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Vector<const Diagnostic*>& notes, int& overloadIndex)
    {
        if (tryMatches.empty())
            return false;

        // Filter matches depending on argument
        bool                       hasCorrectResult = false;
        VectorNative<OneTryMatch*> tryResult;
        for (uint32_t i = 0; i < tryMatches.size(); i++)
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

        static constexpr uint32_t MAX_OVERLOADS = 5;
        Diagnostic*               note          = nullptr;

        switch (result)
        {
            case MatchResult::WhereFailed:
                note = Diagnostic::note(node, node->token, "all [[where]] constraints have failed");
                break;
            case MatchResult::TooManyArguments:
                note = Diagnostic::note(node, node->token, "too many arguments");
                break;
            case MatchResult::TooManyGenericArguments:
                note = Diagnostic::note(node, node->token, "too many generic arguments");
                break;
            case MatchResult::NotEnoughArguments:
                note = Diagnostic::note(node, node->token, "not enough arguments");
                break;
            case MatchResult::NotEnoughGenericArguments:
                note = Diagnostic::note(node, node->token, "not enough generic arguments");
                break;
            case MatchResult::InvalidNamedArgument:
                note = Diagnostic::note(node, node->token, "invalid named argument");
                break;
            case MatchResult::BadSignature:
            {
                const auto badType = tryResult[0]->symMatchContext.badSignatureInfos.badSignatureGivenType->getDisplayName();
                Utf8       n;
                if (tryResult[0]->ufcs && paramIdx == 0)
                    n = form("the UFCS argument does not match (type is [[%s]])", badType.cstr());
                else if (tryResult[0]->callParameters && tryResult[0]->callParameters->childCount() == 1)
                    n = form("the argument does not match (type is [[%s]])", badType.cstr());
                else
                    n = form("the %s does not match (type is [[%s]])", Naming::niceArgumentRank(paramIdx + 1).cstr(), badType.cstr());
                note = Diagnostic::note(node, node->token, n);
                break;
            }
            case MatchResult::BadGenericSignature:
            {
                const auto badType = tryResult[0]->symMatchContext.badSignatureInfos.badSignatureGivenType->getDisplayName();
                Utf8       n;
                if (tryResult[0]->genericParameters && tryResult[0]->genericParameters->childCount() == 1)
                    n = form("the generic argument does not match (type is [[%s]])", badType.cstr());
                else
                    n = form("the generic %s does not match (type is [[%s]])", Naming::niceArgumentRank(paramIdx + 1).cstr(), badType.cstr());
                note = Diagnostic::note(node, node->token, n);
                break;
            }
            default:
                SWAG_ASSERT(false);
                break;
        }

        const auto maxOverloads = std::min(tryResult.size(), MAX_OVERLOADS);

        // Additional error message per overload
        Vector<Utf8> addMsg;
        for (uint32_t i = 0; i < maxOverloads; i++)
        {
            Vector<const Diagnostic*> errs0, notes0;
            SemanticError::getDiagnosticForMatch(context, *tryResult[i], errs0, notes0);
            addMsg.push_back(errs0[0]->textMsg);
        }

        // Determine if all "per overload" errors are the same
        bool allAddMsgAreEquals = true;
        for (uint32_t i = 1; i < maxOverloads; i++)
        {
            if (addMsg[i] != addMsg[0])
            {
                allAddMsgAreEquals = false;
                break;
            }
        }

        // Replace the generic error message with the most specific one, if all specifics are equals
        if (result == MatchResult::BadSignature || result == MatchResult::BadGenericSignature)
        {
            if (allAddMsgAreEquals)
            {
                note->textMsg = addMsg[0];
            }
        }

        FormatAst     fmtAst;
        FormatContext fmtContext;
        for (uint32_t i = 0; i < maxOverloads; i++)
        {
            // Output the function signature
            fmtAst.clear();
            if (tryResult[i]->overload->node->is(AstNodeKind::FuncDecl))
            {
                const auto funcNode = castAst<AstFuncDecl>(tryResult[i]->overload->node, AstNodeKind::FuncDecl);
                fmtAst.outputFuncSignature(fmtContext, funcNode, funcNode->genericParameters, funcNode->parameters, nullptr);
            }
            else if (tryResult[i]->overload->node->is(AstNodeKind::VarDecl))
            {
                const auto varNode = castAst<AstVarDecl>(tryResult[i]->overload->node, AstNodeKind::VarDecl);
                const auto lambda  = castAst<AstTypeExpression>(varNode->typeInfo->declNode, AstNodeKind::TypeLambda);
                fmtAst.outputFuncSignature(fmtContext, varNode, nullptr, lambda->parameters, nullptr);
            }
            else
            {
                SWAG_ASSERT(false);
            }

            auto n = fmtAst.getUtf8();
            if (n.back() == '\n')
                n.count--;
            if (n.back() == ';')
                n.count--;

            SyntaxColorContext cxt;
            n = doSyntaxColor(n, cxt);

            Utf8 overTxt;
            overTxt += Log::colorToVTS(LogColor::DarkYellow);
            overTxt += form("overload %d", overloadIndex++);
            overTxt += Log::colorToVTS(LogColor::White);
            overTxt += ": ";
            overTxt += n;

            // Additional (more precise) information in case of bad signature
            if (result == MatchResult::BadSignature || result == MatchResult::BadGenericSignature)
            {
                if (!allAddMsgAreEquals)
                {
                    overTxt += " -- ";
                    Vector<Utf8> tokens;
                    Diagnostic::tokenizeError(addMsg[i], tokens);
                    int d = tokens[1].find(", got");
                    if (d != -1)
                        tokens[1].remove(d, tokens[1].length() - d);
                    overTxt += tokens[1];
                }
            }

            note->preRemarks.push_back(overTxt);
        }

        if (tryResult.size() > MAX_OVERLOADS)
            note->preRemarks.push_back("...");

        // Locate to the first error
        Vector<const Diagnostic*> errs0, notes0;
        SemanticError::getDiagnosticForMatch(context, *tryResult[0], errs0, notes0);
        note->sourceFile    = errs0[0]->sourceFile;
        note->startLocation = errs0[0]->startLocation;
        note->endLocation   = errs0[0]->endLocation;
        note->canBeMerged   = false;
        notes.push_back(note);

        return true;
    }

    bool cannotMatchSingle(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches)
    {
        // Be sure this is not because of an invalid special function signature
        if (tryMatches[0]->overload->node->is(AstNodeKind::FuncDecl))
            SWAG_CHECK(Semantic::checkFuncPrototype(context, castAst<AstFuncDecl>(tryMatches[0]->overload->node, AstNodeKind::FuncDecl)));

        Vector<const Diagnostic*> errs0, errs1;
        SemanticError::getDiagnosticForMatch(context, *tryMatches[0], errs0, errs1);
        SWAG_ASSERT(!errs0.empty());
        SemanticError::commonErrorNotes(context, tryMatches, node, const_cast<Diagnostic*>(errs0[0]), errs1);
        return context->report(*errs0[0], errs1);
    }

    constexpr uint32_t MAX_OVERLOADS = 5;

    bool cannotMatchOverload(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches)
    {
        // Primary error (kept unless all per-overload reasons are identical)
        Diagnostic                err{node, node->token, toErr(Err0506)};
        Vector<const Diagnostic*> notes;
        SemanticError::commonErrorNotes(context, tryMatches, node, &err, notes);

        // Single note that will carry ALL overloads as remarks
        Diagnostic* note = Diagnostic::note(node, node->token, "candidate overloads");

        // Anchor diagnostics to first candidate
        Vector<const Diagnostic*> firstErrs, firstNotes;
        SemanticError::getDiagnosticForMatch(context, *tryMatches[0], firstErrs, firstNotes);
        SWAG_ASSERT(!firstErrs.empty());
        note->sourceFile    = firstErrs[0]->sourceFile;
        note->startLocation = firstErrs[0]->startLocation;
        note->endLocation   = firstErrs[0]->endLocation;
        note->canBeMerged   = true;

        // Collect overload lines + track whether all short reasons are the same
        int        overloadIndex = 1;
        const auto maxOverloads  = std::min<uint32_t>(tryMatches.size(), MAX_OVERLOADS);

        FormatAst     fmtAst;
        FormatContext fmtContext;

        Utf8 firstShortReason;
        bool haveFirstShort  = false;
        bool allReasonsEqual = true;

        for (uint32_t i = 0; i < maxOverloads; i++)
        {
            OneTryMatch* match = tryMatches[i];

            // 1) Pretty-print the candidate signature
            fmtAst.clear();
            if (match->overload->node->is(AstNodeKind::FuncDecl))
            {
                const auto funcNode = castAst<AstFuncDecl>(match->overload->node, AstNodeKind::FuncDecl);
                fmtAst.outputFuncSignature(fmtContext, funcNode, funcNode->genericParameters, funcNode->parameters, nullptr);
            }
            else if (match->overload->node->is(AstNodeKind::VarDecl))
            {
                const auto varNode = castAst<AstVarDecl>(match->overload->node, AstNodeKind::VarDecl);
                const auto lambda  = castAst<AstTypeExpression>(varNode->typeInfo->declNode, AstNodeKind::TypeLambda);
                fmtAst.outputFuncSignature(fmtContext, varNode, nullptr, lambda->parameters, nullptr);
            }
            else
            {
                SWAG_ASSERT(false);
            }

            Utf8 sig = fmtAst.getUtf8();
            if (!sig.empty() && (sig.back() == '\n' || sig.back() == ';'))
                sig.count--;
            SyntaxColorContext cxt;
            sig = doSyntaxColor(sig, cxt);

            // 2) Get the specific error for THIS overload
            Vector<const Diagnostic*> errs0, notes0;
            SemanticError::getDiagnosticForMatch(context, *match, errs0, notes0);

            Utf8 shortMsg;
            if (!errs0.empty())
            {
                Vector<Utf8> tokens;
                Diagnostic::tokenizeError(errs0[0]->textMsg, tokens);
                if (tokens.size() > 1)
                    shortMsg = tokens[1];
                else
                    shortMsg = errs0[0]->textMsg;

                // Keep output compact
                int d = shortMsg.find(", got");
                if (d != -1)
                    shortMsg.remove(d, shortMsg.length() - d);

                // Track equality across all overload reasons
                if (!haveFirstShort)
                {
                    firstShortReason = shortMsg;
                    haveFirstShort   = true;
                }
                else if (shortMsg != firstShortReason)
                {
                    allReasonsEqual = false;
                }
            }

            // 3) Compose: "overload N: <sig> -- <shortMsg>"
            Utf8 overTxt;
            overTxt += Log::colorToVTS(LogColor::DarkYellow);
            overTxt += form("overload %d", overloadIndex++);
            overTxt += Log::colorToVTS(LogColor::White);
            overTxt += ": ";
            overTxt += sig;

            if (!shortMsg.empty())
            {
                overTxt += " -- ";
                overTxt += shortMsg;
            }

            note->preRemarks.push_back(overTxt);
        }

        if (tryMatches.size() > MAX_OVERLOADS)
            note->preRemarks.push_back("...");

        // Only adjust the main error text if ALL per-overload short reasons are identical.
        // Otherwise keep the original main error message verbatim.
        if (allReasonsEqual && !firstErrs.empty())
        {
            Vector<Utf8> tokens;
            Diagnostic::tokenizeError(firstErrs[0]->textMsg, tokens);
            if (!tokens.empty())
                err.textMsg = tokens[0];
        }

        // Render the overload list
        note->remarks = std::move(note->preRemarks);

        Vector<const Diagnostic*> oneNote;
        oneNote.push_back(note);
        return context->report(err, oneNote);
    }
}

bool SemanticError::cannotMatchIdentifierError(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node)
{
    const AstNode* genericParameters = nullptr;

    // node can be null when we try to resolve a userOp
    if (node && node->is(AstNodeKind::Identifier))
    {
        const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        genericParameters     = identifier->genericParameters;
    }
    else
        node = context->node;

    // Take non generic errors in priority
    {
        Vector<OneTryMatch*> n;
        for (auto oneMatch : tryMatches)
        {
            const auto& one = *oneMatch;
            switch (one.symMatchContext.result)
            {
                case MatchResult::BadSignature:
                case MatchResult::DuplicatedNamedArgument:
                case MatchResult::InvalidNamedArgument:
                case MatchResult::MissingNamedArgument:
                case MatchResult::MissingArguments:
                case MatchResult::NotEnoughArguments:
                case MatchResult::TooManyArguments:
                case MatchResult::WhereFailed:
                case MatchResult::NotEnoughGenericArguments:
                    n.push_back(oneMatch);
                    break;
                default:
                    break;
            }
        }
        if (!n.empty())
            tryMatches = n;
    }

    // Remove duplicates overloads, as we can have one with UFCS, and one without, as we
    // could have tried both
    {
        Vector<OneTryMatch*> n;
        for (auto oneMatch : tryMatches)
        {
            for (const auto oneMatch1 : tryMatches)
            {
                if (oneMatch == oneMatch1)
                    continue;
                if (oneMatch->overload != oneMatch1->overload)
                    continue;

                // If the UFCS version has matched the UFCS parameter, then take that one
                if (oneMatch->ufcs && oneMatch->symMatchContext.badSignatureInfos.badSignatureParameterIdx > 0)
                    oneMatch1->overload = nullptr;

                    // If the UFCS has failed because of const, takes the UFCS error
                else if (oneMatch->ufcs && oneMatch->symMatchContext.badSignatureInfos.badSignatureParameterIdx == 0 && oneMatch->symMatchContext.badSignatureInfos.castErrorType == CastErrorType::Const)
                    oneMatch1->overload = nullptr;

                    // If this is a lambda call that comes from a struct, then this is ambiguous. Do we keep the error where the struct
                    // has been used to find the lambda and UFCS, or do we consider the struct is just there to find the lambda?
                    // We consider that UFCS of 'struct.lambda.call(struct)' has less priority than other errors
                else if (oneMatch->ufcs && oneMatch->scope && oneMatch->scope->is(ScopeKind::Struct) && oneMatch->overload->symbol->is(SymbolKind::Variable))
                    oneMatch->overload = nullptr;

                    // Otherwise, if with UFCS we do not have enough argument, we use UFCS in priority
                else if (oneMatch->ufcs && oneMatch1->symMatchContext.result == MatchResult::NotEnoughArguments)
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
            const auto& one = *oneMatch;
            if (!one.overload->hasFlag(OVERLOAD_GENERIC))
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
            const auto& one = *oneMatch;
            if (one.overload->hasFlag(OVERLOAD_GENERIC))
                n.push_back(oneMatch);
        }
        if (!n.empty())
            tryMatches = n;
    }

    // Take where if failed in priority
    {
        Vector<OneTryMatch*> n;
        for (auto oneMatch : tryMatches)
        {
            const auto& one = *oneMatch;
            if (one.symMatchContext.result == MatchResult::WhereFailed)
                n.push_back(oneMatch);
        }
        if (!n.empty())
            tryMatches = n;
    }

    if (tryMatches.size() == 1)
        return cannotMatchSingle(context, node, tryMatches);
    return cannotMatchOverload(context, node, tryMatches);
}
