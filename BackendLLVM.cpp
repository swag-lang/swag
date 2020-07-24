#include "pch.h"
#include "BackendLLVM.h"
#include "Workspace.h"
#include "OS.h"
#include "BackendSetupWin32.h"
#include "BackendLinkerWin32.h"

void BackendLLVM::setup()
{
}

bool BackendLLVM::createRuntime(const BuildParameters& buildParameters)
{
    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;

    auto& pp      = perThread[ct][precompileIndex];
    auto& context = *pp.context;
    auto& modu    = *pp.module;

    // swag_interface_t
    {
        llvm::Type* members[] = {
            llvm::Type::getInt8PtrTy(context),
            llvm::Type::getInt8PtrTy(context)};
        pp.interfaceTy = llvm::StructType::create(context, members, "swag_interface_t");
    }

    // swag_context_t
    {
        llvm::Type* members[] = {
            pp.interfaceTy};
        pp.contextTy = llvm::StructType::create(context, members, "swag_context_t");
        SWAG_ASSERT(pp.contextTy->isSized());
    }

    // swag_slice_t
    {
        llvm::Type* members[] = {
            llvm::Type::getInt8PtrTy(context),
            llvm::Type::getInt64Ty(context),
        };
        pp.sliceTy = llvm::StructType::create(context, members, "swag_slice_t");
        SWAG_ASSERT(pp.sliceTy->isSized());
    }

    // swag_alloctor_t
    {
        llvm::Type* params[] = {
            llvm::Type::getInt64PtrTy(context),
            llvm::Type::getInt64PtrTy(context),
        };
        pp.allocatorTy = llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false);
    }

    // swag_alloctor_t
    {
        llvm::Type* params[] = {
            llvm::Type::getInt8PtrTy(context),
        };
        pp.bytecodeRunTy = llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, true);
    }

    // swag_process_infos_t
    {
        llvm::Type* members[] = {
            pp.sliceTy,
            llvm::Type::getInt32Ty(context),
            pp.contextTy->getPointerTo(),
            pp.bytecodeRunTy->getPointerTo(),
        };
        pp.processInfosTy = llvm::StructType::create(context, members, "swag_process_infos_t");
        SWAG_ASSERT(pp.processInfosTy->isSized());
    }

    // mainContext
    if (precompileIndex == 0)
    {
        pp.mainContext       = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantAggregateZero::get(pp.contextTy), "mainContext");
        pp.defaultAllocTable = new llvm::GlobalVariable(modu, pp.allocatorTy->getPointerTo(), false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantPointerNull::get(pp.allocatorTy->getPointerTo()), "defaultAllocTable");
        pp.processInfos      = new llvm::GlobalVariable(modu, pp.processInfosTy, false, llvm::GlobalValue::ExternalLinkage, llvm::ConstantAggregateZero::get(pp.processInfosTy), "processInfos");
    }
    else
    {
        pp.mainContext       = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::InternalLinkage, nullptr, "mainContext");
        pp.defaultAllocTable = new llvm::GlobalVariable(modu, pp.allocatorTy->getPointerTo(), false, llvm::GlobalValue::InternalLinkage, nullptr, "defaultAllocTable");
        pp.processInfos      = new llvm::GlobalVariable(modu, pp.processInfosTy, false, llvm::GlobalValue::InternalLinkage, nullptr, "processInfos");
    }

    // Runtime functions
    {
        {
            modu.getOrInsertFunction("swag_runtime_tlsAlloc", llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt32Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_tlsGetValue", llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt32Ty(context),
                llvm::Type::getInt8PtrTy(context),
            };
            modu.getOrInsertFunction("swag_runtime_tlsSetValue", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt32Ty(context),
                llvm::Type::getInt8PtrTy(context)->getPointerTo(),
            };
            modu.getOrInsertFunction("swag_runtime_convertArgcArgv", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
            };
            modu.getOrInsertFunction("swag_runtime_loadDynamicLibrary", llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt64Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_malloc", llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
            };
            modu.getOrInsertFunction("swag_runtime_free", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt64Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_realloc", llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt64Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_memcpy", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt32Ty(context),
                llvm::Type::getInt64Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_memset", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt64Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_memcmp", llvm::FunctionType::get(llvm::Type::getInt32Ty(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt32Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_strcmp", llvm::FunctionType::get(llvm::Type::getInt8Ty(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt32Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_print_n", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8PtrTy(context),
            };
            modu.getOrInsertFunction("swag_runtime_print", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt64Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_print_i64", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getDoubleTy(context),
            };
            modu.getOrInsertFunction("swag_runtime_print_f64", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }

        {
            llvm::Type* params[] = {
                llvm::Type::getInt8Ty(context),
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt32Ty(context),
                llvm::Type::getInt8PtrTy(context),
                llvm::Type::getInt8Ty(context),
            };
            modu.getOrInsertFunction("swag_runtime_assert", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }
    }

    // Cache things
    pp.cst0_i8  = llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 0);
    pp.cst1_i8  = llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 1);
    pp.cst0_i16 = llvm::ConstantInt::get(llvm::Type::getInt16Ty(context), 0);
    pp.cst0_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    pp.cst1_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 1);
    pp.cst2_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 2);
    pp.cst3_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 3);
    pp.cst0_i64 = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
    pp.cst0_f32 = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 0);
    pp.cst0_f64 = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0);
    pp.cst_null = llvm::ConstantPointerNull::get(llvm::Type::getInt64PtrTy(context));

    return true;
}

JobResult BackendLLVM::preCompile(const BuildParameters& buildParameters, Job* ownerJob)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];

    if (pp.pass == BackendPreCompilePass::Init)
    {
        pp.pass = BackendPreCompilePass::FunctionBodies;

        SWAG_ASSERT(!module->name.empty());
        pp.filename = format("%s%d", module->name.c_str(), precompileIndex);
        pp.filename += buildParameters.postFix;
        pp.filename += ".obj";

        pp.context = new llvm::LLVMContext();
        pp.module  = new llvm::Module(perThread[ct][precompileIndex].filename.c_str(), *perThread[ct][precompileIndex].context);
        pp.builder = new llvm::IRBuilder<>(*perThread[ct][precompileIndex].context);

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', llvm backend, generating files", perThread[ct][precompileIndex].filename.c_str(), module->byteCodeTestFunc.size()));

        createRuntime(buildParameters);
        emitDataSegment(buildParameters, &module->bssSegment);
        emitDataSegment(buildParameters, &module->mutableSegment);
        emitDataSegment(buildParameters, &module->constantSegment);
    }

    if (pp.pass == BackendPreCompilePass::FunctionBodies)
    {
        pp.pass = BackendPreCompilePass::End;
        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlivePending;
    }

    if (pp.pass == BackendPreCompilePass::End)
    {
        if (precompileIndex == 0)
        {
            emitInitDataSeg(buildParameters);
            emitInitConstantSeg(buildParameters);
            emitGlobalInit(buildParameters);
            emitGlobalDrop(buildParameters);
            emitMain(buildParameters);
        }

        // Output file
        generateObjFile(buildParameters);
    }

    return JobResult::ReleaseJob;
}

bool BackendLLVM::generateObjFile(const BuildParameters& buildParameters)
{
    int   ct              = buildParameters.compileType;
    int   precompileIndex = buildParameters.precompileIndex;
    auto& pp              = perThread[ct][precompileIndex];
    auto& modu            = *pp.module;

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    modu.setTargetTriple(targetTriple);

    std::string Error;
    auto        target = llvm::TargetRegistry::lookupTarget(targetTriple, Error);

    auto CPU      = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto                RM               = llvm::Optional<llvm::Reloc::Model>();
    auto                theTargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);
    modu.setDataLayout(theTargetMachine->createDataLayout());

    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    auto path       = targetPath + "/" + pp.filename;

    auto                 filename = path;
    std::error_code      EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    llvm::legacy::PassManager llvmPass;
    llvm::PassManagerBuilder  pmb;

    // Optimize passes
    int optimLevel = 0;
    if (g_CommandLine.optim)
        optimLevel = g_CommandLine.optim;
    else if (!g_CommandLine.debug)
        optimLevel = buildParameters.target.backendOptimizeLevel;
    if (optimLevel)
    {
        pmb.OptLevel  = optimLevel;
        pmb.SizeLevel = 0;
        pmb.Inliner   = llvm::createFunctionInliningPass(pmb.OptLevel, pmb.SizeLevel, true);
        pmb.populateModulePassManager(llvmPass);
    }

    // Generate obj file pass
    theTargetMachine->addPassesToEmitFile(llvmPass, dest, nullptr, llvm::CGFT_ObjectFile);

    // Output IR code
    if (buildParameters.target.backendLLVM.outputIR)
    {
        auto                 filenameIR = path;
        llvm::raw_fd_ostream destFileIR(filename + ".ir", EC, llvm::sys::fs::OF_None);
        modu.print(destFileIR, nullptr);
        destFileIR.flush();
        destFileIR.close();
    }

    llvmPass.run(modu);
    dest.flush();
    dest.close();

    // Output IR code
    if (buildParameters.target.backendLLVM.outputIR)
    {
        auto                 filenameIR = path;
        llvm::raw_fd_ostream destFileIR(filename + ".ir", EC, llvm::sys::fs::OF_None);
        modu.print(destFileIR, nullptr);
        destFileIR.flush();
        destFileIR.close();
    }

    return true;
}

bool BackendLLVM::compile(const BuildParameters& buildParameters)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return true;

    int ct = buildParameters.compileType;

    Utf8 linkArguments;
    BackendLinkerWin32::getArguments(buildParameters, module, linkArguments);

    // Add all object files
    auto targetPath = BackendLinkerWin32::getCacheFolder(buildParameters);
    for (auto i = 0; i < numPreCompileBuffers; i++)
    {
        SWAG_ASSERT(!perThread[ct][i].filename.empty());
        auto path = targetPath + "/" + perThread[ct][i].filename.c_str();
        linkArguments += path + " ";
    }

    bool     verbose   = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;
    uint32_t numErrors = 0;
    auto     cmdLine   = "\"" + BackendSetupWin32::linkerPath + BackendSetupWin32::linkerExe + "\" " + linkArguments;
    SWAG_CHECK(OS::doProcess(cmdLine, BackendSetupWin32::linkerPath, verbose, numErrors, LogColor::DarkCyan, "CL "));

    g_Workspace.numErrors += numErrors;
    module->numErrors += numErrors;
    return numErrors == 0;
}
