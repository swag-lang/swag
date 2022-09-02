#include "pch.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ModuleManager.h"
#include "TypeManager.h"
#include "Os.h"
#include "Module.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "BackendX64.h"
#include "JobThread.h"

static thread_local X64PerThread g_PP;

void* ByteCodeRun::ffiGetFuncAddress(JobContext* context, ByteCodeInstruction* ip)
{
    auto nodeFunc = CastAst<AstFuncDecl>((AstNode*) ip->a.pointer, AstNodeKind::FuncDecl);
    return ffiGetFuncAddress(context, nodeFunc);
}

void* ByteCodeRun::ffiGetFuncAddress(JobContext* context, AstFuncDecl* nodeFunc)
{
    SWAG_ASSERT(nodeFunc->resolvedSymbolOverload);
    SWAG_ASSERT(nodeFunc->resolvedSymbolOverload->typeInfo);
    auto  typeFunc = CastTypeInfo<TypeInfoFuncAttr>(nodeFunc->resolvedSymbolOverload->typeInfo, TypeInfoKind::FuncAttr);
    auto& funcName = nodeFunc->token.text;

    // Load module if specified
    auto moduleName = ModuleManager::getForeignModuleName(nodeFunc);
    if (!g_ModuleMgr->loadModule(moduleName))
    {
        // Perhaps the module is dependent on another module, so we need to be sure that our dependencies are
        // all loaded : load all, from last to first (dependencies are added in reverse order, latest first)
        SWAG_ASSERT(context->sourceFile);
        auto module = context->sourceFile->module;
        for (auto& dep : module->moduleDependencies)
            g_ModuleMgr->loadModule(dep->name);

        // Then try again
        g_ModuleMgr->resetFailedModule(moduleName);
        if (!g_ModuleMgr->loadModule(moduleName))
        {
#ifdef SWAG_DEV_MODE
            SWAG_ASSERT(false);
#endif
            Diagnostic diag{nodeFunc, Fmt(Err(Err0257), moduleName.c_str(), funcName.c_str(), g_ModuleMgr->loadModuleError.c_str())};
            diag.showSource = false;
            context->report(diag);
            return nullptr;
        }
    }

    void* fn           = nullptr;
    auto  foreignValue = typeFunc->attributes.getValue(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_function);
    if (foreignValue && !foreignValue->text.empty())
        fn = g_ModuleMgr->getFnPointer(moduleName, foreignValue->text);
    else
        fn = g_ModuleMgr->getFnPointer(moduleName, funcName);

    if (!fn)
    {
#ifdef SWAG_DEV_MODE
        SWAG_ASSERT(false);
#endif
        context->report({Fmt(Err(Err0258), funcName.c_str())});
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
    case NativeTypeKind::Rune:
        return &ffi_type_uint32;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        return &ffi_type_sint64;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
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
        OS::atomicSetIfNotNull((void**) &ip->d.pointer, (uint8_t*) ffiGetFuncAddress(&context->jc, ip));
    if (OS::atomicTestNull((void**) &ip->d.pointer))
        return;
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::FuncAttr);
    ffiCall(context, (void*) ip->d.pointer, typeInfoFunc, ip->numVariadicParams);
}

void ByteCodeRun::ffiCall(ByteCodeRunContext* context, void* foreignPtr, TypeInfoFuncAttr* typeInfoFunc, int numCVariadicParams)
{
    const auto& cc = g_CallConv[typeInfoFunc->callConv];

    VectorNative<uint32_t> pushRAParam;
    uint32_t               cptParam = 0;

    // Function call parameters
    ffi_cif cif;
    int     numParameters = (int) typeInfoFunc->parameters.size();
    context->ffiArgs.clear();
    context->ffiArgsValues.clear();
    Register* sp = (Register*) context->sp;

    // Variadic parameters are first on the stack, so need to treat them before
    if (typeInfoFunc->isVariadic())
    {
        // Pointer
        context->ffiArgs.push_back(&ffi_type_pointer);
        context->ffiArgsValues.push_back(&sp->pointer);
        pushRAParam.push_front(cptParam++);
        sp++;

        // Count
        context->ffiArgs.push_back(&ffi_type_uint64);
        context->ffiArgsValues.push_back(&sp->u64);
        pushRAParam.push_front(cptParam++);
        sp++;

        numParameters--;
    }
    else if (typeInfoFunc->isCVariadic())
    {
        numParameters--;
    }

    for (int i = 0; i < numParameters; i++)
    {
        auto typeParam = ((TypeInfoParam*) typeInfoFunc->parameters[i])->typeInfo;
        typeParam      = TypeManager::concreteReferenceType(typeParam);
        auto fromTTi   = ffiFromTypeInfo(typeParam);
        if (!fromTTi)
        {
            context->raiseError(Fmt("ffi failed to convert argument type `%s`", typeParam->name.c_str()));
            return;
        }

        if (typeParam->isNative(NativeTypeKind::String) || typeParam->kind == TypeInfoKind::Slice)
        {
            context->ffiArgs.push_back(fromTTi);
            context->ffiArgsValues.push_back(&sp->pointer);
            pushRAParam.push_front(cptParam++);
            sp++;
            context->ffiArgs.push_back(&ffi_type_uint64);
            context->ffiArgsValues.push_back(&sp->u64);
            pushRAParam.push_front(cptParam++);
            sp++;
        }
        else if (typeParam->isNative(NativeTypeKind::Any) || typeParam->kind == TypeInfoKind::Interface)
        {
            context->ffiArgs.push_back(fromTTi);
            context->ffiArgsValues.push_back(&sp->pointer);
            pushRAParam.push_front(cptParam++);
            sp++;
            context->ffiArgs.push_back(&ffi_type_pointer);
            context->ffiArgsValues.push_back(&sp->pointer);
            pushRAParam.push_front(cptParam++);
            sp++;
        }
        else if (cc.structByRegister && typeParam->kind == TypeInfoKind::Struct && typeParam->sizeOf <= sizeof(void*))
        {
            if (!context->ffiStructByCopyDone)
            {
                context->ffiStructByCopyDone = true;

                context->ffi_StructByCopy1.alignment = 1;
                context->ffi_StructByCopy1.size      = 1;
                context->ffi_StructByCopy1.type      = FFI_TYPE_STRUCT;
                context->ffi_StructByCopy1.elements  = &context->ffi_StructByCopy1T[0];

                context->ffi_StructByCopy2.alignment = 2;
                context->ffi_StructByCopy2.size      = 2;
                context->ffi_StructByCopy2.type      = FFI_TYPE_STRUCT;
                context->ffi_StructByCopy1.elements  = &context->ffi_StructByCopy2T[0];

                context->ffi_StructByCopy4.alignment = 4;
                context->ffi_StructByCopy4.size      = 4;
                context->ffi_StructByCopy4.type      = FFI_TYPE_STRUCT;
                context->ffi_StructByCopy1.elements  = &context->ffi_StructByCopy4T[0];

                context->ffi_StructByCopy8.alignment = 8;
                context->ffi_StructByCopy8.size      = 8;
                context->ffi_StructByCopy8.type      = FFI_TYPE_STRUCT;
                context->ffi_StructByCopy1.elements  = &context->ffi_StructByCopy8T[0];
            }

            switch (typeParam->sizeOf)
            {
            case 1:
                context->ffiArgs.push_back(&context->ffi_StructByCopy1);
                break;
            case 2:
                context->ffiArgs.push_back(&context->ffi_StructByCopy2);
                break;
            case 4:
                context->ffiArgs.push_back(&context->ffi_StructByCopy4);
                break;
            case 8:
                context->ffiArgs.push_back(&context->ffi_StructByCopy8);
                break;
            }

            context->ffiArgsValues.push_back(sp->pointer);
            pushRAParam.push_front(cptParam++);
            sp++;
        }
        else if (typeParam->flags & TYPEINFO_RETURN_BY_COPY)
        {
            context->ffiArgs.push_back(fromTTi);
            context->ffiArgsValues.push_back(&sp->pointer);
            pushRAParam.push_front(cptParam++);
            sp++;
        }
        else
        {
            context->ffiArgs.push_back(fromTTi);
            pushRAParam.push_front(cptParam++);
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
                context->raiseError(Fmt("ffi failed to convert argument type `%s`", typeParam->name.c_str()));
                return;
            }

            sp++;
        }
    }

    numParameters = (int) context->ffiArgs.size();

    // Function return type
    void*     retCopyAddr = nullptr;
    ffi_type* typeResult  = &ffi_type_void;
    auto      returnType  = TypeManager::concreteReferenceType(typeInfoFunc->returnType);
    if (returnType != g_TypeMgr->typeInfoVoid)
    {
        // Special return
        if (returnType->kind == TypeInfoKind::Slice ||
            returnType->kind == TypeInfoKind::Interface ||
            returnType->isNative(NativeTypeKind::Any) ||
            returnType->isNative(NativeTypeKind::String))
        {
            numParameters++;
            context->ffiArgs.push_back(&ffi_type_pointer);
            context->ffiArgsValues.push_back(&context->registersRR);
            retCopyAddr = context->registersRR;
        }
        else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            numParameters++;
            context->ffiArgs.push_back(&ffi_type_pointer);
            context->ffiArgsValues.push_back(context->registersRR);
            retCopyAddr = context->registersRR[0].pointer;
        }
        else
        {
            typeResult = ffiFromTypeInfo(returnType);
            if (!typeResult)
            {
                context->raiseError(Fmt("ffi failed to convert return type `%s`", typeInfoFunc->returnType->getDisplayNameC()));
                return;
            }
        }
    }

    if (typeInfoFunc->isCVariadic())
    {
        for (int i = 0; i < numCVariadicParams; i++)
        {
            context->ffiArgs.push_back(&ffi_type_uint64);
            context->ffiArgsValues.push_back(&sp->u64);
            pushRAParam.push_front(cptParam++);
            sp++;
        }
    }

    // Initialize the cif
    auto ffiArgs = context->ffiArgs.empty() ? nullptr : &context->ffiArgs[0];
    if (typeInfoFunc->isCVariadic())
        ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI, numParameters, numParameters + numCVariadicParams, typeResult, ffiArgs);
    else
        ffi_prep_cif(&cif, FFI_DEFAULT_ABI, numParameters, typeResult, ffiArgs);

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
            context->raiseError(Fmt("ffi failed to get return result of type `%s`", typeInfoFunc->returnType->getDisplayNameC()));
            return;
        }
    }

    static int ii = 0;
    ii++;
    if (ii == 0x1935)
        int a = 0;

    // Make the call
    //ffi_call(&cif, FFI_FN(foreignPtr), resultPtr, context->ffiArgsValues.empty() ? nullptr : &context->ffiArgsValues[0]);

    if (!g_PP.concat.firstBucket)
    {
        g_PP.concat.init();
        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);
        auto const buffer                 = VirtualAlloc(nullptr, systemInfo.dwPageSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        g_PP.concat.firstBucket->datas    = (uint8_t*) buffer;
        g_PP.concat.currentSP             = g_PP.concat.firstBucket->datas;
        g_PP.concat.firstBucket->capacity = systemInfo.dwPageSize;
    }

    uint32_t stackSize = (uint32_t) max(cc.byRegisterCount, pushRAParam.size()) * sizeof(void*);
    if (typeInfoFunc->returnByCopy())
        stackSize += sizeof(void*);
    stackSize &= 0xFFFFFFFFFFFFFFF0;
    stackSize += 16;

    auto startOffset = g_PP.concat.currentSP - g_PP.concat.firstBucket->datas;
    g_PP.concat.addU8(0x57);                // push rdi
    g_PP.concat.addString3("\x48\x89\xF9"); // mov rcx, rdi
    g_PP.concat.addString3("\x48\x81\xEC"); // sub rsp, stackSize
    g_PP.concat.addU32(stackSize);
    g_PP.concat.addString2("\x48\xBF"); // move rdi, sp
    g_PP.concat.addU64((uint64_t) context->sp);
    BackendX64::emitCallParameters(g_PP, 0, typeInfoFunc, pushRAParam, retCopyAddr);
    g_PP.concat.addString3("\x48\x89\xCF"); // mov rdi, rcx
    g_PP.concat.addString2("\x48\xB8");     // move rax, foreignPtr
    g_PP.concat.addU64((uint64_t) foreignPtr);
    g_PP.concat.addString2("\xff\xd0"); // call rax

    if (returnType != g_TypeMgr->typeInfoVoid && !retCopyAddr)
    {
        g_PP.concat.addString2("\x48\xB9");
        g_PP.concat.addU64((uint64_t) context->registersRR); // move rcx, context->registersRR

        if (cc.useReturnByRegisterFloat && returnType->isNativeFloat())
            g_PP.concat.addString4("\xF2\x0F\x11\x01"); // movsd [rcx], xmm0
        else
            g_PP.concat.addString3("\x48\x89\x01"); // mov [rcx], rax
    }
    g_PP.concat.addString3("\x48\x81\xC4"); // add rsp, stackSize
    g_PP.concat.addU32(stackSize);
    g_PP.concat.addU8(0x5F); // pop rdi
    g_PP.concat.addU8(0xC3); // ret

    typedef void (*funcPtr)();
    auto ptr = (funcPtr) (g_PP.concat.firstBucket->datas + startOffset);
    ptr();

    g_PP.concat.currentSP = (uint8_t*) ptr;
}
