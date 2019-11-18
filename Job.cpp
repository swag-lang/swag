#include "pch.h"
#include "Job.h"
#include "ThreadManager.h"
#include "Global.h"
#include "SymTable.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "Diagnostic.h"

void Job::doneJob()
{
    scoped_lock lk(executeMutex);
    for (auto p : dependentJobs.list)
        g_ThreadMgr.addJob(p);
    dependentJobs.clear();
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
    symbol->dependentJobs.add(this);
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