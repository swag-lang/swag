#include "pch.h"
#ifdef _WIN32
#include <comdef.h>
#include <windows.h>
#include "Log.h"
#include "LLVMSetup.h"
#include "Backend.h"
#include "ComWin32.hpp"

namespace BackendSetupWin32
{
    string winSdkPath, winSdkVersion;
} // namespace BackendSetupWin32

namespace OS
{
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

    static bool searchOnePath(vector<string>& toTest, string& folder)
    {
        for (auto& one : toTest)
        {
            if (fs::exists(one))
            {
                folder = one;
                return true;
            }
        }

        return false;
    }

    static bool getWinSdkFolder(string& libPath, string& libVersion)
    {
        // Try in register first
        HKEY hKey;
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
            vector<string> toTest;
            toTest.push_back(R"(C:\Program Files (x86)\Windows Kits\10)");
            toTest.push_back(R"(D:\Program Files (x86)\Windows Kits\10)");
            if (!searchOnePath(toTest, libPath))
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

    void setupBackend()
    {
        if (!g_CommandLine.output)
            return;

        // Windows sdk folders and version
        if (!getWinSdkFolder(BackendSetupWin32::winSdkPath, BackendSetupWin32::winSdkVersion))
        {
            g_Log.error("error: backend: cannot locate windows sdk folder");
            exit(-1);
        }

        // Compiler
        switch (g_CommandLine.backendType)
        {
        case BackendType::LLVM:
        case BackendType::X64:
            LLVM::setup();
            break;
        }

        if (g_CommandLine.verbose && g_CommandLine.verbosePath)
        {
            g_Log.verbose(format("winSdkPath is '%s'\n", BackendSetupWin32::winSdkPath.c_str()));
            g_Log.verbose(format("winSdkVersion is '%s'\n", BackendSetupWin32::winSdkVersion.c_str()));
        }
    }
} // namespace OS
#endif