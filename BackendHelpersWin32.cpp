#include "pch.h"
#include "Log.h"

namespace BackendHelpersWin32
{
    string visualStudioPath;
    string winSdkPath, winSdkVersion;
    string compilerPath;
    string compilerExe;

    static string getStringRegKey(HKEY hKey, const string& strValueName)
    {
        string strValue;
        char   szBuffer[512];
        DWORD  dwBufferSize = sizeof(szBuffer);
        ULONG  nError;
        nError = RegQueryValueExA(hKey, strValueName.c_str(), 0, NULL, (LPBYTE) szBuffer, &dwBufferSize);
        if (nError == ERROR_SUCCESS)
            strValue = szBuffer;
        return strValue;
    }

    static bool getVSFolder(string& vsTarget)
    {
        vector<string> toTest;
        toTest.push_back(R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC)");
        toTest.push_back(R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC)");
        toTest.push_back(R"(C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Tools\MSVC)");
        toTest.push_back(R"(C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC)");
        for (auto& one : toTest)
        {
            if (fs::exists(one))
            {
                vsTarget = one;
                break;
            }
        }

        if (vsTarget.empty())
            return false;

        for (auto& p : fs::directory_iterator(vsTarget))
            vsTarget = p.path().string();

        return !vsTarget.empty();
    }

    static bool getWinSdkFolder(string& libPath, string& libVersion)
    {
        HKEY hKey;

        // Folder, try in register first
        LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0\\", 0, KEY_READ, &hKey);
        if (lRes == ERROR_SUCCESS)
        {
            libPath    = getStringRegKey(hKey, "InstallationFolder");
            libVersion = getStringRegKey(hKey, "ProductVersion");
            if (!libVersion.empty())
                libVersion += ".0";
            RegCloseKey(hKey);
        }

        // Folder, fallback to direct folder
        if (libPath.empty())
        {
            libPath = R"(C:\Program Files (x86)\Windows Kits\10)";
            if (!fs::exists(libPath))
                return false;
        }

        // Version, fallback to direct folder
        if (libVersion.empty())
        {
            auto libSub = libPath + "\\" + "lib";
            if (!fs::exists(libSub))
                return false;
            fs::path tmpPath;
            for (auto& p : fs::directory_iterator(libSub))
                tmpPath = p.path();
            libVersion = tmpPath.filename().string();
        }

        return !libPath.empty() && !libVersion.empty();
    }

    void setup()
    {
        if (!g_CommandLine.backendOutput)
            return;

        // Visual studio folder
        // For vcruntime & msvcrt (mandatory under windows, even with clang...)
        // For clang-cl, it seems that it can find the folder itself
        if (!getVSFolder(visualStudioPath))
        {
            g_Log.error("error: backend: cannot locate visual studio folder");
            exit(-1);
        }

        // Windows sdk folders and version
        if (!getWinSdkFolder(winSdkPath, winSdkVersion))
        {
            g_Log.error("error: backend: cannot locate windows sdk folder");
            exit(-1);
        }

        // Compiler
        switch (g_CommandLine.backendType)
        {
        case BackendType::C_Vs:
            compilerExe  = "cl.exe";
            compilerPath = BackendHelpersWin32::visualStudioPath + R"(\bin\Hostx64\x64\)";
            break;
        case BackendType::C_Clang:
            compilerExe  = "clang-cl.exe";
            compilerPath = "C:/Program Files/LLVM/bin/";
            break;
        case BackendType::LLVM:
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            break;
        }

        if (g_CommandLine.backendType == BackendType::C_Vs || g_CommandLine.backendType == BackendType::C_Clang)
        {
            auto fullPath = compilerPath + compilerExe;
            if (!fs::exists(fullPath))
            {
                g_Log.error(format("error: c backend: cannot locate compiler '%s'", fullPath.c_str()));
                exit(-1);
            }
        }
    }

} // namespace BackendHelpersWin32