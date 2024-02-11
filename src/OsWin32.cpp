#include "pch.h"
#ifdef _WIN32
#include <DbgHelp.h>
#include "ByteCodeDebugger.h"
#include "Context.h"
#include "ErrorIds.h"
#include "Report.h"
#include "SCBE.h"
#include "SCBE_Coff.h"
#include "TypeManager.h"
#include "Workspace.h"

#pragma comment(lib, "dbghelp.lib")

namespace OS
{
    namespace
    {
        BackendTarget      g_NativeTarget;
        HANDLE             g_ConsoleHandle     = nullptr;
        WORD               g_DefaultAttributes = 0;
        thread_local void* g_ExceptionParams[4];
        Path               g_WinSdkFolder;
    }

    bool getWinSdk()
    {
        static Mutex mt;
        ScopedLock   lk(mt);

        if (g_WinSdkFolder == "<error>")
            return false;
        if (!g_WinSdkFolder.empty())
            return true;

        g_WinSdkFolder = "<error";

        HKEY hKey;
        auto rc = RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(SOFTWARE\Microsoft\Windows Kits\Installed Roots)", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY | KEY_ENUMERATE_SUB_KEYS, &hKey);
        if (rc != S_OK)
            return false;

        DWORD length;
        rc = RegQueryValueExW(hKey, L"KitsRoot10", nullptr, nullptr, nullptr, &length);
        if (rc != S_OK)
            return false;

        const auto value = new wchar_t[length + 1];
        rc               = RegQueryValueExW(hKey, L"KitsRoot10", nullptr, nullptr, (LPBYTE) value, &length);
        RegCloseKey(hKey);
        if (rc != S_OK)
            return false;
        value[length] = 0;

        // Convert to UTF8
        const std::wstring wstr{value};
        const int          sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int) wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string        str(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int) wstr.size(), str.data(), sizeNeeded, nullptr, nullptr);

        g_WinSdkFolder = str.c_str();
        g_WinSdkFolder.append("Lib");

        int                                                           bestVersion[4] = {0};
        Utf8                                                          bestName;
        visitFolders(g_WinSdkFolder.string().c_str(), [&](const char* cFileName)
        {
            int        i0, i1, i2, i3;
            const auto success = sscanf_s(cFileName, "%d.%d.%d.%d", &i0, &i1, &i2, &i3);
            if (success < 4)
                return;

            if (i0 < bestVersion[0])
                return;
            if (i0 == bestVersion[0])
            {
                if (i1 < bestVersion[1])
                    return;
                if (i1 == bestVersion[1])
                {
                    if (i2 < bestVersion[2])
                        return;
                    if (i2 == bestVersion[2])
                    {
                        if (i3 < bestVersion[3])
                            return;
                    }
                }
            }

            bestName       = cFileName;
            bestVersion[0] = i0;
            bestVersion[1] = i1;
            bestVersion[2] = i2;
            bestVersion[3] = i3;
        });

        if (bestVersion[0] == 0)
            return false;

        g_WinSdkFolder.append(bestName);
        if (g_CommandLine.verbosePath)
            g_Log.messageVerbose(FMT("winsdk path is [[%s]]", g_WinSdkFolder.string().c_str()));

        return true;
    }

    bool setupBuild()
    {
        if (g_CommandLine.target.os != SwagTargetOs::Windows)
            return true;

        if (!getWinSdk())
        {
            Report::error(Err(Fat0036));
            g_Log.lock();
            g_Log.setColor(LogColor::Cyan);
            g_Log.print("=> in order to build a windows executable or dll, you must install the latest version of the windows 10/11 sdk!\n");
            g_Log.print("=> you can try https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/\n");
            g_Log.setDefaultColor();
            g_Log.unlock();
            return false;
        }

        SWAG_ASSERT(g_CommandLine.target.arch == SwagTargetArch::X86_64);

        Path p0 = g_WinSdkFolder;
        p0.append("um\\x64");
        g_CommandLine.libPaths.push_back(p0);
        Path p1 = g_WinSdkFolder;
        p1.append("ucrt\\x64");
        g_CommandLine.libPaths.push_back(p1);
        return true;
    }

    void setup()
    {
        // Current target
        g_NativeTarget.os   = SwagTargetOs::Windows;
        g_NativeTarget.arch = SwagTargetArch::X86_64;
        g_NativeTarget.cpu  = llvm::sys::getHostCPUName().str().c_str();

        // We do not want assert, but just reports of the CRT
        if (!IsDebuggerPresent())
        {
            _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        }

        // Log
        g_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleOutputCP(65001);

        DWORD dwMode = 0;
        GetConsoleMode(g_ConsoleHandle, &dwMode);
        SetConsoleMode(g_ConsoleHandle, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(g_ConsoleHandle, &info);
        g_DefaultAttributes = info.wAttributes;
    }

    const BackendTarget& getNativeTarget()
    {
        return g_NativeTarget;
    }

    bool doProcess(Module* module, const Utf8& cmdline, const string& currentDirectory, uint32_t& numErrors)
    {
        STARTUPINFOA        si;
        PROCESS_INFORMATION pi;
        SECURITY_ATTRIBUTES saAttr;
        HANDLE              hChildStdoutRd, hChildStdoutWr;
        DWORD               dwRead;
        CHAR                chBuf[4096];
        DWORD               exit;

        const UINT errmode = GetErrorMode();
        SetErrorMode(SEM_FAILCRITICALERRORS);

        // Create a pipe to receive compiler results
        ZeroMemory(&saAttr, sizeof(saAttr));
        saAttr.nLength              = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle       = TRUE;
        saAttr.lpSecurityDescriptor = nullptr;
        if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
        {
            Report::error(FMT(Err(Err0628), cmdline.c_str()));
            return false;
        }

        // Create process
        ZeroMemory(&si, sizeof(si));
        si.cb         = sizeof(si);
        si.hStdError  = hChildStdoutWr;
        si.hStdOutput = hChildStdoutWr;
        si.dwFlags    = STARTF_USESTDHANDLES;
        ZeroMemory(&pi, sizeof(pi));

        {
            if (!CreateProcessA(nullptr,
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
                Report::errorOS(FMT(Err(Err0629), cmdline.c_str()));
                return false;
            }
        }

        // Wait until child process exits
        Utf8 strout;
        bool ok  = true;
        chBuf[0] = 0;
        while (true)
        {
            PeekNamedPipe(hChildStdoutRd, chBuf, 4096, &dwRead, nullptr, nullptr);
            if (dwRead || !strout.empty())
            {
                const bool notLast = dwRead != 0;

                // Read compiler results
                if (notLast && ReadFile(hChildStdoutRd, chBuf, 4096, &dwRead, nullptr))
                {
                    strout.append(chBuf, dwRead);
                    while (dwRead == 4096)
                    {
                        if (ReadFile(hChildStdoutRd, chBuf, 4096, &dwRead, nullptr))
                            strout.append(chBuf, dwRead);
                    }
                }

                // Process result
                Vector<Utf8> lines;
                Utf8::tokenize(strout, '\n', lines, true);
                strout.clear();
                if (notLast)
                {
                    strout = lines.back();
                    lines.pop_back();
                }

                for (auto oneLine : lines)
                {
                    if (oneLine.empty())
                    {
                        g_Log.lock();
                        g_Log.eol();
                        g_Log.unlock();
                        continue;
                    }

                    if (oneLine.back() == '\r')
                        oneLine.removeBack();

                    // Error
                    const auto pz0 = strstr(oneLine.c_str(), "error:");
                    const auto pz1 = strstr(oneLine.c_str(), "panic:");
                    if (pz0 || pz1)
                    {
                        numErrors++;
                        ok = false;
                    }

                    // Messages
                    g_Log.lock();
                    g_Log.setDefaultColor();
                    g_Log.print(oneLine + "\n");
                    g_Log.unlock();
                }

                continue;
            }

            GetExitCodeProcess(pi.hProcess, &exit);
            if (exit != STILL_ACTIVE)
            {
                switch (exit)
                {
                case 0:
                case (DWORD) -666:
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
        CloseHandle(hChildStdoutWr);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        SetErrorMode(errmode);
        return ok;
    }

    Path getExePath()
    {
        char az[_MAX_PATH];
        GetModuleFileNameA(nullptr, az, _MAX_PATH);
        return az;
    }

    Utf8 getLastErrorAsString()
    {
        // Get the error message, if any.
        const DWORD errorMessageID = GetLastError();
        if (errorMessageID == 0)
            return std::string(); // No error message has been recorded

        LPSTR        messageBuffer = nullptr;
        const size_t size          = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                                    nullptr,
                                                    errorMessageID,
                                                    MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                                                    (LPSTR) &messageBuffer,
                                                    0,
                                                    nullptr);

        // Remove unwanted characters
        char* pz = messageBuffer;
        for (size_t i = 0; i < size; i++)
        {
            if (pz[i] < 32)
                pz[i] = 32;
        }

        Utf8 message(messageBuffer, (uint32_t) size);
        message.trim();
        if (message.length() && message.back() == '.')
            message.removeBack();

        // Free the buffer.
        LocalFree(messageBuffer);
        return message;
    }

    void* loadLibrary(const char* name)
    {
        return (void*) LoadLibraryA(name);
    }

    void* getProcAddress(void* handle, const char* name)
    {
        return (void*) GetProcAddress((HMODULE) handle, name);
    }

    uint64_t getFileWriteTime(const char* fileName)
    {
        const auto hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
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

    // Hack. Don't know why i have some flush problems on written files.
    void ensureFileIsWritten(const char* fileName)
    {
        auto writeTime = getFileWriteTime(fileName);
        while (!writeTime)
        {
            this_thread::yield();
            writeTime = getFileWriteTime(fileName);
        }
    }

    void visitFiles(const char* folder, const function<void(const char*)>& user)
    {
        WIN32_FIND_DATAA findfile;
        Utf8             searchPath = folder;
        searchPath += "\\*";
        const HANDLE h = FindFirstFileA(searchPath.c_str(), &findfile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    continue;
                user(findfile.cFileName);
            } while (FindNextFileA(h, &findfile));

            FindClose(h);
        }
    }

    void visitFolders(const char* folder, const function<void(const char*)>& user, const char* match)
    {
        WIN32_FIND_DATAA findfile;
        Utf8             searchPath = folder;
        searchPath += "\\";
        searchPath += match;
        const HANDLE h = FindFirstFileA(searchPath.c_str(), &findfile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (!(findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    continue;
                if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                    continue;
                user(findfile.cFileName);
            } while (FindNextFileA(h, &findfile));

            FindClose(h);
        }
    }

    void visitFilesFolders(const char* folder, const function<void(uint64_t, const char*, bool)>& user)
    {
        WIN32_FIND_DATAA findfile;
        Utf8             searchPath = folder;
        searchPath += "\\*";
        const HANDLE h = FindFirstFileA(searchPath.c_str(), &findfile);
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

                const bool isFolder = findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
                user(writeTime, findfile.cFileName, isFolder);

            } while (FindNextFileA(h, &findfile));

            FindClose(h);
        }
    }

    void visitFilesRec(const char* folder, const function<void(const char*)>& user)
    {
        WIN32_FIND_DATAA findfile;
        Path             searchPath = folder;
        searchPath += "\\*";

        Path         path = folder;
        const HANDLE h    = FindFirstFileA(searchPath.string().c_str(), &findfile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                path = folder;
                path.append(findfile.cFileName);

                if (findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if ((findfile.cFileName[0] == '.') && (!findfile.cFileName[1] || (findfile.cFileName[1] == '.' && !findfile.cFileName[2])))
                        continue;
                    visitFilesRec(path.string().c_str(), user);
                }
                else
                {
                    user(path.string().c_str());
                }

            } while (FindNextFileA(h, &findfile));

            FindClose(h);
        }
    }

    constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
    using THREADNAME_INFO = struct tagTHREADNAME_INFO
    {
        DWORD  dwType;     // Must be 0x1000.
        LPCSTR szName;     // Pointer to name (in user addr space).
        DWORD  dwThreadID; // Thread ID (-1=caller thread).
        DWORD  dwFlags;    // Reserved for future use, must be zero.
    };
#pragma pack(pop)

    void setThreadName(uint32_t dwThreadID, const char* threadName)
    {
        THREADNAME_INFO info;
        info.dwType     = 0x1000;
        info.szName     = threadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags    = 0;

        SWAG_TRY
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*) &info);
        }
        SWAG_EXCEPT (SWAG_EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }
    void setThreadName(const char* threadName)
    {
        setThreadName(GetCurrentThreadId(), threadName);
    }

    void setThreadName(thread* thread, const char* threadName)
    {
        const auto  handle   = static_cast<HANDLE>(thread->native_handle());
        const DWORD threadId = GetThreadId(handle);
        setThreadName(threadId, threadName);
        // SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
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
        MessageBoxA(nullptr, expr, title, MB_OK | MB_ICONERROR);
    }

    void exit(int code)
    {
        ExitProcess(code);
    }

    bool isDebuggerAttached()
    {
        return IsDebuggerPresent() ? true : false;
    }

    Utf8 captureStack()
    {
        static constexpr int SYM_LEN_NAME = 128;

        TCHAR   sym[sizeof(SYMBOL_INFO) + SYM_LEN_NAME * sizeof(TCHAR)];
        DWORD64 displacement = 0;

        const auto psym    = (SYMBOL_INFO*) sym;
        psym->SizeOfStruct = sizeof(SYMBOL_INFO);
        psym->MaxNameLen   = SYM_LEN_NAME;

        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        Utf8       str;
        void*      where[100];
        const auto nb = RtlCaptureStackBackTrace(2, sizeof(where) / sizeof(void*), where, nullptr);
        if (SymInitialize(GetCurrentProcess(), nullptr, TRUE))
        {
            for (int i = 0; i < nb; i++)
            {
                displacement         = 0;
                const auto hasSymbol = SymFromAddr(GetCurrentProcess(), (ULONG64) where[i], (DWORD64*) &displacement, psym);
                displacement         = 0;
                const auto hasLine   = SymGetLineFromAddr64(GetCurrentProcess(), (ULONG64) where[i], (DWORD*) &displacement, &line);

                if (hasSymbol)
                {
                    str += Log::colorToVTS(LogColor::Name);
                    str += psym->Name;
                    if (hasLine)
                    {
                        str += Log::colorToVTS(LogColor::Location);
                        str += FMT(" %s:%d", line.FileName, line.LineNumber);
                    }

                    str += "\n";
                }
            }
        }

        return str;
    }

    void raiseException(int code, const char* msg)
    {
        msg                  = msg ? _strdup(msg) : nullptr;
        g_ExceptionParams[0] = nullptr;
        g_ExceptionParams[1] = (void*) msg;
        g_ExceptionParams[2] = (void*) (msg ? strlen(msg) : 0);
        g_ExceptionParams[3] = (void*) SwagExceptionKind::Panic;
        RaiseException(code, 0, 4, (ULONG_PTR*) &g_ExceptionParams[0]);
    }

    void assertBox(const char* expr, const char* file, int line)
    {
        char msg[2048];
        msg[0] = 0;

        strcat_s(msg, "Assertion failed\n");

        strcat_s(msg, "File: ");
        strcat_s(msg, file);
        strcat_s(msg, "\n");

        strcat_s(msg, "Line: ");
        char lineB[64];
        _itoa_s(line, lineB, 10);
        strcat_s(msg, lineB);
        strcat_s(msg, "\n");

        strcat_s(msg, "Expression: ");
        strcat_s(msg, expr);
        strcat_s(msg, "\n");

        const auto result = MessageBoxA(nullptr, msg, "Swag meditation !", MB_CANCELTRYCONTINUE | MB_ICONERROR);
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
        default:
            break;
        }
    }

    Path getTemporaryFolder()
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
        return InterlockedExchange((LONG*) addr, replaceWith);
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

    uint8_t bitCountNz(uint8_t value)
    {
        return (uint8_t) __popcnt16(value);
    }

    uint16_t bitCountNz(uint16_t value)
    {
        return __popcnt16(value);
    }

    uint32_t bitCountNz(uint32_t value)
    {
        return __popcnt(value);
    }

    uint64_t bitCountNz(uint64_t value)
    {
        return __popcnt64(value);
    }

    uint8_t bitCountTz(uint8_t value)
    {
        unsigned long index;
        const auto    res = _BitScanForward(&index, value);
        if (!res)
            return 8;
        return (uint8_t) index;
    }

    uint16_t bitCountTz(uint16_t value)
    {
        unsigned long index;
        const auto    res = _BitScanForward(&index, value);
        if (!res)
            return 16;
        return (uint16_t) index;
    }

    uint32_t bitCountTz(uint32_t value)
    {
        unsigned long index;
        const auto    res = _BitScanForward(&index, value);
        if (!res)
            return 32;
        return (uint32_t) index;
    }

    uint64_t bitCountTz(uint64_t value)
    {
        unsigned long index;
        const auto    res = _BitScanForward64(&index, value);
        if (!res)
            return 64;
        return (uint64_t) index;
    }

    uint8_t bitCountLz(uint8_t value)
    {
        unsigned long index;
        const auto    res = _BitScanReverse(&index, value);
        if (!res)
            return 8;
        return (uint8_t) (8 - index - 1);
    }

    uint16_t bitCountLz(uint16_t value)
    {
        unsigned long index;
        const auto    res = _BitScanReverse(&index, value);
        if (!res)
            return 16;
        return (uint8_t) (16 - index - 1);
    }

    uint32_t bitCountLz(uint32_t value)
    {
        unsigned long index;
        const auto    res = _BitScanReverse(&index, value);
        if (!res)
            return 32;
        return (uint8_t) (32 - index - 1);
    }

    uint64_t bitCountLz(uint64_t value)
    {
        unsigned long index;
        const auto    res = _BitScanReverse64(&index, value);
        if (!res)
            return 64;
        return (uint8_t) (64 - index - 1);
    }

    uint16_t byteSwap(uint16_t value)
    {
        return _byteswap_ushort(value);
    }

    uint32_t byteSwap(uint32_t value)
    {
        return _byteswap_ulong(value);
    }

    uint64_t byteSwap(uint64_t value)
    {
        return _byteswap_uint64(value);
    }

    thread_local SCBE_X64 g_X64GenFFI;

    void ffi(ByteCodeRunContext* context, void* foreignPtr, TypeInfoFuncAttr* typeInfoFunc, const VectorNative<uint32_t>& pushRAParam, void* retCopyAddr)
    {
        const auto& cc         = typeInfoFunc->getCallConv();
        const auto  returnType = TypeManager::concreteType(typeInfoFunc->returnType);

        uint32_t stackSize = (uint32_t) max(cc.paramByRegisterCount, pushRAParam.size()) * sizeof(void*);
        stackSize += sizeof(void*);
        MK_ALIGN16(stackSize);

        static constexpr int JIT_SIZE_BUFFER = 16 * 1024;
        auto&                gen             = g_X64GenFFI;
        if (!gen.concat.firstBucket)
        {
            // Generate a buffer big enough to store the call, and be aware that this could be recursive, that's
            // why the buffer is kind of big
            auto& concat = gen.concat;
            concat.init(0);
            concat.firstBucket->capacity = JIT_SIZE_BUFFER + 128;
            concat.firstBucket->data     = (uint8_t*) VirtualAlloc(nullptr, gen.concat.firstBucket->capacity, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            concat.currentSP             = gen.concat.firstBucket->data;

            // We need to generate unwind stuff to get a correct callstack, and in case the runtime caises an exception
            // with 'RaiseException' (panic, error, etc.)
            // The function information is always the same, that's why we generate only one table per SCBE_X64.
            VectorNative<CPURegister> unwindRegs;
            VectorNative<uint32_t>    unwindOffsetRegs;
            VectorNative<uint16_t>    unwind;

            // Fake emit in order to compute the unwind infos
            gen.emit_Push(RDI);
            unwindRegs.push_back(RDI);
            unwindOffsetRegs.push_back(gen.concat.totalCount());
            gen.emit_OpN_Immediate(RSP, stackSize, CPUOp::SUB, CPUBits::B64);
            const auto sizeProlog = gen.concat.totalCount();
            SCBE_Coff::computeUnwind(unwindRegs, unwindOffsetRegs, stackSize, sizeProlog, unwind);

            // Add function table
            auto* rtFunc              = new RUNTIME_FUNCTION(); // leak, but it's fine
            rtFunc->BeginAddress      = 0;
            rtFunc->EndAddress        = JIT_SIZE_BUFFER;
            rtFunc->UnwindInfoAddress = JIT_SIZE_BUFFER;
            uint32_t offset           = 0;
            concat.currentSP          = gen.concat.firstBucket->data + JIT_SIZE_BUFFER;
            SCBE_Coff::emitUnwind(gen.concat, offset, sizeProlog, unwind);
            RtlAddFunctionTable(rtFunc, 1, (DWORD64) gen.concat.firstBucket->data);

            // Restore back the start of the buffer
            concat.currentSP = gen.concat.firstBucket->data;
        }

        const auto startOffset = gen.concat.currentSP - gen.concat.firstBucket->data;
        SWAG_ASSERT(startOffset < JIT_SIZE_BUFFER);

        gen.emit_Push(RDI);
        gen.emit_OpN_Immediate(RSP, stackSize, CPUOp::SUB, CPUBits::B64);
        gen.emit_Load64_Immediate(RDI, (uint64_t) context->sp, true);
        gen.emit_Call_Parameters(typeInfoFunc, pushRAParam, 0, retCopyAddr);
        gen.emit_Load64_Immediate(RAX, (uint64_t) foreignPtr, true);
        gen.emit_Call_Indirect(RAX);

        if (!returnType->isVoid() && !retCopyAddr)
        {
            gen.emit_Load64_Immediate(RDI, (uint64_t) context->registersRR, true);
            gen.emit_Call_Result(typeInfoFunc, 0);
        }

        gen.emit_OpN_Immediate(RSP, stackSize, CPUOp::ADD, CPUBits::B64);
        gen.emit_Pop(RDI);
        gen.emit_Ret();

        // The real deal : make the call
        using funcPtr = void(*)();
        const auto ptr = (funcPtr) (gen.concat.firstBucket->data + startOffset);
        ptr();

        gen.concat.currentSP = (uint8_t*) ptr;
    }

    Utf8 getClipboardString()
    {
        if (!OpenClipboard(GetDesktopWindow()))
            return "";

        if (IsClipboardFormatAvailable(CF_TEXT))
        {
            const auto hglob = GetClipboardData(CF_TEXT);
            if (hglob)
            {
                const auto pz     = GlobalLock(hglob);
                Utf8       result = (const char*) pz;
                GlobalUnlock(pz);
                return result;
            }
        }

        CloseClipboard();
        return "";
    }

    Key promptChar(int& c, bool& ctrl, bool& shift)
    {
        static INPUT_RECORD buffer[1024];
        static DWORD        dwRead      = 0;
        static DWORD        dwReadIndex = 0;

        const auto hStdin = GetStdHandle(STD_INPUT_HANDLE);
        SetConsoleMode(hStdin, ENABLE_WINDOW_INPUT);

        while (true)
        {
            if (dwReadIndex >= dwRead)
            {
                dwReadIndex = 0;
                if (!ReadConsoleInput(hStdin, buffer, 1024, &dwRead))
                    continue;
            }

            while (dwReadIndex < dwRead)
            {
                const auto& evt = buffer[dwReadIndex++];
                if (evt.EventType != KEY_EVENT || !evt.Event.KeyEvent.bKeyDown)
                    continue;

                ctrl  = GetAsyncKeyState(VK_CONTROL) < 0;
                shift = GetAsyncKeyState(VK_SHIFT) < 0;

                switch (evt.Event.KeyEvent.wVirtualKeyCode)
                {
                case VK_RETURN:
                    return Key::Return;
                case VK_LEFT:
                    return Key::Left;
                case VK_RIGHT:
                    return Key::Right;
                case VK_UP:
                    return Key::Up;
                case VK_DOWN:
                    return Key::Down;
                case VK_HOME:
                    return Key::Home;
                case VK_END:
                    return Key::End;
                case VK_DELETE:
                    return Key::Delete;
                case VK_BACK:
                    return Key::Back;
                case VK_TAB:
                    return Key::Tab;
                case VK_ESCAPE:
                    return Key::Escape;
                case VK_CONTROL:
                    continue;

                default:
                    if (ctrl && evt.Event.KeyEvent.wVirtualKeyCode == 'V')
                        return Key::PasteFromClipboard;

                    c = evt.Event.KeyEvent.uChar.AsciiChar;
                    if (c >= ' ' && c <= 127)
                        return Key::Ascii;

                    continue;
                }
            }
        }
    }
}

#endif
