#include "pch.h"
#include "ThreadManager.h"
#include "LoadingThread.h"
#include "SourceFile.h"
#include "SyntaxJob.h"
#include "ffi.h"
#include "Global.h"
#include "Stats.h"
#include "Log.h"
#include "CommandLine.h"
#include "Workspace.h"

float toto(float a)
{
    return a / 3;
}

void testffi()
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
}

void printStats(chrono::duration<double>& elapsedTime)
{
    if (!g_CommandLine.stats)
        return;

    g_Log.setColor(LogColor::White);
    wcout << elapsedTime.count() << "s\n";
    wcout << g_Stats.numLines << (g_Stats.numLines == 1 ? " line\n" : " lines\n");
    wcout << g_Stats.numFiles << (g_Stats.numFiles == 1 ? " file\n" : " files\n");
    if (g_Stats.numErrors)
    {
        g_Log.setColor(LogColor::Red);
		wcout << g_Stats.numErrors << (g_Stats.numErrors == 1 ? " error\n" : " errors\n");
    }

    g_Log.setDefaultColor();
}

int main(int argc, const char* argv[])
{
    auto timeBefore = chrono::high_resolution_clock::now();
	if (!g_CommandLine.process(argc, argv))
		return -2;

	g_Workspace.build();

	// Prints stats, then exit
    auto                     timeAfter = chrono::high_resolution_clock::now();
    chrono::duration<double> diff      = timeAfter - timeBefore;
    printStats(diff);

    return g_Stats.numErrors > 0 ? -1 : 0;
}
