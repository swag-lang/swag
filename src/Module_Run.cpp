#include "pch.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeStack.h"
#include "TypeManager.h"
#include "Context.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "ErrorIds.h"

bool Module::computeExecuteResult(ByteCodeRunContext* runContext, SourceFile* sourceFile, AstNode* node, JobContext* callerContext, ExecuteNodeParams* params)
{
    if (params && params->forDebugger)
    {
        if (!node->resultRegisterRC.size())
            return true;
        runContext->registersRR[0].pointer = runContext->registers.buffer[node->resultRegisterRC[0]].pointer;
        if (node->resultRegisterRC.size() > 1)
            runContext->registersRR[1].pointer = runContext->registers.buffer[node->resultRegisterRC[1]].pointer;
        return true;
    }

    // :CheckConstExprFuncReturnType
    // :opAffectConstExpr
    // Result is on the stack. Store it in the compiler segment.
    if (node->semFlags & AST_SEM_EXEC_RET_STACK)
    {
        auto     storageSegment = &sourceFile->module->compilerSegment;
        uint8_t* addrDst;
        auto     storageOffset = storageSegment->reserve(node->typeInfo->sizeOf, &addrDst);
        node->allocateComputedValue();
        node->computedValue->storageSegment = storageSegment;
        node->computedValue->storageOffset  = storageOffset;
        auto addrSrc                        = runContext->bp;
        memcpy(addrDst, addrSrc, node->typeInfo->sizeOf);
        return true;
    }

    // Transform result to a literal value
    if (!node->resultRegisterRC.size())
        return true;

    auto realType  = TypeManager::concreteType(node->typeInfo);
    node->typeInfo = TypeManager::concreteType(node->typeInfo, CONCRETE_FUNC);
    node->setFlagsValueIsComputed();

    runContext->registersRR[0].u64 = runContext->registers.buffer[node->resultRegisterRC[0]].u64;
    if (node->resultRegisterRC.size() > 1)
        runContext->registersRR[1].u64 = runContext->registers.buffer[node->resultRegisterRC[1]].u64;

    // String
    if (realType->isString())
    {
        SWAG_ASSERT(node->resultRegisterRC.size() == 2);
        const char* pz  = (const char*) runContext->registersRR[0].pointer;
        uint32_t    len = runContext->registersRR[1].u32;
        node->computedValue->text.reserve(len + 1);
        node->computedValue->text.count = len;
        memcpy(node->computedValue->text.buffer, pz, len);
        node->computedValue->text.buffer[len] = 0;
        return true;
    }

    // Static array
    if (realType->isArray())
    {
        auto     storageSegment             = SemanticJob::getConstantSegFromContext(node);
        uint8_t* addrDst                    = nullptr;
        auto     offsetStorage              = storageSegment->reserve(realType->sizeOf, &addrDst);
        node->computedValue->storageOffset  = offsetStorage;
        node->computedValue->storageSegment = storageSegment;
        auto addrSrc                        = runContext->registersRR[0].pointer;
        memcpy(addrDst, (const void*) addrSrc, realType->sizeOf);
        return true;
    }

    // Struct return
    if (realType->isStruct())
    {
        // If struct is makred as constexpr, then we can raw copy the slice content
        if (realType->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        {
            auto     storageSegment             = SemanticJob::getConstantSegFromContext(node);
            uint8_t* addrDst                    = nullptr;
            auto     offsetStorage              = storageSegment->reserve(realType->sizeOf, &addrDst);
            node->computedValue->storageOffset  = offsetStorage;
            node->computedValue->storageSegment = storageSegment;
            auto addrSrc                        = runContext->registersRR[0].pointer;
            memcpy(addrDst, (const void*) addrSrc, realType->sizeOf);
            return true;
        }

        // Convert struct to static array
        ExecuteNodeParams opParams;

        // Make a copy of the returned struct, as we will lose the memory
        auto selfSize = Allocator::alignSize(realType->sizeOf);
        auto self     = g_Allocator.alloc(selfSize);
        memcpy(self, (void*) runContext->registersRR[0].pointer, realType->sizeOf);

        // Get number of elements by calling 'opCount'
        SWAG_ASSERT(params->specReturnOpCount);
        opParams.callParams.push_back((uint64_t) self);
        SWAG_CHECK(executeNode(sourceFile, params->specReturnOpCount->node, callerContext, &opParams));
        auto count = runContext->registersRR[0].u64;
        if (!count)
            return callerContext->report({node, Fmt(Err(Err0161), realType->getDisplayNameC()), Diagnostic::isType(realType)});

        // Get the slice by calling 'opSlice'
        SWAG_ASSERT(params->specReturnOpSlice);
        opParams.callParams.clear();
        opParams.callParams.push_back(count - 1);
        opParams.callParams.push_back(0);
        opParams.callParams.push_back((uint64_t) self);
        SWAG_CHECK(executeNode(sourceFile, params->specReturnOpSlice->node, callerContext, &opParams));
        if (!runContext->registersRR[0].u64 || !runContext->registersRR[1].u64)
            return callerContext->report({node, Fmt(Err(Err0162), realType->getDisplayNameC()), Diagnostic::isType(realType)});

        auto      concreteType = TypeManager::concreteType(params->specReturnOpSlice->typeInfo);
        uint32_t  sizeSlice    = 0;
        TypeInfo* sliceType    = nullptr;

        if (concreteType->isString())
        {
            sizeSlice = (uint32_t) runContext->registersRR[1].u64;
            sliceType = g_TypeMgr->typeInfoU8;
        }
        else
        {
            auto typeSlice = CastTypeInfo<TypeInfoSlice>(concreteType, TypeInfoKind::Slice);
            sizeSlice      = (uint32_t) runContext->registersRR[1].u64 * typeSlice->pointedType->sizeOf;
            sliceType      = typeSlice->pointedType;
        }

        SWAG_CHECK(callerContext->checkSizeOverflow("array", sizeSlice, SWAG_LIMIT_ARRAY_SIZE));

        // Copy the content of the slice to the storage segment
        auto     storageSegment             = SemanticJob::getConstantSegFromContext(node);
        uint8_t* addrDst                    = nullptr;
        auto     offsetStorage              = storageSegment->reserve(sizeSlice, &addrDst);
        node->computedValue->storageOffset  = offsetStorage;
        node->computedValue->storageSegment = storageSegment;
        auto addrSrc                        = runContext->registersRR[0].pointer;
        memcpy(addrDst, (const void*) addrSrc, sizeSlice);

        // Then transform the returned type to a static array
        auto typeArray         = allocType<TypeInfoArray>();
        node->typeInfo         = typeArray;
        typeArray->pointedType = sliceType;
        typeArray->finalType   = sliceType;
        typeArray->count       = runContext->registersRR[1].u32;
        typeArray->totalCount  = runContext->registersRR[1].u32;
        typeArray->sizeOf      = sizeSlice;
        typeArray->computeName();
        typeArray->setConst();

        // Call opDrop on the original struct if defined
        if (params->specReturnOpDrop)
        {
            opParams.callParams.clear();
            opParams.callParams.push_back((uint64_t) self);
            SWAG_CHECK(executeNode(sourceFile, params->specReturnOpDrop->node, callerContext, &opParams));
        }

        g_Allocator.free(self, selfSize);
        return true;
    }

    // Default
    if (realType->isNativeIntegerOrRune() ||
        realType->isNativeFloat() ||
        realType->isBool())
    {
        switch (realType->sizeOf)
        {
        case 1:
            node->computedValue->reg.u64 = runContext->registersRR[0].u8;
            return true;
        case 2:
            node->computedValue->reg.u64 = runContext->registersRR[0].u16;
            return true;
        case 4:
            node->computedValue->reg.u64 = runContext->registersRR[0].u32;
            return true;
        case 8:
            node->computedValue->reg.u64 = runContext->registersRR[0].u64;
            return true;
        }
    }

    return callerContext->report({node, Fmt(Err(Err0058), realType->getDisplayNameC())});
}

bool Module::executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext, ExecuteNodeParams* params)
{
    // In case bytecode generation has raised an error
    if (node->sourceFile->numErrors)
        return false;
    if (node->sourceFile->module->numErrors)
        return false;

    SWAG_ASSERT(node->semFlags & AST_SEM_BYTECODE_GENERATED);
    SWAG_ASSERT(node->semFlags & AST_SEM_BYTECODE_RESOLVED);

    ByteCode*           bc = nullptr;
    ByteCode            bcTmp;
    ByteCodeInstruction instTmp;
    bool                foreignCall = false;

    // Direct call to a foreign function ?
    if (!node->extension || !node->extension->bytecode || !node->extension->bytecode->bc)
    {
        SWAG_ASSERT(node->isForeign());
        bc                  = &bcTmp;
        instTmp.op          = ByteCodeOp::ForeignCall;
        instTmp.a.pointer   = (uint8_t*) CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        instTmp.b.pointer   = (uint8_t*) node->typeInfo;
        bc->out             = &instTmp;
        bc->numInstructions = 1;
        bc->maxInstructions = 1;
        foreignCall         = true;
    }
    else
    {
        SWAG_ASSERT(node->extension->bytecode->bc->out);
        bc = node->extension->bytecode->bc;
    }

    // Setup flags before running
    auto cxt = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
    SWAG_ASSERT(cxt);
    cxt->flags = getDefaultContextFlags(this);
    cxt->flags |= (uint64_t) ContextFlags::ByteCode;

    // Global setup
    g_ByteCodeStackTrace->clear();
    g_ByteCodeStackTrace->currentContext = g_RunContext;
    g_RunContext->callerContext          = callerContext;
    g_RunContext->setup(sourceFile, node, bc);
    g_RunContext->oldBc = nullptr;

    // Setup run context
    if (params)
    {
        if (params->inheritSp)
            g_RunContext->sp = params->inheritSp;
        if (params->inheritSpAlt)
            g_RunContext->spAlt = params->inheritSpAlt;
        if (params->inheritBp)
            g_RunContext->bp = params->inheritBp;
        if (params->inheritStack)
            g_RunContext->stack = params->inheritStack;
    }

    // Params ?
    auto module = sourceFile->module;
    if (params && !params->callParams.empty())
    {
        for (auto r : params->callParams)
            g_RunContext->push(r);

        if (!foreignCall)
            module->runner.localCall(g_RunContext, bc, (uint32_t) params->callParams.size());
    }
    else if (!foreignCall)
    {
        bc->enterByteCode(g_RunContext);
    }

    // We need to take care of the room necessary in the stack, as bytecode instruction IncSPBP is not
    // generated for expression (just for functions)
    if (node->ownerScope)
    {
        if (!params || !params->inheritSp)
        {
            auto decSP = SemanticJob::getMaxStackSize(node);
            g_RunContext->decSP(decSP);

            // :opAffectConstExpr
            // Reserve room on the stack to store the result
            if (node->semFlags & AST_SEM_EXEC_RET_STACK)
                g_RunContext->decSP(node->typeInfo->sizeOf);

            g_RunContext->bp = g_RunContext->sp;
        }
    }

    bool result = true;

    if (foreignCall)
    {
        module->runner.ffiCall(g_RunContext, &bc->out[0]);
    }
    else
    {
        result = module->runner.run(g_RunContext);
        bc->leaveByteCode(g_RunContext);
    }

    g_ByteCodeStackTrace->clear();

    if (!result)
        return false;

    // Free auto allocated memory
    for (auto ptr : bc->autoFree)
        g_Allocator.free(ptr.first, ptr.second);

    // Get result
    SWAG_CHECK(computeExecuteResult(g_RunContext, sourceFile, node, callerContext, params));
    return true;
}
