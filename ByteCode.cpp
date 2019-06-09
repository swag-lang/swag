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
        case ByteCodeOp::MovSPBP:
            break;
        case ByteCodeOp::IncSP:
        case ByteCodeOp::DecSP:
            wprintf(L"SPA: %d ", ip->a.s32);
            break;
        case ByteCodeOp::RCxFromStack8:
        case ByteCodeOp::RCxFromStack16:
        case ByteCodeOp::RCxFromStack32:
        case ByteCodeOp::RCxFromStack64:
        case ByteCodeOp::RCxFromDataSeg8:
        case ByteCodeOp::RCxFromDataSeg16:
        case ByteCodeOp::RCxFromDataSeg32:
        case ByteCodeOp::RCxFromDataSeg64:
        case ByteCodeOp::RCxRefFromDataSeg:
        case ByteCodeOp::RCxRefFromStack:
            wprintf(L"RCA: %u SPB: %d ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::AffectOp8:
        case ByteCodeOp::AffectOp16:
        case ByteCodeOp::AffectOp32:
        case ByteCodeOp::AffectOp64:
        case ByteCodeOp::AffectOpPlusEqS8:
        case ByteCodeOp::AffectOpPlusEqS16:
        case ByteCodeOp::AffectOpPlusEqS32:
        case ByteCodeOp::AffectOpPlusEqS64:
        case ByteCodeOp::AffectOpPlusEqU8:
        case ByteCodeOp::AffectOpPlusEqU16:
        case ByteCodeOp::AffectOpPlusEqU32:
        case ByteCodeOp::AffectOpPlusEqU64:
        case ByteCodeOp::AffectOpPlusEqF32:
        case ByteCodeOp::AffectOpPlusEqF64:
        case ByteCodeOp::AffectOpMinusEqS8:
        case ByteCodeOp::AffectOpMinusEqS16:
        case ByteCodeOp::AffectOpMinusEqS32:
        case ByteCodeOp::AffectOpMinusEqS64:
        case ByteCodeOp::AffectOpMinusEqU8:
        case ByteCodeOp::AffectOpMinusEqU16:
        case ByteCodeOp::AffectOpMinusEqU32:
        case ByteCodeOp::AffectOpMinusEqU64:
        case ByteCodeOp::AffectOpMinusEqF32:
        case ByteCodeOp::AffectOpMinusEqF64:
            wprintf(L"RCA: %u RCB: %u ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::PushRCx:
        case ByteCodeOp::IntrinsicAssert:
        case ByteCodeOp::IntrinsicPrintChar:
        case ByteCodeOp::IntrinsicPrintF32:
        case ByteCodeOp::IntrinsicPrintF64:
        case ByteCodeOp::IntrinsicPrintS32:
        case ByteCodeOp::IntrinsicPrintS64:
        case ByteCodeOp::IntrinsicPrintString:
            wprintf(L"RCA: %u ", ip->a.u32);
            break;
        case ByteCodeOp::CopyRCxVa:
            wprintf(L"RCB: %u VA: { %I64d - %Lf }", ip->b.u32, ip->a.s64, ip->a.f64);
            break;
        case ByteCodeOp::CopyRRxRCx:
            wprintf(L"RRA: %u RCB: %u ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::CopyRCxRRx:
            wprintf(L"RCA: %u RRB: %u ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::Jump:
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->a.s32 + i + 1);
            break;
        case ByteCodeOp::JumpNotTrue:
            g_Log.setColor(LogColor::Gray);
            wprintf(L"RCA: %u ", ip->a.u32);
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
            wprintf(L"RCA: %u RCB: %u RCC: %u", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        }

        g_Log.eol();
    }

    g_Log.setDefaultColor();
    g_Log.unlock();
}