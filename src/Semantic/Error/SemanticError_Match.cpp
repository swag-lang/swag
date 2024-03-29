#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/Naming.h"
#include "Syntax/SyntaxColor.h"

namespace
{
    bool cannotMatchIdentifier(SemanticContext*            context,
                               const MatchResult           result,
                               uint32_t                    paramIdx,
                               VectorNative<OneTryMatch*>& tryMatches,
                               AstNode*                    node,
                               Vector<const Diagnostic*>&  notes,
                               int&                        overloadIndex)
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

        static constexpr int MAX_OVERLOADS = 5;
        Diagnostic*          note          = nullptr;

        switch (result)
        {
            case MatchResult::ValidIfFailed:
                note = Diagnostic::note(node, node->token, "all [[#validif]] have failed");
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
                    note = Diagnostic::note(node, node->token, form("the %s does not match", Naming::niceArgumentRank(paramIdx + 1).c_str()));
                break;
            case MatchResult::BadGenericSignature:
                note = Diagnostic::note(node, node->token, form("the generic %s does not match", Naming::niceArgumentRank(paramIdx + 1).c_str()));
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }

        const auto maxOverloads = min(tryResult.size(), MAX_OVERLOADS);

        // Additional error message per overload
        Vector<Utf8> addMsg;
        for (uint32_t i = 0; i < maxOverloads; i++)
        {
            Vector<const Diagnostic*> errs0, errs1;
            Vector<Utf8>              parts;

            SemanticError::getDiagnosticForMatch(context, *tryResult[i], errs0, errs1);
            Diagnostic::tokenizeError(errs0[0]->textMsg, parts);
            SWAG_ASSERT(parts.size() > 1);
            addMsg.push_back(parts[1]);
        }

        // Determine if all "per function" errors are the same
        bool allAddMsgAreEquals = true;
        for (uint32_t i = 1; i < maxOverloads; i++)
        {
            if (addMsg[i] != addMsg[0])
                allAddMsgAreEquals = false;
        }

        // Replace the generic error message with the most specific one, if all specifics are equals
        if (result == MatchResult::BadSignature || result == MatchResult::BadGenericSignature)
        {
            if (allAddMsgAreEquals)
                note->textMsg = addMsg[0];
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
                const auto lambda  = castAst<AstTypeLambda>(varNode->typeInfo->declNode, AstNodeKind::TypeLambda);
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
            n = syntaxColor(n, cxt);

            note->preRemarks.push_back(form("overload %d: %s", overloadIndex++, n.c_str()));

            // Additional (more precise) information in case of bad signature
            if (result == MatchResult::BadSignature || result == MatchResult::BadGenericSignature)
            {
                if (!allAddMsgAreEquals)
                {
                    Utf8 msg = " => ";
                    msg += addMsg[i];
                    note->preRemarks.push_back(msg);
                }
            }
        }

        if (tryResult.size() > MAX_OVERLOADS)
            note->preRemarks.push_back("...");

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

    bool cannotMatchOverload(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches)
    {
        // Multiple tryMatches
        Diagnostic                err{node, node->token, formErr(Err0613, tryMatches.size(), tryMatches[0]->overload->symbol->name.c_str())};
        Vector<const Diagnostic*> notes;
        SemanticError::commonErrorNotes(context, tryMatches, node, &err, notes);

        int overloadIndex = 1;
        cannotMatchIdentifier(context, MatchResult::ValidIfFailed, 0, tryMatches, node, notes, overloadIndex);
        cannotMatchIdentifier(context, MatchResult::NotEnoughParameters, 0, tryMatches, node, notes, overloadIndex);
        cannotMatchIdentifier(context, MatchResult::TooManyParameters, 0, tryMatches, node, notes, overloadIndex);
        cannotMatchIdentifier(context, MatchResult::NotEnoughGenericParameters, 0, tryMatches, node, notes, overloadIndex);
        cannotMatchIdentifier(context, MatchResult::TooManyGenericParameters, 0, tryMatches, node, notes, overloadIndex);

        // For a bad signature, only show the ones with the greatest match
        for (int what = 0; what < 2; what++)
        {
            Vector<const Diagnostic*> notesSig;
            int                       paramIdx = 0;
            while (true)
            {
                Vector<const Diagnostic*> notesTmp;
                const auto                m = what == 0 ? MatchResult::BadSignature : MatchResult::BadGenericSignature;
                if (!cannotMatchIdentifier(context, m, paramIdx++, tryMatches, node, notesTmp, overloadIndex))
                    break;
                notesSig = notesTmp;
            }

            notes.insert(notes.end(), notesSig.begin(), notesSig.end());
        }

        return context->report(err, notes);
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
                case MatchResult::DuplicatedNamedParameter:
                case MatchResult::InvalidNamedParameter:
                case MatchResult::MissingNamedParameter:
                case MatchResult::MissingParameters:
                case MatchResult::NotEnoughParameters:
                case MatchResult::TooManyParameters:
                case MatchResult::ValidIfFailed:
                case MatchResult::NotEnoughGenericParameters:
                case MatchResult::CannotDeduceGenericType:
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
                // has been used to find the lambda and UFCS, or do we consider the struct is just there to find the lambda ?
                // We consider that UFCS of 'struct.lambda.call(struct)' has less priority than other errors
                else if (oneMatch->ufcs && oneMatch->scope && oneMatch->scope->is(ScopeKind::Struct) && oneMatch->overload->symbol->is(SymbolKind::Variable))
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

    // Take validif if failed in priority
    {
        Vector<OneTryMatch*> n;
        for (auto oneMatch : tryMatches)
        {
            const auto& one = *oneMatch;
            if (one.symMatchContext.result == MatchResult::ValidIfFailed)
                n.push_back(oneMatch);
        }
        if (!n.empty())
            tryMatches = n;
    }

    if (tryMatches.size() == 1)
        return cannotMatchSingle(context, node, tryMatches);
    return cannotMatchOverload(context, node, tryMatches);
}
