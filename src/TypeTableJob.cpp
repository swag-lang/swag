#include "pch.h"
#include "Module.h"
#include "TypeTableJob.h"
#include "Generic.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ModuleManager.h"

bool TypeTableJob::computeStruct()
{
    auto concreteType = (ConcreteTypeInfoStruct*) concreteTypeInfoValue;
    auto realType     = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
    auto attributes   = realType->declNode ? realType->declNode->attributeFlags : 0;

    concreteType->base.sizeOf = realType->sizeOf;

    // Flags
    if (!(cflags & MAKE_CONCRETE_PARTIAL))
    {
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
    }

    // Special functions lambdas
    concreteType->opInit     = nullptr;
    concreteType->opDrop     = nullptr;
    concreteType->opPostCopy = nullptr;
    concreteType->opPostMove = nullptr;

    if (!(cflags & MAKE_CONCRETE_PARTIAL))
    {
        Utf8 callName;
        if (realType->opUserInitFct && realType->opUserInitFct->isForeign())
        {
            realType->opUserInitFct->computeFullNameForeign(false);
            g_ModuleMgr->addPatchFuncAddress(&concreteType->opInit, realType->opUserInitFct);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opInit), realType->opUserInitFct->fullnameForeign);
        }
        else if (realType->opInit)
        {
            concreteType->opInit        = ByteCodeRun::makeLambda(baseContext, realType->opUserInitFct, realType->opInit);
            realType->opInit->isUsed    = true;
            realType->opInit->forceEmit = true;

            if (realType->opInit->node)
            {
                auto funcNode = CastAst<AstFuncDecl>(realType->opInit->node, AstNodeKind::FuncDecl);
                callName = funcNode->getCallName();
            }
            else
                callName = realType->opInit->getCallName();

            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opInit), callName);
        }

        if (realType->opUserDropFct && realType->opUserDropFct->isForeign())
        {
            realType->opUserDropFct->computeFullNameForeign(false);
            g_ModuleMgr->addPatchFuncAddress(&concreteType->opDrop, realType->opUserDropFct);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opDrop), realType->opUserDropFct->fullnameForeign);
        }
        else if (realType->opDrop)
        {
            concreteType->opDrop        = ByteCodeRun::makeLambda(baseContext, realType->opUserDropFct, realType->opDrop);
            realType->opDrop->isUsed    = true;
            realType->opDrop->forceEmit = true;

            if (realType->opDrop->node)
            {
                auto funcNode = CastAst<AstFuncDecl>(realType->opDrop->node, AstNodeKind::FuncDecl);
                callName = funcNode->getCallName();
            }
            else
                callName = realType->opDrop->getCallName();

            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opDrop), callName);
        }

        if (realType->opUserPostCopyFct && realType->opUserPostCopyFct->isForeign())
        {
            realType->opUserPostCopyFct->computeFullNameForeign(false);
            g_ModuleMgr->addPatchFuncAddress(&concreteType->opPostCopy, realType->opUserPostCopyFct);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostCopy), realType->opUserPostCopyFct->fullnameForeign);
        }
        else if (realType->opPostCopy)
        {
            concreteType->opPostCopy        = ByteCodeRun::makeLambda(baseContext, realType->opUserPostCopyFct, realType->opPostCopy);
            realType->opPostCopy->isUsed    = true;
            realType->opPostCopy->forceEmit = true;

            if (realType->opPostCopy->node)
            {
                auto funcNode = CastAst<AstFuncDecl>(realType->opPostCopy->node, AstNodeKind::FuncDecl);
                callName = funcNode->getCallName();
            }
            else
                callName = realType->opPostCopy->getCallName();

            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostCopy), callName);
        }

        if (realType->opUserPostMoveFct && realType->opUserPostMoveFct->isForeign())
        {
            realType->opUserPostMoveFct->computeFullNameForeign(false);
            g_ModuleMgr->addPatchFuncAddress(&concreteType->opPostMove, realType->opUserPostMoveFct);
            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostMove), realType->opUserPostMoveFct->fullnameForeign);
        }
        else if (realType->opPostMove)
        {
            concreteType->opPostMove        = ByteCodeRun::makeLambda(baseContext, realType->opUserPostMoveFct, realType->opPostMove);
            realType->opPostMove->isUsed    = true;
            realType->opPostMove->forceEmit = true;

            if (realType->opPostMove->node)
            {
                auto funcNode = CastAst<AstFuncDecl>(realType->opPostMove->node, AstNodeKind::FuncDecl);
                callName = funcNode->getCallName();
            }
            else
                callName = realType->opPostMove->getCallName();

            storageSegment->addInitPtrFunc(OFFSETOF(concreteType->opPostMove), callName);
        }
    }

    auto&      mapPerSeg = typeTable->getMapPerSeg(storageSegment);
    ScopedLock lk(mapPerSeg.mutex);

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

    // Parent generic type
    concreteType->fromGeneric = nullptr;
    if (realType->fromGeneric)
    {
        SWAG_CHECK(typeTable->makeConcreteSubTypeInfo(baseContext, &concreteType->fromGeneric, concreteType, storageSegment, storageOffset, realType->fromGeneric, cflags));
    }

    // Generic types
    concreteType->generics.buffer = 0;
    concreteType->generics.count  = realType->genericParameters.size();
    if (concreteType->generics.count)
    {
        uint32_t count = (uint32_t) concreteType->generics.count;
        uint32_t storageArray;
        auto     addrArray = (ConcreteTypeValue*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeValue), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->generics.buffer, storageArray);
        for (int param = 0; param < concreteType->generics.count; param++)
        {
            SWAG_CHECK(typeTable->makeConcreteTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->genericParameters[param], cflags));
            storageArray += sizeof(ConcreteTypeValue);
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
            auto     addrArray = (ConcreteTypeValue*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeValue), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->fields.buffer, storageArray);
            for (int param = 0; param < concreteType->fields.count; param++)
            {
                SWAG_CHECK(typeTable->makeConcreteTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->fields[param], cflags));
                storageArray += sizeof(ConcreteTypeValue);
            }
        }
    }

    // Methods
    concreteType->methods.buffer = 0;
    concreteType->methods.count  = 0;
    if (!(cflags & MAKE_CONCRETE_PARTIAL))
    {
        if (attributes & ATTRIBUTE_EXPORT_TYPE_METHODS)
        {
            concreteType->methods.count = realType->methods.size();
            if (concreteType->methods.count)
            {
                uint32_t count = (uint32_t) concreteType->methods.count;
                uint32_t storageArray;
                auto     addrArray = (ConcreteTypeValue*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeValue), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->methods.buffer, storageArray);
                for (int param = 0; param < concreteType->methods.count; param++)
                {
                    SWAG_CHECK(typeTable->makeConcreteTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->methods[param], cflags));

                    // 'value' will contain a pointer to the lambda.
                    // Register it for later patches
                    uint32_t     fieldOffset = storageArray + offsetof(ConcreteTypeValue, value);
                    AstFuncDecl* funcNode    = CastAst<AstFuncDecl>(realType->methods[param]->typeInfo->declNode, AstNodeKind::FuncDecl);
                    patchMethods.push_back({funcNode, fieldOffset});

                    storageArray += sizeof(ConcreteTypeValue);
                }
            }
        }
    }

    // Interfaces
    concreteType->interfaces.buffer = 0;
    concreteType->interfaces.count  = 0;
    if (!(cflags & MAKE_CONCRETE_PARTIAL))
    {
        concreteType->interfaces.count = realType->interfaces.size();
        if (concreteType->interfaces.count)
        {
            uint32_t count = (uint32_t) concreteType->interfaces.count;
            uint32_t storageArray;
            auto     addrArray = (ConcreteTypeValue*) typeTable->makeConcreteSlice(baseContext, count * sizeof(ConcreteTypeValue), concreteTypeInfoValue, storageSegment, storageOffset, &concreteType->interfaces.buffer, storageArray);
            for (int param = 0; param < concreteType->interfaces.count; param++)
            {
                SWAG_CHECK(typeTable->makeConcreteTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->interfaces[param], cflags));

                // :ItfIsConstantSeg
                // Compute the storage of the interface for @interfaceof
                uint32_t fieldOffset = offsetof(ConcreteTypeValue, value);
                uint32_t valueOffset = storageArray + fieldOffset;
                storageSegment->addInitPtr(valueOffset, realType->interfaces[param]->offset, SegmentKind::Constant);
                addrArray[param].value = module->constantSegment.address(realType->interfaces[param]->offset);

                storageArray += sizeof(ConcreteTypeValue);
            }
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

    if (!(cflags & MAKE_CONCRETE_PARTIAL))
    {
        waitAllStructInterfaces(realType);
        if (baseContext->result == ContextResult::Pending)
            return JobResult::KeepJobAlive;
        waitAllStructMethods(realType);
        if (baseContext->result == ContextResult::Pending)
            return JobResult::KeepJobAlive;
        waitStructGenerated(realType);
        if (baseContext->result == ContextResult::Pending)
            return JobResult::KeepJobAlive;
    }

    computeStruct();

    return JobResult::ReleaseJob;
}
