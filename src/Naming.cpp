#include "pch.h"
#include "TypeInfo.h"
#include "AstNode.h"
#include "Naming.h"

namespace Naming
{
    Utf8 getKindName(SymbolName* symbol, AstNode* node, TypeInfo* typeInfo, uint32_t overFlags, Utf8& article)
    {
        if (typeInfo->isTuple())
        {
            article = "a";
            return "tuple";
        }

        if (typeInfo->isLambda())
        {
            article = "a";
            return "lambda";
        }

        if (typeInfo->isClosure())
        {
            article = "a";
            return "closure";
        }

        if (node->isGeneratedSelf())
        {
            article = "an";
            return "implicit parameter";
        }

        if (overFlags & OVERLOAD_VAR_CAPTURE)
        {
            article = "a";
            return "captured parameter";
        }

        if (overFlags & OVERLOAD_VAR_FUNC_PARAM && node->ownerFct && node->ownerFct->typeInfo->isLambda())
        {
            article = "a";
            return "lambda parameter";
        }

        if (overFlags & OVERLOAD_VAR_FUNC_PARAM && node->ownerFct && node->ownerFct->typeInfo->isClosure())
        {
            article = "a";
            return "closure parameter";
        }

        if (overFlags & OVERLOAD_VAR_FUNC_PARAM)
        {
            article = "a";
            return "function parameter";
        }

        if (overFlags & OVERLOAD_VAR_INLINE)
        {
            article = "a";
            return "function parameter";
        }

        if (overFlags & OVERLOAD_VAR_GLOBAL)
        {
            article = "a";
            return "global variable";
        }

        if (overFlags & OVERLOAD_VAR_LOCAL)
        {
            article = "a";
            return "local variable";
        }

        if (overFlags & OVERLOAD_VAR_STRUCT)
        {
            article = "a";
            return "field";
        }

        if (overFlags & OVERLOAD_CONSTANT)
        {
            article = "a";
            return "constant";
        }

        if (node->kind == AstNodeKind::FuncDecl && node->attributeFlags & ATTRIBUTE_MACRO)
        {
            article = "a";
            return "macro";
        }

        if (node->kind == AstNodeKind::FuncDecl && node->attributeFlags & ATTRIBUTE_MIXIN)
        {
            article = "a";
            return "mixin";
        }

        if (node->kind == AstNodeKind::FuncDecl && node->attributeFlags & ATTRIBUTE_INLINE)
        {
            article = "an";
            return "inlined function";
        }

        return getArticleKindName(symbol->kind);
    }

    Utf8 getNakedKindName(SymbolOverload* overload)
    {
        Utf8 article;
        return getKindName(overload->symbol, overload->node, overload->typeInfo, overload->flags, article);
    }

    Utf8 getArticleKindName(SymbolOverload* overload)
    {
        Utf8 article;
        auto result = getKindName(overload->symbol, overload->node, overload->typeInfo, overload->flags, article);
        article += " ";
        article += result;
        return article;
    }

    Utf8 getKindName(SymbolKind kind, Utf8& article)
    {
        switch (kind)
        {
        case SymbolKind::Attribute:
            article = "an";
            return "attribute";
        case SymbolKind::Enum:
            article = "an";
            return "enum";
        case SymbolKind::EnumValue:
            article = "an";
            return "enum value";
        case SymbolKind::Function:
            article = "a";
            return "function";
        case SymbolKind::Namespace:
            article = "a";
            return "namespace";
        case SymbolKind::TypeAlias:
            article = "a";
            return "type alias";
        case SymbolKind::Alias:
            article = "a";
            return "name alias";
        case SymbolKind::Variable:
            article = "a";
            return "variable";
        case SymbolKind::Struct:
            article = "a";
            return "struct";
        case SymbolKind::Interface:
            article = "an";
            return "interface";
        case SymbolKind::GenericType:
            article = "a";
            return "generic type";
        case SymbolKind::PlaceHolder:
            article = "a";
            return "placeholder";
        }

        article = "a";
        return "symbol";
    }

    Utf8 getArticleKindName(SymbolKind kind)
    {
        Utf8 article;
        auto result = getKindName(kind, article);
        article += " ";
        article += result;
        return article;
    }

    Utf8 getNakedKindName(SymbolKind kind)
    {
        Utf8 article;
        return getKindName(kind, article);
    }

    Utf8 getKindName(TypeInfo* typeInfo, Utf8& article)
    {
        switch (typeInfo->kind)
        {
        case TypeInfoKind::Namespace:
            article = "a";
            return "namespace";
        case TypeInfoKind::Enum:
            article = "an";
            return "enum";

        case TypeInfoKind::Array:
            article = "an";
            return "array";

        case TypeInfoKind::Slice:
            article = "a";
            return "slice";

        case TypeInfoKind::Pointer:
            article = "a";
            if (typeInfo->isPointerRef())
                return "reference";
            if (typeInfo->isAutoConstPointerRef())
                return "struct";
            return "pointer";

        case TypeInfoKind::FuncAttr:
            article = "a";
            return "function";

        case TypeInfoKind::LambdaClosure:
            article = "a";
            if (typeInfo->isClosure())
                return "closure";
            return "lambda";

        case TypeInfoKind::TypeListTuple:
            article = "a";
            return "tuple";
        case TypeInfoKind::TypeListArray:
            article = "an";
            return "array";

        case TypeInfoKind::Variadic:
        case TypeInfoKind::TypedVariadic:
        case TypeInfoKind::CVariadic:
            article = "a";
            return "variadic";

        case TypeInfoKind::Struct:
            article = "a";
            return "struct";

        case TypeInfoKind::Interface:
            article = "an";
            return "interface";

        case TypeInfoKind::Generic:
            article = "a";
            return "generic type";

        case TypeInfoKind::Alias:
            article = "an";
            return "alias type";

        case TypeInfoKind::Native:
            article = "a";
            return "type";
        }

        article = "a";
        return "type";
    }

    Utf8 getArticleKindName(TypeInfo* typeInfo)
    {
        Utf8 article;
        auto result = getKindName(typeInfo, article);
        article += " ";
        article += result;
        return article;
    }

    Utf8 getNakedKindName(TypeInfo* typeInfo)
    {
        Utf8 article;
        return getKindName(typeInfo, article);
    }

}; // namespace Naming