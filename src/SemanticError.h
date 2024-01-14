#pragma once
#include "TypeMatch.h"
#include "Symbol.h"

struct AlternativeScope;
struct AstIdentifier;
struct AstIdentifierRef;
struct AstNode;
struct ByteCode;

struct Diagnostic;
struct ErrorContext;
struct Module;
struct OneTryMatch;
struct Scope;
struct SemanticContext;
struct SourceFile;
struct Token;

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

    bool warnUnusedFunction(Module* moduleToGen, ByteCode* one);
    bool warnUnusedVariables(SemanticContext* context, Scope* scope);
    bool warnUnreachableCode(SemanticContext* context);
    bool warnDeprecated(SemanticContext* context, AstNode* identifier);

    void getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, Vector<const Diagnostic*>& result0, Vector<const Diagnostic*>& result1, uint32_t getFlags = GDFM_ALL);

    bool cannotMatchIdentifierError(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node);

    void unknownIdentifierError(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node);

    void symbolErrorNotes(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag, Vector<const Diagnostic*>& notes);
    void symbolErrorRemarks(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag);
    bool notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg = nullptr, AstNode* hintType = nullptr);
    bool duplicatedSymbolError(ErrorContext* context, SourceFile* sourceFile, Token& token, SymbolKind thisKind, const Utf8& thisName, SymbolKind otherKind, AstNode* otherSymbolDecl);
    bool error(SemanticContext* context, const Utf8& msg);
} // namespace SemanticError
