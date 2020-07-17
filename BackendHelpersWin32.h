#pragma once
#ifdef _WIN32

namespace BackendHelpersWin32
{
    extern string visualStudioPath;
    extern string winSdkPath, winSdkVersion;
    extern string compilerExe;
    extern string compilerPath;

    void setup();
} // namespace BackendHelpersWin32

#endif