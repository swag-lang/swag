#pragma once
struct Module;
struct BuildParameters;

namespace BackendLinker
{
    bool link(const BuildParameters& buildParameters, Module* module, Vector<Path>& objectFiles);
} // namespace BackendLinker
