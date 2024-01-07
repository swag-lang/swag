#include "pch.h"
#include "ByteCodeGen.h"
#include "Ast.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Context.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Semantic.h"
#include "ThreadManager.h"
#include "TypeManager.h"

bool ByteCodeGen::setupRuntime(ByteCodeGenContext* context, AstNode* node)
{
    // Register allocator interface to the default bytecode context
    if (node->token.text == g_LangSpec->name_SystemAllocator)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
        YIELD();
        SWAG_ASSERT(typeStruct->interfaces.size() == 1);
        auto itable = context->sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
        SWAG_ASSERT(itable);
        SWAG_ASSERT(((void**) itable)[0]);
        g_SystemAllocatorTable = itable;
    }

    if (node->token.text == g_LangSpec->name_DebugAllocator)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
        YIELD();
        SWAG_ASSERT(typeStruct->interfaces.size() == 1);
        auto itable = context->sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
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

bool ByteCodeGen::skipNodes(ByteCodeGenContext* context, AstNode* node)
{
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    auto res = Ast::visit(context, node, [](ErrorContext* cxt, AstNode* n)
                          {
                              if (n->kind != AstNodeKind::Literal)
                                  return Ast::VisitResult::Continue;
                              if (n->semFlags & SEMFLAG_LITERAL_SUFFIX)
                              {
                                  cxt->report({ n->childs.front(), Fmt(Err(Err0532), n->childs.front()->token.ctext()) });
                                  return Ast::VisitResult::Stop;
                              }

                              return Ast::VisitResult::Continue; });

    return res == Ast::VisitResult::Stop ? false : true;
}

void ByteCodeGen::inherhitLocation(ByteCodeInstruction* inst, AstNode* node)
{
    inst->node = node;
}

void ByteCodeGen::askForByteCode(Job* job, AstNode* node, uint32_t flags, ByteCode* caller)
{
    if (!node)
        return;

    auto sourceFile = node->sourceFile;

    // If this is a foreign function, we do not need bytecode
    AstFuncDecl* funcDecl = nullptr;
    if (node->kind == AstNodeKind::FuncDecl)
    {
        funcDecl = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
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
        auto extension = node->extByteCode();
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
            auto extension = node->extByteCode();
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

void ByteCodeGen::getDependantCalls(AstNode* depNode, VectorNative<AstNode*>& dep)
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
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(depNode->typeInfo, TypeInfoKind::Struct);
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