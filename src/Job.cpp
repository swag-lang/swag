#include "pch.h"
#include "Job.h"
#include "Diagnostic.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ErrorIds.h"

void Job::addDependentJob(Job* job)
{
    ScopedLock lk(executeMutex);
    dependentJobs.add(job);
}

void Job::waitSymbolNoLock(SymbolName* symbol)
{
    setPending(symbol, JobWaitKind::WaitSymbol, nullptr, nullptr);
    symbol->addDependentJobNoLock(this);
}

void Job::waitAllStructInterfacesReg(TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*)typeInfo)->pointedType;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);

    if (module->waitImplForToSolve(this, typeInfoStruct))
    {
        setPending(nullptr, JobWaitKind::WaitInterfacesFor, nullptr, typeInfoStruct);
        return;
    }

    if (typeInfoStruct->cptRemainingInterfacesReg == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(nullptr, JobWaitKind::WaitInterfacesReg, nullptr, typeInfoStruct);
}

void Job::waitAllStructInterfaces(TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);

    if (module->waitImplForToSolve(this, typeInfoStruct))
    {
        setPending(nullptr, JobWaitKind::WaitInterfacesFor, nullptr, typeInfoStruct);
        return;
    }

    if (typeInfoStruct->cptRemainingInterfaces == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(nullptr, JobWaitKind::WaitInterfaces, nullptr, typeInfoStruct);
}

void Job::waitAllStructSpecialMethods(TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->cptRemainingSpecialMethods == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(nullptr, JobWaitKind::WaitSpecialMethods, nullptr, typeInfoStruct);
}

void Job::waitAllStructMethods(TypeInfo* typeInfo)
{
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    auto       typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    ScopedLock lk(typeInfoStruct->mutex);
    if (typeInfoStruct->cptRemainingMethods == 0)
        return;
    SWAG_ASSERT(typeInfoStruct->declNode);
    SWAG_ASSERT(typeInfoStruct->scope);
    ScopedLock lk1(typeInfoStruct->scope->symTable.mutex);
    typeInfoStruct->scope->dependentJobs.add(this);
    setPending(nullptr, JobWaitKind::WaitMethods, nullptr, typeInfoStruct);
}

void Job::waitStructGenerated(TypeInfo* typeInfo)
{
    if (typeInfo->isArrayOfStruct())
        typeInfo = ((TypeInfoArray*) typeInfo)->finalType;
    if (typeInfo->isPointerTo(TypeInfoKind::Struct))
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;
    if (typeInfo->flags & TYPEINFO_GENERIC)
        return;

    auto typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    if (!typeInfoStruct->declNode)
        return;
    if (typeInfoStruct->declNode->kind == AstNodeKind::InterfaceDecl)
        return;

    auto structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);
    bool mustWait   = false;

    {
        SharedLock lk(structNode->mutex);
        mustWait = !(structNode->semFlags & AST_SEM_BYTECODE_GENERATED);
    }

    if (mustWait)
    {
        ScopedLock lk(structNode->mutex);
        if (!(structNode->semFlags & AST_SEM_BYTECODE_GENERATED))
        {
            structNode->dependentJobs.add(this);
            setPending(structNode->resolvedSymbolName, JobWaitKind::SemByteCodeGenerated, structNode, nullptr);
        }
    }
}

void Job::waitOverloadCompleted(SymbolOverload* overload)
{
    bool mustWait = false;

    {
        SharedLock lk(overload->symbol->mutex);
        mustWait = overload->flags & OVERLOAD_INCOMPLETE;
    }

    if (mustWait)
    {
        ScopedLock lk(overload->symbol->mutex);
        if (overload->flags & OVERLOAD_INCOMPLETE)
        {
            waitSymbolNoLock(overload->symbol);
            return;
        }
    }
}

void Job::waitFuncDeclFullResolve(AstFuncDecl* funcDecl)
{
    bool mustWait = false;

    {
        SharedLock lk(funcDecl->mutex);
        mustWait = !(funcDecl->semFlags & AST_SEM_FULL_RESOLVE);
    }

    if (mustWait)
    {
        ScopedLock lk(funcDecl->mutex);
        if (!(funcDecl->semFlags & AST_SEM_FULL_RESOLVE))
        {
            funcDecl->dependentJobs.add(this);
            setPending(funcDecl->resolvedSymbolName, JobWaitKind::SemFullResolve, funcDecl, nullptr);
        }
    }
}

void Job::waitTypeCompleted(TypeInfo* typeInfo)
{
    if (!typeInfo)
        return;
    auto orgTypeInfo = typeInfo;
    typeInfo         = TypeManager::concreteType(typeInfo);
    if (typeInfo->kind == TypeInfoKind::Slice)
        typeInfo = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice)->pointedType;
    if (typeInfo->kind == TypeInfoKind::Array)
        typeInfo = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array)->finalType;
    typeInfo = TypeManager::concreteType(typeInfo);
    if (typeInfo->kind != TypeInfoKind::Struct && typeInfo->kind != TypeInfoKind::Interface)
        return;
    if (!typeInfo->declNode)
        return;

    SWAG_ASSERT(typeInfo->declNode->resolvedSymbolOverload);
    waitOverloadCompleted(typeInfo->declNode->resolvedSymbolOverload);
    if (baseContext->result == ContextResult::Pending)
        return;

    // Be sure type sizeof is correct, because it could have been created before the
    // raw type has been completed because of //

    orgTypeInfo = TypeManager::concreteType(orgTypeInfo);
    if (orgTypeInfo->kind == TypeInfoKind::Array)
        orgTypeInfo->sizeOf = ((TypeInfoArray*) orgTypeInfo)->finalType->sizeOf * ((TypeInfoArray*) orgTypeInfo)->totalCount;
    if (orgTypeInfo->flags & TYPEINFO_FAKE_ALIAS)
        orgTypeInfo->sizeOf = ((TypeInfoAlias*) orgTypeInfo)->rawType->sizeOf;
    if (typeInfo->flags & TYPEINFO_FAKE_ALIAS)
        typeInfo->sizeOf = ((TypeInfoAlias*) typeInfo)->rawType->sizeOf;
}

void Job::setPending(SymbolName* symbolToWait, JobWaitKind waitKind, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_ASSERT(baseContext);
    if (baseContext->result == ContextResult::Pending)
        return;
    waitingSymbolSolved = symbolToWait;
    waitingKind         = waitKind;
    waitingIdNode       = node;
    waitingIdType       = typeInfo;
    baseContext->result = ContextResult::Pending;
}

bool JobContext::report(const char* hint, const Diagnostic& diag, const Diagnostic* note, const Diagnostic* note1)
{
    PushErrHint errh(hint);
    return report(diag, note, note1);
}

bool JobContext::report(const Diagnostic& diag, const Diagnostic* note, const Diagnostic* note1)
{
    vector<const Diagnostic*> notes;
    if (note)
        notes.push_back(note);
    if (note1)
        notes.push_back(note1);
    if (!sourceFile)
        sourceFile = diag.sourceFile;
    return report(diag, notes);
}

void JobContext::setErrorContext(const Diagnostic& diag, vector<const Diagnostic*>& notes)
{
    if (diag.errorLevel == DiagnosticLevel::Error)
        hasError = true;

    if (expansionNode.size())
    {
        auto& exp = expansionNode[0];

        auto        first       = exp.first;
        const char* kindName    = nullptr;
        const char* kindArticle = "";
        bool        showExpand  = true;
        Utf8        hint;
        switch (exp.second)
        {
        case JobContext::ExpansionType::Generic:
            kindName    = g_E[Err0112];
            kindArticle = "of ";
            break;
        case JobContext::ExpansionType::Inline:
            kindName    = g_E[Err0118];
            kindArticle = "of ";
            break;
        case JobContext::ExpansionType::SelectIf:
            kindName    = g_E[Err0128];
            kindArticle = "to ";
            break;
        case JobContext::ExpansionType::CheckIf:
            kindName    = g_E[Err0129];
            kindArticle = "to ";
            break;
        case JobContext::ExpansionType::Node:
            kindName    = g_E[Nte0017];
            kindArticle = "";
            if (first->kind == AstNodeKind::AffectOp)
            {
                kindName    = g_E[Nte0018];
                kindArticle = "to ";
                first       = first->childs.front();
                hint        = Utf8::format(g_E[Hnt0011], first->typeInfo->getDisplayName().c_str());
            }
            else if (first->kind == AstNodeKind::Return)
            {
                auto returnNode = CastAst<AstReturn>(first, AstNodeKind::Return);
                if (returnNode->resolvedFuncDecl)
                {
                    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(returnNode->resolvedFuncDecl->typeInfo, TypeInfoKind::FuncAttr);
                    hint          = Utf8::format(g_E[Hnt0012], returnNode->resolvedFuncDecl->getDisplayName().c_str(), typeFunc->returnType->getDisplayName().c_str());
                }
            }
            else
            {
                showExpand = false;
            }

            break;
        }

        if (showExpand)
        {
            auto name = first->resolvedSymbolName ? first->resolvedSymbolName->name : first->token.text;
            if (name.empty())
                name = first->token.text;

            if (!name.empty())
            {
                auto note  = new Diagnostic{first, Utf8::format(g_E[Nte0002], kindName, kindArticle, name.c_str()), DiagnosticLevel::Note};
                note->hint = hint;
                notes.push_back(note);
            }
            else
            {
                auto note  = new Diagnostic{first, Utf8::format(g_E[Nte0003], kindName), DiagnosticLevel::Note};
                note->hint = hint;
                notes.push_back(note);
            }
        }
    }

    if (diag.sourceNode && diag.sourceNode->sourceFile && diag.sourceNode->sourceFile->sourceNode)
    {
        auto sourceNode = diag.sourceNode->sourceFile->sourceNode;
        auto note       = new Diagnostic{sourceNode, g_E[Nte0004], DiagnosticLevel::Note};
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

bool JobContext::checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue)
{
    if (value <= maxValue)
        return true;
    return report({node, Utf8::format(g_E[Err0505], typeOverflow, maxValue)});
}

bool JobContext::internalError(const char* msg, AstNode* specNode)
{
    if (!specNode)
        specNode = node;
    return sourceFile->internalError(specNode, msg);
}