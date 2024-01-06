#include "pch.h"
#include "Semantic.h"
#include "Ast.h"
#include "Workspace.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ByteCode.h"
#include "ModuleManager.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "Parser.h"

bool Semantic::waitForStructUserOps(SemanticContext* context, AstNode* node)
{
    SymbolName* symbol = nullptr;
    SWAG_CHECK(waitUserOp(context, g_LangSpec->name_opPostCopy, node, &symbol));
    YIELD();
    SWAG_CHECK(waitUserOp(context, g_LangSpec->name_opPostMove, node, &symbol));
    YIELD();
    SWAG_CHECK(waitUserOp(context, g_LangSpec->name_opDrop, node, &symbol));
    YIELD();
    return true;
}

bool Semantic::resolveImplForAfterFor(SemanticContext* context)
{
    auto id   = context->node;
    auto node = CastAst<AstImpl>(context->node->parent, AstNodeKind::Impl);

    if (id->resolvedSymbolName->kind != SymbolKind::Struct)
        return context->report({id->childs.back(), Fmt(Err(Err0290), id->resolvedSymbolName->name.c_str(), Naming::aKindName(id->resolvedSymbolName->kind).c_str())});

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
        auto       typeStruct = CastTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
        ScopedLock lk1(typeStruct->mutex);
        typeStruct->cptRemainingInterfacesReg++;
        typeStruct->cptRemainingInterfaces++;
        node->sourceFile->module->decImplForToSolve(typeStruct);

        ScopedLock lk2(node->structScope->parentScope->mutex);
        ScopedLock lk3(node->structScope->mutex);

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

bool Semantic::resolveImplForType(SemanticContext* context)
{
    auto     node       = CastAst<AstImpl>(context->node, AstNodeKind::Impl);
    auto     sourceFile = node->sourceFile;
    auto     module     = sourceFile->module;
    AstNode* first;
    AstNode* back;

    // Race condition in case a templated function is instantiated in the impl block during that access
    {
        SharedLock lk(node->mutex);
        first = node->childs[0];
        back  = node->childs[1];
    }

    auto typeStruct = CastTypeInfo<TypeInfoStruct>(back->typeInfo, TypeInfoKind::Struct);

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

    auto typeBaseInterface = CastTypeInfo<TypeInfoStruct>(first->typeInfo, TypeInfoKind::Interface);
    auto typeParamItf      = typeStruct->hasInterface(typeBaseInterface);
    SWAG_ASSERT(typeParamItf);

    auto constSegment = back->computedValue->storageSegment;
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
    auto node = CastAst<AstImpl>(context->node, AstNodeKind::Impl);
    auto job  = context->baseJob;

    // Be sure the first identifier is an interface
    auto typeInfo = node->identifier->typeInfo;
    if (!typeInfo->isInterface())
    {
        Diagnostic diag{node->identifier, Fmt(Err(Err0646), node->identifier->token.ctext(), Naming::aKindName(typeInfo).c_str())};
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
    auto typeBaseInterface = CastTypeInfo<TypeInfoStruct>(node->childs[0]->typeInfo, TypeInfoKind::Interface);
    auto typeStruct        = CastTypeInfo<TypeInfoStruct>(node->childs[1]->typeInfo, TypeInfoKind::Struct);

    // Be sure interface has been fully solved
    {
        ScopedLock lk(node->identifier->mutex);
        ScopedLock lk1(node->identifier->resolvedSymbolName->mutex);
        if (node->identifier->resolvedSymbolName->cptOverloads)
        {
            Semantic::waitSymbolNoLock(job, node->identifier->resolvedSymbolName);
            return true;
        }
    }

    // We need now the pointer to the itable
    auto     typeInterface   = CastTypeInfo<TypeInfoStruct>(typeBaseInterface->itable, TypeInfoKind::Struct);
    uint32_t numFctInterface = (uint32_t) typeInterface->fields.size();

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
            typeParamItf = g_TypeMgr->makeParam();
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

    for (size_t i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];
        if (child->kind != AstNodeKind::FuncDecl)
            continue;
        auto childFct = CastAst<AstFuncDecl>(child, AstNodeKind::FuncDecl);

        // We need to search the function (as a variable) in the interface
        auto itfSymbol = typeInterface->findChildByNameNoLock(child->token.text); // O(n) !

        // Must be an 'impl' function
        if (!(childFct->specFlags & AstFuncDecl::SPECFLAG_IMPL))
        {
            // Cannot have the same name as a function of the interface
            if (itfSymbol)
            {
                Diagnostic diag{childFct, childFct->tokenName, Fmt(Err(Err0280), childFct->token.text.c_str(), typeInterface->name.c_str())};
                diag.hint = Nte(Nte1124);
                auto hlp  = Diagnostic::note(Nte(Nte0124));
                return context->report(diag, hlp);
            }

            continue;
        }

        if (!itfSymbol)
        {
            Diagnostic diag{childFct, childFct->tokenName, Fmt(Err(Err0024), childFct->token.text.c_str(), typeInterface->name.c_str())};
            auto       note = Diagnostic::note(findClosestMatchesMsg(childFct->token.text, {{typeInterface->scope}}, IdentifierSearchFor::Whatever));
            return context->report(diag, note);
        }

        // We need to be sure function semantic is done
        {
            auto symbolName = node->scope->symTable.find(child->token.text);
            SWAG_ASSERT(symbolName);
            ScopedLock lk(symbolName->mutex);
            if (symbolName->cptOverloads)
            {
                Semantic::waitSymbolNoLock(job, symbolName);
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
        auto              typeLambda = CastTypeInfo<TypeInfoFuncAttr>(itfSymbol->typeInfo, TypeInfoKind::LambdaClosure);
        auto              typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeLambda->isSame(typeFunc, CASTFLAG_EXACT | CASTFLAG_INTERFACE, bi))
        {
            switch (bi.matchResult)
            {
            case MatchResult::BadSignature:
            {
                Diagnostic diag{childFct->parameters->childs[bi.badSignatureNum2], Fmt(Err(Err0652), child->token.ctext(), typeBaseInterface->name.c_str())};
                diag.hint = Nte(Nte1113);
                auto note = Diagnostic::note(typeLambda->parameters[bi.badSignatureNum1]->declNode, Fmt(Nte(Nte0081), typeLambda->parameters[bi.badSignatureNum1]->typeInfo->getDisplayNameC()));
                return context->report(diag, note);
            }

            case MatchResult::MissingReturnType:
            {
                Diagnostic diag{child, child->token, Fmt(Err(Err0652), child->token.ctext(), typeBaseInterface->name.c_str())};
                diag.hint = Nte(Nte1114);
                auto note = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, Fmt(Nte(Nte0083), typeLambda->returnType->getDisplayNameC()));
                return context->report(diag, note);
            }

            case MatchResult::NoReturnType:
            {
                Diagnostic diag{childFct->returnType, Fmt(Err(Err0652), child->token.ctext(), typeBaseInterface->name.c_str())};
                auto       note = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, Nte(Nte0082));
                return context->report(diag, note);
            }

            case MatchResult::MismatchReturnType:
            {
                Diagnostic diag{childFct->returnType, Fmt(Err(Err0652), child->token.ctext(), typeBaseInterface->name.c_str())};
                diag.hint = Diagnostic::isType(childFct->returnType->typeInfo);
                auto note = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, Fmt(Nte(Nte0083), typeLambda->returnType->getDisplayNameC()));
                return context->report(diag, note);
            }

            case MatchResult::MismatchThrow:
            {
                Diagnostic diag{child, child->token, Fmt(Err(Err0652), child->token.ctext(), typeBaseInterface->name.c_str())};
                diag.hint       = Nte(Nte1115);
                auto note       = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, Nte(Nte0002));
                note->showRange = false;
                return context->report(diag, note);
            }

            default:
            {
                Diagnostic diag{child, child->token, Fmt(Err(Err0652), child->token.ctext(), typeBaseInterface->name.c_str())};
                auto       note = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->token, Nte(Nte0002));
                note->showRange = false;
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
    Vector<const Diagnostic*> notes;
    InterfaceRef              itfRef;
    bool                      doneItfRef = false;
    Utf8                      content;
    for (uint32_t idx = 0; idx < numFctInterface; idx++)
    {
        // Function is there
        if (mapItIdxToFunc[idx])
            continue;

        if (!doneItfRef)
        {
            doneItfRef = true;
            SWAG_CHECK(TypeManager::collectInterface(context, typeStruct, typeBaseInterface, itfRef, true));
            YIELD();
        }

        auto missingNode = typeInterface->fields[idx];
        if (!itfRef.itf)
        {
            Diagnostic diag{node, node->token, Fmt(Err(Err0657), typeBaseInterface->name.c_str(), typeInfo->getDisplayNameC())};
            auto       note = Diagnostic::note(missingNode->declNode, missingNode->declNode->token, Fmt("missing '%s'", missingNode->name.c_str()));
            return context->report(diag, note);
        }

        content += "func impl ";
        content += missingNode->name;

        content += "(using self";
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(missingNode->typeInfo, TypeInfoKind::LambdaClosure);
        for (size_t i = 1; i < typeFunc->parameters.size(); i++)
        {
            content += ", ";
            auto type = typeFunc->parameters[i]->typeInfo;
            type->computeScopedNameExport();
            content += Fmt("p%d: %s", i, type->scopedNameExport.c_str());
        }
        content += ")";

        content += " => ";
        content += itfRef.fieldRef;
        content += ".";
        content += typeBaseInterface->name;
        content += ".";
        content += missingNode->name;

        content += "(";
        for (size_t i = 1; i < typeFunc->parameters.size(); i++)
        {
            if (i != 1)
                content += ",";
            content += Fmt("p%d", i);
        }
        content += ")\n";
    }

    // We have generated methods, so restart
    if (!content.empty())
    {
        int numChilds = (int) node->childs.size();

        Parser parser;
        parser.setup(context, context->sourceFile->module, context->sourceFile);
        SWAG_CHECK(parser.constructEmbeddedAst(content, node, node, CompilerAstKind::MissingInterfaceMtd, true));

        for (size_t i = numChilds; i < node->childs.size(); i++)
            context->baseJob->nodes.push_back(node->childs[i]);
        context->result = ContextResult::NewChilds;
        return true;
    }

    // Construct itable in the constant segment
    // Header has 2 pointers
    // :itableHeader
    auto     constSegment = getConstantSegFromContext(node);
    void**   ptrITable;
    int      sizeOfHeader = 2 * sizeof(void*);
    uint32_t itableOffset = constSegment->reserve((numFctInterface * sizeof(void*)) + sizeOfHeader, (uint8_t**) &ptrITable, sizeof(void*));
    auto     offset       = itableOffset;

    // :itableHeader
    // The first value will be the concrete type to the interface
    // The second value will be the concrete type to the corresponding struct
    *ptrITable++ = nullptr;
    offset += sizeof(void*);
    *ptrITable++ = nullptr;
    offset += sizeof(void*);

    for (uint32_t i = 0; i < numFctInterface; i++)
    {
        auto funcChild = mapItIdxToFunc[i];
        if (funcChild->isForeign())
        {
            funcChild->computeFullNameForeign(true);
            constSegment->addInitPtrFunc(offset, funcChild->fullnameForeign);

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
    auto node          = CastAst<AstStruct>(context->node, AstNodeKind::InterfaceDecl);
    auto sourceFile    = context->sourceFile;
    auto typeInterface = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Interface);

    typeInterface->declNode   = node;
    typeInterface->name       = node->token.text;
    typeInterface->structName = node->token.text;

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;

    VectorNative<AstNode*>& childs = (node->flags & AST_STRUCT_COMPOUND) ? context->tmpNodes : node->content->childs;
    if (node->flags & AST_STRUCT_COMPOUND)
    {
        context->tmpNodes.clear();
        flattenStructChilds(context, node->content, context->tmpNodes);
    }

    // itable
    auto typeITable        = makeType<TypeInfoStruct>();
    typeITable->name       = node->token.text;
    typeITable->structName = node->token.text;
    typeITable->scope      = Ast::newScope(node, node->token.text, ScopeKind::Struct, nullptr);
    typeITable->flags |= TYPEINFO_STRUCT_IS_ITABLE;

    for (size_t i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];
        if (child->kind != AstNodeKind::VarDecl)
            continue;

        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);

        TypeInfoParam* typeParam = nullptr;
        if (!(node->flags & AST_FROM_GENERIC))
        {
            typeParam           = g_TypeMgr->makeParam();
            typeParam->name     = child->token.text;
            typeParam->offset   = storageOffset;
            typeParam->declNode = child;
            SWAG_CHECK(collectAttributes(context, child, &typeParam->attributes));
            typeITable->fields.push_back(typeParam);

            // Verify signature
            typeParam->typeInfo = child->typeInfo->getConcreteAlias();
            auto typeLambda     = CastTypeInfo<TypeInfoFuncAttr>(typeParam->typeInfo, TypeInfoKind::LambdaClosure);

            if (typeLambda->parameters.size() == 0)
            {
                Diagnostic diag{varDecl->type, Fmt(Err(Err0677), child->token.ctext())};
                return context->report(diag);
            }

            auto firstParamType = typeLambda->parameters[0]->typeInfo;
            SWAG_VERIFY(firstParamType->isPointer(), context->report({typeLambda->parameters[0]->declNode, Fmt(Err(Err0679), firstParamType->getDisplayNameC())}));
            auto firstParamPtr = CastTypeInfo<TypeInfoPointer>(firstParamType, TypeInfoKind::Pointer);
            SWAG_VERIFY(firstParamPtr->pointedType == typeInterface, context->report({typeLambda->parameters[0]->declNode, Fmt(Err(Err0679), firstParamType->getDisplayNameC())}));
        }

        typeParam           = typeITable->fields[storageIndex];
        typeParam->typeInfo = child->typeInfo;
        typeParam->declNode = child;
        typeParam->index    = storageIndex;

        if (!(node->flags & AST_IS_GENERIC))
        {
            SWAG_VERIFY(!child->typeInfo->isGeneric(), context->report({child, Fmt(Err(Err0681), node->token.ctext(), child->typeInfo->getDisplayNameC())}));
        }

        if (typeParam->attributes.hasAttribute(g_LangSpec->name_Swag_Offset))
        {
            auto attr = typeParam->attributes.getAttribute(g_LangSpec->name_Swag_Offset);
            SWAG_ASSERT(attr);
            return context->report({attr->node, attr->node->token, Err(Err0682)});
        }

        typeParam->offset                                          = storageOffset;
        child->resolvedSymbolOverload->computedValue.storageOffset = storageOffset;
        child->resolvedSymbolOverload->storageIndex                = storageIndex;

        SWAG_ASSERT(child->typeInfo->sizeOf == sizeof(void*));
        typeITable->sizeOf += sizeof(void*);

        // Register lambda variable in the scope of the itable struct
        auto              overload = child->resolvedSymbolOverload;
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

    SWAG_VERIFY(!typeITable->fields.empty(), context->report({node, node->token, Fmt(Err(Err0683), node->token.ctext())}));
    typeInterface->itable = typeITable;

    // Struct interface, with one pointer for the data, and one pointer for itable
    if (!(node->flags & AST_FROM_GENERIC))
    {
        auto typeParam      = g_TypeMgr->makeParam();
        typeParam->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        typeParam->offset   = 0;
        typeInterface->fields.push_back(typeParam);
        typeInterface->sizeOf += sizeof(void*);

        typeParam           = g_TypeMgr->makeParam();
        typeParam->typeInfo = typeITable;
        typeParam->offset   = sizeof(void*);
        typeParam->index    = 1;
        typeInterface->fields.push_back(typeParam);
        typeInterface->sizeOf += sizeof(void*);
    }

    // Check public
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (!(node->flags & AST_FROM_GENERIC))
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

void Semantic::decreaseMethodCount(AstFuncDecl* funcNode, TypeInfoStruct* typeInfoStruct)
{
    ScopedLock lk(typeInfoStruct->mutex);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);

    SWAG_ASSERT(typeInfoStruct->cptRemainingMethods);
    typeInfoStruct->cptRemainingMethods--;
    bool isSpecOp = funcNode->isSpecialFunctionName();
    if (isSpecOp)
        typeInfoStruct->cptRemainingSpecialMethods--;

    if (!typeInfoStruct->cptRemainingMethods || (isSpecOp && !typeInfoStruct->cptRemainingSpecialMethods))
        typeInfoStruct->scope->dependentJobs.setRunning();
}

bool Semantic::checkImplScopes(SemanticContext* context, AstImpl* node, Scope* scopeImpl, Scope* scope)
{
    // impl scope and corresponding identifier scope must be the same !
    if (scopeImpl != scope)
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0661), node->token.ctext())};
        auto       note = Diagnostic::note(Fmt(Nte(Nte1086), scopeImpl->parentScope->getFullName().c_str(), node->token.ctext(), scope->parentScope->getFullName().c_str()));
        return context->report(diag, Diagnostic::hereIs(node->identifier->resolvedSymbolOverload), note);
    }

    return true;
}

bool Semantic::resolveImpl(SemanticContext* context)
{
    auto node = CastAst<AstImpl>(context->node, AstNodeKind::Impl);

    // Be sure this is a struct
    auto typeInfo = node->identifier->typeInfo;
    if (!typeInfo->isStruct() && !typeInfo->isEnum())
    {
        Diagnostic diag{node->identifier, Fmt(Err(Err0662), node->identifier->token.ctext(), typeInfo->getDisplayNameC())};
        return context->report(diag, Diagnostic::hereIs(node->identifier->resolvedSymbolOverload));
    }

    auto typeIdentifier = node->identifier->resolvedSymbolOverload->typeInfo;
    SWAG_VERIFY(!typeIdentifier->isAlias(), context->report({node->identifier, Err(Err0664)}));

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Struct:
    {
        auto structNode = CastAst<AstStruct>(node->identifier->resolvedSymbolOverload->node, AstNodeKind::StructDecl);
        SWAG_CHECK(checkImplScopes(context, node, node->structScope, structNode->scope));
        break;
    }
    case TypeInfoKind::Enum:
    {
        auto enumNode = CastAst<AstEnum>(node->identifier->resolvedSymbolOverload->node, AstNodeKind::EnumDecl);
        SWAG_CHECK(checkImplScopes(context, node, node->structScope, enumNode->scope));
        break;
    }
    default:
        SWAG_ASSERT(false);
        break;
    }

    return true;
}

bool Semantic::preResolveGeneratedStruct(SemanticContext* context)
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

        Ast::visit(structNode->genericParameters, [&](AstNode* n)
                   {
                       n->inheritOwners(structNode);
                       n->ownerStructScope = structNode->scope;
                       n->ownerScope       = structNode->scope;
                       n->flags |= AST_IS_GENERIC; });
    }

    return true;
}

bool Semantic::preResolveStructContent(SemanticContext* context)
{
    auto node = (AstStruct*) context->node->parent;
    SWAG_ASSERT(node->kind == AstNodeKind::StructDecl || node->kind == AstNodeKind::InterfaceDecl);

    auto typeInfo = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    SWAG_CHECK(collectAttributes(context, node, &typeInfo->attributes));

    if (node->attributeFlags & ATTRIBUTE_NO_COPY)
        typeInfo->flags |= TYPEINFO_STRUCT_NO_COPY;

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
                auto funcParam      = g_TypeMgr->makeParam();
                funcParam->name     = param->token.text;
                funcParam->typeInfo = param->typeInfo;
                if (param->specFlags & AstVarDecl::SPECFLAG_GENERIC_TYPE)
                    funcParam->flags |= TYPEINFOPARAM_GENERIC_TYPE;
                else if (param->specFlags & AstVarDecl::SPECFLAG_GENERIC_CONSTANT)
                    funcParam->flags |= TYPEINFOPARAM_GENERIC_CONSTANT;
                typeInfo->genericParameters.push_back(funcParam);
                typeInfo->sizeOf += param->typeInfo->sizeOf;
            }

            node->flags |= AST_IS_GENERIC;
            typeInfo->flags |= TYPEINFO_GENERIC;
            symbolFlags |= OVERLOAD_GENERIC;
        }

        typeInfo->forceComputeName();
    }

    // Register symbol with its type
    SymbolKind symbolKind = SymbolKind::Struct;
    switch (node->kind)
    {
    case AstNodeKind::StructDecl:
        symbolKind = SymbolKind::Struct;
        break;
    case AstNodeKind::InterfaceDecl:
        symbolKind     = SymbolKind::Interface;
        typeInfo->kind = TypeInfoKind::Interface;
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    if (node->attributeFlags & ATTRIBUTE_GEN)
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
        default:
            break;
        }

        result.push_back(child);
    }
}

bool Semantic::solveValidIf(SemanticContext* context, AstStruct* structDecl)
{
    ScopedLock lk1(structDecl->mutex);

    // Execute #validif/#validifx block
    auto expr = structDecl->validif->childs.back();

    if (!expr->hasComputedValue())
    {
        auto node                  = context->node;
        context->validIfParameters = structDecl->genericParameters;

        PushErrCxtStep ec(context, node, ErrCxtStepKind::ValidIf, nullptr);
        auto           result      = executeCompilerNode(context, expr, false);
        context->validIfParameters = nullptr;
        if (!result)
            return false;
        YIELD();
    }

    // Result
    SWAG_ASSERT(expr->computedValue);
    if (!expr->computedValue->reg.b)
    {
        Diagnostic diag{structDecl->validif, structDecl->validif->token, Fmt(Err(Err0617), structDecl->typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    return true;
}

bool Semantic::resolveStruct(SemanticContext* context)
{
    auto node       = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    auto job        = context->baseJob;

    SWAG_ASSERT(typeInfo->declNode);

    // #validif
    if (node->validif && !typeInfo->isGeneric())
    {
        SWAG_CHECK(solveValidIf(context, node));
        YIELD();
    }

    // Structure packing
    if (node->specFlags & AstStruct::SPECFLAG_UNION)
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
        if (node->attributeFlags & ATTRIBUTE_OPAQUE)
        {
            SWAG_VERIFY(!sourceFile->forceExport, context->report({node, node->token, Err(Err0667)}));
            SWAG_VERIFY(node->isPublic(), context->report({node, node->token, Err(Err0666)}));
        }
    }

    // Be default, a tuple is 'constexpr'
    // If one of the childs is not, then the attribute will be removed
    if (typeInfo->isTuple())
        node->attributeFlags |= ATTRIBUTE_CONSTEXPR;

    uint32_t storageOffset     = 0;
    uint32_t storageIndexField = 0;
    uint32_t storageIndexConst = 0;
    uint64_t structFlags       = TYPEINFO_STRUCT_ALL_UNINITIALIZED | TYPEINFO_STRUCT_EMPTY;

    // No need to flatten structure if it's not a compound (optim)
    VectorNative<AstNode*>& childs = (node->flags & AST_STRUCT_COMPOUND) ? context->tmpNodes : node->content->childs;
    if (node->flags & AST_STRUCT_COMPOUND)
    {
        context->tmpNodes.clear();
        flattenStructChilds(context, node->content, context->tmpNodes);
    }

    typeInfo->alignOf = 0;
    typeInfo->sizeOf  = 0;

    // If one of my childs is incomplete, then we must wait
    for (size_t i = 0; i < childs.size(); i++)
    {
        auto child = childs[i];

        // Waiting for myself !
        if (child->typeInfo == typeInfo)
        {
            Diagnostic diag{node, Fmt(Err(Err0201), typeInfo->getDisplayNameC())};
            auto       note = Diagnostic::note(child, Nte(Nte0064));
            return context->report(diag, note);
        }

        Semantic::waitTypeCompleted(job, child->typeInfo);
        if (context->result != ContextResult::Done)
        {
            job->waitingHintNode = child;
            return true;
        }
    }

    {
        SWAG_RACE_CONDITION_WRITE(typeInfo->raceFields);
        for (size_t i = 0; i < childs.size(); i++)
        {
            auto child = childs[i];
            if (child->kind != AstNodeKind::VarDecl && child->kind != AstNodeKind::ConstDecl)
                continue;

            auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

            // Using can only be used on a structure
            if (child->flags & AST_DECL_USING && child->kind == AstNodeKind::ConstDecl)
                return context->report({child, Err(Err0668)});
            if (child->flags & AST_DECL_USING && !child->typeInfo->isStruct() && !child->typeInfo->isPointerTo(TypeInfoKind::Struct))
                return context->report({child, Fmt(Err(Err0669), child->typeInfo->getDisplayNameC())});

            TypeInfoParam* typeParam = nullptr;
            if (!(node->flags & AST_FROM_GENERIC) || !(child->flags & AST_REGISTERED_IN_STRUCT))
            {
                typeParam           = g_TypeMgr->makeParam();
                typeParam->name     = child->token.text;
                typeParam->typeInfo = child->typeInfo;
                typeParam->offset   = storageOffset;
                if (varDecl->flags & AST_DECL_USING)
                    typeParam->flags |= TYPEINFOPARAM_HAS_USING;
                SWAG_CHECK(collectAttributes(context, child, &typeParam->attributes));
                if (child->kind == AstNodeKind::VarDecl)
                    typeInfo->fields.push_back(typeParam);
                else
                    typeInfo->consts.push_back(typeParam);
            }

            child->flags |= AST_REGISTERED_IN_STRUCT;
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
            if (!(varDecl->flags & AST_EXPLICITLY_NOT_INITIALIZED))
            {
                if (varDecl->assignment || varDecl->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS)
                    structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
                else if (!varTypeInfo->isStruct() && !varTypeInfo->isArrayOfStruct())
                    structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
            }

            // Inherit flags
            if (varTypeInfo->flags & TYPEINFO_STRUCT_NO_COPY)
                structFlags |= TYPEINFO_STRUCT_NO_COPY;

            // Remove attribute constexpr if necessary
            if (child->typeInfo->isStruct() && !(child->typeInfo->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR))
                node->attributeFlags &= ~ATTRIBUTE_CONSTEXPR;

            // Var is a struct
            if (varTypeInfo->isStruct())
            {
                if (varTypeInfo->declNode->attributeFlags & ATTRIBUTE_EXPORT_TYPE_NOZERO)
                    structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                structFlags |= varTypeInfo->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES;

                if (!(varTypeInfo->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED))
                    structFlags &= ~TYPEINFO_STRUCT_ALL_UNINITIALIZED;
                if (!(varTypeInfo->flags & TYPEINFO_STRUCT_EMPTY))
                    structFlags &= ~TYPEINFO_STRUCT_EMPTY;

                if (varDecl->type && varDecl->type->specFlags & AstType::SPECFLAG_HAS_STRUCT_PARAMETERS)
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
                    SWAG_ASSERT(varDecl->semFlags & SEMFLAG_EXEC_RET_STACK);
                    structFlags |= TYPEINFO_STRUCT_HAS_INIT_VALUES;
                }
            }

            // Var is an array of structs
            else if (varTypeInfo->isArray() && !varDecl->assignment)
            {
                auto varTypeArray = CastTypeInfo<TypeInfoArray>(varTypeInfo, TypeInfoKind::Array);
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
            else if (varDecl->assignment && !(varDecl->flags & AST_EXPLICITLY_NOT_INITIALIZED))
            {
                SWAG_CHECK(checkIsConstExpr(context, varDecl->assignment->flags & AST_CONST_EXPR, varDecl->assignment, Err(Err0670)));

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
            if (!(node->flags & AST_IS_GENERIC))
            {
                if (varTypeInfo->isGeneric())
                {
                    if (varDecl->type)
                        child = varDecl->type;

                    if (!node->genericParameters)
                    {
                        Diagnostic diag{node, node->tokenName, Fmt(Err(Err0671), node->token.ctext(), varDecl->token.ctext())};
                        auto       note = Diagnostic::note(child, child->token, Diagnostic::isType(child->typeInfo));
                        return context->report(diag, note);
                    }

                    Diagnostic diag{node, node->tokenName, Fmt(Err(Err0672), node->token.ctext(), varDecl->token.ctext())};
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
                        return context->report({attr->node, attr->parameters[0].token, Fmt(Err(Err0673), forceOffset->text.c_str())});
                    }
                }
            }

            // Compute struct alignement
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
            if (!(child->specFlags & AstVarDecl::SPECFLAG_AUTO_NAME))
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
                if (!(node->flags & AST_GENERATED) && hasItemName)
                {
                    return context->report({child, child->token, Fmt(Err(Err0674), child->token.ctext())});
                }

                if (!hasItemName)
                {
                    auto  overload = child->resolvedSymbolOverload;
                    Utf8  name     = Fmt("item%u", storageIndexField);
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
    // We need to be sure that alignement will be respected, so we force "Swag.Align" attribute
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
    if ((node->attributeFlags & ATTRIBUTE_PUBLIC) && !(typeInfo->isTuple()))
    {
        if (!(node->flags & AST_FROM_GENERIC))
            node->ownerScope->addPublicNode(node);
    }

    // Need to recompute it if it's from generic
    if (node->flags & AST_FROM_GENERIC)
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
        node->flags &= ~AST_NO_BYTECODE;
        node->flags |= AST_NO_BYTECODE_CHILDS;

        SWAG_ASSERT(!node->hasExtByteCode() || !node->extByteCode()->byteCodeJob);
        node->allocateExtension(ExtensionKind::ByteCode);
        auto extension                       = node->extByteCode();
        extension->byteCodeJob               = Allocator::alloc<ByteCodeGenJob>();
        extension->byteCodeJob->sourceFile   = sourceFile;
        extension->byteCodeJob->module       = sourceFile->module;
        extension->byteCodeJob->dependentJob = context->baseJob->dependentJob;
        extension->byteCodeJob->nodes.push_back(node);
        node->byteCodeFct = ByteCodeGenJob::emitStruct;

        if (node->attributeFlags & ATTRIBUTE_GEN)
            node->resolvedSymbolName->addDependentJob(extension->byteCodeJob);
        else
            g_ThreadMgr.addJob(extension->byteCodeJob);
    }

    return true;
}