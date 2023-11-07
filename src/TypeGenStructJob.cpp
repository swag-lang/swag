#include "pch.h"
#include "TypeGenStructJob.h"
#include "Ast.h"
#include "ByteCode.h"
#include "Generic.h"
#include "Module.h"
#include "ModuleManager.h"

bool TypeGenStructJob::computeStruct()
{
    auto concreteType = (ExportedTypeInfoStruct*) exportedTypeInfoValue;
    auto realType     = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
    auto attributes   = realType->declNode ? realType->declNode->attributeFlags : 0;

    concreteType->base.sizeOf = realType->sizeOf;

    // Flags
    if (!(cflags & GEN_EXPORTED_TYPE_PARTIAL))
    {
        if (realType->flags & TYPEINFO_STRUCT_NO_COPY)
            exportedTypeInfoValue->flags &= ~(uint16_t) ExportedTypeInfoFlags::CanCopy;
        if (realType->opPostCopy || realType->opUserPostCopyFct)
            exportedTypeInfoValue->flags |= (uint32_t) ExportedTypeInfoFlags::HasPostCopy;
        if (realType->opPostMove || realType->opUserPostMoveFct)
            exportedTypeInfoValue->flags |= (uint32_t) ExportedTypeInfoFlags::HasPostMove;
        if (realType->opDrop || realType->opUserDropFct)
            exportedTypeInfoValue->flags |= (uint32_t) ExportedTypeInfoFlags::HasDrop;
    }

    // Special functions lambdas
    concreteType->opInit     = nullptr;
    concreteType->opDrop     = nullptr;
    concreteType->opPostCopy = nullptr;
    concreteType->opPostMove = nullptr;

    if (!(cflags & GEN_EXPORTED_TYPE_PARTIAL))
    {
        Utf8 callName;
        if (realType->opUserInitFct && realType->opUserInitFct->isForeign())
        {
            realType->opUserInitFct->computeFullNameForeign(false);
            g_ModuleMgr->addPatchFuncAddress(storageSegment, &concreteType->opInit, realType->opUserInitFct);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opInit), realType->opUserInitFct->fullnameForeign);
        }
        else if (realType->opInit)
        {
            concreteType->opInit        = ByteCodeRun::makeLambda(baseContext, realType->opUserInitFct, realType->opInit);
            realType->opInit->isUsed    = true;
            realType->opInit->isInSeg   = true;
            realType->opInit->forceEmit = true;
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opInit), realType->opInit->getCallNameFromDecl());
        }

        if (realType->opUserDropFct && realType->opUserDropFct->isForeign())
        {
            realType->opUserDropFct->computeFullNameForeign(false);
            g_ModuleMgr->addPatchFuncAddress(storageSegment, &concreteType->opDrop, realType->opUserDropFct);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opDrop), realType->opUserDropFct->fullnameForeign);
        }
        else if (realType->opDrop)
        {
            concreteType->opDrop        = ByteCodeRun::makeLambda(baseContext, realType->opUserDropFct, realType->opDrop);
            realType->opDrop->isUsed    = true;
            realType->opDrop->isInSeg   = true;
            realType->opDrop->forceEmit = true;
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opDrop), realType->opDrop->getCallNameFromDecl());
        }

        if (realType->opUserPostCopyFct && realType->opUserPostCopyFct->isForeign())
        {
            realType->opUserPostCopyFct->computeFullNameForeign(false);
            g_ModuleMgr->addPatchFuncAddress(storageSegment, &concreteType->opPostCopy, realType->opUserPostCopyFct);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostCopy), realType->opUserPostCopyFct->fullnameForeign);
        }
        else if (realType->opPostCopy)
        {
            concreteType->opPostCopy        = ByteCodeRun::makeLambda(baseContext, realType->opUserPostCopyFct, realType->opPostCopy);
            realType->opPostCopy->isUsed    = true;
            realType->opPostCopy->isInSeg   = true;
            realType->opPostCopy->forceEmit = true;
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostCopy), realType->opPostCopy->getCallNameFromDecl());
        }

        if (realType->opUserPostMoveFct && realType->opUserPostMoveFct->isForeign())
        {
            realType->opUserPostMoveFct->computeFullNameForeign(false);
            g_ModuleMgr->addPatchFuncAddress(storageSegment, &concreteType->opPostMove, realType->opUserPostMoveFct);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostMove), realType->opUserPostMoveFct->fullnameForeign);
        }
        else if (realType->opPostMove)
        {
            concreteType->opPostMove        = ByteCodeRun::makeLambda(baseContext, realType->opUserPostMoveFct, realType->opPostMove);
            realType->opPostMove->isUsed    = true;
            realType->opPostMove->isInSeg   = true;
            realType->opPostMove->forceEmit = true;
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostMove), realType->opPostMove->getCallNameFromDecl());
        }
    }

    auto&      mapPerSeg = typeGen->getMapPerSeg(storageSegment);
    ScopedLock lk(mapPerSeg.mutex);

    // Simple structure name, without generics
    SWAG_CHECK(typeGen->genExportedString(baseContext, &concreteType->structName, realType->structName, storageSegment, OFFSETOF(concreteType->structName)));

    // Struct attributes
    SWAG_CHECK(typeGen->genExportedAttributes(baseContext, realType->attributes, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, cflags));

    if (!realType->replaceTypes.empty())
    {
        // Update fields with types if generic
        for (auto field : realType->fields)
        {
            field->typeInfo = Generic::doTypeSubstitution(realType->replaceTypes, field->typeInfo);
        }

        // Update methods with types if generic
        if (attributes & ATTRIBUTE_EXPORT_TYPE_METHODS)
        {
            for (auto method : realType->methods)
            {
                method->typeInfo = Generic::doTypeSubstitution(realType->replaceTypes, method->typeInfo);
            }
        }
    }

    // Parent generic type
    concreteType->fromGeneric = nullptr;
    if (realType->fromGeneric)
    {
        SWAG_CHECK(typeGen->genExportedSubTypeInfo(baseContext, &concreteType->fromGeneric, concreteType, storageSegment, storageOffset, realType->fromGeneric, cflags));
    }

    // Generic types
    concreteType->generics.buffer = 0;
    concreteType->generics.count  = realType->genericParameters.size();
    if (concreteType->generics.count)
    {
        uint32_t count = (uint32_t) concreteType->generics.count;
        uint32_t storageArray;
        auto     addrArray = (ExportedTypeValue*) typeGen->genExportedSlice(baseContext,
                                                                        count * sizeof(ExportedTypeValue),
                                                                        exportedTypeInfoValue,
                                                                        storageSegment,
                                                                        storageOffset,
                                                                        &concreteType->generics.buffer,
                                                                        storageArray);
        for (size_t param = 0; param < concreteType->generics.count; param++)
        {
            SWAG_CHECK(typeGen->genExportedTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->genericParameters[param], cflags));
            storageArray += sizeof(ExportedTypeValue);
        }
    }

    // Fields
    concreteType->fields.buffer = 0;
    concreteType->fields.count  = 0;
    if ((attributes & ATTRIBUTE_EXPORT_TYPE_METHODS) || !(realType->flags & TYPEINFO_STRUCT_IS_ITABLE))
    {
        concreteType->fields.count = realType->fields.size();
        if (concreteType->fields.count)
        {
            uint32_t count = (uint32_t) concreteType->fields.count;
            uint32_t storageArray;
            auto     addrArray = (ExportedTypeValue*) typeGen->genExportedSlice(baseContext,
                                                                            count * sizeof(ExportedTypeValue),
                                                                            exportedTypeInfoValue,
                                                                            storageSegment,
                                                                            storageOffset,
                                                                            &concreteType->fields.buffer,
                                                                            storageArray);
            for (size_t param = 0; param < concreteType->fields.count; param++)
            {
                SWAG_CHECK(typeGen->genExportedTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->fields[param], cflags));
                storageArray += sizeof(ExportedTypeValue);
            }
        }
    }

    // Methods
    concreteType->methods.buffer = 0;
    concreteType->methods.count  = 0;
    if (!(cflags & GEN_EXPORTED_TYPE_PARTIAL))
    {
        if (attributes & ATTRIBUTE_EXPORT_TYPE_METHODS)
        {
            concreteType->methods.count = realType->methods.size();
            if (concreteType->methods.count)
            {
                uint32_t count = (uint32_t) concreteType->methods.count;
                uint32_t storageArray;
                auto     addrArray = (ExportedTypeValue*) typeGen->genExportedSlice(baseContext,
                                                                                count * sizeof(ExportedTypeValue),
                                                                                exportedTypeInfoValue,
                                                                                storageSegment,
                                                                                storageOffset,
                                                                                &concreteType->methods.buffer,
                                                                                storageArray);
                for (size_t param = 0; param < concreteType->methods.count; param++)
                {
                    SWAG_CHECK(typeGen->genExportedTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->methods[param], cflags));

                    // 'value' will contain a pointer to the lambda.
                    // Register it for later patches
                    uint32_t     fieldOffset = storageArray + offsetof(ExportedTypeValue, value);
                    AstFuncDecl* funcNode    = CastAst<AstFuncDecl>(realType->methods[param]->typeInfo->declNode, AstNodeKind::FuncDecl);
                    patchMethods.push_back({funcNode, fieldOffset});

                    storageArray += sizeof(ExportedTypeValue);
                }
            }
        }
    }

    // Interfaces
    concreteType->interfaces.buffer = 0;
    concreteType->interfaces.count  = 0;
    if (!(cflags & GEN_EXPORTED_TYPE_PARTIAL))
    {
        concreteType->interfaces.count = realType->interfaces.size();
        if (concreteType->interfaces.count)
        {
            uint32_t count = (uint32_t) concreteType->interfaces.count;
            uint32_t storageArray;
            auto     addrArray = (ExportedTypeValue*) typeGen->genExportedSlice(baseContext,
                                                                            count * sizeof(ExportedTypeValue),
                                                                            exportedTypeInfoValue,
                                                                            storageSegment,
                                                                            storageOffset,
                                                                            &concreteType->interfaces.buffer,
                                                                            storageArray);
            for (size_t param = 0; param < concreteType->interfaces.count; param++)
            {
                SWAG_CHECK(typeGen->genExportedTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->interfaces[param], cflags));

                // :ItfIsConstantSeg
                uint32_t fieldOffset = offsetof(ExportedTypeValue, value);
                uint32_t valueOffset = storageArray + fieldOffset;
                storageSegment->addInitPtr(valueOffset, realType->interfaces[param]->offset, SegmentKind::Constant);
                addrArray[param].value = module->constantSegment.address(realType->interfaces[param]->offset);

                storageArray += sizeof(ExportedTypeValue);
            }
        }
    }

    typeGen->tableJobDone(this, storageSegment);

    return true;
}

JobResult TypeGenStructJob::execute()
{
    JobContext context;
    context.sourceFile = sourceFile;
    context.baseJob    = this;
    context.node       = nodes.front();
    baseContext        = &context;

    SWAG_ASSERT(typeInfo->isStruct() || typeInfo->isInterface());
    auto realType = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);

    waitTypeCompleted(typeInfo);
    if (baseContext->result != ContextResult::Done)
        return JobResult::KeepJobAlive;

    if (!(cflags & GEN_EXPORTED_TYPE_PARTIAL))
    {
        waitAllStructInterfaces(realType);
        if (baseContext->result != ContextResult::Done)
            return JobResult::KeepJobAlive;
        waitAllStructMethods(realType);
        if (baseContext->result != ContextResult::Done)
            return JobResult::KeepJobAlive;
        waitStructGeneratedAlloc(realType);
        if (baseContext->result != ContextResult::Done)
            return JobResult::KeepJobAlive;
    }

    computeStruct();

    return JobResult::ReleaseJob;
}
