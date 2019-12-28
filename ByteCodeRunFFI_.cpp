#include "pch.h"
#include "ByteCodeRun.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ByteCodeModuleManager.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "ModuleCompileJob.h"
#include "TypeManager.h"
#include "OS.h"

void* ByteCodeRun::ffiGetFuncAddress(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto nodeFunc = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(nodeFunc->resolvedSymbolOverload->typeInfo, TypeInfoKind::FuncAttr);

    // Load module if specified
    ComputedValue moduleName;
    bool          hasModuleName = typeFunc->attributes.getValue("swag.foreign.module", moduleName);
    if (hasModuleName)
    {
        // We need to be sure that the corresponding module has been built, as we can start a run without
        // dependencies being built. We are polling, this is not ideal, but a 'run' cannot be paused for now
        auto depModule = g_Workspace.getModuleByName(moduleName.text);
        while (depModule && depModule->getHasBeenBuilt() != BUILDRES_FULL)
        {
            this_thread::sleep_for(chrono::milliseconds(1));
            if (depModule->numErrors)
                return nullptr;
        }

        if (!g_ModuleMgr.loadModule(moduleName.text))
        {
            context->error(format("fail to load module '%s' => %s", moduleName.text.c_str(), OS::getLastErrorAsString().c_str()));
            return nullptr;
        }
    }

    auto& funcName = nodeFunc->name;
    auto  fn       = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : "", funcName);
    if (!fn)
    {
        auto it = typeFunc->attributes.values.find("swag.foreign.function");
        if (it != typeFunc->attributes.values.end())
            fn = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : "", it->second.second.text);
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
        if (externalModule->buildParameters.type == BackendOutputType::DynamicLib)
        {
            context->error(format("cannot resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }

        auto compileJob                  = g_Pool_moduleCompileJob.alloc();
        compileJob->module               = externalModule;
        compileJob->buildParameters      = externalModule->buildParameters;
        compileJob->buildParameters.type = BackendOutputType::DynamicLib;
        compileJob->mutexDone            = &mutexDone;
        compileJob->condVar              = &condVar;
        g_ThreadMgr.addJob(compileJob);

        // Sync wait for the dll to be generated
        unique_lock lk(mutexDone);
        condVar.wait(lk);

        // Last try
        if (!g_ModuleMgr.loadModule(moduleName.text))
        {
            context->error(format("fail to load module '%s' => %s", moduleName.text.c_str(), OS::getLastErrorAsString().c_str()));
            return nullptr;
        }

        fn = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : "", funcName);
        if (!externalModule)
        {
            context->error(format("cannot resolve external function call to '%s'", funcName.c_str()));
            return nullptr;
        }
    }

    return fn;
}

ffi_type* ByteCodeRun::ffiFromTypeInfo(TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Pointer)
        return &ffi_type_pointer;
    if (typeInfo->kind == TypeInfoKind::Struct)
        return &ffi_type_pointer;
    if (typeInfo->kind == TypeInfoKind::Slice)
        return &ffi_type_pointer;
    if (typeInfo->kind == TypeInfoKind::Interface)
        return &ffi_type_pointer;
    if (typeInfo->isNative(NativeTypeKind::String))
        return &ffi_type_pointer;

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
    if (!ip->d.pointer)
        ip->d.pointer = (uint8_t*) ffiGetFuncAddress(context, ip);
    if (!ip->d.pointer)
        return;
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::FuncAttr);

    // Function call parameters
    ffi_cif cif;
    int     numParameters = (int) typeInfoFunc->parameters.size();
    ffiArgs.clear();
    ffiArgsValues.clear();
    Register* sp = (Register*) context->sp;
    for (int i = 0; i < numParameters; i++)
    {
        auto typeParam = ((TypeInfoParam*) typeInfoFunc->parameters[i])->typeInfo;
        typeParam      = TypeManager::concreteType(typeParam);
        ffiArgs.push_back(ffiFromTypeInfo(typeParam));
        if (!ffiArgs.back())
        {
            context->hasError = true;
            context->errorMsg = format("ffi failed to convert argument type '%s'", typeParam->name.c_str());
            return;
        }

        if (typeParam->isNative(NativeTypeKind::String))
        {
            ffiArgsValues.push_back(&sp->pointer);
            sp += 2;
        }
        else if (typeParam->kind == TypeInfoKind::Slice)
        {
            ffiArgsValues.push_back(&sp->pointer);
            sp++;

            ffiArgs.push_back(&ffi_type_uint32);
            ffiArgsValues.push_back(&sp->u32);
            sp++;
        }
        else if (typeParam->kind == TypeInfoKind::Interface)
        {
            ffiArgsValues.push_back(&sp->pointer);
            sp++;

            ffiArgs.push_back(&ffi_type_pointer);
            ffiArgsValues.push_back(&sp->pointer);
            sp++;
        }
        else if (typeParam->flags & TYPEINFO_RETURN_BY_COPY)
        {
            ffiArgsValues.push_back(&sp->pointer);
            sp++;
        }
        else
        {
            switch (typeParam->sizeOf)
            {
            case 1:
                ffiArgsValues.push_back(&sp->u8);
                break;
            case 2:
                ffiArgsValues.push_back(&sp->u16);
                break;
            case 4:
                ffiArgsValues.push_back(&sp->u32);
                break;
            case 8:
                ffiArgsValues.push_back(&sp->u64);
                break;
            default:
                context->hasError = true;
                context->errorMsg = format("ffi failed to convert argument type '%s'", typeParam->name.c_str());
                return;
            }

            sp++;
        }
    }

    numParameters = (int) ffiArgs.size();

    // Function return type
    ffi_type* typeResult = &ffi_type_void;
    auto      returnType = TypeManager::concreteType(typeInfoFunc->returnType);
    if (returnType != g_TypeMgr.typeInfoVoid)
    {
        // Special return
        if (returnType->kind == TypeInfoKind::Slice)
        {
            numParameters++;
            ffiArgs.push_back(&ffi_type_pointer);
            ffiArgsValues.push_back(&context->registersRR);
        }
        else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            numParameters++;
            ffiArgs.push_back(&ffi_type_pointer);
            ffiArgsValues.push_back(context->registersRR);
        }
        else
        {
            typeResult = ffiFromTypeInfo(returnType);
            if (!typeResult)
            {
                context->hasError = true;
                context->errorMsg = format("ffi failed to convert return type '%s'", typeInfoFunc->returnType->name.c_str());
                return;
            }
        }
    }

    // Initialize the cif
    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, numParameters, typeResult, ffiArgs.empty() ? nullptr : &ffiArgs[0]);

    void* resultPtr = nullptr;
    if (typeResult != &ffi_type_void && !(returnType->flags & TYPEINFO_RETURN_BY_COPY))
    {
        SWAG_ASSERT(context->registersRR);
        switch (returnType->sizeOf)
        {
        case 1:
            resultPtr = &context->registersRR->u8;
            break;
        case 2:
            resultPtr = &context->registersRR->u16;
            break;
        case 4:
            resultPtr = &context->registersRR->u32;
            break;
        case 8:
            resultPtr = &context->registersRR->u64;
            break;
        default:
            context->hasError = true;
            context->errorMsg = format("ffi failed to get return result of type '%s'", typeInfoFunc->returnType->name.c_str());
            return;
        }
    }

    // Make the call
    ffi_call(&cif, FFI_FN(ip->d.pointer), resultPtr, ffiArgsValues.empty() ? nullptr : &ffiArgsValues[0]);
}
