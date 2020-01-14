#include "pch.h"
#include "Ast.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "TypeManager.h"

namespace Ast
{
    bool outputLiteral(Concat& concat, AstNode* node, TypeInfo* typeInfo, const Utf8& text, Register& reg)
    {
        if (typeInfo == g_TypeMgr.typeInfoNull)
        {
            CONCAT_FIXED_STR(concat, "null");
            return true;
        }

        if (typeInfo->kind == TypeInfoKind::TypeList)
        {
            SWAG_CHECK(Ast::output(concat, node));
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

    bool output(Concat& concat, AstNode* node, int indent)
    {
        if (!node)
            return true;

        if (node->flags & AST_IDENTIFIER_BACKTICK)
            concat.addChar('`');

        switch (node->kind)
        {
        case AstNodeKind::MakePointer:
            concat.addChar('&');
            SWAG_CHECK(output(concat, node->childs.front(), indent));
            break;

        case AstNodeKind::RawMove:
            if (node->flags & AST_NO_DROP)
                concat.addString("nodrop ");
            if (node->flags & AST_FORCE_MOVE)
                concat.addString("move ");
            SWAG_CHECK(output(concat, node->childs.front(), indent));
            break;

        case AstNodeKind::ArrayPointerIndex:
        {
            auto arrayNode = CastAst<AstPointerDeRef>(node, AstNodeKind::ArrayPointerIndex);
            SWAG_CHECK(output(concat, arrayNode->array, indent));
            concat.addChar('[');
            SWAG_CHECK(output(concat, arrayNode->access, indent));
            concat.addChar(']');
            break;
        }

        case AstNodeKind::ExpressionList:
        {
            auto exprNode = CastAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
            if (exprNode->listKind == TypeInfoListKind::Curly)
                concat.addChar('{');
            else if (exprNode->listKind == TypeInfoListKind::Bracket)
                concat.addChar('[');

            int idx = 0;
            for (auto child : exprNode->childs)
            {
                if (idx++)
                    concat.addChar(',');
                SWAG_CHECK(output(concat, child, indent));
            }

            if (exprNode->listKind == TypeInfoListKind::Curly)
                concat.addChar('}');
            else if (exprNode->listKind == TypeInfoListKind::Bracket)
                concat.addChar(']');
            break;
        }

        case AstNodeKind::CompilerIfBlock:
            SWAG_CHECK(output(concat, node->childs.front(), indent));
            break;

        case AstNodeKind::CompilerMacro:
            CONCAT_FIXED_STR(concat, "#macro");
            concat.addEolIndent(indent);
            SWAG_CHECK(output(concat, node->childs.front(), indent));
            break;

        case AstNodeKind::CompilerInline:
            CONCAT_FIXED_STR(concat, "#inline");
            concat.addEolIndent(indent);
            SWAG_CHECK(output(concat, node->childs.front(), indent));
            break;

        case AstNodeKind::CompilerMixin:
            CONCAT_FIXED_STR(concat, "#mixin ");
            SWAG_CHECK(output(concat, node->childs.front(), indent));
            break;

        case AstNodeKind::CompilerPrint:
            CONCAT_FIXED_STR(concat, "#print ");
            SWAG_CHECK(output(concat, node->childs.front(), indent));
            break;

        case AstNodeKind::CompilerIf:
        {
            auto compilerIf = CastAst<AstIf>(node, AstNodeKind::CompilerIf);
            if (compilerIf->flags & AST_COMPILER_IF_DONE)
            {
                if (compilerIf->ifBlock->flags & AST_NO_SEMANTIC)
                    SWAG_CHECK(output(concat, compilerIf->elseBlock, indent));
                else
                    SWAG_CHECK(output(concat, compilerIf->ifBlock, indent));
            }
            else
            {
                CONCAT_FIXED_STR(concat, "#if ");
                SWAG_CHECK(output(concat, compilerIf->boolExpression, indent));
                concat.addEolIndent(indent);
                SWAG_CHECK(output(concat, compilerIf->ifBlock, indent));
                if (compilerIf->elseBlock)
                {
                    concat.addEolIndent(indent);
                    CONCAT_FIXED_STR(concat, "#else");
                    concat.addEolIndent(indent);
                    SWAG_CHECK(output(concat, compilerIf->elseBlock, indent));
                }
            }
            break;
        }

        case AstNodeKind::If:
        {
            auto compilerIf = CastAst<AstIf>(node, AstNodeKind::If, AstNodeKind::CompilerIf);
            CONCAT_FIXED_STR(concat, "if ");
            SWAG_CHECK(output(concat, compilerIf->boolExpression, indent));
            concat.addEolIndent(indent);
            SWAG_CHECK(output(concat, compilerIf->ifBlock, indent));
            if (compilerIf->elseBlock)
            {
                concat.addEolIndent(indent);
                CONCAT_FIXED_STR(concat, "else");
                concat.addEolIndent(indent);
                SWAG_CHECK(output(concat, compilerIf->elseBlock, indent));
            }
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

            SWAG_CHECK(output(concat, loopNode->expression, indent));
            concat.addEolIndent(indent);
            SWAG_CHECK(output(concat, loopNode->block, indent));
            break;
        }

        case AstNodeKind::While:
        {
            auto whileNode = CastAst<AstWhile>(node, AstNodeKind::While);
            CONCAT_FIXED_STR(concat, "while ");
            SWAG_CHECK(output(concat, whileNode->boolExpression, indent));
            concat.addEolIndent(indent);
            SWAG_CHECK(output(concat, whileNode->block, indent));
            break;
        }

        case AstNodeKind::CompilerSpecialFunction:
        {
            switch (node->token.id)
            {
            case TokenId::CompilerCallerFile:
                CONCAT_FIXED_STR(concat, "#callerfile");
                break;
            case TokenId::CompilerCallerLine:
                CONCAT_FIXED_STR(concat, "#callerline");
                break;
            case TokenId::CompilerCallerFunction:
                CONCAT_FIXED_STR(concat, "#callerfunction");
                break;
            default:
                return node->sourceFile->report({node, node->token, "Ast::output, unknown compiler function"});
            }
            break;
        }

        case AstNodeKind::QuestionExpression:
            SWAG_CHECK(output(concat, node->childs[0]));
            CONCAT_FIXED_STR(concat, " ? (");
            SWAG_CHECK(output(concat, node->childs[1]));
            CONCAT_FIXED_STR(concat, ") : (");
            SWAG_CHECK(output(concat, node->childs[2]));
            concat.addChar(')');
            break;

        case AstNodeKind::VarDecl:
        {
            AstVarDecl* varDecl = static_cast<AstVarDecl*>(node);
            if (varDecl->type)
            {
                CONCAT_FIXED_STR(concat, "var ");
                concat.addString(node->name);
                CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(output(concat, varDecl->type));
                if (varDecl->assignment)
                {
                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(output(concat, varDecl->assignment));
                }
            }
            else
            {
                concat.addString(node->name);
                CONCAT_FIXED_STR(concat, " := ");
                if (varDecl->assignment)
                    SWAG_CHECK(output(concat, varDecl->assignment));
            }

            break;
        }

        case AstNodeKind::LetDecl:
        case AstNodeKind::ConstDecl:
        {
            AstVarDecl* varDecl = static_cast<AstVarDecl*>(node);
            switch (node->kind)
            {
            case AstNodeKind::LetDecl:
                CONCAT_FIXED_STR(concat, "let ");
                break;
            case AstNodeKind::ConstDecl:
                CONCAT_FIXED_STR(concat, "const ");
                break;
            }

            concat.addString(node->name);
            if (varDecl->type)
            {
                CONCAT_FIXED_STR(concat, ": ");
                SWAG_CHECK(output(concat, varDecl->type));
            }

            if (varDecl->assignment)
            {
                CONCAT_FIXED_STR(concat, " = ");
                SWAG_CHECK(output(concat, varDecl->assignment));
            }
            break;
        }

        case AstNodeKind::Return:
        {
            CONCAT_FIXED_STR(concat, "return ");
            if (!node->childs.empty())
                SWAG_CHECK(output(concat, node->childs.front()));
            break;
        }

        case AstNodeKind::IntrinsicProp:
        {
            auto propertyNode = CastAst<AstProperty>(node, AstNodeKind::IntrinsicProp);
            concat.addString(propertyNode->name);
            concat.addChar('(');
            SWAG_CHECK(output(concat, propertyNode->expression));
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
                    SWAG_CHECK(output(concat, back));
                    break;
                }
            }

            int idx = 0;
            for (auto child : node->childs)
            {
                if (idx)
                    CONCAT_FIXED_STR(concat, ".");
                SWAG_CHECK(output(concat, child));
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
            if (symbol && overload && symbol->name[0] != '@')
            {
                if (((symbol->kind == SymbolKind::Variable) && (overload->flags & OVERLOAD_VAR_GLOBAL)) ||
                    (symbol->kind == SymbolKind::Function) ||
                    (symbol->kind == SymbolKind::TypeAlias))
                {
                    SWAG_VERIFY(overload->flags & OVERLOAD_PUBLIC, identifier->sourceFile->report({identifier, identifier->token, format("identifier '%s' should be public", identifier->name.c_str())}));
                }
            }

            if (symbol && symbol->ownerTable->scope->isGlobal())
                concat.addString(symbol->fullName);
            else
                concat.addString(node->name);

            if (identifier->genericParameters)
            {
                concat.addChar('!');
                concat.addChar('(');
                SWAG_CHECK(output(concat, identifier->genericParameters));
                concat.addChar(')');
            }

            if (identifier->callParameters)
            {
                concat.addChar('(');
                SWAG_CHECK(output(concat, identifier->callParameters));
                concat.addChar(')');
            }

            break;
        }

        case AstNodeKind::Statement:
            concat.addChar('{');
            concat.addEolIndent(indent);
            for (auto child : node->childs)
            {
                concat.addIndent(1);
                SWAG_CHECK(output(concat, child, indent + 1));
                concat.addEolIndent(indent);
            }

            concat.addChar('}');
            concat.addEolIndent(indent);
            break;

        case AstNodeKind::FuncCallParam:
        {
            auto funcParam = CastAst<AstFuncCallParam>(node, AstNodeKind::FuncCallParam);
            if (!funcParam->namedParam.empty())
            {
                concat.addString(funcParam->namedParam);
                concat.addChar(':');
            }

            SWAG_CHECK(output(concat, funcParam->childs.front()));
            break;
        }

        case AstNodeKind::FuncCallParams:
        {
            int idx = 0;
            for (auto child : node->childs)
            {
                if (idx)
                    concat.addChar(',');
                SWAG_CHECK(output(concat, child));
                idx++;
            }

            break;
        }

        case AstNodeKind::SingleOp:
            concat.addString(node->token.text);
            SWAG_CHECK(output(concat, node->childs[0]));
            break;

        case AstNodeKind::AffectOp:
            SWAG_CHECK(output(concat, node->childs[0]));
            concat.addChar(' ');
            concat.addString(node->token.text);
            concat.addChar(' ');
            SWAG_CHECK(output(concat, node->childs[1]));
            break;

        case AstNodeKind::FactorOp:
        case AstNodeKind::BinaryOp:
            concat.addChar('(');
            SWAG_CHECK(output(concat, node->childs[0]));
            concat.addChar(' ');
            concat.addString(node->token.text);
            concat.addChar(' ');
            SWAG_CHECK(output(concat, node->childs[1]));
            concat.addChar(')');
            break;

        case AstNodeKind::AutoCast:
            CONCAT_FIXED_STR(concat, "acast ");
            SWAG_CHECK(output(concat, node->childs[0]));
            break;

        case AstNodeKind::Cast:
            CONCAT_FIXED_STR(concat, "cast(");
            SWAG_CHECK(output(concat, node->childs[0]));
            concat.addChar(')');
            SWAG_CHECK(output(concat, node->childs[1]));
            break;

        case AstNodeKind::TypeExpression:
        {
            AstTypeExpression* typeNode = static_cast<AstTypeExpression*>(node);
            if (typeNode->isConst)
                CONCAT_FIXED_STR(concat, "const ");
            if (typeNode->isSlice)
                CONCAT_FIXED_STR(concat, "[..] ");
            if (typeNode->arrayDim == UINT32_MAX)
                CONCAT_FIXED_STR(concat, "[] ");
            else if (typeNode->arrayDim)
                concat.addStringFormat("[%u] ", typeNode->arrayDim);

            for (int i = 0; i < typeNode->ptrCount; i++)
                concat.addChar('*');
            if (typeNode->identifier)
                SWAG_CHECK(output(concat, typeNode->identifier));
            else
                concat.addString(node->token.literalType->name);
            break;
        }

        case AstNodeKind::CompilerAssert:
            CONCAT_FIXED_STR(concat, "#assert(");
            SWAG_CHECK(output(concat, node->childs[0]));
            if (node->childs.size() > 1)
            {
                concat.addChar(',');
                SWAG_CHECK(output(concat, node->childs[1]));
            }

            concat.addChar(')');
            break;

        case AstNodeKind::Literal:
            SWAG_CHECK(outputLiteral(concat, node, node->token.literalType, node->token.text, node->token.literalValue));
            break;

        default:
            return node->sourceFile->report({node, node->token, "Ast::output, unknown node"});
        }

        return true;
    }
}; // namespace Ast
