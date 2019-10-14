#include "pch.h"
#include "ByteCodeRun.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ByteCodeModuleManager.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "ModuleCompileJob.h"
#include "TypeManager.h"

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
        auto externalModule = g_Workspace.getModuleByName(moduleName.text);
        if (!externalModule)
        {
            context->error(format("cannot resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }

        // Need to compile the dll version of the module in order to be able to call a function
        // from the compiler
        if (externalModule->buildParameters.type == BackendOutputType::Dll)
        {
            context->error(format("cannot resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }

        auto compileJob                  = g_Pool_moduleCompileJob.alloc();
        compileJob->module               = externalModule;
        compileJob->buildParameters      = externalModule->buildParameters;
        compileJob->buildParameters.type = BackendOutputType::Dll;
        compileJob->mutexDone            = &mutexDone;
        compileJob->condVar              = &condVar;
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
