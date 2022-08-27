#include "pch.h"
#include "BackendLLVM.h"
#include "Module.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "BackendLLVM_Macros.h"

void BackendLLVM::getLocalCallParameters(const BuildParameters&      buildParameters,
                                         llvm::AllocaInst*           allocR,
                                         llvm::AllocaInst*           allocRR,
                                         llvm::AllocaInst*           allocT,
                                         VectorNative<llvm::Value*>& params,
                                         TypeInfoFuncAttr*           typeFuncBC,
                                         VectorNative<uint32_t>&     pushRAParams,
                                         const vector<llvm::Value*>& values,
                                         bool                        closureToLambda)
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

    int idxFirst = 0;
    if (typeFuncBC->isClosure() && closureToLambda)
    {
        idxFirst = 1;
        popRAidx--;
    }

    for (int idxCall = idxFirst; idxCall < numCallParams; idxCall++)
    {
        auto typeParam = typeFuncBC->parameters[idxCall]->typeInfo;
        typeParam      = TypeManager::concreteReferenceType(typeParam);
        for (int j = 0; j < typeParam->numRegisters(); j++)
        {
            auto index = pushRAParams[popRAidx--];

            // By value
            if (passByValue(typeParam))
            {
                auto ty = swagTypeToLLVMType(buildParameters, module, typeParam);
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

llvm::FunctionType* BackendLLVM::createFunctionTypeLocal(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFuncBC, bool closureToLambda)
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
    if (closureToLambda)
    {
        auto it = pp.mapFctTypeInternalClosure.find(typeFuncBC);
        if (it != pp.mapFctTypeInternalClosure.end())
            return it->second;
    }
    else
    {
        auto it = pp.mapFctTypeInternal.find(typeFuncBC);
        if (it != pp.mapFctTypeInternal.end())
            return it->second;
    }

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

    int startIdx = 0;
    if (typeFuncBC->isClosure() && closureToLambda)
        startIdx = 1;

    for (int i = startIdx; i < numParams; i++)
    {
        auto param     = typeFuncBC->parameters[i];
        auto typeParam = TypeManager::concreteReferenceType(param->typeInfo);
        if (passByValue(typeParam))
        {
            auto ty = swagTypeToLLVMType(buildParameters, module, typeParam);
            params.push_back(ty);
        }
        else
        {
            for (int r = 0; r < typeParam->numRegisters(); r++)
                params.push_back(llvm::Type::getInt64Ty(context));
        }
    }

    auto result = llvm::FunctionType::get(llvm::Type::getVoidTy(context), {params.begin(), params.end()}, isVarArg);
    if (closureToLambda)
        pp.mapFctTypeInternalClosure[typeFuncBC] = result;
    else
        pp.mapFctTypeInternal[typeFuncBC] = result;
    return result;
}

bool BackendLLVM::storeLocalParam(llvm::LLVMContext& context, const BuildParameters& buildParameters, llvm::Function* func, TypeInfoFuncAttr* typeFunc, int idx, llvm::Value* r0, int sizeOf, uint64_t toAdd, int deRefSize)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = *perThread[ct][precompileIndex];
    auto& builder         = *pp.builder;

    auto param = typeFunc->registerIdxToType(idx);
    auto arg   = func->getArg(idx);

    // First two parameters are occupied by the variadic slice
    if (typeFunc->isVariadic())
        idx += 2;

    if (toAdd || deRefSize)
    {
        llvm::Value* ra;
        if (passByValue(param))
            ra = builder.CreateInBoundsGEP(TO_PTR_I8(arg), builder.getInt64(toAdd));
        else
        {
            ra = builder.CreateIntToPtr(arg, builder.getInt8PtrTy());
            ra = builder.CreateInBoundsGEP(ra, builder.getInt64(toAdd));
        }
        if (deRefSize)
        {
            llvm::Value* v1;
            switch (deRefSize)
            {
            case 1:
                v1 = builder.CreateLoad(TO_PTR_I8(ra));
                ra = builder.CreateIntCast(v1, builder.getInt64Ty(), false);
                break;
            case 2:
                v1 = builder.CreateLoad(TO_PTR_I16(ra));
                ra = builder.CreateIntCast(v1, builder.getInt64Ty(), false);
                break;
            case 4:
                v1 = builder.CreateLoad(TO_PTR_I32(ra));
                ra = builder.CreateIntCast(v1, builder.getInt64Ty(), false);
                break;
            case 8:
                ra = builder.CreateLoad(TO_PTR_I64(ra));
                break;
            }

            builder.CreateStore(ra, r0);
        }
        else
        {
            builder.CreateStore(ra, TO_PTR_PTR_I8(r0));
        }
    }
    else if (passByValue(param))
    {
        // By convention, all remaining bits should be zero
        if (param->isNativeIntegerSigned() && param->sizeOf < 8)
            builder.CreateStore(builder.CreateIntCast(arg, builder.getInt64Ty(), true), r0);
        else if (param->isNativeIntegerUnsigned() && param->sizeOf < 8)
            builder.CreateStore(builder.CreateIntCast(arg, builder.getInt64Ty(), false), r0);

        // This can be casted to an integer
        else if (sizeOf)
        {
            if (sizeOf == sizeof(void*))
                builder.CreateStore(arg, r0);
            else
                builder.CreateStore(builder.CreateIntCast(arg, builder.getInt64Ty(), false), r0);
        }

        // Real type
        else
        {
            auto ty = swagTypeToLLVMType(buildParameters, module, param);
            r0      = builder.CreatePointerCast(r0, ty->getPointerTo());
            builder.CreateStore(arg, r0);
        }
    }
    else
    {
        if (arg->getType()->isPointerTy())
            builder.CreateStore(arg, builder.CreatePointerCast(r0, arg->getType()->getPointerTo()));
        else
            builder.CreateStore(arg, r0);
    }

    return true;
}
