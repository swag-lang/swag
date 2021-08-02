#include "pch.h"
#include "Module.h"
#include "TypeTableJob.h"
#include "Generic.h"
#include "ByteCode.h"
#include "Ast.h"

bool TypeTableJob::computeStruct()
{
    auto concreteType = (ConcreteTypeInfoStruct*) concreteTypeInfoValue;
    auto realType     = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
    auto attributes   = realType->declNode ? realType->declNode->attributeFlags : 0;

    // Flags
    if (realType->flags & TYPEINFO_STRUCT_NO_COPY)
        concreteTypeInfoValue->flags &= ~(uint16_t) TypeInfoFlags::CanCopy;
    if (realType->opPostCopy || realType->opUserPostCopyFct)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::HasPostCopy;
    if (realType->opPostMove || realType->opUserPostMoveFct)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::HasPostMove;
    if (realType->opDrop || realType->opUserDropFct)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::HasDrop;
    if (realType->flags & TYPEINFO_STRUCT_IS_TUPLE)
        concreteTypeInfoValue->flags |= (uint16_t) TypeInfoFlags::Tuple;

    // Special functions lambdas
    concreteType->opInit     = nullptr;
    concreteType->opDrop     = nullptr;
    concreteType->opPostCopy = nullptr;
    concreteType->opPostMove = nullptr;

    if (realType->opInit || (realType->opUserInitFct && realType->opUserInitFct->isForeign()))
    {
        concreteType->opInit = ByteCodeRun::makeLambda(baseContext, realType->opUserInitFct, realType->opInit);
        if (!realType->opInit)
        {
            realType->opUserInitFct->computeFullNameForeign(false);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opInit), realType->opUserInitFct->fullnameForeign, DataSegment::RelocType::Foreign);
        }
        else
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opInit), realType->opInit->getCallName(), DataSegment::RelocType::Local);
    }

    if (realType->opDrop || (realType->opUserDropFct && realType->opUserDropFct->isForeign()))
    {
        concreteType->opDrop = ByteCodeRun::makeLambda(baseContext, realType->opUserDropFct, realType->opDrop);
        if (!realType->opDrop)
        {
            realType->opUserDropFct->computeFullNameForeign(false);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opDrop), realType->opUserDropFct->fullnameForeign, DataSegment::RelocType::Foreign);
        }
        else
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opDrop), realType->opDrop->getCallName(), DataSegment::RelocType::Local);
    }

    if (realType->opPostCopy || (realType->opUserPostCopyFct && realType->opUserPostCopyFct->isForeign()))
    {
        concreteType->opPostCopy = ByteCodeRun::makeLambda(baseContext, realType->opUserPostCopyFct, realType->opPostCopy);
        if (!realType->opPostCopy)
        {
            realType->opUserPostCopyFct->computeFullNameForeign(false);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostCopy), realType->opUserPostCopyFct->fullnameForeign, DataSegment::RelocType::Foreign);
        }
        else
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostCopy), realType->opPostCopy->getCallName(), DataSegment::RelocType::Local);
    }

    if (realType->opPostMove || (realType->opUserPostMoveFct && realType->opUserPostMoveFct->isForeign()))
    {
        concreteType->opPostMove = ByteCodeRun::makeLambda(baseContext, realType->opUserPostMoveFct, realType->opPostMove);
        if (!realType->opPostMove)
        {
            realType->opUserPostMoveFct->computeFullNameForeign(false);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostMove), realType->opUserPostMoveFct->fullnameForeign, DataSegment::RelocType::Foreign);
        }
        else
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostMove), realType->opPostMove->getCallName(), DataSegment::RelocType::Local);
    }

    auto&       mapPerSeg = typeTable->getMapPerSeg(storageSegment);
    scoped_lock lk(mapPerSeg.mutex);

    // Simple structure name, without generics
    SWAG_CHECK(typeTable->makeConcreteString(baseContext, &concreteType->structName, realType->structName, storageSegment, OFFSETOF(concreteType->structName)));

    // Struct attributes
    SWAG_CHECK(typeTable->makeConcreteAttributes(baseContext, realType->attributes, concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, cflags));

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

    // Generic types
    concreteType->generics.buffer = 0;
    concreteType->generics.count  = realType->genericParameters.size();
    if (concreteType->generics.count)
    {
        uint32_t count = (uint32_t) concreteType->generics.count;
        uint32_t storageArray;
        auto     addrArray = (ConcreteTypeInfoParam*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->generics.buffer, storageArray);
        for (int param = 0; param < concreteType->generics.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageSegment, storageArray, realType->genericParameters[param], cflags));
            storageArray += sizeof(ConcreteTypeInfoParam);
        }
    }

    // Fields
    concreteType->fields.buffer = 0;
    concreteType->fields.count  = realType->fields.size();
    if (concreteType->fields.count)
    {
        uint32_t count = (uint32_t) concreteType->fields.count;
        uint32_t storageArray;
        auto     addrArray = (ConcreteTypeInfoParam*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->fields.buffer, storageArray);
        for (int param = 0; param < concreteType->fields.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageSegment, storageArray, realType->fields[param], cflags));
            storageArray += sizeof(ConcreteTypeInfoParam);
        }
    }

    // Methods
    concreteType->methods.buffer = 0;
    concreteType->methods.count  = 0;
    if (attributes & ATTRIBUTE_EXPORT_TYPE_METHODS)
    {
        concreteType->methods.count = realType->methods.size();
        if (concreteType->methods.count)
        {
            uint32_t count = (uint32_t) concreteType->methods.count;
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeInfoParam*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->methods.buffer, storageArray);
            for (int param = 0; param < concreteType->methods.count; param++)
            {
                SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageSegment, storageArray, realType->methods[param], cflags));

                // 'value' will contain a pointer to the lambda.
                // Register it for later patches
                uint32_t     fieldOffset = storageArray + offsetof(ConcreteTypeInfoParam, value);
                AstFuncDecl* funcNode    = CastAst<AstFuncDecl>(realType->methods[param]->typeInfo->declNode, AstNodeKind::FuncDecl);
                patchMethods.push_back({funcNode, fieldOffset});

                storageArray += sizeof(ConcreteTypeInfoParam);
            }
        }
    }

    // Interfaces
    concreteType->interfaces.buffer = 0;
    concreteType->interfaces.count  = realType->interfaces.size();
    if (concreteType->interfaces.count)
    {
        uint32_t count = (uint32_t) concreteType->interfaces.count;
        uint32_t storageArray;
        auto     addrArray = (ConcreteTypeInfoParam*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeInfoParam), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->interfaces.buffer, storageArray);
        for (int param = 0; param < concreteType->interfaces.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteParam(baseContext, addrArray + param, storageSegment, storageArray, realType->interfaces[param], cflags));

            // :ItfIsConstantSeg
            // Compute the storage of the interface for @interfaceof
            uint32_t fieldOffset = offsetof(ConcreteTypeInfoParam, value);
            uint32_t valueOffset = storageArray + fieldOffset;
            storageSegment->addInitPtr(valueOffset, realType->interfaces[param]->offset, SegmentKind::Constant);
            addrArray[param].value = module->constantSegment.address(realType->interfaces[param]->offset);

            storageArray += sizeof(ConcreteTypeInfoParam);
        }
    }

    typeTable->tableJobDone(this, storageSegment);

    return true;
}

JobResult TypeTableJob::execute()
{
    JobContext context;
    context.sourceFile = sourceFile;
    context.baseJob    = this;
    context.node       = nodes.front();
    baseContext        = &context;

    SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Struct || typeInfo->kind == TypeInfoKind::Interface);
    auto realType = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);

    waitTypeCompleted(typeInfo);
    if (baseContext->result == ContextResult::Pending)
        return JobResult::KeepJobAlive;
    waitForAllStructInterfaces(realType);
    if (baseContext->result == ContextResult::Pending)
        return JobResult::KeepJobAlive;
    waitForAllStructMethods(realType);
    if (baseContext->result == ContextResult::Pending)
        return JobResult::KeepJobAlive;
    waitStructGenerated(realType);
    if (baseContext->result == ContextResult::Pending)
        return JobResult::KeepJobAlive;

    // We also wait for dependencies, because we need to know the foreign address of special
    // functions that will be stored in the struct type.
    // And we cannot retrieve thoses addresses before the dlls have been generated.
    if (!sourceFile->module->waitForDependenciesDone(this))
        return JobResult::KeepJobAlive;

    computeStruct();

    return JobResult::ReleaseJob;
}
