#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Workspace.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ByteCode.h"
#include "ModuleManager.h"

bool SemanticJob::waitForStructUserOps(SemanticContext* context, AstNode* node)
{
    waitUserOp(context, "opPostCopy", node);
    if (context->result == ContextResult::Pending)
        return true;
    waitUserOp(context, "opPostMove", node);
    if (context->result == ContextResult::Pending)
        return true;
    waitUserOp(context, "opDrop", node);
    if (context->result == ContextResult::Pending)
        return true;
    return true;
}

bool SemanticJob::resolveImplFor(SemanticContext* context)
{
    auto node = CastAst<AstImpl>(context->node, AstNodeKind::Impl);
    auto job  = context->job;

    // Be sure the first identifier is an interface
    auto typeInfo = node->identifier->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Interface)
    {
        Diagnostic diag{node->identifier, format("'%s' is %s and should be an interface", node->identifier->name.c_str(), TypeInfo::getArticleKindName(typeInfo))};
        Diagnostic note{node->identifier->resolvedSymbolOverload->node, node->identifier->resolvedSymbolOverload->node->token, format("this is the definition of '%s'", node->identifier->name.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note);
    }

    // Be sure the second identifier is a struct
    typeInfo = node->identifierFor->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Struct)
    {
        Diagnostic diag{node->identifierFor, format("'%s' is %s and should be a struct", node->identifierFor->name.c_str(), TypeInfo::getArticleKindName(typeInfo))};
        Diagnostic note{node->identifierFor->resolvedSymbolOverload->node, node->identifierFor->resolvedSymbolOverload->node->token, format("this is the definition of '%s'", node->identifier->name.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note);
    }

    VectorNative<AstNode*>& childs = (node->flags & AST_STRUCT_COMPOUND) ? job->tmpNodes : node->childs;
    if (node->flags & AST_STRUCT_COMPOUND)
    {
        job->tmpNodes.clear();
        flattenStructChilds(context, node, job->tmpNodes);
    }

    SWAG_ASSERT(node->childs[0]->kind == AstNodeKind::IdentifierRef);
    SWAG_ASSERT(node->childs[1]->kind == AstNodeKind::IdentifierRef);
    auto typeBaseInterface = CastTypeInfo<TypeInfoStruct>(node->childs[0]->typeInfo, TypeInfoKind::Interface);
    auto typeStruct        = CastTypeInfo<TypeInfoStruct>(node->childs[1]->typeInfo, TypeInfoKind::Struct);

    // Be sure interface has been fully solved
    {
        scoped_lock lk(node->identifier->mutex);
        if (node->identifier->resolvedSymbolName->cptOverloads)
        {
            job->waitForSymbolNoLock(node->identifier->resolvedSymbolName);
            return true;
        }
    }

    // We need now the pointer to the itable
    auto     typeInterface   = CastTypeInfo<TypeInfoStruct>(typeBaseInterface->itable, TypeInfoKind::Struct);
    uint32_t numFctInterface = (uint32_t) typeInterface->fields.size();

    map<TypeInfoParam*, AstNode*> mapItToFunc;
    VectorNative<AstFuncDecl*>    mapItIdxToFunc;
    mapItIdxToFunc.set_size_clear(numFctInterface);

    // Register interface in the structure
    TypeInfoParam* typeParamItf = nullptr;
    {
        unique_lock lk(typeStruct->mutex);
        typeParamItf = typeStruct->hasInterfaceNoLock(typeBaseInterface);
        if (!typeParamItf)
        {
            typeParamItf = g_Allocator.alloc<TypeInfoParam>();
            typeBaseInterface->computeScopedName();
            typeParamItf->namedParam = typeBaseInterface->scopedName;
            typeParamItf->typeInfo   = typeBaseInterface;
            typeParamItf->node       = typeBaseInterface->declNode;
            typeStruct->interfaces.push_back(typeParamItf);
        }
    }

    for (int i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];
        if (child->kind != AstNodeKind::FuncDecl)
            continue;

        // We need to be sure function semantic is done
        {
            auto symbolName = node->scope->symTable.find(child->name);
            SWAG_ASSERT(symbolName);
            scoped_lock lk(symbolName->mutex);
            if (symbolName->cptOverloads)
            {
                job->waitForSymbolNoLock(symbolName);
                return true;
            }
        }

        // We need to be have a bytecode pointer to be able to reference it in the itable
        if (!(child->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            ByteCodeGenJob::askForByteCode(context->job, child, ASKBC_WAIT_SEMANTIC_RESOLVED);
            if (context->result == ContextResult::Pending)
                return true;
        }

        // We need to search the function (as a variable) in the interface
        auto symbolName = typeInterface->findChildByNameNoLock(child->name); // O(n) !
        if (!symbolName)
        {
            Diagnostic diag{child, child->token, format("function '%s' is not part of interface '%s'", child->name.c_str(), typeBaseInterface->name.c_str())};
            Diagnostic note{typeBaseInterface->declNode, typeBaseInterface->declNode->token, format("this is the definition of interface '%s'", typeBaseInterface->name.c_str()), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        // Match function signature
        auto typeLambda = CastTypeInfo<TypeInfoFuncAttr>(symbolName->typeInfo, TypeInfoKind::Lambda);
        auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeLambda->isSame(typeFunc, ISSAME_EXACT | ISSAME_INTERFACE))
        {
            Diagnostic diag{child, child->token, format("function '%s' has an incorrect signature for interface '%s'", child->name.c_str(), typeBaseInterface->name.c_str())};
            Diagnostic note{symbolName->node, "should be", DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        // First parameter in the impl block must be a pointer to the struct
        SWAG_VERIFY(typeFunc->parameters.size(), context->report({child, child->token, format("missing first parameter 'self' for interface function '%s'", child->name.c_str())}));
        auto firstParamType = typeFunc->parameters[0]->typeInfo;
        SWAG_VERIFY(firstParamType->kind == TypeInfoKind::Pointer, context->report({typeFunc->parameters[0]->node, format("bad type for first parameter of interface function implementation ('self' expected, '%s' provided)", firstParamType->name.c_str())}));
        auto firstParamPtr = CastTypeInfo<TypeInfoPointer>(firstParamType, TypeInfoKind::Pointer);
        SWAG_VERIFY(firstParamPtr->ptrCount == 1, context->report({typeFunc->parameters[0]->node, format("bad type for first parameter of interface function implementation ('self' expected, '%s' provided)", firstParamType->name.c_str())}));
        SWAG_VERIFY(firstParamPtr->pointedType == typeStruct, context->report({typeFunc->parameters[0]->node, format("bad type for first parameter of interface function implementation ('self' expected, '%s' provided)", firstParamType->name.c_str())}));

        // use resolvedUserOpSymbolOverload to store the match
        mapItToFunc[symbolName]           = child;
        mapItIdxToFunc[symbolName->index] = (AstFuncDecl*) child;
    }

    // Be sure every functions of the interface has been covered
    Diagnostic                diag{node, node->token, format("some functions of interface '%s' are not implemented", typeBaseInterface->name.c_str())};
    vector<const Diagnostic*> notes;
    for (uint32_t idx = 0; idx < numFctInterface; idx++)
    {
        if (mapItIdxToFunc[idx] == nullptr)
        {
            auto missingNode = typeInterface->fields[idx];
            notes.push_back(new Diagnostic({missingNode->node, missingNode->node->token, format("missing '%s'", missingNode->namedParam.c_str()), DiagnosticLevel::Note}));
        }
    }

    if (!notes.empty())
        return context->report(diag, notes);

    // Construct itable in the constant segment
    auto     module       = context->job->sourceFile->module;
    uint32_t itableOffset = module->constantSegment.reserve(numFctInterface * sizeof(void*));
    void**   ptrITable    = (void**) module->constantSegment.address(itableOffset);
    auto     offset       = itableOffset;
    for (uint32_t i = 0; i < numFctInterface; i++)
    {
        auto funcChild = mapItIdxToFunc[i];
        if (funcChild->attributeFlags & ATTRIBUTE_FOREIGN)
        {
            funcChild->computeFullNameForeign(true);

            // Need to patch the segment at runtime, init, with the real address of the function
            module->constantSegment.addInitPtrFunc(offset, funcChild->fullnameForeign, DataSegment::RelocType::Foreign);

            // This will be filled when the module will be loaded, with the real function address
            *ptrITable = nullptr;
            g_ModuleMgr.addPatchFuncAddress((void**) module->constantSegment.address(offset), funcChild);
        }
        else
        {
            *ptrITable = doByteCodeLambda(funcChild->bc);
            module->constantSegment.addInitPtrFunc(offset, funcChild->bc->callName(), DataSegment::RelocType::Local);
        }

        ptrITable++;
        offset += sizeof(void*);
    }

    // Setup constant segment offset
    typeParamItf->offset = itableOffset;
    decreaseInterfaceCount(typeStruct);

    return true;
}

void SemanticJob::decreaseInterfaceCount(TypeInfoStruct* typeInfoStruct)
{
    unique_lock lk(typeInfoStruct->mutex);
    unique_lock lk1(typeInfoStruct->scope->symTable.mutex);

    SWAG_ASSERT(typeInfoStruct->cptRemainingInterfaces);
    typeInfoStruct->cptRemainingInterfaces--;
    if (!typeInfoStruct->cptRemainingInterfaces)
    {
        typeInfoStruct->scope->dependentJobs.setRunning();
    }
}

void SemanticJob::decreaseMethodCount(TypeInfoStruct* typeInfoStruct)
{
    unique_lock lk(typeInfoStruct->mutex);
    unique_lock lk1(typeInfoStruct->scope->symTable.mutex);

    SWAG_ASSERT(typeInfoStruct->cptRemainingMethods);
    typeInfoStruct->cptRemainingMethods--;
    if (!typeInfoStruct->cptRemainingMethods)
    {
        typeInfoStruct->scope->dependentJobs.setRunning();
    }
}

bool SemanticJob::CheckImplScopes(SemanticContext* context, AstImpl* node, Scope* scopeImpl, Scope* scope)
{
    if (scopeImpl != scope)
    {
        if (scopeImpl->flags & SCOPE_PRIVATE)
        {
            Diagnostic diag{node->identifier, format("the implementation block for '%s' is private but the corresponding identifier is not", node->identifier->name.c_str())};
            Diagnostic note{node->identifier->resolvedSymbolOverload->node, node->identifier->resolvedSymbolOverload->node->token, format("this is the definition of '%s'", node->identifier->name.c_str()), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        if (scope->flags & SCOPE_PRIVATE)
        {
            Diagnostic diag{node->identifier, format("the implementation block for '%s' is not private but the corresponding identifier is", node->identifier->name.c_str())};
            Diagnostic note{node->identifier->resolvedSymbolOverload->node, node->identifier->resolvedSymbolOverload->node->token, format("this is the definition of '%s'", node->identifier->name.c_str()), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        SWAG_ASSERT(false);
    }

    return true;
}

bool SemanticJob::resolveImpl(SemanticContext* context)
{
    auto node = CastAst<AstImpl>(context->node, AstNodeKind::Impl);

    // Be sure this is a struct
    auto typeInfo = node->identifier->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Struct && typeInfo->kind != TypeInfoKind::Enum)
    {
        Diagnostic diag{node->identifier, format("'%s' is %s and should be a struct or an enum", node->identifier->name.c_str(), TypeInfo::getArticleKindName(typeInfo))};
        Diagnostic note{node->identifier->resolvedSymbolOverload->node, node->identifier->resolvedSymbolOverload->node->token, format("this is the definition of '%s'", node->identifier->name.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note);
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto structNode = CastAst<AstStruct>(node->identifier->resolvedSymbolOverload->node, AstNodeKind::StructDecl);
        SWAG_CHECK(CheckImplScopes(context, node, node->structScope, structNode->scope));
    }

    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        auto enumNode = CastAst<AstEnum>(node->identifier->resolvedSymbolOverload->node, AstNodeKind::EnumDecl);
        SWAG_CHECK(CheckImplScopes(context, node, node->structScope, enumNode->scope));
    }

    return true;
}

bool SemanticJob::preResolveStruct(SemanticContext* context)
{
    auto node = (AstStruct*) context->node->parent;
    SWAG_ASSERT(node->kind == AstNodeKind::StructDecl || node->kind == AstNodeKind::InterfaceDecl || node->kind == AstNodeKind::TypeSet);
    auto typeInfo = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);

    // Add generic parameters
    uint32_t symbolFlags = 0;
    if (!(node->flags & AST_FROM_GENERIC))
    {
        if (node->genericParameters)
        {
            node->flags |= AST_IS_GENERIC;
            typeInfo->flags |= TYPEINFO_GENERIC;
            symbolFlags |= OVERLOAD_GENERIC;
            for (auto param : node->genericParameters->childs)
            {
                auto funcParam        = g_Allocator.alloc<TypeInfoParam>();
                funcParam->namedParam = param->name;
                funcParam->name       = param->typeInfo->name;
                funcParam->typeInfo   = param->typeInfo;
                funcParam->sizeOf     = param->typeInfo->sizeOf;
                typeInfo->genericParameters.push_back(funcParam);
                typeInfo->sizeOf += param->typeInfo->sizeOf;
            }

            typeInfo->forceComputeName();
        }
    }

    // Attributes
    if (node->parentAttributes && !(node->flags & AST_FROM_GENERIC))
        SWAG_CHECK(collectAttributes(context, typeInfo->attributes, node->parentAttributes, node, node->kind, node->attributeFlags));

    // Register symbol with its type
    SymbolKind symbolKind = SymbolKind::Struct;
    switch (node->kind)
    {
    case AstNodeKind::StructDecl:
        symbolKind = SymbolKind::Struct;
        break;
    case AstNodeKind::InterfaceDecl:
        symbolKind = SymbolKind::Interface;
        break;
    case AstNodeKind::TypeSet:
        symbolKind = SymbolKind::TypeSet;
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, symbolKind, nullptr, symbolFlags | OVERLOAD_INCOMPLETE, nullptr, 0));
    return true;
}

void SemanticJob::flattenStructChilds(SemanticContext* context, AstNode* parent, VectorNative<AstNode*>& result)
{
#ifdef SWAG_HAS_ASSERT
    auto node = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
#endif

    for (auto child : parent->childs)
    {
        switch (child->kind)
        {
        case AstNodeKind::Statement:
        case AstNodeKind::CompilerIfBlock:
        case AstNodeKind::CompilerAst:
        case AstNodeKind::CompilerAssert:
            SWAG_ASSERT(node->flags & AST_STRUCT_COMPOUND);
            flattenStructChilds(context, child, result);
            continue;

        case AstNodeKind::CompilerIf:
        {
            SWAG_ASSERT(node->flags & AST_STRUCT_COMPOUND);
            AstIf* compilerIf = CastAst<AstIf>(child, AstNodeKind::CompilerIf);
            if (!(compilerIf->ifBlock->flags & AST_NO_SEMANTIC))
                flattenStructChilds(context, compilerIf->ifBlock, result);
            else if (compilerIf->elseBlock)
                flattenStructChilds(context, compilerIf->elseBlock, result);
            continue;
        }

        case AstNodeKind::AttrUse:
            continue;
        }

        result.push_back(child);
    }
}

bool SemanticJob::resolveStruct(SemanticContext* context)
{
    auto node       = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    auto job        = context->job;

    SWAG_ASSERT(typeInfo->declNode);
    SWAG_ASSERT(typeInfo->declNode == node);

    if (node->attributeFlags & ATTRIBUTE_PACK)
        node->packing = 1;

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;
    uint32_t structFlags   = TYPEINFO_STRUCT_ALL_UNINITIALIZED;

    // No need to flatten structure if it's not a compound (optim)
    VectorNative<AstNode*>& childs = (node->flags & AST_STRUCT_COMPOUND) ? job->tmpNodes : node->content->childs;
    if (node->flags & AST_STRUCT_COMPOUND)
    {
        job->tmpNodes.clear();
        flattenStructChilds(context, node->content, job->tmpNodes);
    }

    for (int i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];
        if (child->kind != AstNodeKind::VarDecl)
            continue;

        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);

        // Using can only be used on a structure
        if (child->flags & AST_DECL_USING && child->typeInfo->kind != TypeInfoKind::Struct)
            return context->report({child, format("'using' on a field is only valid for a struct type ('%s' provided)", child->typeInfo->name.c_str())});

        TypeInfoParam* typeParam = nullptr;
        if (!(node->flags & AST_FROM_GENERIC) || !(child->flags & AST_STRUCT_REGISTERED))
        {
            typeParam             = g_Allocator.alloc<TypeInfoParam>();
            typeParam->namedParam = child->name;
            typeParam->name       = child->typeInfo->name;
            typeParam->typeInfo   = child->typeInfo;
            typeParam->sizeOf     = child->typeInfo->sizeOf;
            typeParam->offset     = storageOffset;
            typeParam->hasUsing   = varDecl->flags & AST_DECL_USING;
            if (child->parentAttributes)
                SWAG_CHECK(collectAttributes(context, typeParam->attributes, child->parentAttributes, child, AstNodeKind::VarDecl, child->attributeFlags));
            typeInfo->fields.push_back(typeParam);
        }

        child->flags |= AST_STRUCT_REGISTERED;
        typeParam           = typeInfo->fields[storageIndex];
        typeParam->typeInfo = child->typeInfo;
        typeParam->node     = child;
        typeParam->index    = storageIndex;

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

            if (varDecl->type && varDecl->type->flags & AST_HAS_STRUCT_PARAMETERS)
            {
                structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                if (!(varDecl->type->flags & AST_VALUE_COMPUTED))
                {
                    varDecl->type->flags |= AST_VALUE_COMPUTED;
                    SWAG_CHECK(collectAssignment(context, varDecl->type->computedValue.reg.offset, varDecl, &sourceFile->module->constantSegment));
                }
            }
        }

        // Var is an array of structs
        else if (varDecl->typeInfo->kind == TypeInfoKind::Array && !varDecl->assignment)
        {
            auto varTypeArray = CastTypeInfo<TypeInfoArray>(varDecl->typeInfo, TypeInfoKind::Array);
            if (varTypeArray->pointedType->kind == TypeInfoKind::Struct)
            {
                if (varTypeArray->pointedType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
                    structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                if (!(varTypeArray->pointedType->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
                    structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
            }
        }

        // Var has an initialization
        else if (varDecl->assignment && !(varDecl->flags & AST_EXPLICITLY_NOT_INITIALIZED))
        {
            SWAG_VERIFY(varDecl->assignment->flags & AST_CONST_EXPR, context->report({varDecl->assignment, "initialization expression cannot be evaluated at compile time"}));

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
            SWAG_VERIFY(!(child->typeInfo->flags & TYPEINFO_GENERIC), context->report({child, format("cannot resolve struct because type '%s' is generic", child->typeInfo->name.c_str())}));
        }

        auto realStorageOffset = storageOffset;

        // Attribute 'swag.offset' can be used to force the storage offset of the member
        ComputedValue forceOffset;
        bool          relocated = false;
        if (typeParam && typeParam->attributes.getValue("swag.offset", "name", forceOffset))
        {
            for (auto p : typeInfo->fields)
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
                auto attr = typeParam->attributes.getAttribute("swag.offset");
                SWAG_ASSERT(attr);
                return context->report({attr->node, format("cannot find structure member '%s' to compute variable relocation", forceOffset.text.c_str())});
            }
        }

        // Compute padding
        auto paddingSizeof = child->typeInfo->sizeOf;
        if (child->typeInfo->kind == TypeInfoKind::Struct)
            paddingSizeof = ((TypeInfoStruct*) child->typeInfo)->maxPaddingSize;
        if (!relocated && node->packing > 1 && paddingSizeof)
        {
            auto padding = realStorageOffset & (paddingSizeof - 1);
            padding %= node->packing;
            if (padding)
            {
                padding = paddingSizeof - padding;
                padding %= node->packing;
                realStorageOffset += padding;
                storageOffset += padding;
            }
        }

        if (child->typeInfo->kind != TypeInfoKind::Struct)
            typeInfo->maxPaddingSize = max(typeInfo->maxPaddingSize, child->typeInfo->sizeOf);

        typeParam->offset                             = realStorageOffset;
        child->resolvedSymbolOverload->storageOffset  = realStorageOffset;
        child->resolvedSymbolOverload->storageIndex   = storageIndex;
        child->resolvedSymbolOverload->attributeFlags = child->attributeFlags;

        typeInfo->sizeOf = max(typeInfo->sizeOf, (int) realStorageOffset + child->typeInfo->sizeOf);

        if (relocated)
            storageOffset = max(storageOffset, realStorageOffset + child->typeInfo->sizeOf);
        else if (node->packing)
            storageOffset += child->typeInfo->sizeOf;

        // Created a generic alias
        if (!(child->flags & AST_AUTO_NAME))
        {
            bool hasItemName = false;
            if (child->name.length() > 4 && child->name[0] == 'i' && child->name[1] == 't' && child->name[2] == 'e' && child->name[3] == 'm')
                hasItemName = true;

            // User cannot name its variables itemX
            if (!(node->flags & AST_GENERATED) && hasItemName)
            {
                return context->report({child, format("structure member name '%s' starts with 'item', and this is reserved by the language", child->name.c_str())});
            }

            if (!hasItemName)
            {
                auto    overload = child->resolvedSymbolOverload;
                Utf8Crc name     = format("item%u", storageIndex);
                auto&   symTable = node->scope->symTable;
                symTable.addSymbolTypeInfo(context, child, child->typeInfo, SymbolKind::Variable, nullptr, overload->flags, nullptr, overload->storageOffset, &name);
            }
        }

        storageIndex++;
    }

    // A struct cannot have a zero size
    typeInfo->sizeOf = max(typeInfo->sizeOf, 1);

    // Check public
    if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE))
    {
        if (!node->ownerScope->isGlobal())
            return context->report({node, node->token, format("embedded struct '%s' cannot be public", node->name.c_str())});
        if (!(node->flags & AST_FROM_GENERIC) || (node->flags & AST_FROM_BAKE))
            node->ownerScope->addPublicStruct(node);
    }

    // Need to recompute it if it's from generic
    if (node->flags & AST_FROM_GENERIC)
    {
        typeInfo->flags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
        typeInfo->flags &= ~TYPEINFO_STRUCT_HAS_INIT_VALUES;
    }

    typeInfo->flags |= (structFlags & TYPEINFO_STRUCT_ALL_UNINITIALIZED);
    typeInfo->flags |= (structFlags & TYPEINFO_STRUCT_HAS_INIT_VALUES);

    // Register symbol with its type
    node->typeInfo               = typeInfo;
    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::Struct);
    SWAG_CHECK(node->resolvedSymbolOverload);

    // If there's an alias, register it also
    if (node->nodeAlias)
        SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, node->nodeAlias, node->typeInfo, SymbolKind::TypeAlias));

    // We are parsing the swag module
    if (sourceFile->isBootstrapFile)
        g_Workspace.swagScope.registerType(node->typeInfo);

    // Generate all functions associated with a struct
    if (!(typeInfo->flags & TYPEINFO_GENERIC))
    {
        node->flags &= ~AST_NO_BYTECODE;
        node->flags |= AST_NO_BYTECODE_CHILDS;

        SWAG_ASSERT(!node->byteCodeJob);
        node->byteCodeJob               = g_Pool_byteCodeGenJob.alloc();
        node->byteCodeJob->sourceFile   = sourceFile;
        node->byteCodeJob->module       = sourceFile->module;
        node->byteCodeJob->dependentJob = context->job->dependentJob;
        node->byteCodeJob->nodes.push_back(node);
        node->byteCodeFct = ByteCodeGenJob::emitStruct;
        g_ThreadMgr.addJob(node->byteCodeJob);
    }

    return true;
}

bool SemanticJob::resolveInterface(SemanticContext* context)
{
    auto node          = CastAst<AstStruct>(context->node, AstNodeKind::InterfaceDecl);
    auto sourceFile    = context->sourceFile;
    auto typeInterface = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    auto job           = context->job;

    typeInterface->declNode   = node;
    typeInterface->name       = node->name;
    typeInterface->nakedName  = node->name;
    typeInterface->structName = node->name;
    typeInterface->kind       = TypeInfoKind::Interface;

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;

    VectorNative<AstNode*>& childs = (node->flags & AST_STRUCT_COMPOUND) ? job->tmpNodes : node->content->childs;
    if (node->flags & AST_STRUCT_COMPOUND)
    {
        job->tmpNodes.clear();
        flattenStructChilds(context, node->content, job->tmpNodes);
    }

    // itable
    auto typeITable        = g_Allocator.alloc<TypeInfoStruct>();
    typeITable->name       = node->name;
    typeITable->nakedName  = node->name;
    typeITable->structName = node->name;
    typeITable->scope      = Ast::newScope(node, node->name, ScopeKind::Struct, nullptr);

    for (int i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];
        if (child->kind != AstNodeKind::VarDecl)
            continue;

        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);

        TypeInfoParam* typeParam = nullptr;
        if (!(node->flags & AST_FROM_GENERIC))
        {
            typeParam             = g_Allocator.alloc<TypeInfoParam>();
            typeParam->namedParam = child->name;
            typeParam->name       = child->typeInfo->name;
            typeParam->sizeOf     = child->typeInfo->sizeOf;
            typeParam->offset     = storageOffset;
            typeParam->node       = child;
            if (child->parentAttributes)
                SWAG_CHECK(collectAttributes(context, typeParam->attributes, child->parentAttributes, child, AstNodeKind::VarDecl, child->attributeFlags));
            typeITable->fields.push_back(typeParam);

            // Verify signature
            typeParam->typeInfo = TypeManager::concreteType(child->typeInfo, CONCRETE_ALIAS);
            SWAG_VERIFY(typeParam->typeInfo->kind == TypeInfoKind::Lambda, context->report({child, format("an interface can only contain members of type 'lambda' ('%s' provided)", child->typeInfo->name.c_str())}));
            auto typeLambda = CastTypeInfo<TypeInfoFuncAttr>(typeParam->typeInfo, TypeInfoKind::Lambda);
            SWAG_VERIFY(typeLambda->parameters.size() >= 1, context->report({child, format("missing parameters for interface member '%s' ('self' expected as first parameter)", child->name.c_str())}));
            auto firstParamType = typeLambda->parameters[0]->typeInfo;
            SWAG_VERIFY(firstParamType->kind == TypeInfoKind::Pointer, context->report({typeLambda->parameters[0]->node, format("bad type for first parameter of interface member ('self' expected, '%s' provided)", firstParamType->name.c_str())}));
            auto firstParamPtr = CastTypeInfo<TypeInfoPointer>(firstParamType, TypeInfoKind::Pointer);
            SWAG_VERIFY(firstParamPtr->ptrCount == 1, context->report({typeLambda->parameters[0]->node, format("bad type for first parameter of interface member ('self' expected, '%s' provided)", firstParamType->name.c_str())}));
            SWAG_VERIFY(firstParamPtr->pointedType == typeInterface, context->report({typeLambda->parameters[0]->node, format("bad type for first parameter of interface member ('self' expected, '%s' provided)", firstParamType->name.c_str())}));
        }

        typeParam           = typeITable->fields[storageIndex];
        typeParam->typeInfo = child->typeInfo;
        typeParam->node     = child;
        typeParam->index    = storageIndex;

        SWAG_VERIFY(!varDecl->assignment, context->report({varDecl->assignment, "cannot initialize an interface member"}));

        if (!(node->flags & AST_IS_GENERIC))
        {
            SWAG_VERIFY(!(child->typeInfo->flags & TYPEINFO_GENERIC), context->report({child, format("cannot resolve interface because type '%s' is generic", child->typeInfo->name.c_str())}));
        }

        if (typeParam->attributes.hasAttribute("swag.offset"))
        {
            auto attr = typeParam->attributes.getAttribute("swag.offset");
            SWAG_ASSERT(attr);
            return context->report({attr->node, "cannot relocate an interface member"});
        }

        typeParam->offset                            = storageOffset;
        child->resolvedSymbolOverload->storageOffset = storageOffset;
        child->resolvedSymbolOverload->storageIndex  = storageIndex;

        SWAG_ASSERT(child->typeInfo->sizeOf == sizeof(void*));
        typeITable->sizeOf += sizeof(void*);

        // Register lambda variable in the scope of the itable struct
        auto  overload = child->resolvedSymbolOverload;
        auto& symTable = typeITable->scope->symTable;
        symTable.addSymbolTypeInfo(context, child, child->typeInfo, SymbolKind::Variable, nullptr, overload->flags, nullptr, overload->storageOffset);

        storageOffset += sizeof(void*);
        storageIndex++;
    }

    SWAG_VERIFY(!typeITable->fields.empty(), context->report({node, node->token, format("interface '%s' is empty", node->name.c_str())}));
    typeInterface->itable = typeITable;

    // Struct interface, with one pointer for the data, and one pointer for itable
    if (!(node->flags & AST_FROM_GENERIC))
    {
        auto typeParam      = g_Allocator.alloc<TypeInfoParam>();
        typeParam->typeInfo = g_TypeMgr.typeInfoPVoid;
        typeParam->name     = typeParam->typeInfo->name;
        typeParam->sizeOf   = typeParam->typeInfo->sizeOf;
        typeParam->offset   = 0;
        typeInterface->fields.push_back(typeParam);
        typeInterface->sizeOf += sizeof(void*);

        typeParam           = g_Allocator.alloc<TypeInfoParam>();
        typeParam->typeInfo = typeITable;
        typeParam->name     = typeParam->typeInfo->name;
        typeParam->sizeOf   = typeParam->typeInfo->sizeOf;
        typeParam->offset   = sizeof(void*);
        typeParam->index    = 1;
        typeInterface->fields.push_back(typeParam);
        typeInterface->sizeOf += sizeof(void*);
    }

    // Check public
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (!node->ownerScope->isGlobal())
            return context->report({node, node->token, format("embedded interface '%s' cannot be public", node->name.c_str())});

        if (!(node->flags & AST_FROM_GENERIC))
            node->ownerScope->addPublicInterface(node);
    }

    // Register symbol with its type
    node->typeInfo               = typeInterface;
    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::Interface);
    SWAG_CHECK(node->resolvedSymbolOverload);

    // We are parsing the swag module
    if (sourceFile->isBootstrapFile)
        g_Workspace.swagScope.registerType(node->typeInfo);

    return true;
}

bool SemanticJob::resolveTypeSet(SemanticContext* context)
{
    auto node    = CastAst<AstStruct>(context->node, AstNodeKind::TypeSet);
    auto typeSet = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    auto job     = context->job;

    typeSet->declNode   = node;
    typeSet->name       = node->name;
    typeSet->nakedName  = node->name;
    typeSet->structName = node->name;
    typeSet->kind       = TypeInfoKind::TypeSet;

    VectorNative<AstNode*>& childs = (node->flags & AST_STRUCT_COMPOUND) ? job->tmpNodes : node->content->childs;
    if (node->flags & AST_STRUCT_COMPOUND)
    {
        job->tmpNodes.clear();
        flattenStructChilds(context, node->content, job->tmpNodes);
    }

    // Check public
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (!node->ownerScope->isGlobal())
            return context->report({node, node->token, format("embedded typeset '%s' cannot be public", node->name.c_str())});

        if (!(node->flags & AST_FROM_GENERIC))
            node->ownerScope->addPublicInterface(node);
    }

    uint32_t storageIndex = 0;
    for (int i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];
        if (child->kind != AstNodeKind::Alias)
            continue;

        TypeInfoParam* typeParam = nullptr;
        if (!(node->flags & AST_FROM_GENERIC))
        {
            typeParam             = g_Allocator.alloc<TypeInfoParam>();
            typeParam->namedParam = child->name;
            typeParam->name       = child->typeInfo->name;
            typeParam->sizeOf     = 0;
            typeParam->node       = child;
            if (child->parentAttributes)
                SWAG_CHECK(collectAttributes(context, typeParam->attributes, child->parentAttributes, child, AstNodeKind::VarDecl, child->attributeFlags));
            typeSet->fields.push_back(typeParam);
        }

        typeParam           = typeSet->fields[storageIndex];
        typeParam->typeInfo = child->typeInfo;
        typeParam->node     = child;
        typeParam->index    = storageIndex;

        if (!(node->flags & AST_IS_GENERIC))
        {
            SWAG_VERIFY(!(child->typeInfo->flags & TYPEINFO_GENERIC), context->report({child, format("cannot resolve typeset because type '%s' is generic", child->typeInfo->name.c_str())}));
        }

        if (typeParam->attributes.hasAttribute("swag.offset"))
        {
            auto attr = typeParam->attributes.getAttribute("swag.offset");
            SWAG_ASSERT(attr);
            return context->report({attr->node, "cannot relocate a typeset member"});
        }

        storageIndex++;
    }

    SWAG_VERIFY(!typeSet->fields.empty(), context->report({node, node->token, format("typeset '%s' is empty", node->name.c_str())}));

    // Register symbol with its type
    node->typeInfo               = typeSet;
    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::TypeSet);
    SWAG_CHECK(node->resolvedSymbolOverload);

    return true;
}
