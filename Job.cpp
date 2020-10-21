#include "pch.h"
#include "Job.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "Diagnostic.h"
#include "Module.h"
#include "SemanticJob.h"

void Job::addDependentJob(Job* job)
{
    scoped_lock lk(executeMutex);
    dependentJobs.add(job);
}

void Job::waitForSymbolNoLock(SymbolName* symbol)
{
    if (flags & JOB_COMPILER_PASS)
    {
        if (symbol->ownerTable->scope->isGlobal())
        {
            for (auto node : symbol->nodes)
            {
                if (node->flags & AST_DONE_COMPILER_PASS)
                    continue;

                auto job          = g_Pool_semanticJob.alloc();
                job->sourceFile   = node->sourceFile;
                job->module       = node->sourceFile->module;
                job->dependentJob = dependentJob;
                job->flags |= JOB_COMPILER_PASS;
                job->nodes.push_back(node);
                jobsToAdd.push_back(job);
            }

            symbol->nodes.clear();
        }
    }

    setPending(symbol, "WAIT_SYMBOL", nullptr, nullptr);
    symbol->addDependentJobNoLock(this);
}

void Job::waitForAllStructInterfaces(TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->finalType;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    auto        typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    scoped_lock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->cptRemainingInterfaces == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    scoped_lock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(nullptr, "WAIT_INTERFACES", nullptr, typeInfoStruct);
}

void Job::waitForAllStructMethods(TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->finalType;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    auto        typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    scoped_lock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->cptRemainingMethods == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    scoped_lock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(nullptr, "WAIT_METHODS", nullptr, typeInfoStruct);
}

void Job::setPending(SymbolName* symbolToWait, const char* id, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_ASSERT(baseContext);
    waitingSymbolSolved = symbolToWait;
    waitingId           = id;
    waitingIdNode       = node;
    waitingIdType       = typeInfo;
    baseContext->result = ContextResult::Pending;
}

bool JobContext::report(const Diagnostic& diag, const Diagnostic* note, const Diagnostic* note1)
{
    vector<const Diagnostic*> notes;
    if (note)
        notes.push_back(note);
    if (note1)
        notes.push_back(note1);
    return report(diag, notes);
}

bool JobContext::report(const Diagnostic& diag, const vector<const Diagnostic*>& notes)
{
    auto copyNotes = notes;

    if (expansionNode.size())
    {
        auto  first = expansionNode[0];
        auto& name  = first->resolvedSymbolName ? first->resolvedSymbolName->name : first->name;
        if (name.empty())
            name = first->token.text;
        if (!name.empty())
        {
            auto note = new Diagnostic{first, first->token, format("occurred during expansion of '%s'", name.c_str()), DiagnosticLevel::Note};
            copyNotes.push_back(note);
        }
        else
        {
            auto note = new Diagnostic{first, first->token, "occurred during an expansion here", DiagnosticLevel::Note};
            copyNotes.push_back(note);
        }
    }

    if (diag.sourceNode && diag.sourceNode->sourceFile && diag.sourceNode->sourceFile->sourceNode)
    {
        auto sourceNode = diag.sourceNode->sourceFile->sourceNode;
        auto note       = new Diagnostic{sourceNode, sourceNode->token, "occurred in generated code", DiagnosticLevel::Note};
        copyNotes.push_back(note);
    }

    SWAG_ASSERT(sourceFile);
    return sourceFile->report(diag, copyNotes);
}