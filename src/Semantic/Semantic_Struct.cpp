#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenJob.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Parser/Parser.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"
#include "Wmf/ModuleManager.h"
#include "Wmf/Workspace.h"

bool Semantic::resolveImplForAfterFor(SemanticContext* context)
{
    const auto id   = context->node;
    const auto node = castAst<AstImpl>(context->node->parent, AstNodeKind::Impl);

    if (id->resolvedSymbolName()->isNot(SymbolKind::Struct))
        return context->report({id->lastChild(), formErr(Err0709, id->resolvedSymbolName()->name.cstr(), Naming::aKindName(id->resolvedSymbolName()->kind).cstr())});

    const auto structDecl = castAst<AstStruct>(id->resolvedSymbolOverload()->node, AstNodeKind::StructDecl);

    if (id->resolvedSymbolOverload()->hasFlag(OVERLOAD_GENERIC))
    {
        if (!node->hasAstFlag(AST_FROM_GENERIC))
        {
            const auto typeStruct = castTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
            node->token.sourceFile->module->decImplForToSolve(typeStruct);
        }

        return true;
    }

    if (structDecl->scope != node->structScope)
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
        ScopedLock lk1(typeStruct->mutex);
        typeStruct->cptRemainingInterfacesReg++;
        typeStruct->cptRemainingInterfaces++;
        node->token.sourceFile->module->decImplForToSolve(typeStruct);

        ScopedLock lk2(node->structScope->parentScope->mutex);
        ScopedLock lk3(structDecl->scope->mutex);

        node->structScope->parentScope->removeChildNoLock(node->structScope);
        for (const auto s : node->structScope->childrenScopes)
        {
            s->parentScope->removeChildNoLock(s);
            structDecl->scope->addChildNoLock(s);
        }
    }
    else
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(structDecl->typeInfo, TypeInfoKind::Struct);
        node->token.sourceFile->module->decImplForToSolve(typeStruct);
    }

    return true;
}

bool Semantic::resolveImplForType(SemanticContext* context)
{
    const auto node       = castAst<AstImpl>(context->node, AstNodeKind::Impl);
    const auto sourceFile = node->token.sourceFile;
    const auto module     = sourceFile->module;

    // Race condition in case a templated function is instantiated in the impl block during that access
    const auto nameItf    = node->secondChildSafe();
    const auto nameStruct = node->firstChildSafe();

    const auto typeStruct = castTypeInfo<TypeInfoStruct>(nameStruct->typeInfo, TypeInfoKind::Struct);

    if (node->identifierFor->typeInfo->isGeneric())
        return true;

    // Make a concrete type for the given struct
    auto& typeGen = module->typeGen;
    nameStruct->allocateComputedValue();
    nameStruct->computedValue()->storageSegment = getConstantSegFromContext(nameStruct);
    SWAG_CHECK(typeGen.genExportedTypeInfo(context, typeStruct, nameStruct->computedValue()->storageSegment, &nameStruct->computedValue()->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT));
    YIELD();

    // Make a concrete type for the given interface
    nameItf->allocateComputedValue();
    nameItf->computedValue()->storageSegment = getConstantSegFromContext(nameItf);
    SWAG_CHECK(typeGen.genExportedTypeInfo(context, nameItf->typeInfo, nameItf->computedValue()->storageSegment, &nameItf->computedValue()->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT));
    YIELD();

    const auto typeBaseInterface = castTypeInfo<TypeInfoStruct>(nameItf->typeInfo, TypeInfoKind::Interface);
    const auto typeParamItf      = typeStruct->hasInterface(typeBaseInterface);
    SWAG_ASSERT(typeParamItf);

    const auto constSegment = nameStruct->computedValue()->storageSegment;
    SWAG_ASSERT(typeParamItf->offset);

    // @itableHeader
    ScopedLock lk(typeStruct->mutex);

    {
        const auto addr     = constSegment->address(nameItf->computedValue()->storageOffset);
        const auto itfTable = reinterpret_cast<void**>(constSegment->address(typeParamItf->offset - 2 * sizeof(void*)));
        *itfTable           = addr;
        constSegment->addInitPtr(typeParamItf->offset - 2 * sizeof(void*), nameItf->computedValue()->storageOffset, constSegment->kind);
    }

    {
        const auto addr     = constSegment->address(nameStruct->computedValue()->storageOffset);
        const auto itfTable = reinterpret_cast<void**>(constSegment->address(typeParamItf->offset - sizeof(void*)));
        *itfTable           = addr;
        constSegment->addInitPtr(typeParamItf->offset - sizeof(void*), nameStruct->computedValue()->storageOffset, constSegment->kind);
    }

    return true;
}

bool Semantic::resolveImplFor(SemanticContext* context)
{
    const auto node = castAst<AstImpl>(context->node, AstNodeKind::Impl);
    const auto job  = context->baseJob;

    // Race condition in case a templated function is instantiated in the impl block during that access
    const auto first = node->firstChildSafe();
    const auto back  = node->secondChildSafe();

    // Be sure the first identifier is an interface
    auto typeInfo = node->identifier->typeInfo;
    if (!typeInfo->isInterface())
    {
        Diagnostic err{node->identifier, formErr(Err0325, node->identifier->token.cstr(), Naming::aKindName(typeInfo).cstr())};
        err.addNote(Diagnostic::hereIs(node->identifier->resolvedSymbolOverload()));
        return context->report(err);
    }

    // Be sure the second identifier is a struct
    typeInfo = node->identifierFor->typeInfo;
    SWAG_ASSERT(typeInfo->isStruct());

    VectorNative<AstNode*>& children = context->tmpNodes;
    context->tmpNodes.clear();
    flattenStructChildren(context, node, context->tmpNodes);

    SWAG_ASSERT(first->is(AstNodeKind::IdentifierRef));
    SWAG_ASSERT(back->is(AstNodeKind::IdentifierRef));
    const auto typeBaseInterface = castTypeInfo<TypeInfoStruct>(back->typeInfo, TypeInfoKind::Interface);
    const auto typeStruct        = castTypeInfo<TypeInfoStruct>(first->typeInfo, TypeInfoKind::Struct);

    // Be sure interface has been fully solved
    {
        ScopedLock lk(node->identifier->mutex);
        ScopedLock lk1(node->identifier->resolvedSymbolName()->mutex);
        if (node->identifier->resolvedSymbolName()->cptOverloads)
        {
            waitSymbolNoLock(job, node->identifier->resolvedSymbolName());
            return true;
        }
    }

    // We need now the pointer to the itable
    const auto     typeInterface   = castTypeInfo<TypeInfoStruct>(typeBaseInterface->itable, TypeInfoKind::Struct);
    const uint32_t numFctInterface = typeInterface->fields.size();

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

    for (const auto child : children)
    {
        if (child->isNot(AstNodeKind::FuncDecl))
            continue;
        const auto childFct = castAst<AstFuncDecl>(child, AstNodeKind::FuncDecl);

        // We need to search the function (as a variable) in the interface
        auto itfSymbol = typeInterface->findChildByNameNoLock(child->token.text); // O(n)!

        // Must be an 'impl' function
        if (!childFct->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL))
        {
            // Cannot have the same name as a function of the interface
            if (itfSymbol)
            {
                Diagnostic err{childFct, childFct->tokenName, formErr(Err0609, childFct->token.text.cstr(), typeInterface->name.cstr())};
                err.addNote("functions in an [[impl]] block cannot shadow interface functions");
                return context->report(err);
            }

            continue;
        }

        if (!itfSymbol)
        {
            Diagnostic err{childFct, childFct->tokenName, formErr(Err0092, childFct->token.text.cstr(), typeInterface->getDisplayNameC())};
            if (childFct->hasSpecFlag(AstFuncDecl::SPEC_FLAG_IMPL))
                err.addNote(childFct->implLoc, childFct->implLoc, "the function is supposed to be part of the interface due to [[impl]]");
            err.addNote(SemanticError::findClosestMatchesMsg(childFct->token.text, {{.scope = typeInterface->scope, .flags = 0}}, IdentifierSearchFor::Whatever));
            return context->report(err);
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

        if (typeInfo->isGeneric())
            continue;

        // We need to have a bytecode pointer to be able to reference it in the itable
        if (!child->isForeign())
        {
            ScopedLock lk(child->mutex);
            if (!child->hasExtByteCode() || !child->extByteCode()->bc)
            {
                child->allocateExtension(ExtensionKind::ByteCode);
                child->extByteCode()->bc             = Allocator::alloc<ByteCode>();
                child->extByteCode()->bc->node       = child;
                child->extByteCode()->bc->sourceFile = child->token.sourceFile;
            }

            child->extByteCode()->bc->isUsed          = true;
            child->extByteCode()->bc->isInDataSegment = true;
        }

        // Match function signature
        BadSignatureInfos bi;
        const auto        typeLambda = castTypeInfo<TypeInfoFuncAttr>(itfSymbol->typeInfo, TypeInfoKind::LambdaClosure);
        const auto        typeFunc   = castTypeInfo<TypeInfoFuncAttr>(child->typeInfo, TypeInfoKind::FuncAttr);
        const auto        nodeLambda = castAst<AstTypeExpression>(typeLambda->declNode, AstNodeKind::TypeLambda);
        if (!typeLambda->isSame(typeFunc, CAST_FLAG_EXACT | CAST_FLAG_INTERFACE, bi))
        {
            switch (bi.matchResult)
            {
                case MatchResult::BadSignature:
                {
                    Diagnostic err{childFct, childFct->getTokenName(), formErr(Err0503, typeBaseInterface->name.cstr(), child->token.cstr())};
                    err.addNote(childFct->parameters->children[bi.badSignatureNum2], form("parameter mismatch (type is [[%s]])", childFct->parameters->children[bi.badSignatureNum2]->typeInfo->getDisplayNameC()));
                    err.addNote(typeLambda->parameters[bi.badSignatureNum1]->declNode, form("the parameter should have type [[%s]]", typeLambda->parameters[bi.badSignatureNum1]->typeInfo->getDisplayNameC()));
                    return context->report(err);
                }

                case MatchResult::MissingReturnType:
                {
                    Diagnostic err{child, child->getTokenName(), formErr(Err0503, typeBaseInterface->name.cstr(), child->token.cstr())};
                    err.addNote("a return type is missing");
                    err.addNote(itfSymbol->declNode, itfSymbol->declNode->token, form("the interface declaration returns type [[%s]]", typeLambda->returnType->getDisplayNameC()));
                    return context->report(err);
                }

                case MatchResult::NoReturnType:
                {
                    Diagnostic err{childFct->returnType, formErr(Err0503, typeBaseInterface->name.cstr(), child->token.cstr())};
                    err.addNote(itfSymbol->declNode, itfSymbol->declNode->token, "the interface declaration yields no return");
                    return context->report(err);
                }

                case MatchResult::MismatchReturnType:
                {
                    Diagnostic err{childFct->returnType, formErr(Err0503, typeBaseInterface->name.cstr(), child->token.cstr())};
                    err.addNote(nodeLambda->returnType, form("the expected return type is [[%s]]", typeLambda->returnType->getDisplayNameC()));
                    return context->report(err);
                }

                case MatchResult::MismatchThrow:
                {
                    Diagnostic err{child, child->getTokenName(), formErr(Err0503, typeBaseInterface->name.cstr(), child->token.cstr())};
                    err.addNote("one function declares [[throw]], while the other does not");
                    const auto note   = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->getTokenName(), "this is the expected signature");
                    note->canBeMerged = false;
                    err.addNote(note);
                    return context->report(err);
                }

                default:
                {
                    Diagnostic err{child, child->getTokenName(), formErr(Err0503, typeBaseInterface->name.cstr(), child->token.cstr())};
                    const auto note   = Diagnostic::note(itfSymbol->declNode, itfSymbol->declNode->getTokenName(), "this is the expected signature");
                    note->canBeMerged = false;
                    err.addNote(note);
                    return context->report(err);
                }
            }
        }

        // use resolvedUserOpSymbolOverload to store the match
        mapItToFunc[itfSymbol]           = child;
        mapItIdxToFunc[itfSymbol->index] = castAst<AstFuncDecl>(child);
    }

    // If the structure is generic, then do nothing; we cannot solve
    if (typeInfo->isGeneric())
    {
        decreaseInterfaceCount(typeStruct);
        return true;
    }

    // Be sure every function of the interface has been covered. If it's not the case,
    // we generate the missing parts.
    SWAG_CHECK(Ast::generateMissingInterfaceFct(context, mapItIdxToFunc, typeStruct, typeBaseInterface, typeInterface));
    YIELD();

    // Construct itable in the constant segment
    // - Header of one pointer (the interface type)
    // - Header of one pointer (the object type)
    // - Then one pointer for interface functions
    // - One more void*
    // We have at least one room for a function, even if the interface is empty
    // @itableHeader
    const auto     constSegment = getConstantSegFromContext(node);
    void**         ptrITable;
    constexpr int  sizeOfHeader = 2 * sizeof(void*);
    const uint32_t itableOffset = constSegment->reserve(sizeOfHeader + (std::max(numFctInterface, static_cast<uint32_t>(1)) * sizeof(void*)), reinterpret_cast<uint8_t**>(&ptrITable), sizeof(void*));
    auto           offset       = itableOffset;

    // @itableHeader
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

            // Be sure to never have a null value in the constant segment, for sanity checks.02
            // This will be filled when the module will be loaded, with the real function address
            *reinterpret_cast<uint64_t*>(ptrITable) = DataSegment::FAKE_PTR;

            g_ModuleMgr->addPatchFuncAddress(constSegment, reinterpret_cast<void**>(constSegment->address(offset)), funcChild);
        }
        else
        {
            funcChild->extByteCode()->bc->isUsed          = true;
            funcChild->extByteCode()->bc->isInDataSegment = true;

            *ptrITable = ByteCode::doByteCodeLambda(funcChild->extByteCode()->bc);
            constSegment->addInitPtrFunc(offset, funcChild->getCallName());
        }

        ptrITable++;
        offset += sizeof(void*);
    }

    // Check public
    if (!numFctInterface)
    {
        SWAG_CHECK(collectAttributes(context, first, nullptr));
        SWAG_CHECK(collectAttributes(context, back, nullptr));
        if (first->hasAttribute(ATTRIBUTE_PUBLIC) && back->hasAttribute(ATTRIBUTE_PUBLIC))
            node->ownerScope->addPublicNode(node);
    }

    // @ItfIsConstantSeg
    // Setup constant segment offset. We put the offset to the start of the functions, not to the concrete type offset (0)
    typeParamItf->offset = itableOffset + sizeOfHeader;
    decreaseInterfaceCount(typeStruct);

    return true;
}

bool Semantic::resolveInterface(SemanticContext* context)
{
    const auto node          = castAst<AstStruct>(context->node, AstNodeKind::InterfaceDecl);
    const auto sourceFile    = context->sourceFile;
    const auto typeInterface = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Interface);

    typeInterface->declNode   = node;
    typeInterface->name       = node->token.text;
    typeInterface->structName = node->token.text;

    uint32_t storageOffset = 0;
    uint32_t storageIndex  = 0;

    VectorNative<AstNode*>& children = node->hasAstFlag(AST_STRUCT_COMPOUND) ? context->tmpNodes : node->content->children;
    if (node->hasAstFlag(AST_STRUCT_COMPOUND))
    {
        context->tmpNodes.clear();
        flattenStructChildren(context, node->content, context->tmpNodes);
    }

    // itable
    const auto typeITable  = makeType<TypeInfoStruct>();
    typeITable->name       = node->token.text;
    typeITable->structName = node->token.text;
    typeITable->scope      = Ast::newScope(node, node->token.text, ScopeKind::Struct, nullptr);
    typeITable->addFlag(TYPEINFO_STRUCT_IS_ITABLE);

    for (auto child : children)
    {
        if (child->isNot(AstNodeKind::VarDecl))
            continue;

        const auto varDecl = castAst<AstVarDecl>(child, AstNodeKind::VarDecl);

        TypeInfoParam* typeParam = nullptr;
        if (!node->hasAstFlag(AST_FROM_GENERIC))
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
                const Diagnostic err{varDecl->type, formErr(Err0614, child->token.cstr())};
                return context->report(err);
            }

            const auto firstParamType = typeLambda->parameters[0]->typeInfo;
            SWAG_VERIFY(firstParamType->isPointer(), context->report({typeLambda->parameters[0]->declNode, formErr(Err0427, firstParamType->getDisplayNameC())}));
            const auto firstParamPtr = castTypeInfo<TypeInfoPointer>(firstParamType, TypeInfoKind::Pointer);
            SWAG_VERIFY(firstParamPtr->pointedType == typeInterface, context->report({typeLambda->parameters[0]->declNode, formErr(Err0427, firstParamType->getDisplayNameC())}));
        }

        typeParam           = typeITable->fields[storageIndex];
        typeParam->typeInfo = child->typeInfo;
        typeParam->declNode = child;
        typeParam->index    = storageIndex;

        if (!node->hasAstFlag(AST_GENERIC))
        {
            SWAG_VERIFY(!child->typeInfo->isGeneric(), context->report({child, formErr(Err0214, node->token.cstr(), child->typeInfo->getDisplayNameC())}));
        }

        if (typeParam->attributes.hasAttribute(g_LangSpec->name_Swag_Offset))
        {
            const auto attr = typeParam->attributes.getAttribute(g_LangSpec->name_Swag_Offset);
            SWAG_ASSERT(attr);
            return context->report({attr->node, attr->node->token, toErr(Err0250)});
        }

        typeParam->offset                                            = storageOffset;
        child->resolvedSymbolOverload()->computedValue.storageOffset = storageOffset;
        child->resolvedSymbolOverload()->storageIndex                = storageIndex;

        SWAG_ASSERT(child->typeInfo->sizeOf == sizeof(void*));
        typeITable->sizeOf += sizeof(void*);

        // Register lambda variable in the scope of the itable struct
        const auto        overload = child->resolvedSymbolOverload();
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

    typeInterface->itable = typeITable;

    // Struct interface, with one pointer for the data, and one pointer for itable
    if (!node->hasAstFlag(AST_FROM_GENERIC))
    {
        auto typeParam      = TypeManager::makeParam();
        typeParam->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        typeInterface->fields.push_back(typeParam);
        typeInterface->sizeOf += sizeof(void*);

        typeParam           = TypeManager::makeParam();
        typeParam->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        typeParam->index    = 1;
        typeParam->offset   = sizeof(void*);
        typeInterface->fields.push_back(typeParam);
        typeInterface->sizeOf += sizeof(void*);
    }

    // Check public
    if (node->hasAttribute(ATTRIBUTE_PUBLIC) && !node->hasAstFlag(AST_FROM_GENERIC))
        node->ownerScope->addPublicNode(node);

    // Register symbol with its type
    node->typeInfo = typeInterface;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::Interface;

    // We are parsing the swag module
    if (sourceFile->hasFlag(FILE_BOOTSTRAP))
        g_Workspace->swagScope.registerType(node->typeInfo);

    // @BecauseOfThat
    ScopedLock lk(node->mutex);
    node->addSemFlag(SEMFLAG_PRE_RESOLVE);
    node->setResolvedSymbolOverload(node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd));
    SWAG_CHECK(node->resolvedSymbolOverload());
    node->dependentJobs.setRunning();

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
    // impl scope and corresponding identifier scope must be the same!
    if (scopeImpl != scope)
    {
        Diagnostic err{node, node->token, formErr(Err0217, node->token.cstr())};
        err.addNote(Diagnostic::hereIs(node->identifier->resolvedSymbolOverload()));

        auto curName = scopeImpl->parentScope->getFullName();
        if (curName.empty() && scopeImpl->parentScope->is(ScopeKind::Module))
            curName = context->sourceFile->module->nameNormalized;

        auto parentName = scope->parentScope->getFullName();
        if (parentName.empty() && scope->parentScope->is(ScopeKind::Module))
            parentName = context->sourceFile->module->nameNormalized;

        err.addNote(form("[[impl]] has [[%s]] as parent scope, but [[%s]] has [[%s]]", curName.cstr(), node->token.cstr(), parentName.cstr()));
        return context->report(err);
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
        Diagnostic err{node->identifier, formErr(Err0326, node->identifier->token.cstr(), typeInfo->getDisplayNameC())};
        err.addNote(Diagnostic::hereIs(node->identifier->resolvedSymbolOverload()));
        return context->report(err);
    }

    const auto typeIdentifier = node->identifier->resolvedSymbolOverload()->typeInfo;
    SWAG_VERIFY(!typeIdentifier->isAlias(), context->report({node->identifier, toErr(Err0322)}));

    switch (typeInfo->kind)
    {
        case TypeInfoKind::Struct:
        {
            const auto structNode = castAst<AstStruct>(node->identifier->resolvedSymbolOverload()->node, AstNodeKind::StructDecl);
            SWAG_CHECK(checkImplScopes(context, node, node->structScope, structNode->scope));
            break;
        }
        case TypeInfoKind::Enum:
        {
            const auto enumNode = castAst<AstEnum>(node->identifier->resolvedSymbolOverload()->node, AstNodeKind::EnumDecl);
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

    // The generated struct could need the user attributes of the original struct or function where it belongs.
    // So we must be sure that the original parent has collected its attributes.
    if (parent->ownerStructScope && !parent->ownerStructScope->is(ScopeKind::Impl))
    {
        const auto parentStruct = castAst<AstStruct>(parent->ownerStructScope->owner, AstNodeKind::StructDecl, AstNodeKind::InterfaceDecl);
        ScopedLock lk(parentStruct->mutex);
        if (!parentStruct->hasSemFlag(SEMFLAG_PRE_RESOLVE))
        {
            parentStruct->dependentJobs.add(context->baseJob);
            context->baseJob->setPending(JobWaitKind::WaitPreResolve, parentStruct->resolvedSymbolName(), parentStruct, nullptr);
            return true;
        }
    }

    if (parent->ownerFct)
    {
        const auto parentFunc = castAst<AstFuncDecl>(parent->ownerFct, AstNodeKind::FuncDecl);
        ScopedLock lk(parentFunc->mutex);
        if (!parentFunc->hasSemFlag(SEMFLAG_PRE_RESOLVE))
        {
            parentFunc->dependentJobs.add(context->baseJob);
            context->baseJob->setPending(JobWaitKind::WaitPreResolve, parentFunc->resolvedSymbolName(), parentFunc, nullptr);
            return true;
        }
    }

    // We convert the {...} expression to a structure. As the structure can contain generic parameters,
    // we need to copy them. But from the function or the structure?
    // For now, we give the priority to the generic parameters from the function, if there are any
    // But this will not work in all cases
    if (!structNode->genericParameters)
    {
        if (parent->ownerFct)
        {
            const auto parentFunc = castAst<AstFuncDecl>(parent->ownerFct, AstNodeKind::FuncDecl);
            if (parentFunc->genericParameters)
                structNode->genericParameters = Ast::clone(parentFunc->genericParameters, nullptr, AST_GENERATED_GENERIC_PARAM);
        }

        if (parent->ownerStructScope)
        {
            const auto parentStruct = castAst<AstStruct>(parent->ownerStructScope->owner);
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
                n->addAstFlag(AST_GENERIC);
            });
        }
    }

    return true;
}

bool Semantic::preResolveStructContent(SemanticContext* context)
{
    const auto node = castAst<AstStruct>(context->node->parent);
    SWAG_ASSERT(node->is(AstNodeKind::StructDecl) || node->is(AstNodeKind::InterfaceDecl));

    // where
    if (!node->constraints.empty() && !node->hasAstFlag(AST_GENERIC))
    {
        SWAG_CHECK(solveConstraints(context, node));
        YIELD();
    }

    const auto typeInfo = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    SWAG_CHECK(collectAttributes(context, node, &typeInfo->attributes));

    if (node->hasAttribute(ATTRIBUTE_NO_COPY))
        typeInfo->addFlag(TYPEINFO_STRUCT_NO_COPY);

    typeInfo->declNode = node;
    node->scope->owner = node;

    // Add generic parameters
    OverloadFlags overFlags = 0;
    if (!node->hasAstFlag(AST_FROM_GENERIC))
    {
        if (node->genericParameters)
        {
            for (const auto param : node->genericParameters->children)
            {
                auto funcParam      = TypeManager::makeParam();
                funcParam->name     = param->token.text;
                funcParam->typeInfo = param->typeInfo;
                if (param->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_TYPE))
                    funcParam->flags.add(TYPEINFOPARAM_GENERIC_TYPE);
                else if (param->hasSpecFlag(AstVarDecl::SPEC_FLAG_GENERIC_CONSTANT))
                    funcParam->flags.add(TYPEINFOPARAM_GENERIC_CONSTANT);
                typeInfo->genericParameters.push_back(funcParam);
                typeInfo->sizeOf += param->typeInfo->sizeOf;
            }

            node->addAstFlag(AST_GENERIC);
            typeInfo->addFlag(TYPEINFO_GENERIC);
            overFlags.add(OVERLOAD_GENERIC);
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
        node->resolvedSymbolName()->flags.add(SYMBOL_ATTRIBUTE_GEN);

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = symbolKind;
    toAdd.flags    = overFlags | OVERLOAD_INCOMPLETE;

    // @BecauseOfThat
    ScopedLock lk(node->mutex);
    node->addSemFlag(SEMFLAG_PRE_RESOLVE);
    node->setResolvedSymbol(toAdd.symbolName, node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd));
    SWAG_CHECK(node->resolvedSymbolOverload());
    node->dependentJobs.setRunning();

    return true;
}

void Semantic::flattenStructChildren(SemanticContext* context, AstNode* parent, VectorNative<AstNode*>& result)
{
    SharedLock lock(parent->mutex);
    for (auto child : parent->children)
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
                flattenStructChildren(context, child, result);
                continue;

            case AstNodeKind::CompilerIf:
            {
                const AstIf* compilerIf = castAst<AstIf>(child, AstNodeKind::CompilerIf);
                if (!compilerIf->ifBlock->hasAstFlag(AST_NO_SEMANTIC))
                    flattenStructChildren(context, compilerIf->ifBlock, result);
                else if (compilerIf->elseBlock)
                    flattenStructChildren(context, compilerIf->elseBlock, result);
                continue;
            }

            case AstNodeKind::AttrUse:
            {
                const AstAttrUse* attrUse = castAst<AstAttrUse>(child, AstNodeKind::AttrUse);
                if (attrUse->content->is(AstNodeKind::Statement))
                    flattenStructChildren(context, attrUse->content, result);
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

bool Semantic::solveConstraints(SemanticContext* context, AstStruct* structDecl)
{
    ScopedLock lk1(structDecl->mutex);

    // Execute where constraints
    for (const auto it : structDecl->constraints)
    {
        const auto expr = it->lastChild();

        if (!expr->hasFlagComputedValue())
        {
            const auto node          = context->node;
            context->whereParameters = structDecl->genericParameters;

            PushErrCxtStep ec(context, node, ErrCxtStepKind::DuringWhere, nullptr);
            const auto     result    = executeCompilerNode(context, expr, false);
            context->whereParameters = nullptr;
            if (!result)
                return false;
            YIELD();
        }

        // Result
        SWAG_ASSERT(expr->hasComputedValue());
        if (!expr->computedValue()->reg.b)
        {
            ErrorParam                errorParam;
            Vector<const Diagnostic*> diagError;
            Vector<const Diagnostic*> diagNote;

            errorParam.destStructDecl = structDecl;
            errorParam.diagError      = &diagError;
            errorParam.diagNote       = &diagNote;
            errorParam.errorNode      = structDecl->instantiatedFrom;

            SemanticError::errorConstraintFailed(context, errorParam, it);

            return context->report(*diagError[0], diagNote);
        }
    }

    return true;
}

bool Semantic::postResolveStruct(SemanticContext* context)
{
    if (context->node->isNot(AstNodeKind::StructDecl))
        return true;

    const auto node     = castAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    const auto typeInfo = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);

    waitStructUsing(context->baseJob, typeInfo);
    YIELD();

    sendCompilerMsgTypeDecl(context);

    // Verify that we have only one possible conversion
    {
        SharedLock lk(typeInfo->mutex);

        VectorNative<std::pair<TypeInfoParam*, uint32_t>> usingFields;
        typeInfo->collectUsingFields(usingFields);
        VectorNative<TypeInfoStruct*> here;
        VectorNative<AstNode*>        where;
        for (const auto key : usingFields | std::views::keys)
        {
            const auto     typeStruct = key->typeInfo->getStructOrPointedStruct();
            const uint32_t idx        = here.find(typeStruct);
            if (idx != UINT32_MAX)
            {
                Diagnostic err(key->declNode, formErr(Err0349, typeStruct->getDisplayNameC()));
                err.addNote(Diagnostic::hereIs(where[idx], "this is the other definition"));
                err.addNote(form("this makes the conversion from [[%s]] to [[%s]] ambiguous", typeInfo->getDisplayNameC(), typeStruct->getDisplayNameC()));
                return context->report(err);
            }

            here.push_back(typeStruct);
            where.push_back(key->declNode);
        }
    }

    // Generate all functions associated with a struct
    const auto sourceFile = context->sourceFile;
    if (!typeInfo->isGeneric())
    {
        node->removeAstFlag(AST_NO_BYTECODE);
        node->addAstFlag(AST_NO_BYTECODE_CHILDREN);
        SWAG_ASSERT(!node->hasExtByteCode() || !node->extByteCode()->byteCodeJob);

        node->allocateExtension(ExtensionKind::ByteCode);
        const auto extByteCode   = node->extByteCode();
        extByteCode->byteCodeJob = ByteCodeGenJob::newJob(context->baseJob->dependentJob, sourceFile, node);
        node->byteCodeFct        = ByteCodeGen::emitStruct;

        if (node->hasAttribute(ATTRIBUTE_GEN))
            node->resolvedSymbolName()->addDependentJob(extByteCode->byteCodeJob);
        else
            g_ThreadMgr.addJob(extByteCode->byteCodeJob);
    }

    return true;
}

bool Semantic::resolveStruct(SemanticContext* context)
{
    auto node       = castAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto sourceFile = context->sourceFile;
    auto typeInfo   = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    auto job        = context->baseJob;

    {
        ScopedLock lk1(node->mutex);
        node->addSemFlag(SEMFLAG_STRUCT_START_SOLVE);
        node->dependentJobs.setRunning();
    }

    SWAG_ASSERT(typeInfo->declNode);

    // Structure packing
    if (node->hasSpecFlag(AstStruct::SPEC_FLAG_UNION))
        node->packing = 0;
    else
    {
        if (auto value    = typeInfo->attributes.getValue(g_LangSpec->name_Swag_Pack, g_LangSpec->name_value))
            node->packing = value->reg.u8;
    }

    // Check 'opaque' attribute
    if (!sourceFile->hasFlag(FILE_GENERATED))
    {
        if (node->hasAttribute(ATTRIBUTE_OPAQUE))
        {
            SWAG_VERIFY(!sourceFile->hasFlag(FILE_FORCE_EXPORT), context->report({node, node->token, toErr(Err0218)}));
            SWAG_VERIFY(node->isPublic(), context->report({node, node->token, toErr(Err0618)}));
        }
    }

    // Be default, a tuple is 'constexpr'
    // If one of the children is not, then the attribute will be removed
    if (typeInfo->isTuple())
        node->addAttribute(ATTRIBUTE_CONSTEXPR);

    TypeInfoFlags structFlags = TYPEINFO_STRUCT_ALL_UNINITIALIZED | TYPEINFO_STRUCT_EMPTY;

    // No need to flatten structure if it's not a compound (optim)
    VectorNative<AstNode*>& children = node->hasAstFlag(AST_STRUCT_COMPOUND) ? context->tmpNodes : node->content->children;
    if (node->hasAstFlag(AST_STRUCT_COMPOUND))
    {
        context->tmpNodes.clear();
        flattenStructChildren(context, node->content, context->tmpNodes);
    }

    typeInfo->alignOf = 0;
    typeInfo->sizeOf  = 0;

    // If one of my children is incomplete, then we must wait
    for (auto child : children)
    {
        // Waiting for myself!
        if (child->typeInfo == typeInfo)
        {
            Diagnostic err{node, node->getTokenName(), formErr(Err0064, node->token.cstr())};
            err.addNote(child, "this is the field causing the recursion");
            return context->report(err);
        }

        waitTypeCompleted(job, child->typeInfo);
        if (context->result != ContextResult::Done)
        {
            job->waitingHintNode = child;
            return true;
        }
    }

    {
        uint32_t storageOffset     = 0;
        uint32_t storageIndexField = 0;
        uint32_t storageIndexConst = 0;

        SWAG_RACE_CONDITION_WRITE(typeInfo->raceFields);
        for (auto child : children)
        {
            if (child->isNot(AstNodeKind::VarDecl) && child->isNot(AstNodeKind::ConstDecl))
                continue;

            auto varDecl = castAst<AstVarDecl>(child, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);

            TypeInfoParam* typeParam = nullptr;
            if (!node->hasAstFlag(AST_FROM_GENERIC) || !child->hasAstFlag(AST_REGISTERED_IN_STRUCT))
            {
                typeParam           = TypeManager::makeParam();
                typeParam->name     = child->token.text;
                typeParam->typeInfo = child->typeInfo;
                typeParam->offset   = storageOffset;
                if (varDecl->hasAstFlag(AST_DECL_USING))
                {
                    typeParam->flags.add(TYPEINFOPARAM_HAS_USING);
                    typeInfo->flags.add(TYPEINFO_HAS_USING);
                }

                SWAG_CHECK(collectAttributes(context, child, &typeParam->attributes));
                if (child->is(AstNodeKind::VarDecl))
                    typeInfo->fields.push_back(typeParam);
                else
                    typeInfo->constDecl.push_back(typeParam);
            }

            child->addAstFlag(AST_REGISTERED_IN_STRUCT);
            if (child->is(AstNodeKind::VarDecl))
                typeParam = typeInfo->fields[storageIndexField];
            else
                typeParam = typeInfo->constDecl[storageIndexConst];
            typeParam->typeInfo = child->typeInfo;
            typeParam->declNode = child;

            if (child->isNot(AstNodeKind::VarDecl))
            {
                storageIndexConst++;
                continue;
            }

            typeParam->index = storageIndexField;
            auto varTypeInfo = varDecl->typeInfo;

            // If variable is not empty, then struct is not
            if (!varTypeInfo->isStruct() && !varTypeInfo->isArrayOfStruct())
                structFlags.remove(TYPEINFO_STRUCT_EMPTY);

            // If variable is initialized, struct is too.
            if (!varDecl->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
            {
                if (varDecl->assignment || varDecl->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS))
                    structFlags.remove(TYPEINFO_STRUCT_ALL_UNINITIALIZED);
                else if (!varTypeInfo->isStruct() && !varTypeInfo->isArrayOfStruct())
                    structFlags.remove(TYPEINFO_STRUCT_ALL_UNINITIALIZED);
            }

            // Inherit flags
            if (varTypeInfo->hasFlag(TYPEINFO_STRUCT_NO_COPY))
                structFlags.add(TYPEINFO_STRUCT_NO_COPY);

            // Remove attribute constexpr if necessary
            if (child->typeInfo->isStruct() && !child->typeInfo->declNode->hasAttribute(ATTRIBUTE_CONSTEXPR))
                node->removeAttribute(ATTRIBUTE_CONSTEXPR);

            // Var is a struct
            if (varTypeInfo->isStruct())
            {
                if (varTypeInfo->declNode->hasAttribute(ATTRIBUTE_EXPORT_TYPE_NO_ZERO))
                    structFlags.add(TYPEINFO_STRUCT_HAS_INIT_VALUES);
                structFlags.add(varTypeInfo->flags.mask(TYPEINFO_STRUCT_HAS_INIT_VALUES));

                if (!varTypeInfo->hasFlag(TYPEINFO_STRUCT_ALL_UNINITIALIZED))
                    structFlags.remove(TYPEINFO_STRUCT_ALL_UNINITIALIZED);
                if (!varTypeInfo->hasFlag(TYPEINFO_STRUCT_EMPTY))
                    structFlags.remove(TYPEINFO_STRUCT_EMPTY);

                if (varDecl->type && varDecl->type->hasSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS))
                {
                    structFlags.add(TYPEINFO_STRUCT_HAS_INIT_VALUES);
                    if (!varDecl->type->hasFlagComputedValue())
                    {
                        auto constSegment = getConstantSegFromContext(varDecl);
                        varDecl->type->setFlagsValueIsComputed();
                        varDecl->type->computedValue()->storageSegment = constSegment;
                        SWAG_CHECK(collectAssignment(context, constSegment, varDecl->type->computedValue()->storageOffset, varDecl));
                    }
                }

                // @opAffectConstExpr
                // Collect has already been simulated with an opAffect
                else if (varDecl->assignment)
                {
                    SWAG_ASSERT(varDecl->hasSemFlag(SEMFLAG_EXEC_RET_STACK));
                    structFlags.add(TYPEINFO_STRUCT_HAS_INIT_VALUES);
                }
            }

            // Var is an array of structs
            else if (varTypeInfo->isArray() && !varDecl->assignment)
            {
                auto varTypeArray = castTypeInfo<TypeInfoArray>(varTypeInfo, TypeInfoKind::Array);
                if (varTypeArray->pointedType->isStruct())
                {
                    structFlags.add(varTypeArray->pointedType->flags.mask(TYPEINFO_STRUCT_HAS_INIT_VALUES));
                    if (!varTypeArray->pointedType->hasFlag(TYPEINFO_STRUCT_ALL_UNINITIALIZED))
                        structFlags.remove(TYPEINFO_STRUCT_ALL_UNINITIALIZED);
                    if (!varTypeArray->pointedType->hasFlag(TYPEINFO_STRUCT_EMPTY))
                        structFlags.remove(TYPEINFO_STRUCT_EMPTY);
                }
            }

            // Var has an initialization
            else if (varDecl->assignment && !varDecl->hasAstFlag(AST_EXPLICITLY_NOT_INITIALIZED))
            {
                SWAG_CHECK(checkIsConstExpr(context, varDecl->assignment->hasAstFlag(AST_CONST_EXPR), varDecl->assignment, toErr(Err0442)));

                auto typeInfoAssignment = varDecl->assignment->typeInfo->getConcreteAlias();
                typeInfoAssignment      = TypeManager::concreteType(varDecl->assignment->typeInfo, CONCRETE_ENUM);

                if (typeInfoAssignment->isString())
                {
                    structFlags.add(TYPEINFO_STRUCT_HAS_INIT_VALUES);
                    if (typeParam)
                    {
                        typeParam->allocateComputedValue();
                        typeParam->value->reg = varDecl->assignment->computedValue()->reg;
                    }
                }
                else if (!typeInfoAssignment->isNative() || varDecl->assignment->computedValue()->reg.u64)
                {
                    structFlags.add(TYPEINFO_STRUCT_HAS_INIT_VALUES);
                    if (typeParam)
                    {
                        typeParam->allocateComputedValue();
                        typeParam->value->reg = varDecl->assignment->computedValue()->reg;
                    }
                }

                structFlags.remove(TYPEINFO_STRUCT_ALL_UNINITIALIZED);
            }

            // If the struct is not generic, be sure that a field is not generic either
            if (!node->hasAstFlag(AST_GENERIC))
            {
                if (varTypeInfo->isGeneric())
                {
                    if (varDecl->type)
                        child = varDecl->type;

                    Diagnostic err{node, node->getTokenName(), formErr(Err0754, node->token.cstr(), varDecl->token.cstr())};
                    err.addNote(Diagnostic::isType(child));
                    return context->report(err);
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
                        return context->report({attr->node, attr->parameters[0].token, formErr(Err0104, forceOffset->text.cstr())});
                    }
                }
            }

            // Compute struct alignment
            auto alignOf      = TypeManager::alignOf(child->typeInfo);
            typeInfo->alignOf = std::max(typeInfo->alignOf, alignOf);

            // Compute padding before the current field
            if (!relocated && node->packing > 1 && alignOf)
            {
                auto userAlignOf = typeParam->attributes.getValue(g_LangSpec->name_Swag_Align, g_LangSpec->name_value);
                if (userAlignOf)
                    alignOf = userAlignOf->reg.u8;
                else
                    alignOf = std::min(alignOf, node->packing);
                realStorageOffset = static_cast<uint32_t>(TypeManager::align(realStorageOffset, alignOf));
                storageOffset     = static_cast<uint32_t>(TypeManager::align(storageOffset, alignOf));
            }

            typeParam->offset                                            = realStorageOffset;
            child->resolvedSymbolOverload()->computedValue.storageOffset = realStorageOffset;
            child->resolvedSymbolOverload()->storageIndex                = storageIndexField;

            auto childType   = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);
            typeInfo->sizeOf = std::max(typeInfo->sizeOf, realStorageOffset + childType->sizeOf);

            if (relocated)
                storageOffset = std::max(storageOffset, realStorageOffset + childType->sizeOf);
            else if (node->packing)
                storageOffset += childType->sizeOf;

            // Special field name starts with 'item' followed by a number
            if (!Parser::isItemName(child->token.text))
            {
                auto  overload = child->resolvedSymbolOverload();
                Utf8  varName  = form("item%u", storageIndexField);
                auto& symTable = node->scope->symTable;

                AddSymbolTypeInfo toAdd;
                toAdd.node           = child;
                toAdd.typeInfo       = child->typeInfo;
                toAdd.kind           = SymbolKind::Variable;
                toAdd.flags          = overload->flags;
                toAdd.storageOffset  = overload->computedValue.storageOffset;
                toAdd.storageSegment = overload->computedValue.storageSegment;
                toAdd.aliasName      = varName;

                symTable.addSymbolTypeInfo(context, toAdd);
            }
            else
            {
                typeParam->flags.add(TYPEINFOPARAM_AUTO_NAME);
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
        typeInfo->alignOf = std::min(typeInfo->alignOf, node->packing);
    typeInfo->alignOf = std::max(static_cast<uint32_t>(1), typeInfo->alignOf);

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
        typeInfo->sizeOf = static_cast<uint32_t>(TypeManager::align(typeInfo->sizeOf, typeInfo->alignOf));

    // Check public
    if (node->hasAttribute(ATTRIBUTE_PUBLIC) && !typeInfo->isTuple())
    {
        if (!node->hasAstFlag(AST_FROM_GENERIC))
            node->ownerScope->addPublicNode(node);
    }

    // Need to recompute it if it's from generic
    if (node->hasAstFlag(AST_FROM_GENERIC))
    {
        typeInfo->removeFlag(TYPEINFO_STRUCT_ALL_UNINITIALIZED);
        typeInfo->removeFlag(TYPEINFO_STRUCT_HAS_INIT_VALUES);
        typeInfo->removeFlag(TYPEINFO_STRUCT_EMPTY);
    }

    typeInfo->addFlag(structFlags.mask(TYPEINFO_STRUCT_ALL_UNINITIALIZED));
    typeInfo->addFlag(structFlags.mask(TYPEINFO_STRUCT_EMPTY));
    typeInfo->addFlag(structFlags.mask(TYPEINFO_STRUCT_HAS_INIT_VALUES));
    typeInfo->addFlag(structFlags.mask(TYPEINFO_STRUCT_NO_COPY));

    // Register symbol with its type
    node->typeInfo = typeInfo;

    AddSymbolTypeInfo toAdd;
    toAdd.node     = node;
    toAdd.typeInfo = node->typeInfo;
    toAdd.kind     = SymbolKind::Struct;

    {
        // @BecauseOfThat
        ScopedLock lk(node->mutex);
        node->addSemFlag(SEMFLAG_PRE_RESOLVE);
        auto newOver = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
        SWAG_CHECK(newOver);
        SWAG_ASSERT(newOver == node->resolvedSymbolOverload());
        node->dependentJobs.setRunning();
    }

    // We are parsing the swag module
    if (sourceFile->hasFlag(FILE_BOOTSTRAP))
        g_Workspace->swagScope.registerType(node->typeInfo);

    return true;
}
