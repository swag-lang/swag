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

bool ByteCodeGenJob::generateStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfo, TypeInfoFuncAttr* typeInfoFunc)
{
    auto sourceFile = context->sourceFile;
    auto structNode = CastAst<AstStruct>(typeInfo->structNode, AstNodeKind::StructDecl);

    structNode->lock();
    if (structNode->opInit->bc)
    {
        structNode->unlock();
        return true;
    }

    auto opInit        = g_Pool_byteCode.alloc();
    opInit->sourceFile = context->sourceFile;
    opInit->name       = structNode->ownerScope->fullname + "_" + structNode->name + "_opInit";
    replaceAll(opInit->name, '.', '_');
    opInit->typeInfoFunc      = typeInfoFunc;
    opInit->maxCallParameters = 1;
    opInit->usedRegisters.insert(0);
    opInit->usedRegisters.insert(1);
    opInit->usedRegisters.insert(2);

    sourceFile->module->addByteCodeFunc(opInit);
    structNode->opInit->bc = opInit;

    ByteCodeGenContext cxt{*context};
    cxt.bc = opInit;

    // No special value, so we can just clear the struct
    if (!(typeInfo->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR))
    {
		emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
        switch (typeInfo->sizeOf)
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
            emitInstruction(&cxt, ByteCodeOp::ClearX, 0)->b.u32 = typeInfo->sizeOf;
            break;
        }
    }
    else
    {

        for (auto child : structNode->childs)
        {
            auto varDecl = CastAst<AstVarDecl>(child, AstNodeKind::VarDecl);
            auto typeVar = varDecl->typeInfo;

            emitInstruction(&cxt, ByteCodeOp::RAFromStackParam64, 0, 24);
            if (!g_CommandLine.optimizeByteCode || varDecl->resolvedSymbolOverload->storageOffset)
            {
                emitInstruction(&cxt, ByteCodeOp::IncPointerVB, 0)->b.u32 = varDecl->resolvedSymbolOverload->storageOffset;
            }

            if (varDecl->assignment)
            {
                if (typeVar->isNative(NativeType::String))
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
                        return internalError(context, "generateStructInit, invalid native type sizeof");
                    }
                }
                else
                {
                    return internalError(context, "generateStructInit, invalid assignment type");
                }
            }
            else
            {
                bool done = false;

                // Whatever, generated opInit function, as the user can call it
                if (typeVar->kind == TypeInfoKind::Struct)
                {
                    auto typeVarStruct = static_cast<TypeInfoStruct*>(typeVar);
                    if (!generateStructInit(context, static_cast<TypeInfoStruct*>(typeVar), typeInfoFunc))
                        return false;

                    // Function call if necessary
                    if (typeVar->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR)
                    {
                        emitInstruction(&cxt, ByteCodeOp::PushRAParam, 0);
                        auto inst       = emitInstruction(&cxt, ByteCodeOp::LocalCall, 0);
                        inst->a.pointer = (uint8_t*) static_cast<AstStruct*>(typeVarStruct->structNode)->opInit->bc;
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
    }

    emitInstruction(&cxt, ByteCodeOp::Ret);
    emitInstruction(&cxt, ByteCodeOp::End);
    structNode->unlock();

    //structNode->opInit->bc->print();
    return true;
}

bool ByteCodeGenJob::emitDefaultStruct(ByteCodeGenContext* context)
{
    auto node       = CastAst<AstStruct>(context->node, AstNodeKind::StructDecl);
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
    auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(node->opInit->typeInfo, TypeInfoKind::FuncAttr);

    SWAG_CHECK(generateStructInit(context, typeStruct, typeFunc));
    return true;
}

bool ByteCodeGenJob::emitStructInit(ByteCodeGenContext* context, TypeInfoStruct* typeInfo, uint32_t regOffset)
{
    auto node       = context->node;
    auto resolved   = node->resolvedSymbolOverload;
    auto structNode = CastAst<AstStruct>(typeInfo->structNode, AstNodeKind::StructDecl);

    // Type
    auto typeInfoFunc = g_Pool_typeInfoFuncAttr.alloc();
    auto param        = g_Pool_typeInfoFuncAttrParam.alloc();
    param->typeInfo   = typeInfo;
    typeInfoFunc->parameters.push_back(param);
    typeInfoFunc = (TypeInfoFuncAttr*) g_TypeMgr.registerType(typeInfoFunc);

    // Be sure referenced function has bytecode
    if (!generateStructInit(context, typeInfo, typeInfoFunc))
        return false;

    // Just clear the content of the structure
    if (!(typeInfo->flags & TYPEINFO_STRUCT_HAS_CONSTRUCTOR))
    {
        auto inst   = emitInstruction(context, ByteCodeOp::ClearRefFromStackX);
        inst->a.u32 = resolved->storageOffset;
        inst->b.u32 = typeInfo->sizeOf;
    }
    else
    {
        assert(node->ownerFct);
        node->ownerFct->bc->maxCallParameters = max(1, node->ownerFct->bc->maxCallParameters);

        // Push self
        RegisterList r0;
        r0          = reserveRegisterRC(context);
        auto inst   = emitInstruction(context, ByteCodeOp::RARefFromStack, r0);
        inst->b.s32 = resolved->storageOffset;

        // Offset variable reference
        if (regOffset != UINT32_MAX)
            emitInstruction(context, ByteCodeOp::IncPointer, r0, regOffset, r0);

        // Then call
        emitInstruction(context, ByteCodeOp::PushRAParam, r0, 0);
        inst            = emitInstruction(context, ByteCodeOp::LocalCall, 0);
        inst->a.pointer = (uint8_t*) structNode->opInit->bc;
        inst->b.u64     = 1;
        inst->c.pointer = (uint8_t*) typeInfoFunc;
        emitInstruction(context, ByteCodeOp::IncSP, 8);
    }

    return true;
}
