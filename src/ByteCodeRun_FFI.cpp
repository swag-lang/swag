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
#include "JobThread.h"

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
            // Not sure why. Probably an hidden bug somewhere.
            // Give this other tries.
            bool lastChance = false;
            for (int i = 0; i < 10; i++)
            {
                this_thread::sleep_for(100ms);
                if (g_ModuleMgr->loadModule(moduleName))
                {
                    lastChance = true;
                    break;
                }
            }

            if (!lastChance)
            {
#ifdef SWAG_DEV_MODE
                SWAG_ASSERT(false);
#endif
                Diagnostic diag{nodeFunc, Fmt(Err(Err0592), moduleName.c_str(), funcName.c_str(), g_ModuleMgr->loadModuleError.c_str())};
                diag.showSourceCode = false;
                context->report(diag);
                return nullptr;
            }
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
    {
        OS::atomicSetIfNotNull((void**) &ip->d.pointer, (uint8_t*) ffiGetFuncAddress(&context->jc, ip));
        if (OS::atomicTestNull((void**) &ip->d.pointer))
            return;
    }

    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>((TypeInfo*) ip->b.pointer, TypeInfoKind::FuncAttr);
    ffiCall(context, ip, (void*) ip->d.pointer, typeInfoFunc, ip->numVariadicParams);
}

void ByteCodeRun::ffiCall(ByteCodeRunContext* context, ByteCodeInstruction* ip, void* foreignPtr, TypeInfoFuncAttr* typeInfoFunc, int numCVariadicParams)
{
    uint32_t cptParam = 0;

    // Function call parameters
    context->ffiPushRAParam.clear();
    int numParameters = (int) typeInfoFunc->parameters.size();

    // Variadic parameters are first on the stack, so need to treat them before
    if (typeInfoFunc->isVariadic())
    {
        context->ffiPushRAParam.push_front(cptParam++);
        context->ffiPushRAParam.push_front(cptParam++);
        numParameters--;
    }
    else if (typeInfoFunc->isCVariadic())
    {
        numParameters--;
    }

    for (int i = 0; i < numParameters; i++)
    {
        auto typeParam = ((TypeInfoParam*) typeInfoFunc->parameters[i])->typeInfo;
        typeParam      = TypeManager::concreteType(typeParam);

        if (typeParam->isSlice() ||
            typeParam->isInterface() ||
            typeParam->isAny() ||
            typeParam->isString())
        {
            context->ffiPushRAParam.push_front(cptParam++);
            context->ffiPushRAParam.push_front(cptParam++);
        }
        else
        {
            context->ffiPushRAParam.push_front(cptParam++);
        }
    }

    // Function return type
    void* retCopyAddr = nullptr;
    auto  returnType  = typeInfoFunc->concreteReturnType();
    if (returnType != g_TypeMgr->typeInfoVoid)
    {
        if (returnType->isSlice() ||
            returnType->isInterface() ||
            returnType->isAny() ||
            returnType->isString())
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
            context->ffiPushRAParam.push_front(cptParam++);
        }
    }

#ifdef SWAG_STATS
    if (g_CommandLine.profile)
    {
        auto now = OS::timerNow();
        context->bc->profileCumTime += now - context->bc->profileStart;
        context->bc->profileStart = now;
    }
#endif

    OS::ffi(context, foreignPtr, typeInfoFunc, context->ffiPushRAParam, retCopyAddr);

#ifdef SWAG_STATS
    if (g_CommandLine.profile)
    {
        auto now = OS::timerNow();
        context->bc->profileCumTime += now - context->bc->profileStart;
        context->bc->profileFFI += now - context->bc->profileStart;
        context->bc->ffiProfile[ip->a.pointer] += now - context->bc->profileStart;
        context->bc->profileStart = now;
    }
#endif
}
