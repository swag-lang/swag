#include "pch.h"
#include "Backend/SCBE/Encoder/X64/ScbeX64.h"
#include "Core/Math.h"
#ifdef _WIN32
#include "Backend/Context.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Backend/SCBE/Obj/ScbeCoff.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Report/Log.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#ifdef SWAG_STATS
#include "Core/Timer.h"
#endif
#include "Wmf/Workspace.h"
#include <DbgHelp.h>

#pragma comment(lib, "dbghelp.lib")

namespace
{
    BackendTarget      g_NativeTarget;
    HANDLE             g_ConsoleHandle     = nullptr;
    WORD               g_DefaultAttributes = 0;
    thread_local void* g_ExceptionParams[4];
    Path               g_WinSdkFolder;

    constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
    struct ThreadNameInfo
    {
        DWORD  dwType;     // Must be 0x1000.
        LPCSTR szName;     // Pointer to name (in user address space).
        DWORD  dwThreadID; // Thread ID (-1=caller thread).
        DWORD  dwFlags;    // Reserved for future use, must be zero.
    };
#pragma pack(pop)

    void setThreadNamePriv(uint32_t dwThreadID, const char* threadName)
    {
        ThreadNameInfo info;
        info.dwType     = 0x1000;
        info.szName     = threadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags    = 0;

        SWAG_TRY
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
        }
        SWAG_EXCEPT(SWAG_EXCEPTION_EXECUTE_HANDLER)
        {
        }
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
        rc               = RegQueryValueExW(hKey, L"KitsRoot10", nullptr, nullptr, reinterpret_cast<LPBYTE>(value), &length);
        RegCloseKey(hKey);
        if (rc != S_OK)
            return false;
        value[length] = 0;

        // Convert to UTF8
        const std::wstring wStr{value};
        const int          sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wStr.data(), static_cast<int>(wStr.size()), nullptr, 0, nullptr, nullptr);
        std::string        str(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, wStr.data(), static_cast<int>(wStr.size()), str.data(), sizeNeeded, nullptr, nullptr);

        g_WinSdkFolder = str.c_str();
        g_WinSdkFolder.append("Lib");

        int  bestVersion[4] = {};
        Utf8 bestName;
        OS::visitFolders(g_WinSdkFolder, [&](const char* cFileName) {
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
            g_Log.messageVerbose(form("winsdk path is [[%s]]", g_WinSdkFolder.cstr()));

        return true;
    }
}

namespace OS
{
    bool setupBuild()
    {
        if (g_CommandLine.target.os != SwagTargetOs::Windows)
            return true;

        if (!getWinSdk())
        {
            Report::error(toErr(Fat0036));
            g_Log.lock();
            g_Log.setColor(LogColor::Cyan);
            g_Log.write("=> in order to build a windows executable or dll, you must install the latest version of the windows 10/11 sdk!\n");
            g_Log.write("=> you can try https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/\n");
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

        // We do not want to assert, but just reports of the CRT
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

    bool doProcess(Module* /*module*/, const Utf8& cmdline, const std::string& currentDirectory, uint32_t& numErrors)
    {
        STARTUPINFOA        si;
        PROCESS_INFORMATION pi;
        SECURITY_ATTRIBUTES saAttr;
        HANDLE              hChildStdoutRd, hChildStdoutWr;
        DWORD               dwRead;
        CHAR                chBuf[4096];
        DWORD               exit;

        const UINT errMode = GetErrorMode();
        SetErrorMode(SEM_FAILCRITICALERRORS);

        // Create a pipe to receive compiler results
        ZeroMemory(&saAttr, sizeof(saAttr));
        saAttr.nLength              = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle       = TRUE;
        saAttr.lpSecurityDescriptor = nullptr;
        if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
        {
            Report::error(formErr(Err0765, cmdline.cstr()));
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
                                const_cast<LPSTR>(cmdline.cstr()),
                                nullptr,
                                nullptr,
                                TRUE,
                                CREATE_NO_WINDOW,
                                nullptr,
                                currentDirectory.c_str(),
                                &si,
                                &pi))
            {
                Report::errorOS(formErr(Err0764, cmdline.cstr()));
                return false;
            }
        }

        // Wait until child process exits
        Utf8 strOut;
        bool ok  = true;
        chBuf[0] = 0;
        while (true)
        {
            PeekNamedPipe(hChildStdoutRd, chBuf, 4096, &dwRead, nullptr, nullptr);
            if (dwRead || !strOut.empty())
            {
                const bool notLast = dwRead != 0;

                // Read compiler results
                if (notLast && ReadFile(hChildStdoutRd, chBuf, 4096, &dwRead, nullptr))
                {
                    strOut.append(chBuf, dwRead);
                    while (dwRead == 4096)
                    {
                        if (ReadFile(hChildStdoutRd, chBuf, 4096, &dwRead, nullptr))
                            strOut.append(chBuf, dwRead);
                    }
                }

                // Process result
                Vector<Utf8> lines;
                Utf8::tokenize(strOut, '\n', lines, true);
                strOut.clear();
                if (notLast)
                {
                    strOut = lines.back();
                    lines.pop_back();
                }

                for (auto oneLine : lines)
                {
                    if (oneLine.empty())
                    {
                        g_Log.lock();
                        g_Log.writeEol();
                        g_Log.unlock();
                        continue;
                    }

                    if (oneLine.back() == '\r')
                        oneLine.removeBack();

                    // Error
                    const auto pz0 = strstr(oneLine, "error:");
                    const auto pz1 = strstr(oneLine, "panic:");
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
                    case static_cast<DWORD>(-666):
                        break;
                    case STATUS_ACCESS_VIOLATION:
                        g_Log.lock();
                        g_Log.setColor(LogColor::Red);
                        numErrors++;
                        std::cout << cmdline.cstr();
                        std::cout << ": access violation during process execution\n";
                        g_Log.setDefaultColor();
                        g_Log.unlock();
                        ok = false;
                        break;
                    default:
                        g_Log.lock();
                        g_Log.setColor(LogColor::Red);
                        numErrors++;
                        std::cout << cmdline.cstr();
                        std::cout << ": process execution failed\n";
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

        SetErrorMode(errMode);
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
            return Utf8{};

        DWORD langId = 0;
        GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_ILANGUAGE | LOCALE_RETURN_NUMBER, reinterpret_cast<LPWSTR>(&langId), sizeof(langId) / sizeof(wchar_t));

        LPSTR messageBuffer = nullptr;

        const size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                           nullptr,
                                           errorMessageID,
                                           langId,
                                           reinterpret_cast<LPSTR>(&messageBuffer),
                                           0,
                                           nullptr);
        if (!size)
            return Utf8{};

        // Remove unwanted characters
        const auto pz = messageBuffer;
        for (uint32_t i = 0; i < size; i++)
        {
            const uint8_t c = static_cast<uint8_t>(pz[i]);
            if (c < 32)
                pz[i] = 32;
        }

        Utf8 message(messageBuffer, static_cast<uint32_t>(size));
        message.trim();
        if (!message.empty() && message.back() == '.')
            message.removeBack();

        // Free the buffer.
        LocalFree(messageBuffer);
        return message;
    }

    void* loadLibrary(const char* name)
    {
        return LoadLibraryA(name);
    }

    void* getProcAddress(void* handle, const char* name)
    {
        return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle), name));
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
            result = static_cast<uint64_t>(ftWrite.dwHighDateTime) << 32;
            result |= ftWrite.dwLowDateTime;
        }

        CloseHandle(hFile);
        return result;
    }

    // Hack. Don't know why I have some flush problems on written files.
    void ensureFileIsWritten(const char* fileName)
    {
        auto writeTime = getFileWriteTime(fileName);
        while (!writeTime)
        {
            std::this_thread::yield();
            writeTime = getFileWriteTime(fileName);
        }
    }

    void visitFiles(const char* folder, const std::function<void(const char*)>& user)
    {
        WIN32_FIND_DATAA findFile;
        Utf8             searchPath = folder;
        searchPath += "\\*";
        const HANDLE h = FindFirstFileA(searchPath, &findFile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (findFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    continue;
                user(findFile.cFileName);
            } while (FindNextFileA(h, &findFile));

            FindClose(h);
        }
    }

    void visitFolders(const char* folder, const std::function<void(const char*)>& user, const char* match)
    {
        WIN32_FIND_DATAA findFile;
        Utf8             searchPath = folder;
        searchPath += "\\";
        searchPath += match;
        const HANDLE h = FindFirstFileA(searchPath, &findFile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (!(findFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    continue;
                if (findFile.cFileName[0] == '.' && (!findFile.cFileName[1] || (findFile.cFileName[1] == '.' && !findFile.cFileName[2])))
                    continue;
                user(findFile.cFileName);
            } while (FindNextFileA(h, &findFile));

            FindClose(h);
        }
    }

    void visitFilesFolders(const char* folder, const std::function<void(uint64_t, const char*, bool)>& user)
    {
        WIN32_FIND_DATAA findFile;
        Utf8             searchPath = folder;
        searchPath += "\\*";
        const HANDLE h = FindFirstFileA(searchPath, &findFile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (findFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (findFile.cFileName[0] == '.' && (!findFile.cFileName[1] || (findFile.cFileName[1] == '.' && !findFile.cFileName[2])))
                        continue;
                }

                uint64_t writeTime = static_cast<uint64_t>(findFile.ftLastWriteTime.dwHighDateTime) << 32;
                writeTime |= findFile.ftLastWriteTime.dwLowDateTime;

                const bool isFolder = findFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
                user(writeTime, findFile.cFileName, isFolder);
            } while (FindNextFileA(h, &findFile));

            FindClose(h);
        }
    }

    void visitFilesRec(const char* folder, const std::function<void(const char*)>& user)
    {
        WIN32_FIND_DATAA findFile;
        Path             searchPath = folder;
        searchPath += "\\*";

        Path         path = folder;
        const HANDLE h    = FindFirstFileA(searchPath, &findFile);
        if (h != INVALID_HANDLE_VALUE)
        {
            do
            {
                path = folder;
                path.append(findFile.cFileName);

                if (findFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (findFile.cFileName[0] == '.' && (!findFile.cFileName[1] || (findFile.cFileName[1] == '.' && !findFile.cFileName[2])))
                        continue;
                    visitFilesRec(path, user);
                }
                else
                {
                    user(path);
                }
            } while (FindNextFileA(h, &findFile));

            FindClose(h);
        }
    }

    void setThreadName(std::thread* thread, const char* threadName)
    {
        const auto  handle   = thread->native_handle();
        const DWORD threadId = GetThreadId(handle);
        setThreadNamePriv(threadId, threadName);
        // SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
    }

    uint64_t tlsAlloc()
    {
        return TlsAlloc();
    }

    void tlsSetValue(uint64_t id, void* value)
    {
        TlsSetValue(static_cast<uint32_t>(id), value);
    }

    void* tlsGetValue(uint64_t id)
    {
        return TlsGetValue(static_cast<uint32_t>(id));
    }

    void errorBox(const char* title, const char* expr)
    {
        MessageBoxA(nullptr, expr, title, MB_OK | MB_ICONERROR);
        DebugBreak();
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

        const auto ptrSymbol    = reinterpret_cast<SYMBOL_INFO*>(sym);
        ptrSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        ptrSymbol->MaxNameLen   = SYM_LEN_NAME;

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
                const auto hasSymbol = SymFromAddr(GetCurrentProcess(), reinterpret_cast<ULONG64>(where[i]), &displacement, ptrSymbol);
                displacement         = 0;
                const auto hasLine   = SymGetLineFromAddr64(GetCurrentProcess(), reinterpret_cast<ULONG64>(where[i]), reinterpret_cast<DWORD*>(&displacement), &line);

                if (hasSymbol)
                {
                    str += Log::colorToVTS(LogColor::Name);
                    str += ptrSymbol->Name;
                    if (hasLine)
                    {
                        str += Log::colorToVTS(LogColor::White);
                        str += " at ";
                        str += Log::colorToVTS(LogColor::Location);
                        str += form("%s:%d", line.FileName, line.LineNumber);
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
        g_ExceptionParams[1] = const_cast<char*>(msg);
        g_ExceptionParams[2] = reinterpret_cast<void*>(msg ? strlen(msg) : 0);
        g_ExceptionParams[3] = reinterpret_cast<void*>(SwagExceptionKind::Panic);
        RaiseException(code, 0, 4, reinterpret_cast<ULONG_PTR*>(&g_ExceptionParams[0]));
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
        return static_cast<double>(timer) / static_cast<double>(freq.QuadPart);
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
        return _InterlockedExchangeAdd8(reinterpret_cast<char*>(addr), value);
    }

    int16_t atomicAdd(int16_t* addr, int16_t value)
    {
        return _InterlockedExchangeAdd16(addr, value);
    }

    int32_t atomicAdd(int32_t* addr, int32_t value)
    {
        return _InterlockedExchangeAdd(reinterpret_cast<LONG*>(addr), value);
    }

    int64_t atomicAdd(int64_t* addr, int64_t value)
    {
        return _InterlockedExchangeAdd64(addr, value);
    }

    int8_t atomicAnd(int8_t* addr, int8_t value)
    {
        return _InterlockedAnd8(reinterpret_cast<char*>(addr), value);
    }

    int16_t atomicAnd(int16_t* addr, int16_t value)
    {
        return _InterlockedAnd16(addr, value);
    }

    int32_t atomicAnd(int32_t* addr, int32_t value)
    {
        return _InterlockedAnd(reinterpret_cast<LONG*>(addr), value);
    }

    int64_t atomicAnd(int64_t* addr, int64_t value)
    {
        return _InterlockedAnd64(addr, value);
    }

    int8_t atomicOr(int8_t* addr, int8_t value)
    {
        return _InterlockedOr8(reinterpret_cast<char*>(addr), value);
    }

    int16_t atomicOr(int16_t* addr, int16_t value)
    {
        return _InterlockedOr16(addr, value);
    }

    int32_t atomicOr(int32_t* addr, int32_t value)
    {
        return _InterlockedOr(reinterpret_cast<LONG*>(addr), value);
    }

    int64_t atomicOr(int64_t* addr, int64_t value)
    {
        return _InterlockedOr64(addr, value);
    }

    int8_t atomicXor(int8_t* addr, int8_t value)
    {
        return _InterlockedXor8(reinterpret_cast<char*>(addr), value);
    }

    int16_t atomicXor(int16_t* addr, int16_t value)
    {
        return _InterlockedXor16(addr, value);
    }

    int32_t atomicXor(int32_t* addr, int32_t value)
    {
        return _InterlockedXor(reinterpret_cast<LONG*>(addr), value);
    }

    int64_t atomicXor(int64_t* addr, int64_t value)
    {
        return _InterlockedXor64(addr, value);
    }

    int8_t atomicXchg(int8_t* addr, int8_t replaceWith)
    {
        return InterlockedExchange8(reinterpret_cast<char*>(addr), replaceWith);
    }

    int16_t atomicXchg(int16_t* addr, int16_t replaceWith)
    {
        return InterlockedExchange16(addr, replaceWith);
    }

    int32_t atomicXchg(int32_t* addr, int32_t replaceWith)
    {
        return InterlockedExchange(reinterpret_cast<LONG*>(addr), replaceWith);
    }

    int64_t atomicXchg(int64_t* addr, int64_t replaceWith)
    {
        return InterlockedExchange64(addr, replaceWith);
    }

    int8_t atomicCmpXchg(int8_t* addr, int8_t compareTo, int8_t replaceWith)
    {
        return _InterlockedCompareExchange8(reinterpret_cast<char*>(addr), replaceWith, compareTo);
    }

    int16_t atomicCmpXchg(int16_t* addr, int16_t compareTo, int16_t replaceWith)
    {
        return _InterlockedCompareExchange16(addr, replaceWith, compareTo);
    }

    int32_t atomicCmpXchg(int32_t* addr, int32_t compareTo, int32_t replaceWith)
    {
        return _InterlockedCompareExchange(reinterpret_cast<LONG*>(addr), replaceWith, compareTo);
    }

    int64_t atomicCmpXchg(int64_t* addr, int64_t compareTo, int64_t replaceWith)
    {
        return _InterlockedCompareExchange64(addr, replaceWith, compareTo);
    }

    uint8_t bitCountNz(uint8_t value)
    {
        return static_cast<uint8_t>(__popcnt16(value));
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
        return static_cast<uint8_t>(index);
    }

    uint16_t bitCountTz(uint16_t value)
    {
        unsigned long index;
        const auto    res = _BitScanForward(&index, value);
        if (!res)
            return 16;
        return static_cast<uint16_t>(index);
    }

    uint32_t bitCountTz(uint32_t value)
    {
        unsigned long index;
        const auto    res = _BitScanForward(&index, value);
        if (!res)
            return 32;
        return static_cast<uint32_t>(index);
    }

    uint64_t bitCountTz(uint64_t value)
    {
        unsigned long index;
        const auto    res = _BitScanForward64(&index, value);
        if (!res)
            return 64;
        return index;
    }

    uint8_t bitCountLz(uint8_t value)
    {
        unsigned long index;
        const auto    res = _BitScanReverse(&index, value);
        if (!res)
            return 8;
        return static_cast<uint8_t>(8 - index - 1);
    }

    uint16_t bitCountLz(uint16_t value)
    {
        unsigned long index;
        const auto    res = _BitScanReverse(&index, value);
        if (!res)
            return 16;
        return static_cast<uint8_t>(16 - index - 1);
    }

    uint32_t bitCountLz(uint32_t value)
    {
        unsigned long index;
        const auto    res = _BitScanReverse(&index, value);
        if (!res)
            return 32;
        return static_cast<uint8_t>(32 - index - 1);
    }

    uint64_t bitCountLz(uint64_t value)
    {
        unsigned long index;
        const auto    res = _BitScanReverse64(&index, value);
        if (!res)
            return 64;
        return static_cast<uint8_t>(64 - index - 1);
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

    namespace
    {
#ifdef _M_X64
        thread_local ScbeX64 g_GenFFI;
#else
        static_assert(false, "unsupported architecture");
#endif
    }

    void ffi(ByteCodeRunContext* context, void* foreignPtr, const TypeInfoFuncAttr* typeInfoFunc, const VectorNative<CpuPushParam>& pushCPUParams, void* retCopyAddr)
    {
        const auto returnType = TypeManager::concreteType(typeInfoFunc->returnType);

        g_GenFFI.cpu = &g_GenFFI;
        g_GenFFI.cc  = CallConv::get(CallConvKind::Swag);

        uint32_t stackSize = sizeof(void*);
        stackSize += pushCPUParams.size() * sizeof(void*);
        stackSize = std::max(stackSize, static_cast<uint32_t>(g_GenFFI.cc->paramsRegistersInteger.size() * sizeof(void*)));
        stackSize = Math::align(stackSize, g_GenFFI.cc->stackAlign);

        static constexpr int JIT_SIZE_BUFFER = 16 * 1024;
        uint64_t             startOffset     = 0;

        {
#ifdef SWAG_STATS
            Timer timer(&g_Stats.ffiGenTime);
#endif
            if (!g_GenFFI.concat.firstBucket)
            {
                // Generate a buffer big enough to store the call, and be aware that this could be recursive, that's
                // why the buffer is kind of big
                auto& concat = g_GenFFI.concat;
                concat.init(0);
                concat.firstBucket->capacity = JIT_SIZE_BUFFER + 128;
                concat.firstBucket->data     = static_cast<uint8_t*>(VirtualAlloc(nullptr, g_GenFFI.concat.firstBucket->capacity, MEM_COMMIT, PAGE_EXECUTE_READWRITE));
                concat.currentSP             = g_GenFFI.concat.firstBucket->data;

                VectorNative<CpuReg>   unwindRegs;
                VectorNative<uint32_t> unwindOffsetRegs;

                // Fake emit in order to compute the unwind infos
                g_GenFFI.emitPush(g_GenFFI.cc->ffiBaseRegister);
                unwindRegs.push_back(g_GenFFI.cc->ffiBaseRegister);
                unwindOffsetRegs.push_back(g_GenFFI.concat.totalCount());

                g_GenFFI.emitOpBinaryRegImm(CpuReg::Rsp, stackSize, CpuOp::SUB, OpBits::B64);
                const auto sizeProlog = g_GenFFI.concat.totalCount();

                // We need to generate unwind stuff to get a correct callstack, and in case the runtime raises an exception
                // with 'RaiseException' (panic, error, etc.)
                // The function information is always the same, that's why we generate only one table per SCBE_X64.
                VectorNative<uint16_t> unwind;
                ScbeCoff::computeUnwind(unwindRegs, unwindOffsetRegs, stackSize, sizeProlog, unwind);

                // Add function table
                auto* rtFunc              = new RUNTIME_FUNCTION(); // leak, but it's fine
                rtFunc->BeginAddress      = 0;
                rtFunc->EndAddress        = JIT_SIZE_BUFFER;
                rtFunc->UnwindInfoAddress = JIT_SIZE_BUFFER;
                uint32_t offset           = 0;
                concat.currentSP          = g_GenFFI.concat.firstBucket->data + JIT_SIZE_BUFFER;
                ScbeCoff::emitUnwind(g_GenFFI.concat, offset, sizeProlog, unwind);
                RtlAddFunctionTable(rtFunc, 1, reinterpret_cast<DWORD64>(g_GenFFI.concat.firstBucket->data));

                // Restore back the start of the buffer
                concat.currentSP = g_GenFFI.concat.firstBucket->data;
            }

            startOffset = g_GenFFI.concat.currentSP - g_GenFFI.concat.firstBucket->data;
            SWAG_ASSERT(startOffset < JIT_SIZE_BUFFER);

            g_GenFFI.emitPush(g_GenFFI.cc->ffiBaseRegister);
            g_GenFFI.emitOpBinaryRegImm(CpuReg::Rsp, stackSize, CpuOp::SUB, OpBits::B64);
            g_GenFFI.emitLoadRegImm(g_GenFFI.cc->ffiBaseRegister, reinterpret_cast<uint64_t>(context->sp), OpBits::B64);

            g_GenFFI.emitComputeCallParameters(typeInfoFunc, pushCPUParams, g_GenFFI.cc->ffiBaseRegister, 0, retCopyAddr);

            g_GenFFI.emitLoadRegImm(g_GenFFI.cc->computeRegI0, reinterpret_cast<uint64_t>(foreignPtr), OpBits::B64);
            g_GenFFI.emitCallReg(g_GenFFI.cc->computeRegI0, typeInfoFunc->getCallConv());

            if (!returnType->isVoid() && !retCopyAddr)
            {
                g_GenFFI.emitLoadRegImm(g_GenFFI.cc->ffiBaseRegister, reinterpret_cast<uint64_t>(context->registersRR), OpBits::B64);
                g_GenFFI.emitStoreCallResult(g_GenFFI.cc->ffiBaseRegister, 0, typeInfoFunc);
            }

            g_GenFFI.emitOpBinaryRegImm(CpuReg::Rsp, stackSize, CpuOp::ADD, OpBits::B64);
            g_GenFFI.emitPop(g_GenFFI.cc->ffiBaseRegister);
            g_GenFFI.emitRet();
        }

        // The real deal : make the call
        using FuncPtr  = void (*)();
        const auto ptr = reinterpret_cast<FuncPtr>(g_GenFFI.concat.firstBucket->data + startOffset);
        ptr();

        g_GenFFI.concat.currentSP = reinterpret_cast<uint8_t*>(ptr);
    }

    Utf8 getClipboardString()
    {
        if (!OpenClipboard(GetDesktopWindow()))
            return "";

        if (IsClipboardFormatAvailable(CF_TEXT))
        {
            const auto hGlob = GetClipboardData(CF_TEXT);
            if (hGlob)
            {
                const auto pz     = GlobalLock(hGlob);
                Utf8       result = static_cast<const char*>(pz);
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

        const auto hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        SetConsoleMode(hStdIn, ENABLE_WINDOW_INPUT);

        while (true)
        {
            if (dwReadIndex >= dwRead)
            {
                dwReadIndex = 0;
                if (!ReadConsoleInput(hStdIn, buffer, 1024, &dwRead))
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

                        c = static_cast<int>(static_cast<unsigned>(evt.Event.KeyEvent.uChar.AsciiChar));
                        if (c >= ' ' && c <= 127)
                            return Key::Ascii;

                        continue;
                }
            }
        }
    }

    void cls()
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        SMALL_RECT                 scrollRect;
        COORD                      scrollTarget;
        CHAR_INFO                  fill;

        // Get the number of character cells in the current buffer.
        if (!GetConsoleScreenBufferInfo(g_ConsoleHandle, &csbi))
            return;

        // Scroll the rectangle of the entire buffer.
        scrollRect.Left   = 0;
        scrollRect.Top    = 0;
        scrollRect.Right  = csbi.dwSize.X;
        scrollRect.Bottom = csbi.dwSize.Y;

        // Scroll it upwards off the top of the buffer with a magnitude of the entire height.
        scrollTarget.X = 0;
        scrollTarget.Y = static_cast<SHORT>(0 - csbi.dwSize.Y);

        // Fill with empty spaces with the buffer's default text attribute.
        fill.Char.UnicodeChar = TEXT(' ');
        fill.Attributes       = csbi.wAttributes;

        // Do the scroll
        ScrollConsoleScreenBuffer(g_ConsoleHandle, &scrollRect, nullptr, scrollTarget, &fill);

        // Move the cursor to the top left corner too.
        csbi.dwCursorPosition.X = 0;
        csbi.dwCursorPosition.Y = 0;

        SetConsoleCursorPosition(g_ConsoleHandle, csbi.dwCursorPosition);
    }
}

#endif
