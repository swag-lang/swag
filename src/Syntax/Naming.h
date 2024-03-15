#pragma once

struct AstFuncDecl;
struct AstNode;
struct SymbolName;
struct SymbolOverload;
struct TypeInfo;
using OverloadFlags = Flags<uint32_t>;
enum class SymbolKind : uint8_t;
enum class TokenId : uint16_t;
enum class ScopeKind : uint8_t;

namespace Naming
{
    Utf8 kindName(const SymbolName* symbol, const AstNode* node, const TypeInfo* typeInfo, OverloadFlags overFlags, Utf8& article);
    Utf8 kindName(const SymbolOverload* overload);
    Utf8 aKindName(const SymbolOverload* overload);

    Utf8 kindName(SymbolKind kind, Utf8& article);
    Utf8 kindName(SymbolKind kind);
    Utf8 aKindName(SymbolKind kind);

    Utf8 kindName(const TypeInfo* typeInfo, Utf8& article);
    Utf8 kindName(const TypeInfo* typeInfo);
    Utf8 aKindName(const TypeInfo* typeInfo);

    Utf8 kindName(const AstNode* node, Utf8& article);
    Utf8 kindName(const AstNode* node);
    Utf8 aKindName(const AstNode* node);

    Utf8 kindName(ScopeKind kind, Utf8& article);
    Utf8 kindName(ScopeKind kind);
    Utf8 aKindName(ScopeKind kind);

    Utf8 niceArgumentRank(uint32_t idx);
    Utf8 niceParameterRank(uint32_t idx);

    Utf8 tokenToName(TokenId id);
    Utf8 funcToName(const AstFuncDecl* node);
} // namespace Naming
