#pragma once
#ifdef _WIN32

namespace BackendSetupWin32
{
    extern string visualStudioPath;
    extern string winSdkPath, winSdkVersion;
    extern string compilerExe;
    extern string compilerPath;

    void setup();
} // namespace BackendHelpersWin32

#endif