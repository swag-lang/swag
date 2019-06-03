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
    switch (ip->op)
    {
    case ByteCodeOp::PushBool:
        context->push(ip->r0.b);
        break;
    case ByteCodeOp::PushS8:
        context->push(ip->r0.s8);
        break;
    case ByteCodeOp::PushS16:
        context->push(ip->r0.s16);
        break;
    case ByteCodeOp::PushS32:
        context->push(ip->r0.s32);
        break;
    case ByteCodeOp::PushS64:
        context->push(ip->r0.s64);
        break;
    case ByteCodeOp::PushU8:
        context->push(ip->r0.u8);
        break;
    case ByteCodeOp::PushU16:
        context->push(ip->r0.u16);
        break;
    case ByteCodeOp::PushU32:
        context->push(ip->r0.u32);
        break;
    case ByteCodeOp::PushU64:
        context->push(ip->r0.u64);
        break;
    case ByteCodeOp::PushF32:
        context->push((float) ip->r0.f64);
        break;
    case ByteCodeOp::PushF64:
        context->push(ip->r0.f64);
        break;
    case ByteCodeOp::PushString:
        context->push(ip->r0.u32);
        break;

    case ByteCodeOp::BinOpPlusS32:
    {
        auto val1 = context->popS32();
        auto val2 = context->popS32();
        context->push(val2 + val1);
        break;
    }
    case ByteCodeOp::BinOpPlusS64:
    {
        auto val1 = context->popS64();
        auto val2 = context->popS64();
        context->push(val2 + val1);
        break;
    }
    case ByteCodeOp::BinOpPlusU32:
    {
        auto val1 = context->popU32();
        auto val2 = context->popU32();
        context->push(val2 + val1);
        break;
    }
    case ByteCodeOp::BinOpPlusU64:
    {
        auto val1 = context->popU64();
        auto val2 = context->popU64();
        context->push(val2 + val1);
        break;
    }
    case ByteCodeOp::BinOpPlusF32:
    {
        auto val1 = context->popF32();
        auto val2 = context->popF32();
        context->push(val2 + val1);
        break;
    }
    case ByteCodeOp::BinOpPlusF64:
    {
        auto val1 = context->popU64();
        auto val2 = context->popU64();
        context->push(val2 + val1);
        break;
    }

    case ByteCodeOp::BinOpMinusS32:
    {
        auto val1 = context->popS32();
        auto val2 = context->popS32();
        context->push(val2 - val1);
        break;
    }
    case ByteCodeOp::BinOpMinusS64:
    {
        auto val1 = context->popS64();
        auto val2 = context->popS64();
        context->push(val2 - val1);
        break;
    }
    case ByteCodeOp::BinOpMinusU32:
    {
        auto val1 = context->popU32();
        auto val2 = context->popU32();
        context->push(val2 - val1);
        break;
    }
    case ByteCodeOp::BinOpMinusU64:
    {
        auto val1 = context->popU64();
        auto val2 = context->popU64();
        context->push(val2 - val1);
        break;
    }
    case ByteCodeOp::BinOpMinusF32:
    {
        auto val1 = context->popF32();
        auto val2 = context->popF32();
        context->push(val2 - val1);
        break;
    }
    case ByteCodeOp::BinOpMinusF64:
    {
        auto val1 = context->popU64();
        auto val2 = context->popU64();
        context->push(val2 - val1);
        break;
    }

    case ByteCodeOp::BinOpMulS32:
    {
        auto val1 = context->popS32();
        auto val2 = context->popS32();
        context->push(val2 * val1);
        break;
    }
    case ByteCodeOp::BinOpMulS64:
    {
        auto val1 = context->popS64();
        auto val2 = context->popS64();
        context->push(val2 * val1);
        break;
    }
    case ByteCodeOp::BinOpMulU32:
    {
        auto val1 = context->popU32();
        auto val2 = context->popU32();
        context->push(val2 * val1);
        break;
    }
    case ByteCodeOp::BinOpMulU64:
    {
        auto val1 = context->popU64();
        auto val2 = context->popU64();
        context->push(val2 * val1);
        break;
    }
    case ByteCodeOp::BinOpMulF32:
    {
        auto val1 = context->popF32();
        auto val2 = context->popF32();
        context->push(val2 * val1);
        break;
    }
    case ByteCodeOp::BinOpMulF64:
    {
        auto val1 = context->popU64();
        auto val2 = context->popU64();
        context->push(val2 * val1);
        break;
    }

    case ByteCodeOp::BinOpDivF32:
    {
        auto val1 = context->popF32();
        auto val2 = context->popF32();
        if (val1 == 0.0f)
            context->error("division by zero");
        else
            context->push(val2 / val1);
        break;
    }
    case ByteCodeOp::BinOpDivF64:
    {
        auto val1 = context->popU64();
        auto val2 = context->popU64();
        if (val1 == 0.0f)
            context->error("division by zero");
        else
            context->push(val2 / val1);
        break;
    }

    case ByteCodeOp::Ret:
    {
        context->epbc--;
        context->bc = context->stack_bc[context->epbc];
        context->ip = context->stack_ip[context->epbc];
        break;
    }
    case ByteCodeOp::LocalFuncCall:
    {
        context->stack_bc[context->epbc] = context->bc;
        context->stack_ip[context->epbc] = context->ip;
        context->epbc++;
        context->bc = (ByteCode*) ip->r0.pointer;
        context->ip = context->bc->out;
        break;
    }

    case ByteCodeOp::IntrinsicAssert:
    {
        auto val = context->popBool();
        if (!val)
            context->error("intrisic @assert failed");
        break;
    }

    case ByteCodeOp::IntrinsicPrintF64:
    {
        auto val = context->popF64();
        g_Log.lock();
        g_Log.print(to_string(val));
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintS64:
    {
        auto val = context->popS64();
        g_Log.lock();
        g_Log.print(to_string(val));
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintChar:
    {
        auto val = context->popU32();
        g_Log.lock();
        Utf8 msg;
        msg += (char32_t) val;
        g_Log.print(msg);
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintString:
    {
        auto val = context->popU32();
        assert(val < context->bc->strBuffer.size());
        g_Log.lock();
        g_Log.print(context->bc->strBuffer[val]);
        g_Log.unlock();
        break;
    }

	default:
        context->error(format("Unknown byte code '%s'", g_ByteCodeOpNames[(int) ip->op]));
        break;
    }
}

bool ByteCodeRun::run(ByteCodeRunContext* context)
{
    context->ip = context->bc->out;
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
