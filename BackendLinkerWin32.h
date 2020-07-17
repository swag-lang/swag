#pragma once
#ifdef _WIN32
struct Module;
struct BuildParameters;

namespace BackendLinkerWin32
{
    void getLibPaths(vector<Utf8>& libPath);

    string getResultFile(const BuildParameters& buildParameters);
    void   getArguments(const BuildParameters& buildParameters, Module* module, Utf8& arguments);
} // namespace BackendLinkerWin32

#endif