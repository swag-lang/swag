#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "Ast.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeManager.h"
#include "Scope.h"
#include "SemanticJob.h"

bool ByteCodeGenJob::generateStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct)
{
    auto sourceFile   = context->sourceFile;
    auto opInitFct    = CastAst<AstFuncDecl>(typeInfoStruct->opInitFct, AstNodeKind::FuncDecl);
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(opInitFct->typeInfo, TypeInfoKind::FuncAttr);
    auto structNode   = CastAst<AstStruct>(typeInfoStruct->structNode, AstNodeKind::StructDecl);

    scoped_lock lk(typeInfoStruct->opInitFct->mutex);
    if (typeInfoStruct->opInitFct->bc && typeInfoStruct->opInitFct->bc->out)
        return true;

    ByteCode* opInit;
    if (typeInfoStruct->opInitFct->bc)
        opInit = typeInfoStruct->opInitFct->bc;
    else
        opInit = g_Pool_byteCode.alloc();
    opInit->sourceFile = context->sourceFile;
    opInit->name       = structNode->ownerScope->fullname + "_" + structNode->name + "_opInit";
    replaceAll(opInit->name, '.', '_');
    opInit->typeInfoFunc      = typeInfoFunc;
    opInit->maxCallParameters = 1;
	opInit->maxReservedRegisterRC = 3;

    if (!typeInfoStruct->opInitFct->bc)
        sourceFile->module->addByteCodeFunc(opInit);
    typeInfoStruct->opInitFct->bc = opInit;

    ByteCodeGenContext cxt{*context};
    cxt.bc = opInit;

    // All fields are explicitly not initialized, so we are done, function is empty
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED)
    {
        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        return true;
    }

    // No special value, so we can just clear the struct
    if (!(typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
    {
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        switch (typeInfoStruct->sizeOf)
        {
        case 1:
            emitInstruction(&cxt, ByteCodeOp::Clear8, 0);
            break;
        case 2:
            emitInstruction(&cxt, ByteCodeOp::Clear16, 0);
            break;
        case 4:
            emitInstruction(&cxt, ByteCodeOp::Clear32, 0);
            break;
        case 8:
            emitInstruction(&cxt, ByteCodeOp::Clear64, 0);
            break;
        default:
            emitInstruction(&cxt, ByteCodeOp::ClearX, 0)->b.u32 = typeInfoStruct->sizeOf;
            break;
        }

        emitInstruction(&cxt, ByteCodeOp::Ret);
        emitInstruction(&cxt, ByteCodeOp::End);
        return true;
    }

    for (auto child : structNode->content->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        auto typeVar = TypeManager::concreteType(varDecl->typeInfo);

        // Reference to the field
        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        if (varDecl->resolvedSymbolOverload->storageOffset)
            emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = varDecl->resolvedSymbolOverload->storageOffset;

        if (varDecl->assignment)
        {
            if (typeVar->kind == TypeInfoKind::Array)
            {
                auto     typeList      = CastTypeInfo<TypeInfoList>(varDecl->assignment->typeInfo, TypeInfoKind::TypeList);
                auto     module        = sourceFile->module;
                uint32_t storageOffset = module->reserveConstantSegment(typeVar->sizeOf);
                module->mutexConstantSeg.lock();
                auto offset = storageOffset;
                SemanticJob::collectLiterals(context->sourceFile, offset, varDecl->assignment, nullptr, SegmentBuffer::Constant);
                module->mutexConstantSeg.unlock();

                auto inst   = emitInstruction(&cxt, ByteCodeOp::RARefFromConstantSeg, 1, 2);
                inst->c.u64 = ((uint64_t) storageOffset << 32) | (uint32_t) typeList->childs.size();

                emitInstruction(&cxt, ByteCodeOp::Copy, 0, 1)->c.u32 = typeVar->sizeOf;
            }
            else if (typeVar->isNative(NativeType::String))
            {
                auto module      = sourceFile->module;
                auto stringIndex = module->reserveString(varDecl->assignment->computedValue.text);

                emitInstruction(&cxt, ByteCodeOp::CopyRARBStr, 1, 2)->c.u32 = stringIndex;
                emitInstruction(&cxt, ByteCodeOp::AffectOp64, 0, 1, 0);
                emitInstruction(&cxt, ByteCodeOp::AffectOp64, 0, 2, 8);
            }
            else if (typeVar->kind == TypeInfoKind::Native)
            {
                switch (typeVar->sizeOf)
                {
                case 1:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u8;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp8, 0, 1);
                    break;
                case 2:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u16;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp16, 0, 1);
                    break;
                case 4:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32 = varDecl->assignment->computedValue.reg.u32;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp32, 0, 1);
                    break;
                case 8:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB64, 1)->b.u64 = varDecl->assignment->computedValue.reg.u64;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp64, 0, 1);
                    break;
                default:
                    return internalError(context, "generateStructInit, invalid native type sizeof", varDecl);
                }
            }
            else
            {
                return internalError(context, "generateStructInit, invalid assignment type", varDecl);
            }
        }
        else
        {
            bool done = false;

            // Whatever, generated opInit function, as the user can call it
            if (typeVar->kind == TypeInfoKind::Struct)
            {
                auto typeVarStruct = static_cast<TypeInfoStruct*>(typeVar);
                SWAG_ASSERT(typeVarStruct->opInitFct);
                if (!generateStructInit(context, typeVarStruct))
                    return false;

                // Function call if necessary
                if (typeVar->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES)
                {
                    emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
                    auto inst = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
                    SWAG_ASSERT(typeInfoStruct->opInitFct->bc->out);
                    inst->a.pointer = (uint8_t*) typeVarStruct->opInitFct->bc;
                    inst->b.u64     = 1;
                    inst->c.pointer = (uint8_t*) typeInfoFunc;
                    emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
                    done = true;
                }
            }

            if (!done)
            {
                switch (typeVar->sizeOf)
                {
                case 1:
                    emitInstruction(&cxt, ByteCodeOp::Clear8, 0);
                    break;
                case 2:
                    emitInstruction(&cxt, ByteCodeOp::Clear16, 0);
                    break;
                case 4:
                    emitInstruction(&cxt, ByteCodeOp::Clear32, 0);
                    break;
                case 8:
                    emitInstruction(&cxt, ByteCodeOp::Clear64, 0);
                    break;
                default:
                    emitInstruction(&cxt, ByteCodeOp::ClearX, 0)->b.u32 = typeVar->sizeOf;
                    break;
                }
            }
        }
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);

    //opInit->print();
    return true;
}

void ByteCodeGenJob::emitStructParameters(ByteCodeGenContext* context, uint32_t regOffset)
{
    auto node     = static_cast<AstVarDecl*>(context->node);
    auto resolved = node->resolvedSymbolOverload;

    if (node->type && (node->type->flags & AST_HAS_STRUCT_PARAMETERS))
    {
        RegisterList r0   = reserveRegisterRC(context);
        RegisterList r1   = reserveRegisterRC(context);
        auto         inst = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
        inst->b.s32       = resolved->storageOffset;

        // Offset variable reference
        if (regOffset != UINT32_MAX)
            emitInstruction(context, ByteCodeOp::IncPointer, r0, regOffset, r0);

        auto typeExpression = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        auto identifier     = CastAst<AstIdentifier>(typeExpression->identifier->childs.back(), AstNodeKind::Identifier);
        for (auto child : identifier->callParameters->childs)
        {
            auto param     = CastAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
            auto typeParam = CastTypeInfo<TypeInfoParam>(param->resolvedParameter, TypeInfoKind::Param);
            emitInstruction(context, ByteCodeOp::CopyRARB, r1, r0);
            emitInstruction(context, ByteCodeOp::IncRAVB, r1)->b.u32 = typeParam->offset;
            emitAffectEqual(context, r1, child->resultRegisterRC, child->typeInfo, child->typeInfo);
            freeRegisterRC(context, child->resultRegisterRC);
        }

        freeRegisterRC(context, r0);
        freeRegisterRC(context, r1);
    }
}

bool ByteCodeGenJob::emitStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfoStruct, uint32_t regOffset)
{
    auto node     = context->node;
    auto resolved = node->resolvedSymbolOverload;

    // Be sure referenced function has bytecode
    if (!generateStructInit(context, typeInfoStruct))
        return false;

    // All fields are explicitly not initialized, so we are done
    if (typeInfoStruct->flags & TYPEINFO_STRUCT_ALL_UNINITIALIZED)
        return true;

    // Just clear the content of the structure
    if (!(typeInfoStruct->flags & TYPEINFO_STRUCT_HAS_INIT_VALUES))
    {
        auto inst   = emitInstruction(context, ByteCodeOp::ClearRefFromStackX);
        inst->a.u32 = resolved->storageOffset;
        inst->b.u32 = typeInfoStruct->sizeOf;
    }
    else
    {
        SWAG_ASSERT(node->ownerFct);
        node->ownerFct->bc->maxCallParameters = max(1, node->ownerFct->bc->maxCallParameters);

        // Push self
        RegisterList r0   = reserveRegisterRC(context);
        auto         inst = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
        inst->b.s32       = resolved->storageOffset;

        // Offset variable reference
        if (regOffset != UINT32_MAX)
            emitInstruction(context, ByteCodeOp::IncPointer, r0, regOffset, r0);

        // Then call
        emitInstruction(context, ByteCodeOp::PushRAParam, r0, 0);
        inst            = emitInstruction(context, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) typeInfoStruct->opInitFct->bc;
        inst->b.u64     = 1;
        inst->c.pointer = (uint8_t*) typeInfoStruct->opInitFct->typeInfo;
        emitInstruction(context, ByteCodeOp::IncSP, 8);

        freeRegisterRC(context, r0);
    }

    return true;
}
