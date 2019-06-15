#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "AstNode.h"
#include "CommandLine.h"
#include "Ast.h"
#include "TypeInfo.h"
#include "TypeManager.h"

bool BackendC::emitFuncSignature(AstFuncDecl* node)
{
    outputC.addString("void __");
    outputC.addString(node->name.c_str());
    outputC.addString("(");

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

    // Result registers
    int cptParams = 0;
    if (typeFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        outputC.addString("__register* rr0");
        cptParams++;
    }

    // Parameters
    for (auto param : typeFunc->parameters)
    {
        if (cptParams)
            outputC.addString(", ");
        outputC.addString(format("__register* rp%u", param->index));
        cptParams++;
    }

    outputC.addString(")");
    return true;
}

bool BackendC::emitFuncSignatures()
{
	outputC.addString("/****************************** PROTOTYPES ****************************/\n");
    for (auto one : module->byteCodeFunc)
    {
        auto node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

        // Do we need to generate that function ?
        if (node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        if ((node->attributeFlags & ATTRIBUTE_TEST) && !g_CommandLine.test)
            continue;

        emitFuncSignature(node);
        outputC.addString(";\n");
    }

    outputC.addString("\n");
    return true;
}

bool BackendC::emitFunctions()
{
	outputC.addString("/****************************** FUNCTIONS *****************************/\n");
    bool ok = true;
    for (auto one : module->byteCodeFunc)
    {
        auto node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

        // Do we need to generate that function ?
        if (node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        if ((node->attributeFlags & ATTRIBUTE_TEST) && !g_CommandLine.test)
            continue;

        // Signature
        emitFuncSignature(node);
        outputC.addString(" {\n");

        // Generate one local variable per used register
        auto bc = node->bc;
        if (bc->usedRegisters.size())
        {
            int index = 0;
            for (auto r : bc->usedRegisters)
            {
                if (index == 0)
                    outputC.addString("__register ");
                else
                    outputC.addString(", ");
                index = (index + 1) % 10;
                outputC.addString(format("r%d", r));
                if (index == 0)
                    outputC.addString(";\n");
            }

            outputC.addString(";\n");
        }

        // Generate one variable per function call parameter
        if (bc->maxCallParameters)
        {
            int index = 0;
            for (int i = 0; i < bc->maxCallParameters; i++)
            {
                if (index == 0)
                    outputC.addString("__register ");
                else
                    outputC.addString(", ");
                outputC.addString(format("rc%u", index));
                index = (index + 1) % 10;
                if (index == 0)
                    outputC.addString(";\n");
            }

            outputC.addString(";\n");
        }

        // Local stack
        if (node->stackSize)
        {
            outputC.addString(format("swag_uint8_t stack[%u];\n", node->stackSize));
        }

        // For function call results
        outputC.addString("__register rt0;\n");

        // Generate bytecode
        auto ip = bc->out;
        for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
        {
            outputC.addString(format("lbl%08u:; ", i));

			outputC.addString(" /* ");
            for (int dec = g_ByteCodeOpNamesLen[(int) ip->op]; dec < ByteCode::ALIGN_RIGHT_OPCODE; dec++)
                outputC.addString(" ");
            outputC.addString(g_ByteCodeOpNames[(int) ip->op]);
			outputC.addString(" */ ");

            switch (ip->op)
            {
            case ByteCodeOp::End:
            case ByteCodeOp::DecSP:
            case ByteCodeOp::IncSP:
            case ByteCodeOp::MovSPBP:
                break;

            case ByteCodeOp::RCxRefFromStack:
                outputC.addString(format("r%u.pointer = stack + %u;", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::RCxFromStack8:
                outputC.addString(format("r%u.u8 = *(swag_uint8_t*) (stack + %d);", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::RCxFromStack16:
                outputC.addString(format("r%u.u16 = *(swag_uint16_t*) (stack + %d);", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::RCxFromStack32:
                outputC.addString(format("r%u.u32 = *(swag_uint32_t*) (stack + %d);", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::RCxFromStack64:
                outputC.addString(format("r%u.u64 = *(swag_uint64_t*) (stack + %d);", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::CopyRCxVa32:
                outputC.addString(format("r%u.u32 = 0x%x;", ip->b.u32, ip->a.u32));
                break;
            case ByteCodeOp::CopyRCxVa64:
                outputC.addString(format("r%u.u64 = 0x%I64x;", ip->b.u32, ip->a.u64));
                break;
            case ByteCodeOp::CopyRCxVaStr:
                outputC.addString(format("r%u.pointer = __string%u;", ip->b.u32, ip->a.u32));
                break;

            case ByteCodeOp::AffectOp8:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) = r%u.u8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOp16:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) = r%u.u16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOp32:
                outputC.addString(format("*(swag_uint32_t*)(r%u.pointer) = r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOp64:
                outputC.addString(format("*(swag_uint64_t*)(r%u.pointer) = r%u.u64;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::BinOpPlusS32:
                outputC.addString(format("r%u.s32 = r%u.s32 + r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpPlusS64:
                outputC.addString(format("r%u.s64 = r%u.s64 + r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpPlusU32:
                outputC.addString(format("r%u.u32 = r%u.u32 + r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpPlusU64:
                outputC.addString(format("r%u.u64 = r%u.u64 + r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpPlusF32:
                outputC.addString(format("r%u.f32 = r%u.f32 + r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpPlusF64:
                outputC.addString(format("r%u.f64= r%u.f64 + r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::BinOpMinusS32:
                outputC.addString(format("r%u.s32 = r%u.s32 - r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMinusS64:
                outputC.addString(format("r%u.s64 = r%u.s64 - r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMinusU32:
                outputC.addString(format("r%u.u32 = r%u.u32 - r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMinusU64:
                outputC.addString(format("r%u.u64 = r%u.u64 - r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMinusF32:
                outputC.addString(format("r%u.f32 = r%u.f32 - r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMinusF64:
                outputC.addString(format("r%u.f64 = r%u.f64 - r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::BinOpMulS32:
                outputC.addString(format("r%u.s32 = r%u.s32 * r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMulS64:
                outputC.addString(format("r%u.s64 = r%u.s64 * r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMulU32:
                outputC.addString(format("r%u.u32 = r%u.u32 * r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMulU64:
                outputC.addString(format("r%u.u64 = r%u.u64 * r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMulF32:
                outputC.addString(format("r%u.f32 = r%u.f32 * r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMulF64:
                outputC.addString(format("r%u.f64= r%u.f64 * r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::XorS32:
                outputC.addString(format("r%u.s32 = r%u.s32 ^ r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::XorS64:
                outputC.addString(format("r%u.s64 = r%u.s64 ^ r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::XorU32:
                outputC.addString(format("r%u.u32 = r%u.u32 ^ r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::XorU64:
                outputC.addString(format("r%u.u64 = r%u.u64 ^ r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::ShiftLeftS32:
                outputC.addString(format("r%u.s32 = r%u.s32 << r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::ShiftLeftS64:
                outputC.addString(format("r%u.s64 = r%u.s64 << r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::ShiftLeftU32:
                outputC.addString(format("r%u.u32 = r%u.u32 << r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::ShiftLeftU64:
                outputC.addString(format("r%u.u64 = r%u.u64 << r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::ShiftRightS32:
                outputC.addString(format("r%u.s32 = r%u.s32 >> r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::ShiftRightS64:
                outputC.addString(format("r%u.s64 = r%u.s64 >> r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::ShiftRightU32:
                outputC.addString(format("r%u.u32 = r%u.u32 >> r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::ShiftRightU64:
                outputC.addString(format("r%u.u64 = r%u.u64 >> r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::BinOpDivF32:
                outputC.addString(format("r%u.f32 = r%u.f32 / r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpDivF64:
                outputC.addString(format("r%u.f64 = r%u.f64 / r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::AffectOpMinusEqS8:
                outputC.addString(format("*(swag_int8_t*)(r%u.pointer) -= r%u.s8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqS16:
                outputC.addString(format("*(swag_int16_t*)(r%u.pointer) -= r%u.s16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqS32:
                outputC.addString(format("*(swag_int32_t*)(r%u.pointer) -= r%u.s32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqS64:
                outputC.addString(format("*(swag_int64_t*)(r%u.pointer) -= r%u.s64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqU8:
                outputC.addString(format("*(swag_uint8_t*)(r%u.pointer) -= r%u.u8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqU16:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) -= r%u.u16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqU32:
                outputC.addString(format("*(swag_uint32_t*)(r%u.pointer) -= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqU64:
                outputC.addString(format("*(swag_uint64_t*)(r%u.pointer) -= r%u.u64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqF32:
                outputC.addString(format("*(swag_float32_t*)(r%u.pointer) -= r%u.f32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqF64:
                outputC.addString(format("*(swag_float64_t*)(r%u.pointer) -= r%u.f64;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::AffectOpPlusEqS8:
                outputC.addString(format("*(swag_int8_t*)(r%u.pointer) += r%u.s8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpPlusEqS16:
                outputC.addString(format("*(swag_int16_t*)(r%u.pointer) += r%u.s16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpPlusEqS32:
                outputC.addString(format("*(swag_int32_t*)(r%u.pointer) += r%u.s32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpPlusEqS64:
                outputC.addString(format("*(swag_int64_t*)(r%u.pointer) += r%u.s64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpPlusEqU8:
                outputC.addString(format("*(swag_uint8_t*)(r%u.pointer) += r%u.u8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpPlusEqU16:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) += r%u.u16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpPlusEqU32:
                outputC.addString(format("*(swag_uint32_t*)(r%u.pointer) += r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpPlusEqU64:
                outputC.addString(format("*(swag_uint64_t*)(r%u.pointer) += r%u.u64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpPlusEqF32:
                outputC.addString(format("*(swag_float32_t*)(r%u.pointer) += r%u.f32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpPlusEqF64:
                outputC.addString(format("*(swag_float64_t*)(r%u.pointer) += r%u.f64;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::AffectOpMulEqS8:
                outputC.addString(format("*(swag_int8_t*)(r%u.pointer) *= r%u.s8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMulEqS16:
                outputC.addString(format("*(swag_int16_t*)(r%u.pointer) *= r%u.s16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMulEqS32:
                outputC.addString(format("*(swag_int32_t*)(r%u.pointer) *= r%u.s32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMulEqS64:
                outputC.addString(format("*(swag_int64_t*)(r%u.pointer) *= r%u.s64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMulEqU8:
                outputC.addString(format("*(swag_uint8_t*)(r%u.pointer) *= r%u.u8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMulEqU16:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) *= r%u.u16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMulEqU32:
                outputC.addString(format("*(swag_uint32_t*)(r%u.pointer) *= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMulEqU64:
                outputC.addString(format("*(swag_uint64_t*)(r%u.pointer) *= r%u.u64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMulEqF32:
                outputC.addString(format("*(swag_float32_t*)(r%u.pointer) *= r%u.f32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMulEqF64:
                outputC.addString(format("*(swag_float64_t*)(r%u.pointer) *= r%u.f64;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::AffectOpDivEqF32:
                outputC.addString(format("*(swag_float32_t*)(r%u.pointer) /= r%u.f32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpDivEqF64:
                outputC.addString(format("*(swag_float64_t*)(r%u.pointer) /= r%u.f64;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::AffectOpAndEqS8:
                outputC.addString(format("*(swag_int8_t*)(r%u.pointer) &= r%u.s8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpAndEqS16:
                outputC.addString(format("*(swag_int16_t*)(r%u.pointer) &= r%u.s16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpAndEqS32:
                outputC.addString(format("*(swag_int32_t*)(r%u.pointer) &= r%u.s32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpAndEqS64:
                outputC.addString(format("*(swag_int64_t*)(r%u.pointer) &= r%u.s64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpAndEqU8:
                outputC.addString(format("*(swag_uint8_t*)(r%u.pointer) &= r%u.u8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpAndEqU16:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) &= r%u.u16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpAndEqU32:
                outputC.addString(format("*(swag_uint32_t*)(r%u.pointer) &= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpAndEqU64:
                outputC.addString(format("*(swag_uint64_t*)(r%u.pointer) &= r%u.u64;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::AffectOpOrEqS8:
                outputC.addString(format("*(swag_int8_t*)(r%u.pointer) |= r%u.s8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpOrEqS16:
                outputC.addString(format("*(swag_int16_t*)(r%u.pointer) |= r%u.s16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpOrEqS32:
                outputC.addString(format("*(swag_int32_t*)(r%u.pointer) |= r%u.s32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpOrEqS64:
                outputC.addString(format("*(swag_int64_t*)(r%u.pointer) |= r%u.s64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpOrEqU8:
                outputC.addString(format("*(swag_uint8_t*)(r%u.pointer) |= r%u.u8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpOrEqU16:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) |= r%u.u16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpOrEqU32:
                outputC.addString(format("*(swag_uint32_t*)(r%u.pointer) |= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpOrEqU64:
                outputC.addString(format("*(swag_uint64_t*)(r%u.pointer) |= r%u.u64;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::AffectOpShiftLeftEqS8:
                outputC.addString(format("*(swag_int8_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftLeftEqS16:
                outputC.addString(format("*(swag_int16_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftLeftEqS32:
                outputC.addString(format("*(swag_int32_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftLeftEqS64:
                outputC.addString(format("*(swag_int64_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftLeftEqU8:
                outputC.addString(format("*(swag_uint8_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftLeftEqU16:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftLeftEqU32:
                outputC.addString(format("*(swag_uint32_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftLeftEqU64:
                outputC.addString(format("*(swag_uint64_t*)(r%u.pointer) <<= r%u.u32;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::AffectOpShiftRightEqS8:
                outputC.addString(format("*(swag_int8_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftRightEqS16:
                outputC.addString(format("*(swag_int16_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftRightEqS32:
                outputC.addString(format("*(swag_int32_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftRightEqS64:
                outputC.addString(format("*(swag_int64_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftRightEqU8:
                outputC.addString(format("*(swag_uint8_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftRightEqU16:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftRightEqU32:
                outputC.addString(format("*(swag_uint32_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpShiftRightEqU64:
                outputC.addString(format("*(swag_uint64_t*)(r%u.pointer) >>= r%u.u32;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::AffectOpXOrEqS8:
                outputC.addString(format("*(swag_int8_t*)(r%u.pointer) ^= r%u.s8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpXOrEqS16:
                outputC.addString(format("*(swag_int16_t*)(r%u.pointer) ^= r%u.s16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpXOrEqS32:
                outputC.addString(format("*(swag_int32_t*)(r%u.pointer) ^= r%u.s32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpXOrEqS64:
                outputC.addString(format("*(swag_int64_t*)(r%u.pointer) ^= r%u.s64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpXOrEqU8:
                outputC.addString(format("*(swag_uint8_t*)(r%u.pointer) ^= r%u.u8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpXOrEqU16:
                outputC.addString(format("*(swag_uint16_t*)(r%u.pointer) ^= r%u.u16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpXOrEqU32:
                outputC.addString(format("*(swag_uint32_t*)(r%u.pointer) ^= r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpXOrEqU64:
                outputC.addString(format("*(swag_uint64_t*)(r%u.pointer) ^= r%u.u64;", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::CompareOpGreaterS32:
                outputC.addString(format("r%u.b = r%u.s32 > r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpGreaterS64:
                outputC.addString(format("r%u.b = r%u.s64 > r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpGreaterU32:
                outputC.addString(format("r%u.b = r%u.u32 > r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpGreaterU64:
                outputC.addString(format("r%u.b = r%u.u64 > r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpGreaterF32:
                outputC.addString(format("r%u.b = r%u.f32 > r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpGreaterF64:
                outputC.addString(format("r%u.b = r%u.f64 > r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::CompareOpLowerS32:
                outputC.addString(format("r%u.b = r%u.s32 < r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpLowerS64:
                outputC.addString(format("r%u.b = r%u.s64 < r%u.s64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpLowerU32:
                outputC.addString(format("r%u.b = r%u.u32 < r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpLowerU64:
                outputC.addString(format("r%u.b = r%u.u64 < r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpLowerF32:
                outputC.addString(format("r%u.b = r%u.f32 < r%u.f32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpLowerF64:
                outputC.addString(format("r%u.b = r%u.f64 < r%u.f64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::CompareOpEqual32:
                outputC.addString(format("r%u.b = r%u.u32 == r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpEqual64:
                outputC.addString(format("r%u.b = r%u.u64 == r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpEqualBool:
                outputC.addString(format("r%u.b = r%u.b == r%u.b;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::Jump:
                outputC.addString(format("goto lbl%08u;", ip->a.s32 + i + 1));
                break;
            case ByteCodeOp::JumpNotTrue:
                outputC.addString(format("if(!r%d.b) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1));
                break;
            case ByteCodeOp::Ret:
                outputC.addString("return;");
                break;

            case ByteCodeOp::IntrinsicPrintS32:
                outputC.addString(format("__print_i32(r%u.s32);", ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicPrintS64:
                outputC.addString(format("__print_i64(r%u.s64);", ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicPrintF32:
                outputC.addString(format("__print_f32(r%u.f32);", ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicPrintF64:
                outputC.addString(format("__print_f64(r%u.f64);", ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicPrintString:
                outputC.addString(format("__print((const char*) r%u.pointer);", ip->a.u32));
                break;

            case ByteCodeOp::IntrinsicAssert:
                outputC.addString(format("__assert(r%u.b, \"%s\", %d);", ip->a.u32, module->files[ip->sourceFileIdx]->path.string().c_str(), ip->startLocation.line + 1));
                break;

            case ByteCodeOp::NegBool:
                outputC.addString(format("r%u.b = !r%u.b;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::NegF32:
                outputC.addString(format("r%u.f32 = -r%u.f32;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::NegF64:
                outputC.addString(format("r%u.f64 = -r%u.f64;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::NegS32:
                outputC.addString(format("r%u.s32 = -r%u.s32;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::NegS64:
                outputC.addString(format("r%u.s64 = -r%u.s64;", ip->a.u32, ip->a.u32));
                break;

            case ByteCodeOp::InvertS32:
                outputC.addString(format("r%u.s32 = ~r%u.s32;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::InvertS64:
                outputC.addString(format("r%u.s64 = ~r%u.s64;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::InvertU32:
                outputC.addString(format("r%u.u32 = ~r%u.u32;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::InvertU64:
                outputC.addString(format("r%u.u64 = ~r%u.u64;", ip->a.u32, ip->a.u32));
                break;

            case ByteCodeOp::ClearMaskU32:
                outputC.addString(format("r%u.u32 &= 0x%x;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::ClearMaskU64:
                outputC.addString(format("r%u.u64 &= 0xFFFFFFFF | ((uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::ClearMaskU32U64:
                outputC.addString(format("r%u.u32 &= 0x%x; ", ip->a.u32, ip->b.u32));
                outputC.addString(format("r%u.u64 &= 0xFFFFFFFF | ((uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32));
                break;

            case ByteCodeOp::CopyRRxRCx:
                outputC.addString(format("*rr%u = r%u;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CopyRCxRRx:
                outputC.addString(format("r%u = rt%u;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::RCxFromStackParam64:
                outputC.addString(format("r%u = *rp%u;", ip->a.u32, ip->c.u32));
                break;
            case ByteCodeOp::PushRCxSaved:
            case ByteCodeOp::PopRCxSaved:
                break;
            case ByteCodeOp::PushRCxParam:
                outputC.addString(format("rc%u = r%u;", ip->b.u32, ip->a.u32));
                break;
            case ByteCodeOp::LocalFuncCall:
            {
                auto funcBC     = (ByteCode*) ip->a.pointer;
                auto typeFuncBC = CastTypeInfo<TypeInfoFuncAttr>(funcBC->node->typeInfo, TypeInfoKind::FuncAttr);
                outputC.addString(format("__%s(", funcBC->node->name.c_str()));

                int cptCall = 0;
                if (typeFuncBC->returnType != g_TypeMgr.typeInfoVoid)
                {
                    outputC.addString("&rt0");
                    cptCall++;
                }

                for (int idxCall = 0; idxCall < (int) typeFuncBC->parameters.size(); idxCall++)
                {
                    if (cptCall)
                        outputC.addString(", ");
                    outputC.addString(format("&rc%u", idxCall));
                    cptCall++;
                }

                outputC.addString(");");
            }
            break;
            default:
                ok = false;
                outputC.addString("// ");
                outputC.addString(g_ByteCodeOpNames[(int) ip->op]);
                module->internalError(format("unknown instruction '%s' during bytecode generation", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }

            outputC.addString("\n");
        }

        outputC.addString("}\n\n");
    }

    return ok;
}
