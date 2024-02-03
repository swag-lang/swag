#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Report.h"
#include "Semantic.h"
#include "SourceFile.h"

bool Semantic::canHaveGlobalAccess(AstNode* node)
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
    case AstNodeKind::TypeAlias:
    case AstNodeKind::NameAlias:
        return true;
    default:
        return false;
    }
}

bool Semantic::canHaveAccess(AstNode* node)
{
    if (!canHaveGlobalAccess(node))
        return false;
    if (!node->ownerScope || !node->ownerScope->isGlobalOrImpl())
        return false;
    if (node->flags & AST_FROM_GENERIC)
        return false;
    if (node->sourceFile->forceExport || node->sourceFile->imported)
        return false;
    if (node->token.text[0] == '@')
        return false;
    return true;
}

bool Semantic::canInheritAccess(AstNode* node)
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
        if (!(node->ownerFct->flags & AST_IS_GENERIC) && !node->ownerFct->mustUserInline(true))
            return false;
    }

    // Content of a struct will propagate if struct is not opaque
    if (node->kind == AstNodeKind::StructContent)
    {
        if (node->ownerStructScope->owner->attributeFlags & ATTRIBUTE_OPAQUE)
            return false;
    }

    return true;
}

void Semantic::setNodeAccess(AstNode* node)
{
    const auto overload = node->resolvedSymbolOverload;
    if (!overload)
        return;
    if (!overload->node)
        return;
    if (overload->node->flags & AST_GENERATED)
        return;
    if (overload->symbol->kind == SymbolKind::Namespace)
        return;

    if (overload->node->attributeFlags & ATTRIBUTE_ACCESS_MASK)
    {
        node->semFlags |= attributeToAccess(overload->node->attributeFlags);
        return;
    }

    node->semFlags |= overload->node->semFlags & SEMFLAG_ACCESS_MASK;
}

void Semantic::doInheritAccess(AstNode* forNode, AstNode* node)
{
    if (node->semFlags & SEMFLAG_ACCESS_PRIVATE)
    {
        forNode->semFlags &= ~(SEMFLAG_ACCESS_PUBLIC | SEMFLAG_ACCESS_INTERNAL);
        forNode->semFlags |= SEMFLAG_ACCESS_PRIVATE;
    }
    else if (node->semFlags & SEMFLAG_ACCESS_INTERNAL && !(forNode->semFlags & SEMFLAG_ACCESS_PRIVATE))
    {
        forNode->semFlags &= ~SEMFLAG_ACCESS_PUBLIC;
        forNode->semFlags |= SEMFLAG_ACCESS_INTERNAL;
    }
    else if (node->semFlags & SEMFLAG_ACCESS_PUBLIC && !(forNode->semFlags & (SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE)))
    {
        forNode->semFlags |= SEMFLAG_ACCESS_PUBLIC;
    }
}

void Semantic::inheritAccess(AstNode* node)
{
    doInheritAccess(node->parent, node);
    if (node->typeInfo)
    {
        const auto retType = node->typeInfo->getFinalType();
        if (retType->declNode && node->parent != retType->declNode)
            doInheritAccess(node->parent, retType->declNode);
    }
}

void Semantic::computeAccess(AstNode* node)
{
    if (!canHaveAccess(node))
        return;
    if (node->semFlags & SEMFLAG_ACCESS_COMPUTED)
        return;
    node->semFlags |= SEMFLAG_ACCESS_COMPUTED;
    computeAccessRec(node);
}

void Semantic::computeAccessRec(AstNode* node)
{
    setNodeAccess(node);
    for (const auto c : node->childs)
    {
        if (!canInheritAccess(c))
            continue;
        computeAccessRec(c);
        inheritAccess(c);
    }
}

uint64_t Semantic::attributeToAccess(uint64_t attribute)
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

void Semantic::setDefaultAccess(AstNode* node)
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
    if (node->flags & AST_GENERATED)
        return;
    if (!node->ownerScope || !node->ownerScope->isGlobalOrImpl())
        return;

    // If identifier comes from a generic, then access is the same as the source original
    // generic function or struct
    if (node->typeInfo && node->typeInfo->flags & TYPEINFO_FROM_GENERIC)
    {
        if (node->kind == AstNodeKind::StructDecl)
        {
            const auto structNode = CastAst<AstStruct>(node, AstNodeKind::StructDecl);
            SWAG_ASSERT(structNode->originalGeneric);
            node->semFlags |= structNode->originalGeneric->semFlags & SEMFLAG_ACCESS_MASK;
        }

        if (node->kind == AstNodeKind::FuncDecl)
        {
            const auto funcNode = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
            SWAG_ASSERT(funcNode->originalGeneric);
            node->semFlags |= funcNode->originalGeneric->semFlags & SEMFLAG_ACCESS_MASK;
        }

        return;
    }

    if (node->sourceFile && node->sourceFile->globalAttr & ATTRIBUTE_ACCESS_MASK)
        node->semFlags |= attributeToAccess(node->sourceFile->globalAttr);
    else if (node->ownerStructScope && !(node->flags & AST_IN_IMPL))
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

static AstNode* getErrorCulprit(AstNode* n, AstNode** onNode)
{
    if (!n)
        return nullptr;
    if (!Semantic::canInheritAccess(n))
        return nullptr;

    if (n->kind == AstNodeKind::Identifier || n->kind == AstNodeKind::FuncCall)
    {
        if (n->semFlags & (SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
        {
            return n;
        }
    }

    if (n->kind == AstNodeKind::Identifier ||
        n->kind == AstNodeKind::FuncDeclType ||
        n->kind == AstNodeKind::VarDecl)
    {
        if (n->typeInfo)
        {
            const auto retType = n->typeInfo->getFinalType();
            if (retType->declNode)
            {
                const auto res = getErrorCulprit(retType->declNode, onNode);
                if (res)
                {
                    if (*onNode == nullptr)
                        *onNode = n;
                    return res;
                }

                if (retType->declNode && retType->declNode->semFlags & (SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
                {
                    if (n->kind == AstNodeKind::ConstDecl || n->kind == AstNodeKind::VarDecl)
                    {
                        const auto varDecl = CastAst<AstVarDecl>(n, AstNodeKind::ConstDecl, AstNodeKind::VarDecl);
                        if (varDecl->type)
                            *onNode = varDecl->type;
                    }
                    if (!*onNode)
                        *onNode = n;
                    return retType->declNode;
                }
            }
        }
    }

    for (const auto c : n->childs)
    {
        const auto res = getErrorCulprit(c, onNode);
        if (res)
            return res;
    }

    return nullptr;
}

bool Semantic::checkAccess(JobContext* context, AstNode* node)
{
    computeAccess(node);
    if (!(node->attributeFlags & ATTRIBUTE_PUBLIC))
        return true;
    if (!(node->semFlags & (SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE)))
        return true;

    AstNode*   onNode  = nullptr;
    const auto culprit = getErrorCulprit(node, &onNode);
    if (!culprit)
        return Report::internalError(node, "bad access, but cannot find the culprit");
    if (!onNode)
        onNode = culprit;

    const auto       accessCulprit = (culprit->semFlags & SEMFLAG_ACCESS_PRIVATE) ? "private" : "internal";
    const Diagnostic diag{node,
                          node->getTokenName(),
                          Fmt(Err(Err0426),
                              Naming::kindName(node->resolvedSymbolOverload).c_str(),
                              node->token.ctext(),
                              Naming::kindName(culprit->resolvedSymbolOverload).c_str(),
                              culprit->token.ctext(),
                              accessCulprit)};

    const Diagnostic* note  = nullptr;
    const Diagnostic* note1 = nullptr;
    if (onNode == culprit)
        note = Diagnostic::note(culprit, culprit->token, Fmt(Nte(Nte0146), Naming::kindName(culprit->resolvedSymbolOverload).c_str(), accessCulprit));
    else
    {
        note  = Diagnostic::note(onNode, onNode->token, Fmt(Nte(Nte0157), accessCulprit, onNode->typeInfo->getDisplayNameC()));
        note1 = Diagnostic::hereIs(culprit);
    }
    return context->report(diag, note, note1);
}
