
#pragma once
struct SemanticContext;
struct SymbolOverload;
struct OneGenericMatch;

struct Generic
{
	static bool InstanciateFunction(SemanticContext* context, OneGenericMatch& match);
};