#include "pch.h"
#include "Utf8.h"
#include "BackendSetupWin32.h"
#include "Global.h"
#include "CommandLine.h"
#include "Workspace.h"

#ifdef _WIN32

namespace BackendLinkerWin32
{
    void getLibPaths(vector<Utf8>& libPath)
    {
        // For vcruntime & msvcrt (mandatory under windows, even with clang...)
        libPath.push_back(format(R"(%s\lib\x64)", BackendSetupWin32::visualStudioPath.c_str()));

        // Windows sdk library paths
        libPath.push_back(format(R"(%s\lib\%s\um\x64)", BackendSetupWin32::winSdkPath.c_str(), BackendSetupWin32::winSdkVersion.c_str()));
        libPath.push_back(format(R"(%s\lib\%s\ucrt\x64)", BackendSetupWin32::winSdkPath.c_str(), BackendSetupWin32::winSdkVersion.c_str()));

        // Modules
        libPath.push_back(g_Workspace.targetPath.string());

        // Runtime
        libPath.push_back(g_CommandLine.exePath.parent_path().string());

    }
} // namespace BackendLinkerWin32

#endif