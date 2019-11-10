#include "pch.h"
#include "Ast.h"
#include "Scope.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Utf8Crc.h"
#include "Concat.h"

namespace Ast
{
    bool outputLiteral(Concat& concat, AstNode* node, TypeInfo* typeInfo, const Utf8& text, Register& reg)
    {
        auto str = literalToString(typeInfo, text, reg);
        if (typeInfo->isNative(NativeTypeKind::String))
            concat.addString("\"" + str + "\"");
        else
            concat.addString(str);
        return true;
    }

    bool output(Concat& concat, AstNode* node, int indent)
    {
        if (!node)
            return true;

        switch (node->kind)
        {
        case AstNodeKind::CompilerIfBlock:
            SWAG_CHECK(output(concat, node->childs.front(), indent));
            break;

        case AstNodeKind::CompilerIf:
        {
            auto compilerIf = CastAst<AstIf>(node, AstNodeKind::CompilerIf);
            if (compilerIf->flags & AST_COMPILER_IF_DONE)
            {
                if (compilerIf->ifBlock->flags & AST_DISABLED)
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
			CONCAT_FIXED_STR(concat, ")");
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
            }
            else
            {
                concat.addString(node->name);
				CONCAT_FIXED_STR(concat, " := ");
            }

            if (varDecl->assignment)
                SWAG_CHECK(output(concat, varDecl->assignment));
            break;
        }

        case AstNodeKind::Return:
        {
			CONCAT_FIXED_STR(concat, "return ");
            if (!node->childs.empty())
                SWAG_CHECK(output(concat, node->childs.front()));
            break;
        }

        case AstNodeKind::Identifier:
        case AstNodeKind::FuncCall:
        {
            AstIdentifier* identifier = static_cast<AstIdentifier*>(node);
            concat.addString(node->name);
            if (identifier->genericParameters)
            {
				CONCAT_FIXED_STR(concat, "!(");
                SWAG_CHECK(output(concat, identifier->genericParameters));
				CONCAT_FIXED_STR(concat, ")");
            }

            if (identifier->callParameters)
            {
				CONCAT_FIXED_STR(concat, "(");
                SWAG_CHECK(output(concat, identifier->callParameters));
				CONCAT_FIXED_STR(concat, ")");
            }

            break;
        }

        case AstNodeKind::Statement:
			CONCAT_FIXED_STR(concat, "{");
            concat.addEolIndent(indent);
            for (auto child : node->childs)
            {
                concat.addIndent(1);
                SWAG_CHECK(output(concat, child, indent + 1));
                concat.addEolIndent(indent);
            }

			CONCAT_FIXED_STR(concat, "}");
            concat.addEolIndent(indent);
            break;

        case AstNodeKind::FuncCallParam:
        {
            auto funcParam = CastAst<AstFuncCallParam>(node, AstNodeKind::FuncCallParam);
            if (!funcParam->namedParam.empty())
            {
                concat.addString(funcParam->namedParam);
				CONCAT_FIXED_STR(concat, ": ");
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
					CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(concat, child));
                idx++;
            }

            break;
        }

        case AstNodeKind::IdentifierRef:
        {
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

        case AstNodeKind::SingleOp:
            concat.addString(node->token.text);
            SWAG_CHECK(output(concat, node->childs[0]));
            break;

        case AstNodeKind::AffectOp:
            SWAG_CHECK(output(concat, node->childs[0]));
			CONCAT_FIXED_STR(concat, " ");
            concat.addString(node->token.text);
			CONCAT_FIXED_STR(concat, " ");
            SWAG_CHECK(output(concat, node->childs[1]));
            break;

        case AstNodeKind::FactorOp:
        case AstNodeKind::BinaryOp:
			CONCAT_FIXED_STR(concat, "(");
            SWAG_CHECK(output(concat, node->childs[0]));
			CONCAT_FIXED_STR(concat, " ");
            concat.addString(node->token.text);
			CONCAT_FIXED_STR(concat, " ");
            SWAG_CHECK(output(concat, node->childs[1]));
			CONCAT_FIXED_STR(concat, ")");
            break;

        case AstNodeKind::Cast:
			CONCAT_FIXED_STR(concat, "cast(");
            SWAG_CHECK(output(concat, node->childs[0]));
			CONCAT_FIXED_STR(concat, ") ");
            SWAG_CHECK(output(concat, node->childs[1]));
            break;

        case AstNodeKind::TypeExpression:
        {
            AstTypeExpression* typeNode = static_cast<AstTypeExpression*>(node);
            if (typeNode->isConst)
				CONCAT_FIXED_STR(concat, "const ");
            for (int i = 0; i < typeNode->ptrCount; i++)
				CONCAT_FIXED_STR(concat, "*");
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
				CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(output(concat, node->childs[1]));
            }

			CONCAT_FIXED_STR(concat, ")");
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
