#include "pch.h"
#include "Global.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "SemanticJob.h"
#include "TypeInfo.h"
#include "Ast.h"
#include "Scope.h"
#include "Attribute.h"

bool SemanticJob::checkAttribute(SemanticContext* context, AstNode* oneAttribute, AstNode* checkNode, AstNodeKind kind)
{
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(oneAttribute->typeInfo, TypeInfoKind::FuncAttr);

    if (!checkNode)
    {
        if (typeInfo->flags & TYPEINFO_ATTRIBUTE_FUNC)
            return context->errorContext.report({sourceFile, oneAttribute->token, format("attribute '%s' must be followed by a function definition", oneAttribute->name.c_str())});
        if (typeInfo->flags & TYPEINFO_ATTRIBUTE_VAR)
            return context->errorContext.report({sourceFile, oneAttribute->token, format("attribute '%s' must be followed by a variable definition", oneAttribute->name.c_str())});
        SWAG_ASSERT(false);
        return false;
    }

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_FUNC) && kind != AstNodeKind::FuncDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to a function definition", oneAttribute->name.c_str())};
        Diagnostic note{sourceFile, checkNode->token, format("'%s' is %s", checkNode->name.c_str(), AstNode::getKindName(checkNode)), DiagnosticLevel::Note};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->sourceFile, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return context->errorContext.report(diag, &note, &note1);
    }

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_VAR) && kind != AstNodeKind::VarDecl && kind != AstNodeKind::LetDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to a variable definition", oneAttribute->name.c_str())};
        Diagnostic note{sourceFile, checkNode->token, format("'%s' is %s", checkNode->name.c_str(), AstNode::getKindName(checkNode)), DiagnosticLevel::Note};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->sourceFile, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return context->errorContext.report(diag, &note, &note1);
    }

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_STRUCT) && kind != AstNodeKind::StructDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to a struct definition", oneAttribute->name.c_str())};
        Diagnostic note{sourceFile, checkNode->token, format("'%s' is %s", checkNode->name.c_str(), AstNode::getKindName(checkNode)), DiagnosticLevel::Note};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->sourceFile, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return context->errorContext.report(diag, &note, &note1);
    }

    if ((typeInfo->flags & TYPEINFO_ATTRIBUTE_ENUM) && kind != AstNodeKind::EnumDecl && kind != AstNodeKind::Statement)
    {
        Diagnostic diag{sourceFile, oneAttribute->token, format("attribute '%s' can only be applied to an enum definition", oneAttribute->name.c_str())};
        Diagnostic note{sourceFile, checkNode->token, format("'%s' is %s", checkNode->name.c_str(), AstNode::getKindName(checkNode)), DiagnosticLevel::Note};
        Diagnostic note1{oneAttribute->resolvedSymbolOverload->sourceFile, oneAttribute->resolvedSymbolOverload->node->token, format("this is the declaration of attribute '%s'", oneAttribute->name.c_str()), DiagnosticLevel::Note};
        return context->errorContext.report(diag, &note, &note1);
    }

    return true;
}

bool SemanticJob::collectAttributes(SemanticContext* context, SymbolAttributes& result, AstAttrUse* attrUse, AstNode* forNode, AstNodeKind kind, uint32_t& flags)
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
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);

            if (result.attributes.find(typeInfo) != result.attributes.end())
            {
                Diagnostic diag{sourceFile, forNode->token, format("attribute '%s' assigned twice to '%s'", child->name.c_str(), forNode->name.c_str())};
                Diagnostic note{sourceFile, child->token, "this is the faulty attribute", DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }

            // Predefined attributes will mark some flags
            if (child->name == "constexpr")
                flags |= ATTRIBUTE_CONSTEXPR;
            else if (child->name == "printbc")
                flags |= ATTRIBUTE_PRINTBYTECODE;
            else if (child->name == "test")
                flags |= ATTRIBUTE_TEST;
            else if (child->name == "compiler")
                flags |= ATTRIBUTE_COMPILER;
            else if (child->name == "public")
                flags |= ATTRIBUTE_PUBLIC;
            else if (child->name == "foreign")
                flags |= ATTRIBUTE_FOREIGN;

            result.attributes.insert(typeInfo);
            result.values.insert(curAttr->values.begin(), curAttr->values.end());
        }

        curAttr = curAttr->parentAttributes;
    }

    return true;
}

bool SemanticJob::resolveAttrDecl(SemanticContext* context)
{
    auto node       = CastAst<AstAttrDecl>(context->node, AstNodeKind::AttrDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    if (node->semanticPass == 0)
    {
        SWAG_CHECK(setupFuncDeclParams(context, typeInfo, node, node->parameters, false));
        SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(sourceFile, node, node->typeInfo, SymbolKind::Attribute));
    }

    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Attribute));
    return true;
}

bool SemanticJob::resolveAttrUse(SemanticContext* context)
{
    auto        sourceFile    = context->sourceFile;
    auto        node          = CastAst<AstAttrUse>(context->node, AstNodeKind::AttrUse);
    auto        nextStatement = node->childParentIdx < node->parent->childs.size() - 1 ? node->parent->childs[node->childParentIdx + 1] : nullptr;
    AstNodeKind kind          = nextStatement ? nextStatement->kind : AstNodeKind::Module;

    ComputedValue dummy;
    dummy.reg.b = true;

    for (auto child : node->childs)
    {
        SWAG_CHECK(checkAttribute(context, child, nextStatement, kind));

        // Collect parameters
        auto identifierRef = CastAst<AstIdentifierRef>(child, AstNodeKind::IdentifierRef);
        auto identifier    = static_cast<AstIdentifier*>(identifierRef->childs.back());

        // Be sure this is an attribute
        auto resolvedName = identifier->resolvedSymbolName;
        auto resolved     = identifier->resolvedSymbolOverload;
        if (resolvedName->kind != SymbolKind::Attribute)
        {
            Diagnostic diag{sourceFile, identifier, format("invalid attribute '%s'", resolvedName->name.c_str())};
            Diagnostic note{resolved->sourceFile, resolved->node->token, format("this is the definition of '%s'", resolvedName->name.c_str()), DiagnosticLevel::Note};
            context->errorContext.report(diag, &note);
            return false;
        }

		// Register attribute itself
        node->values[identifierRef->resolvedSymbolName->fullName] = dummy;

		// And all its parameters
        if (identifier->callParameters)
        {
            for (auto one : identifier->callParameters->childs)
            {
                auto param = CastAst<AstFuncCallParam>(one, AstNodeKind::FuncCallParam);
                SWAG_VERIFY(param->flags & AST_VALUE_COMPUTED, context->errorContext.report({sourceFile, param, "attribute parameter cannot be evaluated at compile time"}));
                string attrFullName        = Scope::makeFullName(identifierRef->resolvedSymbolName->fullName, param->resolvedParameter->namedParam);
                node->values[attrFullName] = param->computedValue;
            }
        }
    }

    if (nextStatement)
    {
        nextStatement->parentAttributes = node;
    }

    return true;
}
