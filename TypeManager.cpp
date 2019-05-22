#include "pch.h"
#include "AstNode.h"
#include "TypeManager.h"
#include "Diagnostic.h"

TypeInfo *TypeManager::makeCompatibles(SourceFile* sourceFile, AstNode* requestedTypeNode, AstNode* fromNode)
{
    if (!requestedTypeNode)
        return fromNode->typeInfo;
    if (!fromNode)
        return requestedTypeNode->typeInfo;
    if (requestedTypeNode->typeInfo == fromNode->typeInfo)
        return requestedTypeNode->typeInfo;

	sourceFile->report({sourceFile, requestedTypeNode->token, "incompatible types"});
	return false;
}