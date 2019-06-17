#include "pch.h"
#include "Attribute.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "CommandLine.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Scope.h"

const char* BackendC::swagTypeToCType(TypeInfo* typeInfo)
{
    assert(typeInfo->kind == TypeInfoKind::Native);
    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
        return "swag_bool";
    case NativeType::S8:
        return "swag_int8_t";
    case NativeType::S16:
        return "swag_int16_t";
    case NativeType::S32:
        return "swag_int32_t";
    case NativeType::S64:
        return "swag_int64_t";
    case NativeType::U8:
        return "swag_uint8_t";
    case NativeType::U16:
        return "swag_uint16_t";
    case NativeType::U32:
        return "swag_uint32_t";
    case NativeType::U64:
        return "swag_uint64_t";
    case NativeType::F32:
        return "swag_float32_t";
    case NativeType::F64:
        return "swag_float64_t";
    case NativeType::Char:
        return "swag_char_t";
    case NativeType::String:
        return "const char*";
    case NativeType::Void:
        return "void";
    default:
        assert(false);
        return "";
    }
}

void BackendC::emitFuncSignatureSwg(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    if (!(node->attributeFlags & ATTRIBUTE_PUBLIC))
        return;

    bufferSwg.addString("func ");
    bufferSwg.addString(node->name.c_str());
    bufferSwg.addString(": (");
    bufferSwg.addString(")");

    bufferSwg.addString(";\n");
}

void BackendC::emitFuncSignaturePublic(Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    buffer.addString(swagTypeToCType(typeFunc->returnType));
    buffer.addString(" ");
    buffer.addString(module->name);
    buffer.addString("_");
    buffer.addString(node->name.c_str());
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

void BackendC::emitFuncSignatureInternalC(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    bufferC.addString("void __");
    bufferC.addString(node->name.c_str());
    bufferC.addString("(");

    // Result registers
    int cptParams = 0;
    if (typeFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        bufferC.addString("__register* rr0");
        cptParams++;
    }

    // Parameters
    for (auto param : typeFunc->parameters)
    {
        if (cptParams)
            bufferC.addString(", ");
        bufferC.addString(format("__register* rp%u", param->index));
        cptParams++;
    }

    bufferC.addString(")");
}

bool BackendC::emitFuncSignatures()
{
    emitSeparator(bufferSwg, "PROTOTYPES");
    emitSeparator(bufferH, "PROTOTYPES");
    emitSeparator(bufferC, "PROTOTYPES");

    bufferSwg.addString(format("#[swag.foreign(\"%s\")]\n", module->name.c_str()));
    bufferSwg.addString("{\n");

    for (auto one : module->byteCodeFunc)
    {
        auto node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

        // Do we need to generate that function ?
        if (node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        if ((node->attributeFlags & ATTRIBUTE_TEST) && !g_CommandLine.test)
            continue;
        if (node->attributeFlags & ATTRIBUTE_FOREIGN)
            continue;

        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

        emitFuncSignatureInternalC(typeFunc, node);
        bufferC.addString(";\n");

        if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        {
            bufferH.addString("SWAG_EXTERN SWAG_IMPEXP ");
            emitFuncSignaturePublic(bufferH, typeFunc, node);
            bufferH.addString(";\n");
        }

        emitFuncSignatureSwg(typeFunc, node);
    }

    bufferSwg.addString("}\n");
    bufferC.addString("\n");
    return true;
}

bool BackendC::emitInternalFunction(TypeInfoFuncAttr* typeFunc, AstFuncDecl* node)
{
    bool ok = true;

    // Signature
    bufferC.addString("static ");
    emitFuncSignatureInternalC(typeFunc, node);
    bufferC.addString(" {\n");

    // Generate one local variable per used register
    auto bc = node->bc;
    if (bc->usedRegisters.size())
    {
        int index = 0;
        for (auto r : bc->usedRegisters)
        {
            if (index == 0)
                bufferC.addString("__register ");
            else
                bufferC.addString(", ");
            index = (index + 1) % 10;
            bufferC.addString(format("r%d", r));
            if (index == 0)
                bufferC.addString(";\n");
        }

        bufferC.addString(";\n");
    }

    // Generate one variable per function call parameter
    if (bc->maxCallParameters)
    {
        int index = 0;
        for (int i = 0; i < bc->maxCallParameters; i++)
        {
            if (index == 0)
                bufferC.addString("__register ");
            else
                bufferC.addString(", ");
            bufferC.addString(format("rc%u", index));
            index = (index + 1) % 10;
            if (index == 0)
                bufferC.addString(";\n");
        }

        bufferC.addString(";\n");
    }

    // Local stack
    if (node->stackSize)
    {
        bufferC.addString(format("swag_uint8_t stack[%u];\n", node->stackSize));
    }

    // For function call results
    bufferC.addString("__register rt0;\n");

    // Generate bytecode
    auto ip = bc->out;
    for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
    {
        bufferC.addString(format("lbl%08u:; ", i));

        bufferC.addString(" /* ");
        for (int dec = g_ByteCodeOpNamesLen[(int) ip->op]; dec < ByteCode::ALIGN_RIGHT_OPCODE; dec++)
            bufferC.addString(" ");
        bufferC.addString(g_ByteCodeOpNames[(int) ip->op]);
        bufferC.addString(" */ ");

        switch (ip->op)
        {
        case ByteCodeOp::End:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::IncSP:
        case ByteCodeOp::MovSPBP:
            break;

        case ByteCodeOp::RCxRefFromStack:
            bufferC.addString(format("r%u.pointer = stack + %u;", ip->a.u32, ip->b.s32));
            break;
        case ByteCodeOp::RCxFromStack8:
            bufferC.addString(format("r%u.u8 = *(swag_uint8_t*) (stack + %d);", ip->a.u32, ip->b.s32));
            break;
        case ByteCodeOp::RCxFromStack16:
            bufferC.addString(format("r%u.u16 = *(swag_uint16_t*) (stack + %d);", ip->a.u32, ip->b.s32));
            break;
        case ByteCodeOp::RCxFromStack32:
            bufferC.addString(format("r%u.u32 = *(swag_uint32_t*) (stack + %d);", ip->a.u32, ip->b.s32));
            break;
        case ByteCodeOp::RCxFromStack64:
            bufferC.addString(format("r%u.u64 = *(swag_uint64_t*) (stack + %d);", ip->a.u32, ip->b.s32));
            break;
        case ByteCodeOp::CopyRCxVa32:
            bufferC.addString(format("r%u.u32 = 0x%x;", ip->b.u32, ip->a.u32));
            break;
        case ByteCodeOp::CopyRCxVa64:
            bufferC.addString(format("r%u.u64 = 0x%I64x;", ip->b.u32, ip->a.u64));
            break;
        case ByteCodeOp::CopyRCxVaStr:
            bufferC.addString(format("r%u.pointer = __string%u;", ip->b.u32, ip->a.u32));
            break;

        case ByteCodeOp::AffectOp8:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) = r%u.u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) = r%u.u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) = r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) = r%u.u64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::BinOpPlusS32:
            bufferC.addString(format("r%u.s32 = r%u.s32 + r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusS64:
            bufferC.addString(format("r%u.s64 = r%u.s64 + r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusU32:
            bufferC.addString(format("r%u.u32 = r%u.u32 + r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusU64:
            bufferC.addString(format("r%u.u64 = r%u.u64 + r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusF32:
            bufferC.addString(format("r%u.f32 = r%u.f32 + r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpPlusF64:
            bufferC.addString(format("r%u.f64= r%u.f64 + r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::BinOpMinusS32:
            bufferC.addString(format("r%u.s32 = r%u.s32 - r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusS64:
            bufferC.addString(format("r%u.s64 = r%u.s64 - r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusU32:
            bufferC.addString(format("r%u.u32 = r%u.u32 - r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusU64:
            bufferC.addString(format("r%u.u64 = r%u.u64 - r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusF32:
            bufferC.addString(format("r%u.f32 = r%u.f32 - r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMinusF64:
            bufferC.addString(format("r%u.f64 = r%u.f64 - r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::BinOpMulS32:
            bufferC.addString(format("r%u.s32 = r%u.s32 * r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulS64:
            bufferC.addString(format("r%u.s64 = r%u.s64 * r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulU32:
            bufferC.addString(format("r%u.u32 = r%u.u32 * r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulU64:
            bufferC.addString(format("r%u.u64 = r%u.u64 * r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulF32:
            bufferC.addString(format("r%u.f32 = r%u.f32 * r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpMulF64:
            bufferC.addString(format("r%u.f64= r%u.f64 * r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::XorS32:
            bufferC.addString(format("r%u.s32 = r%u.s32 ^ r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::XorS64:
            bufferC.addString(format("r%u.s64 = r%u.s64 ^ r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::XorU32:
            bufferC.addString(format("r%u.u32 = r%u.u32 ^ r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::XorU64:
            bufferC.addString(format("r%u.u64 = r%u.u64 ^ r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::ShiftLeftS32:
            bufferC.addString(format("r%u.s32 = r%u.s32 << r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftLeftS64:
            bufferC.addString(format("r%u.s64 = r%u.s64 << r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftLeftU32:
            bufferC.addString(format("r%u.u32 = r%u.u32 << r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftLeftU64:
            bufferC.addString(format("r%u.u64 = r%u.u64 << r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::ShiftRightS32:
            bufferC.addString(format("r%u.s32 = r%u.s32 >> r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftRightS64:
            bufferC.addString(format("r%u.s64 = r%u.s64 >> r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftRightU32:
            bufferC.addString(format("r%u.u32 = r%u.u32 >> r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ShiftRightU64:
            bufferC.addString(format("r%u.u64 = r%u.u64 >> r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::BinOpDivF32:
            bufferC.addString(format("r%u.f32 = r%u.f32 / r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpDivF64:
            bufferC.addString(format("r%u.f64 = r%u.f64 / r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpMinusEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r%u.pointer) -= r%u.s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r%u.pointer) -= r%u.s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r%u.pointer) -= r%u.s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r%u.pointer) -= r%u.s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) -= r%u.u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) -= r%u.u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) -= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) -= r%u.u64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqF32:
            bufferC.addString(format("*(swag_float32_t*)(r%u.pointer) -= r%u.f32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqF64:
            bufferC.addString(format("*(swag_float64_t*)(r%u.pointer) -= r%u.f64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpPlusEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r%u.pointer) += r%u.s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r%u.pointer) += r%u.s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r%u.pointer) += r%u.s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r%u.pointer) += r%u.s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) += r%u.u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) += r%u.u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) += r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) += r%u.u64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqF32:
            bufferC.addString(format("*(swag_float32_t*)(r%u.pointer) += r%u.f32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPlusEqF64:
            bufferC.addString(format("*(swag_float64_t*)(r%u.pointer) += r%u.f64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpMulEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r%u.pointer) *= r%u.s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r%u.pointer) *= r%u.s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r%u.pointer) *= r%u.s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r%u.pointer) *= r%u.s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) *= r%u.u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) *= r%u.u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) *= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) *= r%u.u64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqF32:
            bufferC.addString(format("*(swag_float32_t*)(r%u.pointer) *= r%u.f32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMulEqF64:
            bufferC.addString(format("*(swag_float64_t*)(r%u.pointer) *= r%u.f64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpDivEqF32:
            bufferC.addString(format("*(swag_float32_t*)(r%u.pointer) /= r%u.f32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpDivEqF64:
            bufferC.addString(format("*(swag_float64_t*)(r%u.pointer) /= r%u.f64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpAndEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r%u.pointer) &= r%u.s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r%u.pointer) &= r%u.s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r%u.pointer) &= r%u.s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r%u.pointer) &= r%u.s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) &= r%u.u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) &= r%u.u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) &= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpAndEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) &= r%u.u64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpOrEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r%u.pointer) |= r%u.s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r%u.pointer) |= r%u.s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r%u.pointer) |= r%u.s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r%u.pointer) |= r%u.s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) |= r%u.u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) |= r%u.u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) |= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpOrEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) |= r%u.u64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpShiftLeftEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftLeftEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpShiftRightEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpShiftRightEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOpXOrEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r%u.pointer) ^= r%u.s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r%u.pointer) ^= r%u.s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r%u.pointer) ^= r%u.s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r%u.pointer) ^= r%u.s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) ^= r%u.u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) ^= r%u.u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) ^= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpXOrEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) ^= r%u.u64;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CompareOpGreaterS32:
            bufferC.addString(format("r%u.b = r%u.s32 > r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterS64:
            bufferC.addString(format("r%u.b = r%u.s64 > r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterU32:
            bufferC.addString(format("r%u.b = r%u.u32 > r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterU64:
            bufferC.addString(format("r%u.b = r%u.u64 > r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterF32:
            bufferC.addString(format("r%u.b = r%u.f32 > r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpGreaterF64:
            bufferC.addString(format("r%u.b = r%u.f64 > r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CompareOpLowerS32:
            bufferC.addString(format("r%u.b = r%u.s32 < r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerS64:
            bufferC.addString(format("r%u.b = r%u.s64 < r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerU32:
            bufferC.addString(format("r%u.b = r%u.u32 < r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerU64:
            bufferC.addString(format("r%u.b = r%u.u64 < r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerF32:
            bufferC.addString(format("r%u.b = r%u.f32 < r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpLowerF64:
            bufferC.addString(format("r%u.b = r%u.f64 < r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CompareOpEqual32:
            bufferC.addString(format("r%u.b = r%u.u32 == r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqual64:
            bufferC.addString(format("r%u.b = r%u.u64 == r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqualBool:
            bufferC.addString(format("r%u.b = r%u.b == r%u.b;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::Jump:
            bufferC.addString(format("goto lbl%08u;", ip->a.s32 + i + 1));
            break;
        case ByteCodeOp::JumpNotTrue:
            bufferC.addString(format("if(!r%d.b) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1));
            break;
        case ByteCodeOp::Ret:
            bufferC.addString("return;");
            break;

        case ByteCodeOp::IntrinsicPrintS32:
            bufferC.addString(format("__print_i32(r%u.s32);", ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicPrintS64:
            bufferC.addString(format("__print_i64(r%u.s64);", ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicPrintF32:
            bufferC.addString(format("__print_f32(r%u.f32);", ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicPrintF64:
            bufferC.addString(format("__print_f64(r%u.f64);", ip->a.u32));
            break;
        case ByteCodeOp::IntrinsicPrintString:
            bufferC.addString(format("__print((const char*) r%u.pointer);", ip->a.u32));
            break;

        case ByteCodeOp::IntrinsicAssert:
            bufferC.addString(format("__assert(r%u.b, \"%s\", %d);", ip->a.u32, normalizePath(module->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1));
            break;

        case ByteCodeOp::NegBool:
            bufferC.addString(format("r%u.b = !r%u.b;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::NegF32:
            bufferC.addString(format("r%u.f32 = -r%u.f32;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::NegF64:
            bufferC.addString(format("r%u.f64 = -r%u.f64;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::NegS32:
            bufferC.addString(format("r%u.s32 = -r%u.s32;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::NegS64:
            bufferC.addString(format("r%u.s64 = -r%u.s64;", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::InvertS32:
            bufferC.addString(format("r%u.s32 = ~r%u.s32;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::InvertS64:
            bufferC.addString(format("r%u.s64 = ~r%u.s64;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::InvertU32:
            bufferC.addString(format("r%u.u32 = ~r%u.u32;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::InvertU64:
            bufferC.addString(format("r%u.u64 = ~r%u.u64;", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::ClearMaskU32:
            bufferC.addString(format("r%u.u32 &= 0x%x;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ClearMaskU64:
            bufferC.addString(format("r%u.u64 &= 0xFFFFFFFF | ((uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ClearMaskU32U64:
            bufferC.addString(format("r%u.u32 &= 0x%x; ", ip->a.u32, ip->b.u32));
            bufferC.addString(format("r%u.u64 &= 0xFFFFFFFF | ((uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CopyRRxRCx:
            bufferC.addString(format("*rr%u = r%u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRCxRRx:
            bufferC.addString(format("r%u = rt%u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RCxFromStackParam64:
            bufferC.addString(format("r%u = *rp%u;", ip->a.u32, ip->c.u32));
            break;
        case ByteCodeOp::PushRCxSaved:
        case ByteCodeOp::PopRCxSaved:
            break;
        case ByteCodeOp::PushRCxParam:
            bufferC.addString(format("rc%u = r%u;", ip->b.u32, ip->a.u32));
            break;

        case ByteCodeOp::LocalCall:
        {
            auto funcBC     = (ByteCode*) ip->a.pointer;
            auto typeFuncBC = CastTypeInfo<TypeInfoFuncAttr>(funcBC->node->typeInfo, TypeInfoKind::FuncAttr);
            bufferC.addString(format("__%s(", funcBC->node->name.c_str()));

            int cptCall = 0;
            if (typeFuncBC->returnType != g_TypeMgr.typeInfoVoid)
            {
                bufferC.addString("&rt0");
                cptCall++;
            }

            for (int idxCall = 0; idxCall < (int) typeFuncBC->parameters.size(); idxCall++)
            {
                if (cptCall)
                    bufferC.addString(", ");
                bufferC.addString(format("&rc%u", idxCall));
                cptCall++;
            }

            bufferC.addString(");");
        }
        break;

        case ByteCodeOp::ForeignCall:
        {
            auto nodeFunc     = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
            auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::FuncAttr);
            bufferC.addString(format("%s_%s", nodeFunc->ownerScope->name.c_str(), nodeFunc->name.c_str()));
			bufferC.addString("();");
        }
        break;

        default:
            ok = false;
            bufferC.addString("// ");
            bufferC.addString(g_ByteCodeOpNames[(int) ip->op]);
            module->internalError(format("unknown instruction '%s' during backend generation", g_ByteCodeOpNames[(int) ip->op]));
            break;
        }

        bufferC.addString("\n");
    }

    bufferC.addString("}\n\n");
    return ok;
}

bool BackendC::emitFunctions()
{
    emitSeparator(bufferC, "INTERNAL FUNCTIONS");

    bool ok = true;
    for (auto one : module->byteCodeFunc)
    {
        auto node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

        // Do we need to generate that function ?
        if (node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        if ((node->attributeFlags & ATTRIBUTE_TEST) && !g_CommandLine.test)
            continue;
        if (node->attributeFlags & ATTRIBUTE_FOREIGN)
            continue;

        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        ok &= emitInternalFunction(typeFunc, node);

        if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        {
            emitFuncSignaturePublic(bufferC, typeFunc, node);
            bufferC.addString(" {\n");
            bufferC.addString(format("__%s();\n", node->name.c_str()));
            bufferC.addString("}\n\n");
        }
    }

    return ok;
}
