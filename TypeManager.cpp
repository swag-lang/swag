#include "pch.h"
#include "AstNode.h"
#include "TypeManager.h"
#include "Diagnostic.h"

bool TypeManager::makeCompatibles(SourceFile* sourceFile, AstNode* sourceNode, AstNode* fromNode)
{
    if (!sourceNode || !fromNode)
        return true;
    if (sourceNode->typeInfo == fromNode->typeInfo)
        return true;

	sourceFile->report({sourceFile, sourceNode->token, "incompatible types"});
	return false;
}