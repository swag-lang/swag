#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Wmf/SourceFile.h"

bool Semantic::canHaveGlobalAccess(const AstNode* node)
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

bool Semantic::canHaveAccess(const AstNode* node)
{
    if (!canHaveGlobalAccess(node))
        return false;
    if (!node->ownerScope || !node->ownerScope->isGlobalOrImpl())
        return false;
    if (node->hasAstFlag(AST_FROM_GENERIC))
        return false;
    if (node->token.sourceFile->hasFlag(FILE_FORCE_EXPORT) || node->token.sourceFile->imported)
        return false;
    if (node->hasIntrinsicName())
        return false;
    return true;
}

bool Semantic::canInheritAccess(const AstNode* node)
{
    if (!node->parent)
        return false;
    if (node->resolvedSymbolOverload() && node->resolvedSymbolOverload()->node->token.sourceFile->hasFlag(FILE_RUNTIME))
        return false;
    if (node->resolvedSymbolOverload() && node->resolvedSymbolOverload()->node->token.sourceFile->hasFlag(FILE_BOOTSTRAP))
        return false;

    // Content of the function will propagate only if the function is inlined or generic
    if (node->ownerScope && node->ownerScope->is(ScopeKind::FunctionBody))
    {
        if (!node->ownerFct->hasAstFlag(AST_GENERIC) && !node->ownerFct->mustUserInline(true))
            return false;
    }

    // Content of a struct will propagate if struct is not opaque
    if (node->is(AstNodeKind::StructContent))
    {
        if (node->ownerStructScope->owner->hasAttribute(ATTRIBUTE_OPAQUE))
            return false;
    }

    return true;
}

void Semantic::setNodeAccess(AstNode* node)
{
    const auto overload = node->resolvedSymbolOverload();
    if (!overload)
        return;
    if (!overload->node)
        return;
    if (overload->node->hasAstFlag(AST_GENERATED))
        return;
    if (overload->symbol->is(SymbolKind::Namespace))
        return;

    if (overload->node->hasAttribute(ATTRIBUTE_ACCESS_MASK))
    {
        node->addSemFlag(attributeToAccess(overload->node->attributeFlags));
        return;
    }

    node->addSemFlag(overload->node->semFlags.mask(SEMFLAG_ACCESS_MASK));
}

void Semantic::doInheritAccess(AstNode* forNode, const AstNode* node)
{
    if (node->hasSemFlag(SEMFLAG_ACCESS_PRIVATE))
    {
        forNode->removeSemFlag(SEMFLAG_ACCESS_PUBLIC | SEMFLAG_ACCESS_INTERNAL);
        forNode->addSemFlag(SEMFLAG_ACCESS_PRIVATE);
    }
    else if (node->hasSemFlag(SEMFLAG_ACCESS_INTERNAL) && !forNode->hasSemFlag(SEMFLAG_ACCESS_PRIVATE))
    {
        forNode->removeSemFlag(SEMFLAG_ACCESS_PUBLIC);
        forNode->addSemFlag(SEMFLAG_ACCESS_INTERNAL);
    }
    else if (node->hasSemFlag(SEMFLAG_ACCESS_PUBLIC) && !forNode->hasSemFlag(SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
    {
        forNode->addSemFlag(SEMFLAG_ACCESS_PUBLIC);
    }
}

void Semantic::inheritAccess(const AstNode* node)
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
    if (node->hasSemFlag(SEMFLAG_ACCESS_COMPUTED))
        return;
    node->addSemFlag(SEMFLAG_ACCESS_COMPUTED);

    // The first will not lock, and this is normal!
    doComputeAccess(node);
}

void Semantic::doComputeAccess(AstNode* node)
{
    setNodeAccess(node);
    for (const auto c : node->children)
    {
        if (!canInheritAccess(c))
            continue;
        computeAccessRec(c);
        inheritAccess(c);
    }
}

void Semantic::computeAccessRec(AstNode* node)
{
    if (node->is(AstNodeKind::StructDecl))
        node->mutex.lock_shared();
    doComputeAccess(node);
    if (node->is(AstNodeKind::StructDecl))
        node->mutex.unlock_shared();
}

AstSemFlags Semantic::attributeToAccess(const AttributeFlags& attribute)
{
    AstSemFlags result = 0;
    if (attribute.has(ATTRIBUTE_PRIVATE))
        result.add(SEMFLAG_ACCESS_PRIVATE);
    else if (attribute.has(ATTRIBUTE_INTERNAL))
        result.add(SEMFLAG_ACCESS_INTERNAL);
    else if (attribute.has(ATTRIBUTE_PUBLIC))
        result.add(SEMFLAG_ACCESS_PUBLIC);
    return result;
}

void Semantic::setDefaultAccess(AstNode* node)
{
    if (!canHaveGlobalAccess(node))
        return;
    if (node->hasAttribute(ATTRIBUTE_ACCESS_MASK))
        return;
    if (node->token.sourceFile && node->token.sourceFile->hasFlag(FILE_BOOTSTRAP))
        return;
    if (node->token.sourceFile && node->token.sourceFile->hasFlag(FILE_RUNTIME))
        return;
    if (node->hasAstFlag(AST_STRUCT_MEMBER))
        return;
    if (node->hasAstFlag(AST_GENERATED))
        return;
    if (!node->ownerScope || !node->ownerScope->isGlobalOrImpl())
        return;

    // If identifier comes from a generic, then access is the same as the source original
    // generic function or struct
    if (node->typeInfo && node->typeInfo->isFromGeneric())
    {
        if (node->is(AstNodeKind::StructDecl))
        {
            const auto structNode = castAst<AstStruct>(node, AstNodeKind::StructDecl);
            SWAG_ASSERT(structNode->originalGeneric);
            node->addSemFlag(structNode->originalGeneric->semFlags.mask(SEMFLAG_ACCESS_MASK));
        }

        if (node->is(AstNodeKind::FuncDecl))
        {
            const auto funcNode = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
            SWAG_ASSERT(funcNode->originalGeneric);
            node->addSemFlag(funcNode->originalGeneric->semFlags.mask(SEMFLAG_ACCESS_MASK));
        }

        return;
    }

    if (node->token.sourceFile && node->token.sourceFile->globalAttr.has(ATTRIBUTE_ACCESS_MASK))
        node->addSemFlag(attributeToAccess(node->token.sourceFile->globalAttr));
    else if (node->ownerStructScope && !node->hasAstFlag(AST_IN_IMPL))
        node->addSemFlag(attributeToAccess(node->ownerStructScope->owner->attributeFlags));
    else if (node->token.sourceFile && node->token.sourceFile->hasFlag(FILE_FROM_TESTS))
        node->addSemFlag(attributeToAccess(ATTRIBUTE_PRIVATE));
    else if (node->token.sourceFile && node->token.sourceFile->hasFlag(FILE_FORCE_EXPORT))
        node->addSemFlag(attributeToAccess(ATTRIBUTE_PUBLIC));
    else if (node->token.sourceFile && node->token.sourceFile->imported)
        node->addSemFlag(attributeToAccess(ATTRIBUTE_PUBLIC));
    else
        node->addSemFlag(attributeToAccess(ATTRIBUTE_INTERNAL));
}

namespace
{
    AstNode* getErrorCulprit(AstNode* n, AstNode** onNode)
    {
        if (!n)
            return nullptr;
        if (!Semantic::canInheritAccess(n))
            return nullptr;

        if (n->is(AstNodeKind::Identifier))
        {
            if (n->hasSemFlag(SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
                return n;
        }

        if (n->is(AstNodeKind::Identifier) ||
            n->is(AstNodeKind::FuncDeclType) ||
            n->is(AstNodeKind::VarDecl))
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

                    if (retType->declNode && retType->declNode->hasSemFlag(SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
                    {
                        if (n->is(AstNodeKind::ConstDecl) || n->is(AstNodeKind::VarDecl))
                        {
                            const auto varDecl = castAst<AstVarDecl>(n, AstNodeKind::ConstDecl, AstNodeKind::VarDecl);
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

        SharedLock lk(n->mutex);
        for (const auto c : n->children)
        {
            const auto res = getErrorCulprit(c, onNode);
            if (res)
                return res;
        }

        return nullptr;
    }
}

bool Semantic::checkAccess(JobContext* context, AstNode* node)
{
    {
        SharedLock lk(node->mutex);
        computeAccess(node);
    }

    if (!node->hasAttribute(ATTRIBUTE_PUBLIC))
        return true;
    if (!node->hasSemFlag(SEMFLAG_ACCESS_INTERNAL | SEMFLAG_ACCESS_PRIVATE))
        return true;

    AstNode*   onNode  = nullptr;
    const auto culprit = getErrorCulprit(node, &onNode);
    if (!onNode)
        onNode = culprit;
    if (!culprit)
        return Report::internalError(node, "bad access, but cannot find the culprit");

    const auto accessCulprit = culprit->hasSemFlag(SEMFLAG_ACCESS_PRIVATE) ? "private" : "internal";
    const auto msg           = formErr(Err0296,
                                       Naming::kindName(node).cstr(),
                                       node->token.cstr(),
                                       Naming::kindName(culprit).cstr(),
                                       culprit->token.cstr(),
                                       accessCulprit);

    Diagnostic err{node, node->getTokenName(), msg};
    if (onNode == culprit)
        err.addNote(culprit, culprit->token, formNte(Nte0158, Naming::kindName(culprit->resolvedSymbolOverload()).cstr(), accessCulprit));
    else
    {
        err.addNote(onNode, onNode->token, formNte(Nte0172, accessCulprit, onNode->typeInfo->getDisplayNameC()));
        err.addNote(Diagnostic::hereIs(culprit));
    }
    return context->report(err);
}
