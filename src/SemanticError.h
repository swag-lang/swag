#pragma once
#include "Symbol.h"
#include "TypeMatch.h"

struct AlternativeScope;
struct AstIdentifier;
struct AstIdentifierRef;
struct AstNode;
struct ByteCode;
struct Diagnostic;
struct ErrorContext;
struct Module;
struct OneMatch;
struct OneSymbolMatch;
struct OneTryMatch;
struct Scope;
struct SemanticContext;
struct SourceFile;
struct SymbolName;
struct Token;

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

    void getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, Vector<const Diagnostic*>& diagError, Vector<const Diagnostic*>& diagNote);

    bool cannotMatchIdentifierError(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node);
    bool ambiguousGenericError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& overloads, VectorNative<OneMatch*>& genericMatches);
    bool ambiguousOverloadError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& overloads, VectorNative<OneMatch*>& matches, uint32_t flags);
    bool ambiguousSymbolError(SemanticContext* context, AstIdentifier* identifier, SymbolName* symbol, VectorNative<OneSymbolMatch>& dependentSymbols);

    void unknownIdentifierError(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node);

    void commonErrorNotes(SemanticContext* context, const VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* diag, Vector<const Diagnostic*>& notes);
    bool notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg = nullptr, AstNode* hintType = nullptr);
    bool duplicatedSymbolError(ErrorContext* context,
                               SourceFile*   sourceFile,
                               Token&        token,
                               SymbolKind    thisKind,
                               const Utf8&   thisName,
                               SymbolKind    otherKind,
                               AstNode*      otherSymbolDecl);
    bool error(SemanticContext* context, const Utf8& msg);
} // namespace SemanticError
