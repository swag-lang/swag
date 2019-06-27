#include "pch.h"
#include "ByteCodeRun.h"
#include "ByteCodeRunContext.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "ByteCode.h"
#include "Log.h"
#include "Module.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "ByteCodeModuleManager.h"
#include "ffi.h"
#include "SymTable.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "ModuleCompileJob.h"

ByteCodeRun g_Run;

void* ByteCodeRun::ffiGetFuncAddress(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto nodeFunc = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);

    // Load module if specified
    ComputedValue moduleName;
    bool          hasModuleName = nodeFunc->resolvedSymbolOverload->attributes.getValue("swag.foreign.module", moduleName);
    if (hasModuleName)
        g_ModuleMgr.loadModule(context, moduleName.text);

    auto funcName = nodeFunc->resolvedSymbolName->fullName;
    replaceAll(funcName, '.', '_');

    auto fn = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : "", funcName);
    if (!fn)
    {
        if (!hasModuleName || g_ModuleMgr.isModuleLoaded(moduleName.text))
        {
            context->error(format("can't resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }

        // Compile the generated files
        auto externalModule = context->sourceFile->module->workspace->getModuleByName(moduleName.text);
        if (!externalModule)
        {
            context->error(format("can't resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }

        // Need to compile the dll version of the module in order to be able to call a function
        // from the compiler
        if (externalModule->backendParameters.type == BackendType::Dll)
        {
            context->error(format("can't resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }

        auto compileJob                    = g_Pool_moduleCompileJob.alloc();
        compileJob->module                 = externalModule;
        compileJob->backendParameters      = externalModule->backendParameters;
        compileJob->backendParameters.type = BackendType::Dll;
        compileJob->mutexDone              = &mutexDone;
        compileJob->condVar                = &condVar;
        g_ThreadMgr.addJob(compileJob);

        // Sync wait for the dll to be generated
        std::unique_lock<std::mutex> lk(mutexDone);
        condVar.wait(lk);

        // Last try
        g_ModuleMgr.loadModule(context, moduleName.text);
        fn = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : "", funcName);
        if (!externalModule)
        {
            context->error(format("can't resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }
    }

    return fn;
}

void ByteCodeRun::ffiCall(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto fn = ffiGetFuncAddress(context, ip);
    if (!fn)
        return;

    ffi_cif   cif;
    ffi_type* args[10];
    void*     values[10];

    args[0] = &ffi_type_void;
    //values[0] = &f;

    // Initialize the cif
    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 0, &ffi_type_void, args);
    ffi_call(&cif, FFI_FN(fn), nullptr, values);
}

inline bool ByteCodeRun::runNode(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto registersRC = context->registersRC;
    auto registersRR = context->registersRR;

    switch (ip->op)
    {
    case ByteCodeOp::JumpNotTrue:
    {
        if (!registersRC[ip->a.u32].b)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::Jump:
    {
        context->ip += ip->a.s32;
        break;
    }
    case ByteCodeOp::Ret:
    {
        if (context->sp == context->stack + context->stackSize)
            return false;
        context->ip = context->pop<ByteCodeInstruction*>();
        context->bc = context->pop<ByteCode*>();
        context->bp = context->pop<uint8_t*>();
        break;
    }
    case ByteCodeOp::LocalCall:
    {
        context->push(context->bp);
        context->push(context->bc);
        context->push(context->ip);
        context->bc = (ByteCode*) ip->a.pointer;
        context->ip = context->bc->out;
        context->bp = context->sp;
        break;
    }
    case ByteCodeOp::ForeignCall:
    {
        ffiCall(context, ip);
        break;
    }

    case ByteCodeOp::IncPointer:
    {
        registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::DecPointer:
    {
        registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::DeRef8:
    {
        registersRC[ip->a.u32].u8 = *(uint8_t*) registersRC[ip->a.u32].pointer;
        break;
    }
    case ByteCodeOp::DeRef16:
    {
        registersRC[ip->a.u32].u16 = *(uint16_t*) registersRC[ip->a.u32].pointer;
        break;
    }
    case ByteCodeOp::DeRef32:
    {
        registersRC[ip->a.u32].u32 = *(uint32_t*) registersRC[ip->a.u32].pointer;
        break;
    }
    case ByteCodeOp::DeRef64:
    {
        registersRC[ip->a.u32].u64 = *(uint64_t*) registersRC[ip->a.u32].pointer;
        break;
    }
    case ByteCodeOp::DeRefPointer:
    {
        registersRC[ip->a.u32].pointer = *(uint8_t**) registersRC[ip->a.u32].pointer;
        break;
    }
    case ByteCodeOp::BoundCheck:
    {
        int maxOffset = registersRC[ip->a.u32].u32;
        int curOffset = registersRC[ip->b.u32].u32;
        if (curOffset > maxOffset)
            context->error(format("index out of range (index is '%d', maximum index is '%d')", curOffset, maxOffset));
        break;
    }

    case ByteCodeOp::PushRCxSaved:
    case ByteCodeOp::PushRCxParam:
    {
        context->push(registersRC[ip->a.u32].u64);
        break;
    }
    case ByteCodeOp::PopRCxSaved:
    {
        registersRC[ip->a.u32].u64 = context->pop<uint64_t>();
        break;
    }
    case ByteCodeOp::IncSP:
    {
        context->incSP(ip->a.u32);
        break;
    }
    case ByteCodeOp::DecSP:
    {
        context->decSP(ip->a.u32);
        break;
    }
    case ByteCodeOp::CopyRCxVaStr:
    {
        auto module = context->sourceFile->module;
        assert(ip->a.u32 < module->strBuffer.size());
        const auto& str                = module->strBuffer[ip->a.u32];
        registersRC[ip->b.u32].pointer = (uint8_t*) str.c_str();
        registersRC[ip->c.u32].u32     = (uint32_t) str.length();
        break;
    }
    case ByteCodeOp::CopyRCxVa32:
    {
        registersRC[ip->b.u32].u32 = ip->a.u32;
        break;
    }
    case ByteCodeOp::CopyRCxVa64:
    {
        registersRC[ip->b.u32].u64 = ip->a.u64;
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
    case ByteCodeOp::MovSPBP:
    {
        context->bp = context->sp;
        break;
    }
    case ByteCodeOp::RCxFromStack8:
    {
        auto offset               = ip->b.s32;
        registersRC[ip->a.u32].u8 = *(uint8_t*) (context->bp + offset);
        break;
    }
    case ByteCodeOp::RCxFromStack16:
    {
        auto offset                = ip->b.s32;
        registersRC[ip->a.u32].u16 = *(uint16_t*) (context->bp + offset);
        break;
    }
    case ByteCodeOp::RCxFromStack32:
    {
        auto offset                = ip->b.s32;
        registersRC[ip->a.u32].u32 = *(uint32_t*) (context->bp + offset);
        break;
    }
    case ByteCodeOp::RCxFromStack64:
    case ByteCodeOp::RCxFromStackParam64:
    {
        auto offset                = ip->b.s32;
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + offset);
        break;
    }
    case ByteCodeOp::RCxRefFromStack:
    {
        auto offset                    = ip->b.s32;
        registersRC[ip->a.u32].pointer = context->bp + offset;
        break;
    }

    case ByteCodeOp::RCxFromDataSeg8:
    {
        auto module               = context->sourceFile->module;
        auto offset               = ip->b.s32;
        registersRC[ip->a.u32].u8 = *(uint8_t*) (&module->dataSegment[offset]);
        break;
    }
    case ByteCodeOp::RCxFromDataSeg16:
    {
        auto module                = context->sourceFile->module;
        auto offset                = ip->b.s32;
        registersRC[ip->a.u32].u16 = *(uint16_t*) (&module->dataSegment[offset]);
        break;
    }
    case ByteCodeOp::RCxFromDataSeg32:
    {
        auto module                = context->sourceFile->module;
        auto offset                = ip->b.s32;
        registersRC[ip->a.u32].u32 = *(uint32_t*) (&module->dataSegment[offset]);
        break;
    }
    case ByteCodeOp::RCxFromDataSeg64:
    {
        auto module                = context->sourceFile->module;
        auto offset                = ip->b.s32;
        registersRC[ip->a.u32].u64 = *(uint64_t*) (&module->dataSegment[offset]);
        break;
    }
    case ByteCodeOp::RCxRefFromDataSeg:
    {
        auto module                    = context->sourceFile->module;
        auto offset                    = ip->b.s32;
        registersRC[ip->a.u32].pointer = &module->dataSegment[offset];
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

    case ByteCodeOp::MulRCxS32:
    {
        registersRC[ip->a.u32].s32 *= ip->b.s32;
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

    case ByteCodeOp::IntrinsicAssert:
    {
        if (!registersRC[ip->a.u32].b)
            context->error("intrinsic @assert failed");
        break;
    }

    case ByteCodeOp::IntrinsicPrintF32:
    {
        g_Log.lock();
        g_Log.print(to_string(registersRC[ip->a.u32].f32));
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintF64:
    {
        g_Log.lock();
        g_Log.print(to_string(registersRC[ip->a.u32].f64));
        g_Log.unlock();
        break;
    }
    case ByteCodeOp::IntrinsicPrintS32:
    {
        g_Log.lock();
        g_Log.print(to_string(registersRC[ip->a.u32].s32));
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
        g_Log.lock();
        g_Log.print((const char*) registersRC[ip->a.u32].pointer);
        g_Log.unlock();
        break;
    }

    case ByteCodeOp::AffectOp8:
        *(uint8_t*) registersRC[ip->a.u32].pointer = registersRC[ip->b.u32].u8;
        break;
    case ByteCodeOp::AffectOp16:
        *(uint16_t*) registersRC[ip->a.u32].pointer = registersRC[ip->b.u32].u16;
        break;
    case ByteCodeOp::AffectOp32:
        *(uint32_t*) registersRC[ip->a.u32].pointer = registersRC[ip->b.u32].u32;
        break;
    case ByteCodeOp::AffectOp64:
        *(uint64_t*) (registersRC[ip->a.u32].pointer + ip->c.s32) = registersRC[ip->b.u32].u64;
        break;
    case ByteCodeOp::AffectOpPointer:
        *(void**) registersRC[ip->a.u32].pointer = registersRC[ip->b.u32].pointer;
        break;

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
    case ByteCodeOp::CompareOpEqualString:
    {
        registersRC[ip->c.u32].b = !strcmp((const char*) registersRC[ip->a.u32].pointer, (const char*) registersRC[ip->b.u32].pointer);
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

    case ByteCodeOp::BitmaskAndS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 & registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BitmaskAndS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 & registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::BitmaskAndU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 & registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BitmaskAndU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 & registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::BitmaskOrS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 | registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::BitmaskOrS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 | registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::BitmaskOrU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 | registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::BitmaskOrU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 | registersRC[ip->b.u32].u64;
        break;
    }

    case ByteCodeOp::ShiftLeftS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 << registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::ShiftLeftS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 << registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::ShiftLeftU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 << registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::ShiftLeftU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 << registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::ShiftRightS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 >> registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::ShiftRightS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 >> registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::ShiftRightU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 >> registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::ShiftRightU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 >> registersRC[ip->b.u32].u32;
        break;
    }

    case ByteCodeOp::XorS32:
    {
        registersRC[ip->c.u32].s32 = registersRC[ip->a.u32].s32 ^ registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::XorS64:
    {
        registersRC[ip->c.u32].s64 = registersRC[ip->a.u32].s64 ^ registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::XorU32:
    {
        registersRC[ip->c.u32].u32 = registersRC[ip->a.u32].u32 ^ registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::XorU64:
    {
        registersRC[ip->c.u32].u64 = registersRC[ip->a.u32].u64 ^ registersRC[ip->b.u32].u64;
        break;
    }

    case ByteCodeOp::InvertS32:
    {
        registersRC[ip->a.u32].s32 = ~registersRC[ip->a.u32].s32;
        break;
    }
    case ByteCodeOp::InvertS64:
    {
        registersRC[ip->a.u32].s64 = ~registersRC[ip->a.u32].s64;
        break;
    }
    case ByteCodeOp::InvertU32:
    {
        registersRC[ip->a.u32].u32 = ~registersRC[ip->a.u32].u32;
        break;
    }
    case ByteCodeOp::InvertU64:
    {
        registersRC[ip->a.u32].u64 = ~registersRC[ip->a.u32].u64;
        break;
    }

    case ByteCodeOp::ClearMaskU32:
    {
        registersRC[ip->a.u32].u32 &= ip->b.u32;
        break;
    }
    case ByteCodeOp::ClearMaskU64:
    {
        registersRC[ip->a.u32].u64 &= 0xFFFFFFFF | (((uint64_t) ip->b.u32) << 32);
        break;
    }
    case ByteCodeOp::ClearMaskU32U64:
    {
        registersRC[ip->a.u32].u32 &= ip->b.u32;
        registersRC[ip->a.u32].u64 &= 0xFFFFFFFF | (((uint64_t) ip->c.u32) << 32);
        break;
    }
    case ByteCodeOp::CastS32F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].s32);
        break;
    }
    case ByteCodeOp::CastU32F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].u32);
        break;
    }
    case ByteCodeOp::CastU64F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].u64);
        break;
    }
    case ByteCodeOp::CastS16S32:
    {
        registersRC[ip->a.u32].s32 = static_cast<int32_t>(registersRC[ip->a.u32].s16);
        break;
    }
    case ByteCodeOp::CastS64F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].s64);
        break;
    }
    case ByteCodeOp::CastF64F32:
    {
        registersRC[ip->a.u32].f32 = static_cast<float>(registersRC[ip->a.u32].f64);
        break;
    }
    case ByteCodeOp::CastF32F64:
    {
        registersRC[ip->a.u32].f64 = static_cast<double>(registersRC[ip->a.u32].f32);
        break;
    }
    case ByteCodeOp::CastU64F64:
    {
        registersRC[ip->a.u32].f64 = static_cast<double>(registersRC[ip->a.u32].u64);
        break;
    }
    case ByteCodeOp::CastS64F64:
    {
        registersRC[ip->a.u32].f64 = static_cast<double>(registersRC[ip->a.u32].s64);
        break;
    }
    case ByteCodeOp::CastS64S32:
    {
        registersRC[ip->a.u32].s32 = static_cast<int32_t>(registersRC[ip->a.u32].s64);
        break;
    }
    case ByteCodeOp::CastF32S32:
    {
        registersRC[ip->a.u32].s32 = static_cast<int32_t>(registersRC[ip->a.u32].f32);
        break;
    }
    case ByteCodeOp::CastS32S16:
    {
        registersRC[ip->a.u32].s16 = static_cast<int16_t>(registersRC[ip->a.u32].s32);
        break;
    }
    case ByteCodeOp::CastS8S16:
    {
        registersRC[ip->a.u32].s16 = static_cast<int16_t>(registersRC[ip->a.u32].s8);
        break;
    }
    case ByteCodeOp::CastS32S8:
    {
        registersRC[ip->a.u32].s8 = static_cast<int8_t>(registersRC[ip->a.u32].s32);
        break;
    }
    case ByteCodeOp::CastF32S64:
    {
        registersRC[ip->a.u32].s64 = static_cast<int64_t>(registersRC[ip->a.u32].f32);
        break;
    }
    case ByteCodeOp::CastS32S64:
    {
        registersRC[ip->a.u32].s64 = static_cast<int64_t>(registersRC[ip->a.u32].s32);
        break;
    }
    case ByteCodeOp::CastF64S64:
    {
        registersRC[ip->a.u32].s64 = static_cast<int64_t>(registersRC[ip->a.u32].f64);
        break;
    }

    case ByteCodeOp::AffectOpPlusEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].f64;
        break;
    }
    case ByteCodeOp::AffectOpPlusEqPointer:
    {
        *(uint8_t**) registersRC[ip->a.u32].pointer += registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqPointer:
    {
        *(uint8_t**) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s32;
        break;
    }

    case ByteCodeOp::AffectOpMinusEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::AffectOpMinusEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer -= registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::AffectOpMulEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpMulEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpMulEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpMulEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpMulEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOpMulEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOpMulEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpMulEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::AffectOpMulEqF32:
    {
        *(float*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].f32;
        break;
    }
    case ByteCodeOp::AffectOpMulEqF64:
    {
        *(double*) registersRC[ip->a.u32].pointer *= registersRC[ip->b.u32].f64;
        break;
    }

    case ByteCodeOp::AffectOpDivEqF32:
    {
        auto val = registersRC[ip->b.u32].f32;
        if (val == 0.0f)
            context->error("division by zero");
        else
            *(float*) registersRC[ip->a.u32].pointer /= val;
        break;
    }
    case ByteCodeOp::AffectOpDivEqF64:
    {
        auto val = registersRC[ip->b.u32].f64;
        if (val == 0.0)
            context->error("division by zero");
        else
            *(double*) registersRC[ip->a.u32].pointer /= val;
        break;
    }

    case ByteCodeOp::AffectOpAndEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpAndEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpAndEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpAndEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpAndEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOpAndEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOpAndEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpAndEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer &= registersRC[ip->b.u32].u64;
        break;
    }

    case ByteCodeOp::AffectOpOrEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpOrEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpOrEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpOrEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpOrEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOpOrEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOpOrEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpOrEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer |= registersRC[ip->b.u32].u64;
        break;
    }

    case ByteCodeOp::AffectOpXOrEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpXOrEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer ^= registersRC[ip->b.u32].u64;
        break;
    }

    case ByteCodeOp::AffectOpShiftLeftEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftLeftEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer <<= registersRC[ip->b.u32].u32;
        break;
    }

    case ByteCodeOp::AffectOpShiftRightEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpShiftRightEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer >>= registersRC[ip->b.u32].u32;
        break;
    }

    default:
        if (ip->op < ByteCodeOp::End)
            context->error(format("unknown bytecode instruction '%s'", g_ByteCodeOpNames[(int) ip->op]));
        break;
    }

    return true;
}

bool ByteCodeRun::run(ByteCodeRunContext* context)
{
    while (true)
    {
        // Get instruction
        auto ip = context->ip++;
        if (ip->op == ByteCodeOp::End)
            break;

        if (!runNode(context, ip))
            break;

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
