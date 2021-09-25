#include "pch.h"
#include "BackendLLVM.h"
#include "Module.h"
#include "TypeManager.h"
#include "Workspace.h"

void BackendLLVM::getLocalCallParameters(const BuildParameters&      buildParameters,
                                         llvm::AllocaInst*           allocR,
                                         llvm::AllocaInst*           allocRR,
                                         llvm::AllocaInst*           allocT,
                                         VectorNative<llvm::Value*>& params,
                                         TypeInfoFuncAttr*           typeFuncBC,
                                         VectorNative<uint32_t>&     pushRAParams,
                                         const vector<llvm::Value*>& values)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    int   popRAidx        = (int) pushRAParams.size() - 1;
    int   numCallParams   = (int) typeFuncBC->parameters.size();

    // Return value
    // Normal user case
    if (allocRR)
    {
        for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
        {
            auto r0 = GEP_I32(allocRR, j);
            params.push_back(r0);
        }
    }

    // Special case when calling an internal function
    else
    {
        for (int j = 0; j < typeFuncBC->numReturnRegisters(); j++)
        {
            auto index = pushRAParams[popRAidx--];
            if (index == UINT32_MAX)
            {
                auto v0 = values[popRAidx + 1];
                SWAG_ASSERT(v0);
                params.push_back(v0);
            }
            else
            {
                auto r0 = GEP_I32(allocR, index);
                params.push_back(r0);
            }
        }
    }

    // Two registers for variadics first
    if (typeFuncBC->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        auto index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        auto r0 = GEP_I32(allocR, index);
        params.push_back(builder.CreateLoad(r0));
        index = pushRAParams[popRAidx--];
        SWAG_ASSERT(index != UINT32_MAX);
        r0 = GEP_I32(allocR, index);
        params.push_back(builder.CreateLoad(r0));
        numCallParams--;
    }
    else if (typeFuncBC->flags & TYPEINFO_C_VARIADIC)
    {
        numCallParams--;
    }

    for (int idxCall = 0; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
        typeParam      = TypeManager::concreteReferenceType(typeParam);
        for (int j = 0; j < typeParam->numRegisters(); j++)
        {
            auto index = pushRAParams[popRAidx--];

            // By value
            if (passByValue(typeParam))
            {
                llvm::Type* ty;
                swagTypeToLLVMType(buildParameters, module, typeParam, &ty);
                if (index == UINT32_MAX)
                {
                    auto v0 = values[popRAidx + 1];
                    SWAG_ASSERT(v0);
                    params.push_back(v0);
                }
                else
                {
                    auto r0 = builder.CreatePointerCast(GEP_I32(allocR, index), ty->getPointerTo());
                    params.push_back(builder.CreateLoad(r0));
                }

                continue;
            }

            // By register value. If we have a value and not a register, store the value in a temporary register
            if (index == UINT32_MAX)
            {
                auto v0 = values[popRAidx + 1];
                SWAG_ASSERT(v0);
                if (allocT)
                {
                    if (v0->getType()->isPointerTy())
                        v0 = builder.CreatePtrToInt(v0, builder.getInt64Ty());
                    else if (v0->getType()->isIntegerTy())
                        v0 = builder.CreateIntCast(v0, builder.getInt64Ty(), false);
                    params.push_back(v0);
                }
                else
                {
                    params.push_back(v0);
                }
            }

            // By register value.
            else
            {
                auto v0 = builder.CreateLoad(GEP_I32(allocR, index));
                params.push_back(v0);
            }
        }
    }

    // C variadic arguments
    if (typeFuncBC->flags & TYPEINFO_C_VARIADIC)
    {
        auto numVariadics = popRAidx + 1;
        for (int idxCall = 0; idxCall < numVariadics; idxCall++)
        {
            auto index = pushRAParams[popRAidx--];
            auto v0    = builder.CreateLoad(GEP_I32(allocR, index));
            params.push_back(v0);
        }
    }
}

llvm::FunctionType* BackendLLVM::createFunctionTypeLocal(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFuncBC)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& context         = *pp.context;

    if (!typeFuncBC)
    {
        VectorNative<llvm::Type*> params;
        return llvm::FunctionType::get(llvm::Type::getVoidTy(context), {params.begin(), params.end()}, false);
    }

    // Already done ?
    auto it = pp.mapFctTypeInternal.find(typeFuncBC);
    if (it != pp.mapFctTypeInternal.end())
        return it->second;

    // Registers to get the result
    VectorNative<llvm::Type*> params;
    for (int i = 0; i < typeFuncBC->numReturnRegisters(); i++)
        params.push_back(llvm::Type::getInt64PtrTy(context));

    // Variadics first
    int  numParams = (int) typeFuncBC->parameters.size();
    bool isVarArg  = false;
    if (typeFuncBC->flags & (TYPEINFO_VARIADIC | TYPEINFO_TYPED_VARIADIC))
    {
        params.push_back(llvm::Type::getInt64Ty(context));
        params.push_back(llvm::Type::getInt64Ty(context));
        numParams--;
    }
    else if (typeFuncBC->flags & TYPEINFO_C_VARIADIC)
    {
        isVarArg = true;
        numParams--;
    }

    for (int i = 0; i < numParams; i++)
    {
        auto param     = typeFuncBC->parameters[i];
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (passByValue(typeParam))
        {
            llvm::Type* ty;
            swagTypeToLLVMType(buildParameters, module, typeParam, &ty);
            params.push_back(ty);
        }
        else
        {
            for (int r = 0; r < typeParam->numRegisters(); r++)
                params.push_back(llvm::Type::getInt64Ty(context));
        }
    }

    auto result                       = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {params.begin(), params.end()}, isVarArg);
    pp.mapFctTypeInternal[typeFuncBC] = result;
    return result;
}

bool BackendLLVM::storeLocalParam(const BuildParameters& buildParameters, llvm::Function* func, TypeInfoFuncAttr* typeFunc, int idx, llvm::Value* r0, int sizeOf)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    auto param  = typeFunc->registerIdxToType(idx);
    auto offArg = idx + typeFunc->numReturnRegisters();
    auto arg    = func->getArg(offArg);

    if (passByValue(param))
    {
        // This can be casted to an integer
        if (sizeOf)
        {
            builder.CreateStore(builder.CreateIntCast(arg, builder.getInt64Ty(), false), r0);
        }

        // Real type
        else
        {
            llvm::Type* ty = nullptr;
            SWAG_CHECK(swagTypeToLLVMType(buildParameters, module, param, &ty));
            r0 = builder.CreatePointerCast(r0, ty->getPointerTo());
            builder.CreateStore(arg, r0);
        }
    }
    else
    {
        SWAG_ASSERT(sizeOf == 0);
        builder.CreateStore(arg, r0);
    }

    return true;
}

void BackendLLVM::localCall(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const char* name, const vector<uint32_t>& regs, const vector<llvm::Value*>& values)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;
    auto& modu            = *pp.module;

    auto typeFuncBC = g_Workspace->runtimeModule->getRuntimeTypeFct(name);
    auto FT         = createFunctionTypeLocal(buildParameters, typeFuncBC);

    // Invert regs
    VectorNative<uint32_t> pushRAParams;
    for (int i = (int) regs.size() - 1; i >= 0; i--)
        pushRAParams.push_back(regs[i]);
    vector<llvm::Value*> pushVParams;
    for (int i = (int) values.size() - 1; i >= 0; i--)
        pushVParams.push_back(values[i]);

    VectorNative<llvm::Value*> fctParams;
    getLocalCallParameters(buildParameters, allocR, nullptr, allocT, fctParams, typeFuncBC, pushRAParams, pushVParams);

    builder.CreateCall(modu.getOrInsertFunction(name, FT), {fctParams.begin(), fctParams.end()});
}