#pragma once
struct Module;
struct BuildParameters;

namespace BackendLinker
{
    bool link(const BuildParameters& buildParameters, Module* module, const Vector<Path>& objectFiles);
} // namespace BackendLinker
