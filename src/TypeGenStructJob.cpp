#include "pch.h"
#include "TypeGenStructJob.h"
#include "Ast.h"
#include "ByteCode.h"
#include "Generic.h"
#include "Module.h"
#include "ModuleManager.h"
#include "Semantic.h"

bool TypeGenStructJob::computeStruct()
{
	const auto concreteType = reinterpret_cast<ExportedTypeInfoStruct*>(exportedTypeInfoValue);
	const auto realType     = castTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);
	const auto attributes   = realType->declNode ? realType->declNode->attributeFlags : 0;

	concreteType->base.sizeOf = realType->sizeOf;

	// Flags
	if (!(genFlags & GEN_EXPORTED_TYPE_PARTIAL))
	{
		if (realType->hasFlag(TYPEINFO_STRUCT_NO_COPY))
			exportedTypeInfoValue->flags &= ~static_cast<uint16_t>(ExportedTypeInfoFlags::CanCopy);
		if (realType->opPostCopy || realType->opUserPostCopyFct)
			exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::HasPostCopy);
		if (realType->opPostMove || realType->opUserPostMoveFct)
			exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::HasPostMove);
		if (realType->opDrop || realType->opUserDropFct)
			exportedTypeInfoValue->flags |= static_cast<uint32_t>(ExportedTypeInfoFlags::HasDrop);
	}

	// Special functions lambdas
	concreteType->opInit     = nullptr;
	concreteType->opDrop     = nullptr;
	concreteType->opPostCopy = nullptr;
	concreteType->opPostMove = nullptr;

	if (!(genFlags & GEN_EXPORTED_TYPE_PARTIAL))
	{
		Utf8 callName;
		if (realType->opUserInitFct && realType->opUserInitFct->isForeign())
		{
			g_ModuleMgr->addPatchFuncAddress(storageSegment, &concreteType->opInit, realType->opUserInitFct);
			storageSegment->addInitPtrFunc(OFFSET_OF(concreteType->opInit), realType->opUserInitFct->getFullNameForeignImport());
		}
		else if (realType->opInit)
		{
			concreteType->opInit        = ByteCodeRun::makeLambda(baseContext, realType->opUserInitFct, realType->opInit);
			realType->opInit->isUsed    = true;
			realType->opInit->isInSeg   = true;
			realType->opInit->forceEmit = true;
			storageSegment->addInitPtrFunc(OFFSET_OF(concreteType->opInit), realType->opInit->getCallNameFromDecl());
		}

		if (realType->opUserDropFct && realType->opUserDropFct->isForeign())
		{
			g_ModuleMgr->addPatchFuncAddress(storageSegment, &concreteType->opDrop, realType->opUserDropFct);
			storageSegment->addInitPtrFunc(OFFSET_OF(concreteType->opDrop), realType->opUserDropFct->getFullNameForeignImport());
		}
		else if (realType->opDrop)
		{
			concreteType->opDrop        = ByteCodeRun::makeLambda(baseContext, realType->opUserDropFct, realType->opDrop);
			realType->opDrop->isUsed    = true;
			realType->opDrop->isInSeg   = true;
			realType->opDrop->forceEmit = true;
			storageSegment->addInitPtrFunc(OFFSET_OF(concreteType->opDrop), realType->opDrop->getCallNameFromDecl());
		}

		if (realType->opUserPostCopyFct && realType->opUserPostCopyFct->isForeign())
		{
			g_ModuleMgr->addPatchFuncAddress(storageSegment, &concreteType->opPostCopy, realType->opUserPostCopyFct);
			storageSegment->addInitPtrFunc(OFFSET_OF(concreteType->opPostCopy), realType->opUserPostCopyFct->getFullNameForeignImport());
		}
		else if (realType->opPostCopy)
		{
			concreteType->opPostCopy        = ByteCodeRun::makeLambda(baseContext, realType->opUserPostCopyFct, realType->opPostCopy);
			realType->opPostCopy->isUsed    = true;
			realType->opPostCopy->isInSeg   = true;
			realType->opPostCopy->forceEmit = true;
			storageSegment->addInitPtrFunc(OFFSET_OF(concreteType->opPostCopy), realType->opPostCopy->getCallNameFromDecl());
		}

		if (realType->opUserPostMoveFct && realType->opUserPostMoveFct->isForeign())
		{
			g_ModuleMgr->addPatchFuncAddress(storageSegment, &concreteType->opPostMove, realType->opUserPostMoveFct);
			storageSegment->addInitPtrFunc(OFFSET_OF(concreteType->opPostMove), realType->opUserPostMoveFct->getFullNameForeignImport());
		}
		else if (realType->opPostMove)
		{
			concreteType->opPostMove        = ByteCodeRun::makeLambda(baseContext, realType->opUserPostMoveFct, realType->opPostMove);
			realType->opPostMove->isUsed    = true;
			realType->opPostMove->isInSeg   = true;
			realType->opPostMove->forceEmit = true;
			storageSegment->addInitPtrFunc(OFFSET_OF(concreteType->opPostMove), realType->opPostMove->getCallNameFromDecl());
		}
	}

	auto&      mapPerSeg = typeGen->getMapPerSeg(storageSegment);
	ScopedLock lk(mapPerSeg.mutex);

	// Simple structure name, without generics
	SWAG_CHECK(typeGen->genExportedString(baseContext, &concreteType->structName, realType->structName, storageSegment, OFFSET_OF(concreteType->structName)));

	// Struct attributes
	SWAG_CHECK(typeGen->genExportedAttributes(baseContext, realType->attributes, exportedTypeInfoValue, storageSegment, storageOffset, &concreteType->attributes, genFlags));

	if (!realType->replaceTypes.empty())
	{
		// Update fields with types if generic
		for (const auto field : realType->fields)
		{
			field->typeInfo = Generic::replaceGenericTypes(realType->replaceTypes, field->typeInfo);
		}

		// Update methods with types if generic
		if (attributes & ATTRIBUTE_EXPORT_TYPE_METHODS)
		{
			for (const auto method : realType->methods)
			{
				method->typeInfo = Generic::replaceGenericTypes(realType->replaceTypes, method->typeInfo);
			}
		}
	}

	// Parent generic type
	concreteType->fromGeneric = nullptr;
	if (realType->fromGeneric)
	{
		SWAG_CHECK(typeGen->genExportedSubTypeInfo(baseContext, &concreteType->fromGeneric, concreteType, storageSegment, storageOffset, realType->fromGeneric, genFlags));
	}

	// Generic types
	concreteType->generics.buffer = nullptr;
	concreteType->generics.count  = realType->genericParameters.size();
	if (concreteType->generics.count)
	{
		const uint32_t count = static_cast<uint32_t>(concreteType->generics.count);
		uint32_t       storageArray;
		const auto     addrArray = static_cast<ExportedTypeValue*>(TypeGen::genExportedSlice(baseContext,
		                                                                                 count * sizeof(ExportedTypeValue),
		                                                                                 exportedTypeInfoValue,
		                                                                                 storageSegment,
		                                                                                 storageOffset,
		                                                                                 &concreteType->generics.buffer,
		                                                                                 storageArray));
		for (size_t param = 0; param < concreteType->generics.count; param++)
		{
			SWAG_CHECK(typeGen->genExportedTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->genericParameters[param], genFlags));
			storageArray += sizeof(ExportedTypeValue);
		}
	}

	// Fields
	concreteType->fields.buffer = nullptr;
	concreteType->fields.count  = 0;
	if ((attributes & ATTRIBUTE_EXPORT_TYPE_METHODS) || !realType->hasFlag(TYPEINFO_STRUCT_IS_ITABLE))
	{
		concreteType->fields.count = realType->fields.size();
		if (concreteType->fields.count)
		{
			const uint32_t count = static_cast<uint32_t>(concreteType->fields.count);
			uint32_t       storageArray;
			const auto     addrArray = static_cast<ExportedTypeValue*>(TypeGen::genExportedSlice(baseContext,
			                                                                                 count * sizeof(ExportedTypeValue),
			                                                                                 exportedTypeInfoValue,
			                                                                                 storageSegment,
			                                                                                 storageOffset,
			                                                                                 &concreteType->fields.buffer,
			                                                                                 storageArray));
			for (size_t param = 0; param < concreteType->fields.count; param++)
			{
				SWAG_CHECK(typeGen->genExportedTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->fields[param], genFlags));
				storageArray += sizeof(ExportedTypeValue);
			}
		}
	}

	// Methods
	concreteType->methods.buffer = nullptr;
	concreteType->methods.count  = 0;
	if (!(genFlags & GEN_EXPORTED_TYPE_PARTIAL))
	{
		if (attributes & ATTRIBUTE_EXPORT_TYPE_METHODS)
		{
			concreteType->methods.count = realType->methods.size();
			if (concreteType->methods.count)
			{
				const uint32_t count = static_cast<uint32_t>(concreteType->methods.count);
				uint32_t       storageArray;
				const auto     addrArray = static_cast<ExportedTypeValue*>(TypeGen::genExportedSlice(baseContext,
				                                                                                 count * sizeof(ExportedTypeValue),
				                                                                                 exportedTypeInfoValue,
				                                                                                 storageSegment,
				                                                                                 storageOffset,
				                                                                                 &concreteType->methods.buffer,
				                                                                                 storageArray));
				for (size_t param = 0; param < concreteType->methods.count; param++)
				{
					SWAG_CHECK(typeGen->genExportedTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->methods[param], genFlags));

					// 'value' will contain a pointer to the lambda.
					// Register it for later patches
					uint32_t     fieldOffset = storageArray + offsetof(ExportedTypeValue, value);
					AstFuncDecl* funcNode    = castAst<AstFuncDecl>(realType->methods[param]->typeInfo->declNode, AstNodeKind::FuncDecl);
					patchMethods.push_back({funcNode, fieldOffset});

					storageArray += sizeof(ExportedTypeValue);
				}
			}
		}
	}

	// Interfaces
	concreteType->interfaces.buffer = nullptr;
	concreteType->interfaces.count  = 0;
	if (!(genFlags & GEN_EXPORTED_TYPE_PARTIAL))
	{
		concreteType->interfaces.count = realType->interfaces.size();
		if (concreteType->interfaces.count)
		{
			const uint32_t count = static_cast<uint32_t>(concreteType->interfaces.count);
			uint32_t       storageArray;
			const auto     addrArray = static_cast<ExportedTypeValue*>(TypeGen::genExportedSlice(baseContext,
			                                                                                 count * sizeof(ExportedTypeValue),
			                                                                                 exportedTypeInfoValue,
			                                                                                 storageSegment,
			                                                                                 storageOffset,
			                                                                                 &concreteType->interfaces.buffer,
			                                                                                 storageArray));
			for (size_t param = 0; param < concreteType->interfaces.count; param++)
			{
				SWAG_CHECK(typeGen->genExportedTypeValue(baseContext, addrArray + param, storageSegment, storageArray, realType->interfaces[param], genFlags));

				// :ItfIsConstantSeg
				const uint32_t fieldOffset = offsetof(ExportedTypeValue, value);
				const uint32_t valueOffset = storageArray + fieldOffset;
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
	const auto realType = castTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind);

	Semantic::waitTypeCompleted(this, typeInfo);
	if (baseContext->result != ContextResult::Done)
		return JobResult::KeepJobAlive;

	if (!(genFlags & GEN_EXPORTED_TYPE_PARTIAL))
	{
		Semantic::waitAllStructInterfaces(this, realType);
		if (baseContext->result != ContextResult::Done)
			return JobResult::KeepJobAlive;
		Semantic::waitAllStructMethods(this, realType);
		if (baseContext->result != ContextResult::Done)
			return JobResult::KeepJobAlive;
		Semantic::waitStructGeneratedAlloc(this, realType);
		if (baseContext->result != ContextResult::Done)
			return JobResult::KeepJobAlive;
	}

	computeStruct();

	return JobResult::ReleaseJob;
}
