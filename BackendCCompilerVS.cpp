#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "Log.h"
#include "Stats.h"

bool BackendCCompilerVS::doProcess(const string& cmdline, const string& compilerPath)
{
    STARTUPINFOA        si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES saAttr;
    HANDLE              hChildStdoutRd, hChildStdoutWr;
    DWORD               dwRead;
    CHAR                chBuf[4096];
    Utf8                strout;
    DWORD               exit;
    UINT                errmode;

    errmode = ::GetErrorMode();
    ::SetErrorMode(SEM_FAILCRITICALERRORS);

    // Create a pipe to receive compiler results
    ::ZeroMemory(&saAttr, sizeof(saAttr));
    saAttr.nLength              = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle       = TRUE;
    saAttr.lpSecurityDescriptor = nullptr;
    if (!::CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
    {
        backend->module->error("can't create cl.exe process (::CreatePipe)");
        return false;
    }

    // Create process
    ::ZeroMemory(&si, sizeof(si));
    si.cb         = sizeof(si);
    si.hStdError  = hChildStdoutWr;
    si.hStdOutput = hChildStdoutWr;
    si.dwFlags    = STARTF_USESTDHANDLES;
    ::ZeroMemory(&pi, sizeof(pi));

    g_Log.verbose(cmdline);

    if (!::CreateProcessA(nullptr,
                          (LPSTR) cmdline.c_str(),
                          nullptr,
                          nullptr,
                          TRUE,
                          CREATE_NO_WINDOW,
                          nullptr,
                          compilerPath.c_str(),
                          &si,
                          &pi))
    {
        backend->module->error("can't create cl.exe process (::CreateProcess)");
        return false;
    }

    // Wait until child process exits
    chBuf[0] = 0;
    while (1)
    {
        ::PeekNamedPipe(hChildStdoutRd, chBuf, 4096, &dwRead, nullptr, nullptr);
        if (dwRead)
        {
            // Read compiler results
            strout.clear();
            if (::ReadFile(hChildStdoutRd, chBuf, 4096, &dwRead, nullptr))
            {
                strout.append(chBuf, dwRead);
                while (dwRead == 4096)
                {
                    if (::ReadFile(hChildStdoutRd, chBuf, 4096, &dwRead, nullptr))
                        strout.append(chBuf, dwRead);
                }
            }

            wchar_t azMB[4096];
            ::MultiByteToWideChar(CP_OEMCP, 0, chBuf, dwRead, azMB, 4096);
            azMB[dwRead] = 0;
            wcout << azMB;
            continue;
        }

        ::GetExitCodeProcess(pi.hProcess, &exit);
        if (exit != STILL_ACTIVE)
            break;
        Sleep(1);
    }

    // Close process and thread handles
    ::CloseHandle(hChildStdoutWr);
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);

    ::SetErrorMode(errmode);

    return true;
}

bool BackendCCompilerVS::getVSTarget(string& vsTarget)
{
    vsTarget = R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC)";
    if (!fs::exists(vsTarget))
    {
        vsTarget = R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC)";
        if (!fs::exists(vsTarget))
        {
            vsTarget = R"(C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC)";
            if (!fs::exists(vsTarget))
            {
                vsTarget = R"(C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Tools\MSVC)";
                if (!fs::exists(vsTarget))
                {
                    backend->module->error("can't find visual studio backend folder");
                    return false;
                }
            }
        }
    }

    for (auto& p : fs::directory_iterator(vsTarget))
        vsTarget = p.path().string();

    return true;
}

bool BackendCCompilerVS::getWinSdk(string& winSdk)
{
    string libPath = R"(C:\Program Files (x86)\Windows Kits\10\Lib)";
    if (!fs::exists(libPath))
    {
        backend->module->error("can't find windows sdk folder");
        return false;
    }

	fs::path tmpPath;
    for (auto& p : fs::directory_iterator(libPath))
		tmpPath = p.path();

	winSdk = tmpPath.filename().string();
    return true;
}

bool BackendCCompilerVS::compile()
{
    string vsTarget;
    SWAG_CHECK(getVSTarget(vsTarget));
    string winSdk;
    SWAG_CHECK(getWinSdk(winSdk));

    string clPath = vsTarget + R"(\bin\Hostx64\x64\)";

    // Folders
    vector<string> libPath;
    libPath.push_back(format(R"(%s\lib\x64)", vsTarget.c_str()));
    libPath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\lib\%s\um\x64)", winSdk.c_str()));
    libPath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\lib\%s\ucrt\x64)", winSdk.c_str()));

    vector<string> includePath;
    includePath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\include\%s\um)", winSdk.c_str()));
    includePath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\include\%s\shared)", winSdk.c_str()));
    includePath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\include\%s\ucrt)", winSdk.c_str()));
	includePath.push_back(format(R"(%s\include)", vsTarget.c_str()));

    // CL arguments
    bool isDebug = false; // true;

    string clArguments = "";
    if (isDebug)
    {
        auto pdbPath = backend->outputFile;
        pdbPath.replace_extension(".pdb");
        clArguments += "/Fd\"" + pdbPath.string() + "\" ";
        clArguments += "/Zi ";
    }

    clArguments += "/nologo ";
	clArguments += "/EHsc ";
    clArguments += "/Tp\"" + backend->destCFile.string() + "\" ";
    clArguments += "/Fo\"" + backend->outputFile.string() + ".obj\" ";
    for (const auto& oneIncludePath : includePath)
        clArguments += "/I\"" + oneIncludePath + "\" ";

    // LINK arguments
    string linkArguments = "";
    linkArguments += "/INCREMENTAL:NO /NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
    if (isDebug)
        linkArguments += "/DEBUG ";
    //linkArguments += "/DLL ";
    for (const auto& oneLibPath : libPath)
        linkArguments += "/LIBPATH:\"" + oneLibPath + "\" ";
    linkArguments += "/OUT:\"" + backend->outputFile.string() + "\" ";

    auto cmdLine = "\"" + clPath + "cl.exe\" " + clArguments + "/link " + linkArguments;
    doProcess(cmdLine, clPath);
    return true;
}