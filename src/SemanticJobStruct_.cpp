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
#include "ErrorIds.h"

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

bool SemanticJob::resolveImplForAfterFor(SemanticContext* context)
{
    auto id   = context->node;
    auto node = CastAst<AstImpl>(context->node->parent, AstNodeKind::Impl);

    if (id->resolvedSymbolName->kind != SymbolKind::Struct)
        return context->report({id->childs.back(), Utf8::format(Msg0290, id->resolvedSymbolName->name.c_str(), SymTable::getArticleKindName(id->resolvedSymbolName->kind))});

    auto structDecl = CastAst<AstStruct>(id->resolvedSymbolOverload->node, AstNodeKind::StructDecl);

    if (id->resolvedSymbolOverload->flags & OVERLOAD_GENERIC)
    {
        if (!(node->flags & AST_FROM_GENERIC))
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
            node->sourceFile->module->decImplForToSolve(typeStruct);
        }

        return true;
    }

    if (structDecl->scope != node->structScope)
    {
        auto        typeStruct = CastTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
        unique_lock lk1(typeStruct->mutex);
        typeStruct->cptRemainingInterfaces++;
        node->sourceFile->module->decImplForToSolve(typeStruct);

        unique_lock lk2(node->structScope->parentScope->mutex);
        unique_lock lk3(node->structScope->mutex);

        node->structScope->parentScope->removeChildNoLock(node->structScope);
        for (auto s : node->structScope->childScopes)
        {
            s->parentScope->removeChildNoLock(s);
            structDecl->scope->addChildNoLock(s);
        }
    }
    else
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
        node->sourceFile->module->decImplForToSolve(typeStruct);
    }

    return true;
}

bool SemanticJob::resolveImplForType(SemanticContext* context)
{
    auto node       = CastAst<AstImpl>(context->node, AstNodeKind::Impl);
    auto sourceFile = node->sourceFile;
    auto module     = sourceFile->module;
    auto back       = node->childs[1];
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(back->typeInfo, TypeInfoKind::Struct);

    if (node->identifierFor->typeInfo->flags & TYPEINFO_GENERIC)
        return true;

    // Make a concrete type for the given struct
    auto& typeTable = module->typeTable;
    SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, typeStruct, nullptr, &back->concreteTypeInfoStorage, CONCRETE_SHOULD_WAIT));
    if (context->result != ContextResult::Done)
        return true;

    auto typeBaseInterface = CastTypeInfo<TypeInfoStruct>(node->childs[0]->typeInfo, TypeInfoKind::Interface);
    auto typeParamItf      = typeStruct->hasInterface(typeBaseInterface);
    SWAG_ASSERT(typeParamItf);

    auto constSegment = getConstantSegFromContext(back);
    SWAG_ASSERT(typeParamItf->offset);
    auto itable = (void**) constSegment->address(typeParamItf->offset);

    // Move back to concrete type, and initialize it
    itable--;
    *itable = constSegment->address(back->concreteTypeInfoStorage);
    constSegment->addInitPtr(typeParamItf->offset - sizeof(void*), back->concreteTypeInfoStorage, constSegment->kind);

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
        Diagnostic diag{node->identifier, Utf8::format(Msg0646, node->identifier->token.text.c_str(), TypeInfo::getArticleKindName(typeInfo))};
        Diagnostic note{node->identifier->resolvedSymbolOverload->node, node->identifier->resolvedSymbolOverload->node->token, Utf8::format(Msg0018, node->identifier->token.text.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note);
    }

    // Be sure the second identifier is a struct
    typeInfo = node->identifierFor->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Struct)
    {
        Diagnostic diag{node->identifierFor, Utf8::format(Msg0648, node->identifierFor->token.text.c_str(), TypeInfo::getArticleKindName(typeInfo))};
        Diagnostic note{node->identifierFor->resolvedSymbolOverload->node, node->identifierFor->resolvedSymbolOverload->node->token, Utf8::format(Msg0018, node->identifier->token.text.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note);
    }

    VectorNative<AstNode*>& childs = job->tmpNodes;
    job->tmpNodes.clear();
    flattenStructChilds(context, node, job->tmpNodes);

    SWAG_ASSERT(node->childs[0]->kind == AstNodeKind::IdentifierRef);
    SWAG_ASSERT(node->childs[1]->kind == AstNodeKind::IdentifierRef);
    auto typeBaseInterface = CastTypeInfo<TypeInfoStruct>(node->childs[0]->typeInfo, TypeInfoKind::Interface);
    auto typeStruct        = CastTypeInfo<TypeInfoStruct>(node->childs[1]->typeInfo, TypeInfoKind::Struct);

    // Be sure interface has been fully solved
    {
        scoped_lock lk(node->identifier->mutex);
        scoped_lock lk1(node->identifier->resolvedSymbolName->mutex);
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
            typeParamItf = allocType<TypeInfoParam>();
            typeBaseInterface->computeScopedName();
            typeParamItf->namedParam = typeBaseInterface->scopedName;
            SWAG_ASSERT(!typeParamItf->namedParam.empty());
            typeParamItf->typeInfo = typeBaseInterface;
            typeParamItf->declNode = typeBaseInterface->declNode;
            typeParamItf->declNode = node;
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
            auto symbolName = node->scope->symTable.find(child->token.text);
            SWAG_ASSERT(symbolName);
            scoped_lock lk(symbolName->mutex);
            if (symbolName->cptOverloads)
            {
                job->waitForSymbolNoLock(symbolName);
                return true;
            }
        }

        if (typeInfo->flags & TYPEINFO_GENERIC)
            continue;

        // We need to be have a bytecode pointer to be able to reference it in the itable
        if (!(child->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            ByteCodeGenJob::askForByteCode(context->job, child, ASKBC_WAIT_SEMANTIC_RESOLVED);
            if (context->result == ContextResult::Pending)
                return true;
        }

        // We need to search the function (as a variable) in the interface
        auto symbolName = typeInterface->findChildByNameNoLock(child->token.text); // O(n) !
        if (!symbolName)
        {
            Diagnostic diag{child, child->token, Utf8::format(Msg0650, child->token.text.c_str(), typeBaseInterface->name.c_str())};
            Diagnostic note{typeBaseInterface->declNode, typeBaseInterface->declNode->token, Utf8::format(Msg0651, typeBaseInterface->name.c_str()), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        // Match function signature
        auto typeLambda = CastTypeInfo<TypeInfoFuncAttr>(symbolName->typeInfo, TypeInfoKind::Lambda);
        auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeLambda->isSame(typeFunc, ISSAME_EXACT | ISSAME_INTERFACE))
        {
            Diagnostic diag{child, child->token, Utf8::format(Msg0652, child->token.text.c_str(), typeBaseInterface->name.c_str())};
            Diagnostic note{symbolName->declNode, symbolName->declNode->token, Msg0653, DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        // First parameter in the impl block must be a pointer to the struct
        SWAG_VERIFY(typeFunc->parameters.size(), context->report({child, child->token, Utf8::format(Msg0654, child->token.text.c_str())}));
        auto firstParamType = typeFunc->parameters[0]->typeInfo;
        SWAG_VERIFY(firstParamType->kind == TypeInfoKind::Pointer, context->report({typeFunc->parameters[0]->declNode, Utf8::format(Msg0655, firstParamType->getDisplayName().c_str())}));
        auto firstParamPtr = CastTypeInfo<TypeInfoPointer>(firstParamType, TypeInfoKind::Pointer);
        SWAG_VERIFY(firstParamPtr->pointedType == typeStruct, context->report({typeFunc->parameters[0]->declNode, Utf8::format(Msg0655, firstParamType->getDisplayName().c_str())}));

        // use resolvedUserOpSymbolOverload to store the match
        mapItToFunc[symbolName]           = child;
        mapItIdxToFunc[symbolName->index] = (AstFuncDecl*) child;
    }

    // If structure is generic, then do nothing, we cannot solve
    if (typeInfo->flags & TYPEINFO_GENERIC)
    {
        decreaseInterfaceCount(typeStruct);
        return true;
    }

    // Be sure every functions of the interface has been covered
    Diagnostic                diag{node, node->token, Utf8::format(Msg0657, typeBaseInterface->name.c_str())};
    vector<const Diagnostic*> notes;
    for (uint32_t idx = 0; idx < numFctInterface; idx++)
    {
        if (mapItIdxToFunc[idx] == nullptr)
        {
            auto missingNode = typeInterface->fields[idx];
            notes.push_back(new Diagnostic({missingNode->declNode, missingNode->declNode->token, Utf8::format("missing '%s'", missingNode->namedParam.c_str()), DiagnosticLevel::Note}));
        }
    }

    if (!notes.empty())
        return context->report(diag, notes);

    // Construct itable in the constant segment
    auto     constSegment = getConstantSegFromContext(node);
    uint32_t itableOffset = constSegment->reserve((numFctInterface + 1) * sizeof(void*), sizeof(void*));
    void**   ptrITable    = (void**) constSegment->address(itableOffset);
    auto     offset       = itableOffset;

    // The first value will be the concrete type to the corresponding struct, filled in resolveImplForType
    *ptrITable++ = nullptr;
    offset += sizeof(void*);

    for (uint32_t i = 0; i < numFctInterface; i++)
    {
        auto funcChild = mapItIdxToFunc[i];
        if (funcChild->attributeFlags & ATTRIBUTE_FOREIGN)
        {
            funcChild->computeFullNameForeign(true);

            // Need to patch the segment at runtime, init, with the real address of the function
            constSegment->addInitPtrFunc(offset, funcChild->fullnameForeign, DataSegment::RelocType::Foreign);

            // This will be filled when the module will be loaded, with the real function address
            *ptrITable = nullptr;
            g_ModuleMgr.addPatchFuncAddress((void**) constSegment->address(offset), funcChild);
        }
        else
        {
            *ptrITable = ByteCode::doByteCodeLambda(funcChild->extension->bc);
            constSegment->addInitPtrFunc(offset, funcChild->extension->bc->callName(), DataSegment::RelocType::Local);
        }

        ptrITable++;
        offset += sizeof(void*);
    }

    // :ItfIsConstantSeg
    // Setup constant segment offset. We put the offset to the start of the functions, not to the concrete type offset (0)
    typeParamItf->offset = itableOffset + sizeof(void*);
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
        typeInfoStruct->scope->dependentJobs.setRunning();
}

void SemanticJob::decreaseMethodCount(TypeInfoStruct* typeInfoStruct)
{
    unique_lock lk(typeInfoStruct->mutex);
    unique_lock lk1(typeInfoStruct->scope->symTable.mutex);

    SWAG_ASSERT(typeInfoStruct->cptRemainingMethods);
    typeInfoStruct->cptRemainingMethods--;
    if (!typeInfoStruct->cptRemainingMethods)
        typeInfoStruct->scope->dependentJobs.setRunning();
}

bool SemanticJob::CheckImplScopes(SemanticContext* context, AstImpl* node, Scope* scopeImpl, Scope* scope)
{
    // impl scope and corresponding identifier scope must be the same !
    if (scopeImpl != scope)
    {
        Diagnostic note{node->identifier->resolvedSymbolOverload->node, node->identifier->resolvedSymbolOverload->node->token, Utf8::format(Msg0018, node->identifier->token.text.c_str()), DiagnosticLevel::Note};
        if ((scopeImpl->flags & SCOPE_PRIVATE) && !(scope->flags & SCOPE_PRIVATE))
        {
            Diagnostic diag{node->identifier, Utf8::format(Msg0659, node->identifier->token.text.c_str())};
            return context->report(diag, &note);
        }

        if ((scope->flags & SCOPE_PRIVATE) && !(scopeImpl->flags & SCOPE_PRIVATE))
        {
            Diagnostic diag{node->identifier, Utf8::format(Msg0660, node->identifier->token.text.c_str())};
            return context->report(diag, &note);
        }

        Diagnostic diag{node,
                        node->token,
                        Utf8::format(Msg0661,
                                     node->token.text.c_str(),
                                     scopeImpl->parentScope->getFullName().c_str(),
                                     node->token.text.c_str(),
                                     scope->parentScope->getFullName().c_str())};
        return context->report(diag, &note);
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
        Diagnostic diag{node->identifier, Utf8::format(Msg0662, node->identifier->token.text.c_str(), TypeInfo::getArticleKindName(typeInfo))};
        Diagnostic note{node->identifier->resolvedSymbolOverload->node, node->identifier->resolvedSymbolOverload->node->token, Utf8::format(Msg0018, node->identifier->token.text.c_str()), DiagnosticLevel::Note};
        return context->report(diag, &note);
    }

    auto typeIdentifier = node->identifier->resolvedSymbolOverload->typeInfo;
    SWAG_VERIFY(typeIdentifier->kind != TypeInfoKind::Alias, context->report({node->identifier, Msg0664}));

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Struct:
    {
        auto structNode = CastAst<AstStruct>(node->identifier->resolvedSymbolOverload->node, AstNodeKind::StructDecl);
        SWAG_CHECK(CheckImplScopes(context, node, node->structScope, structNode->scope));
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto enumNode = CastAst<AstEnum>(node->identifier->resolvedSymbolOverload->node, AstNodeKind::EnumDecl);
        SWAG_CHECK(CheckImplScopes(context, node, node->structScope, enumNode->scope));
        break;
    }
    default:
        SWAG_ASSERT(false);
        break;
    }

    return true;
}

bool SemanticJob::preResolveGeneratedStruct(SemanticContext* context)
{
    auto structNode = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto parent     = structNode->originalParent;
    if (!parent)
        return true;
    if (structNode->genericParameters)
        return true;

    // We convert the {...} expression to a structure. As the structure can contain generic parameters,
    // we need to copy them. But from the function or the structure ?
    // For now, we give the priority to the generic parameters from the function, if there are any
    // But this will not work in all cases
    if (parent->ownerFct)
    {
        auto parentFunc = CastAst<AstFuncDecl>(parent->ownerFct, AstNodeKind::FuncDecl);
        if (parentFunc->genericParameters)
            structNode->genericParameters = Ast::clone(parentFunc->genericParameters, nullptr, AST_GENERATED_GENERIC_PARAM);
    }

    if (parent->ownerStructScope && !structNode->genericParameters)
    {
        auto parentStruct = (AstStruct*) parent->ownerStructScope->owner;
        if (parentStruct->genericParameters)
            structNode->genericParameters = Ast::clone(parentStruct->genericParameters, nullptr, AST_GENERATED_GENERIC_PARAM);
    }

    if (structNode->genericParameters)
    {
        Ast::addChildFront(structNode, structNode->genericParameters);

        Ast::visit(structNode->genericParameters, [&](AstNode* n) {
            n->inheritOwners(structNode);
            n->ownerStructScope = structNode->scope;
            n->ownerScope       = structNode->scope;
            n->flags |= AST_IS_GENERIC;
        });
    }

    return true;
}

bool SemanticJob::preResolveStructContent(SemanticContext* context)
{
    auto node = (AstStruct*) context->node->parent;
    SWAG_ASSERT(node->kind == AstNodeKind::StructDecl || node->kind == AstNodeKind::InterfaceDecl);

    auto typeInfo = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    SWAG_CHECK(collectAttributes(context, node, &typeInfo->attributes));

    if (node->attributeFlags & ATTRIBUTE_NO_COPY)
        typeInfo->flags |= TYPEINFO_STRUCT_NO_COPY;

    // Be sure we have only one struct node
    if (node->resolvedSymbolName && node->resolvedSymbolName->nodes.size() > 1)
    {
        Diagnostic  diag({node, node->token, Msg0696});
        Diagnostic* note = nullptr;
        for (auto p : node->resolvedSymbolName->nodes)
        {
            if (p != node)
            {
                note = new Diagnostic{p, p->token, Msg0884, DiagnosticLevel::Note};
                break;
            }
        }

        return context->report(diag, note);
    }

    typeInfo->declNode = node;
    node->scope->owner = node;

    // Add generic parameters
    uint32_t symbolFlags = 0;
    if (!(node->flags & AST_FROM_GENERIC))
    {
        if (node->genericParameters)
        {
            for (auto param : node->genericParameters->childs)
            {
                auto funcParam        = allocType<TypeInfoParam>();
                funcParam->namedParam = param->token.text;
                funcParam->name       = param->typeInfo->name;
                funcParam->typeInfo   = param->typeInfo;
                funcParam->sizeOf     = param->typeInfo->sizeOf;
                typeInfo->genericParameters.push_back(funcParam);
                typeInfo->sizeOf += param->typeInfo->sizeOf;
            }

            node->flags |= AST_IS_GENERIC;
            typeInfo->flags |= TYPEINFO_GENERIC;
            symbolFlags |= OVERLOAD_GENERIC;
        }

        typeInfo->forceComputeName();
    }

    // Attributes
    if (!(node->flags & AST_FROM_GENERIC))
        SWAG_CHECK(collectAttributes(context, node, &typeInfo->attributes));

    // Register symbol with its type
    SymbolKind symbolKind = SymbolKind::Struct;
    switch (node->kind)
    {
    case AstNodeKind::StructDecl:
        symbolKind = SymbolKind::Struct;
        typeInfo->flags |= TYPEINFO_RETURN_BY_COPY;
        break;
    case AstNodeKind::InterfaceDecl:
        symbolKind     = SymbolKind::Interface;
        typeInfo->kind = TypeInfoKind::Interface;
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, symbolKind, nullptr, symbolFlags | OVERLOAD_INCOMPLETE | OVERLOAD_STORE_SYMBOLS, nullptr, 0));
    return true;
}

void SemanticJob::flattenStructChilds(SemanticContext* context, AstNode* parent, VectorNative<AstNode*>& result)
{
    for (auto child : parent->childs)
    {
        switch (child->kind)
        {
        case AstNodeKind::Statement:
        case AstNodeKind::CompilerIfBlock:
        case AstNodeKind::CompilerAst:
        case AstNodeKind::CompilerAssert:
            flattenStructChilds(context, child, result);
            continue;

        case AstNodeKind::CompilerIf:
        {
            AstIf* compilerIf = CastAst<AstIf>(child, AstNodeKind::CompilerIf);
            if (!(compilerIf->ifBlock->flags & AST_NO_SEMANTIC))
                flattenStructChilds(context, compilerIf->ifBlock, result);
            else if (compilerIf->elseBlock)
                flattenStructChilds(context, compilerIf->elseBlock, result);
            continue;
        }

        case AstNodeKind::AttrUse:
        {
            AstAttrUse* attrUse = CastAst<AstAttrUse>(child, AstNodeKind::AttrUse);
            if (attrUse->content->kind == AstNodeKind::Statement)
                flattenStructChilds(context, attrUse->content, result);
            else
                result.push_back(attrUse->content);
            continue;
        }
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

    // Structure packing
    ComputedValue value;
    if (node->structFlags & STRUCTFLAG_UNION)
        node->packing = 0;
    else if (typeInfo->attributes.getValue("Swag.Pack", "value", value))
        node->packing = value.reg.u8;

    // Check 'opaque' attribute
    if (!sourceFile->isGenerated)
    {
        if (node->attributeFlags & ATTRIBUTE_OPAQUE)
        {
            SWAG_VERIFY(node->attributeFlags & ATTRIBUTE_PUBLIC, context->report({node, node->token, Msg0666}));
            SWAG_VERIFY(!sourceFile->forceExport, context->report({node, node->token, Msg0667}));
        }
    }

    uint32_t storageOffset     = 0;
    uint32_t storageIndexField = 0;
    uint32_t storageIndexConst = 0;
    uint32_t structFlags       = TYPEINFO_STRUCT_ALL_UNINITIALIZED;

    // No need to flatten structure if it's not a compound (optim)
    VectorNative<AstNode*>& childs = (node->flags & AST_STRUCT_COMPOUND) ? job->tmpNodes : node->content->childs;
    if (node->flags & AST_STRUCT_COMPOUND)
    {
        job->tmpNodes.clear();
        flattenStructChilds(context, node->content, job->tmpNodes);
    }

    typeInfo->alignOf = 0;
    typeInfo->sizeOf  = 0;

    // If one of my childs is incomplete, then we must wait
    for (int i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];
        job->waitTypeCompleted(child->typeInfo);
        if (context->result != ContextResult::Done)
            return true;
    }

    for (int i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];
        if (child->kind != AstNodeKind::VarDecl && child->kind != AstNodeKind::ConstDecl)
            continue;

        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

        // Using can only be used on a structure
        if (child->flags & AST_DECL_USING && child->kind == AstNodeKind::ConstDecl)
            return context->report({child, Msg0668});
        if (child->flags & AST_DECL_USING && child->typeInfo->kind != TypeInfoKind::Struct && !child->typeInfo->isPointerTo(TypeInfoKind::Struct))
            return context->report({child, Utf8::format(Msg0669, child->typeInfo->getDisplayName().c_str())});

        TypeInfoParam* typeParam = nullptr;
        if (!(node->flags & AST_FROM_GENERIC) || !(child->flags & AST_STRUCT_REGISTERED))
        {
            typeParam             = allocType<TypeInfoParam>();
            typeParam->namedParam = child->token.text;
            typeParam->name       = child->typeInfo->name;
            typeParam->typeInfo   = child->typeInfo;
            typeParam->sizeOf     = child->typeInfo->sizeOf;
            typeParam->offset     = storageOffset;
            if (varDecl->flags & AST_DECL_USING)
                typeParam->flags |= TYPEINFO_HAS_USING;
            SWAG_CHECK(collectAttributes(context, child, &typeParam->attributes));
            if (child->kind == AstNodeKind::VarDecl)
                typeInfo->fields.push_back(typeParam);
            else
                typeInfo->consts.push_back(typeParam);
        }

        child->flags |= AST_STRUCT_REGISTERED;
        if (child->kind == AstNodeKind::VarDecl)
            typeParam = typeInfo->fields[storageIndexField];
        else
            typeParam = typeInfo->consts[storageIndexConst];
        typeParam->typeInfo = child->typeInfo;
        typeParam->declNode = child;

        if (child->kind != AstNodeKind::VarDecl)
        {
            storageIndexConst++;
            continue;
        }

        typeParam->index = storageIndexField;

        // Inherit flags
        if (!(varDecl->flags & AST_EXPLICITLY_NOT_INITIALIZED))
            structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
        if (varDecl->typeInfo->flags & TYPEINFO_STRUCT_NO_COPY)
            structFlags |= TYPEINFO_STRUCT_NO_COPY;

        // Var is a struct
        if (varDecl->typeInfo->kind == TypeInfoKind::Struct)
        {
            structFlags |= varDecl->typeInfo->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES;

            if (!(varDecl->typeInfo->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
                structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;

            if (varDecl->type && varDecl->type->flags & AST_HAS_STRUCT_PARAMETERS)
            {
                structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                if (!(varDecl->type->flags & AST_VALUE_COMPUTED))
                {
                    varDecl->type->flags |= AST_VALUE_COMPUTED;
                    auto constSegment                           = getConstantSegFromContext(varDecl);
                    varDecl->type->computedValue.storageSegment = constSegment;
                    SWAG_CHECK(collectAssignment(context, varDecl->type->computedValue.storageOffset, varDecl, constSegment));
                }
            }

            // :opAffectConstExpr
            // Collect has already been simulated with an opAffect
            else if (varDecl->assignment)
            {
                SWAG_ASSERT(varDecl->semFlags & AST_SEM_EXEC_RET_STACK);
                structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
            }
        }

        // Var is an array of structs
        else if (varDecl->typeInfo->kind == TypeInfoKind::Array && !varDecl->assignment)
        {
            auto varTypeArray = CastTypeInfo<TypeInfoArray>(varDecl->typeInfo, TypeInfoKind::Array);
            if (varTypeArray->pointedType->kind == TypeInfoKind::Struct)
            {
                structFlags |= varTypeArray->pointedType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES;
                if (!(varTypeArray->pointedType->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
                    structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
            }
        }

        // Var has an initialization
        else if (varDecl->assignment && !(varDecl->flags & AST_EXPLICITLY_NOT_INITIALIZED))
        {
            SWAG_VERIFY(varDecl->assignment->flags & AST_CONST_EXPR, context->report({varDecl->assignment, Msg0670}));

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

        // If the struct is not generic, be sure that a field is not generic either
        if (!(node->flags & AST_IS_GENERIC))
        {
            if (varDecl->typeInfo->flags & TYPEINFO_GENERIC)
            {
                if (varDecl->type)
                    child = varDecl->type;
                if (!node->genericParameters)
                    return context->report({child, Utf8::format(Msg0671, child->typeInfo->getDisplayName().c_str(), node->token.text.c_str())});
                return context->report({child, Utf8::format(Msg0672, node->token.text.c_str(), child->typeInfo->getDisplayName().c_str())});
            }
        }

        auto realStorageOffset = storageOffset;

        // Attribute 'Swag.offset' can be used to force the storage offset of the member
        ComputedValue forceOffset;
        bool          relocated = false;
        if (typeParam && typeParam->attributes.getValue("Swag.Offset", "name", forceOffset))
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
                auto attr = typeParam->attributes.getAttribute("Swag.Offset");
                SWAG_ASSERT(attr);
                return context->report({attr->node, Utf8::format(Msg0673, forceOffset.text.c_str())});
            }
        }

        // Compute struct alignement
        auto alignOf      = TypeManager::alignOf(child->typeInfo);
        typeInfo->alignOf = max(typeInfo->alignOf, alignOf);

        // Compute padding before the current field
        if (!relocated && node->packing > 1 && alignOf)
        {
            alignOf           = min(alignOf, node->packing);
            realStorageOffset = (uint32_t) TypeManager::align(realStorageOffset, alignOf);
            storageOffset     = (uint32_t) TypeManager::align(storageOffset, alignOf);
        }

        typeParam->offset                                          = realStorageOffset;
        child->resolvedSymbolOverload->computedValue.storageOffset = realStorageOffset;
        child->resolvedSymbolOverload->storageIndex                = storageIndexField;
        child->resolvedSymbolOverload->attributeFlags              = child->attributeFlags;

        typeInfo->sizeOf = max(typeInfo->sizeOf, (int) realStorageOffset + child->typeInfo->sizeOf);

        if (relocated)
            storageOffset = max(storageOffset, realStorageOffset + child->typeInfo->sizeOf);
        else if (node->packing)
            storageOffset += child->typeInfo->sizeOf;

        // Create a generic alias
        if (!(child->flags & AST_AUTO_NAME))
        {
            bool hasItemName = false;
            if (child->token.text.length() > 4 && child->token.text[0] == 'i' && child->token.text[1] == 't' && child->token.text[2] == 'e' && child->token.text[3] == 'm')
                hasItemName = true;

            // User cannot name its variables itemX
            if (!(node->flags & AST_GENERATED) && hasItemName)
            {
                return context->report({child, Utf8::format(Msg0674, child->token.text.c_str())});
            }

            if (!hasItemName)
            {
                auto  overload = child->resolvedSymbolOverload;
                Utf8  name     = Utf8::format("item%u", storageIndexField);
                auto& symTable = node->scope->symTable;
                symTable.addSymbolTypeInfo(context,
                                           child,
                                           child->typeInfo,
                                           SymbolKind::Variable,
                                           nullptr,
                                           overload->flags,
                                           nullptr,
                                           overload->computedValue.storageOffset,
                                           overload->computedValue.storageSegment,
                                           &name);
            }
        }
        else
        {
            typeParam->flags |= TYPEINFO_AUTO_NAME;
        }

        storageIndexField++;
    }

    // A struct cannot have a zero size
    if (!typeInfo->sizeOf)
    {
        typeInfo->sizeOf  = 1;
        typeInfo->alignOf = 1;
    }

    // User specific alignment
    ComputedValue userAlignOf;
    auto          hasUserAlignOf = typeInfo->attributes.getValue("Swag.Align", "value", userAlignOf);
    if (hasUserAlignOf)
        typeInfo->alignOf = userAlignOf.reg.u8;
    else if (node->packing)
        typeInfo->alignOf = min(typeInfo->alignOf, node->packing);
    typeInfo->alignOf = max(1, typeInfo->alignOf);

    // An opaque struct will be exported as an array of bytes.
    // We need to be sure that alignement will be respected, so we force "Swag.Align" attribute
    // if not already present.
    if (!hasUserAlignOf && typeInfo->attributes.hasAttribute("Swag.Opaque"))
    {
        OneAttribute       ot;
        AttributeParameter otp;
        ot.name           = "Swag.Align";
        otp.name          = "value";
        otp.typeInfo      = g_TypeMgr.typeInfoU8;
        otp.value.reg.u64 = typeInfo->alignOf;
        ot.parameters.push_back(otp);
        typeInfo->attributes.attributes.push_back(ot);
    }

    // Align structure size
    if (typeInfo->alignOf > 1 && !(typeInfo->flags & TYPEINFO_GENERIC))
        typeInfo->sizeOf = (uint32_t) TypeManager::align(typeInfo->sizeOf, typeInfo->alignOf);

    // Check public
    if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE))
    {
        if (!node->ownerScope->isGlobalOrImpl())
            return context->report({node, node->token, Utf8::format(Msg0675, node->token.text.c_str())});
        if (!(node->flags & AST_FROM_GENERIC))
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
    typeInfo->flags |= (structFlags & TYPEINFO_STRUCT_NO_COPY);

    // Register symbol with its type
    node->typeInfo               = typeInfo;
    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::Struct);
    SWAG_CHECK(node->resolvedSymbolOverload);

    // We are parsing the swag module
    if (sourceFile->isBootstrapFile)
        g_Workspace.swagScope.registerType(node->typeInfo);

    // Generate all functions associated with a struct
    if (!(typeInfo->flags & TYPEINFO_GENERIC))
    {
        node->flags &= ~AST_NO_BYTECODE;
        node->flags |= AST_NO_BYTECODE_CHILDS;

        SWAG_ASSERT(!node->extension || !node->extension->byteCodeJob);
        node->allocateExtension();
        auto extension                       = node->extension;
        extension->byteCodeJob               = g_Allocator.alloc<ByteCodeGenJob>();
        extension->byteCodeJob->sourceFile   = sourceFile;
        extension->byteCodeJob->module       = sourceFile->module;
        extension->byteCodeJob->dependentJob = context->job->dependentJob;
        extension->byteCodeJob->nodes.push_back(node);
        node->byteCodeFct = ByteCodeGenJob::emitStruct;
        g_ThreadMgr.addJob(extension->byteCodeJob);
    }

    return true;
}

bool SemanticJob::resolveInterface(SemanticContext* context)
{
    auto node          = CastAst<AstStruct>(context->node, AstNodeKind::InterfaceDecl);
    auto sourceFile    = context->sourceFile;
    auto typeInterface = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Interface);
    auto job           = context->job;

    typeInterface->declNode   = node;
    typeInterface->name       = node->token.text;
    typeInterface->structName = node->token.text;

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;

    VectorNative<AstNode*>& childs = (node->flags & AST_STRUCT_COMPOUND) ? job->tmpNodes : node->content->childs;
    if (node->flags & AST_STRUCT_COMPOUND)
    {
        job->tmpNodes.clear();
        flattenStructChilds(context, node->content, job->tmpNodes);
    }

    // itable
    auto typeITable        = allocType<TypeInfoStruct>();
    typeITable->name       = node->token.text;
    typeITable->structName = node->token.text;
    typeITable->scope      = Ast::newScope(node, node->token.text, ScopeKind::Struct, nullptr);

    for (int i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];
        if (child->kind != AstNodeKind::VarDecl)
            continue;

        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);

        TypeInfoParam* typeParam = nullptr;
        if (!(node->flags & AST_FROM_GENERIC))
        {
            typeParam             = allocType<TypeInfoParam>();
            typeParam->namedParam = child->token.text;
            typeParam->name       = child->typeInfo->name;
            typeParam->sizeOf     = child->typeInfo->sizeOf;
            typeParam->offset     = storageOffset;
            typeParam->declNode   = child;
            SWAG_CHECK(collectAttributes(context, child, &typeParam->attributes));
            typeITable->fields.push_back(typeParam);

            // Verify signature
            typeParam->typeInfo = TypeManager::concreteType(child->typeInfo, CONCRETE_ALIAS);
            SWAG_VERIFY(typeParam->typeInfo->kind == TypeInfoKind::Lambda, context->report({child, Utf8::format(Msg0676, child->typeInfo->getDisplayName().c_str())}));
            auto typeLambda = CastTypeInfo<TypeInfoFuncAttr>(typeParam->typeInfo, TypeInfoKind::Lambda);
            SWAG_VERIFY(typeLambda->parameters.size() >= 1, context->report({child, Utf8::format(Msg0677, child->token.text.c_str())}));
            auto firstParamType = typeLambda->parameters[0]->typeInfo;
            SWAG_VERIFY(firstParamType->kind == TypeInfoKind::Pointer, context->report({typeLambda->parameters[0]->declNode, Utf8::format(Msg0679, firstParamType->getDisplayName().c_str())}));
            auto firstParamPtr = CastTypeInfo<TypeInfoPointer>(firstParamType, TypeInfoKind::Pointer);
            SWAG_VERIFY(firstParamPtr->pointedType == typeInterface, context->report({typeLambda->parameters[0]->declNode, Utf8::format(Msg0679, firstParamType->getDisplayName().c_str())}));
        }

        typeParam           = typeITable->fields[storageIndex];
        typeParam->typeInfo = child->typeInfo;
        typeParam->declNode = child;
        typeParam->index    = storageIndex;

        SWAG_VERIFY(!varDecl->assignment, context->report({varDecl->assignment, Msg0680}));

        if (!(node->flags & AST_IS_GENERIC))
        {
            SWAG_VERIFY(!(child->typeInfo->flags & TYPEINFO_GENERIC), context->report({child, Utf8::format(Msg0681, child->typeInfo->getDisplayName().c_str())}));
        }

        if (typeParam->attributes.hasAttribute("Swag.Offset"))
        {
            auto attr = typeParam->attributes.getAttribute("Swag.Offset");
            SWAG_ASSERT(attr);
            return context->report({attr->node, Msg0682});
        }

        typeParam->offset                                          = storageOffset;
        child->resolvedSymbolOverload->computedValue.storageOffset = storageOffset;
        child->resolvedSymbolOverload->storageIndex                = storageIndex;

        SWAG_ASSERT(child->typeInfo->sizeOf == sizeof(void*));
        typeITable->sizeOf += sizeof(void*);

        // Register lambda variable in the scope of the itable struct
        auto  overload = child->resolvedSymbolOverload;
        auto& symTable = typeITable->scope->symTable;
        symTable.addSymbolTypeInfo(context, child, child->typeInfo, SymbolKind::Variable, nullptr, overload->flags, nullptr, overload->computedValue.storageOffset);

        storageOffset += sizeof(void*);
        storageIndex++;
    }

    SWAG_VERIFY(!typeITable->fields.empty(), context->report({node, node->token, Utf8::format(Msg0683, node->token.text.c_str())}));
    typeInterface->itable = typeITable;

    // Struct interface, with one pointer for the data, and one pointer for itable
    if (!(node->flags & AST_FROM_GENERIC))
    {
        auto typeParam      = allocType<TypeInfoParam>();
        typeParam->typeInfo = g_TypeMgr.typeInfoPVoid;
        typeParam->name     = typeParam->typeInfo->name;
        typeParam->sizeOf   = typeParam->typeInfo->sizeOf;
        typeParam->offset   = 0;
        typeInterface->fields.push_back(typeParam);
        typeInterface->sizeOf += sizeof(void*);

        typeParam           = allocType<TypeInfoParam>();
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
            return context->report({node, node->token, Utf8::format(Msg0684, node->token.text.c_str())});

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