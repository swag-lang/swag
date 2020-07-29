#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "SourceFile.h"
#include "DiagnosticInfos.h"
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

SourceLocation* ByteCodeInstruction::getLocation(ByteCode* bc)
{
    if (flags & BCI_LOCATION_IS_BC)
    {
        if (locationBC >= bc->numInstructions)
            return nullptr;
        return bc->out[locationBC].location;
    }
    
    return location;
}

Utf8 ByteCode::callName()
{
    Utf8 callName;
    if (name.empty())
        callName = node->scopedName;
    else
        callName = name;
    callName += format("_%lX", (uint64_t) this);
    return callName;
}

TypeInfoFuncAttr* ByteCode::callType()
{
    if (node)
        return CastTypeInfo<TypeInfoFuncAttr>(node->typeInfo, TypeInfoKind::FuncAttr);
    return typeInfoFunc;
}

void ByteCode::enterByteCode(ByteCodeRunContext* context)
{
#ifdef SWAG_HAS_ASSERT
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.push();
        g_diagnosticInfos.last().message    = context->bc->name;
        g_diagnosticInfos.last().sourceFile = context->sourceFile;
        g_diagnosticInfos.last().node       = context->bc->node;
    }
#endif

    auto module = context->sourceFile->module;
    if (curRC == (int) module->buildParameters.buildMode->byteCodeMaxRecurse)
    {
        context->hasError = true;
        context->errorMsg = format("recursive overflow during bytecode execution (max recursion is '--bc-max-recurse:%d')", module->buildParameters.buildMode->byteCodeMaxRecurse);
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
#ifdef SWAG_HAS_ASSERT
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.pop();
    }
#endif

    curRC--;
}

void ByteCode::print()
{
    static const wchar_t* bcNum = L"%08d ";

    g_Log.lock();

    g_Log.setColor(LogColor::Magenta);
    g_Log.print(sourceFile->path);
    g_Log.print(", ");
    if (node)
    {
        g_Log.print(AstNode::getNakedKindName(node).c_str());
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
        auto location = ip->getLocation(this);
        if (location && location->line != lastLine && ip->op != ByteCodeOp::End)
        {
            lastLine = location->line;
            auto s   = sourceFile->getLine(location->seekStartLine[REPORT_NUM_CODE_LINES - 1]);
            s.trimLeft();
            g_Log.setColor(LogColor::DarkYellow);
            for (int idx = 0; idx < 9; idx++)
                g_Log.print(" ");
            g_Log.print(s);
            g_Log.print("\n");
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
        case ByteCodeOp::DebugPushScope:
        case ByteCodeOp::DebugPopScope:
            break;
        case ByteCodeOp::DebugDeclLocalVar:
        {
            g_Log.print(format("%s %s %d", ip->node->name.c_str(), ip->node->typeInfo->name.c_str(), ip->node->resolvedSymbolOverload->storageOffset));
            break;
        }

        case ByteCodeOp::Ret:
        case ByteCodeOp::End:
        case ByteCodeOp::PushBP:
        case ByteCodeOp::PopBP:
        case ByteCodeOp::CopySPtoBP:
            break;

        case ByteCodeOp::IncSP:
        case ByteCodeOp::DecSP:
        case ByteCodeOp::SetZeroStack8:
        case ByteCodeOp::SetZeroStack16:
        case ByteCodeOp::SetZeroStack32:
        case ByteCodeOp::SetZeroStack64:
            wprintf(L"VA: { %u }", ip->a.u32);
            break;

        case ByteCodeOp::SetZeroStackX:
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
        case ByteCodeOp::PushRR:
        case ByteCodeOp::PopRR:
        case ByteCodeOp::DeRef8:
        case ByteCodeOp::DeRef16:
        case ByteCodeOp::DeRef32:
        case ByteCodeOp::DeRef64:
        case ByteCodeOp::IntrinsicPrintF64:
        case ByteCodeOp::IntrinsicPrintS64:
        case ByteCodeOp::ClearRA:
        case ByteCodeOp::DecrementRA32:
        case ByteCodeOp::IncrementRA32:
        case ByteCodeOp::LambdaCall:
        case ByteCodeOp::CastS8S16:
        case ByteCodeOp::CastS16S32:
        case ByteCodeOp::CastS32S64:
        case ByteCodeOp::CastS32F32:
        case ByteCodeOp::CastS64F32:
        case ByteCodeOp::CastS64F64:
        case ByteCodeOp::CastU32F32:
        case ByteCodeOp::CastU64F32:
        case ByteCodeOp::CastU64F64:
        case ByteCodeOp::CastF32S32:
        case ByteCodeOp::CastF32F64:
        case ByteCodeOp::CastF64S64:
        case ByteCodeOp::CastF64F32:
        case ByteCodeOp::CastBool8:
        case ByteCodeOp::CastBool16:
        case ByteCodeOp::CastBool32:
        case ByteCodeOp::CastBool64:
        case ByteCodeOp::CopySPVaargs:
        case ByteCodeOp::IntrinsicGetContext:
        case ByteCodeOp::IntrinsicSetContext:
        case ByteCodeOp::NegBool:
        case ByteCodeOp::InvertS8:
        case ByteCodeOp::InvertS16:
        case ByteCodeOp::InvertS32:
        case ByteCodeOp::InvertS64:
        case ByteCodeOp::NegS32:
        case ByteCodeOp::NegS64:
        case ByteCodeOp::NegF32:
        case ByteCodeOp::NegF64:
            wprintf(L"RA: %u ", ip->a.u32);
            break;

        case ByteCodeOp::IntrinsicAssertCastAny:
        case ByteCodeOp::BoundCheck:
        case ByteCodeOp::BoundCheckString:
        case ByteCodeOp::CopyRBtoRA:
        case ByteCodeOp::CopyRBAddrToRA:
        case ByteCodeOp::CopyRRtoRC:
        case ByteCodeOp::CopyRRtoRCCall:
        case ByteCodeOp::CopyRCtoRR:
        case ByteCodeOp::CopyRCtoRRCall:
        case ByteCodeOp::IntrinsicPrintString:
        case ByteCodeOp::DeRefStringSlice:
            wprintf(L"RA: %u RB: %u ", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::SetAtPointer8:
        case ByteCodeOp::SetAtPointer16:
        case ByteCodeOp::SetAtPointer32:
        case ByteCodeOp::SetAtPointer64:
        case ByteCodeOp::DeRefPointer:
        case ByteCodeOp::MemCpyVC32:
        case ByteCodeOp::SetZeroAtPointerXRB:
            wprintf(L"RA: %u RB: %u VC: { %u } ", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::MakeConstantSegPointerOC:
            wprintf(L"RA: %u RB: %u VC: { %u, %u } ", ip->a.u32, ip->b.u32, (uint32_t)(ip->c.u64 >> 32), (uint32_t)(ip->c.u64 & 0xFFFFFFFF));
            break;

        case ByteCodeOp::MakePointerToStackParam:
            wprintf(L"RA: %u VB: { %u } VC: { %u } ", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::SetZeroAtPointerX:
        case ByteCodeOp::GetFromStack8:
        case ByteCodeOp::GetFromStack16:
        case ByteCodeOp::GetFromStack32:
        case ByteCodeOp::GetFromStack64:
        case ByteCodeOp::MakePointerToStack:
        case ByteCodeOp::SetZeroAtPointer8:
        case ByteCodeOp::SetZeroAtPointer16:
        case ByteCodeOp::SetZeroAtPointer32:
        case ByteCodeOp::SetZeroAtPointer64:
        case ByteCodeOp::IncPointerVB32:
        case ByteCodeOp::BinOpShiftRightU64VB:
        case ByteCodeOp::AddVBtoRA32:
        case ByteCodeOp::MakeConstantSegPointer:
            wprintf(L"RA: %u VB: { %u } ", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::ClearMaskU32:
            wprintf(L"RA: %u VB: { 0x%x } ", ip->a.u32, ip->b.u32);
            break;
        case ByteCodeOp::ClearMaskU64:
            wprintf(L"RA: %u VB: { 0x%llx } ", ip->a.u32, ip->b.u64);
            break;

        case ByteCodeOp::GetFromStackParam64:
        case ByteCodeOp::CopySP:
            wprintf(L"RA: %u VB: { %u } VC: { %u }", ip->a.u32, ip->b.u32, ip->c.u32);
            break;

        case ByteCodeOp::GetFromDataSeg8:
        case ByteCodeOp::GetFromDataSeg16:
        case ByteCodeOp::GetFromDataSeg32:
        case ByteCodeOp::GetFromDataSeg64:
        case ByteCodeOp::GetFromBssSeg8:
        case ByteCodeOp::GetFromBssSeg16:
        case ByteCodeOp::GetFromBssSeg32:
        case ByteCodeOp::GetFromBssSeg64:
        case ByteCodeOp::MakeDataSegPointer:
        case ByteCodeOp::MakeBssSegPointer:
        case ByteCodeOp::Mul64byVB32:
        case ByteCodeOp::Div64byVB32:
            wprintf(L"RA: %u VB: { %u }", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CopyVBtoRA32:
            wprintf(L"RA: %u VB: { %x }", ip->a.u32, ip->b.u32);
            break;

        case ByteCodeOp::CopyVBtoRA64:
            wprintf(L"RA: %u VB: { %I64x }", ip->a.u32, ip->b.u64);
            break;

        case ByteCodeOp::Jump:
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->a.s32 + i + 1);
            break;
        case ByteCodeOp::JumpIfZero32:
        case ByteCodeOp::JumpIfZero64:
        case ByteCodeOp::JumpIfNotZero32:
        case ByteCodeOp::JumpIfNotZero64:
        case ByteCodeOp::JumpIfNotTrue:
        case ByteCodeOp::JumpIfTrue:
            g_Log.setColor(LogColor::Gray);
            wprintf(L"RA: %u ", ip->a.u32);
            g_Log.setColor(LogColor::Cyan);
            wprintf(bcNum, ip->b.s32 + i + 1);
            break;

        case ByteCodeOp::MakeLambdaForeign:
        {
            wprintf(L"RA: %u ", ip->a.u32);
            auto func = (AstFuncDecl*) ip->b.pointer;
            SWAG_ASSERT(func);
            g_Log.print("[");
            g_Log.print(func->sourceFile->path);
            g_Log.print(", ");
            g_Log.print(func->name);
            g_Log.print("]");
            break;
        }

        case ByteCodeOp::MakeLambda:
        {
            wprintf(L"RA: %u ", ip->a.u32);
            auto bc = (ByteCode*) ip->b.pointer;
            SWAG_ASSERT(bc);
            g_Log.print("[");
            g_Log.print(bc->sourceFile->path);
            g_Log.print(", ");
            g_Log.print(bc->node->name);
            g_Log.print("]");
            break;
        }

        case ByteCodeOp::ForeignCall:
        {
            auto funcNode = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
            g_Log.print("[");
            g_Log.print(funcNode->name);
            g_Log.print("]");
            break;
        }

        case ByteCodeOp::LocalCall:
        {
            auto bc = (ByteCode*) ip->a.pointer;
            SWAG_ASSERT(bc);
            g_Log.print(bc->node ? bc->node->name : bc->name);
            if (bc->node && bc->node->typeInfo)
                g_Log.print(bc->node->typeInfo->name);
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

void ByteCode::markLabels()
{
    uint32_t count = numJumps;
    auto     ip    = out;
    for (uint32_t i = 0; i < numInstructions && count; i++, ip++)
    {
        switch (ip->op)
        {
        case ByteCodeOp::Jump:
            ip[ip->a.s32 + 1].flags |= BCI_JUMP_DEST;
            numJumps--;
            break;
        case ByteCodeOp::JumpIfTrue:
        case ByteCodeOp::JumpIfNotTrue:
        case ByteCodeOp::JumpIfNotZero32:
        case ByteCodeOp::JumpIfNotZero64:
        case ByteCodeOp::JumpIfZero32:
        case ByteCodeOp::JumpIfZero64:
            ip[ip->b.s32 + 1].flags |= BCI_JUMP_DEST;
            ip[1].flags |= BCI_JUMP_DEST;
            count--;
            break;
        }
    }
}