#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "FormatAst.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Report.h"
#include "Semantic.h"

bool FormatAst::outputNode(const AstNode* node)
{
    node = convertNode(node);
    if (!node)
        return true;

    beautifyCommentBefore(node);
    beautifyBlankLine(node);
    beautifyCommentJustBefore(node);

    // Prepend some stuff
    const auto isNamed = node->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
    if (isNamed)
    {
        concat->addString(isNamed->token.text);
        concat->addChar(':');
        concat->addBlank();
    }

    switch (node->kind)
    {
        case AstNodeKind::SwitchCaseBlock:
        case AstNodeKind::StatementNoScope:
        case AstNodeKind::File:
            SWAG_CHECK(outputChildren(node));
            break;

        case AstNodeKind::ExplicitNoInit:
            CONCAT_FIXED_STR(concat, "undefined");
            break;

        case AstNodeKind::Index:
            CONCAT_FIXED_STR(concat, "#index");
            break;

        case AstNodeKind::GetErr:
            CONCAT_FIXED_STR(concat, "@err");
            break;

        case AstNodeKind::CompilerLoad:
            CONCAT_FIXED_STR(concat, "#load");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::CompilerInclude:
            CONCAT_FIXED_STR(concat, "#include");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::KeepRef:
            CONCAT_FIXED_STR(concat, "ref");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::MoveRef:
            CONCAT_FIXED_STR(concat, "moveref");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::CompilerImport:
            CONCAT_FIXED_STR(concat, "#import");
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addEol();
            break;

        case AstNodeKind::FallThrough:
            CONCAT_FIXED_STR(concat, "fallthrough");
            concat->addEol();
            break;

        case AstNodeKind::Unreachable:
            CONCAT_FIXED_STR(concat, "unreachable");
            concat->addEol();
            break;

        case AstNodeKind::CompilerPlaceHolder:
            CONCAT_FIXED_STR(concat, "#placeholder");
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addEol();
            break;

        case AstNodeKind::CompilerForeignLib:
            CONCAT_FIXED_STR(concat, "#foreignlib");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addEol();
            break;

        case AstNodeKind::CompilerCode:
            concat->addChar(')');
            concat->addEol();
            concat->addIndent(indent);
            concat->addChar('{');
            concat->addEol();
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addEol();
            concat->addIndent(indent);
            concat->addChar('}');
            concat->addEol();
            concat->addIndent(indent);
            break;

        case AstNodeKind::With:
            CONCAT_FIXED_STR(concat, "with");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addEol();
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(node->secondChild()));
            break;

        case AstNodeKind::Throw:
            concat->addString(node->token.text);
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::Try:
        case AstNodeKind::Assume:
            if (node->hasSpecFlag(AstTryCatchAssume::SPEC_FLAG_GENERATED) && node->hasSpecFlag(AstTryCatchAssume::SPEC_FLAG_BLOCK))
            {
                indent++;
                outputChildren(node->firstChild());
                indent--;
            }
            else
            {
                if (node->hasAstFlag(AST_DISCARD))
                {
                    CONCAT_FIXED_STR(concat, "discard");
                    concat->addBlank();
                }

                concat->addString(node->token.text);
                concat->addBlank();
                SWAG_CHECK(outputNode(node->firstChild()));
            }
            break;

        case AstNodeKind::Catch:
        case AstNodeKind::TryCatch:
            if (node->hasAstFlag(AST_DISCARD))
            {
                CONCAT_FIXED_STR(concat, "discard");
                concat->addBlank();
            }

            concat->addString(node->token.text);
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::FuncCallParam:
            SWAG_CHECK(outputNode(node->firstChild()));
            break;
        case AstNodeKind::FuncCallParams:
            SWAG_CHECK(outputFuncCallParams(node));
            break;

        case AstNodeKind::FuncDeclParams:
            concat->addChar('(');
            outputCommaChildren(node);
            concat->addChar(')');
            break;

        case AstNodeKind::FuncDecl:
            SWAG_CHECK(outputFuncDecl(castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl)));
            break;

        case AstNodeKind::AttrDecl:
        {
            const auto attrDecl = castAst<AstAttrDecl>(node, AstNodeKind::AttrDecl);
            CONCAT_FIXED_STR(concat, "attr");
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addChar('(');
            outputCommaChildren(attrDecl->parameters);
            concat->addChar(')');
            concat->addEol();
            break;
        }

        case AstNodeKind::EnumType:
            if (!node->children.empty())
                SWAG_CHECK(outputNode(node->firstChild()));
            break;
        case AstNodeKind::EnumValue:
            concat->addString(node->token.text);
            if (node->childCount())
            {
                concat->addBlank();
                concat->addChar('=');
                concat->addBlank();
                SWAG_CHECK(outputNode(node->firstChild()));
            }
            break;

        case AstNodeKind::EnumDecl:
            SWAG_CHECK(outputEnum(castAst<AstEnum>(node, AstNodeKind::EnumDecl)));
            break;

        case AstNodeKind::StructContent:
            SWAG_CHECK(outputStructDeclContent(node));
            break;
        case AstNodeKind::TupleContent:
            SWAG_CHECK(outputTupleDeclContent(node));
            break;

        case AstNodeKind::StructDecl:
        case AstNodeKind::InterfaceDecl:
            SWAG_CHECK(outputStructDecl(castAst<AstStruct>(node, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl)));
            break;

        case AstNodeKind::Defer:
        {
            const auto deferNode = castAst<AstDefer>(node, AstNodeKind::Defer);
            CONCAT_FIXED_STR(concat, "defer");
            switch (deferNode->deferKind)
            {
                case DeferKind::Error:
                    concat->addChar('(');
                    CONCAT_FIXED_STR(concat, "err");
                    concat->addChar(')');
                    break;
                case DeferKind::NoError:
                    concat->addChar('(');
                    CONCAT_FIXED_STR(concat, "noerr");
                    concat->addChar(')');
                    break;
            }

            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addEol();
            break;
        }

        case AstNodeKind::AttrUse:
        {
            bool hasSomething = true;
            SWAG_CHECK(outputAttrUse(node, hasSomething));
            if (!hasSomething)
                break;
            concat->addEol();
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(castAst<AstAttrUse>(node, AstNodeKind::AttrUse)->content));
            break;
        }

        case AstNodeKind::Init:
            CONCAT_FIXED_STR(concat, "@init(");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() == 2)
            {
                concat->addChar(',');
                concat->addBlank();
                SWAG_CHECK(outputNode(node->lastChild()));
            }
            concat->addChar(')');
            break;

        case AstNodeKind::Drop:
            CONCAT_FIXED_STR(concat, "@drop(");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() == 2)
            {
                concat->addChar(',');
                concat->addBlank();
                SWAG_CHECK(outputNode(node->lastChild()));
            }
            concat->addChar(')');
            break;

        case AstNodeKind::PostMove:
            CONCAT_FIXED_STR(concat, "@postmove(");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() == 2)
            {
                concat->addChar(',');
                concat->addBlank();
                SWAG_CHECK(outputNode(node->lastChild()));
            }
            concat->addChar(')');
            break;

        case AstNodeKind::PostCopy:
            CONCAT_FIXED_STR(concat, "@postcopy(");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() == 2)
            {
                concat->addChar(',');
                concat->addBlank();
                SWAG_CHECK(outputNode(node->lastChild()));
            }
            concat->addChar(')');
            break;

        case AstNodeKind::Break:
        {
            const auto breakNode = castAst<AstBreakContinue>(node, AstNodeKind::Break);
            CONCAT_FIXED_STR(concat, "break");
            if (!breakNode->label.text.empty())
            {
                concat->addBlank();
                concat->addString(breakNode->label.text);
            }
            break;
        }

        case AstNodeKind::Continue:
        {
            const auto continueNode = castAst<AstBreakContinue>(node, AstNodeKind::Continue);
            CONCAT_FIXED_STR(concat, "continue");
            if (!continueNode->label.text.empty())
            {
                concat->addBlank();
                concat->addString(continueNode->label.text);
            }
            break;
        }

        case AstNodeKind::MakePointer:
            concat->addChar('&');
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::MakePointerLambda:
            SWAG_CHECK(outputLambdaExpression(castAst<AstMakePointer>(node, AstNodeKind::MakePointerLambda)->lambda));
            break;

        case AstNodeKind::NoDrop:
            CONCAT_FIXED_STR(concat, "nodrop");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::Move:
            CONCAT_FIXED_STR(concat, "move");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::ArrayPointerSlicing:
        {
            const auto arrayNode = castAst<AstArrayPointerSlicing>(node, AstNodeKind::ArrayPointerSlicing);
            SWAG_CHECK(outputNode(arrayNode->array));
            concat->addChar('[');
            SWAG_CHECK(outputNode(arrayNode->lowerBound));
            if (arrayNode->hasSpecFlag(AstArrayPointerSlicing::SPEC_FLAG_EXCLUDE_UP))
                CONCAT_FIXED_STR(concat, "..<");
            else
                CONCAT_FIXED_STR(concat, "..");
            SWAG_CHECK(outputNode(arrayNode->upperBound));
            concat->addChar(']');
            break;
        }

        case AstNodeKind::ArrayPointerIndex:
        {
            const auto arrayNode = castAst<AstArrayPointerIndex>(node, AstNodeKind::ArrayPointerIndex);
            if (arrayNode->hasSpecFlag(AstArrayPointerIndex::SPEC_FLAG_IS_DEREF))
            {
                CONCAT_FIXED_STR(concat, "dref");
                concat->addBlank();
                SWAG_CHECK(outputNode(arrayNode->array));
            }
            else
            {
                SWAG_CHECK(outputNode(arrayNode->array));
                concat->addChar('[');
                SWAG_CHECK(outputNode(arrayNode->access));
                concat->addChar(']');
            }
            break;
        }

        case AstNodeKind::ExpressionList:
        {
            const auto exprNode = castAst<AstExpressionList>(node, AstNodeKind::ExpressionList);
            if (exprNode->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
                concat->addChar('{');
            else
                concat->addChar('[');
            SWAG_CHECK(outputCommaChildren(exprNode));
            if (exprNode->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
                concat->addChar('}');
            else
                concat->addChar(']');
            break;
        }

        case AstNodeKind::CompilerRun:
        case AstNodeKind::CompilerRunExpression:
        case AstNodeKind::CompilerAst:
        case AstNodeKind::CompilerValidIf:
        case AstNodeKind::CompilerValidIfx:
            SWAG_CHECK(outputCompilerExpr(node));
            break;

        case AstNodeKind::CompilerIf:
            SWAG_CHECK(outputCompilerIf("#if", node));
            break;

        case AstNodeKind::If:
            SWAG_CHECK(outputIf("if", node));
            break;

        case AstNodeKind::CompilerMacro:
            CONCAT_FIXED_STR(concat, "#macro");
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::CompilerMixin:
            SWAG_CHECK(outputCompilerMixin(node));
            break;

        case AstNodeKind::CompilerPrint:
            CONCAT_FIXED_STR(concat, "#print");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::CompilerError:
            CONCAT_FIXED_STR(concat, "#error");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::CompilerWarning:
            CONCAT_FIXED_STR(concat, "#warning");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::CompilerAssert:
            CONCAT_FIXED_STR(concat, "#assert");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() > 1)
            {
                concat->addChar(',');
                concat->addBlank();
                SWAG_CHECK(outputNode(node->secondChild()));
            }
            break;

        case AstNodeKind::For:
            SWAG_CHECK(outputFor(node));
            break;

        case AstNodeKind::Visit:
            SWAG_CHECK(outputVisit(node));
            break;

        case AstNodeKind::Loop:
            SWAG_CHECK(outputLoop(node));
            break;

        case AstNodeKind::While:
            SWAG_CHECK(outputWhile(node));
            break;

        case AstNodeKind::CompilerSpecialValue:
            SWAG_CHECK(outputCompilerSpecialValue(node));
            break;

        case AstNodeKind::ConditionalExpression:
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addBlank();
            concat->addChar('?');
            concat->addBlank();
            SWAG_CHECK(outputNode(node->secondChild()));
            concat->addBlank();
            concat->addChar(':');
            concat->addBlank();
            SWAG_CHECK(outputNode(node->children[2]));
            break;

        case AstNodeKind::TypeAlias:
            CONCAT_FIXED_STR(concat, "typealias");
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addBlank();
            concat->addChar('=');
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::NameAlias:
            CONCAT_FIXED_STR(concat, "namealias");
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addBlank();
            concat->addChar('=');
            concat->addBlank();
            if (node->lastChild()->resolvedSymbolName())
                concat->addString(node->lastChild()->resolvedSymbolName()->getFullName());
            else
                SWAG_CHECK(outputNode(node->lastChild()));
            break;

        case AstNodeKind::ConstDecl:
        case AstNodeKind::VarDecl:
        case AstNodeKind::FuncDeclParam:
            SWAG_CHECK(outputVar(castAst<AstVarDecl>(node)));
            break;

        case AstNodeKind::Using:
            CONCAT_FIXED_STR(concat, "using");
            concat->addBlank();
            SWAG_CHECK(outputCommaChildren(node));
            break;

        case AstNodeKind::Return:
            CONCAT_FIXED_STR(concat, "return");
            if (!node->children.empty())
            {
                concat->addBlank();
                SWAG_CHECK(outputNode(node->firstChild()));
            }
            break;

        case AstNodeKind::IntrinsicDefined:
        case AstNodeKind::IntrinsicLocation:
            concat->addString(node->token.text);
            concat->addChar('(');
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addChar(')');
            break;

        case AstNodeKind::IntrinsicProp:
            concat->addString(node->token.text);
            concat->addChar('(');
            SWAG_CHECK(outputCommaChildren(node));
            concat->addChar(')');
            break;

        case AstNodeKind::IdentifierRef:
            SWAG_CHECK(outputIdentifierRef(node));
            break;

        case AstNodeKind::Identifier:
            SWAG_CHECK(outputIdentifier(node));
            break;

        case AstNodeKind::Switch:
            SWAG_CHECK(outputSwitch(node));
            break;

        case AstNodeKind::Statement:
            SWAG_CHECK(outputStatement(node));
            break;

        case AstNodeKind::SingleOp:
            concat->addString(node->token.text);
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::NullConditionalExpression:
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addBlank();
            CONCAT_FIXED_STR(concat, "orelse");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->secondChild()));
            break;

        case AstNodeKind::AffectOp:
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addBlank();
            concat->addString(node->token.text);
            if (node->hasSpecFlag(AstOp::SPEC_FLAG_OVERFLOW))
                CONCAT_FIXED_STR(concat, ",over");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->secondChild()));
            break;

        case AstNodeKind::FactorOp:
            if (node->hasAstFlag(AST_IN_ATOMIC_EXPR))
                concat->addChar('(');
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addBlank();
            concat->addString(node->token.text);
            if (node->hasSpecFlag(AstOp::SPEC_FLAG_OVERFLOW))
                CONCAT_FIXED_STR(concat, ",over");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->secondChild()));
            if (node->hasAstFlag(AST_IN_ATOMIC_EXPR))
                concat->addChar(')');
            break;

        case AstNodeKind::BinaryOp:
            if (node->hasAstFlag(AST_IN_ATOMIC_EXPR))
                concat->addChar('(');
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addBlank();
            SWAG_CHECK(outputNode(node->secondChild()));
            if (node->hasAstFlag(AST_IN_ATOMIC_EXPR))
                concat->addChar(')');
            break;

        case AstNodeKind::AutoCast:
            CONCAT_FIXED_STR(concat, "acast");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::Cast:
            CONCAT_FIXED_STR(concat, "cast");
            if (node->hasSpecFlag(AstCast::SPEC_FLAG_OVERFLOW))
                CONCAT_FIXED_STR(concat, ",over");
            if (node->hasSpecFlag(AstCast::SPEC_FLAG_BIT))
                CONCAT_FIXED_STR(concat, ",bit");
            concat->addChar('(');
            SWAG_CHECK(outputNode(node->firstChild()));
            CONCAT_FIXED_STR(concat, ") ");
            SWAG_CHECK(outputNode(node->secondChild()));
            break;

        case AstNodeKind::TypeExpression:
            if (node->hasSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE))
            {
                CONCAT_FIXED_STR(concat, "#type");
                concat->addBlank();
            }

            SWAG_CHECK(outputType(castAst<AstTypeExpression>(node)));
            break;

        case AstNodeKind::Literal:
            SWAG_CHECK(outputLiteral(node));
            break;

        case AstNodeKind::ScopeBreakable:
            CONCAT_FIXED_STR(concat, "#scope");
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addEol();
            concat->addIndent(indent);
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::Range:
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addBlank();
            if (node->hasSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP))
                CONCAT_FIXED_STR(concat, "until");
            else
                CONCAT_FIXED_STR(concat, "to");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->secondChild()));
            break;

        case AstNodeKind::TypeLambda:
        case AstNodeKind::TypeClosure:
            SWAG_CHECK(outputTypeLambda(node));
            break;

        case AstNodeKind::CompilerDependencies:
            CONCAT_FIXED_STR(concat, "#dependencies");
            concat->addEol();
            concat->addChar('{');
            concat->addEol();
            indent++;
            SWAG_CHECK(outputChildren(node));
            indent--;
            concat->addIndent(indent);
            concat->addChar('}');
            concat->addEol();
            concat->addIndent(indent);
            break;

        case AstNodeKind::Impl:
            SWAG_CHECK(outputImpl(node));
            break;

        case AstNodeKind::Namespace:
            SWAG_CHECK(outputNamespace(node));
            break;

        default:
            return Report::internalError(const_cast<AstNode*>(node), "FormatAst::outputNode, unknown node kind");
    }

    beautifyCommentAfterSameLine(node);
    return true;
}
