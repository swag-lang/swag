#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Syntax/Ast.h"
#include "Syntax/Parser/Parser.h"

void AstNode::releaseChildren()
{
    for (const auto c : children)
        c->release();
    children.release();
}

void AstNode::release()
{
#ifdef SWAG_STATS
    ++g_Stats.releaseNodes;
#endif

    if (hasExtByteCode())
    {
        if (extByteCode()->bc)
            extByteCode()->bc->release();
        Allocator::free<NodeExtensionByteCode>(extByteCode());
    }

    if (hasExtSemantic())
    {
        Allocator::free<NodeExtensionSemantic>(extSemantic());
    }

    if (hasExtOwner())
    {
        for (const auto c : extOwner()->nodesToFree)
            c->release();
        Allocator::free<NodeExtensionOwner>(extOwner());
    }

    if (hasExtMisc())
    {
        Allocator::free<NodeExtensionMisc>(extMisc());
    }

    if (extension)
        Allocator::free<NodeExtension>(extension);

    // Pre-release, if we need to children to be alive
    if (is(AstNodeKind::FuncDecl))
    {
        if (const auto funcDecl = castAst<AstFuncDecl>(this, AstNodeKind::FuncDecl); funcDecl->content)
            funcDecl->content->ownerScope->release();
    }

    // Release children first
    for (const auto c : children)
        c->release();

    // Then destruct node
    switch (kind)
    {
        case AstNodeKind::VarDecl:
        case AstNodeKind::ConstDecl:
        case AstNodeKind::FuncDeclParam:
            Allocator::free<AstVarDecl>(this);
            break;
        case AstNodeKind::IdentifierRef:
            Allocator::free<AstIdentifierRef>(this);
            break;
        case AstNodeKind::Identifier:
            Allocator::free<AstIdentifier>(this);
            break;
        case AstNodeKind::FuncDecl:
            Allocator::free<AstFuncDecl>(this);
            break;
        case AstNodeKind::AttrDecl:
            Allocator::free<AstAttrDecl>(this);
            break;
        case AstNodeKind::AttrUse:
            Allocator::free<AstAttrUse>(this);
            break;
        case AstNodeKind::FuncCallParam:
            Allocator::free<AstFuncCallParam>(this);
            break;
        case AstNodeKind::BinaryOp:
            Allocator::free<AstBinaryOpNode>(this);
            break;
        case AstNodeKind::ConditionalExpression:
            Allocator::free<AstConditionalOpNode>(this);
            break;
        case AstNodeKind::If:
        case AstNodeKind::CompilerIf:
            Allocator::free<AstIf>(this);
            break;
        case AstNodeKind::SubstBreakContinue:
            Allocator::free<AstSubstBreakContinue>(this);
            break;
        case AstNodeKind::Break:
        case AstNodeKind::Continue:
        case AstNodeKind::FallThrough:
            Allocator::free<AstBreakContinue>(this);
            break;
        case AstNodeKind::ScopeBreakable:
            Allocator::free<AstScopeBreakable>(this);
            break;
        case AstNodeKind::While:
            Allocator::free<AstWhile>(this);
            break;
        case AstNodeKind::For:
            Allocator::free<AstFor>(this);
            break;
        case AstNodeKind::Loop:
            Allocator::free<AstLoop>(this);
            break;
        case AstNodeKind::Visit:
            Allocator::free<AstVisit>(this);
            break;
        case AstNodeKind::Switch:
            Allocator::free<AstSwitch>(this);
            break;
        case AstNodeKind::SwitchCase:
            Allocator::free<AstSwitchCase>(this);
            break;
        case AstNodeKind::SwitchCaseBlock:
            Allocator::free<AstSwitchCaseBlock>(this);
            break;
        case AstNodeKind::TypeExpression:
            Allocator::free<AstTypeExpression>(this);
            break;
        case AstNodeKind::TypeLambda:
        case AstNodeKind::TypeClosure:
            Allocator::free<AstTypeLambda>(this);
            break;
        case AstNodeKind::ArrayPointerSlicing:
            Allocator::free<AstArrayPointerSlicing>(this);
            break;
        case AstNodeKind::ArrayPointerIndex:
            Allocator::free<AstArrayPointerIndex>(this);
            break;
        case AstNodeKind::StructDecl:
        case AstNodeKind::InterfaceDecl:
            Allocator::free<AstStruct>(this);
            break;
        case AstNodeKind::Impl:
            Allocator::free<AstImpl>(this);
            break;
        case AstNodeKind::EnumDecl:
            Allocator::free<AstEnum>(this);
            break;
        case AstNodeKind::EnumValue:
            Allocator::free<AstEnumValue>(this);
            break;
        case AstNodeKind::Init:
            Allocator::free<AstInit>(this);
            break;
        case AstNodeKind::Drop:
        case AstNodeKind::PostCopy:
        case AstNodeKind::PostMove:
            Allocator::free<AstDropCopyMove>(this);
            break;
        case AstNodeKind::Return:
            Allocator::free<AstReturn>(this);
            break;
        case AstNodeKind::CompilerMacro:
            Allocator::free<AstCompilerMacro>(this);
            break;
        case AstNodeKind::CompilerMixin:
            Allocator::free<AstCompilerMixin>(this);
            break;
        case AstNodeKind::Inline:
            Allocator::free<AstInline>(this);
            break;
        case AstNodeKind::CompilerIfBlock:
            Allocator::free<AstCompilerIfBlock>(this);
            break;
        case AstNodeKind::CompilerRun:
        case AstNodeKind::CompilerRunExpression:
        case AstNodeKind::CompilerWhere:
        case AstNodeKind::CompilerWhereEach:
        case AstNodeKind::CompilerAst:
            Allocator::free<AstCompilerSpecFunc>(this);
            break;
        case AstNodeKind::Namespace:
            Allocator::free<AstNameSpace>(this);
            break;
        case AstNodeKind::Try:
        case AstNodeKind::Catch:
        case AstNodeKind::TryCatch:
        case AstNodeKind::Assume:
        case AstNodeKind::Throw:
            Allocator::free<AstTryCatchAssume>(this);
            break;
        case AstNodeKind::TypeAlias:
        case AstNodeKind::NameAlias:
            Allocator::free<AstAlias>(this);
            break;
        case AstNodeKind::Cast:
        case AstNodeKind::AutoCast:
            Allocator::free<AstCast>(this);
            break;
        case AstNodeKind::FuncCallParams:
            Allocator::free<AstFuncCallParams>(this);
            break;
        case AstNodeKind::Range:
            Allocator::free<AstRange>(this);
            break;
        case AstNodeKind::MakePointer:
        case AstNodeKind::MakePointerLambda:
            Allocator::free<AstMakePointer>(this);
            break;
        case AstNodeKind::AffectOp:
        case AstNodeKind::FactorOp:
            Allocator::free<AstOp>(this);
            break;
        case AstNodeKind::Defer:
            Allocator::free<AstDefer>(this);
            break;
        case AstNodeKind::ExpressionList:
            Allocator::free<AstExpressionList>(this);
            break;
        case AstNodeKind::With:
            Allocator::free<AstWith>(this);
            break;
        case AstNodeKind::Literal:
            Allocator::free<AstLiteral>(this);
            break;
        case AstNodeKind::RefSubDecl:
            Allocator::free<AstRefSubDecl>(this);
            break;
        case AstNodeKind::Statement:
        case AstNodeKind::StatementNoScope:
            Allocator::free<AstStatement>(this);
            break;
        case AstNodeKind::File:
            Allocator::free<AstFile>(this);
            break;
        case AstNodeKind::CompilerImport:
            Allocator::free<AstCompilerImport>(this);
            break;
        case AstNodeKind::Using:
            Allocator::free<AstUsing>(this);
            break;
        case AstNodeKind::CompilerGlobal:
            Allocator::free<AstCompilerGlobal>(this);
            break;
        case AstNodeKind::CompilerCode:
            Allocator::free<AstCompilerCode>(this);
            break;
        default:
            Allocator::free<AstNode>(this);
            break;
    }
}
