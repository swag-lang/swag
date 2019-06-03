#include "pch.h"
#include "Global.h"
#include "Diagnostic.h"
#include "PoolFactory.h"
#include "SourceFile.h"

bool SemanticJob::checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode, AstNodeKind kind)
{
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FunctionAttribute);

    if (!checkNode)
    {
        if (typeInfo->flags & TYPEINFO_ATTRIBUTE_FUNC)
            return sourceFile->report({sourceFile, oneAttribute->token, format("attribute '%s' must be followed by a function definition", oneAttribute->name.c_str())});
        if (typeInfo->flags & TYPEINFO_ATTRIBUTE_VAR)
            return sourceFile->report({sourceFile, oneAttribute->token, format("attribute '%s' must be followed by a variable definition", oneAttribute->name.c_str())});
        assert(false);
        return false;
    }

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_FUNC) && kind != AstNodeKind::FuncDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to a function definition", oneAttribute->name.c_str())};
        Diagnostic note{sourceFile, checkNode->token, format("this is %s", Ast::getKindName(checkNode)), DiagnosticLevel::Note};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->sourceFile, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return sourceFile->report(diag, &note, &note1);
    }

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_VAR) && kind != AstNodeKind::VarDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to a variable definition", oneAttribute->name.c_str())};
        Diagnostic note{sourceFile, checkNode->token, format("this is %s", Ast::getKindName(checkNode)), DiagnosticLevel::Note};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->sourceFile, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return sourceFile->report(diag, &note, &note1);
    }

    return true;
}

bool SemanticJob::collectAttributes(SemanticContext* context, set<TypeInfoFuncAttr*>& result, AstNode* attrUse, AstNode* forNode, AstNodeKind kind, uint64_t& flags)
{
    if (!attrUse)
        return true;

    auto sourceFile = context->sourceFile;
    auto curAttr    = attrUse;
    while (curAttr)
    {
        // Check that the attribute matches the following declaration
        for (auto child : curAttr->childs)
        {
            SWAG_CHECK(checkAttribute(context, child, forNode, kind));
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FunctionAttribute);

            if (result.find(typeInfo) != result.end())
            {
                Diagnostic diag{sourceFile, forNode->token, format("attribute '%s' assigned twice to '%s'", child->name.c_str(), forNode->name.c_str())};
                Diagnostic note{sourceFile, child->token, "this is the faulty attribute", DiagnosticLevel::Note};
                return sourceFile->report(diag, &note);
            }

			// Predefined attributes will mark some flags
			if (typeInfo->name == "constexpr")
                flags |= ATTRIBUTE_CONSTEXPR;

            result.insert(typeInfo);
        }

        curAttr = curAttr->attributes;
    }

    return true;
}

bool SemanticJob::resolveAttrDecl(SemanticContext* context)
{
    auto node       = CastAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FunctionAttribute);

    SWAG_CHECK(setupFuncDeclParameters(sourceFile, typeInfo, node->parameters));
    SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(sourceFile, node, node->typeInfo, SymbolKind::Attribute));
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Attribute));

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
