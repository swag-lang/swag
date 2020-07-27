#pragma once
#ifdef _WIN32
struct Module;
struct BuildParameters;

namespace BackendLinkerWin32
{
    void getLibPaths(vector<Utf8>& libPath);
    void getArguments(const BuildParameters& buildParameters, Module* module, Utf8& arguments);
    bool link(const BuildParameters& buildParameters, Module* module, vector<string>& objectFiles);
} // namespace BackendLinkerWin32

#endif