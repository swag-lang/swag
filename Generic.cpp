#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "SymTable.h"

bool Generic::InstanciateFunction(SemanticContext* context, SymbolOverload* symbol)
{
    auto sourceNode = symbol->node;
    auto newFunc    = sourceNode->clone();
    Ast::addChild(sourceNode->parent, newFunc);

	auto newType = symbol->typeInfo->clone();
    return false;
}