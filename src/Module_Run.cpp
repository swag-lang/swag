#include "pch.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeStack.h"
#include "TypeManager.h"
#include "Context.h"
#include "SemanticJob.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool Module::computeExecuteResult(SourceFile* sourceFile, AstNode* node, JobContext* callerContext, ExecuteNodeParams* params)
{
    // :opAffectConstExpr
    // Result is on the stack. Store it in the compiler segment.
    if (node->semFlags & AST_SEM_EXEC_RET_STACK)
    {
        auto storageSegment = &sourceFile->module->compilerSegment;
        auto storageOffset  = storageSegment->reserve(node->typeInfo->sizeOf);
        node->allocateComputedValue();
        node->computedValue->storageSegment = storageSegment;
        node->computedValue->storageOffset  = storageOffset;
        auto addrDst                        = storageSegment->address(storageOffset);
        auto addrSrc                        = g_RunContext.bp;
        memcpy(addrDst, addrSrc, node->typeInfo->sizeOf);
        return true;
    }

    // Transform result to a literal value
    if (!node->resultRegisterRC.size())
        return true;

    auto realType  = TypeManager::concreteReferenceType(node->typeInfo);
    node->typeInfo = TypeManager::concreteReferenceType(node->typeInfo, CONCRETE_FUNC);
    node->setFlagsValueIsComputed();

    // String
    if (realType->isNative(NativeTypeKind::String))
    {
        SWAG_ASSERT(node->resultRegisterRC.size() == 2);
        const char* pz  = (const char*) g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].pointer;
        uint32_t    len = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[1]].u32;
        node->computedValue->text.reserve(len + 1);
        node->computedValue->text.count = len;
        memcpy(node->computedValue->text.buffer, pz, len);
        node->computedValue->text.buffer[len] = 0;
        return true;
    }

    // Static array
    if (realType->kind == TypeInfoKind::Array)
    {
        auto     storageSegment             = SemanticJob::getConstantSegFromContext(node);
        uint8_t* addrDst                    = nullptr;
        auto     offsetStorage              = storageSegment->reserve(realType->sizeOf, &addrDst);
        node->computedValue->storageOffset  = offsetStorage;
        node->computedValue->storageSegment = storageSegment;
        auto addrSrc                        = g_RunContext.registersRR[0].pointer;
        memcpy(addrDst, (const void*) addrSrc, realType->sizeOf);
        return true;
    }

    // Struct return
    if (realType->kind == TypeInfoKind::Struct)
    {
        // If struct is makred as constexpr, then we can raw copy the slice content
        if (realType->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
        {
            auto     storageSegment             = SemanticJob::getConstantSegFromContext(node);
            uint8_t* addrDst                    = nullptr;
            auto     offsetStorage              = storageSegment->reserve(realType->sizeOf, &addrDst);
            node->computedValue->storageOffset  = offsetStorage;
            node->computedValue->storageSegment = storageSegment;
            auto addrSrc                        = g_RunContext.registersRR[0].pointer;
            memcpy(addrDst, (const void*) addrSrc, realType->sizeOf);
            return true;
        }

        // Convert struct to static array
        ExecuteNodeParams opParams;

        // Make a copy of the returned struct, as we will lose the memory
        auto selfSize = Allocator::alignSize(realType->sizeOf);
        auto self     = g_Allocator.alloc(selfSize);
        memcpy(self, (void*) g_RunContext.registersRR[0].pointer, realType->sizeOf);

        // Get number of elements by calling 'opCount'
        SWAG_ASSERT(params->specReturnOpCount);
        opParams.callParams.push_back((uint64_t) self);
        SWAG_CHECK(executeNode(sourceFile, params->specReturnOpCount->node, callerContext, &opParams));
        auto count = g_RunContext.registersRR[0].u64;
        if (!count)
            return callerContext->report({node, Utf8::format(Msg0161, realType->getDisplayName().c_str())});

        // Get the slice by calling 'opSlice'
        SWAG_ASSERT(params->specReturnOpSlice);
        opParams.callParams.clear();
        opParams.callParams.push_back(count - 1);
        opParams.callParams.push_back(0);
        opParams.callParams.push_back((uint64_t) self);
        SWAG_CHECK(executeNode(sourceFile, params->specReturnOpSlice->node, callerContext, &opParams));
        if (!g_RunContext.registersRR[0].u64 || !g_RunContext.registersRR[1].u64)
            return callerContext->report({node, Utf8::format(Msg0162, realType->getDisplayName().c_str())});

        auto typeSlice = CastTypeInfo<TypeInfoSlice>(TypeManager::concreteType(params->specReturnOpSlice->typeInfo), TypeInfoKind::Slice);
        auto sizeSlice = (uint32_t) g_RunContext.registersRR[1].u64 * typeSlice->pointedType->sizeOf;
        SWAG_CHECK(callerContext->checkSizeOverflow("array", sizeSlice, SWAG_LIMIT_ARRAY_SIZE));

        // Copy the content of the slice to the storage segment
        auto     storageSegment             = SemanticJob::getConstantSegFromContext(node);
        uint8_t* addrDst                    = nullptr;
        auto     offsetStorage              = storageSegment->reserve(sizeSlice, &addrDst);
        node->computedValue->storageOffset  = offsetStorage;
        node->computedValue->storageSegment = storageSegment;
        auto addrSrc                        = g_RunContext.registersRR[0].pointer;
        memcpy(addrDst, (const void*) addrSrc, sizeSlice);

        // Then transform the returned type to a static array
        auto typeArray         = allocType<TypeInfoArray>();
        node->typeInfo         = typeArray;
        typeArray->pointedType = typeSlice->pointedType;
        typeArray->finalType   = typeSlice->pointedType;
        typeArray->count       = g_RunContext.registersRR[1].u32;
        typeArray->totalCount  = g_RunContext.registersRR[1].u32;
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
        realType->isNative(NativeTypeKind::Bool))
    {
        switch (realType->sizeOf)
        {
        case 1:
            node->computedValue->reg.u64 = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u8;
            return true;
        case 2:
            node->computedValue->reg.u64 = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u16;
            return true;
        case 4:
            node->computedValue->reg.u64 = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u32;
            return true;
        case 8:
            node->computedValue->reg.u64 = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u64;
            return true;
        }
    }

    return callerContext->report({node, Utf8::format(Msg0058, realType->getDisplayName().c_str())});
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
    SWAG_ASSERT(node->extension && node->extension->bc);
    SWAG_ASSERT(node->extension->bc->out);

    // Setup flags before running
    auto cxt = (SwagContext*) OS::tlsGetValue(g_TlsContextId);
    SWAG_ASSERT(cxt);
    cxt->flags = getDefaultContextFlags(this);
    cxt->flags |= (uint64_t) ContextFlags::ByteCode;

    // Global setup
    g_ByteCodeStack.clear();
    g_RunContext.callerContext = callerContext;
    g_RunContext.setup(sourceFile, node);

    // Params ?
    auto module = sourceFile->module;
    if (params && !params->callParams.empty())
    {
        for (auto r : params->callParams)
            g_RunContext.push(r);
        module->runner.localCall(&g_RunContext, node->extension->bc, (uint32_t) params->callParams.size());
    }
    else
    {
        node->extension->bc->enterByteCode(&g_RunContext);
    }

    // We need to take care of the room necessary in the stack, as bytecode instruction IncSPBP is not
    // generated for expression (just for functions)
    if (node->ownerScope)
    {
        g_RunContext.decSP(node->ownerScope->startStackSize);

        // :opAffectConstExpr
        // Reserve room on the stack to store the result
        if (node->semFlags & AST_SEM_EXEC_RET_STACK)
            g_RunContext.decSP(node->typeInfo->sizeOf);

        g_RunContext.bp = g_RunContext.sp;
    }

    bool result = module->runner.run(&g_RunContext);

    node->extension->bc->leaveByteCode(&g_RunContext, false);
    g_ByteCodeStack.clear();

    if (!result)
        return false;

    // Free auto allocated memory
    for (auto ptr : node->extension->bc->autoFree)
        g_Allocator.free(ptr.first, ptr.second);

    // Get result
    SWAG_CHECK(computeExecuteResult(sourceFile, node, callerContext, params));
    return true;
}

void Module::postCompilerMessage(ConcreteCompilerMessage& msg)
{
    scoped_lock lk(mutexByteCodeCompiler);

    // Cannot decide yet if there's a corresponding #compiler for that message, so push it
    if (numCompilerFunctions > 0)
        compilerMessages.push_back(msg);

    // We can decide, because every #compiler function have been registered.
    // So if there's no #compiler function for the given message, just dismiss it.
    else
    {
        int index = (int) msg.kind;
        if (byteCodeCompiler[index].empty())
            return;
        compilerMessages.push_back(msg);
    }
}

bool Module::flushCompilerMessages(JobContext* context)
{
    while (!compilerMessages.empty())
    {
        auto& msg = compilerMessages.back();

        // Be sure we have a #compiler for that message
        int index = (int) msg.kind;
        if (byteCodeCompiler[index].empty())
        {
            compilerMessages.pop_back();
            continue;
        }

        if (msg.kind == CompilerMsgKind::SemanticFunc)
        {
            context->sourceFile     = files.front();
            context->node           = context->sourceFile->astRoot;
            auto     storageSegment = &context->sourceFile->module->compilerSegment;
            uint32_t storageOffset;
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, (TypeInfo*) msg.type, storageSegment, &storageOffset, MAKE_CONCRETE_SHOULD_WAIT));
            if (context->result != ContextResult::Done)
                return true;
            msg.type = (ConcreteTypeInfo*) storageSegment->address(storageOffset);
        }

        sendCompilerMessage(&msg, context->baseJob);
        SWAG_ASSERT(context->result == ContextResult::Done);

        compilerMessages.pop_back();
    }

    return true;
}

bool Module::sendCompilerMessage(CompilerMsgKind msgKind, Job* dependentJob)
{
    ConcreteCompilerMessage msg;
    msg.kind = msgKind;
    return sendCompilerMessage(&msg, dependentJob);
}

bool Module::sendCompilerMessage(ConcreteCompilerMessage* msg, Job* dependentJob)
{
    scoped_lock lk(mutexByteCodeCompiler);
    int         index = (int) msg->kind;
    if (byteCodeCompiler[index].empty())
        return true;

    // Convert to a concrete message for the user
    msg->moduleName.buffer = (void*) name.c_str();
    msg->moduleName.count  = name.length();

    // Find to do that, as this function can only be called once (not multi threaded)
    g_RunContext.currentCompilerMessage = msg;

    JobContext context;
    context.baseJob = dependentJob;

    PushSwagContext cxt;
    for (auto bc : byteCodeCompiler[index])
    {
        SWAG_CHECK(executeNode(bc->node->sourceFile, bc->node, &context));
    }

    g_RunContext.currentCompilerMessage = nullptr;
    return true;
}
