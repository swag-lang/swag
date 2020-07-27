#pragma once
#ifdef _WIN32

namespace BackendSetupWin32
{
    extern string visualStudioPath;
    extern string winSdkPath, winSdkVersion;
    void          setup();
} // namespace BackendSetupWin32

#endif