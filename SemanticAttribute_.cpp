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
    auto        node          = context->node;
    auto        nextStatement = node->childParentIdx < node->parent->childs.size() - 1 ? node->parent->childs[node->childParentIdx + 1] : nullptr;
    AstNodeKind kind          = nextStatement ? nextStatement->kind : AstNodeKind::Module;

    for (auto child : node->childs)
    {
        SWAG_CHECK(checkAttribute(context, child, nextStatement, kind));
    }

    if (nextStatement)
    {
        nextStatement->attributes = node;
    }

    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode, AstNodeKind kind)
{
    auto          sourceFile = context->sourceFile;
    TypeInfoAttr* typeInfo   = CastTypeInfo<TypeInfoAttr>(oneAttribute->typeInfo, TypeInfoKind::Attribute);

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_FUNC) && kind != AstNodeKind::FuncDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to a function declaration", oneAttribute->name.c_str())};
        Diagnostic note{sourceFile, checkNode->token, "this is not a function", DiagnosticLevel::Note};
        return sourceFile->report(diag, &note);
    }

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_VAR) && kind != AstNodeKind::VarDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to a variable declaration", oneAttribute->name.c_str())};
		Diagnostic note{ sourceFile, checkNode->token, "this is not a variable", DiagnosticLevel::Note };
        return sourceFile->report(diag, &note);
    }

    return true;
}

bool SemanticJob::collectAttributes(SemanticContext* context, vector<TypeInfoAttr*>& result, AstNode* attrUse, AstNodeKind kind)
{
    auto curAttr = attrUse;
    while (curAttr)
    {
        // Check that the attribute matches the following declaration
        for (auto child : curAttr->childs)
        {
            SWAG_CHECK(checkAttribute(context, child, context->node, kind));
            TypeInfoAttr* typeInfo = CastTypeInfo<TypeInfoAttr>(child->typeInfo, TypeInfoKind::Attribute);
            result.push_back(typeInfo);
        }

        curAttr = curAttr->attributes;
    }

    return true;
}