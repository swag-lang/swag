#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "Target.h"

const char* BackendC::swagTypeToCType(TypeInfo* typeInfo)
{
    SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
        return "swag_bool_t";
    case NativeTypeKind::S8:
        return "swag_int8_t";
    case NativeTypeKind::S16:
        return "swag_int16_t";
    case NativeTypeKind::S32:
        return "swag_int32_t";
    case NativeTypeKind::S64:
        return "swag_int64_t";
    case NativeTypeKind::U8:
        return "swag_uint8_t";
    case NativeTypeKind::U16:
        return "swag_uint16_t";
    case NativeTypeKind::U32:
        return "swag_uint32_t";
    case NativeTypeKind::U64:
        return "swag_uint64_t";
    case NativeTypeKind::F32:
        return "swag_float32_t";
    case NativeTypeKind::F64:
        return "swag_float64_t";
    case NativeTypeKind::Char:
        return "swag_char_t";
    case NativeTypeKind::String:
        return "const char*";
    case NativeTypeKind::Void:
        return "void";
    default:
        SWAG_ASSERT(false);
        return "";
    }
}

bool BackendC::emitForeignCall(ByteCodeInstruction* ip, vector<uint32_t>& pushParams)
{
    auto              nodeFunc   = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

	auto returnType = TypeManager::concreteType(typeFuncBC->returnType);
    if (returnType != g_TypeMgr.typeInfoVoid)
    {
        switch (returnType->nativeType)
        {
        case NativeTypeKind::S8:
            bufferC.addString("rt[0].s8 = ");
            break;
        case NativeTypeKind::U8:
            bufferC.addString("rt[0].u8 = ");
            break;
        case NativeTypeKind::S16:
            bufferC.addString("rt[0].s16 = ");
            break;
        case NativeTypeKind::U16:
            bufferC.addString("rt[0].u16 = ");
            break;
        case NativeTypeKind::S32:
            bufferC.addString("rt[0].s32 = ");
            break;
        case NativeTypeKind::U32:
            bufferC.addString("rt[0].u32 = ");
            break;
        case NativeTypeKind::S64:
            bufferC.addString("rt[0].s64 = ");
            break;
        case NativeTypeKind::U64:
            bufferC.addString("rt[0].u32 = ");
            break;
        case NativeTypeKind::Bool:
            bufferC.addString("rt[0].b = ");
            break;
        case NativeTypeKind::F32:
            bufferC.addString("rt[0].f32 = ");
            break;
        case NativeTypeKind::F64:
            bufferC.addString("rt[0].f64 = ");
            break;
        default:
            return module->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid return type");
        }
    }

    bufferC.addString(nodeFunc->fullname);
    bufferC.addString("(");

    int numCallParams = (int) typeFuncBC->parameters.size();
    for (int idxCall = 0; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = TypeManager::concreteType(typeFuncBC->parameters[idxCall]->typeInfo);
        if (idxCall)
            bufferC.addString(", ");

        auto index = pushParams.back();
        pushParams.pop_back();

        // Access to the content of the register
        if (typeParam->kind == TypeInfoKind::Pointer)
        {
            bufferC.addString(format("r[%u]", index));
            bufferC.addString(".pointer");
            index -= 1;
        }
        else if (typeParam->isNative(NativeTypeKind::String))
        {
            bufferC.addString(format("r[%u].pointer", index));
            index -= 2;
        }
        else if (typeParam->kind == TypeInfoKind::Native)
        {
            bufferC.addString(format("r[%u]", index));
            index -= 1;

            switch (typeParam->nativeType)
            {
            case NativeTypeKind::Bool:
                bufferC.addString(".b");
                break;
            case NativeTypeKind::S8:
                bufferC.addString(".s8");
                break;
            case NativeTypeKind::U8:
                bufferC.addString(".u8");
                break;
            case NativeTypeKind::S16:
                bufferC.addString(".s16");
                break;
            case NativeTypeKind::U16:
                bufferC.addString(".u16");
                break;
            case NativeTypeKind::S32:
                bufferC.addString(".s32");
                break;
            case NativeTypeKind::U32:
                bufferC.addString(".u32");
                break;
            case NativeTypeKind::S64:
                bufferC.addString(".s64");
                break;
            case NativeTypeKind::U64:
                bufferC.addString(".u64");
                break;
            default:
                return module->internalError(ip->sourceFileIdx, ip->startLocation, ip->endLocation, "emitForeignCall, invalid param type");
            }
        }
    }

    bufferC.addString(");");
    return true;
}

void BackendC::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    if (!(node->attributeFlags & ATTRIBUTE_PUBLIC))
        return;

    bufferSwg.addString("func ");
    bufferSwg.addString(node->fullname.c_str());
    bufferSwg.addString("(");
    bufferSwg.addString(")");

    bufferSwg.addString(";\n");
}

void BackendC::emitFuncSignaturePublic(Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    buffer.addString(swagTypeToCType(typeFunc->returnType));
    buffer.addString(" ");
    buffer.addString(module->name);
    buffer.addString("_");
    buffer.addString(node->fullname.c_str());
    buffer.addString("(");

    if (node->parameters)
    {
        bool first = true;
        for (auto param : node->parameters->childs)
        {
            if (!first)
                buffer.addString(", ");
            first = false;

            buffer.addString(swagTypeToCType(param->typeInfo));
            buffer.addString(" ");
            buffer.addString(param->name.c_str());
        }
    }

    buffer.addString(")");
}

void BackendC::emitFuncSignatureInternalC(ByteCode* bc)
{
    auto typeFunc = bc->callType();
    auto name     = bc->callName();

    bufferC.addString("void ");
    bufferC.addString(name.c_str());
    bufferC.addString("(");

    // Result
    for (int i = 0; i < typeFunc->numReturnRegisters(); i++)
    {
        if (i)
            bufferC.addString(", ");
        bufferC.addString(format("swag_register_t* rr%d", i));
    }

    // Parameters
    int index = 0;
    for (auto param : typeFunc->parameters)
    {
        auto typeParam = TypeManager::concreteType(param->typeInfo);
        for (int i = 0; i < typeParam->numRegisters(); i++)
        {
            if (index || i || typeFunc->numReturnRegisters())
                bufferC.addString(", ");
            bufferC.addString(format("swag_register_t* rp%u", index++));
        }
    }

    bufferC.addString(")");
}

bool BackendC::emitFuncSignatures()
{
    emitSeparator(bufferSwg, "PROTOTYPES");
    emitSeparator(bufferH, "PROTOTYPES");
    emitSeparator(bufferC, "PROTOTYPES");
    if (!emitFuncSignatures(g_Workspace.runtimeModule))
        return false;
    if (!emitFuncSignatures(module))
        return false;
    return true;
}

bool BackendC::emitFuncSignatures(Module* moduleToGen)
{
    SWAG_ASSERT(moduleToGen);

    bufferSwg.addString(format("#[swag.foreign(\"%s\")]\n", module->name.c_str()));
    bufferSwg.addString("{\n");

    for (auto one : moduleToGen->byteCodeFunc)
    {
        TypeInfoFuncAttr* typeFunc = one->typeInfoFunc;
        AstFuncDecl*      node     = nullptr;

        if (one->node)
        {
            node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

            // Do we need to generate that function ?
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
                continue;
            if (node->attributeFlags & ATTRIBUTE_FOREIGN)
                continue;

            typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        }

        bufferC.addString("static ");
        emitFuncSignatureInternalC(one);
        bufferC.addString(";\n");

        if (node)
        {
            if (node->attributeFlags & ATTRIBUTE_PUBLIC)
            {
                bufferH.addString("SWAG_EXTERN SWAG_IMPEXP ");
                emitFuncSignaturePublic(bufferH, typeFunc, node);
                bufferH.addString(";\n");
            }

            emitFuncSignatureSwg(typeFunc, node);
        }
    }

    bufferSwg.addString("}\n");
    bufferC.addString("\n");
    return true;
}

bool BackendC::emitInternalFunction(Module* moduleToGen, ByteCode* bc)
{
    bool ok       = true;
    auto typeFunc = bc->callType();

    if (bc->node && (bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
        bufferC.addString("#ifdef SWAG_HAS_TEST\n");

    // Signature
    bufferC.addString("static ");
    emitFuncSignatureInternalC(bc);
    bufferC.addString(" {\n");

    // Generate one local variable per used register
    if (bc->maxReservedRegisterRC)
        bufferC.addString(format("swag_register_t r[%u];\n", bc->maxReservedRegisterRC));

    // For function call results
    if (bc->maxCallResults)
        bufferC.addString(format("swag_register_t rt[%u];\n", bc->maxCallResults));

    // Local stack
    if (typeFunc->stackSize)
    {
        bufferC.addString(format("swag_uint8_t stack[%u];\n", typeFunc->stackSize));
    }

    // Generate bytecode
    int              vaargsIdx = 0;
    auto             ip        = bc->out;
    int              lastLine  = -1;
    vector<uint32_t> pushRAParams;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        // Print source code
        if (module->buildParameters.target.backendC.writeSourceCode && !bc->compilerGenerated)
        {
            if (ip->startLocation.line != lastLine)
            {
                if (ip->startLocation.column != ip->endLocation.column)
                {
                    lastLine = ip->startLocation.line;
                    auto s   = bc->sourceFile->getLine(ip->startLocation.seekStartLine[REPORT_NUM_CODE_LINES - 1]);
                    s.erase(0, s.find_first_not_of("\t\n\v\f\r "));
                    if (!s.empty())
                        s.pop_back();
                    bufferC.addString("/* ");
                    bufferC.addString(s);
                    bufferC.addString(" */\n");
                }
            }
        }

        bufferC.addString(format("lbl%08u:; ", i));

        if (module->buildParameters.target.backendC.writeByteCodeInstruction)
        {
            bufferC.addString("/* ");
            for (int dec = g_ByteCodeOpNamesLen[(int) ip->op]; dec < ByteCode::ALIGN_RIGHT_OPCODE; dec++)
                bufferC.addString(" ");
            bufferC.addString(g_ByteCodeOpNames[(int) ip->op]);
            bufferC.addString(" */ ");
        }

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::MovSPBP:
            break;

        case ByteCodeOp::BoundCheckString:
            bufferC.addString(format("__assert(r[%u].u32 <= r[%u].u32 + 1, \"%s\", %d, \": error: index out of range\");", ip->a.u32, ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1));
            break;
        case ByteCodeOp::BoundCheck:
            bufferC.addString(format("__assert(r[%u].u32 < r[%u].u32, \"%s\", %d, \": error: index out of range\");", ip->a.u32, ip->b.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1));
            break;
        case ByteCodeOp::IncPointerVB:
            bufferC.addString(format("r[%u].pointer += %d;", ip->a.u32, ip->b.s32));
            break;
        case ByteCodeOp::IncPointer:
            bufferC.addString(format("r[%u].pointer = r[%u].pointer + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::DecPointer:
            bufferC.addString(format("r[%u].pointer = r[%u].pointer - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::DeRef8:
            bufferC.addString(format("r[%u].u64 = *(swag_uint8_t*) r[%u].pointer;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::DeRef16:
            bufferC.addString(format("r[%u].u64 = *(swag_uint16_t*) r[%u].pointer;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::DeRef32:
            bufferC.addString(format("r[%u].u64 = *(swag_uint32_t*) r[%u].pointer;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::DeRef64:
            bufferC.addString(format("r[%u].u64 = *(swag_uint64_t*) r[%u].pointer;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::DeRefPointer:
            bufferC.addString(format("r[%u].pointer = *(swag_uint8_t**) (r[%u].pointer + %u);", ip->b.u32, ip->a.u32, ip->c.u32));
            break;
        case ByteCodeOp::DeRefStringSlice:
            bufferC.addString(format("r[%u].u64 = *(swag_uint64_t*) (r[%u].pointer + 8); ", ip->b.u32, ip->a.u32));
            bufferC.addString(format("r[%u].pointer = *(swag_uint8_t**) r[%u].pointer; ", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::MulRAVB:
            bufferC.addString(format("r[%u].s32 *= %u;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::RAFromDataSeg8:
            bufferC.addString(format("r[%u].u8 = *(swag_uint8_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromDataSeg16:
            bufferC.addString(format("r[%u].u16 = *(swag_uint16_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromDataSeg32:
            bufferC.addString(format("r[%u].u32 = *(swag_uint32_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromDataSeg64:
            bufferC.addString(format("r[%u].u64 = *(swag_uint64_t*) (__mutableseg + %u);", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::Clear8:
            bufferC.addString(format("*(swag_uint8_t*)r[%u].pointer = 0;", ip->a.u32));
            break;
        case ByteCodeOp::Clear16:
            bufferC.addString(format("*(swag_uint16_t*)r[%u].pointer = 0;", ip->a.u32));
            break;
        case ByteCodeOp::Clear32:
            bufferC.addString(format("*(swag_uint32_t*)r[%u].pointer = 0;", ip->a.u32));
            break;
        case ByteCodeOp::Clear64:
            bufferC.addString(format("*(swag_uint64_t*)r[%u].pointer = 0;", ip->a.u32));
            break;
        case ByteCodeOp::ClearX:
            bufferC.addString(format("memset(r[%u].pointer, 0, %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ClearXVar:
            bufferC.addString(format("memset(r[%u].pointer, 0, r[%u].u32 * %u);", ip->a.u32, ip->b.u32, ip->c.u32));
            break;

        case ByteCodeOp::ClearRefFromStack8:
            bufferC.addString(format("*(swag_uint8_t*)(stack + %u) = 0;", ip->a.u32));
            break;
        case ByteCodeOp::ClearRefFromStack16:
            bufferC.addString(format("*(swag_uint16_t*)(stack + %u) = 0;", ip->a.u32));
            break;
        case ByteCodeOp::ClearRefFromStack32:
            bufferC.addString(format("*(swag_uint32_t*)(stack + %u) = 0;", ip->a.u32));
            break;
        case ByteCodeOp::ClearRefFromStack64:
            bufferC.addString(format("*(swag_uint64_t*)(stack + %u) = 0;", ip->a.u32));
            break;
        case ByteCodeOp::ClearRefFromStackX:
            bufferC.addString(format("memset(stack + %u, 0, %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RARefFromDataSeg:
            bufferC.addString(format("r[%u].pointer = __mutableseg + %u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAAddrFromConstantSeg:
            bufferC.addString(format("r[%u].pointer = (swag_uint8_t*) (__constantseg + %u); ", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RARefFromConstantSeg:
            bufferC.addString(format("r[%u].pointer = __constantseg + %u; ", ip->a.u32, (uint32_t)(ip->c.u64 >> 32)));
            bufferC.addString(format("r[%u].u64 = %u;", ip->b.u32, (ip->c.u64) & 0xFFFFFFFF));
            break;
        case ByteCodeOp::RARefFromStack:
            bufferC.addString(format("r[%u].pointer = stack + %u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStack8:
            bufferC.addString(format("r[%u].u8 = *(swag_uint8_t*) (stack + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStack16:
            bufferC.addString(format("r[%u].u16 = *(swag_uint16_t*) (stack + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStack32:
            bufferC.addString(format("r[%u].u32 = *(swag_uint32_t*) (stack + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStack64:
            bufferC.addString(format("r[%u].u64 = *(swag_uint64_t*) (stack + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::Copy:
            bufferC.addString(format("memcpy(r[%u].pointer, r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32));
            break;
        case ByteCodeOp::CopyVC:
            bufferC.addString(format("memcpy(r[%u].pointer, r[%u].pointer, %u);", ip->a.u32, ip->b.u32, ip->c.u32));
            break;
        case ByteCodeOp::CopyRAVB32:
            bufferC.addString(format("r[%u].u32 = 0x%x;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRAVB64:
            bufferC.addString(format("r[%u].u64 = 0x%I64x;", ip->a.u32, ip->b.u64));
            break;
        case ByteCodeOp::CopyRARBStr:
            bufferC.addString(format("r[%u].pointer = __string%u; ", ip->a.u32, ip->c.u32));
            bufferC.addString(format("r[%u].u32 = %u;", ip->b.u32, moduleToGen->strBuffer[ip->c.u32].size()));
            break;
        case ByteCodeOp::CopyRARB:
            bufferC.addString(format("r[%u] = r[%u]; ", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRARBAddr:
            bufferC.addString(format("r[%u].pointer = (swag_uint8_t*) &r[%u]; ", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ClearRA:
            bufferC.addString(format("r[%u].u64 = 0;", ip->a.u32));
            break;
        case ByteCodeOp::DecRA:
            bufferC.addString(format("r[%u].u32--;", ip->a.u32));
            break;
        case ByteCodeOp::IncRA:
            bufferC.addString(format("r[%u].u32++;", ip->a.u32));
            break;
        case ByteCodeOp::IncRA64:
            bufferC.addString(format("r[%u].u64++;", ip->a.u32));
            break;
        case ByteCodeOp::IncRAVB:
            bufferC.addString(format("r[%u].u32 += %u;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOp8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) = r[%u].u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) = r[%u].u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer + %u) = r[%u].u32;", ip->a.u32, ip->c.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer + %u) = r[%u].u64;", ip->a.u32, ip->c.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp64Null:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer + %u) = 0;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPointer:
            bufferC.addString(format("*(void**)(r[%u].pointer) = r[%u].pointer;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::BinOpPlusS32:
            bufferC.addString(format("r[%u].s32 = r[%u].s32 + r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusS64:
            bufferC.addString(format("r[%u].s64 = r[%u].s64 + r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusU32:
            bufferC.addString(format("r[%u].u32 = r[%u].u32 + r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusU64:
            bufferC.addString(format("r[%u].u64 = r[%u].u64 + r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusF32:
            bufferC.addString(format("r[%u].f32 = r[%u].f32 + r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusF64:
            bufferC.addString(format("r[%u].f64= r[%u].f64 + r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::BinOpMinusS32:
            bufferC.addString(format("r[%u].s32 = r[%u].s32 - r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusS64:
            bufferC.addString(format("r[%u].s64 = r[%u].s64 - r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusU32:
            bufferC.addString(format("r[%u].u32 = r[%u].u32 - r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusU64:
            bufferC.addString(format("r[%u].u64 = r[%u].u64 - r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusF32:
            bufferC.addString(format("r[%u].f32 = r[%u].f32 - r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusF64:
            bufferC.addString(format("r[%u].f64 = r[%u].f64 - r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::BinOpMulS32:
            bufferC.addString(format("r[%u].s32 = r[%u].s32 * r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulS64:
            bufferC.addString(format("r[%u].s64 = r[%u].s64 * r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulU32:
            bufferC.addString(format("r[%u].u32 = r[%u].u32 * r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulU64:
            bufferC.addString(format("r[%u].u64 = r[%u].u64 * r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulF32:
            bufferC.addString(format("r[%u].f32 = r[%u].f32 * r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulF64:
            bufferC.addString(format("r[%u].f64= r[%u].f64 * r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::XorS32:
            bufferC.addString(format("r[%u].s32 = r[%u].s32 ^ r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::XorS64:
            bufferC.addString(format("r[%u].s64 = r[%u].s64 ^ r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::XorU32:
            bufferC.addString(format("r[%u].u32 = r[%u].u32 ^ r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::XorU64:
            bufferC.addString(format("r[%u].u64 = r[%u].u64 ^ r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::ShiftLeftS32:
            bufferC.addString(format("r[%u].s32 = r[%u].s32 << r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftLeftS64:
            bufferC.addString(format("r[%u].s64 = r[%u].s64 << r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftLeftU32:
            bufferC.addString(format("r[%u].u32 = r[%u].u32 << r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftLeftU64:
            bufferC.addString(format("r[%u].u64 = r[%u].u64 << r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::ShiftRightS32:
            bufferC.addString(format("r[%u].s32 = r[%u].s32 >> r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftRightS64:
            bufferC.addString(format("r[%u].s64 = r[%u].s64 >> r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftRightU32:
            bufferC.addString(format("r[%u].u32 = r[%u].u32 >> r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftRightU64:
            bufferC.addString(format("r[%u].u64 = r[%u].u64 >> r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftRightU64VB:
            bufferC.addString(format("r[%u].u64 >>= %u;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::BinOpDivF32:
            bufferC.addString(format("r[%u].f32 = r[%u].f32 / r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpDivF64:
            bufferC.addString(format("r[%u].f64 = r[%u].f64 / r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r[%u].pointer) -= r[%u].s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r[%u].pointer) -= r[%u].s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r[%u].pointer) -= r[%u].s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r[%u].pointer) -= r[%u].s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) -= r[%u].u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) -= r[%u].u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer) -= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer) -= r[%u].u64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            bufferC.addString(format("*(swag_float32_t*)(r[%u].pointer) -= r[%u].f32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            bufferC.addString(format("*(swag_float64_t*)(r[%u].pointer) -= r[%u].f64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r[%u].pointer) += r[%u].s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r[%u].pointer) += r[%u].s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r[%u].pointer) += r[%u].s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r[%u].pointer) += r[%u].s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) += r[%u].u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) += r[%u].u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer) += r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer) += r[%u].u64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            bufferC.addString(format("*(swag_float32_t*)(r[%u].pointer) += r[%u].f32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            bufferC.addString(format("*(swag_float64_t*)(r[%u].pointer) += r[%u].f64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqPointer:
            bufferC.addString(format("*(swag_uint8_t**)(r[%u].pointer) += r[%u].s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqPointer:
            bufferC.addString(format("*(swag_uint8_t**)(r[%u].pointer) -= r[%u].s32;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r[%u].pointer) *= r[%u].s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r[%u].pointer) *= r[%u].s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r[%u].pointer) *= r[%u].s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r[%u].pointer) *= r[%u].s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) *= r[%u].u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) *= r[%u].u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer) *= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer) *= r[%u].u64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            bufferC.addString(format("*(swag_float32_t*)(r[%u].pointer) *= r[%u].f32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            bufferC.addString(format("*(swag_float64_t*)(r[%u].pointer) *= r[%u].f64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpDivEqF32:
            bufferC.addString(format("*(swag_float32_t*)(r[%u].pointer) /= r[%u].f32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            bufferC.addString(format("*(swag_float64_t*)(r[%u].pointer) /= r[%u].f64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r[%u].pointer) &= r[%u].s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r[%u].pointer) &= r[%u].s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r[%u].pointer) &= r[%u].s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r[%u].pointer) &= r[%u].s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) &= r[%u].u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) &= r[%u].u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer) &= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer) &= r[%u].u64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r[%u].pointer) |= r[%u].s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r[%u].pointer) |= r[%u].s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r[%u].pointer) |= r[%u].s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r[%u].pointer) |= r[%u].s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) |= r[%u].u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) |= r[%u].u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer) |= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer) |= r[%u].u64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpShiftLeftEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer) <<= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer) >>= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpPercentEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r[%u].pointer) %%= r[%u].s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r[%u].pointer) %%= r[%u].s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r[%u].pointer) %%= r[%u].s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r[%u].pointer) %%= r[%u].s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) %%= r[%u].u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) %%= r[%u].u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer) %%= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer) %%= r[%u].u64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpXOrEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r[%u].pointer) ^= r[%u].s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r[%u].pointer) ^= r[%u].s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r[%u].pointer) ^= r[%u].s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r[%u].pointer) ^= r[%u].s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r[%u].pointer) ^= r[%u].u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r[%u].pointer) ^= r[%u].u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r[%u].pointer) ^= r[%u].u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r[%u].pointer) ^= r[%u].u64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CompareOpGreaterS32:
            bufferC.addString(format("r[%u].b = r[%u].s32 > r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            bufferC.addString(format("r[%u].b = r[%u].s64 > r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            bufferC.addString(format("r[%u].b = r[%u].u32 > r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            bufferC.addString(format("r[%u].b = r[%u].u64 > r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            bufferC.addString(format("r[%u].b = r[%u].f32 > r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            bufferC.addString(format("r[%u].b = r[%u].f64 > r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterPointer:
            bufferC.addString(format("r[%u].b = r[%u].pointer > r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CompareOpLowerS32:
            bufferC.addString(format("r[%u].b = r[%u].s32 < r[%u].s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerS64:
            bufferC.addString(format("r[%u].b = r[%u].s64 < r[%u].s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerU32:
            bufferC.addString(format("r[%u].b = r[%u].u32 < r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerU64:
            bufferC.addString(format("r[%u].b = r[%u].u64 < r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerF32:
            bufferC.addString(format("r[%u].b = r[%u].f32 < r[%u].f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerF64:
            bufferC.addString(format("r[%u].b = r[%u].f64 < r[%u].f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerPointer:
            bufferC.addString(format("r[%u].b = r[%u].pointer < r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CompareOpEqual8:
            bufferC.addString(format("r[%u].b = r[%u].u8 == r[%u].u8;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqual16:
            bufferC.addString(format("r[%u].b = r[%u].u16 == r[%u].u16;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqual32:
            bufferC.addString(format("r[%u].b = r[%u].u32 == r[%u].u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqual64:
            bufferC.addString(format("r[%u].b = r[%u].u64 == r[%u].u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqualPointer:
            bufferC.addString(format("r[%u].b = r[%u].pointer == r[%u].pointer;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqualBool:
            bufferC.addString(format("r[%u].b = r[%u].b == r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqualString:
            bufferC.addString(format("r[%u].b = __strcmp((const char*) r[%u].pointer, (const char*) r[%u].pointer, r[%u].u32);", ip->c.u32, ip->a.u32, ip->b.u32, ip->c.u32));
            break;
        case ByteCodeOp::IsNullString:
            bufferC.addString(format("r[%u].b = r[%u].pointer == 0;", ip->b.u32, ip->a.u32));
            break;

        case ByteCodeOp::BinOpAnd:
            bufferC.addString(format("r[%u].b = r[%u].b && r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpOr:
            bufferC.addString(format("r[%u].b = r[%u].b || r[%u].b;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::Jump:
            bufferC.addString(format("goto lbl%08u;", ip->a.s32 + i + 1));
            break;
        case ByteCodeOp::JumpNotTrue:
            bufferC.addString(format("if(!r[%u].b) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1));
            break;
        case ByteCodeOp::JumpTrue:
            bufferC.addString(format("if(r[%u].b) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1));
            break;
        case ByteCodeOp::JumpZero32:
            bufferC.addString(format("if(!r[%u].u32) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1));
            break;
        case ByteCodeOp::JumpNotZero32:
            bufferC.addString(format("if(r[%u].u32) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1));
            break;
        case ByteCodeOp::Ret:
            bufferC.addString("return;");
            break;

        case ByteCodeOp::IntrinsicPrintS64:
            bufferC.addString(format("__print_i64(r[%u].s64);", ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            bufferC.addString(format("__print_f64(r[%u].f64);", ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicPrintString:
            bufferC.addString(format("__print_n((const char*) r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::IntrinsicAssert:
            if (ip->c.pointer)
                bufferC.addString(format("__assert(r[%u].b, \"%s\", %d, \"%s\");", ip->a.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1, ip->c.pointer));
            else
                bufferC.addString(format("__assert(r[%u].b, \"%s\", %d, 0);", ip->a.u32, normalizePath(moduleToGen->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1));
            break;
        case ByteCodeOp::IntrinsicAlloc:
            bufferC.addString(format("r[%u].pointer = (swag_uint8_t*) malloc(r[%u].u32);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::IntrinsicRealloc:
            bufferC.addString(format("r[%u].pointer = (swag_uint8_t*) realloc(r[%u].pointer, r[%u].u32);", ip->a.u32, ip->b.u32, ip->c.u32));
            break;
        case ByteCodeOp::IntrinsicFree:
            bufferC.addString(format("free(r[%u].pointer);", ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicGetContext:
            bufferC.addString(format("r[%u].pointer = (swag_uint8_t*) __tlsGetValue(__contextTlsId);", ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicArguments:
            bufferC.addString(format("r[%u].pointer = (swag_uint8_t*) __argumentsSlice[0]; ", ip->a.u32));
            bufferC.addString(format("r[%u].u64 = (swag_uint64_t) __argumentsSlice[1]; ", ip->b.u32));
            break;

        case ByteCodeOp::NegBool:
            bufferC.addString(format("r[%u].b = !r[%u].b;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::NegF32:
            bufferC.addString(format("r[%u].f32 = -r[%u].f32;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::NegF64:
            bufferC.addString(format("r[%u].f64 = -r[%u].f64;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::NegS32:
            bufferC.addString(format("r[%u].s32 = -r[%u].s32;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::NegS64:
            bufferC.addString(format("r[%u].s64 = -r[%u].s64;", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::InvertS32:
            bufferC.addString(format("r[%u].s32 = ~r[%u].s32;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::InvertS64:
            bufferC.addString(format("r[%u].s64 = ~r[%u].s64;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::InvertU32:
            bufferC.addString(format("r[%u].u32 = ~r[%u].u32;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::InvertU64:
            bufferC.addString(format("r[%u].u64 = ~r[%u].u64;", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::ClearMaskU32:
            bufferC.addString(format("r[%u].u32 &= 0x%x;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ClearMaskU64:
            bufferC.addString(format("r[%u].u64 &= 0xFFFFFFFF | ((swag_uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ClearMaskU32U64:
            bufferC.addString(format("r[%u].u32 &= 0x%x; ", ip->a.u32, ip->b.u32));
            bufferC.addString(format("r[%u].u64 &= 0xFFFFFFFF | ((swag_uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CastBool8:
            bufferC.addString(format("r[%u].b = r[%u].u8 ? 1 : 0; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastBool16:
            bufferC.addString(format("r[%u].b = r[%u].u16 ? 1 : 0; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastBool32:
            bufferC.addString(format("r[%u].b = r[%u].u32 ? 1 : 0; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastBool64:
            bufferC.addString(format("r[%u].b = r[%u].u64 ? 1 : 0; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS8S16:
            bufferC.addString(format("r[%u].s16 = (swag_int16_t) r[%u].s8; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS16S32:
            bufferC.addString(format("r[%u].s32 = (swag_int32_t) r[%u].s16; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS32S8:
            bufferC.addString(format("r[%u].s8 = (swag_int8_t) r[%u].s32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS32S16:
            bufferC.addString(format("r[%u].s16 = (swag_int16_t) r[%u].s32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS32S64:
            bufferC.addString(format("r[%u].f64 = (swag_float64_t) r[%u].s32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS32F32:
            bufferC.addString(format("r[%u].f32 = (swag_float32_t) r[%u].s32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS64S32:
            bufferC.addString(format("r[%u].s32 = (swag_int32_t) r[%u].s64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS64F32:
            bufferC.addString(format("r[%u].f32 = (swag_float32_t) r[%u].s64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS64F64:
            bufferC.addString(format("r[%u].f64 = (swag_float64_t) r[%u].s64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastU32F32:
            bufferC.addString(format("r[%u].f32 = (swag_float32_t) r[%u].u32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastU64F32:
            bufferC.addString(format("r[%u].f32 = (swag_float32_t) r[%u].u64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastU64F64:
            bufferC.addString(format("r[%u].f64 = (swag_float64_t) r[%u].u64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF32S32:
            bufferC.addString(format("r[%u].s32 = (swag_int32_t) r[%u].f32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF32S64:
            bufferC.addString(format("r[%u].s64 = (swag_int64_t) r[%u].f32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF32F64:
            bufferC.addString(format("r[%u].f64 = (swag_float64_t) r[%u].f32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF64S64:
            bufferC.addString(format("r[%u].s64 = (swag_int64_t) r[%u].f64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF64F32:
            bufferC.addString(format("r[%u].f32 = (swag_float32_t) r[%u].f64; ", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::CopyRRxRCx:
            bufferC.addString(format("*rr%u = r[%u];", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRCxRRx:
            bufferC.addString(format("r[%u] = *rr%u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRRxRCxCall:
            bufferC.addString(format("rt[%u] = r[%u];", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRCxRRxCall:
            bufferC.addString(format("r[%u] = rt[%u];", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStackParam64:
            bufferC.addString(format("r[%u] = *rp%u;", ip->a.u32, ip->c.u32));
            break;
        case ByteCodeOp::RARefFromStackParam:
            bufferC.addString(format("r[%u].pointer = (swag_int8_t*) &rp%u->pointer;", ip->a.u32, ip->c.u32));
            break;
        case ByteCodeOp::PushRRSaved:
        case ByteCodeOp::PopRRSaved:
            break;

        case ByteCodeOp::MinusToTrue:
            bufferC.addString(format("r[%u].b = r[%u].s32 < 0 ? 1 : 0;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::MinusZeroToTrue:
            bufferC.addString(format("r[%u].b = r[%u].s32 <= 0 ? 1 : 0;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::PlusToTrue:
            bufferC.addString(format("r[%u].b = r[%u].s32 > 0 ? 1 : 0;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::PlusZeroToTrue:
            bufferC.addString(format("r[%u].b = r[%u].s32 >= 0 ? 1 : 0;", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::MakeLambda:
        {
            auto funcBC = (ByteCode*) ip->b.pointer;
            bufferC.addString(format("r[%u].pointer = (swag_uint8_t*) &%s;", ip->a.u32, funcBC->callName().c_str()));
            break;
        }

        case ByteCodeOp::PushRAParam:
            pushRAParams.push_back(ip->a.u32);
            break;
        case ByteCodeOp::MovRASP:
            bufferC.addString(format("r[%u].pointer = (swag_uint8_t*) &r[%u];", ip->a.u32, ip->c.u32));
            break;
        case ByteCodeOp::MovRASPVaargs:
        {
            bufferC.addString(format("swag_register_t vaargs%u[] = { 0, ", vaargsIdx));
            int idxParam = (int) pushRAParams.size() - 1;
            while (idxParam >= 0)
            {
                bufferC.addString(format("r[%u], ", pushRAParams[idxParam]));
                idxParam--;
            }

            bufferC.addString("}; ");
            bufferC.addString(format("r[%u].pointer = sizeof(swag_register_t) + (swag_uint8_t*) &vaargs%u; ", ip->a.u32, vaargsIdx));
            bufferC.addString(format("vaargs%u[0].pointer = r[%u].pointer;", vaargsIdx, ip->a.u32));
            vaargsIdx++;
            break;
        }

        case ByteCodeOp::LambdaCall:
        case ByteCodeOp::LocalCall:
        {
            auto              funcBC     = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->b.pointer;

            // Normal function call
            if (ip->op == ByteCodeOp::LocalCall)
            {
                bufferC.addString(format("{ %s", funcBC->callName().c_str()));
            }

            // Lambda call
            else
            {
                // Need to output the function prototype too
                bufferC.addString("{ typedef void(*tfn)(");
                for (int j = 0; j < typeFuncBC->numReturnRegisters() + typeFuncBC->numParamsRegisters(); j++)
                {
                    if (j)
                        bufferC.addString(",");
                    bufferC.addString("swag_register_t*");
                }

                bufferC.addString("); ");

                // Then the call
                bufferC.addString(format("((tfn)r[%u].pointer)", ip->a.u32));
            }

            bufferC.addString("(");
            for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
            {
                if (j)
                    bufferC.addString(",");
                bufferC.addString(format("&rt[%u]", j));
            }

            int numCallParams = (int) typeFuncBC->parameters.size();
            for (int idxCall = numCallParams - 1; idxCall >= 0; idxCall--)
            {
                auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
                typeParam      = TypeManager::concreteType(typeParam);
                for (int j = 0; j < typeParam->numRegisters(); j++)
                {
                    if ((idxCall != (int) numCallParams - 1) || j || typeFuncBC->numReturnRegisters())
                        bufferC.addString(", ");
                    auto index = pushRAParams.back();
                    pushRAParams.pop_back();
                    bufferC.addString(format("&r[%u]", index));
                }
            }

            pushRAParams.clear();
            bufferC.addString("); }");
        }
        break;

        case ByteCodeOp::ForeignCall:
            SWAG_CHECK(emitForeignCall(ip, pushRAParams));
            break;

        default:
            ok = false;
            bufferC.addString("// ");
            bufferC.addString(g_ByteCodeOpNames[(int) ip->op]);
            moduleToGen->internalError(format("unknown instruction '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
            break;
        }

        bufferC.addString("\n");
    }

    bufferC.addString("}\n");

    if (bc->node && bc->node->attributeFlags & ATTRIBUTE_TEST_FUNC)
        bufferC.addString("#endif\n");

    bufferC.addString("\n");
    return ok;
}

bool BackendC::emitFunctions()
{
    emitSeparator(bufferC, "SWAG FUNCTIONS");
    if (!emitFunctions(g_Workspace.runtimeModule))
        return false;
    emitSeparator(bufferC, "INTERNAL FUNCTIONS");
    if (!emitFunctions(module))
        return false;
    return true;
}

bool BackendC::emitFunctions(Module* moduleToGen)
{
    SWAG_ASSERT(moduleToGen);

    bool ok = true;
    for (auto one : moduleToGen->byteCodeFunc)
    {
        TypeInfoFuncAttr* typeFunc = one->typeInfoFunc;
        AstFuncDecl*      node     = nullptr;

        if (one->node)
        {
            node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);
            node->computeFullName();

            // Do we need to generate that function ?
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if ((node->attributeFlags & ATTRIBUTE_TEST_FUNC) && !g_CommandLine.test)
                continue;
            if (node->attributeFlags & ATTRIBUTE_FOREIGN)
                continue;
            if (!node->content)
                continue;

            typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        }

        ok &= emitInternalFunction(moduleToGen, one);

        if (node && node->attributeFlags & ATTRIBUTE_PUBLIC)
        {
            emitFuncSignaturePublic(bufferC, typeFunc, node);
            bufferC.addString(" {\n");
            bufferC.addString(format("%s();\n", one->callName().c_str()));
            bufferC.addString("}\n\n");
        }
    }

    return ok;
}
