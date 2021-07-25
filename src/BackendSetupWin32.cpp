#include "pch.h"
#ifdef _WIN32
#include <windows.h>
#include "Log.h"
#include "Backend.h"
#include "ComWin32.hpp"
#include "ErrorIds.h"

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
        // Add the windows sdk access to crt libs
        if (g_CommandLine.os == BackendOs::Windows && !isAbiGnu(g_CommandLine.abi))
        {
            string winSdkPath;
            string winSdkVersion;
            if (!getWinSdkFolder(winSdkPath, winSdkVersion))
            {
                g_Log.error(Msg0036);
                exit(-1);
            }

            if (g_CommandLine.verbosePath)
            {
                g_Log.verbose(format("windows sdk path is '%s'\n", winSdkPath.c_str()));
                g_Log.verbose(format("windows sdk version is '%s'\n", winSdkVersion.c_str()));
            }

            const char* target = isArchArm(g_CommandLine.arch) ? "arm64" : "x64";
            g_CommandLine.libPaths.push_back(format(R"(%slib\%s\um\%s)", winSdkPath.c_str(), winSdkVersion.c_str(), target));
            g_CommandLine.libPaths.push_back(format(R"(%slib\%s\ucrt\%s)", winSdkPath.c_str(), winSdkVersion.c_str(), target));
        }
    }
} // namespace OS
#endif