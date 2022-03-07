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

    ScopedLock lk(structNode->mutex);
    if (!(structNode->semFlags & AST_SEM_BYTECODE_GENERATED))
    {
        structNode->dependentJobs.add(this);
        setPending(structNode->resolvedSymbolName, JobWaitKind::SemByteCodeGenerated, structNode, nullptr);
    }
}

void Job::waitOverloadCompleted(SymbolOverload* overload)
{
    {
        SharedLock lk(overload->mutexIncomplete);
        if (!(overload->flags & OVERLOAD_INCOMPLETE))
            return;
    }

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
    ScopedLock lk(funcDecl->funcMutex);
    if (!(funcDecl->funcFlags & FUNC_FLAG_FULL_RESOLVE))
    {
        funcDecl->dependentJobs.add(this);
        setPending(funcDecl->resolvedSymbolName, JobWaitKind::SemFullResolve, funcDecl, nullptr);
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
    waitingHintNode     = nullptr;
    waitingIdType       = typeInfo;
    baseContext->result = ContextResult::Pending;
}

bool JobContext::report(const char* hint, const Diagnostic& diag, const Diagnostic* note, const Diagnostic* note1)
{
    PushErrHint errh(hint);
    return report(diag, note, note1);
}

bool JobContext::report(AstNode* fromNode, const Utf8& msg)
{
    Diagnostic diag{fromNode, msg};
    return report(diag);
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

    if (errorContextStack.size())
    {
        bool doneGeneric = false;
        bool doneInline  = false;
        for (int i = 0; i < (int) errorContextStack.size(); i++)
        {
            auto& exp = errorContextStack[i];
            switch (exp.type)
            {
            case JobContext::ErrorContextType::Generic:
                exp.hide    = doneGeneric;
                doneGeneric = true;
                break;
            case JobContext::ErrorContextType::Inline:
                exp.hide   = doneInline;
                doneInline = true;
                break;
            }
        }

        for (int i = (int) errorContextStack.size() - 1; i >= 0; i--)
        {
            auto& exp = errorContextStack[i];
            if (exp.hide)
                continue;

            auto        first       = exp.node;
            const char* kindName    = nullptr;
            const char* kindArticle = "";
            bool        showContext = true;
            Utf8        hint;
            switch (exp.type)
            {
            case JobContext::ErrorContextType::Message:
            {
                showContext = false;
                if (exp.msg.empty())
                    break;
                if (first)
                {
                    auto note = new Diagnostic{first, exp.msg, exp.level};
                    notes.push_back(note);
                }
                else
                {
                    auto note = new Diagnostic{exp.msg, exp.level};
                    notes.push_back(note);
                }
                break;
            }
            case JobContext::ErrorContextType::Export:
                kindName    = Err(Err0111);
                kindArticle = "of ";
                break;
            case JobContext::ErrorContextType::Generic:
                kindName    = Err(Err0112);
                kindArticle = "of ";
                break;
            case JobContext::ErrorContextType::Inline:
                kindName    = Err(Err0118);
                kindArticle = "of ";
                break;
            case JobContext::ErrorContextType::SelectIf:
                kindName    = Err(Err0128);
                kindArticle = "to ";
                break;
            case JobContext::ErrorContextType::CheckIf:
                kindName    = Err(Err0129);
                kindArticle = "to ";
                break;
            case JobContext::ErrorContextType::Node:
                kindName    = "when solving";
                kindArticle = "";
                switch (first->kind)
                {
                case AstNodeKind::AffectOp:
                    kindName    = "when solving affectation";
                    kindArticle = "to ";
                    first       = first->childs.front();
                    hint        = Hint::isType(first->typeInfo);
                    break;
                case AstNodeKind::Return:
                {
                    auto returnNode = CastAst<AstReturn>(first, AstNodeKind::Return);
                    if (returnNode->resolvedFuncDecl)
                    {
                        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(returnNode->resolvedFuncDecl->typeInfo, TypeInfoKind::FuncAttr);
                        first         = returnNode->resolvedFuncDecl->returnType;
                        if (!first->childs.empty())
                            first = first->childs.front();
                        auto note = new Diagnostic{first, Fmt(Nte(Nte0067), typeFunc->returnType->getDisplayNameC()), DiagnosticLevel::Note};
                        notes.push_back(note);
                        showContext = false;
                    }
                    else
                        showContext = false;
                }
                break;
                default:
                    showContext = false;
                    break;
                }

                break;
            }

            if (showContext)
            {
                auto name = first->resolvedSymbolName ? first->resolvedSymbolName->name : first->token.text;
                if (name.empty())
                    name = first->token.text;

                Utf8 msg;
                if (!name.empty())
                    msg = Fmt(Nte(Nte0002), kindName, kindArticle, name.c_str());
                else
                    msg = Fmt(Nte(Nte0003), kindName);
                auto note  = new Diagnostic{first, msg, DiagnosticLevel::Note};
                note->hint = hint;
                notes.push_back(note);
            }
        }
    }

    // Generated code
    auto sourceNode = diag.sourceNode;
    if (sourceNode && sourceNode->extension && sourceNode->extension->exportNode)
        sourceNode = diag.sourceNode->extension->exportNode;

    if (sourceNode && sourceNode->sourceFile && sourceNode->sourceFile->sourceNode && !sourceNode->sourceFile->fileForSourceLocation)
    {
        auto fileSourceNode = sourceNode->sourceFile->sourceNode;
        auto note           = new Diagnostic{fileSourceNode, Nte(Nte0004), DiagnosticLevel::Note};
        notes.push_back(note);
    }
}

bool JobContext::report(const char* hint, const Diagnostic& diag, const vector<const Diagnostic*>& notes)
{
    PushErrHint errh(hint);
    return report(diag, notes);
}

bool JobContext::report(const Diagnostic& diag, const vector<const Diagnostic*>& notes)
{
    if (silentError)
        return false;

    auto copyNotes = notes;
    setErrorContext(diag, copyNotes);
    SWAG_ASSERT(sourceFile);
    return sourceFile->report(diag, copyNotes);
}

bool JobContext::checkSizeOverflow(const char* typeOverflow, uint64_t value, uint64_t maxValue)
{
    if (value <= maxValue)
        return true;
    return report({node, Fmt(Err(Err0505), typeOverflow, maxValue)});
}

bool JobContext::internalError(const char* msg, AstNode* specNode)
{
    if (!specNode)
        specNode = node;
    return sourceFile->internalError(specNode, msg);
}