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

        default:
            return node->sourceFile->report({node, "Ast::output, unknown node"});
        }

        return true;
    }
}; // namespace Ast
