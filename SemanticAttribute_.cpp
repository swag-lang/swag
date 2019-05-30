#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Scope.h"

bool SemanticJob::resolveAttrDecl(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;

    SWAG_CHECK(node->scope->symTable->addSymbolTypeInfo(sourceFile, node->token, node->name, node->typeInfo, SymbolKind::Attribute));
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveAttrUse(SemanticContext* context)
{
    auto node          = context->node;
    auto nextStatement = node->childParentIdx < node->parent->childs.size() - 1 ? node->parent->childs[node->childParentIdx + 1] : nullptr;

    if (nextStatement)
    {
        nextStatement->attributes = node;
    }

    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::collectAttributes(SemanticContext* context, vector<TypeInfoAttr*>& result, AstNode* attributes, AstNodeKind kind)
{
    auto sourceFile = context->sourceFile;
    auto curAttr    = attributes;
    while (curAttr)
    {
        // Check that the attribute matches the following declaration
        for (auto child : curAttr->childs)
        {
            TypeInfoAttr* typeInfo = CastTypeInfo<TypeInfoAttr>(child->typeInfo, TypeInfoKind::Attribute);
            if (!(typeInfo->flags & TYPEINFO_ATTRIBUTE_FUNC) && kind == AstNodeKind::FuncDecl)
                return sourceFile->report({sourceFile, child->token, format("attribute '%s' must be followed by a function declaration", child->name.c_str())});
            if (!(typeInfo->flags & TYPEINFO_ATTRIBUTE_VAR) && kind == AstNodeKind::VarDecl)
                return sourceFile->report({sourceFile, child->token, format("attribute '%s' must be followed by a variable declaration", child->name.c_str())});

            result.push_back(typeInfo);
        }

        curAttr = curAttr->attributes;
    }

    return true;
}