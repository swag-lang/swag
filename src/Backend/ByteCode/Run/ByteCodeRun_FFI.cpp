#include "pch.h"

#include "Backend/ByteCode/ByteCode.h"
#include "ByteCodeStack.h"
#include "Os/Os.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/ModuleManager.h"

void* ByteCodeRun::ffiGetFuncAddress(JobContext* context, AstFuncDecl* nodeFunc)
{
    SWAG_ASSERT(nodeFunc->resolvedSymbolOverload());
    SWAG_ASSERT(nodeFunc->resolvedSymbolOverload()->typeInfo);
    const auto  typeFunc = castTypeInfo<TypeInfoFuncAttr>(nodeFunc->resolvedSymbolOverload()->typeInfo, TypeInfoKind::FuncAttr);
    const auto& funcName = nodeFunc->token.text;

    // Load module if specified
    const auto moduleName = ModuleManager::getForeignModuleName(nodeFunc);
    if (!g_ModuleMgr->loadModule(moduleName))
    {
        // Perhaps the module is dependent on another module, so we need to be sure that our dependencies are
        // all loaded : load all, from last to first (dependencies are added in reverse order, latest first)
        SWAG_ASSERT(context->sourceFile);
        const auto module = context->sourceFile->module;
        for (const auto& dep : module->moduleDependencies)
            (void) g_ModuleMgr->loadModule(dep->name);

        // Then try again
        g_ModuleMgr->resetFailedModule(moduleName);
        if (!g_ModuleMgr->loadModule(moduleName))
        {
            // Not sure why. Probably an hidden problem somewhere.
            // Give this other tries.
            bool lastChance = false;
            for (uint32_t i = 0; i < 10; i++)
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(100ms);
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
                Diagnostic err{nodeFunc, formErr(Err0746, moduleName.cstr(), funcName.cstr())};
                if (!g_ModuleMgr->loadModuleError.empty())
                    err.addNote(Diagnostic::note(g_ModuleMgr->loadModuleError));
                context->report(err);
                return nullptr;
            }
        }
    }

    void*      fn           = nullptr;
    const auto foreignValue = typeFunc->attributes.getValue(g_LangSpec->name_Swag_Foreign, g_LangSpec->name_function);
    if (foreignValue && !foreignValue->text.empty())
        fn = g_ModuleMgr->getFnPointer(moduleName, foreignValue->text);
    else
        fn = g_ModuleMgr->getFnPointer(moduleName, funcName);

    if (!fn)
    {
#ifdef SWAG_DEV_MODE
        SWAG_ASSERT(false);
#endif
        context->report({formErr(Err0713, funcName.cstr())});
        return nullptr;
    }

    return fn;
}

void ByteCodeRun::ffiCallTrace(ByteCodeRunContext* context, const ByteCodeInstruction* ip)
{
    g_ByteCodeStackTrace->push(context);
    ffiCall(context, ip);
    g_ByteCodeStackTrace->pop();
}

void ByteCodeRun::ffiCall(ByteCodeRunContext* context, const ByteCodeInstruction* ip)
{
    const auto nodeFunc = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(ip->a.pointer), AstNodeKind::FuncDecl);

    if (OS::atomicTestNull(&nodeFunc->ffiAddress))
    {
        OS::atomicSetIfNotNull(&nodeFunc->ffiAddress, ffiGetFuncAddress(&context->jc, nodeFunc));
        if (OS::atomicTestNull(&nodeFunc->ffiAddress))
            return;
    }

    const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(reinterpret_cast<TypeInfo*>(ip->b.pointer), TypeInfoKind::FuncAttr);
    ffiCall(context, ip, nodeFunc->ffiAddress, typeInfoFunc, ip->numVariadicParams);
}

void ByteCodeRun::ffiCall(ByteCodeRunContext* context, [[maybe_unused]] const ByteCodeInstruction* ip, void* foreignPtr, TypeInfoFuncAttr* typeInfoFunc, int numCVariadicParams)
{
    uint32_t cptParam = 0;

    // Function call parameters
    context->ffiPushRAParam.clear();
    int numParameters = static_cast<int>(typeInfoFunc->parameters.size());

    // Variadic parameters are first on the stack, so need to treat them before
    if (typeInfoFunc->isFctVariadic())
    {
        context->ffiPushRAParam.push_front(cptParam++);
        context->ffiPushRAParam.push_front(cptParam++);
        numParameters--;
    }
    else if (typeInfoFunc->isFctCVariadic())
    {
        numParameters--;
    }

    for (int i = 0; i < numParameters; i++)
    {
        auto typeParam = typeInfoFunc->parameters[i]->typeInfo;
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
    if (CallConv::returnByAddress(typeInfoFunc) && !CallConv::returnByStackAddress(typeInfoFunc))
        retCopyAddr = context->registersRR;
    else if (CallConv::returnByStackAddress(typeInfoFunc))
        retCopyAddr = context->registersRR[0].pointer;

    if (typeInfoFunc->isFctCVariadic())
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

        OS::ffi(context, foreignPtr, typeInfoFunc, context->ffiPushRAParam, retCopyAddr);

        now = OS::timerNow();
        context->bc->profileCumTime += now - context->bc->profileStart;
        context->bc->profileFFI += now - context->bc->profileStart;

        if (ByteCode::isForeignCall(ip))
        {
            const auto funcDecl = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(ip->a.pointer), AstNodeKind::FuncDecl);
            const auto callName = funcDecl->getCallName();
            context->bc->ffiProfile[callName].count += 1;
            context->bc->ffiProfile[callName].cum += now - context->bc->profileStart;
        }

        context->bc->profileStart = now;
    }
    else
    {
        OS::ffi(context, foreignPtr, typeInfoFunc, context->ffiPushRAParam, retCopyAddr);
    }
#else
    OS::ffi(context, foreignPtr, typeInfoFunc, context->ffiPushRAParam, retCopyAddr);
#endif
}
