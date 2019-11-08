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
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::U8:
            concat.addString(format("%u", reg.u8));
            break;
        case NativeTypeKind::U16:
            concat.addString(format("%u", reg.u16));
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            concat.addString(format("%u", reg.u32));
            break;
        case NativeTypeKind::U64:
            concat.addString(format("%llu", reg.u64));
            break;
        case NativeTypeKind::S8:
            concat.addString(format("%d", reg.s8));
            break;
        case NativeTypeKind::S16:
            concat.addString(format("%d", reg.s16));
            break;
        case NativeTypeKind::S32:
            concat.addString(format("%d", reg.s32));
            break;
        case NativeTypeKind::S64:
            concat.addString(format("%lld", reg.s64));
            break;
        case NativeTypeKind::F32:
            concat.addString(toStringF64(reg.f32));
            break;
        case NativeTypeKind::F64:
            concat.addString(toStringF64(reg.f64));
            break;
        case NativeTypeKind::Bool:
            concat.addString(reg.b ? "true" : "false");
            break;
        case NativeTypeKind::String:
            concat.addString("\"" + text + "\"");
            break;
        default:
            return node->sourceFile->report({node, node->token, "Ast::outputLiteral, unknown type"});
        }

        return true;
    }

    bool output(Concat& concat, AstNode* node, int indent)
    {
        if (!node)
            return true;

        switch (node->kind)
        {
        case AstNodeKind::CompilerIf:
        {
            auto compilerIf = CastAst<AstIf>(node, AstNodeKind::CompilerIf);
            if (compilerIf->ifBlock->flags & AST_DISABLED)
                SWAG_CHECK(output(concat, compilerIf->elseBlock, indent));
            else
                SWAG_CHECK(output(concat, compilerIf->ifBlock, indent));
            break;
        }
        case AstNodeKind::CompilerIfBlock:
            SWAG_CHECK(output(concat, node->childs.front(), indent));
            break;

        case AstNodeKind::If:
        {
            auto compilerIf = CastAst<AstIf>(node, AstNodeKind::If);
            concat.addString("if ");
            SWAG_CHECK(output(concat, compilerIf->boolExpression, indent));
            concat.addEolIndent(indent);
            SWAG_CHECK(output(concat, compilerIf->ifBlock, indent));
            if (compilerIf->elseBlock)
            {
                concat.addString("else");
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
                concat.addString("#callerfile");
                break;
            case TokenId::CompilerCallerLine:
                concat.addString("#callerline");
                break;
            case TokenId::CompilerCallerFunction:
                concat.addString("#callerfunction");
                break;
            default:
                return node->sourceFile->report({node, node->token, "Ast::output, unknown compiler function"});
            }
            break;
        }

        case AstNodeKind::QuestionExpression:
            SWAG_CHECK(output(concat, node->childs[0]));
            concat.addString(" ? (");
            SWAG_CHECK(output(concat, node->childs[1]));
            concat.addString(") : (");
            SWAG_CHECK(output(concat, node->childs[2]));
            concat.addString(")");
            break;

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
            break;
        }

        case AstNodeKind::Identifier:
        case AstNodeKind::FuncCall:
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
            concat.addString("{");
            concat.addEolIndent(indent);
            for (auto child : node->childs)
            {
                concat.addIndent(1);
                SWAG_CHECK(output(concat, child, indent + 1));
                concat.addEolIndent(indent);
            }

            concat.addString("}");
            concat.addEolIndent(indent);
            break;

        case AstNodeKind::FuncCallParam:
        {
            auto funcParam = CastAst<AstFuncCallParam>(node, AstNodeKind::FuncCallParam);
            if (!funcParam->namedParam.empty())
            {
                concat.addString(funcParam->namedParam);
                concat.addString(": ");
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

        case AstNodeKind::AffectOp:
            SWAG_CHECK(output(concat, node->childs[0]));
            concat.addString(" ");
            concat.addString(node->token.text);
            concat.addString(" ");
            SWAG_CHECK(output(concat, node->childs[1]));
            break;

        case AstNodeKind::FactorOp:
        case AstNodeKind::BinaryOp:
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
            SWAG_CHECK(outputLiteral(concat, node, node->token.literalType, node->token.text, node->token.literalValue));
            break;

        default:
            return node->sourceFile->report({node, node->token, "Ast::output, unknown node"});
        }

        return true;
    }
}; // namespace Ast
