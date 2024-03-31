#include "pch.h"
#include "Format/FormatAst.h"
#include "Report/Report.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool FormatAst::outputNode(FormatContext& context, AstNode* node)
{
    node = convertNode(context, node);
    if (!node)
        return true;

    beautifyBefore(context, node);

    // Prepend some stuff
    const auto isNamed = node->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
    if (isNamed)
    {
        concat->addString(isNamed->token.text);
        concat->addChar(':');
        concat->addBlank();
    }

    bool forceEOL = false;
    switch (node->kind)
    {
        case AstNodeKind::EmptyNode:
            break;

        case AstNodeKind::SwitchCaseBlock:
        case AstNodeKind::StatementNoScope:
        case AstNodeKind::File:
            SWAG_CHECK(outputChildrenEol(context, node));
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
            SWAG_CHECK(outputChildrenBlank(context, node));
            break;
        case AstNodeKind::CompilerInclude:
            CONCAT_FIXED_STR(concat, "#include");
            concat->addBlank();
            SWAG_CHECK(outputChildrenBlank(context, node));
            break;
        case AstNodeKind::CompilerGlobal:
            SWAG_CHECK(outputCompilerGlobal(context, node));
            break;

        case AstNodeKind::KeepRef:
            if (node->hasAstFlag(AST_IS_CONST))
            {
                CONCAT_FIXED_STR(concat, "const");
                concat->addBlank();
            }
            CONCAT_FIXED_STR(concat, "ref");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;
        case AstNodeKind::MoveRef:
            CONCAT_FIXED_STR(concat, "moveref");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;

        case AstNodeKind::Throw:
            concat->addString(node->token.text);
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;

        case AstNodeKind::FallThrough:
            CONCAT_FIXED_STR(concat, "fallthrough");
            forceEOL = true;
            break;
        case AstNodeKind::Unreachable:
            CONCAT_FIXED_STR(concat, "unreachable");
            forceEOL = true;
            break;

        case AstNodeKind::CompilerPlaceHolder:
            CONCAT_FIXED_STR(concat, "#placeholder");
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addBlank();
            SWAG_CHECK(outputChildrenBlank(context, node));
            forceEOL = true;
            break;
        case AstNodeKind::CompilerForeignLib:
            CONCAT_FIXED_STR(concat, "#foreignlib");
            concat->addBlank();
            SWAG_CHECK(outputChildrenBlank(context, node));
            forceEOL = true;
            break;

        case AstNodeKind::CompilerImport:
            SWAG_CHECK(outputCompilerExport(context, node));
            break;

        case AstNodeKind::CompilerCode:
            SWAG_CHECK(outputCompilerCode(context, node));
            break;

        case AstNodeKind::With:
            CONCAT_FIXED_STR(concat, "with");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            concat->addEol();
            concat->addIndent(context.indent);
            SWAG_CHECK(outputNode(context, node->secondChild()));
            break;

        case AstNodeKind::Try:
        case AstNodeKind::Assume:
            SWAG_CHECK(outputTryAssume(context, node));
            break;

        case AstNodeKind::Catch:
        case AstNodeKind::TryCatch:
            SWAG_CHECK(outputCatch(context, node));
            break;

        case AstNodeKind::FuncCallParam:
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;
        case AstNodeKind::FuncCallParams:
            SWAG_CHECK(outputFuncCallParams(context, node));
            break;

        case AstNodeKind::FuncDeclParams:
            concat->addChar('(');
            outputChildrenChar(context, node, ',');
            concat->addChar(')');
            break;

        case AstNodeKind::FuncDecl:
            SWAG_CHECK(outputFuncDecl(context, node));
            break;

        case AstNodeKind::AttrDecl:
        {
            const auto attrDecl = castAst<AstAttrDecl>(node, AstNodeKind::AttrDecl);
            CONCAT_FIXED_STR(concat, "attr");
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addChar('(');
            outputChildrenChar(context, attrDecl->parameters, ',');
            concat->addChar(')');
            forceEOL = true;
            break;
        }

        case AstNodeKind::EnumType:
            if (!node->children.empty())
                SWAG_CHECK(outputNode(context, node->firstChild()));
            break;
        case AstNodeKind::EnumValue:
            SWAG_CHECK(outputEnumValue(context, node));
            break;
        case AstNodeKind::EnumDecl:
            SWAG_CHECK(outputEnum(context, castAst<AstEnum>(node, AstNodeKind::EnumDecl)));
            break;

        case AstNodeKind::StructContent:
            SWAG_CHECK(outputStructDeclContent(context, node));
            break;
        case AstNodeKind::TupleContent:
            SWAG_CHECK(outputTupleDeclContent(context, node));
            break;

        case AstNodeKind::StructDecl:
        case AstNodeKind::InterfaceDecl:
            SWAG_CHECK(outputStructDecl(context, castAst<AstStruct>(node, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl)));
            break;

        case AstNodeKind::Defer:
            SWAG_CHECK(outputDefer(context, node));
            break;

        case AstNodeKind::AttrUse:
            SWAG_CHECK(outputAttrUse(context, castAst<AstAttrUse>(node, AstNodeKind::AttrUse)));
            break;

        case AstNodeKind::Init:
            SWAG_CHECK(outputInit(context, node));
            break;
        case AstNodeKind::Drop:
        case AstNodeKind::PostMove:
        case AstNodeKind::PostCopy:
            SWAG_CHECK(outputDropCopyMove(context, node));
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
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;
        case AstNodeKind::MakePointerLambda:
        {
            const auto lambdaDecl = castAst<AstMakePointer>(node, AstNodeKind::MakePointerLambda);
            SWAG_CHECK(outputLambdaExpression(context, lambdaDecl->lambda));
            break;
        }

        case AstNodeKind::ArrayPointerSlicing:
            SWAG_CHECK(outputArrayPointerSlicing(context, node));
            break;

        case AstNodeKind::ArrayPointerIndex:
            SWAG_CHECK(outputArrayPointerIndex(context, node));
            break;

        case AstNodeKind::ExpressionList:
            SWAG_CHECK(outputExpressionList(context, node));
            break;

        case AstNodeKind::CompilerRun:
        case AstNodeKind::CompilerRunExpression:
        case AstNodeKind::CompilerAst:
        case AstNodeKind::CompilerValidIf:
        case AstNodeKind::CompilerValidIfx:
            SWAG_CHECK(outputCompilerExpr(context, node));
            break;

        case AstNodeKind::CompilerIf:
            SWAG_CHECK(outputCompilerIf(context, "#if", node));
            break;

        case AstNodeKind::If:
            SWAG_CHECK(outputIf(context, "if", node));
            break;

        case AstNodeKind::CompilerMacro:
            CONCAT_FIXED_STR(concat, "#macro");
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;

        case AstNodeKind::CompilerMixin:
            SWAG_CHECK(outputCompilerMixin(context, node));
            break;

        case AstNodeKind::CompilerPrint:
            CONCAT_FIXED_STR(concat, "#print");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;

        case AstNodeKind::CompilerError:
            CONCAT_FIXED_STR(concat, "#error");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;

        case AstNodeKind::CompilerWarning:
            CONCAT_FIXED_STR(concat, "#warning");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;

        case AstNodeKind::CompilerAssert:
            CONCAT_FIXED_STR(concat, "#assert");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            if (node->childCount() > 1)
            {
                concat->addChar(',');
                concat->addBlank();
                SWAG_CHECK(outputNode(context, node->secondChild()));
            }
            break;

        case AstNodeKind::For:
            SWAG_CHECK(outputFor(context, node));
            break;

        case AstNodeKind::Visit:
            SWAG_CHECK(outputVisit(context, node));
            break;

        case AstNodeKind::Loop:
            SWAG_CHECK(outputLoop(context, node));
            break;

        case AstNodeKind::While:
            SWAG_CHECK(outputWhile(context, node));
            break;

        case AstNodeKind::CompilerSpecialValue:
            SWAG_CHECK(outputCompilerSpecialValue(context, node));
            break;

        case AstNodeKind::TypeAlias:
            SWAG_CHECK(outputTypeAlias(context, node));
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
                SWAG_CHECK(outputNode(context, node->lastChild()));
            break;

        case AstNodeKind::ConstDecl:
        case AstNodeKind::VarDecl:
        case AstNodeKind::FuncDeclParam:
            SWAG_CHECK(outputVar(context, castAst<AstVarDecl>(node)));
            break;

        case AstNodeKind::Using:
            if (node->hasAstFlag(AST_GLOBAL_NODE))
            {
                CONCAT_FIXED_STR(concat, "#global");
                concat->addBlank();
            }

            CONCAT_FIXED_STR(concat, "using");
            concat->addBlank();
            SWAG_CHECK(outputChildrenChar(context, node, ','));
            break;

        case AstNodeKind::Return:
            CONCAT_FIXED_STR(concat, "return");
            if (!node->children.empty())
            {
                concat->addBlank();
                SWAG_CHECK(outputNode(context, node->firstChild()));
            }
            break;

        case AstNodeKind::IntrinsicDefined:
        case AstNodeKind::IntrinsicLocation:
            concat->addString(node->token.text);
            concat->addChar('(');
            SWAG_CHECK(outputNode(context, node->firstChild()));
            concat->addChar(')');
            break;

        case AstNodeKind::IntrinsicProp:
            concat->addString(node->token.text);
            concat->addChar('(');
            SWAG_CHECK(outputChildrenChar(context, node, ','));
            concat->addChar(')');
            break;

        case AstNodeKind::IdentifierRef:
            SWAG_CHECK(outputIdentifierRef(context, node));
            break;

        case AstNodeKind::Identifier:
            SWAG_CHECK(outputIdentifier(context, node));
            break;

        case AstNodeKind::Switch:
            SWAG_CHECK(outputSwitch(context, node));
            break;

        case AstNodeKind::Statement:
            SWAG_CHECK(outputStatement(context, node));
            break;

        case AstNodeKind::SingleOp:
            concat->addString(node->token.text);
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;

        case AstNodeKind::NullConditionalExpression:
            SWAG_CHECK(outputNullConditionalExpression(context, node));
            return true;

        case AstNodeKind::ConditionalExpression:
            SWAG_CHECK(outputConditionalExpression(context, node));
            return true;

        case AstNodeKind::NoDrop:
            CONCAT_FIXED_STR(concat, ",nodrop");
            if (!node->firstChild()->is(AstNodeKind::Move))
                concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;

        case AstNodeKind::Move:
            if (node->firstChild()->is(AstNodeKind::NoDrop))
            {
                CONCAT_FIXED_STR(concat, ",moveraw");
                concat->addBlank();
                SWAG_CHECK(outputNode(context, node->firstChild()->firstChild()));
            }
            else
            {
                CONCAT_FIXED_STR(concat, ",move");
                concat->addBlank();
                SWAG_CHECK(outputNode(context, node->firstChild()));
            }

            break;

        case AstNodeKind::AffectOp:
            SWAG_CHECK(outputAffectOp(context, node));
            break;

        case AstNodeKind::FactorOp:
            SWAG_CHECK(outputFactorOp(context, node));
            break;

        case AstNodeKind::BinaryOp:
            SWAG_CHECK(outputBinaryOp(context, node));
            break;

        case AstNodeKind::AutoCast:
            CONCAT_FIXED_STR(concat, "acast");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->firstChild()));
            break;

        case AstNodeKind::Cast:
            SWAG_CHECK(outputCast(context, node));
            break;

        case AstNodeKind::TypeExpression:
            SWAG_CHECK(outputTypeExpression(context, node));
            break;

        case AstNodeKind::Literal:
            SWAG_CHECK(outputLiteral(context, node));
            break;

        case AstNodeKind::ScopeBreakable:
        {
            const auto scopeDecl = castAst<AstScopeBreakable>(node, AstNodeKind::ScopeBreakable);
            CONCAT_FIXED_STR(concat, "#scope");
            if (scopeDecl->hasSpecFlag(AstScopeBreakable::SPEC_FLAG_NAMED))
            {
                concat->addBlank();
                concat->addString(node->token.text);
            }

            concat->addEol();
            concat->addIndent(context.indent);
            SWAG_CHECK(outputNode(context, scopeDecl->block));
            break;
        }

        case AstNodeKind::Range:
            SWAG_CHECK(outputNode(context, node->firstChild()));
            concat->addBlank();
            if (node->hasSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP))
                CONCAT_FIXED_STR(concat, "until");
            else
                CONCAT_FIXED_STR(concat, "to");
            concat->addBlank();
            SWAG_CHECK(outputNode(context, node->secondChild()));
            break;

        case AstNodeKind::TypeLambda:
        case AstNodeKind::TypeClosure:
            SWAG_CHECK(outputTypeLambda(context, node));
            break;

        case AstNodeKind::CompilerDependencies:
            CONCAT_FIXED_STR(concat, "#dependencies");
            concat->addEol();
            SWAG_CHECK(outputChildrenEol(context, node));
            forceEOL = true;
            break;

        case AstNodeKind::Impl:
            SWAG_CHECK(outputImpl(context, node));
            break;

        case AstNodeKind::Namespace:
            SWAG_CHECK(outputNamespace(context, node));
            break;

        default:
            return Report::internalError(node, "FormatAst::outputNode, unknown node kind");
    }

    if (context.beautifyAfter)
        beautifyAfter(context, node);
    if (forceEOL)
        concat->addEol();
    return true;
}
