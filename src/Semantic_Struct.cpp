#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "ByteCodeGenJob.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "ModuleManager.h"
#include "Naming.h"
#include "Parser.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "ThreadManager.h"
#include "TypeManager.h"
#include "Workspace.h"

bool Semantic::resolveImplForAfterFor(SemanticContext* context)
{
    const auto id   = context->node;
    const auto node = castAst<AstImpl>(context->node->parent, AstNodeKind::Impl);

    if (id->resolvedSymbolName->kind != SymbolKind::Struct)
        return context->report({id->childs.back(), FMT(Err(Err0160), id->resolvedSymbolName->name.c_str(), Naming::aKindName(id->resolvedSymbolName->kind).c_str())});

    const auto structDecl = castAst<AstStruct>(id->resolvedSymbolOverload->node, AstNodeKind::StructDecl);

    if (id->resolvedSymbolOverload->flags & OVERLOAD_GENERIC)
    {
        if (!(node->hasAstFlag(AST_FROM_GENERIC)))
        {
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
            node->sourceFile->module->decImplForToSolve(typeStruct);
        }

        return true;
    }

    if (structDecl->scope != node->structScope)
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
        ScopedLock lk1(typeStruct->mutex);
        typeStruct->cptRemainingInterfacesReg++;
        typeStruct->cptRemainingInterfaces++;
        node->sourceFile->module->decImplForToSolve(typeStruct);

        ScopedLock lk2(node->structScope->parentScope->mutex);
        ScopedLock lk3(node->structScope->mutex);

        node->structScope->parentScope->removeChildNoLock(node->structScope);
        for (const auto s : node->structScope->childScopes)
        {
            s->parentScope->removeChildNoLock(s);
            structDecl->scope->addChildNoLock(s);
        }
    }
    else
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
        node->sourceFile->module->decImplForToSolve(typeStruct);
    }

    return true;
}

bool Semantic::resolveImplForType(SemanticContext* context)
{
    const auto node       = castAst<AstImpl>(context->node, AstNodeKind::Impl);
    const auto sourceFile = node->sourceFile;
    const auto module     = sourceFile->module;
    AstNode*   first;
    AstNode*   back;

    // Race condition in case a templated function is instantiated in the impl block during that access
    {
        SharedLock lk(node->mutex);
        first = node->childs[0];
        back  = node->childs[1];
    }

    const auto typeStruct = castTypeInfo<TypeInfoStruct>(back->typeInfo, TypeInfoKind::Struct);

    if (node->identifierFor->typeInfo->isGeneric())
        return true;

    // Make a concrete type for the given struct
    auto& typeGen = module->typeGen;
    back->allocateComputedValue();
    back->computedValue->storageSegment = getConstantSegFromContext(back);
    SWAG_CHECK(typeGen.genExportedTypeInfo(context, typeStruct, back->computedValue->storageSegment, &back->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT));
    YIELD();

    // Make a concrete type for the given interface
    first->allocateComputedValue();
    first->computedValue->storageSegment = getConstantSegFromContext(first);
    SWAG_CHECK(typeGen.genExportedTypeInfo(context, first->typeInfo, first->computedValue->storageSegment, &first->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT));
    YIELD();

    const auto typeBaseInterface = castTypeInfo<TypeInfoStruct>(first->typeInfo, TypeInfoKind::Interface);
    const auto typeParamItf      = typeStruct->hasInterface(typeBaseInterface);
    SWAG_ASSERT(typeParamItf);

    const auto constSegment = back->computedValue->storageSegment;
    SWAG_ASSERT(typeParamItf->offset);
    auto itable = (void**) constSegment->address(typeParamItf->offset);

    // :itableHeader
    // Move back to concrete type, and initialize it
    itable--;
    *itable = constSegment->address(back->computedValue->storageOffset);
    constSegment->addInitPtr(typeParamItf->offset - sizeof(void*), back->computedValue->storageOffset, constSegment->kind);

    // Move back to interface type, and initialize it
    itable--;
    *itable = constSegment->address(first->computedValue->storageOffset);
    constSegment->addInitPtr(typeParamItf->offset - 2 * sizeof(void*), first->computedValue->storageOffset, constSegment->kind);

    return true;
}

bool Semantic::resolveImplFor(SemanticContext* context)
{
    const auto node = castAst<AstImpl>(context->node, AstNodeKind::Impl);
    const auto job  = context->baseJob;

    // Be sure the first identifier is an interface
    auto typeInfo = node->identifier->typeInfo;
    if (!typeInfo->isInterface())
    {
        const Diagnostic diag{node->identifier, FMT(Err(Err0162), node->identifier->token.ctext(), Naming::aKindName(typeInfo).c_str())};
        return context->report(diag, Diagnostic::hereIs(node->identifier->resolvedSymbolOverload));
    }

    // Be sure the second identifier is a struct
    typeInfo = node->identifierFor->typeInfo;
    SWAG_ASSERT(typeInfo->isStruct());

    VectorNative<AstNode*>& childs = context->tmpNodes;
    context->tmpNodes.clear();
    flattenStructChilds(context, node, context->tmpNodes);

    SWAG_ASSERT(node->childs[0]->kind == AstNodeKind::IdentifierRef);
    SWAG_ASSERT(node->childs[1]->kind == AstNodeKind::IdentifierRef);
    const auto typeBaseInterface = castTypeInfo<TypeInfoStruct>(node->childs[0]->typeInfo, TypeInfoKind::Interface);
    const auto typeStruct        = castTypeInfo<TypeInfoStruct>(node->childs[1]->typeInfo, TypeInfoKind::Struct);

    // Be sure interface has been fully solved
    {
        ScopedLock lk(node->identifier->mutex);
        ScopedLock lk1(node->identifier->resolvedSymbolName->mutex);
        if (node->identifier->resolvedSymbolName->cptOverloads)
        {
            waitSymbolNoLock(job, node->identifier->resolvedSymbolName);
            return true;
        }
    }

    // We need now the pointer to the itable
    const auto     typeInterface   = castTypeInfo<TypeInfoStruct>(typeBaseInterface->itable, TypeInfoKind::Struct);
    const uint32_t numFctInterface = (uint32_t) typeInterface->fields.size();

    Map<TypeInfoParam*, AstNode*> mapItToFunc;
    VectorNative<AstFuncDecl*>    mapItIdxToFunc;
    mapItIdxToFunc.set_size_clear(numFctInterface);

    // Register interface in the structure
    TypeInfoParam* typeParamItf = nullptr;
    bool           hasRegItf    = false;
    {
        ScopedLock lk(typeStruct->mutex);
        typeParamItf = typeStruct->hasInterfaceNoLock(typeBaseInterface);
        if (!typeParamItf)
        {
            typeParamItf = TypeManager::makeParam();
            typeBaseInterface->computeScopedName();
            typeParamItf->name = typeBaseInterface->scopedName;
            SWAG_ASSERT(!typeParamItf->name.empty());
            typeParamItf->typeInfo = typeBaseInterface;
            typeParamItf->declNode = typeBaseInterface->declNode;
            typeParamItf->declNode = node;
            typeStruct->interfaces.push_back(typeParamItf);
            hasRegItf = true;
        }
    }

    // Interface has been registered in typeStruct->interfaces, so notify
    if (hasRegItf)
        decreaseInterfaceRegCount(typeStruct);

    for (const auto child : childs)
    {
        if (child->kind != AstNodeKind::FuncDecl)
            continue;
        const auto childFct = castAst<AstFuncDecl>(child, AstNodeKind::FuncDecl);

        // We need to search the function (as a variable) in the interface
        auto itfSymbol = typeInterface->findChildByNameNoLock(child->token.text); // O(n) !

        // Must be an 'impl' function
        if (!childFct->hasSpecFlag(AstFuncDecl::SPECFLAG_IMPL))
        {
            // Cannot have the same name as a function of the interface
            if (itfSymbol)
            {
                Diagnostic diag{childFct, childFct->tokenName, FMT(Err(Err0535), childFct->token.text.c_str(), typeInterface->name.c_str())};
                diag.hint      = Nte(Nte0080);
                const auto hlp = Diagnostic::note(Nte(Nte0019));
                return context->report(diag, hlp);
            }

            continue;
        }

        if (!itfSymbol)
        {
            const Diagnostic diag{childFct, childFct->tokenName, FMT(Err(Err0720), childFct->token.text.c_str(), typeInterface->name.c_str())};
            const auto       note = Diagnostic::note(SemanticError::findClosestMatchesMsg(childFct->token.text, {{typeInterface->scope, 0}}, IdentifierSearchFor::Whatever));
            return context->report(diag, note);
        }

        // We need to be sure function semantic is done
        {
            const auto symbolName = node->scope->symTable.find(child->token.text);
            SWAG_ASSERT(symbolName);
            ScopedLock lk(symbolName->mutex);
            if (symbolName->cptOverloads)
            {
                waitSymbolNoLock(job, symbolName);
                return true;
            }
        }

        if (typeInfo->flags & TYPEINFO_GENERIC)
            continue;

        // We need to be have a bytecode pointer to be able to reference it in the itable
        if (!child->isForeign())
        {
            ScopedLock lk(child->mutex);
            if (!child->hasExtByteCode() || !child->extByteCode()->bc)
            {
                child->allocateExtensionNoLock(ExtensionKind::ByteCode);
                child->extByteCode()->bc             = Allocator::alloc<ByteCode>();
                child->extByteCode()->bc->node       = child;
                child->extByteCode()->bc->sourceFile = child->sourceFile;
            }

            child->extByteCode()->bc->forceEmit = true;
        }

        // Match function signature
        BadSignatureInfos bi;
        const auto        typeLambda = castTypeInfo<TypeInfoFuncAttr>(itfSymbol->typeInfo, TypeInfoKind::LambdaClosure);
        const auto        typeFunc   = castTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeLambda->isSame(typeFunc, CASTFLAG_EXACT | CASTFLAG_INTERFACE, bi))
        {
            switch (bi.matchResult)
            {
            case MatchResult::BadSignature:
            {
                const Diagnostic diag{childFct, childFct->getTokenName(), FMT(Err(Err0430), child->token.ctext(), typeBaseInterface->name.c_str())};
                const auto       note = Diagnostic::note(childFct->parameters->childs[bi.badSignatureNum2],
                                                         FMT(Nte(Nte0102), childFct->parameters->childs[bi.badSignatureNum2]->typeInfo->getDisplayNameC()));
                const auto note1 = Diagnostic::note(typeLambda->parameters[bi.badSignatureNum1]->declNode,
                                                    FMT(Nte(Nte0108), typeLambda->parameters[bi.badSignatureNum1]->typeInfo->getDisplayNameC()));
                return context->report(diag, note, note1);
            }

            case MatchResult::MissingReturnType:
            {
                Diagnostic diag{child, child->getTokenName(), FMT(Err(Err0430), child->token.ctext(), typeBaseInterface->name.c_str())};
                diag.hint       = Nte(Nte0018);
                const auto note = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, FMT(Nte(Nte0123), typeLambda->returnType->getDisplayNameC()));
                return context->report(diag, note);
            }

            case MatchResult::NoReturnType:
            {
                const Diagnostic diag{childFct->returnType, FMT(Err(Err0430), child->token.ctext(), typeBaseInterface->name.c_str())};
                const auto       note = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, Nte(Nte0124));
                return context->report(diag, note);
            }

            case MatchResult::MismatchReturnType:
            {
                Diagnostic diag{childFct->returnType, FMT(Err(Err0430), child->token.ctext(), typeBaseInterface->name.c_str())};
                diag.hint       = Diagnostic::isType(childFct->returnType->typeInfo);
                const auto note = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, FMT(Nte(Nte0123), typeLambda->returnType->getDisplayNameC()));
                return context->report(diag, note);
            }

            case MatchResult::MismatchThrow:
            {
                Diagnostic diag{child, child->getTokenName(), FMT(Err(Err0430), child->token.ctext(), typeBaseInterface->name.c_str())};
                diag.hint         = Nte(Nte0099);
                const auto note   = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, Nte(Nte0163));
                note->canBeMerged = false;
                return context->report(diag, note);
            }

            default:
            {
                const Diagnostic diag{child, child->getTokenName(), FMT(Err(Err0430), child->token.ctext(), typeBaseInterface->name.c_str())};
                const auto       note = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, Nte(Nte0163));
                note->canBeMerged     = false;
                return context->report(diag, note);
            }
            }
        }

        // use resolvedUserOpSymbolOverload to store the match
        mapItToFunc[itfSymbol]           = child;
        mapItIdxToFunc[itfSymbol->index] = (AstFuncDecl*) child;
    }

    // If structure is generic, then do nothing, we cannot solve
    if (typeInfo->isGeneric())
    {
        decreaseInterfaceCount(typeStruct);
        return true;
    }

    // Be sure every functions of the interface has been covered. If it's not the case,
    // we generate the missing parts.
    SWAG_CHECK(Ast::generateMissingInterfaceFct(context, mapItIdxToFunc, typeStruct, typeBaseInterface, typeInterface));
    YIELD();

    // Construct itable in the constant segment
    // Header has 2 pointers
    // :itableHeader
    const auto     constSegment = getConstantSegFromContext(node);
    void**         ptrITable;
    constexpr int  sizeOfHeader = 2 * sizeof(void*);
    const uint32_t itableOffset = constSegment->reserve((numFctInterface * sizeof(void*)) + sizeOfHeader, (uint8_t**) &ptrITable, sizeof(void*));
    auto           offset       = itableOffset;

    // :itableHeader
    // The first value will be the concrete type to the interface
    // The second value will be the concrete type to the corresponding struct
    *ptrITable++ = nullptr;
    offset += sizeof(void*);
    *ptrITable++ = nullptr;
    offset += sizeof(void*);

    for (uint32_t i = 0; i < numFctInterface; i++)
    {
        const auto funcChild = mapItIdxToFunc[i];
        if (funcChild->isForeign())
        {
            funcChild->computeFullNameForeignExport();
            constSegment->addInitPtrFunc(offset, funcChild->fullnameForeignExport);

            // This will be filled when the module will be loaded, with the real function address
            *ptrITable = nullptr;
            g_ModuleMgr->addPatchFuncAddress(constSegment, (void**) constSegment->address(offset), funcChild);
        }
        else
        {
            funcChild->extByteCode()->bc->isUsed  = true;
            funcChild->extByteCode()->bc->isInSeg = true;

            *ptrITable = ByteCode::doByteCodeLambda(funcChild->extByteCode()->bc);
            constSegment->addInitPtrFunc(offset, funcChild->getCallName());
        }

        ptrITable++;
        offset += sizeof(void*);
    }

    // :ItfIsConstantSeg
    // Setup constant segment offset. We put the offset to the start of the functions, not to the concrete type offset (0)
    typeParamItf->offset = itableOffset + sizeOfHeader;
    decreaseInterfaceCount(typeStruct);

    return true;
}

bool Semantic::resolveInterface(SemanticContext* context)
{
    auto       node          = castAst<AstStruct>(context->node, AstNodeKind::InterfaceDecl);
    const auto sourceFile    = context->sourceFile;
    const auto typeInterface = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Interface);

    typeInterface->declNode   = node;
    typeInterface->name       = node->token.text;
    typeInterface->structName = node->token.text;

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;

    VectorNative<AstNode*>& childs = (node->hasAstFlag(AST_STRUCT_COMPOUND)) ? context->tmpNodes : node->content->childs;
    if (node->hasAstFlag(AST_STRUCT_COMPOUND))
    {
        context->tmpNodes.clear();
        flattenStructChilds(context, node->content, context->tmpNodes);
    }

    // itable
    const auto typeITable  = makeType<TypeInfoStruct>();
    typeITable->name       = node->token.text;
    typeITable->structName = node->token.text;
    typeITable->scope      = Ast::newScope(node, node->token.text, ScopeKind::Struct, nullptr);
    typeITable->flags |= TYPEINFO_STRUCT_IS_ITABLE;

    for (auto child : childs)
    {
        if (child->kind != AstNodeKind::VarDecl)
            continue;

        const auto varDecl = castAst<AstVarDecl>(child, AstNodeKind::VarDecl);

        TypeInfoParam* typeParam = nullptr;
        if (!(node->hasAstFlag(AST_FROM_GENERIC)))
        {
            typeParam           = TypeManager::makeParam();
            typeParam->name     = child->token.text;
            typeParam->offset   = storageOffset;
            typeParam->declNode = child;
            SWAG_CHECK(collectAttributes(context, child, &typeParam->attributes));
            typeITable->fields.push_back(typeParam);

            // Verify signature
            typeParam->typeInfo   = child->typeInfo->getConcreteAlias();
            const auto typeLambda = castTypeInfo<TypeInfoFuncAttr>(typeParam->typeInfo, TypeInfoKind::LambdaClosure);

            if (typeLambda->parameters.empty())
            {
                const Diagnostic diag{varDecl->type, FMT(Err(Err0130), child->token.ctext())};
                return context->report(diag);
            }

            const auto firstParamType = typeLambda->parameters[0]->typeInfo;
            SWAG_VERIFY(firstParamType->isPointer(), context->report({typeLambda->parameters[0]->declNode, FMT(Err(Err0315), firstParamType->getDisplayNameC())}));
            const auto firstParamPtr = castTypeInfo<TypeInfoPointer>(firstParamType, TypeInfoKind::Pointer);
            SWAG_VERIFY(firstParamPtr->pointedType == typeInterface, context->report({typeLambda->parameters[0]->declNode, FMT(Err(Err0315), firstParamType->getDisplayNameC())}));
        }

        typeParam           = typeITable->fields[storageIndex];
        typeParam->typeInfo = child->typeInfo;
        typeParam->declNode = child;
        typeParam->index    = storageIndex;

        if (!(node->hasAstFlag(AST_IS_GENERIC)))
        {
            SWAG_VERIFY(!child->typeInfo->isGeneric(), context->report({child, FMT(Err(Err0731), node->token.ctext(), child->typeInfo->getDisplayNameC())}));
        }

        if (typeParam->attributes.hasAttribute(g_LangSpec->name_Swag_Offset))
        {
            const auto attr = typeParam->attributes.getAttribute(g_LangSpec->name_Swag_Offset);
            SWAG_ASSERT(attr);
            return context->report({attr->node, attr->node->token, Err(Err0484)});
        }

        typeParam->offset                                          = storageOffset;
        child->resolvedSymbolOverload->computedValue.storageOffset = storageOffset;
        child->resolvedSymbolOverload->storageIndex                = storageIndex;

        SWAG_ASSERT(child->typeInfo->sizeOf == sizeof(void*));
        typeITable->sizeOf += sizeof(void*);

        // Register lambda variable in the scope of the itable struct
        const auto        overload = child->resolvedSymbolOverload;
        auto&             symTable = typeITable->scope->symTable;
        AddSymbolTypeInfo toAdd;
        toAdd.node          = child;
        toAdd.typeInfo      = child->typeInfo;
        toAdd.kind          = SymbolKind::Variable;
        toAdd.flags         = overload->flags;
        toAdd.storageOffset = overload->computedValue.storageOffset;
        symTable.addSymbolTypeInfo(context, toAdd);

        storageOffset += sizeof(void*);
        storageIndex++;
    }

    SWAG_VERIFY(!typeITable->fields.empty(), context->report({node, node->getTokenName(), FMT(Err(Err0075), node->token.ctext())}));
    typeInterface->itable = typeITable;

    // Struct interface, with one pointer for the data, and one pointer for itable
    if (!(node->hasAstFlag(AST_FROM_GENERIC)))
    {
        auto typeParam      = TypeManager::makeParam();
        typeParam->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        typeParam->offset   = 0;
        typeInterface->fields.push_back(typeParam);
        typeInterface->sizeOf += sizeof(void*);

        typeParam           = TypeManager::makeParam();
        typeParam->typeInfo = typeITable;
        typeParam->offset   = sizeof(void*);
        typeParam->index    = 1;
        typeInterface->fields.push_back(typeParam);
        typeInterface->sizeOf += sizeof(void*);
    }

    // Check public
    if (node->hasAttribute(ATTRIBUTE_PUBLIC))
    {
        if (!(node->hasAstFlag(AST_FROM_GENERIC)))
            node->ownerScope->addPublicNode(node);
    }

    // Register symbol with its type
    node->typeInfo = typeInterface;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::Interface;

    {
        // :BecauseOfThat
        ScopedLock lk(node->mutex);
        node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
        SWAG_CHECK(node->resolvedSymbolOverload);
        node->dependentJobs.setRunning();
    }

    // We are parsing the swag module
    if (sourceFile->isBootstrapFile)
        g_Workspace->swagScope.registerType(node->typeInfo);

    return true;
}

void Semantic::decreaseInterfaceRegCount(TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutex);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);

    SWAG_ASSERT(typeInfoStruct->cptRemainingInterfacesReg);
    typeInfoStruct->cptRemainingInterfacesReg--;
    if (!typeInfoStruct->cptRemainingInterfacesReg)
        typeInfoStruct->scope->dependentJobs.setRunning();
}

void Semantic::decreaseInterfaceCount(TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutex);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);

    SWAG_ASSERT(typeInfoStruct->cptRemainingInterfaces);
    typeInfoStruct->cptRemainingInterfaces--;
    if (!typeInfoStruct->cptRemainingInterfaces)
        typeInfoStruct->scope->dependentJobs.setRunning();
}

void Semantic::decreaseMethodCount(const AstFuncDecl* funcNode, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutex);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);

    SWAG_ASSERT(typeInfoStruct->cptRemainingMethods);
    typeInfoStruct->cptRemainingMethods--;
    const bool isSpecOp = funcNode->isSpecialFunctionName();
    if (isSpecOp)
        typeInfoStruct->cptRemainingSpecialMethods--;

    if (!typeInfoStruct->cptRemainingMethods || (isSpecOp && !typeInfoStruct->cptRemainingSpecialMethods))
        typeInfoStruct->scope->dependentJobs.setRunning();
}

bool Semantic::checkImplScopes(SemanticContext* context, AstImpl* node, const Scope* scopeImpl, const Scope* scope)
{
    // impl scope and corresponding identifier scope must be the same !
    if (scopeImpl != scope)
    {
        const Diagnostic diag{node, node->token, FMT(Err(Err0431), node->token.ctext())};
        const auto       note = Diagnostic::note(FMT(Nte(Nte0132), scopeImpl->parentScope->getFullName().c_str(), node->token.ctext(), scope->parentScope->getFullName().c_str()));
        return context->report(diag, Diagnostic::hereIs(node->identifier->resolvedSymbolOverload), note);
    }

    return true;
}

bool Semantic::resolveImpl(SemanticContext* context)
{
    const auto node = castAst<AstImpl>(context->node, AstNodeKind::Impl);

    // Be sure this is a struct
    const auto typeInfo = node->identifier->typeInfo;
    if (!typeInfo->isStruct() && !typeInfo->isEnum())
    {
        const Diagnostic diag{node->identifier, FMT(Err(Err0161), node->identifier->token.ctext(), typeInfo->getDisplayNameC())};
        return context->report(diag, Diagnostic::hereIs(node->identifier->resolvedSymbolOverload));
    }

    const auto typeIdentifier = node->identifier->resolvedSymbolOverload->typeInfo;
    SWAG_VERIFY(!typeIdentifier->isAlias(), context->report({node->identifier, Err(Err0700)}));

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Struct:
    {
        const auto structNode = castAst<AstStruct>(node->identifier->resolvedSymbolOverload->node, AstNodeKind::StructDecl);
        SWAG_CHECK(checkImplScopes(context, node, node->structScope, structNode->scope));
        break;
    }
    case TypeInfoKind::Enum:
    {
        const auto enumNode = castAst<AstEnum>(node->identifier->resolvedSymbolOverload->node, AstNodeKind::EnumDecl);
        SWAG_CHECK(checkImplScopes(context, node, node->structScope, enumNode->scope));
        break;
    }
    default:
        SWAG_ASSERT(false);
        break;
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::preResolveGeneratedStruct(SemanticContext* context)
{
    const auto structNode = castAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    const auto parent     = structNode->originalParent;
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
        const auto parentFunc = castAst<AstFuncDecl>(parent->ownerFct, AstNodeKind::FuncDecl);
        if (parentFunc->genericParameters)
            structNode->genericParameters = Ast::clone(parentFunc->genericParameters, nullptr, AST_GENERATED_GENERIC_PARAM);
    }

    if (parent->ownerStructScope && !structNode->genericParameters)
    {
        const auto parentStruct = (AstStruct*) parent->ownerStructScope->owner;
        if (parentStruct->genericParameters)
            structNode->genericParameters = Ast::clone(parentStruct->genericParameters, nullptr, AST_GENERATED_GENERIC_PARAM);
    }

    if (structNode->genericParameters)
    {
        Ast::addChildFront(structNode, structNode->genericParameters);

        Ast::visit(structNode->genericParameters, [&](AstNode* n)
        {
            n->inheritOwners(structNode);
            n->ownerStructScope = structNode->scope;
            n->ownerScope       = structNode->scope;
            n->addAstFlag(AST_IS_GENERIC);
        });
    }

    return true;
}

bool Semantic::preResolveStructContent(SemanticContext* context)
{
    const auto node = (AstStruct*) context->node->parent;
    SWAG_ASSERT(node->kind == AstNodeKind::StructDecl || node->kind == AstNodeKind::InterfaceDecl);

    const auto typeInfo = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    SWAG_CHECK(collectAttributes(context, node, &typeInfo->attributes));

    if (node->hasAttribute(ATTRIBUTE_NO_COPY))
        typeInfo->flags |= TYPEINFO_STRUCT_NO_COPY;

    typeInfo->declNode = node;
    node->scope->owner = node;

    // Add generic parameters
    uint32_t symbolFlags = 0;
    if (!(node->hasAstFlag(AST_FROM_GENERIC)))
    {
        if (node->genericParameters)
        {
            for (const auto param : node->genericParameters->childs)
            {
                auto funcParam      = TypeManager::makeParam();
                funcParam->name     = param->token.text;
                funcParam->typeInfo = param->typeInfo;
                if (param->hasSpecFlag(AstVarDecl::SPECFLAG_GENERIC_TYPE))
                    funcParam->flags |= TYPEINFOPARAM_GENERIC_TYPE;
                else if (param->hasSpecFlag(AstVarDecl::SPECFLAG_GENERIC_CONSTANT))
                    funcParam->flags |= TYPEINFOPARAM_GENERIC_CONSTANT;
                typeInfo->genericParameters.push_back(funcParam);
                typeInfo->sizeOf += param->typeInfo->sizeOf;
            }

            node->addAstFlag(AST_IS_GENERIC);
            typeInfo->flags |= TYPEINFO_GENERIC;
            symbolFlags |= OVERLOAD_GENERIC;
        }

        typeInfo->forceComputeName();
    }

    // Register symbol with its type
    auto symbolKind = SymbolKind::Struct;
    switch (node->kind)
    {
    case AstNodeKind::StructDecl:
        symbolKind = SymbolKind::Struct;
        break;
    case AstNodeKind::InterfaceDecl:
        symbolKind = SymbolKind::Interface;
        typeInfo->kind = TypeInfoKind::Interface;
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    if (node->hasAttribute(ATTRIBUTE_GEN))
        node->resolvedSymbolName->flags |= SYMBOL_ATTRIBUTE_GEN;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = symbolKind;
    toAdd.flags    = symbolFlags | OVERLOAD_INCOMPLETE;

    node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
    node->resolvedSymbolName     = toAdd.symbolName;
    SWAG_CHECK(node->resolvedSymbolOverload);

    return true;
}

void Semantic::flattenStructChilds(SemanticContext* context, AstNode* parent, VectorNative<AstNode*>& result)
{
    SharedLock lock(parent->mutex);
    for (auto child : parent->childs)
    {
        switch (child->kind)
        {
        case AstNodeKind::IdentifierRef:
        case AstNodeKind::Identifier:
        case AstNodeKind::Inline:
        case AstNodeKind::Statement:
        case AstNodeKind::CompilerIfBlock:
        case AstNodeKind::CompilerAst:
        case AstNodeKind::CompilerAssert:
            flattenStructChilds(context, child, result);
            continue;

        case AstNodeKind::CompilerIf:
        {
            const AstIf* compilerIf = castAst<AstIf>(child, AstNodeKind::CompilerIf);
            if (!(compilerIf->ifBlock->hasAstFlag(AST_NO_SEMANTIC)))
                flattenStructChilds(context, compilerIf->ifBlock, result);
            else if (compilerIf->elseBlock)
                flattenStructChilds(context, compilerIf->elseBlock, result);
            continue;
        }

        case AstNodeKind::AttrUse:
        {
            const AstAttrUse* attrUse = castAst<AstAttrUse>(child, AstNodeKind::AttrUse);
            if (attrUse->content->kind == AstNodeKind::Statement)
                flattenStructChilds(context, attrUse->content, result);
            else
                result.push_back(attrUse->content);
            continue;
        }
        default:
            break;
        }

        result.push_back(child);
    }
}

bool Semantic::solveValidIf(SemanticContext* context, const AstStruct* structDecl)
{
    ScopedLock lk1(structDecl->mutex);

    // Execute #validif/#validifx block
    const auto expr = structDecl->validif->childs.back();

    if (!expr->hasComputedValue())
    {
        const auto node            = context->node;
        context->validIfParameters = structDecl->genericParameters;

        PushErrCxtStep ec(context, node, ErrCxtStepKind::ValidIf, nullptr);
        const auto     result      = executeCompilerNode(context, expr, false);
        context->validIfParameters = nullptr;
        if (!result)
            return false;
        YIELD();
    }

    // Result
    SWAG_ASSERT(expr->computedValue);
    if (!expr->computedValue->reg.b)
    {
        const Diagnostic diag{structDecl->validif, structDecl->validif->token, FMT(Err(Err0084), structDecl->typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    return true;
}

bool Semantic::resolveStruct(SemanticContext* context)
{
    auto node       = castAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    auto job        = context->baseJob;

    SWAG_ASSERT(typeInfo->declNode);

    // #validif
    if (node->validif && !typeInfo->isGeneric())
    {
        SWAG_CHECK(solveValidIf(context, node));
        YIELD();
    }

    // Structure packing
    if (node->hasSpecFlag(AstStruct::SPECFLAG_UNION))
        node->packing = 0;
    else
    {
        auto value = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Pack, g_LangSpec->name_value);
        if (value)
            node->packing = value->reg.u8;
    }

    // Check 'opaque' attribute
    if (!sourceFile->isGenerated)
    {
        if (node->hasAttribute(ATTRIBUTE_OPAQUE))
        {
            SWAG_VERIFY(!sourceFile->forceExport, context->report({node, node->token, Err(Err0340)}));
            SWAG_VERIFY(node->isPublic(), context->report({node, node->token, Err(Err0339)}));
        }
    }

    // Be default, a tuple is 'constexpr'
    // If one of the childs is not, then the attribute will be removed
    if (typeInfo->isTuple())
        node->addAttribute(ATTRIBUTE_CONSTEXPR);

    uint32_t storageOffset     = 0;
    uint32_t storageIndexField = 0;
    uint32_t storageIndexConst = 0;
    uint64_t structFlags       = TYPEINFO_STRUCT_ALL_UNINITIALIZED | TYPEINFO_STRUCT_EMPTY;

    // No need to flatten structure if it's not a compound (optim)
    VectorNative<AstNode*>& childs = (node->hasAstFlag(AST_STRUCT_COMPOUND)) ? context->tmpNodes : node->content->childs;
    if (node->hasAstFlag(AST_STRUCT_COMPOUND))
    {
        context->tmpNodes.clear();
        flattenStructChilds(context, node->content, context->tmpNodes);
    }

    typeInfo->alignOf = 0;
    typeInfo->sizeOf  = 0;

    // If one of my childs is incomplete, then we must wait
    for (auto child : childs)
    {
        // Waiting for myself !
        if (child->typeInfo == typeInfo)
        {
            Diagnostic diag{node, node->getTokenName(), FMT(Err(Err0614), node->token.ctext())};
            auto       note = Diagnostic::note(child, Nte(Nte0067));
            return context->report(diag, note);
        }

        waitTypeCompleted(job, child->typeInfo);
        if (context->result != ContextResult::Done)
        {
            job->waitingHintNode = child;
            return true;
        }
    }

    {
        SWAG_RACE_CONDITION_WRITE(typeInfo->raceFields);
        for (auto child : childs)
        {
            if (child->kind != AstNodeKind::VarDecl && child->kind != AstNodeKind::ConstDecl)
                continue;

            auto varDecl = castAst<AstVarDecl>(child, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

            // Using can only be used on a structure
            if (child->hasAstFlag(AST_DECL_USING) && child->kind == AstNodeKind::ConstDecl)
                return context->report({child, Err(Err0473)});
            if (child->hasAstFlag(AST_DECL_USING) && !child->typeInfo->isStruct() && !child->typeInfo->isPointerTo(TypeInfoKind::Struct))
                return context->report({child, FMT(Err(Err0475), child->typeInfo->getDisplayNameC())});

            TypeInfoParam* typeParam = nullptr;
            if (!(node->hasAstFlag(AST_FROM_GENERIC)) || !(child->hasAstFlag(AST_REGISTERED_IN_STRUCT)))
            {
                typeParam           = TypeManager::makeParam();
                typeParam->name     = child->token.text;
                typeParam->typeInfo = child->typeInfo;
                typeParam->offset   = storageOffset;
                if (varDecl->hasAstFlag(AST_DECL_USING))
                    typeParam->flags |= TYPEINFOPARAM_HAS_USING;
                SWAG_CHECK(collectAttributes(context, child, &typeParam->attributes));
                if (child->kind == AstNodeKind::VarDecl)
                    typeInfo->fields.push_back(typeParam);
                else
                    typeInfo->consts.push_back(typeParam);
            }

            child->addAstFlag(AST_REGISTERED_IN_STRUCT);
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
            auto varTypeInfo = varDecl->typeInfo;

            // If variable is not empty, then struct is not
            if (!varTypeInfo->isStruct() && !varTypeInfo->isArrayOfStruct())
                structFlags &= ~TYPEINFO_STRUCT_EMPTY;

            // If variable is initialized, struct is too.
            if (!(varDecl->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED)))
            {
                if (varDecl->assignment || varDecl->type->hasSpecFlag(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS))
                    structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
                else if (!varTypeInfo->isStruct() && !varTypeInfo->isArrayOfStruct())
                    structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
            }

            // Inherit flags
            if (varTypeInfo->flags & TYPEINFO_STRUCT_NO_COPY)
                structFlags |= TYPEINFO_STRUCT_NO_COPY;

            // Remove attribute constexpr if necessary
            if (child->typeInfo->isStruct() && !(child->typeInfo->declNode->hasAttribute(ATTRIBUTE_CONSTEXPR)))
                node->removeAttribute(ATTRIBUTE_CONSTEXPR);

            // Var is a struct
            if (varTypeInfo->isStruct())
            {
                if (varTypeInfo->declNode->hasAttribute(ATTRIBUTE_EXPORT_TYPE_NO_ZERO))
                    structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                structFlags |= varTypeInfo->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES;

                if (!(varTypeInfo->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
                    structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
                if (!(varTypeInfo->flags & TYPEINFO_STRUCT_EMPTY))
                    structFlags &= ~TYPEINFO_STRUCT_EMPTY;

                if (varDecl->type && varDecl->type->hasSpecFlag(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS))
                {
                    structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                    if (!varDecl->type->hasComputedValue())
                    {
                        auto constSegment = getConstantSegFromContext(varDecl);
                        varDecl->type->setFlagsValueIsComputed();
                        varDecl->type->computedValue->storageSegment = constSegment;
                        SWAG_CHECK(collectAssignment(context, constSegment, varDecl->type->computedValue->storageOffset, varDecl));
                    }
                }

                // :opAffectConstExpr
                // Collect has already been simulated with an opAffect
                else if (varDecl->assignment)
                {
                    SWAG_ASSERT(varDecl->hasSemFlag(SEMFLAG_EXEC_RET_STACK));
                    structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                }
            }

            // Var is an array of structs
            else if (varTypeInfo->isArray() && !varDecl->assignment)
            {
                auto varTypeArray = castTypeInfo<TypeInfoArray>(varTypeInfo, TypeInfoKind::Array);
                if (varTypeArray->pointedType->isStruct())
                {
                    structFlags |= varTypeArray->pointedType->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES;
                    if (!(varTypeArray->pointedType->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
                        structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
                    if (!(varTypeArray->pointedType->flags & TYPEINFO_STRUCT_EMPTY))
                        structFlags &= ~TYPEINFO_STRUCT_EMPTY;
                }
            }

            // Var has an initialization
            else if (varDecl->assignment && !(varDecl->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED)))
            {
                SWAG_CHECK(checkIsConstExpr(context, varDecl->assignment->hasAstFlag(AST_CONST_EXPR), varDecl->assignment, Err(Err0041)));

                auto typeInfoAssignment = varDecl->assignment->typeInfo->getConcreteAlias();
                typeInfoAssignment      = TypeManager::concreteType(varDecl->assignment->typeInfo, CONCRETE_ENUM);

                if (typeInfoAssignment->isString())
                {
                    structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                    if (typeParam)
                    {
                        typeParam->allocateComputedValue();
                        typeParam->value->reg = varDecl->assignment->computedValue->reg;
                    }
                }
                else if (!typeInfoAssignment->isNative() || varDecl->assignment->computedValue->reg.u64)
                {
                    structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                    if (typeParam)
                    {
                        typeParam->allocateComputedValue();
                        typeParam->value->reg = varDecl->assignment->computedValue->reg;
                    }
                }

                structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
            }

            // If the struct is not generic, be sure that a field is not generic either
            if (!(node->hasAstFlag(AST_IS_GENERIC)))
            {
                if (varTypeInfo->isGeneric())
                {
                    if (varDecl->type)
                        child = varDecl->type;

                    if (!node->genericParameters)
                    {
                        Diagnostic diag{node, node->getTokenName(), FMT(Err(Err0561), node->token.ctext(), varDecl->token.ctext())};
                        auto       note = Diagnostic::note(child, child->token, Diagnostic::isType(child->typeInfo));
                        return context->report(diag, note);
                    }

                    Diagnostic diag{node, node->getTokenName(), FMT(Err(Err0560), node->token.ctext(), varDecl->token.ctext())};
                    auto       note = Diagnostic::note(child, child->token, Diagnostic::isType(child->typeInfo));
                    return context->report(diag, note);
                }
            }

            auto realStorageOffset = storageOffset;

            // Attribute 'Swag.offset' can be used to force the storage offset of the member
            bool relocated = false;
            if (typeParam)
            {
                auto forceOffset = typeParam->attributes.getValue(g_LangSpec->name_Swag_Offset, g_LangSpec->name_name);
                if (forceOffset)
                {
                    for (auto p : typeInfo->fields)
                    {
                        if (p->name == forceOffset->text)
                        {
                            realStorageOffset = p->offset;
                            relocated         = true;
                            break;
                        }
                    }

                    if (!relocated)
                    {
                        auto attr = typeParam->attributes.getAttribute(g_LangSpec->name_Swag_Offset);
                        SWAG_ASSERT(attr);
                        return context->report({attr->node, attr->parameters[0].token, FMT(Err(Err0725), forceOffset->text.c_str())});
                    }
                }
            }

            // Compute struct alignment
            auto alignOf      = TypeManager::alignOf(child->typeInfo);
            typeInfo->alignOf = max(typeInfo->alignOf, alignOf);

            // Compute padding before the current field
            if (!relocated && node->packing > 1 && alignOf)
            {
                auto userAlignOf = typeParam->attributes.getValue(g_LangSpec->name_Swag_Align, g_LangSpec->name_value);
                if (userAlignOf)
                    alignOf = userAlignOf->reg.u8;
                else
                    alignOf = min(alignOf, node->packing);
                realStorageOffset = (uint32_t) TypeManager::align(realStorageOffset, alignOf);
                storageOffset     = (uint32_t) TypeManager::align(storageOffset, alignOf);
            }

            typeParam->offset                                          = realStorageOffset;
            child->resolvedSymbolOverload->computedValue.storageOffset = realStorageOffset;
            child->resolvedSymbolOverload->storageIndex                = storageIndexField;

            auto childType   = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);
            typeInfo->sizeOf = max(typeInfo->sizeOf, (int) realStorageOffset + childType->sizeOf);

            if (relocated)
                storageOffset = max(storageOffset, realStorageOffset + childType->sizeOf);
            else if (node->packing)
                storageOffset += childType->sizeOf;

            // Create a generic alias
            if (!(child->hasSpecFlag(AstVarDecl::SPECFLAG_AUTO_NAME)))
            {
                // Special field name starts with 'item' followed by a number
                bool hasItemName = false;
                if (child->token.text.length() > 4 &&
                    child->token.text[0] == 'i' && child->token.text[1] == 't' && child->token.text[2] == 'e' && child->token.text[3] == 'm')
                {
                    hasItemName = true;
                    for (uint32_t idx = 4; idx < child->token.text.length(); idx++)
                    {
                        if (!isdigit(child->token.text[idx]))
                            hasItemName = false;
                    }
                }

                // User cannot name its variables itemX
                if (!(node->hasAstFlag(AST_GENERATED)) && hasItemName)
                {
                    return context->report({child, child->token, FMT(Err(Err0619), child->token.ctext())});
                }

                if (!hasItemName)
                {
                    auto  overload = child->resolvedSymbolOverload;
                    Utf8  name     = FMT("item%u", storageIndexField);
                    auto& symTable = node->scope->symTable;

                    AddSymbolTypeInfo toAdd;
                    toAdd.node           = child;
                    toAdd.typeInfo       = child->typeInfo;
                    toAdd.kind           = SymbolKind::Variable;
                    toAdd.flags          = overload->flags;
                    toAdd.storageOffset  = overload->computedValue.storageOffset;
                    toAdd.storageSegment = overload->computedValue.storageSegment;
                    toAdd.aliasName      = &name;

                    symTable.addSymbolTypeInfo(context, toAdd);
                }
                else
                {
                    typeParam->flags |= TYPEINFOPARAM_AUTO_NAME;
                }
            }
            else
            {
                typeParam->flags |= TYPEINFOPARAM_AUTO_NAME;
            }

            storageIndexField++;
        }
    }

    // A struct cannot have a zero size
    if (!typeInfo->sizeOf)
    {
        typeInfo->sizeOf  = 1;
        typeInfo->alignOf = 1;
    }

    // User specific alignment
    auto userAlignOf = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Align, g_LangSpec->name_value);
    if (userAlignOf)
        typeInfo->alignOf = userAlignOf->reg.u8;
    else if (node->packing)
        typeInfo->alignOf = min(typeInfo->alignOf, node->packing);
    typeInfo->alignOf = max(1, typeInfo->alignOf);

    // An opaque struct will be exported as an array of bytes.
    // We need to be sure that alignment will be respected, so we force "Swag.Align" attribute
    // if not already present.
    if (!userAlignOf && typeInfo->attributes.hasAttribute(g_LangSpec->name_Swag_Opaque))
    {
        OneAttribute       ot;
        AttributeParameter otp;
        ot.name           = g_LangSpec->name_Swag_Align;
        otp.token.text    = g_LangSpec->name_value;
        otp.typeInfo      = g_TypeMgr->typeInfoU8;
        otp.value.reg.u64 = typeInfo->alignOf;
        ot.parameters.push_back(otp);
        typeInfo->attributes.emplace(ot);
    }

    // Align structure size
    if (typeInfo->alignOf > 1 && !typeInfo->isGeneric())
        typeInfo->sizeOf = (uint32_t) TypeManager::align(typeInfo->sizeOf, typeInfo->alignOf);

    // Check public
    if ((node->hasAttribute(ATTRIBUTE_PUBLIC)) && !typeInfo->isTuple())
    {
        if (!(node->hasAstFlag(AST_FROM_GENERIC)))
            node->ownerScope->addPublicNode(node);
    }

    // Need to recompute it if it's from generic
    if (node->hasAstFlag(AST_FROM_GENERIC))
    {
        typeInfo->flags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
        typeInfo->flags &= ~TYPEINFO_STRUCT_HAS_INIT_VALUES;
        typeInfo->flags &= ~TYPEINFO_STRUCT_EMPTY;
    }

    typeInfo->flags |= (structFlags & TYPEINFO_STRUCT_ALL_UNINITIALIZED);
    typeInfo->flags |= (structFlags & TYPEINFO_STRUCT_EMPTY);
    typeInfo->flags |= (structFlags & TYPEINFO_STRUCT_HAS_INIT_VALUES);
    typeInfo->flags |= (structFlags & TYPEINFO_STRUCT_NO_COPY);

    // Register symbol with its type
    node->typeInfo = typeInfo;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::Struct;

    {
        // :BecauseOfThat
        ScopedLock lk(node->mutex);
        node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
        SWAG_CHECK(node->resolvedSymbolOverload);
        node->dependentJobs.setRunning();
    }

    // We are parsing the swag module
    if (sourceFile->isBootstrapFile)
        g_Workspace->swagScope.registerType(node->typeInfo);

    // Generate all functions associated with a struct
    if (!typeInfo->isGeneric())
    {
        node->removeAstFlag(AST_NO_BYTECODE);
        node->addAstFlag(AST_NO_BYTECODE_CHILDS);
        SWAG_ASSERT(!node->hasExtByteCode() || !node->extByteCode()->byteCodeJob);

        node->allocateExtension(ExtensionKind::ByteCode);
        auto extension         = node->extByteCode();
        extension->byteCodeJob = ByteCodeGenJob::newJob(context->baseJob->dependentJob, sourceFile, node);
        node->byteCodeFct      = ByteCodeGen::emitStruct;

        if (node->hasAttribute(ATTRIBUTE_GEN))
            node->resolvedSymbolName->addDependentJob(extension->byteCodeJob);
        else
            g_ThreadMgr.addJob(extension->byteCodeJob);
    }

    return true;
}
