#include "pch.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeStack.h"
#include "TypeManager.h"
#include "Context.h"
#include "SemanticJob.h"
#include "ErrorIds.h"

bool Module::executeNode(SourceFile* sourceFile, AstNode* node, JobContext* callerContext)
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

    g_ByteCodeStack.clear();
    return executeNodeNoLock(sourceFile, node, callerContext);
}

bool Module::executeNodeNoLock(SourceFile* sourceFile, AstNode* node, JobContext* callerContext)
{
    // Global setup
    g_RunContext.callerContext = callerContext;
    g_RunContext.setup(sourceFile, node);

    node->extension->bc->enterByteCode(&g_RunContext);
    auto module = sourceFile->module;

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

    // :opAffectConstExpr
    // Result is on the stack. Store it in the compiler segment.
    if (node->semFlags & AST_SEM_EXEC_RET_STACK)
    {
        auto offsetStorage = sourceFile->module->compilerSegment.reserve(node->typeInfo->sizeOf);
        node->allocateComputedValue();
        node->computedValue->storageOffset = offsetStorage;
        auto addrDst                       = sourceFile->module->compilerSegment.address(offsetStorage);
        auto addrSrc                       = g_RunContext.bp;
        memcpy(addrDst, addrSrc, node->typeInfo->sizeOf);
    }

    // Transform result to a literal value
    else if (node->resultRegisterRC.size())
    {
        auto realType  = TypeManager::concreteReferenceType(node->typeInfo);
        node->typeInfo = TypeManager::concreteReferenceType(node->typeInfo, CONCRETE_FUNC);
        node->setFlagsValueIsComputed();

        if (realType->isNative(NativeTypeKind::String))
        {
            SWAG_ASSERT(node->resultRegisterRC.size() == 2);
            const char* pz  = (const char*) g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].pointer;
            uint32_t    len = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[1]].u32;
            node->computedValue->text.reserve(len + 1);
            node->computedValue->text.count = len;
            memcpy(node->computedValue->text.buffer, pz, len);
            node->computedValue->text.buffer[len] = 0;
        }
        else if (realType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            bool ok = false;
            if (realType->kind == TypeInfoKind::Struct && realType->flags & TYPEINFO_STRUCT_IS_TUPLE)
                ok = true;
            if (realType->kind == TypeInfoKind::Struct && realType->declNode->attributeFlags & ATTRIBUTE_CONSTEXPR)
                ok = true;
            if (realType->kind == TypeInfoKind::Array)
                ok = true;

            if (!ok)
            {
                if (realType->kind == TypeInfoKind::Struct && !(realType->flags & TYPEINFO_STRUCT_IS_TUPLE))
                    return callerContext->report({node, Utf8::format(Msg0281, realType->getDisplayName().c_str())});
                return callerContext->report({node, Utf8::format(Msg0280, realType->getDisplayName().c_str())});
            }

            auto storageSegment                 = SemanticJob::getConstantSegFromContext(node);
            auto offsetStorage                  = storageSegment->reserve(realType->sizeOf);
            node->computedValue->storageOffset  = offsetStorage;
            node->computedValue->storageSegment = storageSegment;
            auto addrDst                        = storageSegment->address(offsetStorage);
            auto addrSrc                        = g_RunContext.registersRR[0].pointer;
            memcpy(addrDst, (const void*) addrSrc, realType->sizeOf);
        }
        else
        {
            switch (realType->sizeOf)
            {
            case 1:
                node->computedValue->reg.u64 = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u8;
                break;
            case 2:
                node->computedValue->reg.u64 = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u16;
                break;
            case 4:
                node->computedValue->reg.u64 = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u32;
                break;
            default:
                node->computedValue->reg.u64 = g_RunContext.registersRC[0]->buffer[node->resultRegisterRC[0]].u64;
                break;
            }
        }
    }

    // Free auto allocated memory
    for (auto ptr : node->extension->bc->autoFree)
        g_Allocator.free(ptr.first, ptr.second);

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
