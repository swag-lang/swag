#include "pch.h"
#include "Ast.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "TypeManager.h"

namespace Ast
{
    void incIndentStatement(AstNode* node, int& indent)
    {
        if (node->kind == AstNodeKind::CompilerIfBlock && node->childs.front()->kind == AstNodeKind::Statement)
            return;
        if (node->kind != AstNodeKind::Statement)
            indent++;
    }

    void decIndentStatement(AstNode* node, int& indent)
    {
        if (node->kind == AstNodeKind::CompilerIfBlock && node->childs.front()->kind == AstNodeKind::Statement)
            return;
        if (node->kind != AstNodeKind::Statement)
            indent--;
    }

    bool outputAttributes(OutputContext& context, Concat& concat, SymbolAttributes& attributes)
    {
        auto attr = &attributes;
        if (attr && !attr->empty())
        {
            concat.addIndent(context.indent);
            CONCAT_FIXED_STR(concat, "#[");

            for (int j = 0; j < attr->attributes.size(); j++)
            {
                auto& one = attr->attributes[j];
                if (j)
                    CONCAT_FIXED_STR(concat, ", ");

                concat.addString(one.name);
                if (!one.parameters.empty())
                {
                    concat.addChar('(');

                    for (int i = 0; i < one.parameters.size(); i++)
                    {
                        auto& oneParam = one.parameters[i];
                        if (i)
                            CONCAT_FIXED_STR(concat, ", ");
                        Ast::outputLiteral(context, concat, one.node, oneParam.typeInfo, oneParam.value.text, oneParam.value.reg);
                    }

                    concat.addChar(')');
                }
            }

            CONCAT_FIXED_STR(concat, "]");
            concat.addEol();
        }

        return true;
    }

    bool outputLiteral(OutputContext& context, Concat& concat, AstNode* node, TypeInfo* typeInfo, const Utf8& text, Register& reg)
    {
        if (typeInfo == g_TypeMgr.typeInfoNull)
        {
            CONCAT_FIXED_STR(concat, "null");
            return true;
        }

        if (typeInfo->kind == TypeInfoKind::TypeListTuple || typeInfo->kind == TypeInfoKind::TypeListArray)
        {
            SWAG_CHECK(Ast::output(context, concat, node));
            return true;
        }

        SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
        auto str = literalToString(typeInfo, text, reg);
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::String:
            concat.addString("\"" + str + "\"");
            break;
        case NativeTypeKind::Char:
            concat.addString("\"" + str + "\"\'char");
            break;
        case NativeTypeKind::Bool:
            concat.addString(str);
            break;
        default:
            if (!(typeInfo->flags & (TYPEINFO_UNTYPED_INTEGER | TYPEINFO_UNTYPED_FLOAT)))
            {
                str += '\'';
                str += typeInfo->name;
            }

            concat.addString(str);
            break;
        }

        return true;
    }

    bool outputLambdaExpression(OutputContext& context, Concat& concat, AstNode* node)
    {
        AstFuncDecl* funcDecl = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        concat.addString("@(");

        if (funcDecl->parameters && !funcDecl->parameters->childs.empty())
        {
            for (auto p : funcDecl->parameters->childs)
            {
                if (p != funcDecl->parameters->childs.front())
                    concat.addString2(", ");
                concat.addString(p->name);
                if (!p->childs.empty())
                {
                    concat.addString(": ");
                    SWAG_CHECK(output(context, concat, p->childs.front()));
                }

                auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
                if (param->assignment)
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(output(context, concat, param->assignment));
                }
            }
        }

        concat.addChar(')');

        if (funcDecl->flags & AST_SHORT_LAMBDA)
        {
            concat.addString(" => ");
            SWAG_ASSERT(funcDecl->content->kind == AstNodeKind::Return);
            SWAG_CHECK(output(context, concat, funcDecl->content->childs.front()));
        }
        else
        {
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, funcDecl->content));
        }

        return true;
    }

    bool output(OutputContext& context, Concat& concat, AstNode* node)
    {
        if (!node)
            return true;
        if (node->flags & AST_GENERATED)
            return true;

        if (node->flags & AST_IDENTIFIER_BACKTICK)
            concat.addChar('`');

        switch (node->kind)
        {
        case AstNodeKind::AttrUse:
        {
            auto nodeAttr = CastAst<AstAttrUse>(node, AstNodeKind::AttrUse);
            bool first    = true;
            concat.addString2("#[");
            for (auto s : nodeAttr->childs)
            {
                if (s == nodeAttr->content)
                    continue;
                if (!first)
                    concat.addString2(", ");
                first = false;
                SWAG_CHECK(output(context, concat, s));
            }

            concat.addChar(']');
            concat.addEol();
            concat.addIndent(context.indent);
            SWAG_CHECK(output(context, concat, nodeAttr->content));
            break;
        }

        case AstNodeKind::ExplicitNoInit:
            concat.addChar('?');
            break;

        case AstNodeKind::Index:
            concat.addString("@index");
            break;
        case AstNodeKind::Init:
            concat.addString("@init(");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            if (node->childs.size() == 2)
            {
                concat.addString2(", ");
                SWAG_CHECK(output(context, concat, node->childs.back()));
            }
            concat.addChar(')');
            break;
        case AstNodeKind::Drop:
            concat.addString("@drop(");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            if (node->childs.size() == 2)
            {
                concat.addString2(", ");
                SWAG_CHECK(output(context, concat, node->childs.back()));
            }
            concat.addChar(')');
            break;
        case AstNodeKind::PostMove:
            concat.addString("@postmove(");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            if (node->childs.size() == 2)
            {
                concat.addString2(", ");
                SWAG_CHECK(output(context, concat, node->childs.back()));
            }
            concat.addChar(')');
            break;
        case AstNodeKind::PostCopy:
            concat.addString("@postcopy(");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            if (node->childs.size() == 2)
            {
                concat.addString2(", ");
                SWAG_CHECK(output(context, concat, node->childs.back()));
            }
            concat.addChar(')');
            break;

        case AstNodeKind::Break:
        {
            auto nodeBreak = CastAst<AstBreakContinue>(node, AstNodeKind::Break);
            concat.addString("break ");
            concat.addString(nodeBreak->label);
            break;
        }

        case AstNodeKind::Continue:
        {
            auto nodeContinue = CastAst<AstBreakContinue>(node, AstNodeKind::Continue);
            concat.addString("continue ");
            concat.addString(nodeContinue->label);
            break;
        }

        case AstNodeKind::MakePointer:
        {
            concat.addChar('&');
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;
        }

        case AstNodeKind::MakePointerLambda:
        {
            outputLambdaExpression(context, concat, node->ownerMainNode);
            break;
        }

        case AstNodeKind::NoDrop:
            concat.addString("nodrop ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::Move:
            concat.addString("move ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::ArrayPointerIndex:
        {
            auto arrayNode = CastAst<AstArrayPointerIndex>(node, AstNodeKind::ArrayPointerIndex);
            if (arrayNode->isDeref)
            {
                concat.addChar(':');
                SWAG_CHECK(output(context, concat, arrayNode->array));
            }
            else
            {
                SWAG_CHECK(output(context, concat, arrayNode->array));
                concat.addChar('[');
                SWAG_CHECK(output(context, concat, arrayNode->access));
                concat.addChar(']');
            }
            break;
        }

        case AstNodeKind::ExpressionList:
        {
            auto exprNode = CastAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
            if (exprNode->forTuple)
                concat.addString("@{");
            else
                concat.addString("@[");

            int idx = 0;
            for (auto child : exprNode->childs)
            {
                if (child->flags & AST_GENERATED)
                    continue;
                if (idx++)
                    concat.addString2(", ");
                SWAG_CHECK(output(context, concat, child));
            }

            if (exprNode->forTuple)
                concat.addChar('}');
            else
                concat.addChar(']');
            break;
        }

        case AstNodeKind::CompilerAst:
        {
            CONCAT_FIXED_STR(concat, "#ast ");
            auto front = node->childs.front();
            if (front->kind == AstNodeKind::FuncDecl)
            {
                AstFuncDecl* funcDecl = CastAst<AstFuncDecl>(front, AstNodeKind::FuncDecl);
                incIndentStatement(funcDecl->content, context.indent);
                concat.addEolIndent(context.indent);
                SWAG_CHECK(output(context, concat, funcDecl->content));
                decIndentStatement(funcDecl->content, context.indent);
            }
            else
            {
                SWAG_CHECK(output(context, concat, front));
            }
            break;
        }

        case AstNodeKind::CompilerIfBlock:
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::CompilerMacro:
            CONCAT_FIXED_STR(concat, "#macro");
            incIndentStatement(node->childs.front(), context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, node->childs.front()));
            decIndentStatement(node->childs.front(), context.indent);
            break;

        case AstNodeKind::CompilerInline:
            CONCAT_FIXED_STR(concat, "#inline");
            incIndentStatement(node->childs.front(), context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, node->childs.front()));
            decIndentStatement(node->childs.front(), context.indent);
            break;

        case AstNodeKind::CompilerMixin:
            CONCAT_FIXED_STR(concat, "#mixin ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::CompilerPrint:
            CONCAT_FIXED_STR(concat, "#print ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;

        case AstNodeKind::CompilerAssert:
            CONCAT_FIXED_STR(concat, "#assert(");
            SWAG_CHECK(output(context, concat, node->childs[0]));
            if (node->childs.size() > 1)
            {
                concat.addChar(',');
                SWAG_CHECK(output(context, concat, node->childs[1]));
            }

            concat.addChar(')');
            break;

        case AstNodeKind::CompilerIf:
        {
            auto compilerIf = CastAst<AstIf>(node, AstNodeKind::CompilerIf);
            CONCAT_FIXED_STR(concat, "#if ");
            SWAG_CHECK(output(context, concat, compilerIf->boolExpression));

            incIndentStatement(compilerIf->ifBlock, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, compilerIf->ifBlock));
            decIndentStatement(compilerIf->ifBlock, context.indent);

            if (compilerIf->elseBlock)
            {
                concat.addEolIndent(context.indent);
                CONCAT_FIXED_STR(concat, "#else ");
                if (compilerIf->elseBlock->childs.front()->kind != AstNodeKind::CompilerIf)
                {
                    incIndentStatement(compilerIf->elseBlock, context.indent);
                    concat.addEolIndent(context.indent);
                }
                SWAG_CHECK(output(context, concat, compilerIf->elseBlock));
                if (compilerIf->elseBlock->childs.front()->kind != AstNodeKind::CompilerIf)
                {
                    decIndentStatement(compilerIf->elseBlock, context.indent);
                }
            }
            break;
        }

        case AstNodeKind::If:
        {
            auto compilerIf = CastAst<AstIf>(node, AstNodeKind::If, AstNodeKind::CompilerIf);
            CONCAT_FIXED_STR(concat, "if ");
            SWAG_CHECK(output(context, concat, compilerIf->boolExpression));

            incIndentStatement(compilerIf->ifBlock, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, compilerIf->ifBlock));
            decIndentStatement(compilerIf->ifBlock, context.indent);

            if (compilerIf->elseBlock)
            {
                concat.addEolIndent(context.indent);
                CONCAT_FIXED_STR(concat, "else");
                incIndentStatement(compilerIf->elseBlock, context.indent);
                concat.addEolIndent(context.indent);
                SWAG_CHECK(output(context, concat, compilerIf->elseBlock));
                decIndentStatement(compilerIf->elseBlock, context.indent);
            }
            break;
        }

        case AstNodeKind::For:
        {
            auto forNode = CastAst<AstFor>(node, AstNodeKind::For);
            CONCAT_FIXED_STR(concat, "for ");
            SWAG_CHECK(output(context, concat, forNode->preExpression));
            CONCAT_FIXED_STR(concat, "; ");
            SWAG_CHECK(output(context, concat, forNode->boolExpression));
            CONCAT_FIXED_STR(concat, "; ");
            SWAG_CHECK(output(context, concat, forNode->postExpression));
            incIndentStatement(forNode->block, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, forNode->block));
            decIndentStatement(forNode->block, context.indent);
            break;
        }

        case AstNodeKind::Visit:
        {
            auto visitNode = CastAst<AstVisit>(node, AstNodeKind::Visit);
            CONCAT_FIXED_STR(concat, "visit ");

            bool first = true;
            for (auto& a : visitNode->aliasNames)
            {
                if (!first)
                    CONCAT_FIXED_STR(concat, ", ");
                first = false;
                concat.addString(a);
            }

            if (!visitNode->aliasNames.empty())
                CONCAT_FIXED_STR(concat, ": ");
            SWAG_CHECK(output(context, concat, visitNode->expression));
            incIndentStatement(visitNode->block, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, visitNode->block));
            decIndentStatement(visitNode->block, context.indent);
            break;
        }

        case AstNodeKind::Loop:
        {
            auto loopNode = CastAst<AstLoop>(node, AstNodeKind::Loop);
            CONCAT_FIXED_STR(concat, "loop ");
            if (loopNode->specificName)
            {
                concat.addString(loopNode->specificName->name);
                CONCAT_FIXED_STR(concat, ": ");
            }

            SWAG_CHECK(output(context, concat, loopNode->expression));
            incIndentStatement(loopNode->block, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, loopNode->block));
            decIndentStatement(loopNode->block, context.indent);
            break;
        }

        case AstNodeKind::While:
        {
            auto whileNode = CastAst<AstWhile>(node, AstNodeKind::While);
            CONCAT_FIXED_STR(concat, "while ");
            SWAG_CHECK(output(context, concat, whileNode->boolExpression));
            incIndentStatement(whileNode->block, context.indent);
            concat.addEolIndent(context.indent);
            SWAG_CHECK(output(context, concat, whileNode->block));
            decIndentStatement(whileNode->block, context.indent);
            break;
        }

        case AstNodeKind::CompilerSpecialFunction:
        {
            switch (node->token.id)
            {
            case TokenId::CompilerCallerFunction:
                CONCAT_FIXED_STR(concat, "#callerfunction");
                break;
            case TokenId::CompilerCallerLocation:
                CONCAT_FIXED_STR(concat, "#callerlocation");
                break;
            case TokenId::CompilerLocation:
                CONCAT_FIXED_STR(concat, "#location");
                break;
            case TokenId::CompilerOs:
                CONCAT_FIXED_STR(concat, "#os");
                break;
            case TokenId::CompilerBuildCfg:
                CONCAT_FIXED_STR(concat, "#cfg");
                break;
            case TokenId::CompilerHasTag:
                CONCAT_FIXED_STR(concat, "#hastag(");
                SWAG_CHECK(output(context, concat, node->childs[0]));
                CONCAT_FIXED_STR(concat, ")");
                break;
            case TokenId::CompilerTagVal:
                CONCAT_FIXED_STR(concat, "#tagval(");
                SWAG_CHECK(output(context, concat, node->childs[0]));
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(context, concat, node->childs[1]));
                CONCAT_FIXED_STR(concat, ")");
                break;
            default:
                return node->sourceFile->report({node, node->token, "Ast::output, unknown compiler function"});
            }
            break;
        }

        case AstNodeKind::ConditionalExpression:
            SWAG_CHECK(output(context, concat, node->childs[0]));
            CONCAT_FIXED_STR(concat, " ? (");
            SWAG_CHECK(output(context, concat, node->childs[1]));
            CONCAT_FIXED_STR(concat, ") : (");
            SWAG_CHECK(output(context, concat, node->childs[2]));
            concat.addChar(')');
            break;

        case AstNodeKind::Alias:
        {
            CONCAT_FIXED_STR(concat, "alias ");
            concat.addString(node->name.c_str());
            CONCAT_FIXED_STR(concat, " = ");
            SWAG_CHECK(output(context, concat, node->childs.front()));
            break;
        }

        case AstNodeKind::VarDecl:
        {
            AstVarDecl* varDecl = static_cast<AstVarDecl*>(node);

            if (varDecl->type)
            {
                CONCAT_FIXED_STR(concat, "var ");
                if (!varDecl->publicName.empty())
                    concat.addString(varDecl->publicName);
                else
                    concat.addString(varDecl->name);
                CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(output(context, concat, varDecl->type));
                if (varDecl->assignment)
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(output(context, concat, varDecl->assignment));
                }
            }
            else
            {
                if (!varDecl->publicName.empty())
                    concat.addString(varDecl->publicName);
                else
                    concat.addString(varDecl->name);
                CONCAT_FIXED_STR(concat, " := ");
                if (varDecl->assignment)
                    SWAG_CHECK(output(context, concat, varDecl->assignment));
            }

            break;
        }

        case AstNodeKind::ConstDecl:
        {
            AstVarDecl* varDecl = static_cast<AstVarDecl*>(node);
            CONCAT_FIXED_STR(concat, "const ");

            if (!varDecl->publicName.empty())
                concat.addString(varDecl->publicName);
            else
                concat.addString(varDecl->name);

            if (varDecl->type)
            {
                CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(output(context, concat, varDecl->type));
            }

            if (varDecl->assignment)
            {
                CONCAT_FIXED_STR(concat, " = ");
                SWAG_CHECK(output(context, concat, varDecl->assignment));
            }
            break;
        }

        case AstNodeKind::Using:
        {
            CONCAT_FIXED_STR(concat, "using ");
            int idx = 0;
            for (auto child : node->childs)
            {
                if (idx)
                    CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(context, concat, child));
                idx++;
            }

            break;
        }

        case AstNodeKind::Return:
        {
            CONCAT_FIXED_STR(concat, "return ");
            if (!node->childs.empty())
                SWAG_CHECK(output(context, concat, node->childs.front()));
            break;
        }

        case AstNodeKind::IntrinsicProp:
        {
            auto propertyNode = CastAst<AstIntrinsicProp>(node, AstNodeKind::IntrinsicProp);
            concat.addString(propertyNode->name);
            concat.addChar('(');
            int idx = 0;
            for (auto child : node->childs)
            {
                if (idx)
                    CONCAT_FIXED_STR(concat, ",");
                SWAG_CHECK(output(context, concat, child));
                idx++;
            }

            concat.addChar(')');
            break;
        }

        case AstNodeKind::IdentifierRef:
        {
            auto back = node->childs.back();
            if (back->resolvedSymbolName)
            {
                auto symbol = back->resolvedSymbolName;
                if (symbol && symbol->ownerTable->scope->isGlobal())
                {
                    SWAG_CHECK(output(context, concat, back));
                    break;
                }
            }

            int idx = 0;
            for (auto child : node->childs)
            {
                // No need to output some scope names because the identifier will be generated with a full name
                if (child->resolvedSymbolName &&
                    child->resolvedSymbolName->kind == SymbolKind::Namespace &&
                    child == node->childs.front() &&
                    node->childs.size() > 1)
                    continue;

                if (idx)
                    CONCAT_FIXED_STR(concat, ".");
                SWAG_CHECK(output(context, concat, child));
                idx++;
            }

            break;
        }

        case AstNodeKind::Identifier:
        case AstNodeKind::FuncCall:
        {
            auto identifier = static_cast<AstIdentifier*>(node);
            auto symbol     = identifier->resolvedSymbolName;
            auto overload   = identifier->resolvedSymbolOverload;

            if (symbol &&
                overload &&
                symbol->name[0] != '@' &&
                overload->node->ownerScope->isGlobalOrImpl() &&
                !overload->node->sourceFile->isBootstrapFile &&
                !overload->node->sourceFile->isRuntimeFile)
            {
                if (((symbol->kind == SymbolKind::Variable) && (overload->flags & OVERLOAD_VAR_GLOBAL)) ||
                    (symbol->kind == SymbolKind::Function) ||
                    (symbol->kind == SymbolKind::Alias) ||
                    (symbol->kind == SymbolKind::TypeAlias))
                {
                    SWAG_VERIFY(overload->flags & OVERLOAD_PUBLIC, identifier->sourceFile->report({identifier, identifier->token, format("identifier '%s' should be public", identifier->name.c_str())}));
                }
            }

            if (symbol && symbol->ownerTable->scope->isGlobal())
                concat.addString(symbol->getFullName());
            else
                concat.addString(node->name);

            if (identifier->genericParameters)
            {
                concat.addChar('\'');
                concat.addChar('(');
                SWAG_CHECK(output(context, concat, identifier->genericParameters));
                concat.addChar(')');
            }

            if (identifier->callParameters)
            {
                if (identifier->callParameters->flags & AST_CALL_FOR_STRUCT)
                    concat.addChar('{');
                else
                    concat.addChar('(');
                SWAG_CHECK(output(context, concat, identifier->callParameters));
                if (identifier->callParameters->flags & AST_CALL_FOR_STRUCT)
                    concat.addChar('}');
                else
                    concat.addChar(')');
            }

            break;
        }

        case AstNodeKind::StatementNoScope:
        {
            for (auto child : node->childs)
            {
                if (child->flags & AST_GENERATED)
                    continue;
                context.indent++;
                SWAG_CHECK(output(context, concat, child));
                context.indent--;
                concat.addEolIndent(context.indent);
            }

            break;
        }

        case AstNodeKind::Statement:
            if (node->childs.count == 1 &&
                node->parent->kind != AstNodeKind::FuncDecl &&
                node->childs.front()->kind != AstNodeKind::For &&
                node->childs.front()->kind != AstNodeKind::If &&
                node->childs.front()->kind != AstNodeKind::While &&
                node->childs.front()->kind != AstNodeKind::Visit &&
                node->childs.front()->kind != AstNodeKind::Loop &&
                node->childs.front()->kind != AstNodeKind::Switch)
            {
                context.indent++;
                concat.addIndent(1);
                SWAG_CHECK(output(context, concat, node->childs.front()));
                context.indent--;
            }
            else
            {
                concat.addChar('{');
                concat.addEol();
                for (auto child : node->childs)
                {
                    concat.addIndent(context.indent + 1);
                    context.indent++;
                    SWAG_CHECK(output(context, concat, child));
                    context.indent--;
                    concat.addEol();
                }

                concat.addIndent(context.indent);
                concat.addChar('}');
                concat.addEolIndent(context.indent);
            }

            break;

        case AstNodeKind::FuncCallParam:
        {
            auto funcParam = CastAst<AstFuncCallParam>(node, AstNodeKind::FuncCallParam);
            if (!funcParam->namedParam.empty())
            {
                concat.addString(funcParam->namedParam);
                concat.addChar(':');
            }

            SWAG_CHECK(output(context, concat, funcParam->childs.front()));
            break;
        }

        case AstNodeKind::FuncCallParams:
        {
            int idx = 0;
            for (auto child : node->childs)
            {
                if (idx)
                    concat.addString2(", ");
                SWAG_CHECK(output(context, concat, child));
                idx++;
            }

            break;
        }

        case AstNodeKind::SingleOp:
            concat.addString(node->token.text);
            SWAG_CHECK(output(context, concat, node->childs[0]));
            break;

        case AstNodeKind::AffectOp:
            SWAG_CHECK(output(context, concat, node->childs[0]));
            concat.addChar(' ');
            concat.addString(node->token.text);
            concat.addChar(' ');
            SWAG_CHECK(output(context, concat, node->childs[1]));
            break;

        case AstNodeKind::NullConditionalExpression:
            SWAG_CHECK(output(context, concat, node->childs[0]));
            concat.addString(" ?? ");
            SWAG_CHECK(output(context, concat, node->childs[1]));
            break;

        case AstNodeKind::FactorOp:
        case AstNodeKind::BinaryOp:
            concat.addChar('(');
            SWAG_CHECK(output(context, concat, node->childs[0]));
            concat.addChar(' ');
            concat.addString(node->token.text);
            concat.addChar(' ');
            SWAG_CHECK(output(context, concat, node->childs[1]));
            concat.addChar(')');
            break;

        case AstNodeKind::AutoCast:
            CONCAT_FIXED_STR(concat, "autocast ");
            SWAG_CHECK(output(context, concat, node->childs[0]));
            break;

        case AstNodeKind::Cast:
            CONCAT_FIXED_STR(concat, "cast(");
            SWAG_CHECK(output(context, concat, node->childs[0]));
            CONCAT_FIXED_STR(concat, ") ");
            SWAG_CHECK(output(context, concat, node->childs[1]));
            break;

        case AstNodeKind::BitCast:
            CONCAT_FIXED_STR(concat, "bitcast(");
            SWAG_CHECK(output(context, concat, node->childs[0]));
            CONCAT_FIXED_STR(concat, ") ");
            SWAG_CHECK(output(context, concat, node->childs[1]));
            break;

        case AstNodeKind::TypeExpression:
        {
            AstTypeExpression* typeNode = static_cast<AstTypeExpression*>(node);
            if (typeNode->typeFlags & TYPEFLAG_RETVAL)
            {
                CONCAT_FIXED_STR(concat, "retval");
                break;
            }

            if (typeNode->typeFlags & TYPEFLAG_ISCONST)
                CONCAT_FIXED_STR(concat, "const ");
            if (typeNode->typeFlags & TYPEFLAG_ISSLICE)
                CONCAT_FIXED_STR(concat, "[..] ");
            if (typeNode->arrayDim == UINT8_MAX)
                CONCAT_FIXED_STR(concat, "[] ");
            else if (typeNode->arrayDim)
                concat.addStringFormat("[%u] ", typeNode->arrayDim);

            for (int i = 0; i < typeNode->ptrCount; i++)
                concat.addChar('*');
            if (typeNode->identifier)
            {
                SWAG_CHECK(output(context, concat, typeNode->identifier));
            }
            else
            {
                auto literalType = typeNode->literalType;
                if (!literalType)
                    literalType = TypeManager::literalTypeToType(typeNode->token);
                SWAG_ASSERT(literalType);
                SWAG_ASSERT(!literalType->name.empty());
                concat.addString(literalType->name);
            }

            break;
        }

        case AstNodeKind::Literal:
            SWAG_CHECK(outputLiteral(context, concat, node, TypeManager::literalTypeToType(node->token), node->token.text, node->token.literalValue));
            break;

        default:
            return node->sourceFile->report({node, node->token, "Ast::output, unknown node"});
        }

        return true;
    }
}; // namespace Ast
