#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGenJob.h"
#include "Backend/Context.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"

bool ByteCodeGen::setupRuntime(const ByteCodeGenContext* context, const AstNode* node)
{
    // Register the allocator interface to the default bytecode context
    if (node->token.is(g_LangSpec->name_SystemAllocator))
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
        YIELD();
        SWAG_ASSERT(typeStruct->interfaces.size() == 1);
        const auto table = context->sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
        SWAG_ASSERT(table);
        SWAG_ASSERT(reinterpret_cast<void**>(table)[0]);
        g_SystemAllocatorTable = table;
    }

    if (node->token.is(g_LangSpec->name_DebugAllocator))
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
        YIELD();
        SWAG_ASSERT(typeStruct->interfaces.size() == 1);
        const auto table = context->sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
        SWAG_ASSERT(table);
        SWAG_ASSERT(reinterpret_cast<void**>(table)[0]);
        g_DebugAllocatorTable = table;
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

        if (node->is(AstNodeKind::FuncDecl) || context->bc->isCompilerGenerated)
        {
#ifdef SWAG_STATS
            g_Stats.numInstructions += context->bc->numInstructions;
#endif

            // Print the resulting bytecode
            if (node->hasAttribute(ATTRIBUTE_PRINT_BC) && !node->hasAttribute(ATTRIBUTE_GENERATED_FUNC))
            {
                ScopedLock lk(context->sourceFile->module->mutexByteCode);
                context->sourceFile->module->byteCodePrintBC.push_back(context->bc);
            }

            if (node->hasAttribute(ATTRIBUTE_PRINT_BC_GEN) && !node->hasAttribute(ATTRIBUTE_GENERATED_FUNC))
            {
                constexpr ByteCodePrintOptions opt;
                context->bc->print(opt);
            }
        }

        // Byte code is generated (but not yet resolved, as we need all dependencies to be resolved too)
        if (context->bc->node && context->bc->node->is(AstNodeKind::FuncDecl))
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
    // For the next pass (we will have to wait for all those nodes to be resolved before being resolved ourselves)
    ScopedLock lk(node->mutex);
    getDependantCalls(node, node->extByteCode()->dependentNodes);
    context->dependentNodesTmp = node->extByteCode()->dependentNodes;
    node->addSemFlag(SEMFLAG_BYTECODE_GENERATED);
    context->baseJob->dependentJobs.setRunning();

    return true;
}

bool ByteCodeGen::setupByteCodeResolved(const ByteCodeGenContext* context, AstNode* node)
{
    // Inform dependencies that everything is done
    releaseByteCodeJob(node);

    if (!context->bc)
        return true;

    // Register the function in the compiler list, now that we are done
    if (node->hasAttribute(ATTRIBUTE_MESSAGE_FUNC))
        context->sourceFile->module->addCompilerFunc(context->bc);

    // #ast/#run etc... can have a #[Swag.PrintBc]. We need to print it now, because it's compile-time, and the legit
    // pipeline for printing (after bc optimizer) will not be called in that case
    if (!g_ThreadMgr.debuggerMode)
    {
        if (node->hasAttribute(ATTRIBUTE_PRINT_BC) || (node->ownerFct && node->ownerFct->hasAttribute(ATTRIBUTE_PRINT_BC)))
        {
            if (node->hasAttribute(ATTRIBUTE_GENERATED_FUNC) || node->isNot(AstNodeKind::FuncDecl))
            {
                constexpr ByteCodePrintOptions opt;
                context->bc->print(opt);
            }
        }
    }

    // Register runtime function type, by name
    if (context->sourceFile->hasFlag(FILE_RUNTIME))
    {
        ScopedLock lk(context->sourceFile->module->mutexFile);
        context->sourceFile->module->mapRuntimeFct[context->bc->getCallName()] = context->bc;
    }

    if (context->bc->node && context->bc->node->is(AstNodeKind::FuncDecl))
    {
        const auto funcNode = castAst<AstFuncDecl>(context->bc->node, AstNodeKind::FuncDecl);
        if (!funcNode->token.sourceFile->hasFlag(FILE_SHOULD_HAVE_ERROR))
        {
            // Be sure that every used register has been released
            if (context->bc->maxReservedRegisterRC > context->bc->availableRegistersRC.size() + context->bc->staticRegs)
            {
                if (node->hasAttribute(ATTRIBUTE_PRINT_BC))
                {
                    constexpr ByteCodePrintOptions opt;
                    context->bc->print(opt);
                }
                Report::internalError(funcNode, form("function [[%s]] does not release all registers !", funcNode->token.cstr()));
            }
            else if (context->bc->maxReservedRegisterRC < context->bc->availableRegistersRC.size())
            {
                if (node->hasAttribute(ATTRIBUTE_PRINT_BC))
                {
                    constexpr ByteCodePrintOptions opt;
                    context->bc->print(opt);
                }
                Report::internalError(funcNode, form("function [[%s]] releases too many registers !", funcNode->token.cstr()));
            }
        }
    }

    return true;
}

bool ByteCodeGen::skipNodes(ByteCodeGenContext* context, AstNode* node)
{
    node->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
    const auto res = Ast::visit(context, node, [](ErrorContext* cxt, const AstNode* n) {
        if (n->isNot(AstNodeKind::Literal))
            return Ast::VisitResult::Continue;
        if (n->hasSemFlag(SEMFLAG_LITERAL_SUFFIX))
        {
            cxt->report({n->firstChild(), formErr(Err0242, n->firstChild()->token.cstr())});
            return Ast::VisitResult::Stop;
        }

        return Ast::VisitResult::Continue;
    });

    return res != Ast::VisitResult::Stop;
}

bool ByteCodeGen::askForByteCode(JobContext* context, AstNode* node, AskBcFlags flags, bool fromSemantic, ByteCode* caller)
{
    if (!node)
        return true;
    const auto job = context->baseJob;

    AstFuncDecl* funcDecl = nullptr;
    if (node->is(AstNodeKind::FuncDecl))
    {
        funcDecl = castAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);

        // If the function does not have content, or if sematic was not done, then
        // no need for bytecode
        if (funcDecl->content && funcDecl->content->hasAstFlag(AST_NO_SEMANTIC))
            return true;

        // If this is a foreign function, we do not need bytecode
        if (funcDecl->isForeign())
        {
            // Need to wait for function full semantic resolve
            if (flags.has(ASK_BC_WAIT_SEMANTIC_RESOLVED))
            {
                Semantic::waitFuncDeclFullResolve(job, funcDecl);
                YIELD();
            }

            ScopedLock lk(funcDecl->mutex);
            node->addSemFlag(SEMFLAG_BYTECODE_GENERATED | SEMFLAG_BYTECODE_RESOLVED);
            return true;
        }
    }

    if (job)
    {
        // If true, then this is a simple recursive call
        if (node->hasExtByteCode() && node->extByteCode()->byteCodeJob == job)
            return true;

        // Need to wait for function full semantic resolve
        if (flags.has(ASK_BC_WAIT_SEMANTIC_RESOLVED))
        {
            SWAG_ASSERT(funcDecl);
            Semantic::waitFuncDeclFullResolve(job, funcDecl);
            YIELD();
        }
    }

    if (caller)
        caller->dependentCalls.push_back_once(node);

    ScopedLock lk(node->mutex);
    const auto sourceFile = node->token.sourceFile;

    // Need to generate bytecode, if not already done or running
    if (!node->hasSemFlag(SEMFLAG_BYTECODE_GENERATED))
    {
        if (flags.has(ASK_BC_WAIT_DONE))
        {
            SWAG_ASSERT(job);
            job->setPending(JobWaitKind::SemByteCodeGenerated1, nullptr, node, nullptr);
        }

        node->allocateExtension(ExtensionKind::ByteCode);
        const auto extByteCode = node->extByteCode();
        if (!extByteCode->byteCodeJob)
        {
            Job* dependentJob;
            if (flags.has(ASK_BC_WAIT_DONE))
                dependentJob = job;
            else
                dependentJob = job->dependentJob;
            extByteCode->byteCodeJob                      = ByteCodeGenJob::newJob(dependentJob, sourceFile, node);
            extByteCode->byteCodeJob->context.errCxtSteps = job->baseContext->errCxtSteps;
            if (!extByteCode->bc)
            {
                extByteCode->bc             = Allocator::alloc<ByteCode>();
                extByteCode->bc->node       = node;
                extByteCode->bc->sourceFile = node->token.sourceFile;
            }

            extByteCode->bc->typeInfoFunc = funcDecl ? castTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo) : nullptr;
            if (node->hasAstFlag(AST_DEFINED_INTRINSIC))
                extByteCode->bc->name = node->token.text;
            else if (node->token.sourceFile->hasFlag(FILE_RUNTIME))
                extByteCode->bc->name = node->token.text;
            else
            {
                extByteCode->bc->name = node->ownerScope->getFullName();
                extByteCode->bc->name += ".";
                extByteCode->bc->name += node->token.text;
            }

            if (funcDecl)
                sourceFile->module->addByteCodeFunc(node->extByteCode()->bc);

            if (flags.has(ASK_BC_WAIT_DONE))
                job->jobsToAdd.push_back(node->extByteCode()->byteCodeJob);
            else
                g_ThreadMgr.addJob(node->extByteCode()->byteCodeJob);
        }
        else if (flags.has(ASK_BC_WAIT_DONE))
        {
            ScopedLock lk1(extByteCode->byteCodeJob->mutexDependent);
            extByteCode->byteCodeJob->dependentJobs.add(job);
        }

        return true;
    }

    if (flags.has(ASK_BC_WAIT_RESOLVED))
    {
        SWAG_ASSERT(job);
        if (!node->hasSemFlag(SEMFLAG_BYTECODE_RESOLVED))
        {
            const auto extByteCode = node->extByteCode();
            SWAG_ASSERT(extByteCode && extByteCode->byteCodeJob);

            ScopedLock lk1(extByteCode->byteCodeJob->mutexDependent);
            job->setPending(JobWaitKind::AskBcWaitResolve, nullptr, node, nullptr);
            extByteCode->byteCodeJob->dependentJobs.add(job);
            return true;
        }
    }

    return true;
}

void ByteCodeGen::releaseByteCodeJob(AstNode* node)
{
    ScopedLock lk(node->mutex);
    node->addSemFlag(SEMFLAG_BYTECODE_RESOLVED | SEMFLAG_BYTECODE_GENERATED);
    if (node->hasExtByteCode() && node->extByteCode()->byteCodeJob)
    {
        node->extByteCode()->byteCodeJob->dependentJobs.setRunning();
        node->extByteCode()->byteCodeJob = nullptr;
    }
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
