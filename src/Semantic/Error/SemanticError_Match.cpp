#include <algorithm>

#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/SyntaxColor.h"
#pragma optimize("", off)

namespace
{
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

    // Column cap for the signature before " -- <detail>" starts.
    // Long signatures are middle-ellipsized to fit within this cap.
    bool cannotMatchOverload(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches)
    {
        // No candidates at all: fall back to a generic overload error
        if (tryMatches.empty())
        {
            Diagnostic err{node, node->token, toErr(Err0506)};
            return context->report(err, {});
        }

        static constexpr uint32_t SIG_COL_MAX   = 64;
        static constexpr auto     OVER_SEP      = " -- ";
        static constexpr uint32_t MAX_OVERLOADS = 5;

        // Primary error (kept unless all per-overload reasons are identical)
        Diagnostic                err{node, node->token, toErr(Err0506)};
        Vector<const Diagnostic*> notes;
        SemanticError::commonErrorNotes(context, tryMatches, node, &err, notes);

        // Single note that will carry ALL overloads as remarks
        Diagnostic* note = Diagnostic::note(node, node->token, "");

        // Anchor diagnostics to first candidate
        Vector<const Diagnostic*> firstErrs, firstNotes;
        SemanticError::getDiagnosticForMatch(context, *tryMatches[0], firstErrs, firstNotes);
        SWAG_ASSERT(!firstErrs.empty());
        note->sourceFile    = firstErrs[0]->sourceFile;
        note->startLocation = firstErrs[0]->startLocation;
        note->endLocation   = firstErrs[0]->endLocation;
        note->canBeMerged   = true;

        struct Item
        {
            Utf8     sigPlain;   // one-lined and (possibly) truncated, uncolored
            Utf8     sigColored; // same content as sigPlain, but syntax-colored
            Utf8     detail;     // tokens[1] (or full message if no split)
            uint32_t widthPlain;
        };
        Vector<Item> items;

        const uint32_t shown = std::min<uint32_t>(tryMatches.size(), MAX_OVERLOADS);

        FormatAst     fmtAst;
        FormatContext fmtContext;

        // Track reason equality using string IDs (Utf8) from Diagnostic::getErrorId
        bool haveFirst = false;
        Utf8 firstReasonId;
        Utf8 firstReasonDetail;
        bool allEqualIds     = true;
        bool allEqualDetails = true;

        // First pass: collect data & compute maximum signature width (after truncation)
        uint32_t maxSigWidth = 0;

        for (uint32_t i = 0; i < shown; i++)
        {
            OneTryMatch* match = tryMatches[i];

            // 1) Produce the raw signature
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

            // Strip trailing newlines/semicolons/whitespace
            Utf8 sigRaw = fmtAst.getUtf8();
            while (!sigRaw.empty() && (sigRaw.back() == '\n' || sigRaw.back() == ';'))
                sigRaw.removeBack();
            sigRaw.trimRight();

            // Normalize to one line (for measuring & alignment)
            Utf8 sigPlain = Utf8::oneLine(sigRaw);

            // 2) Extract the per-overload detail (token[1]); reason key for equality
            Vector<const Diagnostic*> errs0, notes0;
            SemanticError::getDiagnosticForMatch(context, *match, errs0, notes0);

            Utf8 detailMsg;
            Utf8 reasonId;

            auto extract = [&](const Utf8& full) {
                Vector<Utf8> tokens;
                Diagnostic::tokenizeError(full, tokens);
                detailMsg = tokens.size() >= 2 ? tokens[1] : full;
                reasonId  = Diagnostic::getErrorId(full);
            };

            if (!errs0.empty())
                extract(errs0[0]->textMsg);
            else if (!notes0.empty())
                extract(notes0[0]->textMsg);

            if (!haveFirst)
            {
                firstReasonId     = reasonId;
                firstReasonDetail = detailMsg;
                haveFirst         = true;
            }
            else
            {
                if (reasonId != firstReasonId)
                    allEqualIds = false;
                if (detailMsg != firstReasonDetail)
                    allEqualDetails = false;
            }

            // 3) Truncate the plain signature for a consistent column
            Utf8 sigLimited = (sigPlain.length() > SIG_COL_MAX) ? Utf8::ellipsizeMiddle(sigPlain, SIG_COL_MAX) : sigPlain;

            // 4) Colorize AFTER limiting so colored content matches measured width
            SyntaxColorContext cxt;
            Utf8               sigColored = doSyntaxColor(sigLimited, cxt);

            // 5) Measure the width of the limited plain signature for padding
            const uint32_t w = sigLimited.length();
            maxSigWidth      = std::max(w, maxSigWidth);

            Item it;
            it.sigPlain   = std::move(sigLimited);
            it.sigColored = std::move(sigColored);
            it.detail     = std::move(detailMsg);
            it.widthPlain = w;
            items.push_back(std::move(it));
        }

        // Details start one space after the widest shown signature
        const uint32_t detailCol = (maxSigWidth < SIG_COL_MAX ? maxSigWidth : SIG_COL_MAX) + 1;

        // Second pass: render aligned lines
        uint32_t overloadIndex = 1;
        for (const auto& it : items)
        {
            Utf8 line;
            line += Log::colorToVTS(LogColor::DarkYellow);
            line += form("overload %u", overloadIndex++);
            line += Log::colorToVTS(LogColor::White);
            line += ": ";
            line += it.sigColored;

            // Pad with spaces based on PLAIN width so VT codes don't skew alignment
            uint32_t pad = 1; // at least one space before separator
            if (it.widthPlain + 1 <= detailCol)
                pad = detailCol - it.widthPlain;

            for (uint32_t k = 0; k < pad; k++)
                line += ' ';

            if(!allEqualDetails)
            {
                if (!it.detail.empty())
                {
                    line += OVER_SEP;
                    line += it.detail;
                }
            }

            note->remarks.push_back(std::move(line));
        }

        if (tryMatches.size() > shown)
            note->remarks.push_back(form("... and %u more overload(s) not shown", tryMatches.size() - shown));

        if (allEqualDetails)
        {
            err.remarks.push_back(firstReasonDetail);
        }
        
        // If ALL per-overload reasons are identical, set the main error text to the TITLE (token[0]) of the first error
        if (allEqualIds && !firstErrs.empty())
        {
            Vector<Utf8> tokens;
            Diagnostic::tokenizeError(firstErrs[0]->textMsg, tokens);
            err.textMsg = !tokens.empty() ? tokens[0] : firstErrs[0]->textMsg;

            // Anchor main error to first candidate error for accuracy
            err.sourceFile    = firstErrs[0]->sourceFile;
            err.startLocation = firstErrs[0]->startLocation;
            err.endLocation   = firstErrs[0]->endLocation;
        }

        Vector<const Diagnostic*> oneNote{note};
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
