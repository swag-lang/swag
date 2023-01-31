#pragma once
#include "Utf8.h"
struct SymbolName;
struct AstNode;
struct TypeInfo;
struct SymbolOverload;
enum class SymbolKind : uint8_t;

namespace Naming
{
    Utf8 getKindName(SymbolName* symbol, AstNode* node, TypeInfo* typeInfo, uint32_t overFlags, Utf8& article);
    Utf8 getNakedKindName(SymbolOverload* overload);
    Utf8 getArticleKindName(SymbolOverload* overload);
    Utf8 getArticleKindName(SymbolKind kind);
    Utf8 getNakedKindName(SymbolKind kind);
    Utf8 getKindName(SymbolKind kind, Utf8& article);

    Utf8 getKindName(TypeInfo* typeInfo, Utf8& article);
    Utf8 getArticleKindName(TypeInfo* typeInfo);
    Utf8 getNakedKindName(TypeInfo* typeInfo);
}; // namespace Naming
