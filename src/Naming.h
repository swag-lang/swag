#pragma once
#include "Scope.h"

struct SymbolName;
struct AstNode;
struct TypeInfo;
struct SymbolOverload;
enum class SymbolKind : uint8_t;

namespace Naming
{
    Utf8 kindName(SymbolName* symbol, AstNode* node, TypeInfo* typeInfo, uint32_t overFlags, Utf8& article);
    Utf8 kindName(SymbolOverload* overload);
    Utf8 aKindName(SymbolOverload* overload);

    Utf8 kindName(SymbolKind kind, Utf8& article);
    Utf8 kindName(SymbolKind kind);
    Utf8 aKindName(SymbolKind kind);

    Utf8 kindName(TypeInfo* typeInfo, Utf8& article);
    Utf8 kindName(TypeInfo* typeInfo);
    Utf8 aKindName(TypeInfo* typeInfo);

    Utf8 kindName(AstNode* node, Utf8& article);
    Utf8 kindName(AstNode* node);
    Utf8 aKindName(AstNode* node);

    Utf8 kindName(ScopeKind kind, Utf8& article);
    Utf8 kindName(ScopeKind kind);
    Utf8 aKindName(ScopeKind kind);

    Utf8 niceArgumentRank(size_t idx);
    Utf8 niceParameterRank(size_t idx);

    Utf8 tokenToName(TokenId id);
}; // namespace Naming
