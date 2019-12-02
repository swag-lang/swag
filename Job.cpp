#include "pch.h"
#include "Job.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "Diagnostic.h"
#include "Module.h"

void Job::doneJob()
{
    // Push back dependent jobs
    {
        unique_lock lk(executeMutex);
        for (auto p : dependentJobs.list)
            g_ThreadMgr.addJob(p);
        dependentJobs.clear();
    }
}

void Job::addDependentJob(Job* job)
{
    scoped_lock lk(executeMutex);
    dependentJobs.add(job);
}

void Job::waitForSymbolNoLock(SymbolName* symbol)
{
    waitingSymbolSolved = symbol;
    setPending();
    symbol->addDependentJobNoLock(this);
}

void Job::waitForAllStructInterfaces(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    auto        typeInfoStruct = (TypeInfoStruct*) typeInfo;
    scoped_lock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->cptRemainingInterfaces == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->structNode);
    scoped_lock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending();
}

void Job::setPending()
{
    SWAG_ASSERT(baseContext);
    baseContext->result = ContextResult::Pending;
    g_ThreadMgr.addPendingJob(this);
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

        Diagnostic note{first, first->token, format("occurred during expansion of '%s'", name.c_str()), DiagnosticLevel::Note};
        copyNotes.push_back(&note);

        return sourceFile->report(diag, copyNotes);
    }

    return sourceFile->report(diag, copyNotes);
}