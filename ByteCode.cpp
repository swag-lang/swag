#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "SourceFile.h"
#include "ByteCodeRunContext.h"

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

string ByteCode::callName()
{
    auto callname = name.empty() ? node->fullnameForeign : name;
    callname += format("_%lX", (uint64_t) this);
    return callname;
}

TypeInfoFuncAttr* ByteCode::callType()
{
    if (node)
        return CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    return typeInfoFunc;
}

void ByteCode::enterByteCode(ByteCodeRunContext* context)
{
    if (curRC == (int)context->sourceFile->module->buildParameters.target.byteCodeMaxRecurse)
    {
        context->hasError = true;
        context->errorMsg = format("recursive overflow during bytecode execution (max recursion is '--bc-max-recurse:%d')", context->sourceFile->module->buildParameters.target.byteCodeMaxRecurse);
        return;
    }

    curRC++;
    if (curRC >= registersRC.size())
    {
        auto rc = (Register*) malloc(maxReservedRegisterRC * sizeof(Register));
        registersRC.push_back(rc);
    }
}

void ByteCode::leaveByteCode()
{
    curRC--;
}

void ByteCode::print()
{
    static const wchar_t* bcNum = L"%08d ";

    g_Log.lock();

    g_Log.setColor(LogColor::Magenta);
    g_Log.print(sourceFile->path.string());
    g_Log.print(", ");
    if (node)
    {
        g_Log.print(AstNode::getNakedKindName(node));
        g_Log.print(" ");
        g_Log.print(node->name.c_str());
    }
    else
    {
        g_Log.print(" ");
        g_Log.print(name.c_str());
    }

    g_Log.eol();

    int lastLine = -1;
    for (int i = 0; i < (int) numInstructions; i++)
    {
        auto ip = out + i;

        // Print source code
        if (ip->startLocation.line != lastLine)
        {
            if (ip->startLocation.column != ip->endLocation.column)
            {
                lastLine = ip->startLocation.line;
                auto s   = sourceFile->getLine(ip->startLocation.seekStartLine[REPORT_NUM_CODE_LINES - 1]);
                s.erase(0, s.find_first_not_of("\t\n\v\f\r "));
                g_Log.setColor(LogColor::DarkYellow);
                for (int idx = 0; idx < 9; idx++)
                    g_Log.print(" ");
                g_Log.print(s);
                g_Log.print("\n");
            }
        }

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
        case ByteCodeOp::ClearRefFromStack8:
        case ByteCodeOp::ClearRefFromStack16:
        case ByteCodeOp::ClearRefFromStack32:
        case ByteCodeOp::ClearRefFromStack64:
            wprintf(L"VA: { %u }", ip->a.u32);
            break;

        case ByteCodeOp::ClearRefFromStackX:
            wprintf(L"VA: { %u } VB: { %u }", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::IntrinsicAssert:
            if (ip->c.pointer)
            {
                auto    size = strlen((const char*) ip->c.pointer);
                wstring wc(size + 1, L'#');
                mbstowcs_s(nullptr, &wc[0], size + 1, (const char*) ip->c.pointer, size);
                wprintf(L"RA: %u { \"%s\" }", ip->a.u32, wc.c_str());
            }
            else
                wprintf(L"RA: %u ", ip->a.u32);
            break;

        case ByteCodeOp::PushRAParam:
        case ByteCodeOp::PushRRSaved:
        case ByteCodeOp::PopRRSaved:
        case ByteCodeOp::DeRef8:
        case ByteCodeOp::DeRef16:
        case ByteCodeOp::DeRef32:
        case ByteCodeOp::DeRef64:
        case ByteCodeOp::IntrinsicPrintF64:
        case ByteCodeOp::IntrinsicPrintS64:
        case ByteCodeOp::ClearRA:
        case ByteCodeOp::DecRA:
        case ByteCodeOp::IncRA:
        case ByteCodeOp::IncRA64:
        case ByteCodeOp::LambdaCall:
        case ByteCodeOp::CastS8S16:
        case ByteCodeOp::CastS16S32:
        case ByteCodeOp::CastS32S8:
        case ByteCodeOp::CastS32S16:
        case ByteCodeOp::CastS32S64:
        case ByteCodeOp::CastS32F32:
        case ByteCodeOp::CastS64S32:
        case ByteCodeOp::CastS64F32:
        case ByteCodeOp::CastS64F64:
        case ByteCodeOp::CastU32F32:
        case ByteCodeOp::CastU64F32:
        case ByteCodeOp::CastU64F64:
        case ByteCodeOp::CastF32S32:
        case ByteCodeOp::CastF32S64:
        case ByteCodeOp::CastF32F64:
        case ByteCodeOp::CastF64S64:
        case ByteCodeOp::CastF64F32:
        case ByteCodeOp::CastBool8:
        case ByteCodeOp::CastBool16:
        case ByteCodeOp::CastBool32:
        case ByteCodeOp::CastBool64:
        case ByteCodeOp::MovRASPVaargs:
        case ByteCodeOp::IntrinsicGetContext:
        case ByteCodeOp::Clear8:
        case ByteCodeOp::Clear16:
        case ByteCodeOp::Clear32:
        case ByteCodeOp::Clear64:
        case ByteCodeOp::NegBool:
        case ByteCodeOp::NegF32:
        case ByteCodeOp::NegF64:
        case ByteCodeOp::NegS32:
        case ByteCodeOp::NegS64:
            wprintf(L"RA: %u ", ip->a.u32);
            break;

        case ByteCodeOp::AffectOp8:
        case ByteCodeOp::AffectOp16:
        case ByteCodeOp::AffectOpPointer:
        case ByteCodeOp::IsNullString:
        case ByteCodeOp::BoundCheck:
        case ByteCodeOp::BoundCheckString:
        case ByteCodeOp::CopyRARB:
        case ByteCodeOp::CopyRARBAddr:
        case ByteCodeOp::CopyRCxRRx:
        case ByteCodeOp::CopyRCxRRxCall:
        case ByteCodeOp::CopyRRxRCx:
        case ByteCodeOp::CopyRRxRCxCall:
        case ByteCodeOp::IntrinsicPrintString:
            wprintf(L"RA: %u RB: %u ", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::AffectOp32:
        case ByteCodeOp::AffectOp64:
        case ByteCodeOp::CopyRARBStr:
        case ByteCodeOp::DeRefPointer:
        case ByteCodeOp::CopyVC:
        case ByteCodeOp::ClearXVar:
            wprintf(L"RA: %u RB: %u VC: { %u } ", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::RARefFromConstantSeg:
            wprintf(L"RA: %u RB: %u VC: { %u, %u } ", ip->a.u32, ip->b.u32, (uint32_t)(ip->c.u64 >> 32), (uint32_t)(ip->c.u64 & 0xFFFFFFFF));
            break;

        case ByteCodeOp::RARefFromStackParam:
            wprintf(L"RA: %u VB: { %u } VC: { %u } ", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::ClearX:
        case ByteCodeOp::RAFromStack8:
        case ByteCodeOp::RAFromStack16:
        case ByteCodeOp::RAFromStack32:
        case ByteCodeOp::RAFromStack64:
        case ByteCodeOp::RARefFromStack:
        case ByteCodeOp::AffectOp64Null:
        case ByteCodeOp::IncPointerVB:
        case ByteCodeOp::ShiftRightU64VB:
        case ByteCodeOp::IncRAVB:
        case ByteCodeOp::RAAddrFromConstantSeg:
            wprintf(L"RA: %u VB: { %u } ", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::RAFromStackParam64:
        case ByteCodeOp::MovRASP:
            wprintf(L"RA: %u VB: { %u } VC: { %u }", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::RAFromDataSeg8:
        case ByteCodeOp::RAFromDataSeg16:
        case ByteCodeOp::RAFromDataSeg32:
        case ByteCodeOp::RAFromDataSeg64:
        case ByteCodeOp::RARefFromDataSeg:
        case ByteCodeOp::MulRAVB:
            wprintf(L"RA: %u VB: { %u }", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CopyRAVB32:
            wprintf(L"RA: %u VB: { %x }", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CopyRAVB64:
            wprintf(L"RA: %u VB: { %I64x }", ip->a.u32, ip->b.u64);
            break;

        case ByteCodeOp::Jump:
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->a.s32 + i + 1);
            break;
        case ByteCodeOp::JumpZero32:
        case ByteCodeOp::JumpNotZero32:
        case ByteCodeOp::JumpNotTrue:
        case ByteCodeOp::JumpTrue:
            g_Log.setColor(LogColor::Gray);
            wprintf(L"RA: %u ", ip->a.u32);
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->b.s32 + i + 1);
            break;

        case ByteCodeOp::MakeLambda:
        {
            wprintf(L"RA: %u ", ip->a.u32);
            auto bc = (ByteCode*) ip->b.pointer;
            g_Log.print("[");
            g_Log.print(bc->sourceFile->path.string());
            g_Log.print(", ");
            g_Log.print(bc->node->name);
            g_Log.print("]");
            break;
        }

        case ByteCodeOp::LocalCall:
        {
            auto bc = (ByteCode*) ip->a.pointer;
            g_Log.print("[");
            g_Log.print(bc->sourceFile->path.string());
            g_Log.print(", ");
            g_Log.print(bc->node ? bc->node->name : bc->name);
            g_Log.print(format(" (%lX)", (uint64_t) bc));
            g_Log.print("]");
            break;
        }

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