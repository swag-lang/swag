#include "pch.h"
#include "Job.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "Diagnostic.h"
#include "Module.h"
#include "SemanticJob.h"
#include "Ast.h"

void Job::addDependentJob(Job* job)
{
    scoped_lock lk(executeMutex);
    dependentJobs.add(job);
}

void Job::waitForSymbolNoLock(SymbolName* symbol)
{
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

void Job::waitStructGenerated(TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->finalType;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;
    if (typeInfo->flags & TYPEINFO_GENERIC)
        return;

    auto typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    if (!typeInfoStruct->declNode)
        return;
    if (typeInfoStruct->declNode->kind == AstNodeKind::InterfaceDecl)
        return;
    if (typeInfoStruct->declNode->kind == AstNodeKind::TypeSet)
        return;

    auto        structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);
    scoped_lock lk(structNode->mutex);
    if (!(structNode->flags & AST_BYTECODE_GENERATED))
    {
        structNode->dependentJobs.add(this);
        setPending(structNode->resolvedSymbolName, "AST_BYTECODE_GENERATED", structNode, nullptr);
    }
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

void JobContext::setErrorContext(const Diagnostic& diag, vector<const Diagnostic*>& notes)
{
    if (expansionNode.size())
    {
        auto& exp = expansionNode[0];

        auto        first    = exp.first;
        const char* kindName = nullptr;
        switch (exp.second)
        {
        case JobContext::ExpansionType::Generic:
            kindName = "generic expansion";
            break;
        case JobContext::ExpansionType::Inline:
            kindName = "inline expansion";
            break;
        case JobContext::ExpansionType::SelectIf:
            kindName = "'#selectif' validation of function call";
            break;
        }

        auto& name = first->resolvedSymbolName ? first->resolvedSymbolName->name : first->token.text;
        if (name.empty())
            name = first->token.text;
        if (!name.empty())
        {
            auto note = new Diagnostic{first, first->token, format("occurred during %s ('%s')", kindName, name.c_str()), DiagnosticLevel::Note};
            notes.push_back(note);
        }
        else
        {
            auto note = new Diagnostic{first, first->token, format("occurred during %s", kindName), DiagnosticLevel::Note};
            notes.push_back(note);
        }
    }

    if (diag.sourceNode && diag.sourceNode->sourceFile && diag.sourceNode->sourceFile->sourceNode)
    {
        auto sourceNode = diag.sourceNode->sourceFile->sourceNode;
        auto note       = new Diagnostic{sourceNode, sourceNode->token, "occurred in generated code", DiagnosticLevel::Note};
        notes.push_back(note);
    }
}

bool JobContext::report(const Diagnostic& diag, const vector<const Diagnostic*>& notes)
{
    auto copyNotes = notes;
    setErrorContext(diag, copyNotes);
    SWAG_ASSERT(sourceFile);
    return sourceFile->report(diag, copyNotes);
}