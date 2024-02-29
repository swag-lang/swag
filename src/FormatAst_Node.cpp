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

        case AstNodeKind::CompilerCode:
            concat->addChar(')');
            concat->addEolIndent(indent);
            concat->addChar('{');
            concat->addEolIndent(indent);
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addEolIndent(indent);
            concat->addChar('}');
            concat->addEolIndent(indent);
            break;

        case AstNodeKind::With:
            CONCAT_FIXED_STR(concat, "with");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addEolIndent(indent);
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

        case AstNodeKind::FuncDeclType:
            SWAG_CHECK(outputFuncDeclReturnType(node));
            break;
        case AstNodeKind::FuncDeclParams:
            SWAG_CHECK(outputFuncDeclParams(node));
            break;
        case AstNodeKind::FuncDecl:
            SWAG_CHECK(outputFuncDecl(castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl)));
            break;

        case AstNodeKind::AttrDecl:
            CONCAT_FIXED_STR(concat, "attr");
            concat->addBlank();
            concat->addString(node->token.text);
            SWAG_CHECK(outputFuncDeclParams(castAst<AstAttrDecl>(node, AstNodeKind::AttrDecl)->parameters));
            concat->addEol();
            break;

        case AstNodeKind::EnumType:
            if (!node->children.empty())
                SWAG_CHECK(outputNode(node->firstChild()));
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
            CONCAT_FIXED_STR(concat, "defer ");
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addEol();
            break;

        case AstNodeKind::AttrUse:
        {
            bool hasSomething = true;
            outputAttrUse(node, hasSomething);
            if (!hasSomething)
                break;
            concat->addEolIndent(indent);
            const auto nodeAttr = castAst<AstAttrUse>(node, AstNodeKind::AttrUse);
            SWAG_CHECK(outputNode(nodeAttr->content));
            break;
        }

        case AstNodeKind::ExplicitNoInit:
            CONCAT_FIXED_STR(concat, "undefined");
            break;

        case AstNodeKind::Index:
            CONCAT_FIXED_STR(concat, "#index");
            break;
        case AstNodeKind::GetErr:
            CONCAT_FIXED_STR(concat, "@err");
            break;
        case AstNodeKind::Init:
            CONCAT_FIXED_STR(concat, "@init(");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() == 2)
            {
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(outputNode(node->lastChild()));
            }
            concat->addChar(')');
            break;
        case AstNodeKind::Drop:
            CONCAT_FIXED_STR(concat, "@drop(");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() == 2)
            {
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(outputNode(node->lastChild()));
            }
            concat->addChar(')');
            break;
        case AstNodeKind::PostMove:
            CONCAT_FIXED_STR(concat, "@postmove(");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() == 2)
            {
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(outputNode(node->lastChild()));
            }
            concat->addChar(')');
            break;
        case AstNodeKind::PostCopy:
            CONCAT_FIXED_STR(concat, "@postcopy(");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() == 2)
            {
                CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(outputNode(node->lastChild()));
            }
            concat->addChar(')');
            break;

        case AstNodeKind::Break:
        {
            const auto nodeBreak = castAst<AstBreakContinue>(node, AstNodeKind::Break);
            CONCAT_FIXED_STR(concat, "break ");
            concat->addString(nodeBreak->label.text);
            break;
        }

        case AstNodeKind::Continue:
        {
            const auto nodeContinue = castAst<AstBreakContinue>(node, AstNodeKind::Continue);
            CONCAT_FIXED_STR(concat, "continue ");
            concat->addString(nodeContinue->label.text);
            break;
        }

        case AstNodeKind::MakePointer:
        {
            concat->addChar('&');
            SWAG_CHECK(outputNode(node->firstChild()));
            break;
        }

        case AstNodeKind::MakePointerLambda:
        {
            const auto lambdaNode = castAst<AstMakePointer>(node, AstNodeKind::MakePointerLambda);
            SWAG_CHECK(outputLambdaExpression(lambdaNode->lambda));
            break;
        }

        case AstNodeKind::NoDrop:
            CONCAT_FIXED_STR(concat, "nodrop ");
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::Move:
            CONCAT_FIXED_STR(concat, "move ");
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
                CONCAT_FIXED_STR(concat, "dref ");
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

            int idx = 0;
            for (const auto child : exprNode->children)
            {
                if (child->hasAstFlag(AST_GENERATED))
                    continue;
                if (idx++)
                    CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(outputNode(child));
            }

            if (exprNode->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
                concat->addChar('}');
            else
                concat->addChar(']');
            break;
        }

        case AstNodeKind::CompilerLoad:
            CONCAT_FIXED_STR(concat, "#load ");
            SWAG_CHECK(outputNode(node->firstChild()));
            break;
        case AstNodeKind::CompilerInclude:
            CONCAT_FIXED_STR(concat, "#include ");
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::CompilerRun:
        case AstNodeKind::CompilerRunExpression:
        case AstNodeKind::CompilerAst:
        case AstNodeKind::CompilerValidIf:
        case AstNodeKind::CompilerValidIfx:
        {
            if (node->is(AstNodeKind::CompilerRun) || node->is(AstNodeKind::CompilerRunExpression))
                CONCAT_FIXED_STR(concat, "#run ");
            else if (node->is(AstNodeKind::CompilerAst))
                CONCAT_FIXED_STR(concat, "#ast ");
            else if (node->is(AstNodeKind::CompilerValidIf))
                CONCAT_FIXED_STR(concat, "#validif ");
            else if (node->is(AstNodeKind::CompilerValidIfx))
                CONCAT_FIXED_STR(concat, "#validifx ");

            const auto front = node->firstChild();
            if (front->is(AstNodeKind::FuncDecl))
            {
                const AstFuncDecl* funcDecl = castAst<AstFuncDecl>(front, AstNodeKind::FuncDecl);
                incIndentStatement(funcDecl->content, indent);
                concat->addEolIndent(indent);
                SWAG_CHECK(outputNode(funcDecl->content));
                decIndentStatement(funcDecl->content, indent);
                concat->addEol();
            }
            else
            {
                SWAG_CHECK(outputNode(front));
                concat->addEol();
            }
            break;
        }

        case AstNodeKind::CompilerIfBlock:
            if (node->firstChild()->isNot(AstNodeKind::Statement))
                CONCAT_FIXED_STR(concat, "#do ");
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::CompilerMacro:
            CONCAT_FIXED_STR(concat, "#macro");
            incIndentStatement(node->firstChild(), indent);
            concat->addEolIndent(indent);
            SWAG_CHECK(outputNode(node->firstChild()));
            decIndentStatement(node->firstChild(), indent);
            break;

        case AstNodeKind::CompilerMixin:
        {
            const auto compilerMixin = castAst<AstCompilerMixin>(node, AstNodeKind::CompilerMixin);
            CONCAT_FIXED_STR(concat, "#mixin ");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (!compilerMixin->replaceTokens.empty())
            {
                CONCAT_FIXED_STR(concat, " { ");
                for (const auto m : compilerMixin->replaceTokens)
                {
                    switch (m.first)
                    {
                        case TokenId::KwdBreak:
                            CONCAT_FIXED_STR(concat, "break");
                            break;
                        case TokenId::KwdContinue:
                            CONCAT_FIXED_STR(concat, "continue");
                            break;
                        default:
                            SWAG_ASSERT(false);
                            break;
                    }

                    CONCAT_FIXED_STR(concat, " = ");
                    SWAG_CHECK(outputNode(m.second));
                    CONCAT_FIXED_STR(concat, "; ");
                }
                concat->addChar('}');
            }
            break;
        }

        case AstNodeKind::CompilerPrint:
            CONCAT_FIXED_STR(concat, "#print ");
            SWAG_CHECK(outputNode(node->firstChild()));
            break;
        case AstNodeKind::CompilerError:
            CONCAT_FIXED_STR(concat, "#error ");
            SWAG_CHECK(outputNode(node->firstChild()));
            break;
        case AstNodeKind::CompilerWarning:
            CONCAT_FIXED_STR(concat, "#warning ");
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::CompilerAssert:
            CONCAT_FIXED_STR(concat, "#assert ");
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->childCount() > 1)
            {
                concat->addChar(',');
                SWAG_CHECK(outputNode(node->secondChild()));
            }

            break;

        case AstNodeKind::CompilerIf:
        {
            const auto compilerIf = castAst<AstIf>(node, AstNodeKind::CompilerIf);
            CONCAT_FIXED_STR(concat, "#if ");
            SWAG_CHECK(outputNode(compilerIf->boolExpression));

            incIndentStatement(compilerIf->ifBlock, indent);
            concat->addEolIndent(indent);

            SWAG_CHECK(outputNode(compilerIf->ifBlock));
            decIndentStatement(compilerIf->ifBlock, indent);

            if (compilerIf->elseBlock)
            {
                concat->addEolIndent(indent);
                CONCAT_FIXED_STR(concat, "#else ");

                if (compilerIf->elseBlock->firstChild()->isNot(AstNodeKind::CompilerIf))
                {
                    incIndentStatement(compilerIf->elseBlock, indent);
                    concat->addEolIndent(indent);
                }

                SWAG_CHECK(outputNode(compilerIf->elseBlock));
                if (compilerIf->elseBlock->firstChild()->isNot(AstNodeKind::CompilerIf))
                {
                    decIndentStatement(compilerIf->elseBlock, indent);
                }
            }
            break;
        }

        case AstNodeKind::If:
        {
            const auto compilerIf = castAst<AstIf>(node, AstNodeKind::If, AstNodeKind::CompilerIf);
            CONCAT_FIXED_STR(concat, "if ");

            if (compilerIf->hasSpecFlag(AstIf::SPEC_FLAG_ASSIGN))
            {
                const auto varNode = castAst<AstVarDecl>(compilerIf->firstChild(), AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
                if (varNode->token.is(TokenId::KwdConst))
                    CONCAT_FIXED_STR(concat, "const ");
                else
                    CONCAT_FIXED_STR(concat, "var ");
                SWAG_CHECK(outputVarDecl(varNode, false));
            }
            else
                SWAG_CHECK(outputNode(compilerIf->boolExpression));

            incIndentStatement(compilerIf->ifBlock, indent);
            concat->addEolIndent(indent);
            SWAG_CHECK(outputNode(compilerIf->ifBlock));
            decIndentStatement(compilerIf->ifBlock, indent);

            if (compilerIf->elseBlock)
            {
                concat->addEolIndent(indent);
                CONCAT_FIXED_STR(concat, "else");
                incIndentStatement(compilerIf->elseBlock, indent);
                concat->addEolIndent(indent);
                SWAG_CHECK(outputNode(compilerIf->elseBlock));
                decIndentStatement(compilerIf->elseBlock, indent);
            }
            break;
        }

        case AstNodeKind::For:
        {
            const auto forNode = castAst<AstFor>(node, AstNodeKind::For);
            CONCAT_FIXED_STR(concat, "for ");
            SWAG_CHECK(outputNode(forNode->preExpression));
            CONCAT_FIXED_STR(concat, "; ");
            SWAG_CHECK(outputNode(forNode->boolExpression));
            CONCAT_FIXED_STR(concat, "; ");
            SWAG_CHECK(outputNode(forNode->postExpression));
            incIndentStatement(forNode->block, indent);
            concat->addEolIndent(indent);
            SWAG_CHECK(outputNode(forNode->block));
            decIndentStatement(forNode->block, indent);
            break;
        }

        case AstNodeKind::Visit:
        {
            const auto visitNode = castAst<AstVisit>(node, AstNodeKind::Visit);
            CONCAT_FIXED_STR(concat, "visit ");

            if (visitNode->hasSpecFlag(AstVisit::SPEC_FLAG_WANT_POINTER))
                concat->addChar('&');

            bool first = true;
            for (auto& a : visitNode->aliasNames)
            {
                if (!first)
                    CONCAT_FIXED_STR(concat, ", ");
                first = false;
                concat->addString(a.text);
            }

            if (!visitNode->aliasNames.empty())
                CONCAT_FIXED_STR(concat, ": ");
            SWAG_CHECK(outputNode(visitNode->expression));
            incIndentStatement(visitNode->block, indent);
            concat->addEolIndent(indent);
            SWAG_CHECK(outputNode(visitNode->block));
            decIndentStatement(visitNode->block, indent);
            break;
        }

        case AstNodeKind::Loop:
        {
            const auto loopNode = castAst<AstLoop>(node, AstNodeKind::Loop);
            CONCAT_FIXED_STR(concat, "loop");
            if (loopNode->hasSpecFlag(AstLoop::SPEC_FLAG_BACK))
                CONCAT_FIXED_STR(concat, ",back");
            concat->addBlank();

            if (loopNode->specificName)
            {
                concat->addString(loopNode->specificName->token.text);
                CONCAT_FIXED_STR(concat, ": ");
            }

            SWAG_CHECK(outputNode(loopNode->expression));
            incIndentStatement(loopNode->block, indent);
            concat->addEolIndent(indent);
            SWAG_CHECK(outputNode(loopNode->block));
            decIndentStatement(loopNode->block, indent);
            break;
        }

        case AstNodeKind::While:
        {
            const auto whileNode = castAst<AstWhile>(node, AstNodeKind::While);
            CONCAT_FIXED_STR(concat, "while ");
            SWAG_CHECK(outputNode(whileNode->boolExpression));
            incIndentStatement(whileNode->block, indent);
            concat->addEolIndent(indent);
            SWAG_CHECK(outputNode(whileNode->block));
            decIndentStatement(whileNode->block, indent);
            break;
        }

        case AstNodeKind::CompilerSpecialValue:
        {
            switch (node->token.id)
            {
                case TokenId::CompilerSelf:
                    CONCAT_FIXED_STR(concat, "#self");
                    break;
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
                case TokenId::CompilerSwagOs:
                    CONCAT_FIXED_STR(concat, "#swagos");
                    break;
                case TokenId::CompilerArch:
                    CONCAT_FIXED_STR(concat, "#arch");
                    break;
                case TokenId::CompilerCpu:
                    CONCAT_FIXED_STR(concat, "#cpu");
                    break;
                case TokenId::CompilerBackend:
                    CONCAT_FIXED_STR(concat, "#backend");
                    break;
                case TokenId::CompilerBuildCfg:
                    CONCAT_FIXED_STR(concat, "#cfg");
                    break;
                default:
                    Report::internalError(const_cast<AstNode*>(node), "Ast::outputNode, unknown compiler function");
            }
            break;
        }

        case AstNodeKind::ConditionalExpression:
            SWAG_CHECK(outputNode(node->firstChild()));
            CONCAT_FIXED_STR(concat, " ? (");
            SWAG_CHECK(outputNode(node->secondChild()));
            CONCAT_FIXED_STR(concat, ") : (");
            SWAG_CHECK(outputNode(node->children[2]));
            concat->addChar(')');
            break;

        case AstNodeKind::TypeAlias:
        {
            CONCAT_FIXED_STR(concat, "typealias ");
            concat->addString(node->token.text);
            CONCAT_FIXED_STR(concat, " = ");
            SWAG_CHECK(outputNode(node->firstChild()));
            break;
        }
        case AstNodeKind::NameAlias:
        {
            CONCAT_FIXED_STR(concat, "namealias ");
            concat->addString(node->token.text);
            CONCAT_FIXED_STR(concat, " = ");
            const auto back = node->lastChild();
            if (back->resolvedSymbolName())
                concat->addString(back->resolvedSymbolName()->getFullName());
            else
                SWAG_CHECK(outputNode(back));
            break;
        }

        case AstNodeKind::ConstDecl:
        case AstNodeKind::VarDecl:
        case AstNodeKind::FuncDeclParam:
        {
            const AstVarDecl* varDecl = castAst<AstVarDecl>(node);
            SWAG_CHECK(outputVar(varDecl));
            break;
        }

        case AstNodeKind::Using:
        {
            CONCAT_FIXED_STR(concat, "using ");
            int idx = 0;
            for (const auto child : node->children)
            {
                if (idx)
                    CONCAT_FIXED_STR(concat, ", ");
                SWAG_CHECK(outputNode(child));
                idx++;
            }

            break;
        }

        case AstNodeKind::Return:
        {
            CONCAT_FIXED_STR(concat, "return ");
            if (!node->children.empty())
                SWAG_CHECK(outputNode(node->firstChild()));
            break;
        }

        case AstNodeKind::IntrinsicDefined:
        case AstNodeKind::IntrinsicLocation:
            concat->addString(node->token.text);
            concat->addChar('(');
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addChar(')');
            break;

        case AstNodeKind::IntrinsicProp:
        {
            const auto propertyNode = castAst<AstIntrinsicProp>(node, AstNodeKind::IntrinsicProp);
            concat->addString(propertyNode->token.text);
            concat->addChar('(');
            int idx = 0;
            for (const auto child : node->children)
            {
                if (idx)
                    CONCAT_FIXED_STR(concat, ",");
                SWAG_CHECK(outputNode(child));
                idx++;
            }

            concat->addChar(')');
            break;
        }

        case AstNodeKind::IdentifierRef:
        {
            if (node->hasAstFlag(AST_DISCARD))
                CONCAT_FIXED_STR(concat, "discard ");
            if (node->hasSpecFlag(AstIdentifierRef::SPEC_FLAG_AUTO_SCOPE))
                CONCAT_FIXED_STR(concat, ".");
            int idx = 0;
            for (const auto child : node->children)
            {
                if (idx)
                    CONCAT_FIXED_STR(concat, ".");
                SWAG_CHECK(outputNode(child));
                idx++;
            }

            break;
        }

        case AstNodeKind::Identifier:
        {
            const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
            if (identifier->identifierExtension && identifier->identifierExtension->scopeUpMode != IdentifierScopeUpMode::None)
            {
                CONCAT_FIXED_STR(concat, "#up");
                if (identifier->identifierExtension->scopeUpMode == IdentifierScopeUpMode::Count &&
                    identifier->identifierExtension->scopeUpValue.literalValue.u8 > 1)
                {
                    concat->addChar('(');
                    concat->addStringFormat("%d", identifier->identifierExtension->scopeUpValue.literalValue.u8);
                    concat->addChar(')');
                }

                concat->addBlank();
            }

            concat->addString(node->token.text);

            if (identifier->genericParameters)
            {
                concat->addChar('\'');
                concat->addChar('(');
                SWAG_CHECK(outputNode(identifier->genericParameters));
                concat->addChar(')');
            }

            if (identifier->callParameters)
            {
                if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
                    concat->addChar('{');
                else
                    concat->addChar('(');
                SWAG_CHECK(outputNode(identifier->callParameters));
                if (identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
                    concat->addChar('}');
                else if (identifier->callParameters->children.empty())
                    concat->addChar(')');
                else if (identifier->callParameters->lastChild()->lastChild()->isNot(AstNodeKind::CompilerCode))
                    concat->addChar(')');
            }

            break;
        }

        case AstNodeKind::Switch:
        {
            const auto nodeSwitch = castAst<AstSwitch>(node, AstNodeKind::Switch);
            CONCAT_FIXED_STR(concat, "switch ");
            SWAG_CHECK(outputNode(nodeSwitch->expression));
            concat->addEolIndent(indent);
            concat->addChar('{');
            concat->addEolIndent(indent);

            for (const auto c : nodeSwitch->cases)
            {
                if (c->expressions.empty())
                    CONCAT_FIXED_STR(concat, "default");
                else
                {
                    CONCAT_FIXED_STR(concat, "case ");
                    bool first = true;
                    for (const auto e : c->expressions)
                    {
                        if (!first)
                            CONCAT_FIXED_STR(concat, ", ");
                        SWAG_CHECK(outputNode(e));
                        first = false;
                    }
                }
                concat->addChar(':');
                concat->addEolIndent(indent);
                SWAG_CHECK(outputNode(c->block));
                concat->addEolIndent(indent);
            }

            concat->addChar('}');
            concat->addEolIndent(indent);
            break;
        }

        case AstNodeKind::StatementNoScope:
        {
            for (const auto child : node->children)
            {
                if (child->hasAstFlag(AST_GENERATED))
                    continue;
                indent++;
                SWAG_CHECK(outputNode(child));
                indent--;
                concat->addEolIndent(indent);
            }

            break;
        }

        case AstNodeKind::Statement:
        case AstNodeKind::SwitchCaseBlock:
            concat->addChar('{');
            concat->addEol();
            for (const auto child : node->children)
            {
                concat->addIndent(indent + 1);
                indent++;
                SWAG_CHECK(outputNode(child));
                indent--;
                concat->addEol();
            }

            concat->addIndent(indent);
            concat->addChar('}');
            concat->addEolIndent(indent);
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
        {
            const auto opNode = castAst<AstOp>(node, AstNodeKind::AffectOp);
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addBlank();
            concat->addString(node->token.text);
            if (opNode->hasSpecFlag(AstOp::SPEC_FLAG_OVERFLOW))
                CONCAT_FIXED_STR(concat, ",over");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->secondChild()));
            break;
        }

        case AstNodeKind::FactorOp:
        {
            const auto opNode = castAst<AstOp>(node, AstNodeKind::FactorOp);
            concat->addChar('(');
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addBlank();
            concat->addString(node->token.text);
            if (opNode->hasSpecFlag(AstOp::SPEC_FLAG_OVERFLOW))
                CONCAT_FIXED_STR(concat, ",over");
            concat->addBlank();
            SWAG_CHECK(outputNode(node->secondChild()));
            concat->addChar(')');
            break;
        }

        case AstNodeKind::BinaryOp:
            concat->addChar('(');
            SWAG_CHECK(outputNode(node->firstChild()));
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addBlank();
            SWAG_CHECK(outputNode(node->secondChild()));
            concat->addChar(')');
            break;

        case AstNodeKind::AutoCast:
            CONCAT_FIXED_STR(concat, "acast ");
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
        {
            const auto typeNode = castAst<AstTypeExpression>(node);
            if (typeNode->hasSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE))
                concat->addString("#type ");
            SWAG_CHECK(outputType(typeNode));
            break;
        }

        case AstNodeKind::Literal:
        {
            const auto literalNode = castAst<AstLiteral>(node, AstNodeKind::Literal);
            if (literalNode->literalType == LiteralType::TypeStringRaw)
                CONCAT_FIXED_STR(concat, "#\"");
            else if (literalNode->literalType == LiteralType::TypeString || literalNode->literalType == LiteralType::TypeStringEscape)
                CONCAT_FIXED_STR(concat, "\"");
            else if (literalNode->literalType == LiteralType::TypeStringMultiLine || literalNode->literalType == LiteralType::TypeStringMultiLineEscape)
                CONCAT_FIXED_STR(concat, "\"\"\"");
            else if (literalNode->literalType == LiteralType::TypeCharacter || literalNode->literalType == LiteralType::TypeCharacterEscape)
                CONCAT_FIXED_STR(concat, "`");

            concat->addString(node->token.text);

            if (literalNode->literalType == LiteralType::TypeStringRaw)
                CONCAT_FIXED_STR(concat, "\"#");
            else if (literalNode->literalType == LiteralType::TypeString || literalNode->literalType == LiteralType::TypeStringEscape)
                CONCAT_FIXED_STR(concat, "\"");
            else if (literalNode->literalType == LiteralType::TypeStringMultiLine || literalNode->literalType == LiteralType::TypeStringMultiLineEscape)
                CONCAT_FIXED_STR(concat, "\"\"\"");
            else if (literalNode->literalType == LiteralType::TypeCharacter || literalNode->literalType == LiteralType::TypeCharacterEscape)
                CONCAT_FIXED_STR(concat, "`");

            if (!node->children.empty())
            {
                CONCAT_FIXED_STR(concat, "'");
                SWAG_CHECK(outputNode(node->firstChild()));
            }
            break;
        }

        case AstNodeKind::ScopeBreakable:
            CONCAT_FIXED_STR(concat, "#scope ");
            concat->addString(node->token.text);
            concat->addEolIndent(indent);
            SWAG_CHECK(outputNode(node->firstChild()));
            break;

        case AstNodeKind::Range:
            SWAG_CHECK(outputNode(node->firstChild()));
            if (node->hasSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP))
                CONCAT_FIXED_STR(concat, " until ");
            else
                CONCAT_FIXED_STR(concat, " to ");
            SWAG_CHECK(outputNode(node->secondChild()));
            break;

        case AstNodeKind::TypeLambda:
        case AstNodeKind::TypeClosure:
        {
            const auto typeNode = castAst<AstTypeLambda>(node);
            if (typeNode->hasSpecFlag(AstType::SPEC_FLAG_FORCE_TYPE))
                concat->addString("#type ");
            if (node->is(AstNodeKind::TypeLambda))
                CONCAT_FIXED_STR(concat, "func");
            else
                CONCAT_FIXED_STR(concat, "closure");
            if (!typeNode->parameters)
                CONCAT_FIXED_STR(concat, "()");
            else
                SWAG_CHECK(outputNode(typeNode->parameters));
            if (typeNode->returnType)
            {
                CONCAT_FIXED_STR(concat, "->");
                SWAG_CHECK(outputNode(typeNode->returnType));
            }

            if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_THROW))
                CONCAT_FIXED_STR(concat, " throw");
            else if (node->hasSpecFlag(AstFuncDecl::SPEC_FLAG_ASSUME))
                CONCAT_FIXED_STR(concat, " assume");
            break;
        }

        case AstNodeKind::File:
            for (const auto c : node->children)
                outputNode(c);
            break;

        case AstNodeKind::CompilerDependencies:
            CONCAT_FIXED_STR(concat, "#dependencies");
            concat->addEol();
            concat->addChar('{');
            concat->addEol();
            for (const auto child : node->children)
            {
                concat->addIndent(indent + 1);
                indent++;
                SWAG_CHECK(outputNode(child));
                indent--;
                concat->addEol();
            }

            concat->addIndent(indent);
            concat->addChar('}');
            concat->addEolIndent(indent);
            break;

        case AstNodeKind::Impl:
        {
            const auto nodeImpl = castAst<AstImpl>(node, AstNodeKind::Impl);
            CONCAT_FIXED_STR(concat, "impl ");
            concat->addString(nodeImpl->identifier->token.text);
            if (nodeImpl->identifierFor)
            {
                CONCAT_FIXED_STR(concat, " for ");
                concat->addString(nodeImpl->identifierFor->token.text);
            }
            concat->addEolIndent(indent);
            concat->addChar('{');
            concat->addEol();

            const uint32_t first = nodeImpl->identifierFor ? 2 : 1;
            for (uint32_t i = first; i < node->childCount(); i++)
            {
                const auto child = node->children[i];
                concat->addIndent(indent + 1);
                indent++;
                SWAG_CHECK(outputNode(child));
                indent--;
                concat->addEol();
            }

            concat->addIndent(indent);
            concat->addChar('}');
            concat->addEolIndent(indent);
            break;
        }

        case AstNodeKind::Namespace:
            if (node->hasSpecFlag(AstNameSpace::SPEC_FLAG_GENERATED_TOP_LEVEL))
            {
                SWAG_CHECK(outputChildren(node));
                break;
            }

            CONCAT_FIXED_STR(concat, "namespace");
            concat->addBlank();
            concat->addString(node->token.text);
            concat->addEol();
            concat->addIndent(indent);
            concat->addChar('{');
            concat->addEol();
            indent++;
            outputChildren(node);
            indent--;
            concat->addIndent(indent);
            concat->addChar('}');
            concat->addEol();
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

        default:
            return Report::internalError(const_cast<AstNode*>(node), "FormatAst::outputNode, unknown node kind");
    }

    return true;
}
