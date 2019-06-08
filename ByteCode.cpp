#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Log.h"
#include "Ast.h"
#include "SourceFile.h"

#undef BYTECODE_OP
#define BYTECODE_OP(__op) #__op,

const char* g_ByteCodeOpNames[] = {
#include "ByteCodeOpList.h"
};

Pool<ByteCode> g_Pool_byteCode;

void ByteCode::print()
{
    static const wchar_t* bcNum = L"%08d ";

    g_Log.lock();

    g_Log.setColor(LogColor::Magenta);
    g_Log.print(sourceFile->path.string());
    g_Log.print(", ");
    g_Log.print(Ast::getNakedName(node));
    g_Log.print(" ");
    g_Log.print(node->name.c_str());
    g_Log.eol();

    for (int i = 0; i < (int) numInstructions; i++)
    {
        auto ip = out + i;

        // Instruction rank
        g_Log.setColor(LogColor::Cyan);
        wprintf(bcNum, i);

        // Instruction
        g_Log.setColor(LogColor::White);
        int len = (int) strlen(g_ByteCodeOpNames[(int) ip->op]);
        while (len++ != 20)
            g_Log.print(" ");
        g_Log.print(g_ByteCodeOpNames[(int) ip->op]);
        g_Log.print(" ");

        // Parameters
        g_Log.setColor(LogColor::Gray);
        switch (ip->op)
        {
        case ByteCodeOp::Ret:
        case ByteCodeOp::End:
        case ByteCodeOp::PushBP:
        case ByteCodeOp::PopBP:
            break;
        case ByteCodeOp::IncSP:
            wprintf(L"SP: %d ", ip->a.s32);
            break;
		case ByteCodeOp::RCxFromStack:
            wprintf(L"RC: %u SP: %d ", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::PushRCx:
        case ByteCodeOp::IntrinsicAssert:
        case ByteCodeOp::IntrinsicPrintChar:
		case ByteCodeOp::IntrinsicPrintF32:
        case ByteCodeOp::IntrinsicPrintF64:
		case ByteCodeOp::IntrinsicPrintS32:
        case ByteCodeOp::IntrinsicPrintS64:
        case ByteCodeOp::IntrinsicPrintString:
            wprintf(L"RC: %u ", ip->a.u32);
            break;
        case ByteCodeOp::CopyRCxVa:
            wprintf(L"RC: %u VA: { %I64d - %Lf }", ip->b.u32, ip->a.s64, ip->a.f64);
            break;
        case ByteCodeOp::CopyRRxRCx:
            wprintf(L"RR: %u RC: %u ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRCxRRx:
            wprintf(L"RC: %u RR: %u ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::Jump:
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->a.s32 + i + 1);
            break;
        case ByteCodeOp::JumpNotTrue:
            g_Log.setColor(LogColor::Gray);
            wprintf(L"RC: %u ", ip->a.u32);
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->b.s32 + i + 1);
            break;
        case ByteCodeOp::LocalFuncCall:
        {
            auto bc = (ByteCode*) ip->a.pointer;
            g_Log.print("[");
            g_Log.print(bc->sourceFile->path.string());
            g_Log.print(", ");
            g_Log.print(bc->node->name);
            g_Log.print("]");
        }
        break;
        default:
            g_Log.setColor(LogColor::Gray);
            wprintf(L"RC: %u RC: %u RC: %u", ip->c.u32, ip->b.u32, ip->a.u32);
            break;
        }

        g_Log.eol();
    }

    g_Log.setDefaultColor();
    g_Log.unlock();
}