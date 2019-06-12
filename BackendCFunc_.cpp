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

bool BackendC::emitFunctions()
{
    bool ok = true;
    for (auto one : module->byteCodeFunc)
    {
        auto node = CastAst<AstFuncDecl>(one->node, AstNodeKind::FuncDecl);

        // Do we need to generate that function ?
        if (node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        if ((node->attributeFlags & ATTRIBUTE_TEST) && !g_CommandLine.test)
            continue;

        outputC.addString("void __");
        outputC.addString(node->name.c_str());
        outputC.addString("(");

        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);

        // Result registers
		int cptParams = 0;
		if (typeFunc->returnType != g_TypeMgr.typeInfoVoid)
		{
			outputC.addString("__register& __rr0");
			cptParams++;
		}

        // Parameters
        for (auto param : typeFunc->parameters)
        {
			if (cptParams)
				outputC.addString(", ");
			outputC.addString(format("const __register& __rp%u", param->index));
			cptParams++;
        }

        outputC.addString(") {\n");

        auto bc = node->bc;

        // Generate one local variable per used register
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
                outputC.addString(format("__r%d", r));
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
                outputC.addString(format("__rp%d", index));
                index = (index + 1) % 10;
                if (index == 0)
                    outputC.addString(";\n");
            }

            outputC.addString(";\n");
        }

        // Local stack
        if (node->stackSize)
        {
            outputC.addString(format("uint8_t __stack[%u];\n", node->stackSize));
        }

        // For function call results
        if (typeFunc->returnType == g_TypeMgr.typeInfoVoid)
            outputC.addString("__register __rr0;\n");

        // Generate bytecode
        auto ip = bc->out;
        for (uint32_t i = 0; i < bc->numInstructions; i++, ip++)
        {
            outputC.addString(format("__lbl%08u:; ", i));
            switch (ip->op)
            {
            case ByteCodeOp::End:
            case ByteCodeOp::DecSP:
            case ByteCodeOp::IncSP:
            case ByteCodeOp::MovSPBP:
                break;
            case ByteCodeOp::RCxRefFromStack:
                outputC.addString(format("__r%u.pointer = __stack + %u;", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::RCxFromStack8:
                outputC.addString(format("__r%u.u8 = *(uint8_t*) (__stack + %d);", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::RCxFromStack16:
                outputC.addString(format("__r%u.u16 = *(uint16_t*) (__stack + %d);", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::RCxFromStack32:
                outputC.addString(format("__r%u.u32 = *(uint32_t*) (__stack + %d);", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::RCxFromStack64:
                outputC.addString(format("__r%u.u64 = *(uint64_t*) (__stack + %d);", ip->a.u32, ip->b.s32));
                break;
            case ByteCodeOp::AffectOp8:
                outputC.addString(format("*(uint16_t*)(__r%u.pointer) = __r%u.u8;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOp16:
                outputC.addString(format("*(uint16_t*)(__r%u.pointer) = __r%u.u16;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOp32:
                outputC.addString(format("*(uint32_t*)(__r%u.pointer) = __r%u.u32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOp64:
                outputC.addString(format("*(uint64_t*)(__r%u.pointer) = __r%u.u64;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CopyRCxVa32:
                outputC.addString(format("__r%u.u32 = 0x%x;", ip->b.u32, ip->a.u32));
                break;
            case ByteCodeOp::CopyRCxVa64:
                outputC.addString(format("__r%u.u64 = 0x%I64x;", ip->b.u32, ip->a.u64));
                break;
            case ByteCodeOp::CopyRCxVaStr:
                outputC.addString(format("__r%u.pointer = __string%u;", ip->b.u32, ip->a.u32));
                break;
            case ByteCodeOp::BinOpPlusS32:
                outputC.addString(format("__r%u.s32 = __r%u.s32 + __r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::BinOpMinusS32:
                outputC.addString(format("__r%u.s32 = __r%u.s32 - __r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::AffectOpMinusEqS32:
                outputC.addString(format("*(int32_t*)(__r%u.pointer) -= __r%u.s32;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpEqual32:
                outputC.addString(format("__r%u.b = __r%u.u32 == __r%u.u32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpEqual64:
                outputC.addString(format("__r%u.b = __r%u.u64 == __r%u.u64;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpEqualBool:
                outputC.addString(format("__r%u.b = __r%u.b == __r%u.b;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CompareOpGreaterS32:
                outputC.addString(format("__r%u.b = __r%u.s32 > __r%u.s32;", ip->c.u32, ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::Jump:
                outputC.addString(format("goto __lbl%08u;", ip->a.s32 + i + 1));
                break;
            case ByteCodeOp::JumpNotTrue:
                outputC.addString(format("if(!__r%d.b) goto __lbl%08u;", ip->a.u32, ip->b.s32 + i + 1));
                break;
            case ByteCodeOp::Ret:
                outputC.addString("return;");
                break;
            case ByteCodeOp::IntrinsicPrintS32:
                outputC.addString(format("__print(to_string(__r%u.s32).c_str());", ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicPrintS64:
                outputC.addString(format("__print(to_string(__r%u.s64).c_str());", ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicPrintF32:
                outputC.addString(format("__print(to_string(__r%u.f32).c_str());", ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicPrintF64:
                outputC.addString(format("__print(to_string(__r%u.f64).c_str());", ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicPrintString:
                outputC.addString(format("__print((const char*) __r%u.pointer);", ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicAssert:
                outputC.addString(format("__assert(__r%u.b, R\"(%s)\", %d);", ip->a.u32, module->files[ip->sourceFileIdx]->path.string().c_str(), ip->startLocation.line + 1));
                break;
            case ByteCodeOp::NegBool:
                outputC.addString(format("__r%u.b = !__r%u.b;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::NegF32:
                outputC.addString(format("__r%u.f32 = -__r%u.f32;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::NegF64:
                outputC.addString(format("__r%u.f64 = -__r%u.f64;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::NegS32:
                outputC.addString(format("__r%u.s32 = -__r%u.s32;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::NegS64:
                outputC.addString(format("__r%u.s64 = -__r%u.s64;", ip->a.u32, ip->a.u32));
                break;
            case ByteCodeOp::CopyRRxRCx:
                outputC.addString(format("__rr%u = __r%u;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::CopyRCxRRx:
                outputC.addString(format("__r%u = __rr%u;", ip->a.u32, ip->b.u32));
                break;
            case ByteCodeOp::RCxFromStackParam64:
                outputC.addString(format("__r%u = __rp%u;", ip->a.u32, ip->c.u32));
                break;
			case ByteCodeOp::PushRCxParam:
				outputC.addString(format("__rp%u = __r%u;", ip->b.u32, ip->a.u32));
				break;
            case ByteCodeOp::LocalFuncCall:
            {
                auto funcBC     = (ByteCode*) ip->a.pointer;
                auto typeFuncBC = CastTypeInfo<TypeInfoFuncAttr>(funcBC->node->typeInfo, TypeInfoKind::FuncAttr);
                outputC.addString(format("__%s(", funcBC->node->name.c_str()));

				int cptCall = 0;
				if (typeFuncBC->returnType != g_TypeMgr.typeInfoVoid)
				{
					outputC.addString("__rr0");
					cptCall++;
				}

				for (int idxCall = 0; idxCall < (int)typeFuncBC->parameters.size(); idxCall++)
				{
					if(cptCall)
						outputC.addString(", ");
					outputC.addString(format("__rp%u", idxCall));
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

            outputC.addString(" // ");
            outputC.addString(g_ByteCodeOpNames[(int) ip->op]);
            outputC.addString("\n");
        }

        outputC.addString("}\n");
    }

    return ok;
}
