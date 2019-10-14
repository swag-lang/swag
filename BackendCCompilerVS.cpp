#include "pch.h"
#include "SourceFile.h"
#include "BackendC.h"
#include "BackendCCompilerVS.h"
#include "Workspace.h"

bool BackendCCompilerVS::doProcess(const string& cmdline, const string& compilerPath, bool logAll, uint32_t& numErrors)
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
        g_Log.error(format("cannot create '%s' process (::CreatePipe)", cmdline.c_str()));
        return false;
    }

    // Create process
    ::ZeroMemory(&si, sizeof(si));
    si.cb         = sizeof(si);
    si.hStdError  = hChildStdoutWr;
    si.hStdOutput = hChildStdoutWr;
    si.dwFlags    = STARTF_USESTDHANDLES;
    ::ZeroMemory(&pi, sizeof(pi));

    if (g_CommandLine.verboseBackendCommand)
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
        g_Log.error(format("cannot create '%s' process (::CreateProcess)", cmdline.c_str()));
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
                if (!pz)
                    pz = strstr(oneLine.c_str(), ": fatal error");
                if (!pz)
                    pz = strstr(oneLine.c_str(), ": warning");

                // Error
                if (pz)
                {
                    numErrors++;
                    ok = false;

                    g_Log.setColor(LogColor::Red);
                    g_Log.print(oneLine + "\n");
                }

                // Messages
                else if (logAll)
                {
                    g_Log.setDefaultColor();
                    g_Log.print(oneLine + "\n");
                }
            }

            g_Log.setDefaultColor();
            g_Log.unlock();

            continue;
        }

        ::GetExitCodeProcess(pi.hProcess, &exit);
        if (exit != STILL_ACTIVE)
        {
            switch (exit)
            {
            case 0:
            case -1:
                break;
            case STATUS_ACCESS_VIOLATION:
                g_Log.lock();
                g_Log.setColor(LogColor::Red);
                numErrors++;
                wcout << cmdline.c_str();
                wcout << ": access violation during process execution\n";
                g_Log.setDefaultColor();
                g_Log.unlock();
                ok = false;
                break;
            default:
                g_Log.lock();
                g_Log.setColor(LogColor::Red);
                numErrors++;
                wcout << cmdline.c_str();
                wcout << ": process execution failed\n";
                g_Log.setDefaultColor();
                g_Log.unlock();
                ok = false;
                break;
            }
            break;
        }

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
    auto module = backend->module;

    string vsTarget;
    SWAG_CHECK(getVSTarget(vsTarget));
    string winSdk;
    SWAG_CHECK(getWinSdk(winSdk));

    string clPath = vsTarget + R"(\bin\Hostx64\x64\)";

    // Library paths
    vector<string> libPath;
    libPath.push_back(format(R"(%s\lib\x64)", vsTarget.c_str()));
    libPath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\lib\%s\um\x64)", winSdk.c_str()));
    libPath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\lib\%s\ucrt\x64)", winSdk.c_str()));
    libPath.push_back(g_Workspace.targetPath.string());

    // Include paths
    vector<string> includePaths;
    //includePath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\include\%s\um)", winSdk.c_str()));
    //includePath.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\include\%s\shared)", winSdk.c_str()));
    includePaths.push_back(format(R"(C:\Program Files (x86)\Windows Kits\10\include\%s\ucrt)", winSdk.c_str()));
    includePaths.push_back(format(R"(%s\include)", vsTarget.c_str()));

    // CL arguments
    string clArguments = "";
    if (buildParameters->target.backendDebugInformations)
    {
        fs::path pdbPath = buildParameters->destFile + buildParameters->postFix + ".pdb";
        clArguments += "/Fd\"" + pdbPath.string() + "\" ";
        clArguments += "/Zi ";
    }

    // Append a string related to the version
    string outputTypeName;
    switch (buildParameters->type)
    {
    case BackendOutputType::StaticLib:
        outputTypeName = ".lib";
        break;
    case BackendOutputType::DynamicLib:
        outputTypeName = ".dll";
        break;
    }

    clArguments += "/nologo ";
    clArguments += "/EHsc ";
    clArguments += "/Tc\"" + backend->bufferC.fileName + "\" ";
    string nameObj = buildParameters->destFile + outputTypeName + buildParameters->postFix + ".obj";
    clArguments += "/Fo\"" + nameObj + "\" ";
    switch (buildParameters->target.backendOptimizeLevel)
    {
    case 0:
        break;
    case 1:
        clArguments += "/01 ";
        break;
    default:
        clArguments += "/O2 ";
        break;
    }

    for (const auto& oneIncludePath : includePaths)
        clArguments += "/I\"" + oneIncludePath + "\" ";
    if (buildParameters->flags & BUILDPARAM_FOR_TEST)
        clArguments += "/DSWAG_IS_UNITTEST ";

    bool verbose = g_CommandLine.verbose && g_CommandLine.verboseBackendCommand;

    uint32_t numErrors = 0;
    string   resultFile;
    switch (buildParameters->type)
    {
    case BackendOutputType::StaticLib:
    {
        auto cmdLineCL = "\"" + clPath + "cl.exe\" " + clArguments + " /c";
        SWAG_CHECK(doProcess(cmdLineCL, clPath, verbose, numErrors));

        string libArguments;
        libArguments = "/NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (g_CommandLine.verboseBackendCommand)
            libArguments += "/VERBOSE ";
        resultFile = buildParameters->destFile + buildParameters->postFix + ".lib";
        libArguments += "/OUT:\"" + resultFile + "\" ";
        libArguments += "\"" + nameObj + "\" ";

        if (verbose)
            g_Log.verbose(format("vs compiling '%s' => '%s'", backend->bufferC.fileName.c_str(), resultFile.c_str()));

        auto cmdLineLIB = "\"" + clPath + "lib.exe\" " + libArguments;
        SWAG_CHECK(doProcess(cmdLineLIB, clPath, verbose, numErrors));
    }
    break;

    case BackendOutputType::DynamicLib:
    case BackendOutputType::Binary:
    {
        string linkArguments;
        linkArguments += "legacy_stdio_definitions.lib ";
        for (auto depName : module->moduleDependenciesNames)
            linkArguments += depName + ".lib ";
        for (const auto& oneLibPath : libPath)
            linkArguments += "/LIBPATH:\"" + oneLibPath + "\" ";

        linkArguments += "/INCREMENTAL:NO /NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X64 ";
        if (buildParameters->target.backendDebugInformations)
            linkArguments += "/DEBUG ";

        if (buildParameters->type == BackendOutputType::DynamicLib)
        {
            linkArguments += "/DLL ";
            resultFile = buildParameters->destFile + buildParameters->postFix + ".dll";
            linkArguments += "/OUT:\"" + resultFile + "\" ";
            clArguments += "/DSWAG_IS_DLL ";
        }
        else
        {
            resultFile = buildParameters->destFile + buildParameters->postFix + ".exe";
            linkArguments += "/OUT:\"" + resultFile + "\" ";
            clArguments += "/DSWAG_IS_EXE ";
        }

        if (verbose)
            g_Log.verbose(format("vs compiling '%s' => '%s'", backend->bufferC.fileName.c_str(), resultFile.c_str()));

        auto cmdLineCL = "\"" + clPath + "cl.exe\" " + clArguments + "/link " + linkArguments;
        SWAG_CHECK(doProcess(cmdLineCL, clPath, verbose, numErrors));
    }
    break;
    }

    g_Workspace.numErrors += numErrors;
    backend->module->numErrors += numErrors;
    return true;
}

bool BackendCCompilerVS::runTests()
{
    fs::path path = buildParameters->destFile + ".test.exe";
    if (fs::exists(path))
    {
        g_Log.messageHeaderCentered("Testing backend", backend->module->name.c_str());
        uint32_t numErrors = 0;
        SWAG_CHECK(doProcess(path.string(), path.parent_path().string(), true, numErrors));
        g_Workspace.numErrors += numErrors;
        backend->module->numErrors += numErrors;
    }

    return true;
}