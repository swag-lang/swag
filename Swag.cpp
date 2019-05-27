#include "pch.h"
#include "ffi.h"
#include "Stats.h"
#include "Log.h"
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
    wcout << "elapsed time ...... " << elapsedTime.count() << "s\n";
    wcout << "lines ............. " << g_Stats.numLines << "\n";
    wcout << "files ............. " << g_Stats.numFiles << "\n";
    wcout << "modules ........... " << g_Stats.numModules << "\n";
    if (g_Workspace.numErrors)
    {
        g_Log.setColor(LogColor::Red);
        wcout << "errors ............ " << g_Workspace.numErrors << "\n";
    }

    g_Log.setDefaultColor();
}

int main(int argc, const char* argv[])
{
    auto timeBefore = chrono::high_resolution_clock::now();
    if (!g_CommandLine.process(argc, argv))
        return -2;

	g_Global.setup();
    g_Workspace.build();

    // Prints stats, then exit
    auto                     timeAfter = chrono::high_resolution_clock::now();
    chrono::duration<double> diff      = timeAfter - timeBefore;
    printStats(diff);

    return g_Workspace.numErrors > 0 ? -1 : 0;
}
