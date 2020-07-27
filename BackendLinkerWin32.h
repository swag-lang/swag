#pragma once
#ifdef _WIN32
struct Module;
struct BuildParameters;

namespace BackendLinkerWin32
{
    void getLibPaths(vector<Utf8>& libPath);

    string getOutputFileName(const BuildParameters& buildParameters);
    void   getArguments(const BuildParameters& buildParameters, Module* module, Utf8& arguments);
    string getCacheFolder(const BuildParameters& buildParameters);
} // namespace BackendLinkerWin32

#endif