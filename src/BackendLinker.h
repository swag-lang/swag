#pragma once
struct Module;
struct BuildParameters;

namespace BackendLinker
{
    bool link(const BuildParameters& buildParameters, const Vector<Path>& objectFiles);
}
