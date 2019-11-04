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
    auto node = CastAst<AstImpl>(context->node, AstNodeKind::Impl);

    // Be sure this is a struct
    auto typeInfo = node->identifier->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Struct)
    {
        Diagnostic diag{node->identifier, format("'%s' is %s and should be a struct", node->identifier->name.c_str(), TypeInfo::getArticleKindName(typeInfo))};
        Diagnostic note{node->identifier->resolvedSymbolOverload->node, node->identifier->resolvedSymbolOverload->node->token, format("this is the definition of '%s'", node->identifier->name.c_str()), DiagnosticLevel::Note};
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
    if (node->parentAttributes && !(node->flags & AST_FROM_GENERIC))
        SWAG_CHECK(collectAttributes(context, typeInfo->attributes, node->parentAttributes, node, AstNodeKind::StructDecl, node->attributeFlags));

    // Register symbol with its type
    SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Struct, nullptr, symbolFlags | OVERLOAD_INCOMPLETE, nullptr, 0));

    return true;
}

bool SemanticJob::resolveStruct(SemanticContext* context)
{
    auto node       = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    auto job        = context->job;

    typeInfo->structNode = node;
    typeInfo->name       = format("%s", node->name.c_str());
    if (!typeInfo->opInitFct)
        typeInfo->opInitFct = node->defaultOpInit;
    if (node->attributeFlags & ATTRIBUTE_PACK)
        node->packing = 1;

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;
    uint32_t structFlags   = TYPEINFO_STRUCT_ALL_UNINITIALIZED;

    vector<AstNode*>& childs = (node->flags & AST_STRUCT_COMPOUND) ? job->tmpNodes : node->content->childs;
    if (node->flags & AST_STRUCT_COMPOUND)
        job->tmpNodes = node->content->childs;

    for (int i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];

        switch (child->kind)
        {
        case AstNodeKind::AttrUse:
            continue;
        case AstNodeKind::Statement:
        case AstNodeKind::CompilerIfBlock:
            SWAG_ASSERT(node->flags & AST_STRUCT_COMPOUND);
            job->tmpNodes.insert(job->tmpNodes.end(), child->childs.begin(), child->childs.end());
            continue;
        case AstNodeKind::CompilerIf:
            SWAG_ASSERT(node->flags & AST_STRUCT_COMPOUND);
            AstIf* compilerIf = CastAst<AstIf>(child, AstNodeKind::CompilerIf);
            if (!(compilerIf->ifBlock->flags & AST_DISABLED))
                job->tmpNodes.push_back(compilerIf->ifBlock);
            else if (compilerIf->elseBlock)
                job->tmpNodes.push_back(compilerIf->elseBlock);
            continue;
        }

        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);

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
            SWAG_VERIFY(varDecl->assignment->flags & AST_CONST_EXPR, context->errorContext.report({varDecl->assignment, "cannot evaluate initialization expression at compile time"}));

            auto typeInfoAssignment = TypeManager::concreteType(varDecl->assignment->typeInfo, CONCRETE_ALIAS);
            typeInfoAssignment      = TypeManager::concreteType(varDecl->assignment->typeInfo, CONCRETE_ENUM);

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
            SWAG_VERIFY(!(child->typeInfo->flags & TYPEINFO_GENERIC), context->errorContext.report({child, format("cannot instanciate variable because type '%s' is generic", child->typeInfo->name.c_str())}));
        }

        auto realStorageOffset = storageOffset;

        // Attribute 'swag.offset' can be used to force the storage offset of the member
        ComputedValue forceOffset;
        bool          relocated = false;
        if (typeParam && typeParam->attributes.getValue("swag.offset.name", forceOffset))
        {
            for (auto p : typeInfo->childs)
            {
                if (p->namedParam == forceOffset.text)
                {
                    realStorageOffset = p->offset;
                    relocated         = true;
                    break;
                }
            }

            if (!relocated)
            {
                // The attribute itself stores the corresponding node in its value
                ComputedValue valueNode;
                bool          found = typeParam->attributes.getValue("swag.offset", valueNode);
                SWAG_ASSERT(found);
                auto attrNode = (AstNode*) valueNode.reg.pointer;
                return context->errorContext.report({attrNode, format("cannot find structure member '%s' to compute variable relocation", forceOffset.text.c_str())});
            }
        }

        // Compute padding
        if (!relocated && node->packing > 1 && child->typeInfo->sizeOf)
        {
            auto padding = realStorageOffset & (child->typeInfo->sizeOf - 1);
            padding %= node->packing;
            if (padding)
            {
                padding = child->typeInfo->sizeOf - padding;
                padding %= node->packing;
                realStorageOffset += padding;
                storageOffset += padding;
            }
        }

        typeInfo->childs[storageIndex]->offset       = realStorageOffset;
        child->resolvedSymbolOverload->storageOffset = realStorageOffset;
        child->resolvedSymbolOverload->storageIndex  = storageIndex;

        typeInfo->sizeOf = max(typeInfo->sizeOf, (int) realStorageOffset + child->typeInfo->sizeOf);

        if (relocated)
            storageOffset = max(storageOffset, realStorageOffset + child->typeInfo->sizeOf);
        else if (node->packing)
            storageOffset += child->typeInfo->sizeOf;

        storageIndex++;
    }

    // Check public
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (!node->ownerScope->isGlobal())
            return context->errorContext.report({node, node->token, format("embedded struct '%s' cannot be public", node->name.c_str())});

        if (!(node->flags & AST_FROM_GENERIC))
        {
            SWAG_VERIFY(!typeInfo->childs.empty(), context->errorContext.report({node, node->token, format("struct '%s' is public and cannot be empty", node->name.c_str())}));
            node->ownerScope->addPublicStruct(node);
        }
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
