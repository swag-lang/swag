#pragma once

namespace BackendCompilerWin32
{
    bool compile(const BuildParameters& buildParameters, Module* module, const vector<string>& cFiles);
}
