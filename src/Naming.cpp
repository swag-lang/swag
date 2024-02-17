#include "pch.h"
#include "Naming.h"
#include "Ast.h"
#include "AstFlags.h"
#include "AstNode.h"
#include "TypeInfo.h"

Utf8 Naming::kindName(const SymbolName* symbol, const AstNode* node, const TypeInfo* typeInfo, OverloadFlags overFlags, Utf8& article)
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

	if (overFlags.has(OVERLOAD_VAR_CAPTURE))
	{
		article = "a";
		return "captured parameter";
	}

	if (overFlags.has(OVERLOAD_VAR_FUNC_PARAM) && node->ownerFct && node->ownerFct->typeInfo->isLambda())
	{
		article = "a";
		return "lambda parameter";
	}

	if (overFlags.has(OVERLOAD_VAR_FUNC_PARAM) && node->ownerFct && node->ownerFct->typeInfo->isClosure())
	{
		article = "a";
		return "closure parameter";
	}

	if (overFlags.has(OVERLOAD_VAR_FUNC_PARAM))
	{
		article = "a";
		if (overFlags.has(OVERLOAD_GENERIC))
			return "generic parameter";
		return "function parameter";
	}

	if (overFlags.has(OVERLOAD_VAR_INLINE))
	{
		article = "a";
		return "function parameter";
	}

	if (overFlags.has(OVERLOAD_CONSTANT))
	{
		article = "a";
		if (overFlags.has(OVERLOAD_IS_LET))
			return "variable";
		return "constant";
	}

	if (overFlags.has(OVERLOAD_VAR_GLOBAL))
	{
		article = "a";
		return "global variable";
	}

	if (overFlags.has(OVERLOAD_VAR_LOCAL))
	{
		article = "a";
		return "local variable";
	}

	if (overFlags.has(OVERLOAD_VAR_STRUCT))
	{
		article = "a";
		return "field";
	}

	if (node->kind == AstNodeKind::FuncDecl && node->hasAttribute(ATTRIBUTE_MACRO))
	{
		article = "a";
		return "macro";
	}

	if (node->kind == AstNodeKind::FuncDecl && node->hasAttribute(ATTRIBUTE_MIXIN))
	{
		article = "a";
		return "mixin";
	}

	if (node->kind == AstNodeKind::FuncDecl && node->hasAttribute(ATTRIBUTE_INLINE))
	{
		article = "an";
		return "inlined function";
	}

	return kindName(symbol->kind, article);
}

Utf8 Naming::kindName(const SymbolOverload* overload)
{
	Utf8 article;
	return kindName(overload->symbol, overload->node, overload->typeInfo, overload->flags, article);
}

Utf8 Naming::aKindName(const SymbolOverload* overload)
{
	Utf8       article;
	const auto result = kindName(overload->symbol, overload->node, overload->typeInfo, overload->flags, article);
	article += " ";
	article += result;
	return article;
}

Utf8 Naming::kindName(SymbolKind kind, Utf8& article)
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
	case SymbolKind::NameAlias:
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
	default:
		break;
	}

	article = "a";
	return "symbol";
}

Utf8 Naming::kindName(SymbolKind kind)
{
	Utf8 article;
	return kindName(kind, article);
}

Utf8 Naming::aKindName(SymbolKind kind)
{
	Utf8       article;
	const auto result = kindName(kind, article);
	article += " ";
	article += result;
	return article;
}

Utf8 Naming::kindName(const TypeInfo* typeInfo, Utf8& article)
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

	default:
		break;
	}

	article = "a";
	return "type";
}

Utf8 Naming::kindName(const TypeInfo* typeInfo)
{
	Utf8 article;
	return kindName(typeInfo, article);
}

Utf8 Naming::aKindName(const TypeInfo* typeInfo)
{
	Utf8       article;
	const auto result = kindName(typeInfo, article);
	article += " ";
	article += result;
	return article;
}

Utf8 Naming::kindName(AstNode* node, Utf8& article)
{
	switch (node->kind)
	{
	case AstNodeKind::VarDecl:
		article = "a";
		if (node->ownerScope && node->ownerScope->isGlobal())
			return "global variable";
		if (node->hasAstFlag(AST_STRUCT_MEMBER))
			return "struct member";
		if (node->resolvedSymbolOverload && node->resolvedSymbolOverload->hasFlag(OVERLOAD_VAR_FUNC_PARAM))
			return "function parameter";
		return "local variable";

	case AstNodeKind::ConstDecl:
		article = "a";
		return "constant";

	case AstNodeKind::FuncDecl:
	case AstNodeKind::FuncDeclType:
		article = "a";
		return "function";

	case AstNodeKind::AttrUse:
	case AstNodeKind::AttrDecl:
		article = "an";
		return "attribute";

	case AstNodeKind::EnumDecl:
		article = "an";
		return "enum";

	case AstNodeKind::EnumValue:
		article = "an";
		return "enum value";

	case AstNodeKind::Namespace:
		article = "a";
		return "namespace";

	case AstNodeKind::TypeAlias:
		article = "a";
		return "type alias";

	case AstNodeKind::NameAlias:
		article = "a";
		return "name alias";

	case AstNodeKind::FuncDeclParam:
		article = "a";
		return "function parameter";

	case AstNodeKind::StructDecl:
		article = "a";
		return "struct";

	case AstNodeKind::InterfaceDecl:
		article = "an";
		return "interface";

	case AstNodeKind::Impl:
		{
			article        = "an";
			const auto ast = castAst<AstImpl>(node, AstNodeKind::Impl);
			if (ast->identifierFor)
				return "interface implementation block";
			return "implementation block";
		}

	case AstNodeKind::Identifier:
		article = "an";
		return "identifier";

	case AstNodeKind::IntrinsicProp:
		article = "an";
		return "intrinsic";

	case AstNodeKind::FuncCall:
		article = "a";
		return "function call";

	case AstNodeKind::TypeExpression:
		article = "a";
		return "type";

	case AstNodeKind::CompilerValidIf:
	case AstNodeKind::CompilerValidIfx:
		article = "a";
		return "constraint";

	case AstNodeKind::Defer:
		article = "an";
		return "instruction";

	default:
		break;
	}

	article = "a";
	return "node";
}

Utf8 Naming::kindName(AstNode* node)
{
	Utf8 article;
	return kindName(node, article);
}

Utf8 Naming::aKindName(AstNode* node)
{
	Utf8       article;
	const auto result = kindName(node, article);
	article += " ";
	article += result;
	return article;
}

Utf8 Naming::kindName(ScopeKind kind, Utf8& article)
{
	switch (kind)
	{
	case ScopeKind::Namespace:
		article = "a";
		return "namespace";

	case ScopeKind::Enum:
		article = "an";
		return "enum";

	case ScopeKind::TypeList:
		article = "a";
		return "tuple";

	case ScopeKind::Struct:
		article = "a";
		return "struct";

	case ScopeKind::File:
		article = "a";
		return "file";

	case ScopeKind::Module:
		article = "a";
		return "module";

	case ScopeKind::Statement:
	case ScopeKind::EmptyStatement:
		article = "a";
		return "statement";

	case ScopeKind::Inline:
		article = "an";
		return "inline";

	case ScopeKind::Function:
	case ScopeKind::FunctionBody:
		article = "a";
		return "function";

	default:
		article = "a";
		return "scope";
	}
}

Utf8 Naming::kindName(ScopeKind kind)
{
	Utf8 article;
	return kindName(kind, article);
}

Utf8 Naming::aKindName(ScopeKind kind)
{
	Utf8       article;
	const auto result = kindName(kind, article);
	article += " ";
	article += result;
	return article;
}

Utf8 Naming::niceArgumentRank(size_t idx)
{
	switch (idx)
	{
	case 1:
		return "first argument";
	case 2:
		return "second argument";
	case 3:
		return "third argument";
	default:
		break;
	}
	return FMT("argument [[%d]]", idx);
}

Utf8 Naming::niceParameterRank(size_t idx)
{
	switch (idx)
	{
	case 1:
		return "first parameter";
	case 2:
		return "second parameter";
	case 3:
		return "third parameter";
	default:
		break;
	}
	return FMT("parameter [[%d]]", idx);
}

Utf8 Naming::tokenToName(TokenId id)
{
	switch (id)
	{
	case TokenId::SymComma:
		return ",";
	case TokenId::SymColon:
		return ":";
	case TokenId::SymSemiColon:
		return ";";
	case TokenId::SymLeftCurly:
		return "{";
	case TokenId::SymRightCurly:
		return "}";
	case TokenId::SymLeftParen:
		return "(";
	case TokenId::SymRightParen:
		return ")";
	case TokenId::SymLeftSquare:
		return "[";
	case TokenId::SymRightSquare:
		return "]";
	case TokenId::SymDot:
		return ".";
	case TokenId::SymExclam:
		return "!";
	case TokenId::SymMinusGreat:
		return "->";
	case TokenId::SymEqual:
		return "=";
	case TokenId::SymVertical:
		return "|";
	default:
		break;
	}

	return "???";
}

Utf8 Naming::funcToName(const AstFuncDecl* node)
{
	if (node->hasAttribute(ATTRIBUTE_AST_FUNC))
		return "[[#ast]] block";
	if (node->hasAttribute(ATTRIBUTE_RUN_FUNC | ATTRIBUTE_RUN_GENERATED_FUNC))
		return "[[#run]] block";
	if (node->hasAttribute(ATTRIBUTE_MATCH_VALIDIF_FUNC))
		return "[[#validif]] block";
	if (node->hasAttribute(ATTRIBUTE_MATCH_VALIDIFX_FUNC))
		return "[[#validifx]] block";

	if (node->hasAttribute(ATTRIBUTE_TEST_FUNC) && node->hasAttribute(ATTRIBUTE_SHARP_FUNC))
		return "[[#test]] block";
	if (node->hasAttribute(ATTRIBUTE_MAIN_FUNC))
		return "[[#main]] block";
	if (node->hasAttribute(ATTRIBUTE_COMPILER_FUNC))
		return "[[#message]] block";
	if (node->hasAttribute(ATTRIBUTE_INIT_FUNC))
		return "[[#init]] block";
	if (node->hasAttribute(ATTRIBUTE_DROP_FUNC))
		return "[[#drop]] block";
	if (node->hasAttribute(ATTRIBUTE_PREMAIN_FUNC))
		return "[[#premain]] block";

	if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IS_LAMBDA_EXPRESSION))
		return "lambda";

	if (node->hasAttribute(ATTRIBUTE_SHARP_FUNC))
		return FMT("[[%s]] block", node->token.c_str());

	if (node->hasAttribute(ATTRIBUTE_MIXIN))
		return FMT("[[%s]] mixin", node->token.c_str());
	if (node->hasAttribute(ATTRIBUTE_MACRO))
		return FMT("[[%s]] macro", node->token.c_str());

	return FMT("[[%s]] function", node->token.c_str());
}
