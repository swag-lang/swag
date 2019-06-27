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

#undef BYTECODE_OP
#define BYTECODE_OP(__op) (int) strlen(#__op),
int g_ByteCodeOpNamesLen[] = {
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
        while (len++ < ALIGN_RIGHT_OPCODE)
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
            wprintf(L"SA: %d ", ip->a.s32);
            break;

        case ByteCodeOp::PushRCxParam:
        case ByteCodeOp::PushRCxSaved:
		case ByteCodeOp::DeRef8:
		case ByteCodeOp::DeRef16:
		case ByteCodeOp::DeRef32:
		case ByteCodeOp::DeRef64:
		case ByteCodeOp::DeRefPointer:
		case ByteCodeOp::IntrinsicAssert:
		case ByteCodeOp::IntrinsicPrintChar:
		case ByteCodeOp::IntrinsicPrintF32:
		case ByteCodeOp::IntrinsicPrintF64:
		case ByteCodeOp::IntrinsicPrintS32:
		case ByteCodeOp::IntrinsicPrintS64:
		case ByteCodeOp::IntrinsicPrintString:
            wprintf(L"RA: %u ", ip->a.u32);
            break;

		case ByteCodeOp::IncPointer:
		case ByteCodeOp::AffectOp8:
		case ByteCodeOp::AffectOp16:
		case ByteCodeOp::AffectOp32:
		case ByteCodeOp::AffectOp64:
		case ByteCodeOp::AffectOpPointer:
            wprintf(L"RA: %u RB: %u ", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::RCxFromStack8:
        case ByteCodeOp::RCxFromStack16:
        case ByteCodeOp::RCxFromStack32:
        case ByteCodeOp::RCxFromStack64:
        case ByteCodeOp::RCxFromStackParam64:
        case ByteCodeOp::RCxFromDataSeg8:
        case ByteCodeOp::RCxFromDataSeg16:
        case ByteCodeOp::RCxFromDataSeg32:
        case ByteCodeOp::RCxFromDataSeg64:
        case ByteCodeOp::RCxRefFromDataSeg:
        case ByteCodeOp::RCxRefFromStack:
		case ByteCodeOp::MulRCxS32:
            wprintf(L"RA: %u SB: %d ", ip->a.u32, ip->b.s32);
            break;

        case ByteCodeOp::CopyRCxVa32:
            wprintf(L"RB: %u VA: { %x }", ip->b.u32, ip->a.u32);
            break;

        case ByteCodeOp::CopyRCxVaStr:
            wprintf(L"RB: %u RC: %u VA: { %u }", ip->b.u32, ip->c.u32, ip->a.u32);
            break;

        case ByteCodeOp::CopyRCxVa64:
            wprintf(L"RB: %u VA: { %I64x }", ip->b.u32, ip->a.u64);
            break;

        case ByteCodeOp::Jump:
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->a.s32 + i + 1);
            break;
        case ByteCodeOp::JumpNotTrue:
            g_Log.setColor(LogColor::Gray);
            wprintf(L"A: %u ", ip->a.u32);
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->b.s32 + i + 1);
            break;

        case ByteCodeOp::LocalCall:
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
            wprintf(L"RA: %u RB: %u RC: %u", ip->a.u32, ip->b.u32, ip->c.u32);
            break;
        }

        g_Log.eol();
    }

    g_Log.setDefaultColor();
    g_Log.unlock();
}