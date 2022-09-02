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
    int numParameters = (int) typeInfoFunc->parameters.size();

    // Variadic parameters are first on the stack, so need to treat them before
    if (typeInfoFunc->isVariadic())
    {
        pushRAParam.push_front(cptParam++);
        pushRAParam.push_front(cptParam++);
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

        if (typeParam->isNative(NativeTypeKind::String) ||
            typeParam->isNative(NativeTypeKind::Any) ||
            typeParam->kind == TypeInfoKind::Slice ||
            typeParam->kind == TypeInfoKind::Interface)
        {
            pushRAParam.push_front(cptParam++);
            pushRAParam.push_front(cptParam++);
        }
        else
        {
            pushRAParam.push_front(cptParam++);
        }
    }

    // Function return type
    void* retCopyAddr = nullptr;
    auto  returnType  = TypeManager::concreteReferenceType(typeInfoFunc->returnType);
    if (returnType != g_TypeMgr->typeInfoVoid)
    {
        // Special return
        if (returnType->kind == TypeInfoKind::Slice ||
            returnType->kind == TypeInfoKind::Interface ||
            returnType->isNative(NativeTypeKind::Any) ||
            returnType->isNative(NativeTypeKind::String))
        {
            retCopyAddr = context->registersRR;
        }
        else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            retCopyAddr = context->registersRR[0].pointer;
        }
    }

    if (typeInfoFunc->isCVariadic())
    {
        for (int i = 0; i < numCVariadicParams; i++)
        {
            pushRAParam.push_front(cptParam++);
        }
    }

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
