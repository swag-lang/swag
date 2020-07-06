#include "pch.h"
#include "ByteCodeRun.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ModuleManager.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "ModuleCompileJob.h"
#include "TypeManager.h"
#include "OS.h"
#include "DiagnosticInfos.h"

void* ByteCodeRun::ffiGetFuncAddress(ByteCodeRunContext* context, ByteCodeInstruction* ip)
{
    auto nodeFunc = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    return ffiGetFuncAddress(context, nodeFunc);
}

void* ByteCodeRun::ffiGetFuncAddress(ByteCodeRunContext* context, AstFuncDecl* nodeFunc)
{
    SWAG_ASSERT(nodeFunc->resolvedSymbolOverload);
    SWAG_ASSERT(nodeFunc->resolvedSymbolOverload->typeInfo);
    auto  typeFunc = CastTypeInfo<TypeInfoFuncAttr>(nodeFunc->resolvedSymbolOverload->typeInfo, TypeInfoKind::FuncAttr);
    auto& funcName = nodeFunc->name;

    // Load module if specified
    ComputedValue moduleName;
    bool          hasModuleName = typeFunc->attributes.getValue("swag.foreign", "module", moduleName);
    if (hasModuleName)
    {
        if (!g_ModuleMgr.loadModule(moduleName.text))
        {
            // Sometimes it fails, don't know why. With another attempt just after, it's fine !
            int attempt = 5;
            while (attempt > 0)
            {
                this_thread::sleep_for(100ms);
                if (g_ModuleMgr.loadModule(moduleName.text))
                    break;
                attempt--;
            }

            if (attempt == 0)
            {
                context->error(format("failed to load module '%s' while resolving foreign function '%s' => %s", moduleName.text.c_str(), funcName.c_str(), OS::getLastErrorAsString().c_str()));
                return nullptr;
            }
        }
    }

    auto fn = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : Utf8(""), funcName);
    if (!fn)
    {
        ComputedValue foreignValue;
        if (typeFunc->attributes.getValue("swag.foreign", "function", foreignValue) && !foreignValue.text.empty())
            fn = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : Utf8(""), foreignValue.text);
    }

    if (!fn)
    {
        if (!hasModuleName || g_ModuleMgr.isModuleLoaded(moduleName.text))
        {
            context->error(format("cannot resolve foreign function call to '%s'", funcName.c_str()));
            return nullptr;
        }

        // Compile the generated files
        auto externalModule = g_Workspace.getModuleByName(moduleName.text);
        if (!externalModule)
        {
            context->error(format("cannot resolve foreign function call to '%s'", funcName.c_str()));
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
            context->error(format("failed to load module '%s' while resolving foreign function '%s' => %s", moduleName.text.c_str(), funcName.c_str(), OS::getLastErrorAsString().c_str()));
            return nullptr;
        }

        fn = g_ModuleMgr.getFnPointer(context, hasModuleName ? moduleName.text : Utf8(""), funcName);
        if (!externalModule)
        {
            context->error(format("cannot resolve foreign function call to '%s'", funcName.c_str()));
            return nullptr;
        }
    }

    return fn;
}

ffi_type* ByteCodeRun::ffiFromTypeInfo(TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Pointer ||
        typeInfo->kind == TypeInfoKind::Struct ||
        typeInfo->kind == TypeInfoKind::Array ||
        typeInfo->kind == TypeInfoKind::Slice ||
        typeInfo->kind == TypeInfoKind::Variadic ||
        typeInfo->kind == TypeInfoKind::Interface ||
        typeInfo->isNative(NativeTypeKind::Any) ||
        typeInfo->isNative(NativeTypeKind::String))
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
    case NativeTypeKind::Char:
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

#ifdef SWAG_HAS_ASSERT
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.push();
        AstFuncDecl* funcDecl               = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
        g_diagnosticInfos.last().message    = format("ffi call to '%s'", funcDecl->name.c_str());
        g_diagnosticInfos.last().sourceFile = context->sourceFile;
        g_diagnosticInfos.last().ip         = ip;
    }
#endif

    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::FuncAttr);
    ffiCall(context, ip->d.pointer, typeInfoFunc);
}

void ByteCodeRun::ffiCall(ByteCodeRunContext* context, void* foreignPtr, TypeInfoFuncAttr* typeInfoFunc)
{
    // Function call parameters
    ffi_cif cif;
    int     numParameters = (int) typeInfoFunc->parameters.size();
    ffiArgs.clear();
    ffiArgsValues.clear();
    Register* sp = (Register*) context->sp;

    // Variadic parameters are first on the stack, so need to treat them before
    if (numParameters)
    {
        auto typeParam = ((TypeInfoParam*) typeInfoFunc->parameters.back())->typeInfo;
        if (typeParam->kind == TypeInfoKind::Variadic)
        {
            // Pointer
            ffiArgs.push_back(ffiFromTypeInfo(typeParam));
            ffiArgsValues.push_back(&sp->pointer);
            sp++;

            // Count
            ffiArgs.push_back(&ffi_type_uint64);
            ffiArgsValues.push_back(&sp->u64);
            sp++;

            numParameters--;
        }
    }

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

        if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            ffiArgsValues.push_back(&sp->pointer); // Pointer
            sp++;
            ffiArgs.push_back(&ffi_type_uint32); // Count
            ffiArgsValues.push_back(&sp->u32);
            sp++;
        }
        else if (typeParam->isNative(NativeTypeKind::Any))
        {
            ffiArgsValues.push_back(&sp->pointer); // Value
            sp++;
            ffiArgs.push_back(&ffi_type_pointer); // Type
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
        if (returnType->kind == TypeInfoKind::Slice ||
            returnType->isNative(NativeTypeKind::Any) ||
            returnType->isNative(NativeTypeKind::String))
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
    ffi_call(&cif, FFI_FN(foreignPtr), resultPtr, ffiArgsValues.empty() ? nullptr : &ffiArgsValues[0]);

#ifdef SWAG_HAS_ASSERT
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.pop();
    }
#endif
}
