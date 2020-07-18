#include "pch.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "BackendLLVM.h"
#include "Workspace.h"
#include "ByteCode.h"
#include "Ast.h"
#include "ThreadManager.h"

thread_local Pool<BackendLLVMFunctionBodyJob> g_Pool_backendCFunctionBodyJob;

JobResult BackendLLVMFunctionBodyJob::execute()
{
    BackendLLVM* bachendLLVM = (BackendLLVM*) backend;
    return JobResult::ReleaseJob;
}
