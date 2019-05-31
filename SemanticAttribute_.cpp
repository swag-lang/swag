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
    auto node       = CastAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FunctionAttribute);

    // Set parameters
    if (node->parameters)
    {
        for (auto param : node->parameters->childs)
        {
            typeInfo->parameters.push_back(param->typeInfo);
        }
    }

    SWAG_CHECK(node->scope->symTable->addSymbolTypeInfo(sourceFile, node->token, node->name, node->typeInfo, SymbolKind::Attribute));

    // We need to check the scope hierarchy for symbol ghosting
    auto scope = node->scope->parentScope;
    while (scope)
    {
        SWAG_CHECK(scope->symTable->checkHiddenSymbol(context->sourceFile, node->token, node->name, node->typeInfo, SymbolKind::Attribute));
        scope = scope->parentScope;
    }

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
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FunctionAttribute);

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_FUNC) && kind != AstNodeKind::FuncDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to a function definition", oneAttribute->name.c_str())};
        Diagnostic note{sourceFile, checkNode->token, format("this is %s", Ast::getKindName(checkNode)), DiagnosticLevel::Note};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->sourceFile, oneAttribute->resolvedSymbolOverload->startLocation, oneAttribute->resolvedSymbolOverload->endLocation, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return sourceFile->report(diag, &note, &note1);
    }

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_VAR) && kind != AstNodeKind::VarDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to a variable definition", oneAttribute->name.c_str())};
        Diagnostic note{sourceFile, checkNode->token, format("this is %s", Ast::getKindName(checkNode)), DiagnosticLevel::Note};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->sourceFile, oneAttribute->resolvedSymbolOverload->startLocation, oneAttribute->resolvedSymbolOverload->endLocation, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return sourceFile->report(diag, &note, &note1);
    }

    return true;
}

bool SemanticJob::collectAttributes(SemanticContext* context, set<TypeInfoFuncAttr*>& result, AstNode* attrUse, AstNodeKind kind)
{
    auto sourceFile = context->sourceFile;
    auto curAttr    = attrUse;
    while (curAttr)
    {
        // Check that the attribute matches the following declaration
        for (auto child : curAttr->childs)
        {
            SWAG_CHECK(checkAttribute(context, child, context->node, kind));
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FunctionAttribute);

            if (result.find(typeInfo) != result.end())
            {
                Diagnostic diag{sourceFile, context->node->token, format("attribute '%s' assigned twice to '%s'", child->name.c_str(), context->node->name.c_str())};
                Diagnostic note{sourceFile, child->token, "this is the faulty attribute", DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }

            result.insert(typeInfo);
        }

        curAttr = curAttr->attributes;
    }

    return true;
}