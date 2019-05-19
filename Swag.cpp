#include "pch.h"
#include "ThreadManager.h"
#include "LoadingThread.h"
#include "SourceFile.h"
#include "SyntaxJob.h"
#include "ParseFolderJob.h"
#include "ffi.h"
#include "global.h"
#include "stats.h"

float toto(float a)
{
    return a / 3;
}

int main()
{
    ffi_cif   cif;
    ffi_type* args[10];
    void*     values[10];
    float     f;

    args[0]   = &ffi_type_float;
    values[0] = &f;

    /* Initialize the cif */
    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1, &ffi_type_float, args);
    f            = 666;
    float result = 0;
    ffi_call(&cif, FFI_FN(toto), &result, values);

    int                                   cpt = 0;
    typedef chrono::high_resolution_clock Clock;
    auto                                  t1 = Clock::now();

    auto job = new ParseFolderJob("c:\\boulot\\sdb\\");
    //auto job = new ParseFolderJob("c:\\boulot\\swag\\unittest");
    g_ThreadMgr.init();
    g_ThreadMgr.addJob(job);
    g_ThreadMgr.waitEndJobs();

    auto                     t2   = Clock::now();
    chrono::duration<double> diff = t2 - t1;
    wcout << diff.count() << "s\n";
    wcout << g_Stats.numLines << " lines\n";
    wcout << g_Stats.numFiles << " files\n";
}
