#include "pch.h"
#include "Job.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "Diagnostic.h"
#include "Module.h"

void Job::addDependentJob(Job* job)
{
    scoped_lock lk(executeMutex);
    dependentJobs.add(job);
}

void Job::waitForSymbolNoLock(SymbolName* symbol)
{
    setPending(symbol);
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
    SWAG_ASSERT(typeInfoStruct->structNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    scoped_lock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(nullptr);
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
    SWAG_ASSERT(typeInfoStruct->structNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    scoped_lock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(nullptr);
}

void Job::setPending(SymbolName* symbolToWait)
{
    SWAG_ASSERT(baseContext);
    waitingSymbolSolved = symbolToWait;
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
            Diagnostic note{first, first->token, format("occurred during expansion of '%s'", name.c_str()), DiagnosticLevel::Note};
            copyNotes.push_back(&note);
            return sourceFile->report(diag, copyNotes);
        }
        else
        {
            Diagnostic note{first, first->token, "occurred during an expansion here", DiagnosticLevel::Note};
            copyNotes.push_back(&note);
            return sourceFile->report(diag, copyNotes);
        }
    }

    return sourceFile->report(diag, copyNotes);
}