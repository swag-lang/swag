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
#include "SymTable.h"

const char* BackendC::swagTypeToCType(TypeInfo* typeInfo)
{
    SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);

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
        SWAG_ASSERT(false);
        return "";
    }
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
        bufferC.addString(format("__register* rr%d", i));
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
            bufferC.addString(format("__register* rp%u", index++));
        }
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
        TypeInfoFuncAttr* typeFunc = one->typeInfoFunc;
        AstFuncDecl*      node     = nullptr;

        if (one->node)
        {
            node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

            // Do we need to generate that function ?
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if ((node->attributeFlags & ATTRIBUTE_TEST) && !g_CommandLine.unittest)
                continue;
            if (node->attributeFlags & ATTRIBUTE_FOREIGN)
                continue;

            typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        }

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

bool BackendC::emitInternalFunction(ByteCode* bc)
{
    bool ok       = true;
    auto typeFunc = bc->callType();

    // Signature
    bufferC.addString("static ");
    emitFuncSignatureInternalC(bc);
    bufferC.addString(" {\n");

    // Generate one local variable per used register
	{
		int index = 0;
		for (uint32_t r = 0; r < bc->maxReservedRegisterRC; r++)
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
    // Put in in reverse order, so that if we address one rc register, we got older one just after (useful for variadic)
    if (bc->maxCallParameters)
    {
        int index = 0;
        for (int i = bc->maxCallParameters - 1; i >= 0; i--)
        {
            if (index == 0)
                bufferC.addString("__register ");
            else
                bufferC.addString(", ");
            bufferC.addString(format("rc%u", i));
            index = (index + 1) % 10;
            if (index == 0)
                bufferC.addString(";\n");
        }

        bufferC.addString(";\n");
    }

    // For function call results
    if (bc->maxCallResults)
    {
        int index = 0;
        for (int i = 0; i < bc->maxCallResults; i++)
        {
            if (index == 0)
                bufferC.addString("__register ");
            else
                bufferC.addString(", ");
            bufferC.addString(format("rt%u", index));
            index = (index + 1) % 10;
            if (index == 0)
                bufferC.addString(";\n");
        }

        bufferC.addString(";\n");
    }

    // Local stack
    if (typeFunc->stackSize)
    {
        bufferC.addString(format("swag_uint8_t stack[%u];\n", typeFunc->stackSize));
    }

    // Generate bytecode
    int  vaargsIdx = 0;
    auto ip        = bc->out;
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

        case ByteCodeOp::BoundCheck:
            bufferC.addString(format("__assert(r%u.u32 <= r%u.u32, \"%s\", %d, \": error: index out of range\");", ip->a.u32, ip->b.u32, normalizePath(module->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1));
            break;
        case ByteCodeOp::BoundCheckString:
            bufferC.addString(format("__assert(r%u.u32 <= r%u.u32 + 1, \"%s\", %d, \": error: index out of range\");", ip->a.u32, ip->b.u32, normalizePath(module->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1));
            break;
        case ByteCodeOp::BoundCheckReg:
            bufferC.addString(format("__assert(r%u.u32 < r%u.u32, \"%s\", %d, \": error: index out of range\");", ip->a.u32, ip->b.u32, normalizePath(module->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1));
            break;
        case ByteCodeOp::IncPointerVB:
            bufferC.addString(format("r%u.pointer += %u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::IncPointer:
            bufferC.addString(format("r%u.pointer = r%u.pointer + r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::DecPointer:
            bufferC.addString(format("r%u.pointer = r%u.pointer - r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::DeRef8:
            bufferC.addString(format("r%u.u64 = *(swag_uint8_t*) r%u.pointer;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::DeRef16:
            bufferC.addString(format("r%u.u64 = *(swag_uint16_t*) r%u.pointer;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::DeRef32:
            bufferC.addString(format("r%u.u64 = *(swag_uint32_t*) r%u.pointer;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::DeRef64:
            bufferC.addString(format("r%u.u64 = *(swag_uint64_t*) r%u.pointer;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::DeRefPointer:
            bufferC.addString(format("r%u.pointer = *(swag_uint8_t**) (r%u.pointer + %u);", ip->b.u32, ip->a.u32, ip->c.u32));
            break;
        case ByteCodeOp::DeRefString:
            bufferC.addString(format("r%u.u64 = *(swag_uint64_t*) (r%u.pointer + 8); ", ip->b.u32, ip->a.u32));
            bufferC.addString(format("r%u.pointer = *(swag_uint8_t**) r%u.pointer; ", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::MulRAVB:
            bufferC.addString(format("r%u.s32 *= %u;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::RAFromDataSeg8:
            bufferC.addString(format("r%u.u8 = *(swag_uint8_t*) (__dataseg + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromDataSeg16:
            bufferC.addString(format("r%u.u16 = *(swag_uint16_t*) (__dataseg + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromDataSeg32:
            bufferC.addString(format("r%u.u32 = *(swag_uint32_t*) (__dataseg + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromDataSeg64:
            bufferC.addString(format("r%u.u64 = *(swag_uint64_t*) (__dataseg + %u);", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::Clear8:
            bufferC.addString(format("*(swag_uint8_t*)r%u.pointer = 0;", ip->a.u32));
            break;
        case ByteCodeOp::Clear16:
            bufferC.addString(format("*(swag_uint16_t*)r%u.pointer = 0;", ip->a.u32));
            break;
        case ByteCodeOp::Clear32:
            bufferC.addString(format("*(swag_uint32_t*)r%u.pointer = 0;", ip->a.u32));
            break;
        case ByteCodeOp::Clear64:
            bufferC.addString(format("*(swag_uint64_t*)r%u.pointer = 0;", ip->a.u32));
            break;
        case ByteCodeOp::ClearX:
            bufferC.addString(format("__memclear(r%u.pointer, %u);", ip->a.u32, ip->b.u32));
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
            bufferC.addString(format("__memclear(stack + %u, %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RARefFromDataSeg:
            bufferC.addString(format("r%u.pointer = __dataseg + %u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RARefFromConstantSeg:
            bufferC.addString(format("r%u.pointer = __constantseg + %u; ", ip->a.u32, (uint32_t)(ip->c.u64 >> 32)));
            bufferC.addString(format("r%u.u64 = %u;", ip->b.u32, (ip->c.u64) & 0xFFFFFFFF));
            break;
        case ByteCodeOp::RARefFromStack:
            bufferC.addString(format("r%u.pointer = stack + %u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStack8:
            bufferC.addString(format("r%u.u8 = *(swag_uint8_t*) (stack + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStack16:
            bufferC.addString(format("r%u.u16 = *(swag_uint16_t*) (stack + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStack32:
            bufferC.addString(format("r%u.u32 = *(swag_uint32_t*) (stack + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStack64:
            bufferC.addString(format("r%u.u64 = *(swag_uint64_t*) (stack + %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::Copy:
            bufferC.addString(format("__memcpy(r%u.pointer, r%u.pointer, %d);", ip->a.u32, ip->b.u32, ip->c.u32));
            break;
        case ByteCodeOp::CopyRAVB32:
            bufferC.addString(format("r%u.u32 = 0x%x;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRAVB64:
            bufferC.addString(format("r%u.u64 = 0x%I64x;", ip->a.u32, ip->b.u64));
            break;
        case ByteCodeOp::CopyRARBStr:
            bufferC.addString(format("r%u.pointer = __string%u; ", ip->a.u32, ip->c.u32));
            bufferC.addString(format("r%u.u32 = %u;", ip->b.u32, module->strBuffer[ip->c.u32].size()));
            break;
        case ByteCodeOp::CopyRARB:
            bufferC.addString(format("r%u = r%u; ", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ClearRA:
            bufferC.addString(format("r%u.u64 = 0;", ip->a.u32));
            break;
        case ByteCodeOp::DecRA:
            bufferC.addString(format("r%u.u32--;", ip->a.u32));
            break;
        case ByteCodeOp::IncRA:
            bufferC.addString(format("r%u.u32++;", ip->a.u32));
            break;
        case ByteCodeOp::IncRA64:
            bufferC.addString(format("r%u.u64++;", ip->a.u32));
            break;
        case ByteCodeOp::IncRAVB:
            bufferC.addString(format("r%u.u32 += %u;", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::AffectOp8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) = r%u.u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) = r%u.u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer + %u) = r%u.u32;", ip->a.u32, ip->c.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer + %u) = r%u.u64;", ip->a.u32, ip->c.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOp64Null:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer + %u) = 0;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPointer:
            bufferC.addString(format("*(void**)(r%u.pointer) = r%u.pointer;", ip->a.u32, ip->b.u32));
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
        case ByteCodeOp::ShiftRightU64VB:
            bufferC.addString(format("r%u.u64 >>= %u;", ip->a.u32, ip->b.u32));
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
        case ByteCodeOp::AffectOpPlusEqPointer:
            bufferC.addString(format("*(swag_uint8_t**)(r%u.pointer) += r%u.s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpMinusEqPointer:
            bufferC.addString(format("*(swag_uint8_t**)(r%u.pointer) -= r%u.s32;", ip->a.u32, ip->b.u32));
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

        case ByteCodeOp::AffectOpPercentEqS8:
            bufferC.addString(format("*(swag_int8_t*)(r%u.pointer) %%= r%u.s8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqS16:
            bufferC.addString(format("*(swag_int16_t*)(r%u.pointer) %%= r%u.s16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqS32:
            bufferC.addString(format("*(swag_int32_t*)(r%u.pointer) %%= r%u.s32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqS64:
            bufferC.addString(format("*(swag_int64_t*)(r%u.pointer) %%= r%u.s64;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqU8:
            bufferC.addString(format("*(swag_uint8_t*)(r%u.pointer) %%= r%u.u8;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqU16:
            bufferC.addString(format("*(swag_uint16_t*)(r%u.pointer) %%= r%u.u16;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqU32:
            bufferC.addString(format("*(swag_uint32_t*)(r%u.pointer) %%= r%u.u32;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::AffectOpPercentEqU64:
            bufferC.addString(format("*(swag_uint64_t*)(r%u.pointer) %%= r%u.u64;", ip->a.u32, ip->b.u32));
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
        case ByteCodeOp::CompareOpGreaterPointer:
            bufferC.addString(format("r%u.b = r%u.pointer > r%u.pointer;", ip->c.u32, ip->a.u32, ip->b.u32));
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
        case ByteCodeOp::CompareOpLowerPointer:
            bufferC.addString(format("r%u.b = r%u.pointer < r%u.pointer;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CompareOpEqual8:
            bufferC.addString(format("r%u.b = r%u.u8 == r%u.u8;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqual16:
            bufferC.addString(format("r%u.b = r%u.u16 == r%u.u16;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqual32:
            bufferC.addString(format("r%u.b = r%u.u32 == r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqual64:
            bufferC.addString(format("r%u.b = r%u.u64 == r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqualPointer:
            bufferC.addString(format("r%u.b = r%u.pointer == r%u.pointer;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqualBool:
            bufferC.addString(format("r%u.b = r%u.b == r%u.b;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CompareOpEqualString:
            bufferC.addString(format("r%u.b = __strcmp((const char*) r%u.pointer, (const char*) r%u.pointer);", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::IsNullString:
            bufferC.addString(format("r%u.b = r%u.pointer == 0;", ip->b.u32, ip->a.u32));
            break;
        case ByteCodeOp::IsNullU32:
            bufferC.addString(format("r%u.b = r%u.u32 == 0;", ip->b.u32, ip->a.u32));
            break;

        case ByteCodeOp::BinOpAnd:
            bufferC.addString(format("r%u.b = r%u.b && r%u.b;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::BinOpOr:
            bufferC.addString(format("r%u.b = r%u.b || r%u.b;", ip->c.u32, ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::Jump:
            bufferC.addString(format("goto lbl%08u;", ip->a.s32 + i + 1));
            break;
        case ByteCodeOp::JumpNotTrue:
            bufferC.addString(format("if(!r%d.b) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1));
            break;
        case ByteCodeOp::JumpTrue:
            bufferC.addString(format("if(r%d.b) goto lbl%08u;", ip->a.u32, ip->b.s32 + i + 1));
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
            bufferC.addString(format("__assert(r%u.b, \"%s\", %d, 0);", ip->a.u32, normalizePath(module->files[ip->sourceFileIdx]->path).c_str(), ip->startLocation.line + 1));
            break;
        case ByteCodeOp::IntrinsicAlloc:
            bufferC.addString(format("r%u.pointer = (swag_uint8_t*) __alloc(r%u.u32);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::IntrinsicFree:
            bufferC.addString(format("__free(r%u.pointer);", ip->a.u32));
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
            bufferC.addString(format("r%u.u64 &= 0xFFFFFFFF | ((swag_uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::ClearMaskU32U64:
            bufferC.addString(format("r%u.u32 &= 0x%x; ", ip->a.u32, ip->b.u32));
            bufferC.addString(format("r%u.u64 &= 0xFFFFFFFF | ((swag_uint64_t) 0x%x << 32);", ip->a.u32, ip->b.u32));
            break;

        case ByteCodeOp::CastS8S16:
            bufferC.addString(format("r%u.s16 = (swag_int16_t) r%u.s8; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS16S32:
            bufferC.addString(format("r%u.s32 = (swag_int32_t) r%u.s16; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS32S8:
            bufferC.addString(format("r%u.s8 = (swag_int8_t) r%u.s32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS32S16:
            bufferC.addString(format("r%u.s16 = (swag_int16_t) r%u.s32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS32S64:
            bufferC.addString(format("r%u.f64 = (swag_float64_t) r%u.s32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS32F32:
            bufferC.addString(format("r%u.f32 = (swag_float32_t) r%u.s32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS64S32:
            bufferC.addString(format("r%u.s32 = (swag_int32_t) r%u.s64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS64F32:
            bufferC.addString(format("r%u.f32 = (swag_float32_t) r%u.s64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastS64F64:
            bufferC.addString(format("r%u.f64 = (swag_float64_t) r%u.s64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastU32F32:
			bufferC.addString(format("r%u.f32 = (swag_float32_t) r%u.u32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastU64F32:
			bufferC.addString(format("r%u.f32 = (swag_float32_t) r%u.u64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastU64F64:
			bufferC.addString(format("r%u.f64 = (swag_float64_t) r%u.u64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF32S32:
			bufferC.addString(format("r%u.s32 = (swag_int32_t) r%u.f32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF32S64:
			bufferC.addString(format("r%u.s64 = (swag_int64_t) r%u.f32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF32F64:
			bufferC.addString(format("r%u.f64 = (swag_float64_t) r%u.f32; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF64S64:
			bufferC.addString(format("r%u.s64 = (swag_int64_t) r%u.f64; ", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::CastF64F32:
			bufferC.addString(format("r%u.f32 = (swag_float32_t) r%u.f64; ", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::CopyRR0:
            bufferC.addString(format("__memcpy(rr0->pointer, r%u.pointer, %u);", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRRxRCx:
            bufferC.addString(format("*rr%u = r%u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRRxRCxCall:
            bufferC.addString(format("rt%u = r%u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::CopyRCxRRx:
            bufferC.addString(format("r%u = rt%u;", ip->a.u32, ip->b.u32));
            break;
        case ByteCodeOp::RAFromStackParam64:
            bufferC.addString(format("r%u = *rp%u;", ip->a.u32, ip->c.u32));
            break;
        case ByteCodeOp::RARefFromStackParam:
            bufferC.addString(format("r%u.pointer = (swag_int8_t*) &rp%u->pointer;", ip->a.u32, ip->c.u32));
            break;
        case ByteCodeOp::PushRRSaved:
        case ByteCodeOp::PopRRSaved:
            break;
        case ByteCodeOp::PushRAParam:
            bufferC.addString(format("rc%u = r%u;", ip->b.u32, ip->a.u32));
            break;

        case ByteCodeOp::MinusToTrue:
            bufferC.addString(format("r%u.b = r%u.s32 < 0 ? 1 : 0;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::MinusZeroToTrue:
            bufferC.addString(format("r%u.b = r%u.s32 <= 0 ? 1 : 0;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::PlusToTrue:
            bufferC.addString(format("r%u.b = r%u.s32 > 0 ? 1 : 0;", ip->a.u32, ip->a.u32));
            break;
        case ByteCodeOp::PlusZeroToTrue:
            bufferC.addString(format("r%u.b = r%u.s32 >= 0 ? 1 : 0;", ip->a.u32, ip->a.u32));
            break;

        case ByteCodeOp::MakeLambda:
        {
            auto funcBC = (ByteCode*) ip->b.pointer;
            bufferC.addString(format("r%u.pointer = (swag_uint8_t*) &%s;", ip->a.u32, funcBC->callName().c_str()));
            break;
        }

        case ByteCodeOp::MovRASP:
            bufferC.addString(format("__register vaargs%u[] = {", vaargsIdx));
            for (int j = ip->b.u32 - 1; j >= 0; j--)
                bufferC.addString(format("rc%u, ", j));
            bufferC.addString(format(" }; r%u.pointer = (swag_int8_t*) &vaargs%u;", ip->a.u32, vaargsIdx));
            vaargsIdx++;
            break;

        case ByteCodeOp::LambdaCall:
        case ByteCodeOp::LocalCall:
        {
            auto              funcBC     = (ByteCode*) ip->a.pointer;
            TypeInfoFuncAttr* typeFuncBC = (TypeInfoFuncAttr*) ip->c.pointer;

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
                    bufferC.addString("__register*");
                }

                bufferC.addString("); ");

                // Then the call
                bufferC.addString(format("((tfn)r%u.pointer)", ip->a.u32));
            }

            bufferC.addString("(");
            for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
            {
                if (j)
                    bufferC.addString(",");
                bufferC.addString(format("&rt%d", j));
            }

            auto index         = ip->b.u64 - 1;
            auto numCallParams = typeFuncBC->parameters.size();
            assert(!numCallParams || ip->b.u64);

            for (int idxCall = (int) numCallParams - 1; idxCall >= 0; idxCall--)
            {
                auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
                typeParam      = TypeManager::concreteType(typeParam);
                for (int j = 0; j < typeParam->numRegisters(); j++)
                {
                    if ((idxCall != (int) numCallParams - 1) || j || typeFuncBC->numReturnRegisters())
                        bufferC.addString(", ");
                    bufferC.addString(format("&rc%u", index));
                    index -= 1;
                }
            }

            bufferC.addString("); }");
        }
        break;

        case ByteCodeOp::ForeignCall:
        {
            auto nodeFunc = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
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
        TypeInfoFuncAttr* typeFunc = one->typeInfoFunc;
        AstFuncDecl*      node     = nullptr;

        if (one->node)
        {
            node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);
            node->computeFullName();

            // Do we need to generate that function ?
            if (node->attributeFlags & ATTRIBUTE_COMPILER)
                continue;
            if ((node->attributeFlags & ATTRIBUTE_TEST) && !g_CommandLine.unittest)
                continue;
            if (node->attributeFlags & ATTRIBUTE_FOREIGN)
                continue;

            typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
        }

        ok &= emitInternalFunction(one);

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
