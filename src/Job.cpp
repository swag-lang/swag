#include "pch.h"
#include "Job.h"
#include "Diagnostic.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ErrorIds.h"

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
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
    if (typeInfo->kind != TypeInfoKind::Struct)
        return;

    auto        typeInfoStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
    scoped_lock lk(typeInfoStruct->mutex);

    if (module->waitImplForToSolve(this, typeInfoStruct))
    {
        setPending(nullptr, "WAIT_INTERFACES_FOR", nullptr, typeInfoStruct);
        return;
    }

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
        typeInfo = ((TypeInfoPointer*) typeInfo)->pointedType;
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

    auto        structNode = CastAst<AstStruct>(typeInfoStruct->declNode, AstNodeKind::StructDecl);
    scoped_lock lk(structNode->mutex);
    if (!(structNode->semFlags & AST_SEM_BYTECODE_GENERATED))
    {
        structNode->dependentJobs.add(this);
        setPending(structNode->resolvedSymbolName, "AST_SEM_BYTECODE_GENERATED", structNode, nullptr);
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

    auto symbol   = typeInfo->declNode->resolvedSymbolName;
    auto overload = typeInfo->declNode->resolvedSymbolOverload;
    SWAG_ASSERT(symbol && overload);
    scoped_lock lk(symbol->mutex);
    if (overload->flags & OVERLOAD_INCOMPLETE)
    {
        SWAG_ASSERT(overload->symbol == symbol);
        waitForSymbolNoLock(symbol);
        return;
    }

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

void Job::setPending(SymbolName* symbolToWait, const char* id, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_ASSERT(baseContext);
    waitingSymbolSolved = symbolToWait;
    waitingId           = id;
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
        Utf8        hint;
        switch (exp.second)
        {
        case JobContext::ExpansionType::Generic:
            kindName    = Msg0112;
            kindArticle = "of ";
            break;
        case JobContext::ExpansionType::Inline:
            kindName    = Msg0118;
            kindArticle = "of ";
            break;
        case JobContext::ExpansionType::SelectIf:
            kindName    = Msg0128;
            kindArticle = "to ";
            break;
        case JobContext::ExpansionType::CheckIf:
            kindName    = Msg0129;
            kindArticle = "to ";
            break;
        case JobContext::ExpansionType::Node:
            kindName    = Msg0134;
            kindArticle = "";
            if (first->kind == AstNodeKind::AffectOp)
            {
                kindName    = Msg0131;
                kindArticle = "to ";
                first       = first->childs.front();
                hint        = Utf8::format(Hnt0011, first->typeInfo->getDisplayName().c_str());
            }
            else if (first->kind == AstNodeKind::Return)
            {
                auto returnNode = CastAst<AstReturn>(first, AstNodeKind::Return);
                if (returnNode->resolvedFuncDecl)
                {
                    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(returnNode->resolvedFuncDecl->typeInfo, TypeInfoKind::FuncAttr);
                    hint          = Utf8::format(Hnt0012, returnNode->resolvedFuncDecl->getDisplayName().c_str(), typeFunc->returnType->getDisplayName().c_str());
                }
            }

            break;
        }

        auto name = first->resolvedSymbolName ? first->resolvedSymbolName->name : first->token.text;
        if (name.empty())
            name = first->token.text;

        if (!name.empty())
        {
            auto note  = new Diagnostic{first, first->token, Utf8::format(Note002, kindName, kindArticle, name.c_str()), DiagnosticLevel::Note};
            note->hint = hint;
            notes.push_back(note);
        }
        else
        {
            auto note  = new Diagnostic{first, first->token, Utf8::format(Note003, kindName), DiagnosticLevel::Note};
            note->hint = hint;
            notes.push_back(note);
        }
    }

    if (diag.sourceNode && diag.sourceNode->sourceFile && diag.sourceNode->sourceFile->sourceNode)
    {
        auto sourceNode = diag.sourceNode->sourceFile->sourceNode;
        auto note       = new Diagnostic{sourceNode, sourceNode->token, Note004, DiagnosticLevel::Note};
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
    return report({node, Utf8::format(Msg0505, typeOverflow, maxValue)});
}

bool JobContext::internalError(const char* msg, AstNode* specNode)
{
    if (!specNode)
        specNode = node;
    sourceFile->report({specNode, Utf8::format(Msg0058, msg)});
    return false;
}