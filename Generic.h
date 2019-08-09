
#pragma once
struct SemanticContext;
struct SymbolOverload;

struct Generic
{
	static bool InstanciateFunction(SemanticContext* context, SymbolOverload* symbol);
};