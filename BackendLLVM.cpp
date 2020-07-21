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
        pp.mainContext       = new llvm::GlobalVariable(modu, pp.contextTy, false, llvm::GlobalValue::InternalLinkage, llvm::ConstantAggregateZero::get(pp.contextTy), "mainContext");
        pp.defaultAllocTable = new llvm::GlobalVariable(modu, pp.allocatorTy->getPointerTo(), false, llvm::GlobalValue::InternalLinkage, llvm::ConstantPointerNull::get(pp.allocatorTy->getPointerTo()), "defaultAllocTable");
        pp.processInfos      = new llvm::GlobalVariable(modu, pp.processInfosTy, false, llvm::GlobalValue::InternalLinkage, llvm::ConstantAggregateZero::get(pp.processInfosTy), "processInfos");
    }

    // Runtime functions
    {
        {
            modu.getOrInsertFunction("swag_runtime_tlsAlloc", llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false));
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
            };
            modu.getOrInsertFunction("swag_runtime_print_n", llvm::FunctionType::get(llvm::Type::getVoidTy(context), params, false));
        }
    }

    // Cache things
    pp.zero_i32 = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);

    return true;
}

JobResult BackendLLVM::preCompile(const BuildParameters& buildParameters, Job* ownerJob)
{
    // Do we need to generate the file ?
    if (!mustCompile)
        return JobResult::ReleaseJob;

    int ct              = buildParameters.compileType;
    int precompileIndex = buildParameters.precompileIndex;
    if (perThread[ct][precompileIndex].pass == BackendPreCompilePass::Init)
    {
        perThread[ct][precompileIndex].pass = BackendPreCompilePass::FunctionBodies;

        SWAG_ASSERT(!module->name.empty());
        perThread[ct][precompileIndex].filename = format("%s%d", module->name.c_str(), precompileIndex);
        perThread[ct][precompileIndex].filename += buildParameters.postFix;
        perThread[ct][precompileIndex].filename += ".obj";

        perThread[ct][precompileIndex].context = new llvm::LLVMContext();
        perThread[ct][precompileIndex].module  = new llvm::Module(perThread[ct][precompileIndex].filename.c_str(), *perThread[ct][precompileIndex].context);
        perThread[ct][precompileIndex].builder = new llvm::IRBuilder<>(*perThread[ct][precompileIndex].context);

        if (g_CommandLine.verboseBuildPass)
            g_Log.verbose(format("   module '%s', llvm backend, generating files", perThread[ct][precompileIndex].filename.c_str(), module->byteCodeTestFunc.size()));

        createRuntime(buildParameters);
        emitDataSegment(buildParameters, &module->bssSegment);
        emitDataSegment(buildParameters, &module->mutableSegment);
        emitDataSegment(buildParameters, &module->constantSegment);
    }

    if (perThread[ct][precompileIndex].pass == BackendPreCompilePass::FunctionBodies)
    {
        perThread[ct][precompileIndex].pass = BackendPreCompilePass::End;
        emitAllFunctionBody(buildParameters, module, ownerJob);
        return JobResult::KeepJobAlivePending;
    }

    if (perThread[ct][precompileIndex].pass == BackendPreCompilePass::End)
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
    auto path       = targetPath + "/" + perThread[ct][precompileIndex].filename;

    auto                 filename = path;
    std::error_code      EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    llvm::legacy::PassManager llvmPass;

    /*llvmPass.add(llvm::createPromoteMemoryToRegisterPass());
    llvmPass.add(llvm::createInstructionCombiningPass());
    llvmPass.add(llvm::createReassociatePass());
    llvmPass.add(llvm::createGVNPass());
    llvmPass.add(llvm::createCFGSimplificationPass());

    llvm::PassManagerBuilder pmb;
    pmb.OptLevel           = 3;
    pmb.SizeLevel          = 2;
    pmb.Inliner            = llvm::createFunctionInliningPass(pmb.OptLevel, pmb.SizeLevel, true);
    pmb.DisableUnrollLoops = false;
    pmb.LoopVectorize      = true;
    pmb.SLPVectorize       = true;
    pmb.populateModulePassManager(llvmPass);*/

    theTargetMachine->addPassesToEmitFile(llvmPass, dest, nullptr, llvm::CGFT_ObjectFile);

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
