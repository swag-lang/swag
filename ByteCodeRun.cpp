#include "pch.h"
#include "ByteCodeRun.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "ByteCode.h"
#include "Module.h"
#include "ByteCodeOp.h"
#include "Ast.h"
#include "ByteCodeModuleManager.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "ModuleCompileJob.h"
#include "TypeManager.h"
#include "Context.h"

ByteCodeRun g_Run;

void* ByteCodeRun::ffiGetFuncAddress(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto nodeFunc = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(nodeFunc->resolvedSymbolOverload->typeInfo, TypeInfoKind::FuncAttr);

    // Load module if specified
    ComputedValue moduleName;
    bool          hasModuleName = typeFunc->attributes.getValue("swag.foreign.module", moduleName);
    if (hasModuleName)
        g_ModuleMgr.loadModule(context, moduleName.text);

    auto funcName = nodeFunc->resolvedSymbolName->fullName;
    replaceAll(funcName, '.', '_');
    auto fn = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : "", funcName);
    if (!fn)
    {
        funcName = nodeFunc->resolvedSymbolName->name;
        fn       = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : "", funcName);
    }

    if (!fn)
    {
        if (!hasModuleName || g_ModuleMgr.isModuleLoaded(moduleName.text))
        {
            context->error(format("cannot resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }

        // Compile the generated files
        auto externalModule = context->sourceFile->module->workspace->getModuleByName(moduleName.text);
        if (!externalModule)
        {
            context->error(format("cannot resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }

        // Need to compile the dll version of the module in order to be able to call a function
        // from the compiler
        if (externalModule->backendParameters.type == BackendType::Dll)
        {
            context->error(format("cannot resolve external function call to '%s'", funcName.c_str()));
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
            context->error(format("cannot resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }
    }

    return fn;
}

ffi_type* ByteCodeRun::ffiFromTypeinfo(TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Pointer)
        return &ffi_type_pointer;

    if (typeInfo->isNative(NativeTypeKind::String))
        return nullptr;

    if (typeInfo->kind != TypeInfoKind::Native)
        return nullptr;

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        return &ffi_type_uint8;
    case NativeTypeKind::S8:
        return &ffi_type_sint8;
    case NativeTypeKind::U16:
        return &ffi_type_uint16;
    case NativeTypeKind::S16:
        return &ffi_type_sint16;
    case NativeTypeKind::S32:
        return &ffi_type_sint32;
    case NativeTypeKind::U32:
        return &ffi_type_uint32;
    case NativeTypeKind::S64:
        return &ffi_type_sint64;
    case NativeTypeKind::U64:
        return &ffi_type_uint64;
    case NativeTypeKind::F32:
        return &ffi_type_float;
    case NativeTypeKind::F64:
        return &ffi_type_double;
    case NativeTypeKind::Bool:
        return &ffi_type_uint8;
    }

    return nullptr;
}

void ByteCodeRun::ffiCall(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    if (!ip->cache.pointer)
        ip->cache.pointer = (uint8_t*) ffiGetFuncAddress(context, ip);
    if (!ip->cache.pointer)
        return;
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::FuncAttr);

    // Function call parameters
    ffi_cif cif;
    ffiArgs.resize(typeInfoFunc->parameters.size());
    ffiArgsValues.resize(typeInfoFunc->parameters.size());

    Register* sp = (Register*) context->sp;
    for (int i = 0; i < typeInfoFunc->parameters.size(); i++)
    {
        auto typeParam = ((TypeInfoParam*) typeInfoFunc->parameters[i])->typeInfo;
        ffiArgs[i]     = ffiFromTypeinfo(typeParam);
        if (!ffiArgs[i])
        {
            context->errorMsg = format("ffi failed to convert argument type '%s'", typeParam->name.c_str());
            return;
        }

        switch (typeParam->sizeOf)
        {
        case 1:
            ffiArgsValues[i] = &sp->u8;
            break;
        case 2:
            ffiArgsValues[i] = &sp->u16;
            break;
        case 4:
            ffiArgsValues[i] = &sp->u32;
            break;
        case 8:
            ffiArgsValues[i] = &sp->u64;
            break;
        default:
            context->errorMsg = format("ffi failed to convert argument type '%s'", typeParam->name.c_str());
            return;
        }

        sp++;
    }

    // Function return type
    ffi_type* typeResult = &ffi_type_void;
    if (typeInfoFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        typeResult = ffiFromTypeinfo(typeInfoFunc->returnType);
        if (!typeResult)
        {
            context->errorMsg = format("ffi failed to convert return type '%s'", typeInfoFunc->returnType->name.c_str());
            return;
        }
    }

    // Initialize the cif
    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, (int) typeInfoFunc->parameters.size(), typeResult, &ffiArgs[0]);

    // Store result
    Register result;
    result.pointer = 0;

    void* resultPtr = nullptr;
    if (typeInfoFunc->returnType != g_TypeMgr.typeInfoVoid)
    {
        switch (typeInfoFunc->returnType->sizeOf)
        {
        case 1:
            resultPtr = &result.u8;
            break;
        case 2:
            resultPtr = &result.u16;
            break;
        case 4:
            resultPtr = &result.u32;
            break;
        case 8:
            resultPtr = &result.u64;
            break;
        default:
            context->errorMsg = format("ffi failed to convert return type '%s'", typeInfoFunc->returnType->name.c_str());
            return;
        }
    }

    // Make the call
    ffi_call(&cif, FFI_FN(ip->cache.pointer), resultPtr, &ffiArgsValues[0]);
    context->registersRR[0] = result;
}

inline bool ByteCodeRun::executeInstruction(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto registersRC = context->bc->registersRC[context->bc->curRC];
    auto registersRR = context->registersRR;

    switch (ip->op)
    {
    case ByteCodeOp::JumpZero32:
    {
        if (!registersRC[ip->a.u32].u32)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpNotZero32:
    {
        if (registersRC[ip->a.u32].u32)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpNotTrue:
    {
        if (!registersRC[ip->a.u32].b)
            context->ip += ip->b.s32;
        break;
    }
    case ByteCodeOp::JumpTrue:
    {
        if (registersRC[ip->a.u32].b)
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
        context->bc->leaveByteCode();
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
        SWAG_ASSERT(context->ip);
        context->bp = context->sp;
        context->bc->enterByteCode(context);
        break;
    }
    case ByteCodeOp::LambdaCall:
    {
        context->push(context->bp);
        context->push(context->bc);
        context->push(context->ip);
        context->bc = (ByteCode*) registersRC[ip->a.u32].pointer;

        if (!context->bc)
        {
            context->error("dereferencing a null pointer");
            break;
        }

        context->ip = context->bc->out;
        SWAG_ASSERT(context->ip);
        context->bp = context->sp;
        context->bc->enterByteCode(context);
        break;
    }
    case ByteCodeOp::MakeLambda:
    {
        registersRC[ip->a.u32].pointer = ip->b.pointer;
        break;
    }
    case ByteCodeOp::ForeignCall:
    {
        ffiCall(context, ip);
        break;
    }

    case ByteCodeOp::IncPointerVB:
    {
        registersRC[ip->a.u32].pointer += ip->b.s32;
        break;
    }
    case ByteCodeOp::IncPointer:
    {
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer + registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::DecPointer:
    {
        registersRC[ip->c.u32].pointer = registersRC[ip->a.u32].pointer - registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::DeRef8:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->a.u32].u64 = *(uint8_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRef16:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->a.u32].u64 = *(uint16_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRef32:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->a.u32].u64 = *(uint32_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRef64:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->a.u32].u64 = *(uint64_t*) ptr;
        break;
    }
    case ByteCodeOp::DeRefPointer:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            registersRC[ip->b.u32].pointer = *(uint8_t**) (ptr + ip->c.u32);
        break;
    }
    case ByteCodeOp::DeRefStringSlice:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
        {
            uint64_t** ptrptr              = (uint64_t**) ptr;
            registersRC[ip->a.u32].pointer = (uint8_t*) ptrptr[0];
            registersRC[ip->b.u32].u64     = (uint64_t) ptrptr[1];
        }
        break;
    }

    case ByteCodeOp::BoundCheckString:
    {
        uint32_t curOffset = registersRC[ip->a.u32].u32;
        uint32_t maxOffset = registersRC[ip->b.u32].u32;
        if (curOffset > maxOffset)
            context->error(format("index out of range (index is '%u', maximum index is '%u')", curOffset, maxOffset));
        break;
    }
    case ByteCodeOp::BoundCheck:
    {
        uint32_t curOffset = registersRC[ip->a.u32].u32;
        uint32_t maxOffset = registersRC[ip->b.u32].u32;
        if (curOffset >= maxOffset)
            context->error(format("index out of range (index is '%u', maximum index is '%u')", curOffset, maxOffset - 1));
        break;
    }

    case ByteCodeOp::PushRAParam:
    {
        context->push(registersRC[ip->a.u32].u64);
        break;
    }
    case ByteCodeOp::PushRRSaved:
    {
        context->push(registersRR[ip->a.u32].u64);
        break;
    }
    case ByteCodeOp::PopRRSaved:
    {
        registersRR[ip->a.u32].u64 = context->pop<uint64_t>();
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
    case ByteCodeOp::CopyVC:
    {
        void*    dst  = registersRC[ip->a.u32].pointer;
        void*    src  = registersRC[ip->b.u32].pointer;
        uint32_t size = ip->c.u32;
        memcpy(dst, src, size);
        break;
    }
    case ByteCodeOp::Copy:
    {
        void*    dst  = registersRC[ip->a.u32].pointer;
        void*    src  = registersRC[ip->b.u32].pointer;
        uint32_t size = registersRC[ip->c.u32].u32;

        if (!dst)
        {
            context->error("destination pointer of copy is null");
            break;
        }

        if (!src)
        {
            context->error("source pointer of copy is null");
            break;
        }

        memcpy(dst, src, size);
        break;
    }
    case ByteCodeOp::CopyRARBStr:
    {
        auto module = context->sourceFile->module;
        SWAG_ASSERT(ip->c.u32 < module->strBuffer.size());
        const auto& str                = module->strBuffer[ip->c.u32];
        registersRC[ip->a.u32].pointer = (uint8_t*) str.c_str();
        registersRC[ip->b.u32].u32     = (uint32_t) str.length();
        break;
    }
    case ByteCodeOp::CopyRARB:
    {
        registersRC[ip->a.u32] = registersRC[ip->b.u32];
        break;
    }
    case ByteCodeOp::CopyRARBAddr:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) (registersRC + ip->b.u32);
        break;
    }
    case ByteCodeOp::CopyRAVB32:
    {
        registersRC[ip->a.u32].u32 = ip->b.u32;
        break;
    }
    case ByteCodeOp::CopyRAVB64:
    {
        registersRC[ip->a.u32].u64 = ip->b.u64;
        break;
    }
    case ByteCodeOp::ClearRA:
    {
        registersRC[ip->a.u32].u64 = 0;
        break;
    }
    case ByteCodeOp::DecRA:
    {
        registersRC[ip->a.u32].u32--;
        break;
    }
    case ByteCodeOp::IncRA:
    {
        registersRC[ip->a.u32].u32++;
        break;
    }
    case ByteCodeOp::IncRA64:
    {
        registersRC[ip->a.u32].u64++;
        break;
    }
    case ByteCodeOp::IncRAVB:
    {
        registersRC[ip->a.u32].u32 += ip->b.u32;
        break;
    }

    case ByteCodeOp::CopyRRxRCx:
    case ByteCodeOp::CopyRRxRCxCall:
    {
        registersRR[ip->a.u32] = registersRC[ip->b.u32];
        break;
    }
    case ByteCodeOp::CopyRCxRRx:
    case ByteCodeOp::CopyRCxRRxCall:
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
    case ByteCodeOp::MovRASP:
    case ByteCodeOp::MovRASPVaargs:
        registersRC[ip->a.u32].pointer = context->sp - ip->b.u32;
        break;

    case ByteCodeOp::RAFromStack8:
        registersRC[ip->a.u32].u8 = *(uint8_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::RAFromStack16:
        registersRC[ip->a.u32].u16 = *(uint16_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::RAFromStack32:
        registersRC[ip->a.u32].u32 = *(uint32_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::RAFromStack64:
    case ByteCodeOp::RAFromStackParam64:
        registersRC[ip->a.u32].u64 = *(uint64_t*) (context->bp + ip->b.u32);
        break;
    case ByteCodeOp::RARefFromStack:
    case ByteCodeOp::RARefFromStackParam:
        registersRC[ip->a.u32].pointer = context->bp + ip->b.u32;
        break;

    case ByteCodeOp::ClearRefFromStack8:
        *(uint8_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::ClearRefFromStack16:
        *(uint16_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::ClearRefFromStack32:
        *(uint32_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::ClearRefFromStack64:
        *(uint64_t*) (context->bp + ip->a.u32) = 0;
        break;
    case ByteCodeOp::ClearRefFromStackX:
        memset(context->bp + ip->a.u32, 0, ip->b.u32);
        break;

    case ByteCodeOp::Clear8:
        *(uint8_t*) (registersRC[ip->a.u32].pointer) = 0;
        break;
    case ByteCodeOp::Clear16:
        *(uint16_t*) (registersRC[ip->a.u32].pointer) = 0;
        break;
    case ByteCodeOp::Clear32:
        *(uint32_t*) (registersRC[ip->a.u32].pointer) = 0;
        break;
    case ByteCodeOp::Clear64:
        *(uint64_t*) (registersRC[ip->a.u32].pointer) = 0;
        break;
    case ByteCodeOp::ClearX:
        memset(registersRC[ip->a.u32].pointer, 0, ip->b.u32);
        break;
    case ByteCodeOp::ClearXVar:
        memset(registersRC[ip->a.u32].pointer, 0, registersRC[ip->b.u32].u32 * ip->c.u32);
        break;

    case ByteCodeOp::RAFromDataSeg8:
    {
        auto module = context->sourceFile->module;
        if (!ip->cache.pointer)
            ip->cache.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u8 = *(uint8_t*) (ip->cache.pointer);
        break;
    }
    case ByteCodeOp::RAFromDataSeg16:
    {
        auto module = context->sourceFile->module;
        if (!ip->cache.pointer)
            ip->cache.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u16 = *(uint16_t*) (ip->cache.pointer);
        break;
    }
    case ByteCodeOp::RAFromDataSeg32:
    {
        auto module = context->sourceFile->module;
        if (!ip->cache.pointer)
            ip->cache.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u32 = *(uint32_t*) (ip->cache.pointer);
        break;
    }
    case ByteCodeOp::RAFromDataSeg64:
    {
        auto module = context->sourceFile->module;
        if (!ip->cache.pointer)
            ip->cache.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].u64 = *(uint64_t*) (ip->cache.pointer);
        break;
    }
    case ByteCodeOp::RARefFromDataSeg:
    {
        auto module = context->sourceFile->module;
        if (!ip->cache.pointer)
            ip->cache.pointer = module->mutableSegment.address(ip->b.u32);
        registersRC[ip->a.u32].pointer = ip->cache.pointer;
        break;
    }
    case ByteCodeOp::RAAddrFromConstantSeg:
    {
        auto module = context->sourceFile->module;
        auto offset = ip->b.u32;
        if (!ip->cache.pointer)
            ip->cache.pointer = module->constantSegment.address(offset);
        registersRC[ip->a.u32].pointer = ip->cache.pointer;
        break;
    }
    case ByteCodeOp::RARefFromConstantSeg:
    {
        auto module = context->sourceFile->module;
        auto offset = (uint32_t)(ip->c.u64 >> 32);
        auto count  = (uint32_t)(ip->c.u64 & 0xFFFFFFFF);
        if (!ip->cache.pointer)
            ip->cache.pointer = module->constantSegment.address(offset);
        registersRC[ip->a.u32].pointer = ip->cache.pointer;
        registersRC[ip->b.u32].u64     = count;
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

    case ByteCodeOp::MulRAVB:
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
        {
            if (ip->c.pointer)
                context->error(format("assertion failed, %s", ip->c.pointer));
            else
                context->error("assertion failed");
        }
        break;
    }
    case ByteCodeOp::IntrinsicAlloc:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) malloc(registersRC[ip->b.u32].u32);
        break;
    }
    case ByteCodeOp::IntrinsicRealloc:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) realloc(registersRC[ip->b.u32].pointer, registersRC[ip->c.u32].u32);
        break;
    }
    case ByteCodeOp::IntrinsicFree:
    {
        free(registersRC[ip->a.u32].pointer);
        break;
    }
    case ByteCodeOp::IntrinsicGetContext:
    {
        registersRC[ip->a.u32].pointer = (uint8_t*) TlsGetValue(g_tlsContextId);
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
    case ByteCodeOp::IntrinsicPrintString:
    {
        g_Log.lock();
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            g_Log.print("<null>");
        else
            g_Log.print(string((const char*) ptr, registersRC[ip->b.u32].u32));
        g_Log.unlock();
        break;
    }

    case ByteCodeOp::AffectOp8:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint8_t*) ptr = registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOp16:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint16_t*) ptr = registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOp32:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint32_t*) (ptr + ip->c.u32) = registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOp64:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint64_t*) (ptr + ip->c.u32) = registersRC[ip->b.u32].u64;
        break;
    }
    case ByteCodeOp::AffectOp64Null:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(uint64_t*) (ptr + ip->b.u32) = 0;
        break;
    }
    case ByteCodeOp::AffectOpPointer:
    {
        auto ptr = registersRC[ip->a.u32].pointer;
        if (ptr == nullptr)
            context->error("dereferencing a null pointer");
        else
            *(void**) ptr = registersRC[ip->b.u32].pointer;
        break;
    }

    case ByteCodeOp::CompareOpEqualBool:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].b == registersRC[ip->b.u32].b;
        break;
    }
    case ByteCodeOp::CompareOpEqual8:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u8 == registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::CompareOpEqual16:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].u16 == registersRC[ip->b.u32].u16;
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
    case ByteCodeOp::CompareOpEqualPointer:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].pointer == registersRC[ip->b.u32].pointer;
        break;
    }
    case ByteCodeOp::CompareOpEqualString:
    {
        if (!registersRC[ip->a.u32].pointer || !registersRC[ip->b.u32].pointer)
            registersRC[ip->c.u32].b = registersRC[ip->a.u32].pointer == registersRC[ip->b.u32].pointer;
		else
		{
			auto length = registersRC[ip->c.u32].u32;
			registersRC[ip->c.u32].b = !strncmp((const char*)registersRC[ip->a.u32].pointer, (const char*)registersRC[ip->b.u32].pointer, length);
		}
        break;
    }
    case ByteCodeOp::IsNullString:
    {
        registersRC[ip->b.u32].b = registersRC[ip->a.u32].pointer == nullptr;
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
    case ByteCodeOp::CompareOpLowerPointer:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].pointer < registersRC[ip->b.u32].pointer;
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
    case ByteCodeOp::CompareOpGreaterPointer:
    {
        registersRC[ip->c.u32].b = registersRC[ip->a.u32].pointer > registersRC[ip->b.u32].pointer;
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
    case ByteCodeOp::ShiftRightU64VB:
    {
        registersRC[ip->a.u32].u64 >>= ip->b.u32;
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
    case ByteCodeOp::CastBool8:
    {
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].u8 ? true : false;
        break;
    }
    case ByteCodeOp::CastBool16:
    {
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].u16 ? true : false;
        break;
    }
    case ByteCodeOp::CastBool32:
    {
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].u32 ? true : false;
        break;
    }
    case ByteCodeOp::CastBool64:
    {
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].u64 ? true : false;
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

    case ByteCodeOp::AffectOpPercentEqS8:
    {
        *(int8_t*) registersRC[ip->a.u32].pointer %= registersRC[ip->b.u32].s8;
        break;
    }
    case ByteCodeOp::AffectOpPercentEqS16:
    {
        *(int16_t*) registersRC[ip->a.u32].pointer %= registersRC[ip->b.u32].s16;
        break;
    }
    case ByteCodeOp::AffectOpPercentEqS32:
    {
        *(int32_t*) registersRC[ip->a.u32].pointer %= registersRC[ip->b.u32].s32;
        break;
    }
    case ByteCodeOp::AffectOpPercentEqS64:
    {
        *(int64_t*) registersRC[ip->a.u32].pointer %= registersRC[ip->b.u32].s64;
        break;
    }
    case ByteCodeOp::AffectOpPercentEqU8:
    {
        *(uint8_t*) registersRC[ip->a.u32].pointer %= registersRC[ip->b.u32].u8;
        break;
    }
    case ByteCodeOp::AffectOpPercentEqU16:
    {
        *(uint16_t*) registersRC[ip->a.u32].pointer %= registersRC[ip->b.u32].u16;
        break;
    }
    case ByteCodeOp::AffectOpPercentEqU32:
    {
        *(uint32_t*) registersRC[ip->a.u32].pointer %= registersRC[ip->b.u32].u32;
        break;
    }
    case ByteCodeOp::AffectOpPercentEqU64:
    {
        *(uint64_t*) registersRC[ip->a.u32].pointer %= registersRC[ip->b.u32].u64;
        break;
    }

    case ByteCodeOp::MinusToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 < 0 ? true : false;
        break;
    case ByteCodeOp::MinusZeroToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 <= 0 ? true : false;
        break;
    case ByteCodeOp::PlusToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 > 0 ? true : false;
        break;
    case ByteCodeOp::PlusZeroToTrue:
        registersRC[ip->a.u32].b = registersRC[ip->a.u32].s32 >= 0 ? true : false;
        break;

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

        if (!executeInstruction(context, ip))
            break;

        // Error ?
        if (context->hasError)
        {
            SWAG_ASSERT(ip->sourceFileIdx < context->sourceFile->module->files.size());
            auto sourceFile = context->sourceFile->module->files[ip->sourceFileIdx];
            return context->sourceFile->report({sourceFile, ip->startLocation, ip->endLocation, "error during bytecode execution, " + context->errorMsg});
        }
    }

    return true;
}

bool ByteCodeRun::internalError(ByteCodeRunContext* context)
{
    context->sourceFile->report({context->sourceFile, context->node->token, "internal compiler error during bytecode execution"});
    return false;
}
