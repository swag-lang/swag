#pragma once
#ifdef _WIN32
struct Module;
struct BuildParameters;

namespace BackendLinkerWin32
{
    void getLibPaths(vector<Utf8>& libPath);
    void getArguments(const BuildParameters& buildParameters, Module* module, vector<Utf8>& arguments, bool addQuote);
} // namespace BackendLinkerWin32

#endif