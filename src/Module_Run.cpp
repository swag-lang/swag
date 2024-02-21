#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeStack.h"
#include "Context.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Semantic.h"
#include "TypeManager.h"

bool Module::computeExecuteResult(ByteCodeRunContext* runContext, SourceFile* sourceFile, AstNode* node, JobContext* callerContext, const ExecuteNodeParams* params)
{
    if (params && params->forDebugger)
    {
        if (!node->resultRegisterRc.size())
            return true;
        runContext->registersRR[0].pointer = runContext->registers.buffer[node->resultRegisterRc[0]].pointer;
        if (node->resultRegisterRc.size() > 1)
            runContext->registersRR[1].pointer = runContext->registers.buffer[node->resultRegisterRc[1]].pointer;
        return true;
    }

    // :CheckConstExprFuncReturnType
    // :opAffectConstExpr
    // Result is on the stack. Store it in the compiler segment.
    if (node->hasSemFlag(SEMFLAG_EXEC_RET_STACK))
    {
        const auto storageSegment = &sourceFile->module->compilerSegment;
        uint8_t*   addrDst;
        const auto storageOffset = storageSegment->reserve(node->typeInfo->sizeOf, &addrDst);
        node->allocateComputedValue();
        node->computedValue()->storageSegment = storageSegment;
        node->computedValue()->storageOffset  = storageOffset;
        const auto addrSrc                    = runContext->bp;
        std::copy_n(addrSrc, node->typeInfo->sizeOf, addrDst);
        return true;
    }

    // Transform result to a literal value
    if (!node->resultRegisterRc.size())
        return true;

    const auto realType = TypeManager::concreteType(node->typeInfo);
    node->typeInfo      = TypeManager::concreteType(node->typeInfo, CONCRETE_FUNC);
    node->setFlagsValueIsComputed();

    runContext->registersRR[0].u64 = runContext->registers.buffer[node->resultRegisterRc[0]].u64;
    if (node->resultRegisterRc.size() > 1)
        runContext->registersRR[1].u64 = runContext->registers.buffer[node->resultRegisterRc[1]].u64;

    // String
    if (realType->isString())
    {
        SWAG_ASSERT(node->resultRegisterRc.size() == 2);
        const auto     pz  = reinterpret_cast<const char*>(runContext->registersRR[0].pointer);
        const uint32_t len = runContext->registersRR[1].u32;
        node->computedValue()->text.reserve(len + 1);
        node->computedValue()->text.count = len;
        std::copy_n(pz, len, node->computedValue()->text.buffer);
        node->computedValue()->text.buffer[len] = 0;
        return true;
    }

    // Static array
    if (realType->isArray())
    {
        const auto storageSegment             = Semantic::getConstantSegFromContext(node);
        uint8_t*   addrDst                    = nullptr;
        const auto offsetStorage              = storageSegment->reserve(realType->sizeOf, &addrDst);
        node->computedValue()->storageOffset  = offsetStorage;
        node->computedValue()->storageSegment = storageSegment;
        const auto addrSrc                    = runContext->registersRR[0].pointer;
        std::copy_n(addrSrc, realType->sizeOf, addrDst);
        return true;
    }

    if (realType->isListArray())
    {
        const auto storageSegment             = Semantic::getConstantSegFromContext(node);
        uint8_t*   addrDst                    = nullptr;
        const auto offsetStorage              = storageSegment->reserve(realType->sizeOf, &addrDst);
        node->computedValue()->storageOffset  = offsetStorage;
        node->computedValue()->storageSegment = storageSegment;
        const auto addrSrc                    = runContext->registersRR[0].pointer;
        std::copy_n(addrSrc, realType->sizeOf, addrDst);
        const auto typeList = castTypeInfo<TypeInfoList>(realType, TypeInfoKind::TypeListArray);
        node->typeInfo      = TypeManager::convertTypeListToArray(&runContext->jc, typeList, true);
        return true;
    }

    // Struct return
    if (realType->isStruct())
    {
        // If struct is marked as constexpr, then we can raw copy the struct content
        if (realType->declNode->hasAttribute(ATTRIBUTE_CONSTEXPR) || node->hasSemFlag(SEMFLAG_FORCE_CONST_EXPR))
        {
            const auto storageSegment             = Semantic::getConstantSegFromContext(node);
            uint8_t*   addrDst                    = nullptr;
            const auto offsetStorage              = storageSegment->reserve(realType->sizeOf, &addrDst);
            node->computedValue()->storageOffset  = offsetStorage;
            node->computedValue()->storageSegment = storageSegment;
            const auto addrSrc                    = runContext->registersRR[0].pointer;
            std::copy_n(addrSrc, realType->sizeOf, addrDst);
            return true;
        }

        // Convert struct to static array
        ExecuteNodeParams opParams;

        // Make a copy of the returned struct, as we will lose the memory
        const auto selfSize = Allocator::alignSize(realType->sizeOf);
        auto       self     = Allocator::alloc(selfSize);
        std::copy_n(runContext->registersRR[0].pointer, realType->sizeOf, static_cast<uint8_t*>(self));

        // Call opPostMove if defined
        if (params->specReturnOpPostMove)
        {
            opParams.callParams.clear();
            opParams.callParams.push_back(reinterpret_cast<uint64_t>(self));
            SWAG_CHECK(executeNode(sourceFile, params->specReturnOpPostMove->node, callerContext, &opParams));
        }

        // Get number of elements by calling 'opCount'
        SWAG_ASSERT(params->specReturnOpCount);
        opParams.callParams.push_back(reinterpret_cast<uint64_t>(self));
        SWAG_CHECK(executeNode(sourceFile, params->specReturnOpCount->node, callerContext, &opParams));
        const auto count = runContext->registersRR[0].u64;
        if (!count)
            return callerContext->report({node, FMT(Err(Err0027), realType->getDisplayNameC())});

        // Get the slice by calling 'opSlice'
        SWAG_ASSERT(params->specReturnOpSlice);
        opParams.callParams.clear();
        opParams.callParams.push_back(count - 1);
        opParams.callParams.push_back(0);
        opParams.callParams.push_back(reinterpret_cast<uint64_t>(self));
        SWAG_CHECK(executeNode(sourceFile, params->specReturnOpSlice->node, callerContext, &opParams));
        if (!runContext->registersRR[0].u64 || !runContext->registersRR[1].u64)
            return callerContext->report({node, FMT(Err(Err0028), realType->getDisplayNameC())});

        const auto concreteType = TypeManager::concreteType(params->specReturnOpSlice->typeInfo);
        uint32_t   sizeSlice    = 0;
        TypeInfo*  sliceType    = nullptr;

        if (concreteType->isString())
        {
            sizeSlice = static_cast<uint32_t>(runContext->registersRR[1].u64);
            sliceType = g_TypeMgr->typeInfoU8;
        }
        else
        {
            const auto typeSlice = castTypeInfo<TypeInfoSlice>(concreteType, TypeInfoKind::Slice);
            sizeSlice            = static_cast<uint32_t>(runContext->registersRR[1].u64) * typeSlice->pointedType->sizeOf;
            sliceType            = typeSlice->pointedType;
        }

        SWAG_CHECK(callerContext->checkSizeOverflow("array", sizeSlice, SWAG_LIMIT_ARRAY_SIZE));
        const auto addrSrc = runContext->registersRR[0].pointer;

        if (concreteType->isString())
        {
            node->typeInfo              = g_TypeMgr->typeInfoString;
            node->computedValue()->text = Utf8{reinterpret_cast<const char*>(addrSrc), sizeSlice};
        }
        else
        {
            // Copy the content of the slice to the storage segment
            const auto storageSegment             = Semantic::getConstantSegFromContext(node);
            uint8_t*   addrDst                    = nullptr;
            const auto offsetStorage              = storageSegment->reserve(sizeSlice, &addrDst);
            node->computedValue()->storageOffset  = offsetStorage;
            node->computedValue()->storageSegment = storageSegment;
            std::copy_n(addrSrc, sizeSlice, addrDst);

            // Then transform the returned type to a static array
            const auto typeArray   = makeType<TypeInfoArray>();
            typeArray->pointedType = sliceType;
            typeArray->finalType   = sliceType;
            typeArray->count       = runContext->registersRR[1].u32;
            typeArray->totalCount  = runContext->registersRR[1].u32;
            typeArray->sizeOf      = sizeSlice;
            typeArray->computeName();
            typeArray->setConst();
            node->typeInfo = typeArray;
        }

        // Call opDrop on the original struct if defined
        if (params->specReturnOpDrop)
        {
            opParams.callParams.clear();
            opParams.callParams.push_back(reinterpret_cast<uint64_t>(self));
            SWAG_CHECK(executeNode(sourceFile, params->specReturnOpDrop->node, callerContext, &opParams));
        }

        Allocator::free(self, selfSize);
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
                node->computedValue()->reg.u64 = runContext->registersRR[0].u8;
                return true;
            case 2:
                node->computedValue()->reg.u64 = runContext->registersRR[0].u16;
                return true;
            case 4:
                node->computedValue()->reg.u64 = runContext->registersRR[0].u32;
                return true;
            case 8:
                node->computedValue()->reg.u64 = runContext->registersRR[0].u64;
                return true;
            default:
                break;
        }
    }

    // Pointer
    if (realType->isPointerToTypeInfo())
    {
        const auto module = node->token.sourceFile->module;
        uint32_t   offset = module->compilerSegment.tryOffset(runContext->registersRR[0].pointer);
        if (offset != UINT32_MAX)
        {
            node->addAstFlag(AST_VALUE_IS_GEN_TYPEINFO);
            node->computedValue()->storageOffset  = offset;
            node->computedValue()->storageSegment = &module->compilerSegment;
            return true;
        }

        offset = module->constantSegment.tryOffset(runContext->registersRR[0].pointer);
        if (offset != UINT32_MAX)
        {
            node->addAstFlag(AST_VALUE_IS_GEN_TYPEINFO);
            node->computedValue()->storageOffset  = offset;
            node->computedValue()->storageSegment = &module->constantSegment;
            return true;
        }
    }

    return callerContext->report({node, FMT(Err(Err0029), realType->getDisplayNameC())});
}

bool Module::executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext, ExecuteNodeParams* params)
{
    // In case bytecode generation has raised an error
    if (node->token.sourceFile->numErrors)
        return false;
    if (node->token.sourceFile->module->numErrors)
        return false;

    SWAG_ASSERT(node->hasSemFlag(SEMFLAG_BYTECODE_GENERATED));
    SWAG_ASSERT(node->hasSemFlag(SEMFLAG_BYTECODE_RESOLVED));

    ByteCode*           bc = nullptr;
    ByteCode            bcTmp;
    ByteCodeInstruction instTmp;
    bool                foreignCall = false;

    // Direct call to a foreign function ?
    if (!node->hasExtByteCode() || !node->extByteCode()->bc)
    {
        SWAG_ASSERT(node->isForeign());
        bc                  = &bcTmp;
        instTmp.op          = ByteCodeOp::ForeignCall;
        instTmp.a.pointer   = reinterpret_cast<uint8_t*>(castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl));
        instTmp.b.pointer   = reinterpret_cast<uint8_t*>(node->typeInfo);
        bc->out             = &instTmp;
        bc->numInstructions = 1;
        bc->maxInstructions = 1;
        foreignCall         = true;
    }
    else
    {
        SWAG_ASSERT(node->extByteCode()->bc->out);
        bc = node->extByteCode()->bc;
    }

    // Setup flags before running
    auto cxt = static_cast<SwagContext*>(OS::tlsGetValue(g_TlsContextId));
    SWAG_ASSERT(cxt);
    cxt->flags = getDefaultContextFlags(this);
    cxt->flags |= static_cast<uint64_t>(ContextFlags::ByteCode);

    // Global setup
    g_ByteCodeStackTrace->clear();
    g_RunContext->callerContext = callerContext;
    g_RunContext->setup(sourceFile, node, bc);
    g_RunContext->oldBc        = nullptr;
    g_RunContext->forConstExpr = params ? params->forConstExpr : false;
    g_RunContext->forDebugger  = params ? params->forDebugger : false;

    // Setup run context
    if (params)
    {
        g_RunContext->debugOn    = params->breakOnStart;
        g_RunContext->debugEntry = params->breakOnStart;
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
    if (params && !params->callParams.empty())
    {
        for (auto r : params->callParams)
            g_RunContext->push(r);

        if (!foreignCall)
            ByteCodeRun::localCallNoTrace(g_RunContext, bc, params->callParams.size());
    }
    else if (!foreignCall)
    {
        ByteCodeRun::enterByteCode(g_RunContext, bc);
    }

    // We need to take care of the room necessary in the stack, as bytecode instruction IncSPBP is not
    // generated for expression (just for functions)
    bc->dynStackSize = bc->stackSize;
    if (node->ownerScope)
    {
        if (!params || !params->inheritSp)
        {
            auto decSP = Semantic::getMaxStackSize(node);
            g_RunContext->decSP(decSP);
            bc->dynStackSize += decSP;

            // :opAffectConstExpr
            // Reserve room on the stack to store the result
            if (node->hasSemFlag(SEMFLAG_EXEC_RET_STACK))
                g_RunContext->decSP(node->typeInfo->sizeOf);

            g_RunContext->bp = g_RunContext->sp;
        }
    }

    bool result = true;

    if (foreignCall)
    {
        ByteCodeRun::ffiCall(g_RunContext, &bc->out[0]);
    }
    else
    {
        result = ByteCodeRun::run(g_RunContext);
    }

    g_ByteCodeStackTrace->clear();

    if (!result)
        return false;

    // Free auto allocated memory
    for (auto ptr : bc->autoFree)
        Allocator::free(ptr.first, ptr.second);

    // Get result
    SWAG_CHECK(computeExecuteResult(g_RunContext, sourceFile, node, callerContext, params));
    return true;
}
