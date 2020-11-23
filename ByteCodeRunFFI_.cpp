#include "pch.h"
#include "ByteCodeRun.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ModuleManager.h"
#include "Workspace.h"
#include "ThreadManager.h"
#include "ModuleGenOutputJob.h"
#include "TypeManager.h"
#include "OS.h"
#include "Module.h"

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
    typeFunc->attributes.getValue("swag.foreign", "module", moduleName);
    SWAG_ASSERT(!moduleName.text.empty());

    if (!g_ModuleMgr.loadModule(moduleName.text))
    {
        // Perhaps the module is dependent on another module, so we need to be sure that our dependencies are
        // all loaded : load all, from last to first (dependencies are added in reverse order, latest first)
        auto module = context->sourceFile->module;
        for (auto& dep : module->moduleDependencies)
            g_ModuleMgr.loadModule(dep->name);

        // Then try again
        if (!g_ModuleMgr.loadModule(moduleName.text))
        {
            if (g_CommandLine.devMode)
                SWAG_ASSERT(false);
            context->error(format("failed to load module '%s' while resolving foreign function '%s' => %s", moduleName.text.c_str(), funcName.c_str(), OS::getLastErrorAsString().c_str()));
            return nullptr;
        }
    }

    ComputedValue foreignValue;
    void*         fn = nullptr;
    if (typeFunc->attributes.getValue("swag.foreign", "function", foreignValue) && !foreignValue.text.empty())
        fn = g_ModuleMgr.getFnPointer(moduleName.text, foreignValue.text);
    else
        fn = g_ModuleMgr.getFnPointer(moduleName.text, funcName);

    if (!fn)
    {
        if (g_CommandLine.devMode)
            SWAG_ASSERT(false);
        context->error(format("cannot resolve foreign function call to '%s'", funcName.c_str()));
        return nullptr;
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
        typeInfo->kind == TypeInfoKind::TypedVariadic ||
        typeInfo->kind == TypeInfoKind::Interface ||
        typeInfo->kind == TypeInfoKind::TypeSet ||
        typeInfo->kind == TypeInfoKind::Lambda ||
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
    if (OS::atomicTestNull((void**) &ip->d.pointer))
        OS::atomicSetIfNotNull((void**) &ip->d.pointer, (uint8_t*) ffiGetFuncAddress(context, ip));
    if (OS::atomicTestNull((void**) &ip->d.pointer))
        return;
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::FuncAttr);
    ffiCall(context, (void*) ip->d.pointer, typeInfoFunc);
}

void ByteCodeRun::ffiCall(ByteCodeRunContext* context, void* foreignPtr, TypeInfoFuncAttr* typeInfoFunc)
{
    // Function call parameters
    ffi_cif cif;
    int     numParameters = (int) typeInfoFunc->parameters.size();
    context->ffiArgs.clear();
    context->ffiArgsValues.clear();
    Register* sp = (Register*) context->sp;

    // Variadic parameters are first on the stack, so need to treat them before
    if (numParameters)
    {
        auto typeParam = ((TypeInfoParam*) typeInfoFunc->parameters.back())->typeInfo;
        if (typeParam->kind == TypeInfoKind::Variadic || typeParam->kind == TypeInfoKind::TypedVariadic)
        {
            // Pointer
            context->ffiArgs.push_back(ffiFromTypeInfo(typeParam));
            context->ffiArgsValues.push_back(&sp->pointer);
            sp++;

            // Count
            context->ffiArgs.push_back(&ffi_type_uint64);
            context->ffiArgsValues.push_back(&sp->u64);
            sp++;

            numParameters--;
        }
    }

    for (int i = 0; i < numParameters; i++)
    {
        auto typeParam = ((TypeInfoParam*) typeInfoFunc->parameters[i])->typeInfo;
        typeParam      = TypeManager::concreteReferenceType(typeParam);
        context->ffiArgs.push_back(ffiFromTypeInfo(typeParam));
        if (!context->ffiArgs.back())
        {
            context->hasError = true;
            context->errorMsg = format("ffi failed to convert argument type '%s'", typeParam->name.c_str());
            return;
        }

        if (typeParam->kind == TypeInfoKind::Slice || typeParam->isNative(NativeTypeKind::String))
        {
            context->ffiArgsValues.push_back(&sp->pointer); // Pointer
            sp++;
            context->ffiArgs.push_back(&ffi_type_uint32); // Count
            context->ffiArgsValues.push_back(&sp->u32);
            sp++;
        }
        else if (typeParam->isNative(NativeTypeKind::Any) || typeParam->kind == TypeInfoKind::Interface || typeParam->kind == TypeInfoKind::TypeSet)
        {
            context->ffiArgsValues.push_back(&sp->pointer); // Value
            sp++;
            context->ffiArgs.push_back(&ffi_type_pointer); // Type
            context->ffiArgsValues.push_back(&sp->pointer);
            sp++;
        }
        else if (typeParam->flags & TYPEINFO_RETURN_BY_COPY)
        {
            context->ffiArgsValues.push_back(&sp->pointer);
            sp++;
        }
        else
        {
            switch (typeParam->sizeOf)
            {
            case 1:
                context->ffiArgsValues.push_back(&sp->u8);
                break;
            case 2:
                context->ffiArgsValues.push_back(&sp->u16);
                break;
            case 4:
                context->ffiArgsValues.push_back(&sp->u32);
                break;
            case 8:
                context->ffiArgsValues.push_back(&sp->u64);
                break;
            default:
                context->hasError = true;
                context->errorMsg = format("ffi failed to convert argument type '%s'", typeParam->name.c_str());
                return;
            }

            sp++;
        }
    }

    numParameters = (int) context->ffiArgs.size();

    // Function return type
    ffi_type* typeResult = &ffi_type_void;
    auto      returnType = TypeManager::concreteType(typeInfoFunc->returnType);
    if (returnType != g_TypeMgr.typeInfoVoid)
    {
        // Special return
        if (returnType->kind == TypeInfoKind::Slice ||
            returnType->kind == TypeInfoKind::Interface ||
            returnType->kind == TypeInfoKind::TypeSet ||
            returnType->isNative(NativeTypeKind::Any) ||
            returnType->isNative(NativeTypeKind::String))
        {
            numParameters++;
            context->ffiArgs.push_back(&ffi_type_pointer);
            context->ffiArgsValues.push_back(&context->registersRR);
        }
        else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            numParameters++;
            context->ffiArgs.push_back(&ffi_type_pointer);
            context->ffiArgsValues.push_back(context->registersRR);
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
    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, numParameters, typeResult, context->ffiArgs.empty() ? nullptr : &context->ffiArgs[0]);

    void* resultPtr = nullptr;
    if (typeResult != &ffi_type_void)
    {
        SWAG_ASSERT(context->registersRR);
        switch (returnType->sizeOf)
        {
        case 1:
            resultPtr = &context->registersRR[0].u8;
            break;
        case 2:
            resultPtr = &context->registersRR[0].u16;
            break;
        case 4:
            resultPtr = &context->registersRR[0].u32;
            break;
        case 8:
            resultPtr = &context->registersRR[0].u64;
            break;
        default:
            context->hasError = true;
            context->errorMsg = format("ffi failed to get return result of type '%s'", typeInfoFunc->returnType->name.c_str());
            return;
        }
    }

    // Make the call
    ffi_call(&cif, FFI_FN(foreignPtr), resultPtr, context->ffiArgsValues.empty() ? nullptr : &context->ffiArgsValues[0]);
}
