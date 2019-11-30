#include "pch.h"
#ifdef WIN32
#include "Os.h"
#include "Global.h"
#include "Log.h"
#include <fcntl.h>
#include <io.h>
#include "BuildParameters.h"

namespace OS
{
    static HANDLE consoleHandle     = NULL;
    static WORD   defaultAttributes = 0;

    void setup()
    {
        // We do not want assert, but just reports of the CRT
#if !defined(SWAG_HAS_ASSERT)
        if (!IsDebuggerPresent())
        {
            _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        }
#endif
    }

    void consoleSetup()
    {
        consoleHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        _setmode(_fileno(stdout), _O_U16TEXT);

        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(consoleHandle, &info);
        defaultAttributes = info.wAttributes;
    }

    void consoleSetColor(LogColor color)
    {
        WORD attributes{};
        switch (color)
        {
        case LogColor::Black:
            attributes = 0;
            break;
        case LogColor::White:
            attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case LogColor::Gray:
            attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;
        case LogColor::Red:
            attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case LogColor::Green:
            attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case LogColor::Blue:
            attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case LogColor::Cyan:
            attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case LogColor::Magenta:
            attributes = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case LogColor::Yellow:
            attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case LogColor::DarkRed:
            attributes = FOREGROUND_RED;
            break;
        case LogColor::DarkGreen:
            attributes = FOREGROUND_GREEN;
            break;
        case LogColor::DarkBlue:
            attributes = FOREGROUND_BLUE;
            break;
        case LogColor::DarkCyan:
            attributes = FOREGROUND_BLUE | FOREGROUND_GREEN;
            break;
        case LogColor::DarkMagenta:
            attributes = FOREGROUND_BLUE | FOREGROUND_RED;
            break;
        case LogColor::DarkYellow:
            attributes = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case LogColor::Default:
            attributes = defaultAttributes;
            break;
        }

        WORD back = defaultAttributes & (BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
        ::SetConsoleTextAttribute(consoleHandle, attributes | back);
    }

    bool doProcess(const string& cmdline, const string& currentDirectory, bool logAll, uint32_t& numErrors, LogColor logColor, const char* logPrefix)
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

        if (!::CreateProcessA(nullptr,
                              (LPSTR) cmdline.c_str(),
                              nullptr,
                              nullptr,
                              TRUE,
                              CREATE_NO_WINDOW,
                              nullptr,
                              currentDirectory.c_str(),
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
                        g_Log.setColor(logColor);
                        if (logPrefix)
                            g_Log.print(logPrefix);
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

    string getOutputFileExtension(BackendOutputType type)
    {
        switch (type)
        {
        case BackendOutputType::Binary:
            return ".exe";
        case BackendOutputType::StaticLib:
            return ".lib";
        case BackendOutputType::DynamicLib:
            return ".dll";
        default:
            SWAG_ASSERT(false);
            return "";
        }
    }

    string getLastErrorAsString()
    {
        // Get the error message, if any.
        DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0)
            return std::string(); // No error message has been recorded

        LPSTR  messageBuffer = nullptr;
        size_t size          = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                     NULL,
                                     errorMessageID,
                                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                     (LPSTR) &messageBuffer,
                                     0,
                                     NULL);

        std::string message(messageBuffer, size);

        //Free the buffer.
        LocalFree(messageBuffer);
        return message;
    }

    swag_tls_id_t tlsAlloc()
    {
        return TlsAlloc();
    }

    void tlsSetValue(swag_tls_id_t id, void* value)
    {
        TlsSetValue(id, value);
    }

    void* tlsGetValue(swag_tls_id_t id)
    {
        return TlsGetValue(id);
    }

    void* loadLibrary(const char* name)
    {
        return (void*) LoadLibraryA(name);
    }

    void* getProcAddress(void* handle, const char* name)
    {
        return ::GetProcAddress((HMODULE) handle, name);
    }

    void visitFiles(const char* folder, function<void(const char*)> user)
    {
        WIN32_FIND_DATAA findfile;
        string           searchPath = folder;
        searchPath += "/*";
        HANDLE h = ::FindFirstFileA(searchPath.c_str(), &findfile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    continue;
                user(findfile.cFileName);
            } while (::FindNextFileA(h, &findfile));

            ::FindClose(h);
        }
    }

    void visitFolders(const char* folder, function<void(const char*)> user)
    {
        WIN32_FIND_DATAA findfile;
        string           searchPath = folder;
        searchPath += "/*";
        HANDLE h = ::FindFirstFileA(searchPath.c_str(), &findfile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (!(findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    continue;
                if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                    continue;
                user(findfile.cFileName);
            } while (::FindNextFileA(h, &findfile));

            ::FindClose(h);
        }
    }

}; // namespace OS

#endif