#pragma once
struct Module;
struct BuildParameters;

namespace BackendLinker
{
    bool link(const BuildParameters& buildParameters, Module* module, Vector<string>& objectFiles);
} // namespace BackendLinker