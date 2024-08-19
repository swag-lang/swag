#pragma once
#include "Semantic/Symbol/Symbol.h"

struct AstStruct;
struct AstAttrDecl;
struct AstTypeLambda;
struct AstFuncDecl;
struct AstFuncCallParam;
struct BadSignatureInfos;
struct CollectedScope;
struct AstIdentifier;
struct AstIdentifierRef;
struct AstIf;
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

using MatchIdParamsFlags = Flags<uint32_t>;

enum class IdentifierSearchFor
{
    Whatever,
    Type,
    Function,
    Attribute,
    Keyword,
    TopLevelInstruction,
    Struct,
    Enum,
    Variable,
};

struct ErrorParam
{
    OneTryMatch*               oneTry;
    Vector<const Diagnostic*>* diagError;
    Vector<const Diagnostic*>* diagNote;

    AstNode*           errorNode             = nullptr;
    AstFuncCallParam*  failedParam           = nullptr;
    uint32_t           badParamIdx           = 0;
    AstFuncDecl*       destFuncDecl          = nullptr;
    AstTypeLambda*     destLambdaDecl        = nullptr;
    AstAttrDecl*       destAttrDecl          = nullptr;
    AstStruct*         destStructDecl        = nullptr;
    AstNode*           destParameters        = nullptr;
    AstNode*           destGenericParameters = nullptr;
    BadSignatureInfos* bi                    = nullptr;
    Utf8               explicitCastMsg;

    void addError(const Diagnostic* note) const;
    void addNote(Diagnostic* note, bool addGeneric = true) const;
};

namespace SemanticError
{
    Utf8 findClosestMatchesMsg(const Utf8& searchName, const Vector<Utf8>& best);
    void findClosestMatches(const Utf8& searchName, const Vector<Utf8>& searchList, Vector<Utf8>& result);
    void findClosestMatches(const Utf8& searchName, const VectorNative<CollectedScope>& scopeHierarchy, Vector<Utf8>& best, IdentifierSearchFor searchFor);
    Utf8 findClosestMatchesMsg(const Utf8& searchName, const VectorNative<CollectedScope>& scopeHierarchy, IdentifierSearchFor searchFor);

    bool warnUnusedFunction(const Module* moduleToGen, const ByteCode* one);
    bool warnUnusedVariables(SemanticContext* context, const Scope* scope);
    bool warnUnreachableCode(SemanticContext* context);
    bool warnWhereDoIf(SemanticContext* context);
    bool warnElseDoIf(SemanticContext* context, const AstIf* ifNode);
    bool warnDeprecated(SemanticContext* context, AstNode* identifier);

    void getDiagnosticForMatch(SemanticContext* context, OneTryMatch& oneTry, Vector<const Diagnostic*>& diagError, Vector<const Diagnostic*>& diagNote);

    bool cannotMatchIdentifierError(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node);
    void ambiguousArguments(SemanticContext* context, Diagnostic& err, VectorNative<OneMatch*>& matches);
    bool ambiguousGenericError(SemanticContext* context, AstNode* node, VectorNative<OneTryMatch*>& tryMatches, VectorNative<OneMatch*>& matches);
    bool ambiguousOverloadError(SemanticContext* context, AstNode* node, VectorNative<OneMatch*>& matches, MatchIdParamsFlags flags);
    bool ambiguousSymbolError(SemanticContext* context, AstIdentifier* identifier, const SymbolName* symbol, VectorNative<OneSymbolMatch>& matches);

    bool unknownIdentifierError(SemanticContext* context, const AstIdentifierRef* identifierRef, AstIdentifier* identifier);

    void errorWhereFailed(SemanticContext*, const ErrorParam& errorParam);
    void commonErrorNotes(SemanticContext* context, const VectorNative<OneTryMatch*>& tryMatches, AstNode* node, Diagnostic* err, Vector<const Diagnostic*>& notes);
    bool notAllowedError(ErrorContext* context, AstNode* node, TypeInfo* typeInfo, const char* msg = nullptr, const AstNode* hintType = nullptr);
    bool duplicatedSymbolError(ErrorContext* context, SourceFile* sourceFile, const Token& token, SymbolKind thisKind, const Utf8& thisName, SymbolKind otherKind, AstNode* otherSymbolDecl);
    bool error(SemanticContext* context, const Utf8& msg);
}
