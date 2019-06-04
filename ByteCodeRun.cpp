#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "ByteCode.h"
#include "Log.h"
#include "Module.h"

inline void ByteCodeRun::runNode(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto registersRC = context->registersRC;
    auto registersRR = context->registersRR;

    switch (ip->op)
    {
    case ByteCodeOp::CopyVaRCx:
    {
        registersRC[ip->b.u32] = ip->a;
        break;
    }
    case ByteCodeOp::CopyRRxRCx:
    {
        registersRR[ip->a.u32] = registersRC[ip->b.u32];
        break;
    }
    case ByteCodeOp::CopyRCxRRx:
    {
        registersRC[ip->a.u32] = registersRR[ip->b.u32];
        break;
    }
    case ByteCodeOp::PushBP:
    {
        context->push(context->bp);
        break;
    }
    case ByteCodeOp::PopBP:
    {
        context->bp = context->pop<uint8_t*>();
        break;
    }

    case ByteCodeOp::BinOpPlusS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 + registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpPlusS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 + registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpPlusU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 + registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpPlusU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 + registersRC[ip->b.u32].u64;

        break;
    }
    case ByteCodeOp::BinOpPlusF32:
    {
        registersRC[ip->c.u32].f32 = registersRC[ip->a.u32].f32 + registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::BinOpPlusF64:
    {
        registersRC[ip->c.u32].f64 = registersRC[ip->a.u32].f64 + registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::BinOpMinusS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 - registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpMinusS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 - registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpMinusU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 - registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpMinusU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 - registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::BinOpMinusF32:
    {
        registersRC[ip->c.u32].f32 = registersRC[ip->a.u32].f32 - registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::BinOpMinusF64:
    {
        registersRC[ip->c.u32].f64 = registersRC[ip->a.u32].f64 - registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::BinOpMulS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 * registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpMulS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 * registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpMulU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 * registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpMulU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 * registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::BinOpMulF32:
    {
        registersRC[ip->c.u32].f32 = registersRC[ip->a.u32].f32 * registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::BinOpMulF64:
    {
        registersRC[ip->c.u32].f64 = registersRC[ip->a.u32].f64 * registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::BinOpDivF32:
    {
        auto val1 = registersRC[ip->a.u32].f32;
        auto val2 = registersRC[ip->b.u32].f32;
        if (val2 == 0.0f)
            context->error("division by zero");
        else
            registersRC[ip->c.u32].f32 = val1 / val2;
        break;
    }

    case ByteCodeOp::BinOpDivF64:
    {
        auto val1 = registersRC[ip->a.u32].f64;
        auto val2 = registersRC[ip->b.u32].f64;
        if (val2 == 0.0)
            context->error("division by zero");
        else
            registersRC[ip->c.u32].f64 = val1 / val2;
        break;
    }

    case ByteCodeOp::Ret:
    {
        context->ip = context->pop<ByteCodeInstruction*>();
        context->bc = context->pop<ByteCode*>();
        break;
    }
    case ByteCodeOp::LocalFuncCall:
    {
        context->push(context->bc);
        context->push(context->ip);
        context->bc = (ByteCode*) ip->a.pointer;
        context->ip = context->bc->out;
        break;
    }

    case ByteCodeOp::IntrinsicAssert:
    {
        if (!registersRC[ip->a.u32].b)
            context->error("intrinsic @assert failed");
        break;
    }

    case ByteCodeOp::IntrinsicPrintF64:
    {
        g_Log.lock();
        g_Log.print(to_string(registersRC[ip->a.u32].f64));
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintS64:
    {
        g_Log.lock();
        g_Log.print(to_string(registersRC[ip->a.u32].s64));
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintChar:
    {
        Utf8 msg;
        msg += (char32_t) registersRC[ip->a.u32].u32;
        g_Log.lock();
        g_Log.print(msg);
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintString:
    {
        auto val = registersRC[ip->a.u32].u32;
        assert(val < context->bc->strBuffer.size());
        g_Log.lock();
        g_Log.print(context->bc->strBuffer[val]);
        g_Log.unlock();
        break;
    }

    case ByteCodeOp::CompareOpEqualBool:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].b == registersRC[ip->b.u32].b;
        break;
    }
    case ByteCodeOp::CompareOpEqual32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u32 == registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::CompareOpEqual64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u64 == registersRC[ip->b.u32].u64;
        break;
    }

    case ByteCodeOp::CompareOpLowerS32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].s32 < registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::CompareOpLowerS64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].s64 < registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::CompareOpLowerU32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u32 < registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::CompareOpLowerU64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u64 < registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::CompareOpLowerF32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].f32 < registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::CompareOpLowerF64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].f64 < registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::CompareOpGreaterS32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].s32 > registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::CompareOpGreaterS64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].s64 > registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::CompareOpGreaterU32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u32 > registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::CompareOpGreaterU64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u64 > registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::CompareOpGreaterF32:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].f32 > registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::CompareOpGreaterF64:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].f64 > registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::NegBool:
    {
        registersRC[ip->a.u32].b = !registersRC[ip->a.u32].b;
        break;
    }
    case ByteCodeOp::NegS32:
    {
        registersRC[ip->a.u32].s32 = -registersRC[ip->a.u32].s32;
        break;
    }
    case ByteCodeOp::NegS64:
    {
        registersRC[ip->a.u32].s32 = -registersRC[ip->a.u32].s32;
        break;
    }
    case ByteCodeOp::NegF32:
    {
        registersRC[ip->a.u32].f32 = -registersRC[ip->a.u32].f32;
        break;
    }
    case ByteCodeOp::NegF64:
    {
        registersRC[ip->a.u32].f64 = -registersRC[ip->a.u32].f64;
        break;
    }

    case ByteCodeOp::BinOpAnd:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].b && registersRC[ip->b.u32].b;
        break;
    }
    case ByteCodeOp::BinOpOr:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].b || registersRC[ip->b.u32].b;
        break;
    }

    default:
        context->error(format("unknown byte code instruction '%s'", g_ByteCodeOpNames[(int) ip->op]));
        break;
    }
}

bool ByteCodeRun::run(ByteCodeRunContext* context)
{
    while (true)
    {
        // Get instruction
        auto ip = context->ip++;
        if (ip->op == ByteCodeOp::End)
            break;

        runNode(context, ip);

        // Error ?
        if (context->hasError)
        {
            assert(ip->sourceFileIdx < context->sourceFile->module->files.size());
            auto sourceFile = context->sourceFile->module->files[ip->sourceFileIdx];
            return context->sourceFile->report({sourceFile, ip->startLocation, ip->endLocation, context->errorMsg});
        }
    }

    return true;
}

bool ByteCodeRun::internalError(ByteCodeRunContext* context)
{
    context->sourceFile->report({context->sourceFile, context->node->token, "internal compiler error during bytecode execution"});
    return false;
}
