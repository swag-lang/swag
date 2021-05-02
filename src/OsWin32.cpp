#include "pch.h"
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include "Os.h"
#include "Global.h"
#include "Log.h"
#include "Utf8.h"
#include "BuildParameters.h"
#include "Workspace.h"
#include "Profile.h"
#include "ProfileWin32.h"
#include "Module.h"
#include "Diagnostic.h"
#include "ErrorIds.h"

namespace OS
{
    static HANDLE consoleHandle     = NULL;
    static WORD   defaultAttributes = 0;

    void setup()
    {
        // We do not want assert, but just reports of the CRT
        if (!IsDebuggerPresent())
        {
            _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        }

#ifdef SWAG_HAS_PROFILE
        for (int i = 0; i < MAX_PROFILE_STACK; i++)
        {
            Utf8                   name = format("%d", i);
            VectorNative<uint16_t> uni16;
            name.toUni16(uni16);
            g_Profile[i] = new Concurrency::diagnostic::marker_series((LPCTSTR) uni16.buffer);
        }
#endif
    }

    void consoleSetup()
    {
        consoleHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleOutputCP(65001);

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

    bool doProcess(Module* module, const Utf8& cmdline, const string& currentDirectory, bool logAll, uint32_t& numErrors, LogColor logColor, const char* logPrefix)
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
            g_Log.error(format(Msg0045, cmdline.c_str()));
            return false;
        }

        // Create process
        ::ZeroMemory(&si, sizeof(si));
        si.cb         = sizeof(si);
        si.hStdError  = hChildStdoutWr;
        si.hStdOutput = hChildStdoutWr;
        si.dwFlags    = STARTF_USESTDHANDLES;
        ::ZeroMemory(&pi, sizeof(pi));

        {
            SWAG_PROFILE(PRF_LOAD, format("create process %s", cmdline.c_str()));
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
                g_Log.error(format(Msg0046, cmdline.c_str(), getLastErrorAsString().c_str()));
                return false;
            }
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
                vector<Utf8> lines;
                tokenize(strout.c_str(), '\n', lines);
                for (auto oneLine : lines)
                {
                    if (oneLine.back() == '\r')
                        oneLine.pop_back();

                    const char* pz = nullptr;

                    // Error
                    pz = strstr(oneLine.c_str(), "error:");
                    if (pz)
                    {
                        // Something can have been printed before with a '@print' without an '\n', so just to not miss anything...
                        if (pz != oneLine.c_str() && logAll)
                        {
                            g_Log.lock();
                            g_Log.setColor(logColor);
                            if (logPrefix)
                                g_Log.print(logPrefix);
                            auto tmpLine  = oneLine;
                            tmpLine.count = (int) (pz - oneLine.c_str());
                            g_Log.print(tmpLine + "\n");
                            g_Log.setDefaultColor();
                            g_Log.unlock();
                            oneLine = pz;
                        }

                        if (module)
                        {
                            ok = false;

                            // Extract file and location
                            vector<Utf8> tokens;
                            tokenize(oneLine, ':', tokens);
                            for (auto& t : tokens)
                                t.trim();
                            if (tokens.size() <= 2)
                            {
                                // User message error: just log as a normal message
                                if (logAll)
                                {
                                    g_Log.lock();
                                    g_Log.setColor(logColor);
                                    if (logPrefix)
                                        g_Log.print(logPrefix);
                                    g_Log.print(oneLine + "\n");
                                    g_Log.setDefaultColor();
                                    g_Log.unlock();
                                }
                            }
                            else
                            {
                                auto fileName = tokens[1] + ":";
                                fileName += tokens[2];
                                auto sourceFile = module->findFile(fileName);
                                if (!sourceFile)
                                    sourceFile = g_Workspace.bootstrapModule->findFile(fileName);
                                if (!sourceFile)
                                    sourceFile = g_Workspace.runtimeModule->findFile(fileName);
                                if (sourceFile && tokens.size() == 6) // error: drive letter: path: line: column: message
                                {
                                    SourceLocation startLoc;
                                    startLoc.line         = atoi(tokens[tokens.size() - 3]) - 1;
                                    startLoc.column       = atoi(tokens[tokens.size() - 2]) - 1;
                                    SourceLocation endLoc = startLoc;
                                    auto           msg    = tokens.back();
                                    msg                   = "runtime execution, " + msg;
                                    Diagnostic diag({sourceFile, startLoc, endLoc, msg});
                                    sourceFile->report(diag);
                                }
                                else
                                {
                                    numErrors++;
                                    g_Log.lock();
                                    g_Log.setColor(LogColor::Red);
                                    g_Log.print(oneLine + "\n");
                                    g_Log.setDefaultColor();
                                    g_Log.unlock();
                                }
                            }
                        }
                        else
                        {
                            numErrors++;
                            ok = false;

                            g_Log.lock();
                            g_Log.setColor(LogColor::Red);
                            g_Log.print(oneLine + "\n");
                            g_Log.setDefaultColor();
                            g_Log.unlock();
                        }
                    }

                    // Messages
                    else if (logAll)
                    {
                        g_Log.lock();
                        g_Log.setColor(logColor);
                        if (logPrefix)
                            g_Log.print(logPrefix);
                        g_Log.print(oneLine + "\n");
                        g_Log.setDefaultColor();
                        g_Log.unlock();
                    }
                }

                continue;
            }

            ::GetExitCodeProcess(pi.hProcess, &exit);
            if (exit != STILL_ACTIVE)
            {
                switch (exit)
                {
                case 0:
                case -666:
                    break;
                case STATUS_ACCESS_VIOLATION:
                    g_Log.lock();
                    g_Log.setColor(LogColor::Red);
                    numErrors++;
                    cout << cmdline.c_str();
                    cout << ": access violation during process execution\n";
                    g_Log.setDefaultColor();
                    g_Log.unlock();
                    ok = false;
                    break;
                default:
                    g_Log.lock();
                    g_Log.setColor(LogColor::Red);
                    numErrors++;
                    cout << cmdline.c_str();
                    cout << ": process execution failed\n";
                    g_Log.setDefaultColor();
                    g_Log.unlock();
                    ok = false;
                    break;
                }
                break;
            }

            Sleep(10);
        }

        // Close process and thread handles
        ::CloseHandle(hChildStdoutWr);
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);

        ::SetErrorMode(errmode);

        return ok;
    }

    void doRunProcess(const Utf8& cmdline, const string& currentDirectory)
    {
        STARTUPINFOA        si;
        PROCESS_INFORMATION pi;
        SECURITY_ATTRIBUTES saAttr;
        UINT                errmode;

        errmode = ::GetErrorMode();
        ::SetErrorMode(SEM_FAILCRITICALERRORS);

        ::ZeroMemory(&saAttr, sizeof(saAttr));
        saAttr.nLength              = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle       = TRUE;
        saAttr.lpSecurityDescriptor = nullptr;

        // Create process
        ::ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        ::ZeroMemory(&pi, sizeof(pi));

        {
            SWAG_PROFILE(PRF_LOAD, format("create process %s", cmdline.c_str()));
            if (!::CreateProcessA(nullptr,
                                  (LPSTR) cmdline.c_str(),
                                  nullptr,
                                  nullptr,
                                  TRUE,
                                  0,
                                  nullptr,
                                  currentDirectory.c_str(),
                                  &si,
                                  &pi))
            {
                g_Log.error(format(Msg0046, cmdline.c_str(), getLastErrorAsString().c_str()));
                return;
            }
        }

        // Wait until child process exits
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process and thread handles
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);

        ::SetErrorMode(errmode);
    }

    Utf8 getLastErrorAsString()
    {
        // Get the error message, if any.
        DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0)
            return std::string(); // No error message has been recorded

        LPSTR  messageBuffer = nullptr;
        size_t size          = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                     NULL,
                                     errorMessageID,
                                     MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                                     (LPSTR) &messageBuffer,
                                     0,
                                     NULL);

        // Remove unwanted characters
        char* pz = messageBuffer;
        for (int i = 0; i < size; i++)
        {
            if (pz[i] < 32)
                pz[i] = 32;
        }

        Utf8 message(messageBuffer, (uint32_t) size);
        message.trim();
        if (message.length() && message.back() == '.')
            message.pop_back();

        //Free the buffer.
        LocalFree(messageBuffer);
        return message;
    }

    void* loadLibrary(const char* name)
    {
        return (void*) LoadLibraryA(name);
    }

    void* getProcAddress(void* handle, const char* name)
    {
        return ::GetProcAddress((HMODULE) handle, name);
    }

    uint64_t getFileWriteTime(const char* fileName)
    {
        auto hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return 0;

        uint64_t result = 0;
        FILETIME ftCreate, ftAccess, ftWrite;
        if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        {
            result = ((uint64_t) ftWrite.dwHighDateTime) << 32;
            result |= ftWrite.dwLowDateTime;
        }

        CloseHandle(hFile);
        return result;
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

    void visitFolders(const char* folder, function<void(const char*)> user, const char* match)
    {
        WIN32_FIND_DATAA findfile;
        string           searchPath = folder;
        searchPath += "/";
        searchPath += match;
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

    void visitFilesFolders(const char* folder, function<void(uint64_t, const char*, bool)> user)
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
                {
                    if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                        continue;
                }

                uint64_t writeTime = ((uint64_t) findfile.ftLastWriteTime.dwHighDateTime) << 32;
                writeTime |= findfile.ftLastWriteTime.dwLowDateTime;

                bool isFolder = findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
                user(writeTime, findfile.cFileName, isFolder);

            } while (::FindNextFileA(h, &findfile));

            ::FindClose(h);
        }
    }

    void visitFilesRec(const char* folder, function<void(const char*)> user)
    {
        WIN32_FIND_DATAA findfile;
        string           searchPath = folder;
        searchPath += "/*";

        auto   path = string(folder);
        HANDLE h    = ::FindFirstFileA(searchPath.c_str(), &findfile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                path = folder;
                path += "/";
                path += findfile.cFileName;

                if (findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                        continue;
                    visitFilesRec(path.c_str(), user);
                }
                else
                {
                    user(path.c_str());
                }

            } while (::FindNextFileA(h, &findfile));

            ::FindClose(h);
        }
    }

    const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD  dwType;     // Must be 0x1000.
        LPCSTR szName;     // Pointer to name (in user addr space).
        DWORD  dwThreadID; // Thread ID (-1=caller thread).
        DWORD  dwFlags;    // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    void setThreadName(uint32_t dwThreadID, const char* threadName)
    {
        THREADNAME_INFO info;
        info.dwType     = 0x1000;
        info.szName     = threadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags    = 0;

        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*) &info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }
    void setThreadName(const char* threadName)
    {
        setThreadName(GetCurrentThreadId(), threadName);
    }

    void setThreadName(thread* thread, const char* threadName)
    {
        DWORD threadId = ::GetThreadId(static_cast<HANDLE>(thread->native_handle()));
        setThreadName(threadId, threadName);
    }

    uint64_t tlsAlloc()
    {
        return TlsAlloc();
    }

    void tlsSetValue(uint64_t id, void* value)
    {
        TlsSetValue((uint32_t) id, value);
    }

    void* tlsGetValue(uint64_t id)
    {
        return TlsGetValue((uint32_t) id);
    }

    void errorBox(const char* title, const char* expr)
    {
        ::MessageBoxA(NULL, expr, title, MB_OK | MB_ICONERROR);
    }

    void assertBox(const char* expr, const char* file, int line)
    {
        string msg;
        msg += "Assertion failed\n";
        msg += format("File: %s\n", file);
        msg += format("Line: %d\n", line);
        msg += format("Expression: %s", expr);
        auto result = ::MessageBoxA(NULL, msg.c_str(), "Swag meditation !", MB_CANCELTRYCONTINUE | MB_ICONERROR);
        switch (result)
        {
        case IDCANCEL:
            exit(-1);
            break;
        case IDTRYAGAIN:
            DebugBreak();
            break;
        case IDCONTINUE:
            break;
        }
    }

    bool getSwagFolder(string& folder)
    {
        HKEY key;
        if (RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", NULL, KEY_READ, &key) != ERROR_SUCCESS)
            return false;

        unsigned long type = REG_SZ, size = 1024;
        char          res[1024] = "";
        auto          result    = RegQueryValueExA(key, "SWAG_FOLDER", NULL, &type, (LPBYTE) &res[0], &size);
        RegCloseKey(key);
        folder = res;
        return result == ERROR_SUCCESS;
    }

    void setSwagFolder(const string& folder)
    {
        // SWAG_FOLDER
        LONG lRes = RegSetKeyValueA(HKEY_CURRENT_USER, "Environment", "SWAG_FOLDER", REG_SZ, folder.c_str(), (DWORD) folder.size() + 1);
        if (lRes != ERROR_SUCCESS)
        {
            g_Log.error(format(Msg0048, folder.c_str()));
            exit(-1);
        }

        g_Log.message(format("'SWAG_FOLDER' is '%s'\n", folder.c_str()));

        // PATH
        HKEY hKey;
        lRes = RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey);
        if (lRes != ERROR_SUCCESS)
        {
            g_Log.error(Msg0049);
            exit(-1);
        }

        char  szBuffer[512];
        DWORD dwBufferSize = sizeof(szBuffer);
        lRes               = RegQueryValueExA(hKey, "Path", 0, NULL, (LPBYTE) szBuffer, &dwBufferSize);
        if (lRes != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            g_Log.error(Msg0049);
            exit(-1);
        }

        if (!strstr(szBuffer, folder.c_str()))
        {
            strcat_s(szBuffer, ";");
            strcat_s(szBuffer, folder.c_str());
            lRes = RegSetValueExA(hKey, "Path", 0, REG_SZ, (const BYTE*) szBuffer, (DWORD) strlen(szBuffer) + 1);
            if (lRes != ERROR_SUCCESS)
            {
                RegCloseKey(hKey);
                g_Log.error(format(Msg0051, folder.c_str()));
                exit(-1);
            }

            g_Log.message("'PATH' environment variable has been changed\n\n");
            g_Log.message(szBuffer);
        }
        else
        {
            g_Log.message("'PATH' is up to date\n");
        }

        RegCloseKey(hKey);
    }

    bool touchFile(const fs::path& path)
    {
        HANDLE hFile = CreateFile(path.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return false;

        FILETIME   ft;
        SYSTEMTIME st;

        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);
        BOOL res = SetFileTime(hFile, (LPFILETIME) NULL, (LPFILETIME) NULL, &ft);

        CloseHandle(hFile);
        return res;
    }

    bool watch(function<void(const string&)> cb)
    {
        vector<HANDLE> allHandles;
        vector<string> allModules;

        // Tests modules
        for (auto& p : fs::directory_iterator(g_Workspace.testsPath))
        {
            HANDLE dwChangeHandle;
            dwChangeHandle = FindFirstChangeNotificationA(p.path().string().c_str(), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE);
            if (dwChangeHandle == INVALID_HANDLE_VALUE)
            {
                g_Log.error(Msg0052);
                exit(-1);
            }

            g_Log.verbose(format("watching folder '%s'", p.path().string().c_str()));
            allHandles.push_back(dwChangeHandle);
            allModules.push_back(p.path().string());
        }

        // Examples modules
        for (auto& p : fs::directory_iterator(g_Workspace.examplesPath))
        {
            HANDLE dwChangeHandle;
            dwChangeHandle = FindFirstChangeNotificationA(p.path().string().c_str(), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE);
            if (dwChangeHandle == INVALID_HANDLE_VALUE)
            {
                g_Log.error(Msg0052);
                exit(-1);
            }

            g_Log.verbose(format("watching folder '%s'", p.path().string().c_str()));
            allHandles.push_back(dwChangeHandle);
            allModules.push_back(p.path().string());
        }

        // Workspace modules
        for (auto& p : fs::directory_iterator(g_Workspace.modulesPath))
        {
            HANDLE dwChangeHandle;
            dwChangeHandle = FindFirstChangeNotificationA(p.path().string().c_str(), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE);
            if (dwChangeHandle == INVALID_HANDLE_VALUE)
            {
                g_Log.error(Msg0052);
                exit(-1);
            }

            g_Log.verbose(format("watching folder '%s'", p.path().string().c_str()));
            allHandles.push_back(dwChangeHandle);
            allModules.push_back(p.path().string());
        }

        // Start watching
        while (TRUE)
        {
            g_Log.message(format("Watching for file changes"));
            auto dwWaitStatus = WaitForMultipleObjects((DWORD) allHandles.size(), &allHandles[0], FALSE, INFINITE);
            int  numModule    = dwWaitStatus - WAIT_OBJECT_0;

            g_Log.message("File change detected");
            cb(allModules[numModule]);

            if (FindNextChangeNotification(allHandles[numModule]) == FALSE)
            {
                g_Log.error(Msg0055);
                exit(-1);
            }
        }

        return true;
    }

    string getTemporaryFolder()
    {
        char buffer[_MAX_PATH];
        if (GetTempPathA(_MAX_PATH, buffer))
            return buffer;
        return "";
    }

    uint64_t timerNow()
    {
        LARGE_INTEGER res;
        QueryPerformanceCounter(&res);
        return res.QuadPart;
    }

    double timerToSeconds(uint64_t timer)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return (double) timer / (double) freq.QuadPart;
    }

    bool atomicTestNull(void** ptr)
    {
        return InterlockedCompareExchangePointer(ptr, *ptr, nullptr) == nullptr;
    }

    void atomicSetIfNotNull(void** ptr, void* what)
    {
        InterlockedCompareExchangePointer(ptr, what, nullptr);
    }

    int8_t atomicAdd(int8_t* addr, int8_t value)
    {
        return _InterlockedExchangeAdd8((char*) addr, value);
    }

    int16_t atomicAdd(int16_t* addr, int16_t value)
    {
        return _InterlockedExchangeAdd16(addr, value);
    }

    int32_t atomicAdd(int32_t* addr, int32_t value)
    {
        return _InterlockedExchangeAdd((LONG*) addr, value);
    }

    int64_t atomicAdd(int64_t* addr, int64_t value)
    {
        return _InterlockedExchangeAdd64(addr, value);
    }

    int8_t atomicAnd(int8_t* addr, int8_t value)
    {
        return _InterlockedAnd8((char*) addr, value);
    }

    int16_t atomicAnd(int16_t* addr, int16_t value)
    {
        return _InterlockedAnd16(addr, value);
    }

    int32_t atomicAnd(int32_t* addr, int32_t value)
    {
        return _InterlockedAnd((LONG*) addr, value);
    }

    int64_t atomicAnd(int64_t* addr, int64_t value)
    {
        return _InterlockedAnd64(addr, value);
    }

    int8_t atomicOr(int8_t* addr, int8_t value)
    {
        return _InterlockedOr8((char*) addr, value);
    }

    int16_t atomicOr(int16_t* addr, int16_t value)
    {
        return _InterlockedOr16(addr, value);
    }

    int32_t atomicOr(int32_t* addr, int32_t value)
    {
        return _InterlockedOr((LONG*) addr, value);
    }

    int64_t atomicOr(int64_t* addr, int64_t value)
    {
        return _InterlockedOr64(addr, value);
    }

    int8_t atomicXor(int8_t* addr, int8_t value)
    {
        return _InterlockedXor8((char*) addr, value);
    }

    int16_t atomicXor(int16_t* addr, int16_t value)
    {
        return _InterlockedXor16(addr, value);
    }

    int32_t atomicXor(int32_t* addr, int32_t value)
    {
        return _InterlockedXor((LONG*) addr, value);
    }

    int64_t atomicXor(int64_t* addr, int64_t value)
    {
        return _InterlockedXor64(addr, value);
    }

    int8_t atomicXchg(int8_t* addr, int8_t replaceWith)
    {
        return InterlockedExchange8((char*) addr, replaceWith);
    }

    int16_t atomicXchg(int16_t* addr, int16_t replaceWith)
    {
        return InterlockedExchange16(addr, replaceWith);
    }

    int32_t atomicXchg(int32_t* addr, int32_t replaceWith)
    {
        return InterlockedExchange((LONG*) addr, (LONG) replaceWith);
    }

    int64_t atomicXchg(int64_t* addr, int64_t replaceWith)
    {
        return InterlockedExchange64(addr, replaceWith);
    }

    int8_t atomicCmpXchg(int8_t* addr, int8_t compareTo, int8_t replaceWith)
    {
        return _InterlockedCompareExchange8((char*) addr, replaceWith, compareTo);
    }

    int16_t atomicCmpXchg(int16_t* addr, int16_t compareTo, int16_t replaceWith)
    {
        return _InterlockedCompareExchange16(addr, replaceWith, compareTo);
    }

    int32_t atomicCmpXchg(int32_t* addr, int32_t compareTo, int32_t replaceWith)
    {
        return _InterlockedCompareExchange((LONG*) addr, (LONG) replaceWith, (LONG) compareTo);
    }

    int64_t atomicCmpXchg(int64_t* addr, int64_t compareTo, int64_t replaceWith)
    {
        return _InterlockedCompareExchange64(addr, replaceWith, compareTo);
    }

    int32_t memcmp(const void* dst, const void* src, size_t size)
    {
        auto len = RtlCompareMemory(dst, src, size);
        if (len == size)
            return 0;
        auto p1 = (const int8_t*) dst;
        auto p2 = (const int8_t*) src;
        return p1[len] - p2[len];
    }

    uint8_t bitcountnz(uint8_t value)
    {
        return (uint8_t) __popcnt16(value);
    }

    uint16_t bitcountnz(uint16_t value)
    {
        return __popcnt16(value);
    }

    uint32_t bitcountnz(uint32_t value)
    {
        return __popcnt(value);
    }

    uint64_t bitcountnz(uint64_t value)
    {
        return __popcnt64(value);
    }

    uint8_t bitcounttz(uint8_t value)
    {
        unsigned long index;
        auto          res = _BitScanForward(&index, value);
        if (!res)
            return 8;
        return (uint8_t) index;
    }

    uint16_t bitcounttz(uint16_t value)
    {
        unsigned long index;
        auto          res = _BitScanForward(&index, value);
        if (!res)
            return 16;
        return (uint16_t) index;
    }

    uint32_t bitcounttz(uint32_t value)
    {
        unsigned long index;
        auto          res = _BitScanForward(&index, value);
        if (!res)
            return 32;
        return (uint32_t) index;
    }

    uint64_t bitcounttz(uint64_t value)
    {
        unsigned long index;
        auto          res = _BitScanForward64(&index, value);
        if (!res)
            return 64;
        return (uint64_t) index;
    }

    uint8_t bitcountlz(uint8_t value)
    {
        unsigned long index;
        auto          res = _BitScanReverse(&index, value);
        if (!res)
            return 8;
        return (uint8_t)(8 - index - 1);
    }

    uint16_t bitcountlz(uint16_t value)
    {
        unsigned long index;
        auto          res = _BitScanReverse(&index, value);
        if (!res)
            return 16;
        return (uint8_t)(16 - index - 1);
    }

    uint32_t bitcountlz(uint32_t value)
    {
        unsigned long index;
        auto          res = _BitScanReverse(&index, value);
        if (!res)
            return 32;
        return (uint8_t)(32 - index - 1);
    }

    uint64_t bitcountlz(uint64_t value)
    {
        unsigned long index;
        auto          res = _BitScanReverse64(&index, value);
        if (!res)
            return 64;
        return (uint8_t)(64 - index - 1);
    }

    uint16_t byteswap(uint16_t value)
    {
        return _byteswap_ushort(value);
    }

    uint32_t byteswap(uint32_t value)
    {
        return _byteswap_ulong(value);
    }

    uint64_t byteswap(uint64_t value)
    {
        return _byteswap_uint64(value);
    }

}; // namespace OS

#endif