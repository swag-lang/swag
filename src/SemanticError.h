#pragma once
#include "TypeMatch.h"
#include "Symbol.h"

struct SemanticContext;
struct OneTryMatch;
struct Diagnostic;
struct AstNode;
struct AlternativeScope;
struct SourceFile;
struct ErrorContext;
struct Token;
struct AstIdentifierRef;
struct AstIdentifier;

const uint32_t GDFM_HERE_IS = 0x00000001;
const uint32_t GDFM_ALL     = 0xFFFFFFFF;

enum class IdentifierSearchFor
{
    Whatever,
    Type,
    Function,
    Attribute,
    Keyword,
    TopLevelInstruction,
    Struct,
};

namespace SemanticError
{
    Utf8 findClosestMatchesMsg(const Utf8& searchName, const Vector<Utf8>& best);
    void findClosestMatches(const Utf8& searchName, const Vector<Utf8>& searchList, Vector<Utf8>& result);
    void findClosestMatches(const Utf8& searchName, const VectorNative<AlternativeScope>& scopeHierarchy, Vector<Utf8>& best, IdentifierSearchFor searchFor);
    Utf8 findClosestMatchesMsg(const Utf8& searchName, const VectorNative<AlternativeScope>& scopeHierarchy, IdentifierSearchFor searchFor);

    void getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, Vector<const Diagnostic*>& result0, Vector<const Diagnostic*>& result1, uint32_t getFlags = GDFM_ALL);
    void symbolErrorNotes(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag, Vector<const Diagnostic*>& notes);
    void symbolErrorRemarks(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag);
    bool cannotMatchIdentifierError(SemanticContext* context, MatchResult result, int paramIdx, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Vector<const Diagnostic*>& notes);
    bool cannotMatchIdentifierError(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node);
    void unknownIdentifier(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node);
    bool notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg = nullptr, AstNode* hintType = nullptr);
    bool duplicatedSymbolError(ErrorContext* context, SourceFile* sourceFile, Token& token, SymbolKind thisKind, const Utf8& thisName, SymbolKind otherKind, AstNode* otherSymbolDecl);
    bool error(SemanticContext* context, const Utf8& msg);
} // namespace SemanticError
