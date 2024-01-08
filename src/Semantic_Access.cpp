#include "pch.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "Naming.h"
#include "Report.h"
#include "Semantic.h"

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

void Semantic::doInheritAccess(AstNode* forNode, AstNode* node)
{
    // Access for a tuple definition is only defined by the content
    if (forNode->typeInfo && forNode->typeInfo->isTuple())
        forNode->semFlags &= ~SEMFLAG_ACCESS_MASK;

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
    if (!canInheritAccess(node))
        return;
    doInheritAccess(node->parent, node);
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

void Semantic::setIdentifierAccess(AstIdentifier* identifier, SymbolOverload* overload)
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

    if (n->kind == AstNodeKind::FuncDeclType)
    {
        auto retType = n->typeInfo;
        while (retType->isPointer())
            retType = CastTypeInfo<TypeInfoPointer>(retType, TypeInfoKind::Pointer)->pointedType;
        auto res = getErrorCulprit(retType->declNode, onNode);
        if (res)
        {
            *onNode = n;
            return res;
        }

        if (retType->declNode && retType->declNode->semFlags & (SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
        {
            *onNode = n;
            return retType->declNode;
        }
    }

    if (n->kind == AstNodeKind::Identifier || n->kind == AstNodeKind::FuncCall)
    {
        if (n->flags & AST_GENERATED && n->typeInfo->isTuple() && n->typeInfo->declNode)
        {
            auto res = getErrorCulprit(n->typeInfo->declNode, onNode);
            if (res)
                return res;
        }

        if (n->semFlags & (SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
        {
            return n;
        }
    }

    for (auto c : n->childs)
    {
        auto res = getErrorCulprit(c, onNode);
        if (res)
            return res;
    }

    return nullptr;
}

bool Semantic::checkAccess(JobContext* context, AstNode* node)
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
        AstNode* onNode  = nullptr;
        auto     culprit = getErrorCulprit(node, &onNode);
        if (!culprit)
            return Report::internalError(node, "bad access, but cannot find the culprit");

        auto accessCulprit = (culprit->semFlags & SEMFLAG_ACCESS_PRIVATE) ? "private" : "internal";
        auto token         = node->token;
        if (node->kind == AstNodeKind::FuncDecl)
            token = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl)->tokenName;
        Diagnostic diag{node,
                        token,
                        Fmt(Err(Err0520),
                            Naming::kindName(node->resolvedSymbolOverload).c_str(),
                            node->token.ctext(),
                            Naming::kindName(culprit->resolvedSymbolOverload).c_str(),
                            culprit->token.ctext(),
                            accessCulprit)};

        if (onNode && onNode->kind == AstNodeKind::FuncDeclType)
            diag.addRange(onNode, Diagnostic::isType(onNode->typeInfo));

        Diagnostic* note  = Diagnostic::note(culprit, culprit->token, Fmt(Nte(Nte1127), Naming::kindName(culprit->resolvedSymbolOverload).c_str(), accessCulprit));
        Diagnostic* note1 = nullptr;
        if (culprit->resolvedSymbolOverload->node != culprit)
            note1 = Diagnostic::hereIs(culprit);
        return context->report(diag, note, note1);
    }

    return true;
}
