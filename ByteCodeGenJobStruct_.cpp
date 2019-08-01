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

bool ByteCodeGenJob::generateStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfo, TypeInfoFuncAttr* typeInfoFunc)
{
    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfo->structNode, AstNodeKind::StructDecl);

    structNode->lock();
    if (structNode->opInit)
    {
        structNode->unlock();
        return true;
    }

    structNode->opInit                    = g_Pool_byteCode.alloc();
    structNode->opInit->sourceFile        = context->sourceFile;
    structNode->opInit->name              = "opInit";
    structNode->opInit->typeInfoFunc      = typeInfoFunc;
    structNode->opInit->maxCallParameters = 1;
    structNode->opInit->usedRegisters.insert(0);
    structNode->opInit->usedRegisters.insert(1);
    context->node->ownerFct->bc->maxCallParameters = max(1, context->node->ownerFct->bc->maxCallParameters);
    sourceFile->module->addByteCodeFunc(structNode->opInit);

    ByteCodeGenContext cxt{*context};
    cxt.bc = structNode->opInit;

    for (auto child : structNode->childs)
    {
        auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
        auto typeVar = varDecl->typeInfo;

        emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = varDecl->resolvedSymbolOverload->storageOffset;

        if (varDecl->astAssignment)
        {
            if (typeVar->isNative(NativeType::String))
            {
                return internalError(context, "generateStructInit, invalid native type, string");
            }
            else if (typeVar->kind == TypeInfoKind::Native)
            {
                switch (typeVar->sizeOf)
                {
                case 1:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32 = varDecl->astAssignment->computedValue.reg.u8;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp8, 0, 1);
                    break;
                case 2:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32 = varDecl->astAssignment->computedValue.reg.u16;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp16, 0, 1);
                    break;
                case 4:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB32, 1)->b.u32 = varDecl->astAssignment->computedValue.reg.u32;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp32, 0, 1);
                    break;
                case 8:
                    emitInstruction(&cxt, ByteCodeOp::CopyRAVB64, 1)->b.u64 = varDecl->astAssignment->computedValue.reg.u64;
                    emitInstruction(&cxt, ByteCodeOp::AffectOp64, 0, 1);
                    break;
                default:
                    return internalError(context, "generateStructInit, invalid native type sizeof");
                }
            }
            else
            {
                return internalError(context, "generateStructInit, invalid assignment type");
            }
        }
        else if (typeVar->kind == TypeInfoKind::Struct && (typeVar->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR))
        {
            auto typeVarStruct = static_cast<TypeInfoStruct*>(typeVar);
            if (!generateStructInit(context, static_cast<TypeInfoStruct*>(typeVar), typeInfoFunc))
                return false;

            emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
            auto inst       = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
            inst->a.pointer = (uint8_t*) static_cast<AstStruct*>(typeVarStruct->structNode)->opInit;
            inst->b.u64     = 1;
            inst->c.pointer = (uint8_t*) typeVarStruct;
            emitInstruction(&cxt, ByteCodeOp::IncSP, 8);
        }
        else
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

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);
    structNode->unlock();

    return true;
}

bool ByteCodeGenJob::emitStructInit(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto resolved   = node->resolvedSymbolOverload;
    auto typeInfo   = CastTypeInfo<TypeInfoStruct>(resolved->typeInfo, TypeInfoKind::Struct);
    auto structNode = CastAst<AstStruct>(typeInfo->structNode, AstNodeKind::StructDecl);

    // Just clear the content of the structure
    if (!(typeInfo->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR))
    {
        auto inst   = emitInstruction(context, ByteCodeOp::ClearRefFromStackX);
        inst->a.u32 = resolved->storageOffset;
        inst->b.u32 = typeInfo->sizeOf;
        return true;
    }

    // Type
    auto typeInfoFunc = g_Pool_typeInfoFuncAttr.alloc();
    auto param        = g_Pool_typeInfoFuncAttrParam.alloc();
    param->typeInfo   = typeInfo;
    typeInfoFunc->parameters.push_back(param);
    typeInfoFunc = (TypeInfoFuncAttr*) g_TypeMgr.registerType(typeInfoFunc);

    // Be sure referenced function has bytecode
    if (!generateStructInit(context, typeInfo, typeInfoFunc))
        return false;

    // Push self
    RegisterList r0;
    r0          = reserveRegisterRC(context);
    auto inst   = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
    inst->b.s32 = resolved->storageOffset;

    // Then call
    emitInstruction(context, ByteCodeOp::PushRAParam, r0, 0);
    inst            = emitInstruction(context, ByteCodeOp::LocalCall, 0);
    inst->a.pointer = (uint8_t*) structNode->opInit;
    inst->b.u64     = 1;
    inst->c.pointer = (uint8_t*) typeInfoFunc;
    emitInstruction(context, ByteCodeOp::IncSP, 8);

    return true;
}
