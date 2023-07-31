#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Naming.h"
#include "Report.h"

bool SemanticJob::canHaveGlobalAccess(AstNode* node)
{
    switch (node->kind)
    {
    case AstNodeKind::AttrDecl:
    case AstNodeKind::FuncDecl:
    case AstNodeKind::ConstDecl:
    case AstNodeKind::VarDecl:
    case AstNodeKind::EnumDecl:
    case AstNodeKind::InterfaceDecl:
    case AstNodeKind::StructDecl:
    case AstNodeKind::Alias:
        return true;
    default:
        return false;
    }
}

bool SemanticJob::canInheritAccess(AstNode* node)
{
    if (!node->parent)
        return false;
    if (node->resolvedSymbolOverload && node->resolvedSymbolOverload->node->sourceFile->isRuntimeFile)
        return false;
    if (node->resolvedSymbolOverload && node->resolvedSymbolOverload->node->sourceFile->isBootstrapFile)
        return false;

    // Content of the function will propagate only if the function is inlined or generic
    if (node->ownerScope && node->ownerScope->kind == ScopeKind::FunctionBody)
    {
        if (!(node->ownerFct->flags & AST_IS_GENERIC) &&
            !(node->ownerFct->attributeFlags & (ATTRIBUTE_INLINE | ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN)))
        {
            return false;
        }
    }

    // Content of a struct will propagate if struct is not opaque
    if (node->kind == AstNodeKind::StructContent)
    {
        if (node->ownerStructScope->owner->attributeFlags & ATTRIBUTE_OPAQUE)
            return false;
    }

    return true;
}

void SemanticJob::inheritAccess(AstNode* node)
{
    if (!canInheritAccess(node))
        return;

    // Access for a tuple definition is only defined by the content
    if (node->parent->typeInfo && node->parent->typeInfo->isTuple())
        node->parent->semFlags &= ~SEMFLAG_ACCESS_MASK;

    if (node->semFlags & SEMFLAG_ACCESS_PRIVATE)
    {
        node->parent->semFlags &= ~(SEMFLAG_ACCESS_PUBLIC | SEMFLAG_ACCESS_INTERNAL);
        node->parent->semFlags |= SEMFLAG_ACCESS_PRIVATE;
    }
    else if (node->semFlags & SEMFLAG_ACCESS_INTERNAL && !(node->parent->semFlags & SEMFLAG_ACCESS_PRIVATE))
    {
        node->parent->semFlags &= ~SEMFLAG_ACCESS_PUBLIC;
        node->parent->semFlags |= SEMFLAG_ACCESS_INTERNAL;
    }
    else if (node->semFlags & SEMFLAG_ACCESS_PUBLIC && !(node->parent->semFlags & (SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE)))
    {
        node->parent->semFlags |= SEMFLAG_ACCESS_PUBLIC;
    }
}

uint64_t SemanticJob::attributeToAccess(uint64_t attribute)
{
    uint64_t result = 0;
    if (attribute & ATTRIBUTE_PRIVATE)
        result |= SEMFLAG_ACCESS_PRIVATE;
    else if (attribute & ATTRIBUTE_INTERNAL)
        result |= SEMFLAG_ACCESS_INTERNAL;
    else if (attribute & ATTRIBUTE_PUBLIC)
        result |= SEMFLAG_ACCESS_PUBLIC;
    return result;
}

void SemanticJob::setIdentifierAccess(AstIdentifier* identifier, SymbolOverload* overload)
{
    if (!overload->node)
        return;
    if (overload->node->flags & AST_GENERATED)
        return;
    if (overload->symbol->kind == SymbolKind::Namespace)
        return;

    identifier->semFlags &= ~SEMFLAG_ACCESS_MASK;
    if (overload->node->attributeFlags & ATTRIBUTE_ACCESS_MASK)
        identifier->semFlags |= attributeToAccess(overload->node->attributeFlags);
    else
        identifier->semFlags |= overload->node->semFlags & SEMFLAG_ACCESS_MASK;
}

void SemanticJob::setDefaultAccess(AstNode* node)
{
    if (!canHaveGlobalAccess(node))
        return;
    if (node->attributeFlags & ATTRIBUTE_ACCESS_MASK)
        return;
    if (node->sourceFile && node->sourceFile->isBootstrapFile)
        return;
    if (node->sourceFile && node->sourceFile->isRuntimeFile)
        return;
    if (node->flags & AST_STRUCT_MEMBER)
        return;
    if (!node->ownerScope || !node->ownerScope->isGlobalOrImpl())
        return;

    // If identifier comes from a generic, then access is the same as the source original
    // generic function or struct
    if (node->typeInfo && node->typeInfo->flags & TYPEINFO_FROM_GENERIC)
    {
        if (node->kind == AstNodeKind::StructDecl)
        {
            auto structNode = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
            SWAG_ASSERT(structNode->originalGeneric);
            node->semFlags |= structNode->originalGeneric->semFlags & SEMFLAG_ACCESS_MASK;
        }

        if (node->kind == AstNodeKind::FuncDecl)
        {
            auto funcNode = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
            SWAG_ASSERT(funcNode->originalGeneric);
            node->semFlags |= funcNode->originalGeneric->semFlags & SEMFLAG_ACCESS_MASK;
        }

        return;
    }

    if (node->sourceFile && node->sourceFile->globalAttr & ATTRIBUTE_ACCESS_MASK)
        node->semFlags |= attributeToAccess(node->sourceFile->globalAttr);
    else if (node->ownerStructScope && !(node->flags & AST_INSIDE_IMPL))
        node->semFlags |= attributeToAccess(node->ownerStructScope->owner->attributeFlags);
    else if (node->sourceFile && node->sourceFile->fromTests)
        node->semFlags |= attributeToAccess(ATTRIBUTE_PRIVATE);
    else if (node->sourceFile && node->sourceFile->forceExport)
        node->semFlags |= attributeToAccess(ATTRIBUTE_PUBLIC);
    else if (node->sourceFile && node->sourceFile->imported)
        node->semFlags |= attributeToAccess(ATTRIBUTE_PUBLIC);
    else
        node->semFlags |= attributeToAccess(ATTRIBUTE_INTERNAL);
}

bool SemanticJob::checkAccess(JobContext* context, AstNode* node)
{
    if (!canHaveGlobalAccess(node))
        return true;
    if (!node->ownerScope || !node->ownerScope->isGlobalOrImpl())
        return true;
    if (node->flags & AST_FROM_GENERIC)
        return true;
    if (node->sourceFile->forceExport || node->sourceFile->imported)
        return true;

    bool isPublic = node->attributeFlags & ATTRIBUTE_PUBLIC;
    if (!isPublic)
        return true;

    if (node->semFlags & (SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
    {
        ErrorContext cxt;
        AstNode*     culprit = nullptr;
        Ast::visit(context, node, [&](ErrorContext* cxt, AstNode* n)
                   {
                        if (n->kind != AstNodeKind::Identifier && n->kind != AstNodeKind::FuncCall)
                            return Ast::VisitResult::Continue;
                        if (!canInheritAccess(n))
                            return Ast::VisitResult::NoChilds;
                        if (n->semFlags & (SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
                        {
                            culprit = n;
                            return Ast::VisitResult::Stop;
                        }
                        return Ast::VisitResult::Continue; });

        if (!culprit)
            return Report::internalError(node, "bad access, but cannot find the culprit");

        auto       accessCulprit = (culprit->semFlags & SEMFLAG_ACCESS_PRIVATE) ? "private" : "internal";
        Diagnostic diag{node,
                        node->token,
                        Fmt(Err(Err0520),
                            Naming::kindName(node->resolvedSymbolOverload).c_str(),
                            node->token.ctext(),
                            Naming::kindName(culprit->resolvedSymbolOverload).c_str(),
                            culprit->token.ctext(),
                            accessCulprit)};
        auto       note  = Diagnostic::note(culprit, culprit->token, Fmt(Hnt(Hnt0127), Naming::kindName(culprit->resolvedSymbolOverload).c_str(), accessCulprit));
        auto       note1 = Diagnostic::hereIs(culprit);
        return context->report(diag, note, note1);
    }

    return true;
}
