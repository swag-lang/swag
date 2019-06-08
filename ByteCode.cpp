#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Log.h"
#include "Ast.h"

#undef BYTECODE_OP
#define BYTECODE_OP(__op) L#__op,

const wchar_t* g_ByteCodeOpNames[] = {
#include "ByteCodeOpList.h"
};

Pool<ByteCode> g_Pool_byteCode;

void ByteCode::print()
{
    static const wchar_t* bcNum = L"%08d ";

    g_Log.lock();

    g_Log.setColor(LogColor::Magenta);
    g_Log.print(Ast::getNakedName(node));
    g_Log.print(" ");
    g_Log.print(node->name.c_str());
    g_Log.eol();

    for (int i = 0; i < (int) numInstructions; i++)
    {
        auto ip = out + i;

        g_Log.setColor(LogColor::Cyan);
        wprintf(bcNum, i);

        g_Log.setColor(LogColor::White);
        wprintf(g_ByteCodeOpNames[(int) ip->op]);
		g_Log.print(" ");

        g_Log.setColor(LogColor::Gray);
        switch (ip->op)
        {
        case ByteCodeOp::CopyVaRCx:
            g_Log.setColor(LogColor::Gray);
            wprintf(L"R0: %d ", ip->a.s32);
            break;
        case ByteCodeOp::IntrinsicAssert:
        case ByteCodeOp::IntrinsicPrintChar:
        case ByteCodeOp::IntrinsicPrintF64:
        case ByteCodeOp::IntrinsicPrintS64:
        case ByteCodeOp::IntrinsicPrintString:
            g_Log.setColor(LogColor::Gray);
            wprintf(L"R0: %d ", ip->a.s32);
            break;
        case ByteCodeOp::Jump:
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->a.s32 + i + 1);
            break;
        case ByteCodeOp::JumpNotTrue:
            g_Log.setColor(LogColor::Gray);
            wprintf(L"R0: %d ", ip->a.s32);
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->b.s32 + i + 1);
            break;
        }

        g_Log.eol();
    }

    g_Log.setDefaultColor();
    g_Log.unlock();
}