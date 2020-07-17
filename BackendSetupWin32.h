#pragma once
#ifdef _WIN32

namespace BackendSetupWin32
{
    extern string visualStudioPath;
    extern string winSdkPath, winSdkVersion;
    extern string compilerExe;
    extern string compilerPath;
    extern string linkerExe;
    extern string linkerPath;

    void setup();
} // namespace BackendHelpersWin32

#endif