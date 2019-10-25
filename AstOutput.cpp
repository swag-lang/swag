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
    bool output(Concat& concat, AstNode* node)
    {
        switch (node->kind)
        {
        case AstNodeKind::VarDecl:
        {
            AstVarDecl* varDecl = static_cast<AstVarDecl*>(node);
			if (varDecl->type)
			{
                concat.addString("var ");
                concat.addString(node->name);
				concat.addString(": ");
				SWAG_CHECK(output(concat, varDecl->type));
			}
			else
			{
                concat.addString(node->name);
				concat.addString(" := ");
			}

			if (varDecl->assignment)
				SWAG_CHECK(output(concat, varDecl->assignment));
            break;
        }

        case AstNodeKind::Return:
        {
            concat.addString("return ");
            if (!node->childs.empty())
                SWAG_CHECK(output(concat, node->childs.front()));
            concat.addString(";\n");
            break;
        }

        case AstNodeKind::Identifier:
        {
            AstIdentifier* identifier = static_cast<AstIdentifier*>(node);
            concat.addString(node->name);
            if (identifier->genericParameters)
            {
                concat.addString("!(");
                SWAG_CHECK(output(concat, identifier->genericParameters));
                concat.addString(")");
            }

            if (identifier->callParameters)
            {
                concat.addString("(");
                SWAG_CHECK(output(concat, identifier->callParameters));
                concat.addString(")");
            }

            break;
        }

        case AstNodeKind::Statement:
            concat.addString("{\n");
            for (auto child : node->childs)
            {
                SWAG_CHECK(output(concat, child));
                concat.addString(";\n");
            }

            concat.addString("}\n");
            break;

        case AstNodeKind::FuncCallParam:
            SWAG_CHECK(output(concat, node->childs.front()));
            break;
        case AstNodeKind::FuncCallParams:
        {
            int idx = 0;
            for (auto child : node->childs)
            {
                if (idx)
                    concat.addString(", ");
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
                    concat.addString(".");
                SWAG_CHECK(output(concat, child));
                idx++;
            }

            break;
        }

        case AstNodeKind::FactorOp:
            concat.addString("(");
            SWAG_CHECK(output(concat, node->childs[0]));
            concat.addString(" ");
            concat.addString(node->token.text);
            concat.addString(" ");
            SWAG_CHECK(output(concat, node->childs[1]));
            concat.addString(")");
            break;

        case AstNodeKind::Cast:
            concat.addString("cast(");
            SWAG_CHECK(output(concat, node->childs[0]));
            concat.addString(") ");
            SWAG_CHECK(output(concat, node->childs[1]));
            break;

        case AstNodeKind::TypeExpression:
        {
            AstTypeExpression* typeNode = static_cast<AstTypeExpression*>(node);
            if (typeNode->isConst)
                concat.addString("const ");
            for (int i = 0; i < typeNode->ptrCount; i++)
                concat.addString("*");
            if (typeNode->identifier)
                SWAG_CHECK(output(concat, typeNode->identifier));
            else
                concat.addString(node->token.literalType->name);
            break;
        }

        case AstNodeKind::Literal:
            concat.addString(node->token.text);
            break;

        default:
            return node->sourceFile->report({node, node->token, "Ast::output, unknown node"});
        }

        return true;
    }
}; // namespace Ast
