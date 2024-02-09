#include "pch.h"
#include "ByteCodeGen.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Context.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Report.h"
#include "Scope.h"
#include "Semantic.h"
#include "ThreadManager.h"
#include "TypeManager.h"

bool ByteCodeGen::setupRuntime(const ByteCodeGenContext* context, const AstNode* node)
{
    // Register allocator interface to the default bytecode context
    if (node->token.text == g_LangSpec->name_SystemAllocator)
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
        YIELD();
        SWAG_ASSERT(typeStruct->interfaces.size() == 1);
        const auto itable = context->sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
        SWAG_ASSERT(itable);
        SWAG_ASSERT(((void**) itable)[0]);
        g_SystemAllocatorTable = itable;
    }

    if (node->token.text == g_LangSpec->name_DebugAllocator)
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
        YIELD();
        SWAG_ASSERT(typeStruct->interfaces.size() == 1);
        const auto itable = context->sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
        SWAG_ASSERT(itable);
        SWAG_ASSERT(((void**) itable)[0]);
        g_DebugAllocatorTable = itable;
    }

    if (g_SystemAllocatorTable && g_DebugAllocatorTable)
    {
        g_DefaultContext.allocator.data   = nullptr;
        g_DefaultContext.allocator.itable = g_SystemAllocatorTable;
    }

    return true;
}

bool ByteCodeGen::setupByteCodeGenerated(ByteCodeGenContext* context, AstNode* node)
{
    if (context->bc)
    {
        EMIT_INST0(context, ByteCodeOp::End);

        if (node->kind == AstNodeKind::FuncDecl || context->bc->isCompilerGenerated)
        {
#ifdef SWAG_STATS
            g_Stats.numInstructions += context->bc->numInstructions;
#endif

            // Print resulting bytecode
            if (node->hasAttribute(ATTRIBUTE_PRINT_BC) && !(node->hasAttribute(ATTRIBUTE_GENERATED_FUNC)))
            {
                ScopedLock lk(context->sourceFile->module->mutexByteCode);
                context->sourceFile->module->byteCodePrintBC.push_back(context->bc);
            }

            if (node->hasAttribute(ATTRIBUTE_PRINT_GEN_BC) && !(node->hasAttribute(ATTRIBUTE_GENERATED_FUNC)))
            {
                constexpr ByteCodePrintOptions opt;
                context->bc->print(opt);
            }
        }

        // Byte code is generated (but not yet resolved, as we need all dependencies to be resolved too)
        if (context->bc->node &&
            context->bc->node->kind == AstNodeKind::FuncDecl)
        {
            const auto funcNode = castAst<AstFuncDecl>(context->bc->node, AstNodeKind::FuncDecl);

            // Retrieve the persistent registers
            if (funcNode->registerGetContext != UINT32_MAX)
            {
                context->bc->registerGetContext = funcNode->registerGetContext;
                freeRegisterRC(context, context->bc->registerGetContext);
            }
            if (funcNode->registerStoreRR != UINT32_MAX)
            {
                context->bc->registerStoreRR = funcNode->registerStoreRR;
                freeRegisterRC(context, context->bc->registerStoreRR);
            }
        }
    }

    // Get all nodes we are dependent on
    // For the next pass (we will have to wait for all those nodes to resolved
    // before being resolved ourself)
    ScopedLock lk(node->mutex);
    getDependantCalls(node, node->extByteCode()->dependentNodes);
    context->dependentNodesTmp = node->extByteCode()->dependentNodes;
    node->semFlags |= SEMFLAG_BYTECODE_GENERATED;
    context->baseJob->dependentJobs.setRunning();

    return true;
}

bool ByteCodeGen::setupByteCodeResolved(const ByteCodeGenContext* context, AstNode* node)
{
    // Inform dependencies that everything is done
    releaseByteCodeJob(node);

    if (!context->bc)
        return true;

    // Register function in compiler list, now that we are done
    if (node->hasAttribute(ATTRIBUTE_COMPILER_FUNC))
        context->sourceFile->module->addCompilerFunc(context->bc);

    // #ast/#run etc... can have a #[Swag.PrintBc]. We need to print it now, because it's compile time, and the legit
    // pipeline for printing (after bc optimize) will not be called in that case
    if (!g_ThreadMgr.debuggerMode)
    {
        if (node->hasAttribute(ATTRIBUTE_PRINT_BC) || (node->ownerFct && node->ownerFct->hasAttribute(ATTRIBUTE_PRINT_BC)))
        {
            if (node->hasAttribute(ATTRIBUTE_GENERATED_FUNC) || node->kind != AstNodeKind::FuncDecl)
            {
                constexpr ByteCodePrintOptions opt;
                context->bc->print(opt);
            }
        }
    }

    // Register runtime function type, by name
    if (context->sourceFile->isRuntimeFile)
    {
        ScopedLock lk(context->sourceFile->module->mutexFile);
        context->sourceFile->module->mapRuntimeFct[context->bc->getCallName()] = context->bc;
    }

    if (context->bc->node &&
        context->bc->node->kind == AstNodeKind::FuncDecl)
    {
        const auto funcNode = castAst<AstFuncDecl>(context->bc->node, AstNodeKind::FuncDecl);
        if (!funcNode->sourceFile->shouldHaveError)
        {
            // Be sure that every used registers have been released
            if (context->bc->maxReservedRegisterRC > context->bc->availableRegistersRC.size() + context->bc->staticRegs)
            {
                Report::internalError(funcNode, FMT("function [[%s]] does not release all registers !", funcNode->token.ctext()));
                if (node->hasAttribute(ATTRIBUTE_PRINT_BC))
                {
                    constexpr ByteCodePrintOptions opt;
                    context->bc->print(opt);
                }
            }
            else if (context->bc->maxReservedRegisterRC < context->bc->availableRegistersRC.size())
            {
                Report::internalError(funcNode, FMT("function [[%s]] releases too many registers !", funcNode->token.ctext()));
                if (node->hasAttribute(ATTRIBUTE_PRINT_BC))
                {
                    constexpr ByteCodePrintOptions opt;
                    context->bc->print(opt);
                }
            }
        }
    }

    return true;
}

bool ByteCodeGen::skipNodes(ByteCodeGenContext* context, AstNode* node)
{
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    const auto res = Ast::visit(context, node, [](ErrorContext* cxt, const AstNode* n)
    {
        if (n->kind != AstNodeKind::Literal)
            return Ast::VisitResult::Continue;
        if (n->semFlags & SEMFLAG_LITERAL_SUFFIX)
        {
            cxt->report({n->childs.front(), FMT(Err(Err0403), n->childs.front()->token.ctext())});
            return Ast::VisitResult::Stop;
        }

        return Ast::VisitResult::Continue;
    });

    return res != Ast::VisitResult::Stop;
}

void ByteCodeGen::askForByteCode(Job* job, AstNode* node, uint32_t flags, ByteCode* caller)
{
    if (!node)
        return;

    const auto sourceFile = node->sourceFile;

    // If this is a foreign function, we do not need bytecode
    AstFuncDecl* funcDecl = nullptr;
    if (node->kind == AstNodeKind::FuncDecl)
    {
        funcDecl = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        if (funcDecl->content && (funcDecl->content->flags & AST_NO_SEMANTIC))
            return;

        if (funcDecl->isForeign())
        {
            // Need to wait for function full semantic resolve
            if (flags & ASKBC_WAIT_SEMANTIC_RESOLVED)
            {
                SWAG_ASSERT(job);
                Semantic::waitFuncDeclFullResolve(job, funcDecl);
                if (job->baseContext->result != ContextResult::Done)
                    return;
            }

            ScopedLock lk(node->mutex);
            node->semFlags |= SEMFLAG_BYTECODE_GENERATED | SEMFLAG_BYTECODE_RESOLVED;
            return;
        }
    }

    if (job)
    {
        // If true, then this is a simple recursive call
        if (node->hasExtByteCode() && node->extByteCode()->byteCodeJob == job)
            return;

        // Need to wait for function full semantic resolve
        if (flags & ASKBC_WAIT_SEMANTIC_RESOLVED)
        {
            SWAG_ASSERT(funcDecl);
            Semantic::waitFuncDeclFullResolve(job, funcDecl);
            if (job->baseContext->result != ContextResult::Done)
                return;
        }
    }

    if (caller)
        caller->dependentCalls.push_back_once(node);

    ScopedLock lk(node->mutex);

    // Need to generate bytecode, if not already done or running
    if (!(node->semFlags & SEMFLAG_BYTECODE_GENERATED))
    {
        if (flags & ASKBC_WAIT_DONE)
        {
            SWAG_ASSERT(job);
            job->setPending(JobWaitKind::SemByteCodeGenerated, nullptr, node, nullptr);
        }

        node->allocateExtensionNoLock(ExtensionKind::ByteCode);
        const auto extension = node->extByteCode();
        if (!extension->byteCodeJob)
        {
            Job* dependentJob;
            if (flags & ASKBC_WAIT_DONE)
                dependentJob = job;
            else
                dependentJob = job->dependentJob;
            extension->byteCodeJob                      = ByteCodeGenJob::newJob(dependentJob, sourceFile, node);
            extension->byteCodeJob->context.errCxtSteps = job->baseContext->errCxtSteps;
            if (!extension->bc)
            {
                extension->bc             = Allocator::alloc<ByteCode>();
                extension->bc->node       = node;
                extension->bc->sourceFile = node->sourceFile;
            }

            extension->bc->typeInfoFunc = funcDecl ? (TypeInfoFuncAttr*) funcDecl->typeInfo : nullptr;
            if (node->flags & AST_DEFINED_INTRINSIC)
                extension->bc->name = node->token.text;
            else if (node->sourceFile->isRuntimeFile)
                extension->bc->name = node->token.text;
            else
            {
                extension->bc->name = node->ownerScope->getFullName();
                extension->bc->name += ".";
                extension->bc->name += node->token.text;
            }

            if (node->kind == AstNodeKind::FuncDecl)
                sourceFile->module->addByteCodeFunc(node->extByteCode()->bc);

            if (flags & ASKBC_WAIT_DONE)
                job->jobsToAdd.push_back(node->extByteCode()->byteCodeJob);
            else
                g_ThreadMgr.addJob(node->extByteCode()->byteCodeJob);
        }
        else if (flags & ASKBC_WAIT_DONE)
        {
            ScopedLock lk1(extension->byteCodeJob->mutexDependent);
            extension->byteCodeJob->dependentJobs.add(job);
        }

        return;
    }

    if (flags & ASKBC_WAIT_RESOLVED)
    {
        SWAG_ASSERT(job);
        if (!(node->semFlags & SEMFLAG_BYTECODE_RESOLVED))
        {
            const auto extension = node->extByteCode();
            SWAG_ASSERT(extension && extension->byteCodeJob);

            ScopedLock lk1(extension->byteCodeJob->mutexDependent);
            job->setPending(JobWaitKind::AskBcWaitResolve, nullptr, node, nullptr);
            extension->byteCodeJob->dependentJobs.add(job);
            return;
        }
    }
}

void ByteCodeGen::releaseByteCodeJob(AstNode* node)
{
    ScopedLock lk(node->mutex);
    node->semFlags |= SEMFLAG_BYTECODE_RESOLVED | SEMFLAG_BYTECODE_GENERATED;
    SWAG_ASSERT(node->hasExtByteCode());
    node->extByteCode()->byteCodeJob = nullptr;
}

void ByteCodeGen::getDependantCalls(const AstNode* depNode, VectorNative<AstNode*>& dep)
{
    // Normal function
    SWAG_ASSERT(depNode->hasExtByteCode());
    if (depNode->extByteCode()->bc)
    {
        dep.append(depNode->extByteCode()->bc->dependentCalls);
    }

    // Struct: special functions
    else
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(depNode->typeInfo, TypeInfoKind::Struct);
        if (typeStruct->opInit)
            dep.append(typeStruct->opInit->dependentCalls);
        if (typeStruct->opDrop)
            dep.append(typeStruct->opDrop->dependentCalls);
        if (typeStruct->opPostCopy)
            dep.append(typeStruct->opPostCopy->dependentCalls);
        if (typeStruct->opPostMove)
            dep.append(typeStruct->opPostMove->dependentCalls);
    }
}
