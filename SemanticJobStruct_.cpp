#include "pch.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "Ast.h"
#include "Module.h"
#include "Workspace.h"
#include "TypeManager.h"

bool SemanticJob::waitForStructUserOps(SemanticContext* context, AstNode* node)
{
    SWAG_CHECK(resolveUserOp(context, "opPostCopy", nullptr, node, nullptr, true));
    if (context->result == SemanticResult::Pending)
        return true;
    SWAG_CHECK(resolveUserOp(context, "opPostMove", nullptr, node, nullptr, true));
    if (context->result == SemanticResult::Pending)
        return true;
    SWAG_CHECK(resolveUserOp(context, "opDrop", nullptr, node, nullptr, true));
    if (context->result == SemanticResult::Pending)
        return true;
    return true;
}

bool SemanticJob::resolveImpl(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstImpl>(context->node, AstNodeKind::Impl);

    // Be sure this is a struct
    auto typeInfo = node->identifier->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Struct)
    {
        Diagnostic diag{sourceFile, node->identifier, format("'%s' is %s and should be a struct", node->identifier->name.c_str(), TypeInfo::getArticleKindName(typeInfo))};
        Diagnostic note{node->identifier->resolvedSymbolOverload->sourceFile, node->identifier->resolvedSymbolOverload->node->token.startLocation, node->identifier->resolvedSymbolOverload->node->token.endLocation, format("this is the definition of '%s'", node->identifier->name.c_str()), DiagnosticLevel::Note};
        return context->errorContext.report(diag, &note);
    }

    return true;
}

bool SemanticJob::preResolveStruct(SemanticContext* context)
{
    auto node     = CastAst<AstStruct>(context->node->parent, AstNodeKind::StructDecl);
    auto typeInfo = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);

    // Add generic parameters
    uint32_t symbolFlags = 0;
    if (!(node->flags & AST_FROM_GENERIC))
    {
        if (node->genericParameters)
        {
            typeInfo->flags |= TYPEINFO_GENERIC;
            symbolFlags |= OVERLOAD_GENERIC;
            for (auto param : node->genericParameters->childs)
            {
                auto funcParam        = g_Pool_typeInfoParam.alloc();
                funcParam->namedParam = param->name;
                funcParam->name       = param->typeInfo->name;
                funcParam->typeInfo   = param->typeInfo;
                funcParam->sizeOf     = param->typeInfo->sizeOf;
                typeInfo->genericParameters.push_back(funcParam);
                typeInfo->sizeOf += param->typeInfo->sizeOf;
            }
        }
    }

    // Attributes
    if (node->parentAttributes)
        SWAG_CHECK(collectAttributes(context, typeInfo->attributes, node->parentAttributes, context->node, AstNodeKind::StructDecl, node->attributeFlags));

    // Register symbol with its type
    SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Struct, nullptr, symbolFlags | OVERLOAD_INCOMPLETE, nullptr, 0));
    return true;
}

bool SemanticJob::resolveStruct(SemanticContext* context)
{
    auto node       = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);

    typeInfo->structNode = node;
    typeInfo->name       = format("%s", node->name.c_str());
    if (!typeInfo->opInitFct)
        typeInfo->opInitFct = node->defaultOpInit;
    if (node->attributeFlags & ATTRIBUTE_PACK)
        node->packing = 1;

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;
    uint32_t structFlags   = TYPEINFO_STRUCT_ALL_UNINITIALIZED;

    for (auto child : node->content->childs)
    {
        if (child->kind != AstNodeKind::VarDecl)
            continue;

        auto varDecl = static_cast<AstVarDecl*>(child);

        TypeInfoParam* typeParam = nullptr;
        if (!(node->flags & AST_FROM_GENERIC))
        {
            typeParam             = g_Pool_typeInfoParam.alloc();
            typeParam->namedParam = child->name;
            typeParam->name       = child->typeInfo->name;
            typeParam->typeInfo   = child->typeInfo;
            typeParam->sizeOf     = child->typeInfo->sizeOf;
            typeParam->offset     = storageOffset;
            if (child->parentAttributes)
                SWAG_CHECK(collectAttributes(context, typeParam->attributes, child->parentAttributes, child, AstNodeKind::VarDecl, child->attributeFlags));
            typeInfo->childs.push_back(typeParam);
        }

        // Default value
        if (!(varDecl->flags & AST_EXPLICITLY_NOT_INITIALIZED))
            structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;

        // Var is a struct
        if (varDecl->typeInfo->kind == TypeInfoKind::Struct)
        {
            if (varDecl->typeInfo->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
                structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
            if (!(varDecl->typeInfo->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
                structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
        }

        // Var has an initialization
        else if (varDecl->assignment)
        {
            SWAG_VERIFY(varDecl->assignment->flags & AST_CONST_EXPR, context->errorContext.report({sourceFile, varDecl->assignment, "cannot evaluate initialization expression at compile time"}));

            auto typeInfoAssignment = TypeManager::concreteType(varDecl->assignment->typeInfo, MakeConcrete::FlagAlias);
            typeInfoAssignment      = TypeManager::concreteType(varDecl->assignment->typeInfo, MakeConcrete::FlagEnum);

            if (typeInfoAssignment->isNative(NativeTypeKind::String))
            {
                structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                if (typeParam)
                    typeParam->value.reg = varDecl->assignment->computedValue.reg;
            }
            else if (typeInfoAssignment->kind != TypeInfoKind::Native || varDecl->assignment->computedValue.reg.u64)
            {
                structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                if (typeParam)
                    typeParam->value.reg = varDecl->assignment->computedValue.reg;
            }

            structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
        }

        if (!(node->flags & AST_IS_GENERIC))
        {
            SWAG_VERIFY(!(child->typeInfo->flags & TYPEINFO_GENERIC), context->errorContext.report({sourceFile, child, format("cannot instanciate variable because type '%s' is generic", child->typeInfo->name.c_str())}));
        }

        typeInfo->childs[storageIndex]->offset       = storageOffset;
        child->resolvedSymbolOverload->storageOffset = storageOffset;
        child->resolvedSymbolOverload->storageIndex  = storageIndex;

        // Union, no offset, sizeof is the size of the biggest child
        if (node->packing == 0)
        {
            typeInfo->sizeOf = max(typeInfo->sizeOf, child->typeInfo->sizeOf);
        }

        // Direct matching
        else if (node->packing == 1 || child->typeInfo->sizeOf == 0)
        {
            typeInfo->sizeOf += child->typeInfo->sizeOf;
            storageOffset += child->typeInfo->sizeOf;
        }
        else
        {
            auto padding = storageOffset & (child->typeInfo->sizeOf - 1);
            padding %= node->packing;
            if (padding)
            {
                padding = child->typeInfo->sizeOf - padding;
                padding %= node->packing;
                storageOffset += padding;
                typeInfo->childs[storageIndex]->offset       = storageOffset;
                child->resolvedSymbolOverload->storageOffset = storageOffset;
            }

            typeInfo->sizeOf += child->typeInfo->sizeOf + padding;
            storageOffset += child->typeInfo->sizeOf;
        }

        storageIndex++;
    }

    // Check public
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        scoped_lock lk(node->ownerScope->mutexPublic);
        node->ownerScope->publicStruct.push_back(node);
        node->ownerScope->setHasExports();
    }

    if (!(node->flags & AST_FROM_GENERIC))
    {
        typeInfo->flags |= structFlags;
    }
    else
    {
        typeInfo->flags |= (structFlags & TYPEINFO_STRUCT_ALL_UNINITIALIZED);
        typeInfo->flags |= (structFlags & TYPEINFO_STRUCT_HAS_INIT_VALUES);
    }

    // Register symbol with its type
    node->typeInfo               = typeInfo;
    node->resolvedSymbolOverload = node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Struct);
    SWAG_CHECK(node->resolvedSymbolOverload);

    // We are parsing the swag module
    if (sourceFile->swagFile)
        g_Workspace.swagScope.registerType(node->typeInfo);
    return true;
}
