#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "SymTable.h"

bool Generic::InstanciateFunction(SemanticContext* context, OneGenericMatch& match)
{
    auto symbol     = match.symbolOverload;
    auto sourceNode = symbol->node;
    auto newFunc    = sourceNode->clone();
    Ast::addChild(sourceNode->parent, newFunc);

    auto newType = static_cast<TypeInfoFuncAttr*>(symbol->typeInfo->clone());
    for (int i = 0; i < newType->genericParameters.size(); i++)
    {
        auto param          = newType->genericParameters[i];
		if (param->typeInfo)
			param->typeInfo->release();
        param->typeInfo     = match.cacheGenericMatchesParamsTypes[i];
        param->genericValue = match.cacheGenericMatchesParamsValues[i];
    }

    return false;
}