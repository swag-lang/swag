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
#include "TypeInfo.h"

bool BackendC::emitFunctions()
{
    for (auto one : module->byteCodeFunc)
    {
        auto node = one->node;

        // Do we need to generate that function ?
        if (node->attributeFlags & ATTRIBUTE_COMPILER)
            continue;
        if ((node->attributeFlags & ATTRIBUTE_TEST) && !g_CommandLine.test)
            continue;

        outputC.addString("void __");
        outputC.addString(node->name.c_str());
        outputC.addString("(");
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
            }

            outputC.addString(";\n");
        }

        // Generate bytecode
        auto ip = bc->out;
        for (uint32_t i = 0; i < bc->numInstructions; i++)
        {
            switch (ip->op)
            {
            case ByteCodeOp::CopyRCxVaStr:
                outputC.addString(format("__r%d.pointer = __string%d;\n", ip->b.u32, ip->a.u32));
                break;
            case ByteCodeOp::IntrinsicPrintString:
                outputC.addString(format("__print((const char*) __r%d.pointer);\n", ip->a.u32));
                break;
            case ByteCodeOp::End:
                break;
            case ByteCodeOp::Ret:
                outputC.addString("return;\n");
                break;
            default:
                outputC.addString("// ");
                outputC.addString(g_ByteCodeOpNames[(int) ip->op]);
                outputC.addString("\n");
                module->error(format("unknown byte code instruction '%s'", g_ByteCodeOpNames[(int) ip->op]));
                break;
            }

            ip++;
        }

        outputC.addString("}\n");
    }

    return true;
}
