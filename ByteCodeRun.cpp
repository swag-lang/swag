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
    case ByteCodeOp::CopyV0R1:
    {
        context->registers[ip->r1.u32] = ip->r0;
        break;
    }

    case ByteCodeOp::BinOpPlusS32:
    {
        context->registers[ip->r2.u32].s32 = context->registers[ip->r0.u32].s32 + context->registers[ip->r1.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpPlusS64:
    {
        context->registers[ip->r2.u32].s64 = context->registers[ip->r0.u32].s64 + context->registers[ip->r1.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpPlusU32:
    {
        context->registers[ip->r2.u32].u32 = context->registers[ip->r0.u32].u32 + context->registers[ip->r1.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpPlusU64:
    {
        context->registers[ip->r2.u32].u64 = context->registers[ip->r0.u32].u64 + context->registers[ip->r1.u32].u64;

        break;
    }
    case ByteCodeOp::BinOpPlusF32:
    case ByteCodeOp::BinOpPlusF64:
    {
        context->registers[ip->r2.u32].f64 = context->registers[ip->r0.u32].f64 + context->registers[ip->r1.u32].f64;
        break;
    }

    case ByteCodeOp::BinOpMinusS32:
    {
        context->registers[ip->r2.u32].s32 = context->registers[ip->r0.u32].s32 - context->registers[ip->r1.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpMinusS64:
    {
        context->registers[ip->r2.u32].s64 = context->registers[ip->r0.u32].s64 - context->registers[ip->r1.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpMinusU32:
    {
        context->registers[ip->r2.u32].u32 = context->registers[ip->r0.u32].u32 - context->registers[ip->r1.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpMinusU64:
    {
        context->registers[ip->r2.u32].u64 = context->registers[ip->r0.u32].u64 - context->registers[ip->r1.u32].u64;
        break;
    }
    case ByteCodeOp::BinOpMinusF32:
    case ByteCodeOp::BinOpMinusF64:
    {
        context->registers[ip->r2.u32].f64 = context->registers[ip->r0.u32].f64 - context->registers[ip->r1.u32].f64;
        break;
    }

    case ByteCodeOp::BinOpMulS32:
    {
        context->registers[ip->r2.u32].s32 = context->registers[ip->r0.u32].s32 * context->registers[ip->r1.u32].s32;
        break;
    }
    case ByteCodeOp::BinOpMulS64:
    {
        context->registers[ip->r2.u32].s64 = context->registers[ip->r0.u32].s64 * context->registers[ip->r1.u32].s64;
        break;
    }
    case ByteCodeOp::BinOpMulU32:
    {
        context->registers[ip->r2.u32].u32 = context->registers[ip->r0.u32].u32 * context->registers[ip->r1.u32].u32;
        break;
    }
    case ByteCodeOp::BinOpMulU64:
    {
        context->registers[ip->r2.u32].u64 = context->registers[ip->r0.u32].u64 * context->registers[ip->r1.u32].u64;
        break;
    }
    case ByteCodeOp::BinOpMulF32:
    case ByteCodeOp::BinOpMulF64:
    {
        context->registers[ip->r2.u32].f64 = context->registers[ip->r0.u32].f64 * context->registers[ip->r1.u32].f64;
        break;
    }

    case ByteCodeOp::BinOpDivF32:
    case ByteCodeOp::BinOpDivF64:
    {
        auto val1 = context->registers[ip->r0.u32].f64;
        auto val2 = context->registers[ip->r1.u32].f64;
        if (val2 == 0.0f)
            context->error("division by zero");
        else
            context->registers[ip->r2.u32].f64 = val1 / val2;
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
        if (!context->registers[ip->r0.u32].b)
            context->error("intrisic @assert failed");
        break;
    }

    case ByteCodeOp::IntrinsicPrintF64:
    {
        g_Log.lock();
        g_Log.print(to_string(context->registers[ip->r0.u32].f64));
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintS64:
    {
        g_Log.lock();
        g_Log.print(to_string(context->registers[ip->r0.u32].s64));
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintChar:
    {
        g_Log.lock();
        Utf8 msg;
        msg += (char32_t) context->registers[ip->r0.u32].u32;
        g_Log.print(msg);
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintString:
    {
        auto val = context->registers[ip->r0.u32].u32;
        assert(val < context->bc->strBuffer.size());
        g_Log.lock();
        g_Log.print(context->bc->strBuffer[val]);
        g_Log.unlock();
        break;
    }

    default:
        context->error(format("unknown byte code instruction '%s'", g_ByteCodeOpNames[(int) ip->op]));
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
