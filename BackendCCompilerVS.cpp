#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Global.h"
#include "Module.h"
#include "Log.h"
#include "Stats.h"
#include "Workspace.h"

bool BackendCCompilerVS::doProcess(const string& cmdline, const string& compilerPath, bool logAll)
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
        backend->module->error(format("can't create '%s' process (::CreatePipe)", cmdline.c_str()));
        return false;
    }

    // Create process
    ::ZeroMemory(&si, sizeof(si));
    si.cb         = sizeof(si);
    si.hStdError  = hChildStdoutWr;
    si.hStdOutput = hChildStdoutWr;
    si.dwFlags    = STARTF_USESTDHANDLES;
    ::ZeroMemory(&pi, sizeof(pi));

    if (g_CommandLine.verbose_backend_command)
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
        backend->module->error(format("can't create '%s' process (::CreateProcess)", cmdline.c_str()));
        return false;
    }

    // Wait until child process exits
    bool ok  = true;
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

            // Process result
            g_Log.lock();
            vector<string> lines;
            tokenize(strout.c_str(), '\n', lines);
            for (auto oneLine : lines)
            {
                g_Log.setDefaultColor();

                if (oneLine.back() == '\r')
                    oneLine.erase(oneLine.end() - 1);

                auto pz = strstr(oneLine.c_str(), ": error");
                if (pz)
                {
                    g_Log.setColor(LogColor::Red);
                    backend->module->numErrors++;
                    g_Workspace.numErrors++;
                }

                if (pz || logAll)
                {
                    wchar_t azMB[4096];
                    ::MultiByteToWideChar(CP_OEMCP, 0, oneLine.c_str(), (int) oneLine.length(), azMB, 4096);
                    azMB[oneLine.length()]     = '\n';
                    azMB[oneLine.length() + 1] = 0;
                    g_Log.print(azMB);

                    ok = false;
                }
            }

            g_Log.setDefaultColor();
            g_Log.unlock();

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

    return ok;
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
        auto pdbPath = backend->destFile;
        pdbPath.replace_extension(".pdb");
        clArguments += "/Fd\"" + pdbPath.string() + "\" ";
        clArguments += "/Zi ";
    }

    clArguments += "/nologo ";
    clArguments += "/EHsc ";
    clArguments += "/Tc\"" + backend->destFileC.string() + "\" ";
    clArguments += "/Fo\"" + backend->destFile.string() + ".obj\" ";
    for (const auto& oneIncludePath : includePath)
        clArguments += "/I\"" + oneIncludePath + "\" ";

    // LINK arguments
    string linkArguments = "legacy_stdio_definitions.lib ";
    linkArguments += "/INCREMENTAL:NO /NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
    if (isDebug)
        linkArguments += "/DEBUG ";
    //linkArguments += "/DLL ";
    for (const auto& oneLibPath : libPath)
        linkArguments += "/LIBPATH:\"" + oneLibPath + "\" ";
    linkArguments += "/OUT:\"" + backend->destFile.string() + "\" ";

    g_Log.message(format("vs compiling '%s' => '%s'", backend->destFileC.string().c_str(), backend->destFile.string().c_str()));

    auto cmdLine = "\"" + clPath + "cl.exe\" " + clArguments + "/link " + linkArguments;
    SWAG_CHECK(doProcess(cmdLine, clPath, false));
    return true;
}

bool BackendCCompilerVS::runTests()
{
    if (!g_CommandLine.runBackendTests)
        return true;
    if (backend->module->byteCodeTestFunc.empty())
        return true;

    if (g_CommandLine.verbose_test)
        g_Log.verbose(format("running tests on '%s'\n", backend->destFile.string().c_str()));

    auto path = backend->destFile;
    SWAG_CHECK(doProcess(path.string(), path.parent_path().string(), true));
    return true;
}